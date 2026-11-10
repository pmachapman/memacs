/*	EPROC.C: Programs running live in a buffer

	A process started here (by me.process in Lua) runs with its input
	and output connected to pipes. Its output is added to a buffer at
	the "process mark" as it arrives, and text can be sent to its
	input. The Lua *shell* buffer is built on this.

	Output is only added while the editor is waiting for a command
	key (see proc_poll, called from the keyboard loop), so it never
	changes a buffer in the middle of a command.

	Only the Windows console version has this for now.
*/

/* windows.h must come before the editor's headers (as in xpconio.c) */
#ifdef	_WIN32
#undef	PASCAL
#undef	NEAR
#undef	HIBYTE
#include	<windows.h>
#include	<tlhelp32.h>
#endif
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"

#if	PROCESS

typedef struct EPROC {
	struct EPROC *next;
	int id;			/* number Lua knows it by		*/
	BUFFER *bp;		/* buffer its output goes to, or NULL	*/
	HANDLE hproc;		/* the process				*/
	DWORD pid;
	HANDLE hin;		/* we write its input here		*/
	HANDLE hout;		/* and read its output here		*/
	long mline;		/* process mark: line (from 1)		*/
	int mcol;		/*   and column (from 0)		*/
	int running;		/* still going?				*/
	int eof;		/* its output pipe is closed		*/
	DWORD code;		/* exit code, once it has stopped	*/
	int esc;		/* where we are in an escape sequence	*/
	EWINDOW win;		/* used while adding output		*/
} EPROC;

#define	ESC_NONE	0
#define	ESC_START	1	/* just had ESC				*/
#define	ESC_CSI		2	/* in ESC [ ...				*/
#define	ESC_OSC		3	/* in ESC ] ... (ends with BEL or ESC \) */
#define	ESC_OSCESC	4	/* ESC inside an OSC			*/

#define	MAXREAD		65536L	/* most bytes taken from one process per poll */

static EPROC *procs = NULL;	/* every process we know of */
static int lastid = 0;
static HANDLE job = NULL;	/* processes die when the editor does */

int proc_toplevel = FALSE;	/* TRUE while waiting for a command key */

static EPROC *byid(int id)
{
	EPROC *pp;

	for (pp = procs; pp; pp = pp->next)
		if (pp->id == id)
			return(pp);
	return(NULL);
}

static VOID closepipes(EPROC *pp)
{
	if (pp->hin) {
		CloseHandle(pp->hin);
		pp->hin = NULL;
	}
	if (pp->hout) {
		CloseHandle(pp->hout);
		pp->hout = NULL;
	}
}

static VOID unlink_proc(EPROC *pp)
{
	EPROC **ppp;

	for (ppp = &procs; *ppp; ppp = &(*ppp)->next)
		if (*ppp == pp) {
			*ppp = pp->next;
			break;
		}
	closepipes(pp);
	if (pp->hproc) {
		if (pp->running)
			TerminateProcess(pp->hproc, 1);
		CloseHandle(pp->hproc);
	}
	free(pp);
}

/* ------------------------------------------------------------------ */
/*	Adding output to the buffer					*/
/* ------------------------------------------------------------------ */

/* The line with the given number, or the header line if past the end */

static LINE *nthline(BUFFER *bp, long n)
{
	LINE *lp;

	lp = lforw(bp->b_linep);
	while (--n > 0 && lp != bp->b_linep)
		lp = lforw(lp);
	return(lp);
}

#define	MAXFOLLOW	32

/* Insert len bytes at the process mark of pp's buffer. The text has
   no carriage returns; a newline starts a new line. */

static VOID insertout(EPROC *pp, char *text, int len)
{
	BUFFER *bp;
	EWINDOW *oldwp, *wp;
	BUFFER *oldbp;
	EWINDOW sv;		/* stands in for the buffer's saved dot */
	EWINDOW *follow[MAXFOLLOW];
	int nfollow, svfollow, i, cmark;
	int nownd;
	int oldundo, oldview;
	LINE *mlp;
	SCREEN *scrp;

	bp = pp->bp;
	if (bp == NULL || len <= 0)
		return;

	/* find the mark, and keep it inside the buffer */
	mlp = nthline(bp, pp->mline);
	if (mlp == bp->b_linep) {
		pp->mline = getlinenum(bp, mlp);
		pp->mcol = 0;
	} else if (pp->mcol > lused(mlp))
		pp->mcol = lused(mlp);
	memset(&pp->win, 0, sizeof(EWINDOW));
	pp->win.w_bufp = bp;
	pp->win.w_dotp = pp->win.w_linep = mlp;
	pp->win.w_doto = pp->mcol;
	for (cmark = 0; cmark < NMARKS; cmark++) {
		pp->win.w_markp[cmark] = mlp;
		pp->win.w_marko[cmark] = pp->mcol;
	}

	/* windows whose cursor is at the mark move along with the output */
	nfollow = 0;
	for (scrp = first_screen; scrp; scrp = scrp->s_next_screen)
		for (wp = scrp->s_first_window; wp; wp = wp->w_wndp)
			if (wp->w_bufp == bp && wp->w_dotp == mlp &&
			    wp->w_doto == pp->mcol && nfollow < MAXFOLLOW)
				follow[nfollow++] = wp;

	/* a buffer not on screen keeps its cursor in the BUFFER; let a
	   stand-in window carry it while lines are changed */
	nownd = (bp->b_nwnd == 0);
	svfollow = FALSE;
	if (nownd) {
		memset(&sv, 0, sizeof(EWINDOW));
		sv.w_bufp = bp;
		sv.w_dotp = sv.w_linep = bp->b_dotp;
		sv.w_doto = bp->b_doto;
		for (cmark = 0; cmark < NMARKS; cmark++) {
			sv.w_markp[cmark] = bp->b_markp[cmark];
			sv.w_marko[cmark] = bp->b_marko[cmark];
		}
		svfollow = (bp->b_dotp == mlp && bp->b_doto == pp->mcol);
	}

	/* put our windows in the window list, so line changes fix them */
	scrp = first_screen;
	pp->win.w_wndp = scrp->s_first_window;
	scrp->s_first_window = &pp->win;
	if (nownd) {
		sv.w_wndp = scrp->s_first_window;
		scrp->s_first_window = &sv;
	}
	oldwp = curwp;
	oldbp = curbp;
	curwp = &pp->win;
	curbp = bp;
	oldundo = undoing;
	undoing = TRUE;		/* output is not something to undo */
	oldview = bp->b_mode & MDVIEW;
	bp->b_mode &= ~MDVIEW;

	for (i = 0; i < len; i++)
		if ((text[i] == '\n' ? lnewline() : linsert(1, text[i])) != TRUE)
			break;

	bp->b_mode |= oldview;
	undoing = oldundo;
	curwp = oldwp;
	curbp = oldbp;
	if (nownd)
		scrp->s_first_window = sv.w_wndp;
	scrp->s_first_window = pp->win.w_wndp;

	/* the mark is now after the output */
	pp->mline = getlinenum(bp, pp->win.w_dotp);
	pp->mcol = pp->win.w_doto;
	for (i = 0; i < nfollow; i++) {
		follow[i]->w_dotp = pp->win.w_dotp;
		follow[i]->w_doto = pp->win.w_doto;
		follow[i]->w_flag |= WFMOVE;
	}
	if (nownd) {
		bp->b_dotp = svfollow ? pp->win.w_dotp : sv.w_dotp;
		bp->b_doto = svfollow ? pp->win.w_doto : sv.w_doto;
		for (cmark = 0; cmark < NMARKS; cmark++) {
			bp->b_markp[cmark] = sv.w_markp[cmark];
			bp->b_marko[cmark] = sv.w_marko[cmark];
		}
	}

	/* a process buffer is not a file to be saved */
	bp->b_flag &= ~BFCHG;
	for (scrp = first_screen; scrp; scrp = scrp->s_next_screen)
		for (wp = scrp->s_first_window; wp; wp = wp->w_wndp)
			if (wp->w_bufp == bp)
				wp->w_flag |= WFMODE;
}

/* Add raw output: drop carriage returns, bells and terminal escape
   sequences, which a buffer can't show */

static VOID addout(EPROC *pp, char *buf, int len)
{
	char *out;
	int i, n;
	unsigned char c;

	out = buf;	/* filtered in place */
	n = 0;
	for (i = 0; i < len; i++) {
		c = (unsigned char)buf[i];
		switch (pp->esc) {
		case ESC_START:
			pp->esc = (c == '[') ? ESC_CSI :
				  (c == ']') ? ESC_OSC : ESC_NONE;
			continue;
		case ESC_CSI:
			if (c >= 0x40 && c <= 0x7e)
				pp->esc = ESC_NONE;
			continue;
		case ESC_OSC:
			if (c == 7)
				pp->esc = ESC_NONE;
			else if (c == 27)
				pp->esc = ESC_OSCESC;
			continue;
		case ESC_OSCESC:
			pp->esc = (c == '\\') ? ESC_NONE : ESC_OSC;
			continue;
		}
		if (c == 27) {
			pp->esc = ESC_START;
			continue;
		}
		if (c == '\r' || c == 7 || c == 0)
			continue;
		out[n++] = c;
	}
	insertout(pp, out, n);
}

/* Say that the process has finished */

static VOID addexit(EPROC *pp)
{
	char msg[80];
	LINE *lp;

	if (pp->bp == NULL)
		return;
	lp = nthline(pp->bp, pp->mline);
	sprintf(msg, "%s[Process finished, exit code %lu]",
		(lp != pp->bp->b_linep && pp->mcol > 0) ? "\n" : "",
		(unsigned long)pp->code);
	insertout(pp, msg, (int)strlen(msg));
}

/* ------------------------------------------------------------------ */
/*	The interface							*/
/* ------------------------------------------------------------------ */

/* Start a program. cmdline is a full Windows command line, dir the
   folder to run it in (or NULL), and bp the buffer for its output.
   The process mark starts at the end of the buffer. Returns its id,
   or 0 with a reason in errmsg. */

int PASCAL NEAR proc_start(cmdline, dir, bp, errmsg)

char *cmdline;
char *dir;
BUFFER *bp;
char *errmsg;	/* NSTRING bytes */

{
	SECURITY_ATTRIBUTES sa;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	HANDLE inr = NULL, inw = NULL, outr = NULL, outw = NULL;
	EPROC *pp;
	char *cl;
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jl;

	/* one process per buffer: forget a finished one */
	if ((pp = (EPROC *)proc_find(bp)) != NULL) {
		if (pp->running) {
			strcpy(errmsg, "a process is already running in that buffer");
			return(0);
		}
		unlink_proc(pp);
	}

	if (job == NULL) {
		job = CreateJobObjectA(NULL, NULL);
		if (job) {
			memset(&jl, 0, sizeof(jl));
			jl.BasicLimitInformation.LimitFlags =
				JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
			SetInformationJobObject(job,
				JobObjectExtendedLimitInformation, &jl, sizeof(jl));
		}
	}

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&inr, &inw, &sa, 0) ||
	    !CreatePipe(&outr, &outw, &sa, 0)) {
		strcpy(errmsg, "can't make pipes");
		goto fail;
	}
	/* our ends stay with us */
	SetHandleInformation(inw, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(outr, HANDLE_FLAG_INHERIT, 0);

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = inr;
	si.hStdOutput = outw;
	si.hStdError = outw;

	/* CreateProcess may change the command line */
	if ((cl = malloc(strlen(cmdline) + 1)) == NULL) {
		strcpy(errmsg, "out of memory");
		goto fail;
	}
	strcpy(cl, cmdline);
	/* CREATE_NO_WINDOW gives it a console of its own, out of sight,
	   so a program that writes to the console can't spoil our screen */
	if (!CreateProcessA(NULL, cl, NULL, NULL, TRUE,
			    CREATE_NO_WINDOW | CREATE_SUSPENDED, NULL,
			    (dir && *dir) ? dir : NULL, &si, &pi)) {
		free(cl);
		sprintf(errmsg, "can't start %.*s (error %lu)",
			NSTRING - 40, cmdline, (unsigned long)GetLastError());
		goto fail;
	}
	free(cl);
	if (job)
		AssignProcessToJobObject(job, pi.hProcess);
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(inr);
	CloseHandle(outw);

	if ((pp = (EPROC *)malloc(sizeof(EPROC))) == NULL) {
		TerminateProcess(pi.hProcess, 1);
		CloseHandle(pi.hProcess);
		CloseHandle(inw);
		CloseHandle(outr);
		strcpy(errmsg, "out of memory");
		return(0);
	}
	memset(pp, 0, sizeof(EPROC));
	pp->id = ++lastid;
	pp->bp = bp;
	pp->hproc = pi.hProcess;
	pp->pid = pi.dwProcessId;
	pp->hin = inw;
	pp->hout = outr;
	pp->running = TRUE;
	pp->mline = getlinenum(bp, bp->b_linep);
	pp->mcol = 0;
	pp->next = procs;
	procs = pp;
	return(pp->id);

fail:
	if (inr) CloseHandle(inr);
	if (inw) CloseHandle(inw);
	if (outr) CloseHandle(outr);
	if (outw) CloseHandle(outw);
	return(0);
}

/* The process in a buffer (as a pointer), or NULL */

char *PASCAL NEAR proc_find(bp)

BUFFER *bp;

{
	EPROC *pp;

	for (pp = procs; pp; pp = pp->next)
		if (pp->bp == bp)
			return((char *)pp);
	return(NULL);
}

/* The id of the process in a buffer, or 0 */

int PASCAL NEAR proc_ofbuf(bp)

BUFFER *bp;

{
	EPROC *pp = (EPROC *)proc_find(bp);

	return(pp ? pp->id : 0);
}

/* Send text to a process's input. Returns FALSE if it can't take it. */

int PASCAL NEAR proc_send(id, text, len)

int id;
char *text;
int len;

{
	EPROC *pp = byid(id);
	DWORD done;

	if (pp == NULL || pp->hin == NULL)
		return(FALSE);
	while (len > 0) {
		if (!WriteFile(pp->hin, text, (DWORD)len, &done, NULL))
			return(FALSE);
		text += done;
		len -= (int)done;
	}
	return(TRUE);
}

/* Close a process's input, as if ^Z were typed at a console */

int PASCAL NEAR proc_eof(id)

int id;

{
	EPROC *pp = byid(id);

	if (pp == NULL || pp->hin == NULL)
		return(FALSE);
	CloseHandle(pp->hin);
	pp->hin = NULL;
	return(TRUE);
}

/* Stop whatever the process is running, but not the process itself:
   for a shell, this ends the current command. Returns the number of
   programs stopped. */

int PASCAL NEAR proc_interrupt(id)

int id;

{
	EPROC *pp = byid(id);
	HANDLE snap, h;
	PROCESSENTRY32 pe;
	DWORD kill[256];	/* processes to stop */
	int nkill, done, i, again;

	if (pp == NULL || !pp->running)
		return(0);
	snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE)
		return(0);

	/* collect everything descended from the process, except the
	   console host that keeps its hidden console going */
	nkill = 0;
	do {
		again = FALSE;
		pe.dwSize = sizeof(pe);
		if (!Process32First(snap, &pe))
			break;
		do {
			int parentok = (pe.th32ParentProcessID == pp->pid);

			for (i = 0; !parentok && i < nkill; i++)
				if (pe.th32ParentProcessID == kill[i])
					parentok = TRUE;
			if (!parentok || pe.th32ProcessID == pp->pid)
				continue;
			if (_stricmp(pe.szExeFile, "conhost.exe") == 0)
				continue;
			for (i = 0; i < nkill; i++)
				if (kill[i] == pe.th32ProcessID)
					break;
			if (i == nkill && nkill < 256) {
				kill[nkill++] = pe.th32ProcessID;
				again = TRUE;
			}
		} while (Process32Next(snap, &pe));
	} while (again);
	CloseHandle(snap);

	done = 0;
	for (i = 0; i < nkill; i++) {
		h = OpenProcess(PROCESS_TERMINATE, FALSE, kill[i]);
		if (h) {
			if (TerminateProcess(h, 1))
				done++;
			CloseHandle(h);
		}
	}
	return(done);
}

/* End a process (and anything it started) */

int PASCAL NEAR proc_kill(id)

int id;

{
	EPROC *pp = byid(id);

	if (pp == NULL || !pp->running)
		return(FALSE);
	proc_interrupt(id);
	TerminateProcess(pp->hproc, 1);
	return(TRUE);
}

/* Fill in a process's state: running, exit code, process mark.
   Returns FALSE if there is no such process. */

int PASCAL NEAR proc_info(id, running, code, mline, mcol, bpp)

int id;
int *running;
long *code;
long *mline;
int *mcol;
BUFFER **bpp;

{
	EPROC *pp = byid(id);

	if (pp == NULL)
		return(FALSE);
	*running = pp->running;
	*code = pp->running ? -1 : (long)pp->code;
	*mline = pp->mline;
	*mcol = pp->mcol;
	*bpp = pp->bp;
	return(TRUE);
}

/* Move a process mark (line from 1, column from 0) */

int PASCAL NEAR proc_setmark(id, line, col)

int id;
long line;
int col;

{
	EPROC *pp = byid(id);

	if (pp == NULL)
		return(FALSE);
	pp->mline = line < 1 ? 1 : line;
	pp->mcol = col < 0 ? 0 : col;
	return(TRUE);
}

/* A buffer is being deleted: end its process */

VOID PASCAL NEAR proc_bufgone(bp)

BUFFER *bp;

{
	EPROC *pp;

	while ((pp = (EPROC *)proc_find(bp)) != NULL) {
		if (pp->running)
			proc_kill(pp->id);
		unlink_proc(pp);
	}
}

/* Read up to most bytes of waiting output into the buffer.
   Returns TRUE if there was any. */

static int drain(EPROC *pp, long most)
{
	char buf[4096];
	DWORD avail, got;
	long total = 0;

	while (pp->hout && total < most) {
		if (!PeekNamedPipe(pp->hout, NULL, 0, NULL, &avail, NULL)) {
			/* every writer has gone */
			CloseHandle(pp->hout);
			pp->hout = NULL;
			break;
		}
		if (avail == 0)
			break;
		if (avail > sizeof(buf))
			avail = sizeof(buf);
		if (!ReadFile(pp->hout, buf, avail, &got, NULL) || got == 0)
			break;
		addout(pp, buf, (int)got);
		total += got;
	}
	return(total > 0);
}

/* Take in any output that has arrived, and notice processes that
   have finished. Called while waiting for a key; does nothing unless
   the editor is waiting for a command. Returns TRUE if the screen
   needs updating. */

int PASCAL NEAR proc_poll()

{
	EPROC *pp, *next;
	int changed;

	if (!proc_toplevel || procs == NULL)
		return(FALSE);
	changed = FALSE;
	for (pp = procs; pp; pp = next) {
		next = pp->next;

		/* output */
		if (drain(pp, MAXREAD))
			changed = TRUE;

		/* has it finished? */
		if (pp->running &&
		    WaitForSingleObject(pp->hproc, 0) == WAIT_OBJECT_0) {
			GetExitCodeProcess(pp->hproc, &pp->code);
			pp->running = FALSE;
			drain(pp, 0x7fffffffL);	/* whatever it wrote last */
			closepipes(pp);
			addexit(pp);
			changed = TRUE;
			/* the script may delete buffers, and so processes:
			   look at the others next time */
			proc_toplevel = FALSE;	/* no polling inside it */
			elua_procexit(pp->id, (long)pp->code);
			proc_toplevel = TRUE;
			break;
		}
	}
	return(changed);
}

/* Wait up to ms milliseconds for output, or for a process to finish
   (for scripts). Returns TRUE if anything arrived. */

int PASCAL NEAR proc_wait(ms)

long ms;

{
	DWORD start = GetTickCount();
	int old = proc_toplevel;
	int got;

	do {
		proc_toplevel = TRUE;
		got = proc_poll();
		proc_toplevel = old;
		if (got)
			return(TRUE);
		Sleep(10);
	} while ((long)(GetTickCount() - start) < ms);
	return(FALSE);
}

#else
noeproc()	/* to keep some compilers happy */
{
}
#endif

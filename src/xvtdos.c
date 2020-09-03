/*	XVTDOS.C:	Operating specific I/O and Spawning functions
			under the XVT portability system
			for MicroEMACS 4.00
			(C)Copyright 1995 by Daniel M. Lawrence
*/

#include        <stdio.h>
#include	"estruct.h"
#include	"eproto.h"

#ifdef	XVT
#include        "edef.h"
#include	"elang.h"

/* The Mouse driver only works with typeahead defined */
#if	MOUSE
#undef	TYPEAH
#define	TYPEAH	1
#endif

#if  TURBO
#include <conio.h>
#include <dir.h>
#include <dos.h>
#include <bios.h>

struct ffblk fileblock;	/* structure for directory searches */
#endif
#if	MSC | ZTC
#include <dos.h>

struct find_t fileblock;	/* structure for directory searches */
#endif

#if     LATTICE | MSC | TURBO | IC | MWC | ZTC
union REGS rg;		/* cpu register for use of DOS calls */
struct SREGS segreg;	/* cpu segment registers	     */
int nxtchar = -1;	/* character held from type ahead    */
#endif

#if	MSC | TURBO
#include	<process.h>
#endif

#if	IC
#include	<time.h>
#endif

/*	Some global variable	*/
#define INBUFSIZ	40
static int mexist;	/* is the mouse driver installed? */
static int nbuttons;	/* number of buttons on the mouse */
static int oldbut;	/* Previous state of mouse buttons */
static int oldcol;	/* previous x position of mouse */
static int oldrow;	/* previous y position of mouse */

int execprog(char *cmd);

/*	input buffers and pointers	*/

#define	IBUFSIZE	64	/* this must be a power of 2 */

unsigned char in_buf[IBUFSIZE];	/* input character buffer */
int in_next = 0;		/* pos to retrieve next input character */
int in_last = 0;		/* pos to place most recent input character */

in_init()	/* initialize the input buffer */

{
	in_next = in_last = 0;
}

in_check()	/* is the input buffer non-empty? */

{
	if (in_next == in_last)
		return(FALSE);
	else
		return(TRUE);
}

in_put(event)

int event;	/* event to enter into the input buffer */

{
	in_buf[in_last++] = event;
	in_last &= (IBUFSIZE - 1);
}

int in_get()	/* get an event from the input buffer */

{
	register int event;	/* event to return */

	event = in_buf[in_next++];
	in_next &= (IBUFSIZE - 1);
	return(event);
}

/*
 * This function is called once to set up the terminal device streams.
 */

int ttopen()

{

	/* on all screens we are not sure of the initial position
	   of the cursor					*/
	ttrow = 999;
	ttcol = 999;
	strcpy(os, "XVT");

	mexist = TRUE;
	nbuttons = 3;

	/* initialize our character input queue */
	in_init();

}

/*
 * This function gets called just before we go back home to the command
 * interpreter. On VMS it puts the terminal back in a reasonable state.
 * Another no-operation on CPM.
 */
int ttclose()
{
	/* nothing here! */
}

/*
 * Flush terminal buffer. Does real work where the terminal output is buffered
 * up. A no-operation on systems where byte at a time terminal I/O is done.
 */
int ttflush()
{
}

/* typahead:	Check to see if any characters are already in the
		keyboard buffer
*/

int typahead()

{
	return(in_check());
}

/*
 * Create a subjob with a copy of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as garbage so that you do a full
 * repaint. Bound to "^X C".
 */

int spawncli(f, n)

int f, n;

{
	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

        movecursor(term.t_nrow, 0);             /* Seek to last line.   */
        TTflush();
	TTkclose();
	shellprog("");
#if	WINDOW_TEXT
	refresh_screen(first_screen);
#endif
	TTkopen();
        sgarbf = TRUE;
        return(TRUE);
}

/*
 * Run a one-liner in a subjob. When the command returns, wait for a single
 * character to be typed, then mark the screen as garbage so a full repaint is
 * done. Bound to "C-X !".
 */
int spawn(f, n)

int f, n;

{
        register int s;
        char line[NLINE];

	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

        if ((s=mlreply("!", line, NLINE)) != TRUE)
                return(s);
	movecursor(term.t_nrow - 1, 0);
	TTkclose();
        shellprog(line);
	TTkopen();

	/* if we are interactive, pause here */
	if (clexec == FALSE) {
#if	XVT == 0
		printf(TEXT227);
/*			"\n--- Press any key to Continue ---" */
#endif
		tgetc();
        }
#if	WINDOW_TEXT
	refresh_screen(first_screen);
#endif
        sgarbf = TRUE;
        return (TRUE);
}

/*
 * Run an external program with arguments. When it returns, wait for a single
 * character to be typed, then mark the screen as garbage so a full repaint is
 * done. Bound to "C-X $".
 */

int execprg(f, n)

{
        register int s;
        char line[NLINE];

	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

        if ((s=mlreply("$", line, NLINE)) != TRUE)
                return(s);
	movecursor(term.t_nrow - 1, 0);
	TTkclose();
        execprog(line);
#if	WINDOW_TEXT
	refresh_screen(first_screen);
#endif
	TTkopen();
	/* if we are interactive, pause here */
	if (clexec == FALSE) {
	        mlputs(TEXT6);
/*                     "\r\n\n[End]" */
        	tgetc();
        }
        sgarbf = TRUE;
        return (TRUE);
}

/*
 * Pipe a one line command into a window
 * Bound to ^X @
 */
int pipecmd(f, n)

int f, n;

{
	register EWINDOW *wp;	/* pointer to new window */
	register BUFFER *bp;	/* pointer to buffer to zot */
	register char *tmp;	/* ptr to TMP DOS environment variable */
	FILE *fp;
        char line[NLINE];	/* command line send to shell */
	static char bname[] = "command";
	static char filnam[NSTRING] = "command";

	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

	if ((tmp = getenv("TMP")) == NULL)
		filnam[0] = 0;
	else {
		strcpy(filnam, tmp);
		if (filnam[strlen(filnam) - 1] != '\\')
			strcat(filnam, "\\");
        }
	strcat(filnam,"command");

	/* get the command to pipe in */
        if (mlreply("@", line, NLINE) != TRUE)
                return(FALSE);

	/* get rid of the command output buffer if it exists */
        if ((bp=bfind(bname, FALSE, 0)) != FALSE) {
		/* try to make sure we are off screen */
		wp = wheadp;
		while (wp != NULL) {
			if (wp->w_bufp == bp) {
				onlywind(FALSE, 1);
				break;
			}
			wp = wp->w_wndp;
		}
		/* get rid of the existing command buffer */
		if (zotbuf(bp) != TRUE)
			return(FALSE);
	}

	/* redirect the command output to the output file */
	strcat(line, " >>");
	strcat(line, filnam);
	movecursor(term.t_nrow - 1, 0);

	/* execute the command */
	TTkclose();
        shellprog(line);
#if	WINDOW_TEXT
	refresh_screen(first_screen);
#endif
	TTkopen();
        sgarbf = TRUE;

        /* did the output file get generated? */
	if ((fp = fopen(filnam, "r")) == NULL)
		return(FALSE);
	fclose(fp);

	/* split the current window to make room for the command output */
	if (splitwind(FALSE, 1) == FALSE)
			return(FALSE);

	/* and read the stuff in */
	if (getfile(filnam, FALSE) == FALSE)
		return(FALSE);

	/* make this window in VIEW mode, update all mode lines */
	curwp->w_bufp->b_mode |= MDVIEW;
	wp = wheadp;
	while (wp != NULL) {
		wp->w_flag |= WFMODE;
		wp = wp->w_wndp;
	}

	/* and get rid of the temporary file */
	unlink(filnam);
	return(TRUE);
}

/*
 * filter a buffer through an external DOS program
 * Bound to ^X #
 */
int uefilter(f, n)

int f, n;

{
        register int    s;	/* return status from CLI */
	register BUFFER *bp;	/* pointer to buffer to zot */
        char line[NLINE];	/* command line send to shell */
	char tmpnam[NFILEN];	/* place to store real file name */
	static char bname1[] = "fltinp";

	static char filnam1[] = "fltinp";
	static char filnam2[] = "fltout";

	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

	if (curbp->b_mode&MDVIEW)	/* don't allow this command if	*/
		return(rdonly());	/* we are in read only mode	*/

	/* get the filter name and its args */
        if ((s=mlreply("#", line, NLINE)) != TRUE)
                return(s);

	/* setup the proper file names */
	bp = curbp;
	strcpy(tmpnam, bp->b_fname);	/* save the original name */
	strcpy(bp->b_fname, bname1);	/* set it to our new one */

	/* write it out, checking for errors */
	if (writeout(filnam1, "w") != TRUE) {
		mlwrite(TEXT2);
/*                      "[Cannot write filter file]" */
		strcpy(bp->b_fname, tmpnam);
		return(FALSE);
	}

	strcat(line," <fltinp >fltout");
	movecursor(term.t_nrow - 1, 0);
	TTkclose();
        shellprog(line);
#if	WINDOW_TEXT
	refresh_screen(first_screen);
#endif
	TTkopen();
        sgarbf = TRUE;
	s = TRUE;

	/* on failure, escape gracefully */
	if (s != TRUE || (readin(filnam2,FALSE) == FALSE)) {
		mlwrite(TEXT3);
/*                      "[Execution failed]" */
		strcpy(bp->b_fname, tmpnam);
		unlink(filnam1);
		unlink(filnam2);
		return(s);
	}

	/* reset file name */
	strcpy(bp->b_fname, tmpnam);	/* restore name */
	bp->b_flag |= BFCHG;		/* flag it as changed */

	/* and get rid of the temporary file */
	unlink(filnam1);
	unlink(filnam2);
	return(TRUE);
}

#if	LATTICE
extern int _oserr;
#endif

#if	MWC
extern int errno;
#endif

#if	MSC
extern int _doserrno;
#endif

/*	SHELLPROG: Execute a command in a subshell		*/

int shellprog(cmd)

char *cmd;	/*  Incoming command line to execute  */

{
	char *shell;		/* Name of system command processor */
	char swchar;		/* switch character to use */
	union REGS regs;	/* parameters for dos call */
	char comline[NSTRING];	/* constructed command line */

	/*  detect current switch character and set us up to use it */
	regs.h.ah = 0x37;	/*  get setting data  */
	regs.h.al = 0x00;	/*  get switch character  */
	intdos(&regs, &regs);
	swchar = (char)regs.h.dl;

	/*  get name of system shell  */
	if ((shell = getenv("COMSPEC")) == NULL) {
		return(FALSE);		/*  No shell located  */
	}

	/* trim leading whitespace off the command */
	while (*cmd == ' ' || *cmd == '\t')	/*  find out if null command */
		cmd++;

	/**  If the command line is not empty, bring up the shell  **/
	/**  and execute the command.  Otherwise, bring up the     **/
	/**  shell in interactive mode.   **/

	if (*cmd) {
		strcpy(comline, shell);
		strcat(comline, " ");
		comline[strlen(comline) + 1] = 0;
		comline[strlen(comline)] = swchar;
		strcat(comline, "c ");
		strcat(comline, cmd);
		return(execprog(comline));
	} else
		return(execprog(shell));
}

/*	EXECPROG:	A function to execute a named program
			with arguments
*/

#if	LATTICE | MWC
#define	CFLAG	1
#endif

int execprog(cmd)

char *cmd;	/*  Incoming command line to execute  */

{
	char *sp;		/* temporary string pointer */
	int rv;			/* numeric return value from subprocess */
	char f1[38];		/* FCB1 area (not initialized */
	char f2[38];		/* FCB2 area (not initialized */
	char prog[NSTRING];	/* program filespec */
	char tail[NSTRING];	/* command tail with length byte */
	union REGS regs;	/* parameters for dos call  */
#if	MWC == 0
	struct SREGS segreg;	/* segment registers for dis call */
#endif
	struct Pblock {		/* EXEC parameter block */
		short envptr;	/* 2 byte pointer to environment string */
		char *cline;	/* 4 byte pointer to command line */
		char *fcb1;	/* 4 byte pointer to FCB at PSP+5Ch */
		char *fcb2;	/* 4 byte pointer to FCB at PSP+6Ch */
	} pblock;

	/* parse the command name from the command line */
	sp = prog;
	while (*cmd && (*cmd != ' ') && (*cmd != '\t'))
		*sp++ = *cmd++;
	*sp = 0;

	/* and parse out the command tail */
	while (*cmd && ((*cmd == ' ') || (*cmd == '\t')))
		++cmd;
	*tail = (char)(strlen(cmd)); /* record the byte length */
	strcpy(&tail[1], cmd);
	strcat(&tail[1], "\r");

	/* look up the program on the path trying various extensions */
	if ((sp = flook(prog, TRUE)) == NULL)
		if ((sp = flook(strcat(prog, ".exe"), TRUE)) == NULL) {
			strcpy(&prog[strlen(prog)-4], ".com");
			if ((sp = flook(prog, TRUE)) == NULL)
				return(FALSE);
		}
	strcpy(prog, sp);

#if	MWC == 0
	/* get a pointer to this PSPs environment segment number */
#if	IC
	_segread(&segreg);
#else /* IC */
	segread(&segreg);
#endif /* IC */
#endif /* MWC == 0 */

	/* set up the EXEC parameter block */
	pblock.envptr = 0;	/* make the child inherit the parents env */
	pblock.fcb1 = f1;		/* point to a blank FCB */
	pblock.fcb2 = f2;		/* point to a blank FCB */
        pblock.cline = tail;		/* parameter line pointer */

	/* and make the call */
	regs.h.ah = 0x4b;	/* EXEC Load or Execute a Program */
	regs.h.al = 0x00;	/* load end execute function subcode */
#if	MWC
	regs.x.ds = ((unsigned long)(prog) >> 16);	/* program name ptr */
	regs.x.dx = (unsigned int)(prog);
	regs.x.es = regs.x.ds;
	/*regs.x.es = ((unsigned long)(&pblock) >> 16);	* set up param block ptr */
	regs.x.bx = (unsigned int)(&pblock);
#endif
#if	LATTICE | MSC | TURBO | IC | ZTC
	segreg.ds = ((unsigned long)(prog) >> 16);	/* program name ptr */
	regs.x.dx = (unsigned int)(prog);
	segreg.es = ((unsigned long)(&pblock) >> 16);	/* set up param block ptr */
	regs.x.bx = (unsigned int)(&pblock);
#endif
#if	LATTICE
	if ((intdosx(&regs, &regs, &segreg) & CFLAG) == 0) {
		regs.h.ah = 0x4d;	/* get child process return code */
		intdos(&regs, &regs);	/* go do it */
		rv = regs.x.ax;		/* save child's return code */
	} else
		rv = -_oserr;		/* failed child call */
#endif
#if	MWC
	intcall(&regs, &regs, DOSINT);
	if ((regs.x.flags & CFLAG) == 0) {
		regs.h.ah = 0x4d;	/* get child process return code */
		intcall(&regs, &regs, DOSINT);	/* go do it */
		rv = regs.x.ax;		/* save child's return code */
	} else
		rv = -errno;		/* failed child call */
#endif
#if	TURBO | IC | MSC | ZTC
	intdosx(&regs, &regs, &segreg);
	if (regs.x.cflag == 0) {
		regs.h.ah = 0x4d;	/* get child process return code */
		intdos(&regs, &regs);	/* go do it */
		rv = regs.x.ax;		/* save child's return code */
	} else
#if	IC
		rv = -1;
#else /* IC */
		rv = -_doserrno;	/* failed child call */
#endif /* IC */
#endif
	strcpy(rval, int_asc(rv));
	return((rval < 0) ? FALSE : TRUE);
}

/* return a system dependent string with the current time */

char *timeset()

{
#if	MWC | TURBO | IC | MSC | ZTC
	register char *sp;	/* temp string pointer */
	char buf[16];		/* time data buffer */
	extern char *ctime();

#if	IC
	time((time_t *)buf);
	sp = ctime((time_t *)buf);
#else
	time(buf);
	sp = ctime(buf);
#endif
	sp[strlen(sp)-1] = 0;
	return(sp);
#else
	return(errorm);
#endif
}

#if	TURBO
/*	FILE Directory routines		*/

char path[NFILEN];	/* path of file to find */
char rbuf[NFILEN];	/* return file buffer */

/*	do a wild card directory search (for file name completion) */

char *getffile(fspec)

char *fspec;	/* pattern to match */

{
	register int index;		/* index into various strings */
	register int point;		/* index into other strings */
	register int extflag;		/* does the file have an extension? */
	char fname[NFILEN];		/* file/path for DOS call */

	/* first parse the file path off the file spec */
	strcpy(path, fspec);
	index = strlen(path) - 1;
	while (index >= 0 && (path[index] != '/' &&
				path[index] != '\\' && path[index] != ':'))
		--index;
	path[index+1] = 0;

	/* check for an extension */
	point = strlen(fspec) - 1;
	extflag = FALSE;
	while (point > index) {
		if (fspec[point] == '.') {
			extflag = TRUE;
			break;
		}
		point--;
	}

	/* construct the composite wild card spec */
	strcpy(fname, path);
	strcat(fname, &fspec[index+1]);
	strcat(fname, "*");
	if (extflag == FALSE)
		strcat(fname, ".*");

	/* and call for the first file */
	if (findfirst(fname, &fileblock, FA_DIREC) == -1)
		return(NULL);

	/* return the first file name! */
	strcpy(rbuf, path);
	strcat(rbuf, fileblock.ff_name);
	mklower(rbuf);
	if (fileblock.ff_attrib == 16)
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}

char *getnfile()

{
	register int index;		/* index into various strings */
	register int point;		/* index into other strings */
	register int extflag;		/* does the file have an extension? */
	char fname[NFILEN];		/* file/path for DOS call */

	/* and call for the first file */
	if (findnext(&fileblock) == -1)
		return(NULL);

	/* return the first file name! */
	strcpy(rbuf, path);
	strcat(rbuf, fileblock.ff_name);
	mklower(rbuf);
	if (fileblock.ff_attrib == 16)
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}
#else
#if	MSC | ZTC
/*	FILE Directory routines		*/

char path[NFILEN];	/* path of file to find */
char rbuf[NFILEN];	/* return file buffer */

/*	do a wild card directory search (for file name completion) */

char *getffile(fspec)

char *fspec;	/* pattern to match */

{
	register int index;		/* index into various strings */
	register int point;		/* index into other strings */
	register int extflag;		/* does the file have an extension? */
	char fname[NFILEN];		/* file/path for DOS call */

	/* first parse the file path off the file spec */
	strcpy(path, fspec);
	index = strlen(path) - 1;
	while (index >= 0 && (path[index] != '/' &&
				path[index] != '\\' && path[index] != ':'))
		--index;
	path[index+1] = 0;

	/* check for an extension */
	point = strlen(fspec) - 1;
	extflag = FALSE;
	while (point > index) {
		if (fspec[point] == '.') {
			extflag = TRUE;
			break;
		}
		point--;
	}

	/* construct the composite wild card spec */
	strcpy(fname, path);
	strcat(fname, &fspec[index+1]);
	strcat(fname, "*");
	if (extflag == FALSE)
		strcat(fname, ".*");

	/* and call for the first file */
	if (_dos_findfirst(fname, _A_NORMAL|_A_SUBDIR, &fileblock) != 0)
		return(NULL);

	/* return the first file name! */
	strcpy(rbuf, path);
	strcat(rbuf, fileblock.name);
	mklower(rbuf);
	if (fileblock.attrib == 16)
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}

char *getnfile()

{
	register int index;		/* index into various strings */
	register int point;		/* index into other strings */
	register int extflag;		/* does the file have an extension? */
	char fname[NFILEN];		/* file/path for DOS call */

	/* and call for the first file */
	if (_dos_findnext(&fileblock) != 0)
		return(NULL);

	/* return the first file name! */
	strcpy(rbuf, path);
	strcat(rbuf, fileblock.name);
	mklower(rbuf);
	if (fileblock.attrib == 16)
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}
#else
char *getffile(fspec)

char *fspec;	/* file to match */

{
	return(NULL);
}

char *getnfile()

{
	return(NULL);
}
#endif
#endif
#endif

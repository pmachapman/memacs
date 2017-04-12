/***
	POSIX driver/Microemacs 3.11a,
	Copyright 1989/1991 D. Lawrence, C. Smith
	Copyright 1992 H. Becker

 ***/

/**
	1. FIONREAD is not defined in POSIX. TYPEAH should be 0.

**/

/** Include files **/
#include <stdio.h>			/* Standard I/O definitions	*/
#include "estruct.h"			/* Emacs definitions		*/

/** Do nothing routine **/
int scnothing()
{
	return(0);
}

/** Include files **/
#include "eproto.h"			/* Function definitions		*/
#include "edef.h"			/* Global variable definitions	*/
#include "elang.h"			/* Language definitions		*/

/** Kill predefined **/
#undef CTRL				/* Problems with CTRL		*/

/** Overall include files **/
#include <sys/types.h>			/* System type definitions	*/
#include <sys/stat.h>			/* File status definitions	*/
#include <fcntl.h>			/* I/O control definitions	*/

/** Additional include files **/
#include <signal.h>			/* Signal definitions		*/
#include <termios.h>			/* Terminal I/O definitions	*/
#include <unistd.h>			/* unix i/o definitions		*/

/** Completion include files **/
/** Directory accessing: Try and figure this out... if you can! **/
#include <dirent.h>			/* Directory entry definitions	*/
#define DIRENTRY	dirent

/** Restore predefined definitions **/
#undef CTRL				/* Restore CTRL			*/
#define CTRL 0x0100

/** Parameters **/
#define NKEYENT		300		/* Number of keymap entries	*/
#define NINCHAR		64		/* Input buffer size		*/
#define NOUTCHAR	256		/* Output buffer size		*/
#define MARGIN		8		/* Margin size			*/
#define SCRSIZ		64		/* Scroll for margin		*/
#define NPAUSE		10		/* # times thru update to pause */
#define	MAXVTIME	1		/* x/10s max time for i/o delay	*/

/** CONSTANTS **/
#define TIMEOUT		255		/* No character available	*/

/** Type definitions **/
struct keyent {				/* Key mapping entry		*/
	struct keyent * samlvl;		/* Character on same level	*/
	struct keyent * nxtlvl;		/* Character on next level	*/
	unsigned char ch;		/* Character			*/
	int code;			/* Resulting keycode		*/
};

char *reset = (char*) NULL;		/* reset string kjc */

/** Local variables **/
static struct termios curterm;		/* Current modes		*/
static struct termios oldterm;		/* Original modes		*/
static int inbuf[NINCHAR];		/* Input buffer			*/
static int * inbufh =			/* Head of input buffer		*/
	inbuf;
static int * inbuft =			/* Tail of input buffer		*/
	inbuf;
static unsigned char keyseq[256];	/* Prefix escape sequence table	*/
static struct keyent keymap[NKEYENT];	/* Key map			*/
static struct keyent * nxtkey =		/* Next free key entry		*/
	keymap;
static DIR *dirptr = NULL;		/* Current directory stream	*/
static char path[NFILEN];		/* Path of file to find		*/
static char rbuf[NFILEN];		/* Return file buffer		*/
static char *nameptr;			/* Ptr past end of path in rbuf	*/

#if ANSI
#define	ttbeep	ansibeep
#define	ttmove	ansimove
#else /* ANSI */
#if TERMCAP
#define	ttbeep	tcapbeep
#define	ttmove	tcapmove
#endif

/** Terminal definition block **/
/*
int scclose(), ttgetc(), ttputc(), ttflush();
int ttmove(), sceeol(), sceeop(), ttbeep(), screv();
*/
int ttbeep(), ttmove();

#if COLOR
int scfcol(), scbcol();
#endif /* COLOR */

/** Open terminal device **/
int ttopen()
{
/* look for LINES, COLUMNS in env, use it */

	int	row, col;
	char	*cp;

	if (NULL!=(cp=getenv("LINES"))) {
		sscanf (cp, "%d", &row);
		if (row) {
			term.t_mrow= row-1;
			term.t_nrow= row-1;
		}
	}
	if (NULL!=(cp=getenv("COLUMNS"))) {
		sscanf (cp, "%d", &col);
		if (col) {
			term.t_mcol= col;
			term.t_ncol= col;
		}
	}

	/* Get modes */
	if (tcgetattr(STDIN_FILENO, &oldterm)) {
		perror("Cannot tcgetattr");
		return(-1);
	}

	/* Save to original mode variable */
	curterm = oldterm;

	/* Set new modes */
	curterm.c_iflag &= ~(INLCR|ICRNL|IGNCR);
#if	XONXOFF
	curterm.c_iflag |= IXON | IXOFF;
#endif
	curterm.c_lflag &= ~(ICANON|ISIG|ECHO);
	curterm.c_cc[VMIN] = 1;
	curterm.c_cc[VTIME] = 0;

	/* Set tty mode */
	if (tcsetattr(STDIN_FILENO, TCSANOW, &curterm)) {
		perror("Cannot tcsetattr");
		return(-1);
	}

	/* Success */
	return(0);
}

/** Close terminal device **/
int ttclose()
{
	/* Restore original terminal modes */
	if (reset != (char*)NULL)
		write(STDOUT_FILENO, reset, strlen(reset));

	/* Set tty mode */
	if (tcsetattr(STDIN_FILENO, TCSANOW, &oldterm))
		return(-1);

	/* Success */
	return(0);
}

/** Flush output buffer to display **/
int ttflush()
{
/*
	tcflush(STDOUT_FILENO, TCOFLUSH);
*/
	fflush (stdout);

	return(0);
}

/** Put character onto display **/
int ttputc(ch)
char ch;				/* Character to display		*/
{
/*
        return(write(STDOUT_FILENO, &ch, 1) != 1);
*/
	fputc (ch, stdout);
}

/** Add character sequence to keycode entry **/
void addkey(seq, fn)
unsigned char * seq;			/* Character sequence		*/
int fn;					/* Resulting keycode		*/
{
	int first;
	struct keyent * cur, * nxtcur;

	/* Skip on null sequences */
	if (!seq)
		return;

	/* Skip single character sequences */
	if (strlen(seq) < 2)
		return;

	/* If no keys defined, go directly to insert mode */
	first = 1;
	if (nxtkey != keymap) {
		
		/* Start at top of key map */
		cur = keymap;
		
		/* Loop until matches exhast */
		while (*seq) {
			
			/* Do we match current character */
			if (*seq == cur->ch) {
				
				/* Advance to next level */
				seq++;
				cur = cur->nxtlvl;
				first = 0;
			} else {
				
				/* Try next character on same level */
				nxtcur = cur->samlvl;
				
				/* Stop if no more */
				if (nxtcur)
					cur = nxtcur;
				else
					break;
			}
		}
	}
	
	/* Check for room in keymap */
	if (strlen(seq) > NKEYENT - (nxtkey - keymap))
		return;
		
	/* If first character in sequence is inserted, add to prefix table */
	if (first)
		keyseq[*seq] = 1;
		
	/* If characters are left over, insert them into list */
	for (first = 1; *seq; first = 0) {
		
		/* Make new entry */
		nxtkey->ch = *seq++;
		nxtkey->code = fn;
		
		/* If root, nothing to do */
		if (nxtkey != keymap) {
			
			/* Set first to samlvl, others to nxtlvl */
			if (first)
				cur->samlvl = nxtkey;
			else
				cur->nxtlvl = nxtkey;
		}

		/* Advance to next key */
		cur = nxtkey++;
	}
}

/** Grab input characters, with wait **/
unsigned char grabwait()
{
	unsigned char ch;

	/* Change mode, if necessary */
	if (curterm.c_cc[VTIME]) {
		curterm.c_cc[VMIN] = 1;
		curterm.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &curterm);
	}

	/* Perform read */
	if (read(0, &ch, 1) != 1) {
		puts("** Horrible read error occured **");
		exit(1);
	}

	/* Return new character */
	return(ch);
}

/** Grab input characters, short wait **/
unsigned char grabnowait()
{
	int count;
	unsigned char ch;

	/* Change mode, if necessary */
	if (curterm.c_cc[VTIME] == 0) {
		curterm.c_cc[VMIN] = 0;
		curterm.c_cc[VTIME] = MAXVTIME;
		tcsetattr(STDIN_FILENO, TCSANOW, &curterm);
	}

	/* Perform read */
	count = read(0, &ch, 1);
	if (count < 0) {
		puts("** Horrible read error occured **");
		exit(1);
	}
	if (count == 0)
		return(TIMEOUT);

	/* Return new character */
	return(ch);
}

/** Queue input character **/
void qin(ch)
int ch;					/* Character to add		*/
{
	/* Check for overflow */
	if (inbuft == &inbuf[sizeof(inbuf)]) {
		
		/* Annoy user */
		ttbeep();
		return;
	}
	
	/* Add character */
	*inbuft++ = ch;
}

/** Cook input characters **/
void cook()
{
	unsigned char ch;
	struct keyent * cur;
	
	/* Get first character untimed */
	ch = grabwait();
	qin(ch);
	
	/* Skip if the key isn't a special leading escape sequence */
	if (keyseq[ch] == 0)
		return;

	/* Start at root of keymap */
	cur = keymap;

	/* Loop until keymap exhasts */
	while (cur) {

		/* Did we find a matching character */
		if (cur->ch == ch) {

			/* Is this the end */
			if (cur->nxtlvl == NULL) {

				/* Replace all character with new sequence */
				inbuft = inbuf;
				qin(cur->code);
				return;
			} else {
				/* Advance to next level */
				cur = cur->nxtlvl;
			
				/* Get next character, timed */
				ch = grabnowait();
				if (ch == TIMEOUT)
					return;

				/* Queue character */
				qin(ch);
			}
		} else
			/* Try next character on same level */
			cur = cur->samlvl;
	}
}

/** Return cooked characters **/
int ttgetc()
{
	int ch;

	/* Loop until character found */
	while (1) {
	
		/* Get input from buffer, if available */
		if (inbufh != inbuft) {
			ch = *inbufh++;
			if (inbufh == inbuft)
				inbufh = inbuft = inbuf;
			break;
		} else

			/* Fill input buffer */
			cook();
	}
	
	/* Return next character */
	return(ch);
}

#if TYPEAH
int typahead()
{
	unsigned char ch;

	/* See if internal buffer is non-empty */
	if (inbufh != inbuft)
		return(1);

	/* Now check with system */

	ch= grabnowait();

	if (ch==TIMEOUT)
		return 0;
	qin (ch);
	return 1;
}
#endif /* TYPEAH */

/* Surely more than just BSD systems do this */

/** Perform a stop signal **/
int bktoshell(f, n)
{
	/* Reset the terminal and go to the last line */
	vttidy();
	
	/* Okay, stop... */
	kill(getpid(), SIGTSTP);

	/* We should now be back here after resuming */

	/* Reopen the screen and redraw */
	ttopen();
	curwp->w_flag = WFHARD;
	sgarbf = TRUE;

	/* Success */
	return(0);
}

/** Get time of day **/
char * timeset()
{
	long int buf; /* Should be time_t */
	char * sp, * cp;

	char * ctime();

	/* Get system time */
	time(&buf);

	/* Pass system time to converter */
	sp = ctime(&buf);

	/* Eat newline character */
	for (cp = sp; *cp; cp++)
		if (*cp == '\n') {
			*cp = '\0';
			break;
		}
	return(sp);
}

/** Callout to system to perform command **/
int callout(cmd)
char * cmd;				/* Command to execute		*/
{
	int status;

	/* Close down */
	ttmove(term.t_nrow, 0);
	ttflush();
	ttclose();

	/* Do command */
	status = system(cmd) == 0;

	/* Restart system */
        sgarbf = TRUE;
	if (ttopen()) {
		puts("** Error reopening terminal device **");
		exit(1);
	}

	/* Success */
        return(status);
}

/** Create subshell **/
int spawncli(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char * sh;

	/* Don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* Get shell path */
	sh = getenv("SHELL");
	if (!sh)
		sh = "/bin/sh";

	/* Do shell */
	return(callout(sh));
}

/** Spawn a command **/
int spawn(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char line[NLINE];
	int s;

	/* Don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* Get command line */
	s = mlreply("!", line, NLINE);
	if (!s)
		return(s);

	/* Perform the command */
	s = callout(line);

	/* if we are interactive, pause here */
	if (clexec == FALSE) {
	        mlwrite("[End]");
		ttflush();
		ttgetc();
        }
        return(s);
}

/** Execute program **/
int execprg(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	/* Same as spawn */
	return(spawn(f, n));
}

/** Pipe output of program to buffer **/
int pipecmd(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char line[NLINE];
	int s;
	BUFFER * bp;
	WINDOW * wp;
	static char filnam[] = "command";

	/* Don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* Get pipe-in command */
	s = mlreply("@", line, NLINE);
	if (!s)
		return(s);

	/* Get rid of the command output buffer if it exists */
	bp = bfind(filnam, FALSE, 0);
	if (bp) {
		/* Try to make sure we are off screen */
		wp = wheadp;
		while (wp) {
			if (wp->w_bufp == bp) {
				onlywind(FALSE, 1);
				break;
			}
			wp = wp->w_wndp;
		}
		if (!zotbuf(bp))
			return(0);
	}

	/* Add output specification */
	strcat(line, ">");
	strcat(line, filnam);

	/* Do command */
	s = callout(line);
	if (!s)
		return(s);

	/* Split the current window to make room for the command output */
	if (!splitwind(FALSE, 1))
		return(0);

	/* ...and read the stuff in */
	if (!getfile(filnam, FALSE))
		return(0);

	/* Make this window in VIEW mode, update all mode lines */
	curwp->w_bufp->b_mode |= MDVIEW;
	wp = wheadp;
	while (wp) {
		wp->w_flag |= WFMODE;
		wp = wp->w_wndp;
	}

	/* ...and get rid of the temporary file */
	unlink(filnam);
	return(1);
}

/** Filter buffer through command **/
int filter(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char line[NLINE], tmpnam[NFILEN];
	int s;
	BUFFER * bp;
	static char bname1[] = "fltinp";
	static char filnam1[] = "fltinp";
	static char filnam2[] = "fltout";

	/* Don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* Don't allow filtering of VIEW mode buffer */
	if (curbp->b_mode & MDVIEW)
		return(rdonly());

	/* Get the filter name and its args */
	s = mlreply("#", line, NLINE);
	if (!s)
		return(s);

	/* Setup the proper file names */
	bp = curbp;
	strcpy(tmpnam, bp->b_fname);	/* Save the original name */
	strcpy(bp->b_fname, bname1);	/* Set it to our new one */

	/* Write it out, checking for errors */
	if (!writeout(filnam1, "w")) {
		mlwrite("[Cannot write filter file]");
		strcpy(bp->b_fname, tmpnam);
		return(0);
	}

	/* Setup input and output */
	strcat(line," <fltinp >fltout");

	/* Perform command */
	s = callout(line);

	/* If successful, read in file */
	if (s) {
		s = readin(filnam2, FALSE);
		if (s)
			/* Mark buffer as changed */
			bp->b_flag |= BFCHG;
	}
			

	/* Reset file name */
	strcpy(bp->b_fname, tmpnam);

	/* and get rid of the temporary file */
	unlink(filnam1);
	unlink(filnam2);

	/* Show status */
	if (!s)
		mlwrite("[Execution failed]");
	return(s);
}

/** Get first filename from pattern **/
char *getffile(fspec)
char *fspec;				/* Filename specification	*/
{
	int index, point, extflag;

	/* First parse the file path off the file spec */
	strcpy(path, fspec);
	index = strlen(path) - 1;
	while (index >= 0 && (path[index] != '/' &&
		path[index] != '\\' && path[index] != ':'))
		--index;
	path[index+1] = '\0';


	/* Check for an extension */
	point = strlen(fspec) - 1;
	extflag = FALSE;
	while (point >= 0) {
		if (fspec[point] == '.') {
			extflag = TRUE;
			break;
		}
		point--;
	}

	/* Open the directory pointer */
	if (dirptr) {
		closedir(dirptr);
		dirptr = NULL;
	}

	dirptr = opendir((path[0] == '\0') ? "./" : path);

	if (!dirptr)
		return(NULL);

	strcpy(rbuf, path);
	nameptr = &rbuf[strlen(rbuf)];

	/* ...and call for the first file */
	return(getnfile());
}

/** Get next filename from pattern **/
char *getnfile()
{
	int index;
	struct DIRENTRY * dp;
	struct stat fstat;

	/* ...and call for the next file */
	do {
		dp = readdir(dirptr);
		if (!dp)
			return(NULL);

		/* Check to make sure we skip all weird entries except directories */
		strcpy(nameptr, dp->d_name);

	} while (stat(rbuf, &fstat) ||
		!(S_ISDIR(fstat.st_mode) !! S_ISREG(fstat.st_mode)));

	/* if this entry is a directory name, say so */
	if (S_ISDIR(fstat.st_mode))
		strcat(rbuf, DIRSEPSTR);

	/* Return the next file name! */
	return(rbuf);
}


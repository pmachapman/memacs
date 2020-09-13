/*	UNIX:	Unix specific terminal driver
		for MicroEMACS 4.0
	(C)Copyright 1995 D. Lawrence, C. Smith

	Unicode support by Jean-Michel Dubois
*/


/** Include files **/
#include <stdio.h>			/* Standard I/O definitions	*/
#include "estruct.h"			/* Emacs definitions		*/
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>			/* I/O control definitions	*/

/** Only compile for UNIX machines **/
#if BSD || FREEBSD || USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || (AVVION || TERMIOS)

/** Include files **/
#include "eproto.h"			/* Function definitions		*/
#include "edef.h"			/* Global variable definitions	*/
#include "elang.h"			/* Language definitions		*/

/** Completion include files **/
/** Directory accessing: Try and figure this out... if you can! **/
#if ((BSD || FREEBSD) && !TERMIOS)
#include <sys/dir.h>			/* Directory entry definitions	*/
#define DIRENTRY	direct
#endif /* (BSD && !TERMIOS) */
#if XENIX || VAT
#include <sys/ndir.h>			/* Directory entry definitions	*/
#define DIRENTRY	direct
#endif /* XENIX */
#if ((USG || AIX || AUX) && !VAT) || SMOS || HPUX8 || HPUX9 || SUN || (AVVION || TERMIOS)
#include <dirent.h>			/* Directory entry definitions	*/
#define DIRENTRY	dirent
#endif /* ((USG || AIX || AUX) && !VAT) || SMOS || HPUX8 || HPUX9 || SUN || (AVVION || TERMIOS) */

static DIR *dirptr = NULL;		/* Current directory stream	*/
static char path[NFILEN];		/* Path of file to find		*/
static char rbuf[NFILEN];		/* Return file buffer		*/
static char *nameptr;			/* Ptr past end of path in rbuf	*/

#if BSD || FREEBSD || SUN || HPUX8 || HPUX9 || (AVVION || TERMIOS)
/* Surely more than just BSD systems do this */

/** Perform a stop signal **/
int bktoshell(f, n)
int f;
int n;
{
	/* Reset the terminal and go to the last line */
	vttidy();

	/* Okay, stop... */
	kill(getpid(), SIGTSTP);

	/* We should now be back here after resuming */

	/* Reopen the screen and redraw */
	scopen();
	curwp->w_flag = WFHARD;
	sgarbf = TRUE;

	/* Success */
	return(0);
}

#endif /* BSD || FREEBSD || SUN || HPUX8 || HPUX9 || (AVVION || TERMIOS) */

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

#if AUX || SMOS || HPUX8 || XENIX
/** Rename a file **/
int rename(file1, file2)
char * file1;				/* Old file name		*/
char * file2;				/* New file name		*/
{
	struct stat buf1;
	struct stat buf2;

	/* No good if source file doesn't exist */
	if (stat(file1, &buf1))
		return(-1);

	/* Check for target */
	if (stat(file2, &buf2) == 0) {

		/* See if file is the same */
		if (buf1.st_dev == buf2.st_dev &&
			buf1.st_ino == buf2.st_ino)

			/* Not necessary to rename file */
			return(0);
	}

	/* Get rid of target */
	unlink(file2);

	/* Link two files together */
	if (link(file1, file2))
		return(-1);

	/* Unlink original file */
	return(unlink(file1));
}
#endif /* AUX || SMOS || HPUX8 || XENIX */

/** Callout to system to perform command **/
int callout(cmd, reopen)
char * cmd;				/* Command to execute		*/
int reopen;
{
	int status;

	/* Close down */
	TTmove(term.t_nrow, 0);
	ttflush();
	TTkclose();
	ttclose();
#if	UTF8
	/* restore locale */
	setlocale(LC_CTYPE, locale);	/* restore LC_CTYPE locale */
#endif
	/* Do command */
	status = system(cmd) == 0;
#if	UTF8
	/* Set locale */
	setlocale(LC_CTYPE, "");
#endif
	/* Restart system */
	if (reopen) {
		sgarbf = TRUE;
		TTkopen();
		if (ttopen()) {
			puts("** Error reopening terminal device **");
			exit(1);
		}
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
#if BSD || FREEBSD || SUN
		sh = "/bin/csh";
#endif /* BSD || FREEBSD || SUN */
#if LINUX
		sh = "/bin/bash";
#elif USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || XENIX || (AVVION || TERMIOS)
		sh = "/bin/sh";
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || XENIX || (AVVION || TERMIOS) */

	/* Do shell */
	return(callout(sh, 1));
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
	s = callout(line, 0);

	/* if we are interactive, pause here */
#if	CURSES
	if (clexec == FALSE) {
	        puts("\n[End]");
		getchar();
        }
	/* Restart system */
        sgarbf = TRUE;
	TTkopen();
	if (ttopen()) {
		puts("** Error reopening terminal device **");
		exit(1);
	}
#else
	/* Restart system */
        sgarbf = TRUE;
	TTkopen();
	if (ttopen()) {
		puts("** Error reopening terminal device **");
		exit(1);
	}

	if (clexec == FALSE) {
	        mlwrite("[End]");
		ttflush();
		ttgetc();
        }
#endif
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
	EWINDOW * wp;
	char filnam[NFILEN];

	/* Don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* Get pipe-in command */
	s = mlreply("@", line, NLINE);
	if (!s)
		return(s);

	/* Initialize temporary file name */
	snprintf(filnam, sizeof(filnam), "command%d", getpid());
	/* Get rid of the command output buffer if it exists */
	bp = bfind("command", FALSE, 0);
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
	s = callout(line, 1);
	if (!s)
		return(s);
	/* Split the current window to make room for the command output */
	if (!splitwind(FALSE, 1))
		return(0);

	/* ...and read the stuff in */
	if (!getfile(filnam, FALSE))
		return(0);

	/* Remove file name */
	*curwp->w_bufp->b_fname = '\0';
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
int uefilter(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char line[NLINE], tempname[NFILEN];
	int s;
	BUFFER * bp;
	char fltpin[NFILEN];
	char fltpout[NFILEN];

	/* Initialize temporary files name */
	snprintf(fltpin, sizeof(fltpin), "fltpin%d", getpid());
	snprintf(fltpout, sizeof(fltpout), "fltpout%d", getpid());

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
	strcpy(tempname, bp->b_fname);	/* Save the original name */
	strcpy(bp->b_fname, "fltpin");	/* Set it to our new one */

	/* Write it out, checking for errors */
	if (!writeout(fltpin, "w")) {
		mlwrite("[Cannot write filter file]");
		strcpy(bp->b_fname, tempname);
		return(0);
	}

	/* Setup input and output */
	strcat(line," <");
	strcat(line, fltpin);
	strcat(line, " >");
	strcat(line, fltpout);

	/* Perform command */
	s = callout(line, 1);

	/* If successful, read in file */
	if (s) {
		s = readin(fltpout, FALSE);
		if (s)
			/* Mark buffer as changed */
			bp->b_flag |= BFCHG;
	}


	/* Reset file name */
	strcpy(bp->b_fname, tempname);

	/* and get rid of the temporary file */
	unlink(fltpin);
	unlink(fltpout);

	/* Show status */
	if (!s)
		mlwrite("[Execution failed]");
	return(s);
}

/** Get first filename from pattern **/
char *getffile(fspec)
char *fspec;				/* Filename specification	*/
{
	int index;

	/* First parse the file path off the file spec */
	strcpy(path, fspec);
	index = strlen(path) - 1;
	while (index >= 0 && (path[index] != '/'))
		--index;
	path[index+1] = '\0';

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
		((fstat.st_mode & S_IFMT) & (S_IFREG | S_IFDIR)) == 0);

	/* if this entry is a directory name, say so */
	if ((fstat.st_mode & S_IFMT) == S_IFDIR)
		strcat(rbuf, DIRSEPSTR);

	/* Return the next file name! */
	return(rbuf);
}

#if FLABEL
/*---------------------------------------------------------------------------*

      handle the function keys and function key labels on HP-Terminals
      -----------------------------------------------------------------

      Hellmuth Michaelis	e-mail: hm@hcshh.hcs.de

 *---------------------------------------------------------------------------*/

static unsigned char flabstor[8][50];		/* label & xmit backup store */
static char flabstof[8] = {0,0,0,0,0,0,0,0};	/* filled flag */

int fnclabel(f, n)		/* label a function key */

int f;		/* Default argument */
int n;		/* function key number 1...8 on hp-terminals */

{
	char lbl[20];	/* label string buffer */
	char xmit[5];	/* transmitted string ( ESC 'p'...'w' ) */
	char buf[80];	/* writeout buffer */
	int i;		/* general purpose index */
	int status;	/* return status */

	/* check if we are connected to an hp-terminal */
	if (!hpterm)
		return(FALSE);

	/* must be called with an argument */
	if (f == FALSE) {
		mlwrite(TEXT246);
/*			"%%Need function key number"*/
		return(FALSE);
	}

	/* and it must be a legal key number */
	if (n < 1 || n > 8) {
		mlwrite(TEXT247);
/*			"%%Function key number out of range"*/
		return(FALSE);
	}

	/* get the string to send */
	lbl[0] = '\0';	/* we don't now the label yet */

	if ((status = mlreply(TEXT248, lbl, 19)) != TRUE)
/*			      "Enter Label String: "*/
		return(status);

	lbl[16] = '\0';
 	i = strlen(lbl);

	/* set up escape sequence to send to terminal */
	xmit[0] = 0x1b;
	xmit[1] = 'o' + n;
	xmit[2] = '\0';

	sprintf(flabstor[n-1], "%c&f0a%dk%dd2L%s%s", (char)0x1b, n, i,
					lbl, xmit);
	write(1, flabstor[n-1], strlen(flabstor[n-1]));
	flabstof[n-1] = 1;

	sprintf(buf, "%c&jB", (char)0x1b);
	write(1, buf, strlen(buf));

	return(TRUE);
}

/* display user function key labels */
static void dis_ufk()

{
	int label_num;
	char buf[6];

	if (!hpterm)
		return;

	for (label_num = 0; label_num < 8; label_num++)
		if (flabstof[label_num])
			write(1, flabstor[label_num],
				strlen(flabstor[label_num]));
	sprintf(buf, "%c&jB", (char)0x1b);
	write(1, buf, strlen(buf));
}

/* display system function key labels */
static void dis_sfk()

{
	char buf[6];

	if (!hpterm)
		return;
	sprintf(buf, "%c&jA", (char)0x1b);
	write(1, buf, strlen(buf));
}
#endif /* FLABEL */

#if XENIX && FILOCK
int mkdir(name, mode)
char *name;	/* name of directory to create */
int mode;	/* umask for creation (which we blissfully ignore...) */
{
	char buf[80];

	strcpy(buf, "mkdir ");
	strcat(buf, name);
	strcat(buf, " > /dev/null 2>&1");
	return(system(buf));
}

int rmdir(name)
char *name;	/* name of directory to delete */
{
	char buf[80];

	strcpy(buf,"rmdir ");
	strcat(buf, name);
	strcat(buf, " > /dev/null 2>&1");
	return(system(buf));
}
#endif /* XENIX & FILOCK */

#endif /* BSD || FREEBSD || USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || (AVVION || TERMIOS) */

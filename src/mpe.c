/***
	MPE driver/Microemacs 3.10b/3.10k,
	Copyright 1995 D. Lawrence, C. Smith
 ***/

/** Include files **/
#include <stdio.h>			/* Standard I/O definitions	*/
#include "estruct.h"			/* Emacs definitions		*/

/** Do nothing routine **/
int scnothing()
{
	return(0);
}

/** Only compile for MPE machines **/
#if MPE

/** Include files **/
#include "eproto.h"			/* Function definitions		*/
#include "edef.h"			/* Global variable definitions	*/
#include "elang.h"			/* Language definitions		*/

/** Parameters **/
#define NKEYENT		300		/* Number of keymap entries	*/
#define NINCHAR		64		/* Input buffer size		*/
#define NOUTCHAR	256		/* Output buffer size		*/
#define MARGIN		8		/* Margin size			*/
#define SCRSIZ		64		/* Scroll for margin		*/
#define NPAUSE		10		/* # times thru update to pause */

/** Constants **/
#define CCE		2		/* Command successful		*/
#define TIMEOUT		255		/* No character available	*/

/** Intrinsics **/
#pragma intrinsic FCLOSE		/* File close intrinsic		*/
#pragma intrinsic FREAD			/* File read intrinsic		*/
#pragma intrinsic FOPEN			/* File open intrinsic		*/
#pragma intrinsic FCONTROL		/* File control intrinsic	*/
#pragma intrinsic FDEVICECONTROL	/* Device control intrinsic	*/
#pragma intrinsic FWRITE		/* File write intrinsic		*/

/** Local variables **/
static short mpetermin;			/* Terminal input		*/
static short mpetermout;		/* Terminal output		*/
static short mpetype;			/* Terminal type		*/
static short mpeoecho;			/* Old echo mode		*/
static short mpeotahd;			/* Old typeahead mode		*/
#if COLOR
static int cfcolor = -1;		/* Current forground color	*/
static int cbcolor = -1;		/* Current background color	*/
#endif /* COLOR */
static int inbuf[NINCHAR];		/* Input buffer			*/
static int * inbufh =			/* Head of input buffer		*/
	inbuf;
static int * inbuft =			/* Tail of input buffer		*/
	inbuf;
static unsigned char outbuf[NOUTCHAR];	/* Output buffer		*/
static unsigned char * outbuft = 	/* Output buffer tail		*/
	outbuf;

/** Terminal definition block **/
int scopen(), scclose(), ttgetc(), ttputc(), ttflush();
int scmove(), sceeol(), sceeop(), scbeep(), screv();
#if COLOR
int scfcol(), scbcol();
#endif /* COLOR */
TERM term = {
	23,				/* Maximum number of rows	*/
	23,				/* Current number of rows	*/
	80,				/* Maximum number of columns	*/
	80,				/* Current number of columns	*/
	0,				/* Origin row			*/
	0,				/* Origin column		*/
	MARGIN,				/* Margin for extending lines	*/
	SCRSIZ,				/* Scroll size for extending	*/
	NPAUSE,				/* # times thru update to pause	*/
	scopen,				/* Open terminal routine	*/
	scclose,			/* Close terminal routine	*/
	scnothing,			/* Open keyboard routine	*/
	scnothing,			/* Close keyboard routine	*/
	ttgetc,				/* Get character routine	*/
	ttputc,				/* Put character routine	*/
	ttflush,			/* Flush output routine		*/
	scmove,				/* Move cursor routine		*/
	sceeol,				/* Erase to end of line routine	*/
	sceeop,				/* Erase to end of page routine	*/
	sceeop,				/* Clear desk			*/
	scbeep,				/* Beep! routine		*/
	screv,				/* Set reverse video routine	*/
	scnothing,			/* Set resolution routine	*/
#if COLOR
	scfcol,				/* Set forground color routine	*/
	scbcol				/* Set background color routine	*/
#endif /* COLOR */
#if INSDEL
	scnothing,			/* Insert a screen line		*/
	scnothing,			/* Delete a screen line		*/
#endif /* INSDEL */
};

/** Open terminal device **/
int ttopen()
{
	short value;
	short error;

	/* Open terminal for input */
	mpetermin = FOPEN(0, 0444, 0, -80);
	if (ccode() != CCE) {
#ifdef DEBUG
		printf("Cannot open $STDIN\n");
#endif /* DEBUG */
		goto error_stdin;
	}

	/* Open terminal for output */
	mpetermout = FOPEN(0, 0414, 1, -80);
	if (ccode() != CCE) {
#ifdef DEBUG
		printf("Cannot open $STDLIST\n");
#endif /* DEBUG */
		goto error_stdlist;
	}

	/* Get terminal type */
	FCONTROL(mpetermin, 39, &mpetype);
	if (ccode() != CCE) {
#ifdef DEBUG
		printf("FCONTROL(39) failed\n");
#endif /* DEBUG */
		goto error_other;
	}

	/* Set transparent editing mode */
#ifdef DEBUG
	value = '*';
#else
	value = '\377';
#endif
	FDEVICECONTROL(mpetermin, &value, 1, 192, 15, 2, &error);
	if (ccode() != CCE) {
#ifdef DEBUG
		printf("FDEVICECONTROL(15) failed; error %d\n", error);
#endif /* DEBUG */
		goto error_other;
	}

	/* Turn off echo */
	mpeoecho = 0;
	FDEVICECONTROL(mpetermin, &mpeoecho, 1, 192, 4, 3, &error);
	if (ccode() != CCE) {
#ifdef DEBUG
		printf("FDEVICECONTROL(4) failed; error %d\n", error);
#endif /* DEBUG */
		goto error_other;
	}

	/* Set type ahead */
	mpeotahd = 1;
	FDEVICECONTROL(mpetermin, &mpeotahd, 1, 192, 51, 3, &error);
	if (ccode() != CCE) {
#ifdef DEBUG
		printf("FDEVICECONTROL(51) failed; error %d\n", error);
#endif /* DEBUG */
		goto error_other;
	}

	/* Success */
	return(0);

error_other:
	FCLOSE(mpetermout, 0, 0);
error_stdlist:
	FCLOSE(mpetermin, 0, 0);
error_stdin:
	return -1;
}

/** Close terminal device **/
int ttclose()
{
	short value;
	short error;

	/* Cancel transparent editing mode */
	value = 0;
	FDEVICECONTROL(mpetermin, &value, 1, 192, 15, 2, &error);
#ifdef DEBUG
	if (ccode() != CCE) {
		printf("FDEVICECONTROL(15) failed; error %d\n", error);
	}
#endif /* DEBUG */

	/* Restore echoing */
	FDEVICECONTROL(mpetermin, &mpeoecho, 1, 192, 4, 2, &error);
#ifdef DEBUG
	if (ccode() != CCE) {
		printf("FDEVICECONTROL(4) failed; error %d\n", error);
	}
#endif /* DEBUG */

	/* Restore type ahead */
	FDEVICECONTROL(mpetermin, &mpeotahd, 1, 192, 51, 2, &error);
#ifdef DEBUG
	if (ccode() != CCE) {
		printf("FDEVICECONTROL(51) failed; error %d\n", error);
	}
#endif /* DEBUG */

	/* Close terminal */
	FCLOSE(mpetermin, 0, 0);
	FCLOSE(mpetermout, 0, 0);

	/* Success */
	return(0);
}

/** Flush output buffer to display **/
int ttflush()
{
	int len;

	/* Compute length of write */
	len = outbuft - outbuf;
	if (len == 0)
		return(0);

	/* Reset buffer position */
	outbuft = outbuf;

	/* Perform write to screen */
	FWRITE(mpetermout, outbuf, -len, 0320);
#ifdef DEBUG
	if (ccode() != CCE) {
		printf("FWRITE fails\n");
	}
#endif
	return 0;
}

/** Put character onto display **/
int ttputc(ch)
char ch;				/* Character to display		*/
{
	/* Check for buffer full */
	if (outbuft == &outbuf[sizeof(outbuf)])
		ttflush();

	/* Add to buffer */
	*outbuft++ = ch;
	return(0);
}

/** Put out sequence, with padding **/
void putpad(pad, seq)
int pad;				/* Number of padding characters	*/
char * seq;				/* Character sequence		*/
{
	/* Check for null */
	if (!seq)
		return;

	/* Output sequence */
	while (*seq)
		ttputc(*seq++);

	/* Output padding */
	while (pad-- > 0)
		ttputc('\0');
}

/** Grab input characters, with wait **/
unsigned char grabwait()
{
	char ch;
	int count;

	/* Read character */
	count = FREAD(mpetermin, &ch, -1);
	if (count != 1) {
		puts("** Horrible read error occured **");
		exit(1);
	}

	/* Return new character */
	return ch;
}

/** Grab input characters, short wait **/
unsigned char grabnowait()
{
	char ch;
	int count;
	short value;
	short error;

	/* Set read timeout */
	value = 1;
	FDEVICECONTROL(mpetermin, &value, 1, 192, 2, 2, &error);
#ifdef DEBUG
	if (ccode() != CCE) {
		printf("FDEVICECONTROL(2) failed; error %d\n", error);
	}
#endif /* DEBUG */

	/* Read character */
	count = FREAD(mpetermin, &ch, -1);
	if (count == 0)
		return TIMEOUT;
	if (count != 1) {
		puts("** Horrible read error occured **");
		exit(1);
	}

	/* Return new character */
	return ch;
}

/** Queue input character **/
void qin(ch)
int ch;					/* Character to add		*/
{
	/* Check for overflow */
	if (inbuft == &inbuf[sizeof(inbuf)]) {
		
		/* Annoy user */
		scbeep();
		return;
	}
	
	/* Add character */
	*inbuft++ = ch;
}

/*
 * qrep - replace a key sequence with a single character in the input buffer.
 */
#if PROTO
VOID qrep(int ch)
#else
VOID qrep( ch)
int ch;
#endif
{
	inbuft = inbuf;
	qin(ch);
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
	return ch;
}

#if TYPEAH
int typahead()
{
	/* See if internal buffer is non-empty */
	if (inbufh != inbuft)
		return 1;

	/* Now check with system */
	/* Offhand, I don't see a function call but I'm sure there is a way */
	return 0;
}
#endif /* TYPEAH */

/** Initialize screen package **/
int scopen()
{
	strcpy(os, "MPE");

	/* Open terminal device */
	if (ttopen()) {
		puts("Cannot open terminal");
		exit(1);
	}

	/* Success */
	return(0);
}

/** Close screen package **/
int scclose()
{
	/* Close terminal device */
	ttclose();

	/* Success */
	return(0);
}

/** Move cursor **/
int scmove(row, col)
int row;				/* Row number			*/
int col;				/* Column number		*/
{
	char buffer[12];

	/* Output addressing */
	sprintf(buffer, "\033&a%dc%dY", col, row);
	putpad(6, buffer);

	/* Success */
	return(0);
}

/** Erase to end of line **/
int sceeol()
{
	/* Send erase sequence */
	putpad(0, "\033K");

	/* Success */
	return(0);
}

/** Clear screen **/
int sceeop()
{
#if COLOR
	scfcol(gfcolor);
	scbcol(gbcolor);
#endif /* COLOR */

	/* Send clear sequence */
	putpad(0, "\033H\033J");

	/* Success */
	return(0);
}

/** Set reverse video state **/
int screv(state)
int state;				/* New state			*/
{
#if COLOR
	int ftmp, btmp;		/* temporaries for colors */
#endif /* COLOR */

	/* Set reverse video state */
	putpad(0, state ? "\033&dJ" : "\033&d@");

#if COLOR
	if (state == FALSE) {
		ftmp = cfcolor;
		btmp = cbcolor;
		cfcolor = -1;
		cbcolor = -1;
		scfcol(ftmp);
		scbcol(btmp);
	}
#endif /* COLOR */

	/* Success */
	return(0);
}

/** Beep **/
scbeep()
{
	/* The old standby method */
	ttputc('\7');

	/* Success */
	return(0);
}

#if COLOR
/** Set foreground color **/
int scfcol(color)
int color;		/* Color to set			*/
{
	/* Skip if already the correct color */
	if (color == cfcolor)
		return;

	/* Set the color */
	cfcolor = color;
	return(0);
}

/** Set background color **/
int scbcol(color)
int color;			/* Color to set			*/
{
	/* Skip if already the correct color */
	if (color == cbcolor)
		return;

	/* Set the color */
        cbcolor = color;
	return(0);
}
#endif /* COLOR */

/** Set palette **/
int spal(cmd)
char * cmd;				/* Palette command		*/
{
	int code, dokeymap;
	char * cp;

	/* Check for keymapping command */
	if (strncmp(cmd, "KEYMAP ", 7) == 0)
		dokeymap = 1;
	else if (strncmp(cmd, "CLRMAP ", 7) == 0)
		dokeymap = 0;
	else
		return(0);
	cmd += 7;

	/* Look for space */
	for (cp = cmd; *cp != '\0'; cp++)
		if (*cp == ' ') {
			*cp++ = '\0';
			break;
		}
	if (*cp == '\0')
		return 1;

	/* Perform operation */
	if (dokeymap) {

		/* Convert to keycode */
		code = stock(cmd);

		/* Add to tree */
		addkey(cp, code);
	} else {

		/* Convert to color number */
		code = atoi(cmd);
		if (code < 0 || code > 15)
			return 1;

#if COLOR
		/* Move color code to capability structure */
#endif /* COLOR */
	}
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
	return sp;
}

/** Callout to system to perform command **/
int callout(cmd)
char * cmd;				/* Command to execute		*/
{
	int status;

	/* Close down */
	scmove(term.t_nrow, 0);
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
        return status;
}

/** Create subshell **/
int spawncli(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	char * sh;

	/* Don't allow this command if restricted */
	if (restflag)
		return resterr();

	/* Do shell */
	return callout("ci.pub.sys");
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
		return resterr();

	/* Get command line */
	s = mlreply("!", line, NLINE);
	if (!s)
		return s;

	/* Perform the command */
	s = callout(line);

	/* if we are interactive, pause here */
	if (clexec == FALSE) {
	        mlwrite("[End]");
		ttflush();
		ttgetc();
        }
        return s;
}

/** Execute program **/
int execprg(f, n)
int f;					/* Flags			*/
int n;					/* Argument count		*/
{
	/* Same as spawn */
	return spawn(f, n);
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
	static char filnam[] = "command";

	/* Don't allow this command if restricted */
	if (restflag)
		return resterr();

	/* Get pipe-in command */
	s = mlreply("@", line, NLINE);
	if (!s)
		return s;

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
		return s;

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
	return 1;
}

/** Filter buffer through command **/
int uefilter(f, n)
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
		return resterr();

	/* Don't allow filtering of VIEW mode buffer */
	if (curbp->b_mode & MDVIEW)
		return rdonly();

	/* Get the filter name and its args */
	s = mlreply("#", line, NLINE);
	if (!s)
		return s;

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
	return s;
}

#if FLABEL
int fnclabel(f, n)	/* label a function key */
int f,n;	/* default flag, numeric argument [unused] */
{
	/* on machines with no function keys...don't bother */
	return(TRUE);
}
#endif /* FLABEL */

/** Get first filename from pattern **/
char *getffile(fspec)
char *fspec;				/* Filename specification	*/
{
	return(NULL);
}

/** Get next filename from pattern **/
char *getnfile()
{
	return(NULL);
}

#endif /* MPE */

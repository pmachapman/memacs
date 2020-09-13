/*	UNIX:	Unix specific terminal driver
		for MicroEMACS 4.0
	(C)Copyright 1995 D. Lawrence, C. Smith
*/

/**
	New features: (as of version 3.10)

	1. Timeouts waiting on a function key have been changed from
	35000 to 500000 microseconds.

	2. Additional keymapping entries can be made from the command
	language by issuing a 'set $palette xxx'.  The format of
	xxx is a string as follows:
		"KEYMAP keybinding escape-sequence".
	To add "<ESC><[><A>" as a keybinding of FNN, issue:
		"KEYMAP FNN ~e[A".
	Note that the "~e" sequence represents the escape character in
	the MicroEMACS command language.

	3. Colors are supported.  Under AIX the colors will be pulled
	in automaticly.  For other environments, you can either add
	the termcap entries, C0 to D7.  Or the colors may be defined
	using the command language by issuing a 'set $palette xxx'
	command.  The format of xxx is a string as follows:
		"CLRMAP # escape-sequence".
	The number is a number from 0 to 15, where 0 to 7 is the
	foreground colors, and 8 to 15 as background colors.
	To add foreground color 0 for ansi terminals, issue:
		"CLRMAP 0 ~e[30m".
	
	'Porting notes:

	I have tried to create this file so that it should work
	as well as possible without changes on your part.

	However, if something does go wrong, read the following
	helpful hints:

	1. On SUN-OS4, there is a problem trying to include both
	the termio.h and ioctl.h files.  I wish Sun would get their
	act together.  Even though you get lots of redefined messages,
	it shouldn't cause any problems with the final object.

	2. In the type-ahead detection code, the individual UNIX
	system either has a FIONREAD or a FIORDCHK ioctl call.
	Hopefully, your system uses one of them and this be detected
	correctly without any intervention.

	3. Also lookout for directory handling.  The SCO Xenix system
	is the weirdest I've seen, requiring a special load file
	(see below).  Some machine call the result of a readdir() call
	a "struct direct" or "struct dirent".  Includes files are
	named differently depending of the O/S.  If your system doesn't
	have an opendir()/closedir()/readdir() library call, then
	you should use the public domain utility "ndir".

	To compile:
		Compile all files normally.
	To link:
		Select one of the following operating systems:
			SCO Xenix:
				use "-ltermcap" and "-lx";
			SUN 3 and 4:
				use "-ltermcap";
			IBM-RT, IBM-AIX, ATT UNIX, Altos UNIX, Interactive:
				use "-lcurses".

	- 20 feb 95	New version 4.00 features
	  We added new code to implient a TERMIOS driver
**/

/** Include files **/
#include <stdio.h>			/* Standard I/O definitions	*/
#include "estruct.h"			/* Emacs definitions		*/

/** Do nothing routine **/
int scnothing()
{
	return(0);
}

/** Only compile for UNIX machines **/
#if BSD || FREEBSD || USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || (AVVION || TERMIOS)

/** Include files **/
#include "eproto.h"			/* Function definitions		*/
#include "edef.h"			/* Global variable definitions	*/
#include "elang.h"			/* Language definitions		*/

/** Kill predefined **/
#undef CTRL				/* Problems with CTRL		*/


/** Overall include files **/
#include <sys/types.h>			/* System type definitions	*/
#include <sys/stat.h>			/* File status definitions	*/
#include <sys/ioctl.h>			/* I/O control definitions	*/

/** Additional include files **/
#if	FREEBSD
#define TERMIOS 1
#include <sys/time.h>
#undef	BSD
#include <sys/param.h>
#undef BSD
#define	BSD	0
#endif
#if (BSD && !TERMIOS)
#include <sys/time.h>			/* Timer definitions		*/
#endif /* (BSD && !TERMIOS) */
#if BSD || FREEBSD || SUN || HPUX8 || HPUX9 || (AVVION || TERMIOS) || AIX
#include <signal.h>			/* Signal definitions		*/
#endif /* BSD || FREEBSD || SUN || HPUX8 || HPUX9 || (AVVION || TERMIOS) */
#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX
#include <termio.h>			/* Terminal I/O definitions	*/
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX */
#if (AVVION || TERMIOS)
#include <termios.h>			/* Terminal I/O definitions	*/
#include <unistd.h>
#endif /* (AVVION || TERMIOS) */
#if CURSES
#include <curses.h>			/* Curses screen output		*/
#endif /* CURSES */

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

/** Restore predefined definitions **/
#undef CTRL				/* Restore CTRL			*/
#define CTRL 0x0100

/** Parameters **/
#define NINCHAR		64		/* Input buffer size		*/
#define NOUTCHAR	256		/* Output buffer size		*/
#if TERMCAP || TERMIOS
#define NCAPBUF		1024		/* Termcap storage size		*/
#endif /* TERMCAP */
#define MARGIN		8		/* Margin size			*/
#define SCRSIZ		64		/* Scroll for margin		*/
#define NPAUSE		10		/* # times thru update to pause */

/** CONSTANTS **/
#define TIMEOUT		255		/* No character available	*/

#if TERMCAP || TERMIOS
struct capbind {			/* Capability binding entry	*/
	char * name;			/* Termcap name			*/
	char * store;			/* Storage variable		*/
};
struct keybind {			/* Keybinding entry		*/
	char * name;			/* Termcap name			*/
	int value;			/* Binding value		*/
};
char *reset = (char*) NULL;		/* reset string kjc */
#endif /* TERMCAP */

/** Local variables **/
#if (BSD && !TERMIOS)
static struct sgttyb cursgtty;		/* Current modes		*/
static struct sgttyb oldsgtty;		/* Original modes		*/
static struct tchars oldtchars;		/* Current tchars		*/
static struct ltchars oldlchars;	/* Current ltchars		*/
static char blank[6] =			/* Blank out character set	*/
	{ -1, -1, -1, -1, -1, -1 };
#endif /* (BSD && !TERMIOS) */
#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX
static struct termio curterm;		/* Current modes		*/
static struct termio oldterm;		/* Original modes		*/
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX */
#if (AVVION || TERMIOS)
static struct termios curterm;		/* Current modes		*/
static struct termios oldterm;		/* Original modes		*/
#endif /* (AVVION || TERMIOS) */
#if TERMCAP || TERMIOS
static char tcapbuf[NCAPBUF];		/* Termcap character storage	*/
#define CAP_CL		0		/* Clear to end of page		*/
#define CAP_CM		1		/* Cursor motion		*/
#define CAP_CE		2		/* Clear to end of line		*/
#define CAP_SE		3		/* Standout ends		*/
#define CAP_SO		4		/* Standout (reverse video)	*/
#define CAP_IS		5		/* Initialize screen		*/
#define CAP_KS		6		/* Keypad mode starts		*/
#define CAP_KE		7		/* Keypad mode ends		*/
#define CAP_VB		8		/* Visible bell			*/
#if COLOR
#define CAP_C0		9		/* Foreground color #0		*/
#define CAP_C1		10		/* Foreground color #1		*/
#define CAP_C2		11		/* Foreground color #2		*/
#define CAP_C3		12		/* Foreground color #3		*/
#define CAP_C4		13		/* Foreground color #4		*/
#define CAP_C5		14		/* Foreground color #5		*/
#define CAP_C6		15		/* Foreground color #6		*/
#define CAP_C7		16		/* Foreground color #7		*/
#define CAP_D0		17		/* Background color #0		*/
#define CAP_D1		18		/* Background color #1		*/
#define CAP_D2		19		/* Background color #2		*/
#define CAP_D3		20		/* Background color #3		*/
#define CAP_D4		21		/* Background color #4		*/
#define CAP_D5		22		/* Background color #5		*/
#define CAP_D6		23		/* Background color #6		*/
#define CAP_D7		24		/* Background color #7		*/
#if USG || AIX || AUX
#define CAP_SF		25		/* Set foreground color		*/
#define CAP_SB		26		/* Set background color		*/
#endif /* USG || AIX || AUX */
#endif /* COLOR */
static struct capbind capbind[] = {	/* Capability binding list	*/
	{ "cl" },			/* Clear to end of page		*/
	{ "cm" },			/* Cursor motion		*/
	{ "ce" },			/* Clear to end of line		*/
	{ "se" },			/* Standout ends		*/
	{ "so" },			/* Standout (reverse video)	*/
	{ "is" },			/* Initialize screen		*/
	{ "ks" },			/* Keypad mode starts		*/
	{ "ke" },			/* Keypad mode ends		*/
	{ "vb" },			/* Visible bell			*/
#if COLOR
	{ "c0" },			/* Foreground color #0		*/
	{ "c1" },			/* Foreground color #1		*/
	{ "c2" },			/* Foreground color #2		*/
	{ "c3" },			/* Foreground color #3		*/
	{ "c4" },			/* Foreground color #4		*/
	{ "c5" },			/* Foreground color #5		*/
	{ "c6" },			/* Foreground color #6		*/
	{ "c7" },			/* Foreground color #7		*/
	{ "d0" },			/* Background color #0		*/
	{ "d1" },			/* Background color #1		*/
	{ "d2" },			/* Background color #2		*/
	{ "d3" },			/* Background color #3		*/
	{ "d4" },			/* Background color #4		*/
	{ "d5" },			/* Background color #5		*/
	{ "d6" },			/* Background color #6		*/
	{ "d7" },			/* Background color #7		*/
#if USG || AIX || AUX
	{ "Sf" },			/* Set foreground color		*/
	{ "Sb" },			/* Set background color		*/
#endif /* USG || AIX || AUX */
#endif /* COLOR */
};
#if COLOR
static int cfcolor = -1;		/* Current forground color	*/
static int cbcolor = -1;		/* Current background color	*/
#endif /* COLOR */
static struct keybind keybind[] = {	/* Keybinding list		*/
	{ "bt", SHFT|CTRL|'i' },	/* Back-tab key			*/
	{ "k1", SPEC|'1' },		/* F1 key			*/
	{ "k2", SPEC|'2' },		/* F2 key			*/
	{ "k3", SPEC|'3' },		/* F3 key			*/
	{ "k4", SPEC|'4' },		/* F4 key			*/
	{ "k5", SPEC|'5' },		/* F5 key			*/
	{ "k6", SPEC|'6' },		/* F6 key			*/
	{ "k7", SPEC|'7' },		/* F7 key			*/
	{ "k8", SPEC|'8' },		/* F8 key			*/
	{ "k9", SPEC|'9' },		/* F9 key			*/
	{ "k0", SPEC|'0' },		/* F0 or F10 key		*/
 	{ "k;", SPEC|'0' },		/* F0 or F10 key	(kjc)	*/
	{ "F1", SHFT|SPEC|'1' },	/* Shift-F1 or F11 key		*/
	{ "F2", SHFT|SPEC|'2' },	/* Shift-F2 or F12 key		*/
	{ "F3", SHFT|SPEC|'3' },	/* Shift-F3 or F13 key		*/
	{ "F4", SHFT|SPEC|'4' },	/* Shift-F4 or F14 key		*/
	{ "F5", SHFT|SPEC|'5' },	/* Shift-F5 or F15 key		*/
	{ "F6", SHFT|SPEC|'6' },	/* Shift-F6 or F16 key		*/
	{ "F7", SHFT|SPEC|'7' },	/* Shift-F7 or F17 key		*/
	{ "F8", SHFT|SPEC|'8' },	/* Shift-F8 or F18 key		*/
	{ "F9", SHFT|SPEC|'9' },	/* Shift-F9 or F19 key		*/
	{ "FA", SHFT|SPEC|'0' },	/* Shift-F0 or F20 key		*/
	{ "kA", CTRL|'O' },		/* Insert line key		*/
	{ "kb", CTRL|'H' },		/* Backspace key		*/
	{ "kC", CTRL|'L' },		/* Clear screen key		*/
	{ "kD", SPEC|'D' },		/* Delete character key		*/
	{ "kd", SPEC|'N' },		/* Down arrow key		*/
	{ "kE", CTRL|'K' },		/* Clear to end of line key	*/
	{ "kF", CTRL|'V' },		/* Scroll forward key		*/
	{ "kH", SPEC|'>' },		/* Home down key		*/
 	{ "@7", SPEC|'>' },		/* Home down key	(kjc)	*/
	{ "kh", SPEC|'<' },		/* Home key			*/
	{ "kI", SPEC|'C' },		/* Insert character key		*/
	{ "kL", CTRL|'K' },		/* Delete line key		*/
	{ "kl", SPEC|'B' },		/* Left arrow key		*/
	{ "kN", SPEC|'V' },		/* Next page key		*/
	{ "kP", SPEC|'Z' },		/* Previous page key		*/
	{ "kR", CTRL|'Z' },		/* Scroll backward key		*/
	{ "kr", SPEC|'F' },		/* Right arrow key		*/
	{ "ku", SPEC|'P' },		/* Up arrow key			*/
        { "K1", SPEC|'<' },		/* Keypad 7 -> Home		*/
        { "K2", SPEC|'V' },		/* Keypad 9 -> Page Up		*/
        { "K3", ' ' },			/* Keypad 5 			*/
        { "K4", SPEC|'>' },		/* Keypad 1 -> End		*/
        { "K5", CTRL|'V' },		/* Keypad 3 -> Page Down	*/
 	{ "kw", CTRL|'E' }		/* End of line			*/
};
#endif /* TERMCAP */
static int inbuf[NINCHAR];		/* Input buffer			*/
static int * inbufh =			/* Head of input buffer		*/
	inbuf;
static int * inbuft =			/* Tail of input buffer		*/
	inbuf;
#if TERMCAP
static unsigned char outbuf[NOUTCHAR];	/* Output buffer		*/
static unsigned char * outbuft = 	/* Output buffer tail		*/
	outbuf;
#endif /* TERMCAP */

static DIR *dirptr = NULL;		/* Current directory stream	*/
static char path[NFILEN];		/* Path of file to find		*/
static char rbuf[NFILEN];		/* Return file buffer		*/
static char *nameptr;			/* Ptr past end of path in rbuf	*/

/** Terminal definition block **/
int scopen(), scclose(), ttgetc(), ttputc(), ttflush();
int scmove(), sceeol(), sceeop(), scbeep(), screv();
int sckopen(), sckclose();
#if COLOR
int scfcol(), scbcol();
#endif /* COLOR */

#if	TERMCAP && FLABEL
static void dis_sfk(), dis_ufk();
#endif

TERM term = {
	120,				/* Maximum number of rows	*/
	0,				/* Current number of rows	*/
	132,				/* Maximum number of columns	*/
	0,				/* Current number of columns	*/
	0, 0,				/* upper left corner default screen */
	MARGIN,				/* Margin for extending lines	*/
	SCRSIZ,				/* Scroll size for extending	*/
	NPAUSE,				/* # times thru update to pause	*/
	scopen,				/* Open terminal routine	*/
	scclose,			/* Close terminal routine	*/
	sckopen,			/* Open keyboard routine	*/
	sckclose,			/* Close keyboard routine	*/
	ttgetc,				/* Get character routine	*/
	ttputc,				/* Put character routine	*/
	ttflush,			/* Flush output routine		*/
	scmove,				/* Move cursor routine		*/
	sceeol,				/* Erase to end of line routine	*/
	sceeop,				/* Erase to end of page routine	*/
	sceeop,				/* Clear the desktop		*/
	scbeep,				/* Beep! routine		*/
	screv,				/* Set reverse video routine	*/
	scnothing,			/* Set resolution routine	*/
#if COLOR
	scfcol,				/* Set forground color routine	*/
	scbcol,				/* Set background color routine	*/
#endif /* COLOR */
#if	INSDEL
	scinsline,			 /* insert a screen line 	*/
	scdelline,			 /* delete a screen line 	*/
#endif	/* INSDEL */
};

int hpterm;				/* global flag braindead HP-terminal */

/** Open terminal device **/
int ttopen()
{
	strcpy(os, "UNIX");
#if (BSD && !TERMIOS)
	/* Get tty modes */
	if (ioctl(0, TIOCGETP, &oldsgtty) ||
		ioctl(0, TIOCGETC, &oldtchars) ||
		ioctl(0, TIOCGLTC, &oldlchars))
		return(-1);

	/* Save to original mode variables */
	cursgtty = oldsgtty;

	/* Set new modes */
	cursgtty.sg_flags |= CBREAK;
	cursgtty.sg_flags &= ~(ECHO|CRMOD);

	/* Set tty modes */
	if (ioctl(0, TIOCSETP, &cursgtty) ||
		ioctl(0, TIOCSETC, blank) ||
		ioctl(0, TIOCSLTC, blank))
		return(-1);
#endif /* (BSD && !TERMIOS) */
#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX

#if SMOS
	/* Extended settings; 890619mhs A3 */
	set_parm(0,-1,-1);
#endif /* SMOS */

	/* Get modes */
	if (ioctl(0, TCGETA, &oldterm)) {
		perror("Cannot TCGETA");
		return(-1);
	}

	/* Save to original mode variable */
	curterm = oldterm;

	/* Set new modes */
	/* I do not believe the flow control settings of the OS should
	   be diddled by an application program. But if you do, change this
	   1 to a 0, but be warned, all sorts of terminals will get grief
	   with this */
#if	1
	curterm.c_iflag &= ~(INLCR|ICRNL|IGNCR);
#else
	curterm.c_iflag &= ~(INLCR|ICRNL|IGNCR|IXON|IXANY|IXOFF);
#endif

	curterm.c_lflag &= ~(ICANON|ISIG|ECHO|IEXTEN);
	curterm.c_cc[VMIN] = 1;
	curterm.c_cc[VTIME] = 0;

#if SMOS
	/****THIS IS A BIG GUESS ON MY PART... the code changed
	  too much between versions for me to be sure this will work - DML */

	/* Allow multiple (dual) sessions if already enabled */
	curterm.c_lflag = oldterm.c_lflag & ISIG;

	/* Use old SWTCH char if necessary */
	if (curterm.c_lflag != 0)
		curterm.c_cc[VSWTCH] = oldterm.c_cc[VSWTCH];

	/* Copy VTI settings	*/
	curterm.c_cc[VTBIT] = oldterm.c_cc[VTBIT];

	/* Extended settings; 890619mhs A3 */
	set_parm(0,-1,-1);
#endif /* SMOS */

	/* Set tty mode */
	if (ioctl(0, TCSETA, &curterm)) {
		perror("Cannot TCSETA");
		return(-1);
	}
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX */
#if (AVVION || TERMIOS)
	/* Get modes */
	if (tcgetattr(0, &oldterm)) {
		perror("Cannot tcgetattr");
		return(-1);
	}

	/* Save to original mode variable */
	curterm = oldterm;

	/* Set new modes */
        /* disable XON/XOFF. We want to use ^S/^Q */
	curterm.c_iflag &= ~(INLCR|ICRNL|IGNCR|IXON|IXANY|IXOFF);
	curterm.c_lflag &= ~(ICANON|ISIG|ECHO|IEXTEN);
	curterm.c_cc[VMIN] = 1;
	curterm.c_cc[VTIME] = 0;
#ifdef	VLNEXT
	curterm.c_cc[VLNEXT] = -1;
#endif

#if	AVVION
	/* Set line discipline for Data General */
	curterm.c_line = 0;
#endif

	/* Set tty mode */
	if (tcsetattr(0, TCSANOW, &curterm)) {
		perror("Cannot tcsetattr");
		return(-1);
	}
#endif /* (AVVION || TERMIOS) */

	/* Success */
	return(0);
}

/** Close terminal device **/
int ttclose()
{
#if ((AIX == 0) && (TERMIOS == 0)) || (FREEBSD == 1)
	/* Restore original terminal modes */
	if (reset != (char*)NULL)
		write(1, reset, strlen(reset));
#endif

#if (BSD && !TERMIOS)
	if (ioctl(0, TIOCSETP, &oldsgtty) ||
		ioctl(0, TIOCSETC, &oldtchars) ||
		ioctl(0, TIOCSLTC, &oldlchars))
		return(-1);
#endif /* (BSD && !TERMIOS) */

#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX
#if SMOS
	/* Extended settings; 890619mhs A3 */
	set_parm(0,-1,-1);
#endif /* SMOS */
	if (ioctl(0, TCSETA, &oldterm))
		return(-1);
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX */

#if (AVVION || TERMIOS)
	/* Set tty mode */
	if (tcsetattr(0, TCSANOW, &oldterm)) {
		perror("Cannot tcsetattr");
		return(-1);
	}
#endif /* (AVVION || TERMIOS) */

	/* Success */
	return(0);
}

/** Flush output buffer to display **/
int ttflush()
{
#if TERMCAP
	int len;

	/* Compute length of write */
	len = outbuft - outbuf;
	if (len == 0)
		return(0);

	/* Reset buffer position */
	outbuft = outbuf;

	/* Perform write to screen */
	return(write(1, outbuf, len) != len);
#else /* TERMCAP */
#if	CURSES
	refresh();
#endif /* CURSES */
	return(0);
#endif	/* TERMCAP */
}

/** Put character onto display **/
int ttputc(ch)
char ch;				/* Character to display		*/
{
#if TERMCAP
	/* Check for buffer full */
	if (outbuft == &outbuf[sizeof(outbuf)])
		ttflush();

	/* Add to buffer */
	*outbuft++ = ch;
#endif /* TERMCAP */

#if CURSES
	/* Put character on screen */
	addch(ch);
#endif /* CURSES */

	return(0);
}


/** Grab input characters, with wait **/
unsigned char grabwait()
{
#if (BSD && !TERMIOS)
	unsigned char ch;

	/* Perform read */
	if (read(0, &ch, 1) != 1) {
		puts("** Horrible read error occured **");
		exit(1);
	}
	return(ch);
#endif /* (BSD && !TERMIOS) */
#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX || (AVVION || TERMIOS)
	unsigned char ch;

	/* Change mode, if necessary */
	if (curterm.c_cc[VTIME]) {
		curterm.c_cc[VMIN] = 1;
		curterm.c_cc[VTIME] = 0;
#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX
		ioctl(0, TCSETA, &curterm);
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX */
#if (AVVION || TERMIOS)
		tcsetattr(0, TCSANOW, &curterm);
#endif /* (AVVION || TERMIOS) */		
	}

	/* Perform read */
#if HANDLE_WINCH
	while (read(0, &ch, 1) != 1) {
		if (winch_flag)
			return 0;
	}
#else
	if (read(0, &ch, 1) != 1) {
		puts("** Horrible read error occured **");
		exit(1);
	}
#endif
	/* Return new character */
	return(ch);
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX || (AVVION || TERMIOS) */
}

/** Grab input characters, short wait **/
unsigned char grabnowait()
{
#if (BSD && !TERMIOS)
	static struct timeval timout = { 0, 500000L };
	int count, r;

	/* Select input */
	r = 1;
	count = select(1, &r, NULL, NULL, &timout);
	if (count == 0)
		return(TIMEOUT);
	if (count < 0) {
		puts("** Horrible select error occured **");
		exit(1);
	}

	/* Perform read */
	return(grabwait());
#endif /* (BSD && !TERMIOS) */
#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || (AVVION || TERMIOS)
	int count;
	unsigned char ch;

	/* Change mode, if necessary */
	if (curterm.c_cc[VTIME] == 0) {
		curterm.c_cc[VMIN] = 0;
		curterm.c_cc[VTIME] = 5;
#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX
		ioctl(0, TCSETA, &curterm);
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX */
#if (AVVION || TERMIOS)
		tcsetattr(0, TCSANOW, &curterm);
#endif /* (AVVION || TERMIOS) */		
	}

	/* Perform read */
#if HANDLE_WINCH
	while ((count = read(0, &ch, 1)) < 0) {
		if (winch_flag)
			return 0;
	}
#else
	count = read(0, &ch, 1);
	if (count < 0) {
		puts("** Horrible read error occured **");
		exit(1);
	}
#endif
	if (count == 0)
		return(TIMEOUT);

	/* Return new character */
	return(ch);
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || (AVVION || TERMIOS) */
}

/*
 * qin - queue in a character to the input buffer.
 */
#if PROTO
VOID qin(int ch)
#else
VOID qin( ch)
int ch;
#endif
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
ttflush();
	/* Loop until character is in input buffer */
	while (inbufh == inbuft)
		cook();
	
	/* Get input from buffer, now that it is available */
	ch = *inbufh++;

	/* reset us to the beginning of the buffer if there are no more
	   pending characters */
	if (inbufh == inbuft)
		inbufh = inbuft = inbuf;
	
	/* Return next character */
	return(ch);
}

#if TYPEAH
int typahead()
{
	int count;

	/* See if internal buffer is non-empty */
	if (inbufh != inbuft)
		return(1);

	/* Now check with system */
#ifdef FIONREAD  /* Watch out!  This could bite you! */
	/* Get number of pending characters */
	if (ioctl(0, FIONREAD, &count))
		return(0);
	return(count);
#else /* not FIONREAD */
#ifdef VAT
	return(0);
#else /* not VAT */
	/* Ask hardware for count */
	count = ioctl(0, FIORDCHK, 0);
	if (count < 0)
		return(0);
	return(count);
#endif	/* VAT */
#endif /* FIONREAD */
}
#endif /* TYPEAH */

#if TERMCAP || TERMIOS
/** Put out sequence, with padding **/
void putpad(seq)
char * seq;				/* Character sequence		*/
{
	/* Check for null */
	if (!seq)
		return;

	/* Call on termcap to send sequence */
	tputs(seq, 1, ttputc);
}
#endif /* TERMCAP */

/** Initialize screen package **/
int scopen()
{
#if TERMCAP || TERMIOS
	char * cp, tcbuf[1024];
	int status;
	struct capbind * cb;
	struct keybind * kp;
	char err_str[NSTRING];

	char  * tgetstr();

#ifndef VAT
#define TGETSTR(a,b)	tgetstr((a), (b))
#else
#define TGETSTR(a,b)	tgetstr((a), *(b))
#endif

#if HPUX8 || HPUX9 || VAT || AUX || (AVVION || TERMIOS) || AIX

	/* HP-UX and AUX doesn't seem to have these in the termcap library */
	char PC, * UP;
	short ospeed;
#else /* not HPUX8 || HPUX9 || VAT || AUX */
	extern char PC, * UP;
	extern short ospeed;
#endif /* HPUX8 || HPUX9 || VAT || AUX */

	/* Get terminal type */
	cp = getenv("TERM");
	if (!cp) {
		puts(TEXT182);
/*		"Environment variable \"TERM\" not define!" */
		exit(1);
	}

	/* Try to load termcap */
	status = tgetent(tcbuf, cp);
	if (status == -1) {
		puts("Cannot open termcap file");
		exit(1);
	}
	if (status == 0) {
		sprintf(err_str, TEXT183, cp);
/*		"No entry for terminal type \"%s\"\n" */
		puts(err_str);
		exit(1);
	}

	/* Get size from termcap */
	term.t_nrow = tgetnum("li") - 1;
	term.t_ncol = tgetnum("co");
	if (term.t_nrow < 3 || term.t_ncol < 3) {
		puts("Screen size is too small!");
		exit(1);
	}

	/* initialize max number of rows and cols	 */
	term.t_mrow = term.t_nrow;
	term.t_mcol = term.t_ncol;

	/* Start grabbing termcap commands */
	cp = tcapbuf;

	/* Get the reset string */
	reset = TGETSTR("is", &cp);

	/* Get the pad character */
	if (tgetstr("pc", &cp))
		PC = tcapbuf[0];

	/* Get up line capability */
	UP = TGETSTR("up", &cp);

	/* Get other capabilities */
	cb = capbind;
	while (cb < &capbind[sizeof(capbind)/sizeof(*capbind)]) {
		cb->store = TGETSTR(cb->name, &cp);
		cb++;
	}

	/* Check for minimum */
	if (!capbind[CAP_CL].store && (!capbind[CAP_CM].store || !UP)) {
		puts("This terminal doesn't have enough power to run microEmacs!");
		exit(1);
	}

	/* Set reverse video and erase to end of line */
	if (capbind[CAP_SO].store && capbind[CAP_SE].store)
		revexist = TRUE;
	if (!capbind[CAP_CE].store)
		eolexist = FALSE;

#if	0
	/* Get keybindings */
	kp = keybind;
	while (kp < &keybind[sizeof(keybind)/sizeof(*keybind)]) {
		addkey(TGETSTR(kp->name, &cp), kp->value);
		kp++;
	}
#endif

	/* check for HP-Terminal (so we can label its function keys) */
	hpterm = tgetflag("xs");

	/* Open terminal device */
	if (ttopen()) {
		puts("Cannot open terminal");
		exit(1);
	}

	/* Set speed for padding sequences */
#if (BSD && !TERMIOS)
	ospeed = cursgtty.sg_ospeed;
#endif /* (BSD && !TERMIOS) */
#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX
	ospeed = curterm.c_cflag & CBAUD;
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX */
#if (AVVION || TERMIOS)
	ospeed = cfgetospeed(&curterm);
#endif /* (AVVION || TERMIOS) */
	
	/* Send out initialization sequences */
#if AIX == 0
	putpad(capbind[CAP_IS].store);
#endif
	putpad(capbind[CAP_KS].store);
	sckopen();
#endif /* TERMCAP */

#if CURSES
	/* Initialize screen */
	initscr();

	/* Set size of screen */
	term.t_nrow = LINES - 1;
	term.t_ncol = COLS;

	/* Open terminal device */
	if (ttopen()) {
		puts("Cannot open terminal");
		exit(1);
	}
#endif /* CURSES */

	/* Success */
	return(0);
}

/** Close screen package **/
int scclose()
{
#if TERMCAP
	/* Turn off keypad mode */
	putpad(capbind[CAP_KE].store);
	sckclose();

	/* Close terminal device */
	ttflush();
	ttclose();
#endif /* TERMCAP */

#if	TERMIOS
	/* Close terminal device */
	ttflush();
	ttclose();
#endif	/* TERMIOS */

#if CURSES
	/* Turn off curses */
	endwin();

	/* Close terminal device */
	ttflush();
	ttclose();
#endif /* CURSES */

	/* Success */
	return(0);
}

/* open keyboard -hm */
int sckopen()
{
#if TERMCAP
	putpad(capbind[CAP_KS].store);
	ttflush();
#if	FLABEL
	dis_ufk();
#endif
#endif
}

/* close keyboard -hm */
int sckclose()
{
#if TERMCAP
	putpad(capbind[CAP_KE].store);
	ttflush();
#if	FLABEL
	dis_sfk();
#endif
#endif
}

/** Move cursor **/
int scmove(row, col)
int row;				/* Row number			*/
int col;				/* Column number		*/
{
	char *tgoto();

#if TERMCAP || TERMIOS
	/* Call on termcap to create move sequence */
	putpad(tgoto(capbind[CAP_CM].store, col, row));
#endif /* TERMCAP */

#if CURSES
	move(row, col);
#endif /* CURSES */

	/* Success */
	return(0);
}

/** Erase to end of line **/
int sceeol()
{
#if TERMCAP || TERMIOS
	/* Send erase sequence */
	putpad(capbind[CAP_CE].store);
#endif /* TERMCAP */

#if CURSES
	clrtoeol();
#endif /* CURSES */

	/* Success */
	return(0);
}

/** Clear screen **/
int sceeop()
{
#if TERMCAP || TERMIOS
#if COLOR
	scfcol(gfcolor);
	scbcol(gbcolor);
#endif /* COLOR */
	/* Send clear sequence */
	putpad(capbind[CAP_CL].store);
#endif /* TERMCAP */

#if CURSES
	erase();
#endif /* CURSES */


	/* Success */
	return(0);
}

/** Set reverse video state **/
int screv(state)
int state;				/* New state			*/
{
#if TERMCAP || TERMIOS
#if COLOR
	int ftmp, btmp;		/* temporaries for colors */
#endif /* COLOR */

	/* Set reverse video state */
	putpad(state ? capbind[CAP_SO].store : capbind[CAP_SE].store);

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
#endif /* TERMCAP */

#if CURSES
	if (state)
		standout();
	else
		standend();
#endif /* CURSES */

	/* Success */
	return(0);
}

/** Beep **/
scbeep()
{
#if TERMCAP || TERMIOS
#if !NOISY
	/* Send out visible bell, if it exists */
	if (capbind[CAP_VB].store)
		putpad(capbind[CAP_VB].store);
	else
#endif /* not NOISY */
		/* The old standby method */
		ttputc('\7');
#endif /* TERMCAP */

#if CURSES
	addch('\7');		/* FIX THIS! beep() and flash comes up undefined */
#endif /* CURSES */

	/* Success */
	return(0);
}

#if COLOR
static char cmap[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };

/** Set foreground color **/
int scfcol(color)
int color;		/* Color to set			*/
{
#if TERMCAP || TERMIOS
	/* Skip if already the correct color */
	if (color == cfcolor)
		return(0);

	/* Send out color sequence */
	if (capbind[CAP_C0].store) {
		putpad(capbind[CAP_C0 + (color & 7)].store);
		cfcolor = color;
	}
#if USG || AUX
	else if (capbind[CAP_SF].store) {
		putpad(tparm(capbind[CAP_SF].store, cmap[color & 7]));
		cfcolor = color;
	}
#endif /* USG || AUX */
#endif /* TERMCAP */

#if CURSES
	/* ? */
#endif /* CURSES */
	return(0);
}

/** Set background color **/
int scbcol(color)
int color;			/* Color to set			*/
{
#if TERMCAP || TERMIOS
	/* Skip if already the correct color */
	if (color == cbcolor)
		return(0);

	/* Send out color sequence */
	if (capbind[CAP_C0].store) {
		putpad(capbind[CAP_D0 + (color & 7)].store);
		cbcolor = color;
	}
#if USG || AUX
	else if (capbind[CAP_SB].store) {
		putpad(tparm(capbind[CAP_SB].store, cmap[color & 7]));
		cbcolor = color;
	}
#endif /* USG || AUX */
#endif /* TERMCAP */

#if CURSES
	/* ? */
#endif /* CURSES */
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
	else
#if TERMCAP
#if COLOR
	if (strncmp(cmd, "CLRMAP ", 7) == 0)
		dokeymap = 0;
	else
#endif /* COLOR */
#endif /* TERMCAP */
		return(0);
	cmd += 7;

	/* Look for space */
	for (cp = cmd; *cp != '\0'; cp++)
		if (*cp == ' ') {
			*cp++ = '\0';
			break;
		}
	if (*cp == '\0')
		return(1);

	/* Perform operation */
	if (dokeymap) {

		/* Convert to keycode */
		code = stock(cmd);

		/* Add to tree */
		addkey(cp, code);
	}
#if TERMCAP
#if COLOR
	else {

		/* Convert to color number */
		code = atoi(cmd);
		if (code < 0 || code > 15)
			return(1);

		/* Move color code to capability structure */
		capbind[CAP_C0 + code].store = malloc(strlen(cp) + 1);
		if (capbind[CAP_C0 + code].store)
			strcpy(capbind[CAP_C0 + code].store, cp);
	}
#endif /* COLOR */
#endif /* TERMCAP */
	return(0);
}

#if BSD || FREEBSD || SUN || HPUX8 || HPUX9 || (AVVION || TERMIOS)
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

#if USG || AUX || SMOS || HPUX8 || XENIX
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
#endif /* USG || AUX || SMOS || HPUX8 || XENIX */

/** Callout to system to perform command **/
int callout(cmd)
char * cmd;				/* Command to execute		*/
{
	int status;

	/* Close down */
	scmove(term.t_nrow, 0);
	ttflush();
	sckclose();
	ttclose();

	/* Do command */
	status = system(cmd) == 0;

	/* Restart system */
        sgarbf = TRUE;
	sckopen();
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
#if BSD || FREEBSD || SUN
		sh = "/bin/csh";
#endif /* BSD || FREEBSD || SUN */
#if USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || XENIX || (AVVION || TERMIOS)
		sh = "/bin/sh";
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || XENIX || (AVVION || TERMIOS) */

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
	EWINDOW * wp;
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
	while (index >= 0 && (path[index] != '/'))
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

#if HANDLE_WINCH
/*
 * Window size changes handled via signals.
 */
void winch_changed()
{
	signal(SIGWINCH,winch_changed);
	winch_flag = 1;
}

void winch_new_size()
{
	EWINDOW *wp;
	struct winsize win;
  
	winch_flag=0;
	ioctl(fileno(stdin),TIOCGWINSZ,&win);
	winch_vtresize(win.ws_row,win.ws_col);
	onlywind(0,0);
	TTmove(0,0);
	TTeeop();
}
#endif

#endif /* BSD || FREEBSD || USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || (AVVION || TERMIOS) */

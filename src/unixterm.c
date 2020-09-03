/*	UNIX:	Unix specific terminal driver
		for MicroEMACS 4.0
	(C)Copyright 1995 D. Lawrence, C. Smith

	Unicode support by Jean-Michel Dubois
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
	in automatically.  For other environments, you can either add
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
	  We added new code to implement a TERMIOS driver
	- 10 mar 2020   Jean-Michel Dubois
	  Split in three parts. Now :
	  unix.c now contains directory and system related functions only.
	  tcapunix.c contains termcap screen and keyboard driver.
	  (I should see why there is also a tcap.c file)
	  curses.c contains curses screen and keyboard driver.
	  !!!!! Tested with Linux only !!!!!
**/

/** Include files **/
#include "estruct.h"			/* Emacs definitions		*/

#if	TERMCAP

#include <stdio.h>			/* Standard I/O definitions	*/
#include <signal.h>

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
#include <sys/ioctl.h>			/* I/O control definitions	*/

/** Additional include files **/
#if	LINUX
#include <term.h>
#undef TERMIOS
#endif
#if	FREEBSD
#define TERMIOS 1
#undef	BSD
#include <sys/param.h>
#undef BSD
#define	BSD	0
#endif
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

/** Restore predefined definitions **/
#undef CTRL				/* Restore CTRL			*/
#if	UTF8
#define CTRL 0x01000000
#else
#define CTRL 0x0100
#endif

/** Parameters **/
#define NINCHAR		64		/* Input buffer size		*/
#define NOUTCHAR	256		/* Output buffer size		*/

#ifdef SCO
#define NCAPBUF		2048		/* Termcap storage size		*/
#elif LINUX
#define NCAPBUF		4096
#else
#define NCAPBUF		1024		/* Termcap storage size		*/
#endif

#define MARGIN		8		/* Margin size			*/
#define SCRSIZ		64		/* Scroll for margin		*/
#define NPAUSE		500		/* Pause in milliseconds	*/

/** CONSTANTS **/
#define TIMEOUT		255		/* No character available	*/

struct capbind {			/* Capability binding entry	*/
	char * name;			/* Termcap name			*/
	char * store;			/* Storage variable		*/
};
struct keybind {			/* Keybinding entry		*/
	char * name;			/* Termcap name			*/
	int value;			/* Binding value		*/
};
char *reset = (char*) NULL;		/* reset string kjc */

/** Local variables **/
#if (BSD && !TERMIOS)
static struct sgttyb cursgtty;		/* Current modes		*/
static struct sgttyb oldsgtty;		/* Original modes		*/
static struct tchars oldtchars;		/* Current tchars		*/
static struct ltchars oldlchars;	/* Current ltchars		*/
static char blank[6] =			/* Blank out character set	*/
	{ -1, -1, -1, -1, -1, -1 };
#endif /* (BSD && !TERMIOS) */
#if AVVION || (USG && ! TERMIOS) || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX
static struct termio curterm;		/* Current modes		*/
static struct termio oldterm;		/* Original modes		*/
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || (SUN && !TERMIOS) || XENIX */
#if (AVVION || TERMIOS)
static struct termios curterm;		/* Current modes		*/
static struct termios oldterm;		/* Original modes		*/
#endif /* (AVVION || TERMIOS) */

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
#define CAP_MD		9		/* Bold on			*/
#define CAP_ME		10		/* Bold	off			*/
#if COLOR
#define CAP_C0		11		/* Foreground color #0		*/
#define CAP_C1		12		/* Foreground color #1		*/
#define CAP_C2		13		/* Foreground color #2		*/
#define CAP_C3		14		/* Foreground color #3		*/
#define CAP_C4		15		/* Foreground color #4		*/
#define CAP_C5		16		/* Foreground color #5		*/
#define CAP_C6		17		/* Foreground color #6		*/
#define CAP_C7		18		/* Foreground color #7		*/
#define CAP_D0		19		/* Background color #0		*/
#define CAP_D1		20		/* Background color #1		*/
#define CAP_D2		21		/* Background color #2		*/
#define CAP_D3		22		/* Background color #3		*/
#define CAP_D4		23		/* Background color #4		*/
#define CAP_D5		24		/* Background color #5		*/
#define CAP_D6		25		/* Background color #6		*/
#define CAP_D7		26		/* Background color #7		*/
#if USG || AIX || AUX
#define CAP_SF		27		/* Set foreground color		*/
#define CAP_SB		28		/* Set background color		*/
#endif /* USG || AIX || AUX */
#endif /* COLOR */
static struct capbind capbind[] = {	/* Capability binding list	*/
	{ "cl", NULL },			/* Clear to end of page		*/
	{ "cm", NULL },			/* Cursor motion		*/
	{ "ce", NULL },			/* Clear to end of line		*/
	{ "se", NULL },			/* Standout ends		*/
	{ "so", NULL },			/* Standout (reverse video)	*/
	{ "is", NULL },			/* Initialize screen		*/
	{ "ks", NULL },			/* Keypad mode starts		*/
	{ "ke", NULL },			/* Keypad mode ends		*/
	{ "vb", NULL },			/* Visible bell			*/
	{ "md", NULL },			/* Set bold on			*/
	{ "me", NULL },			/* Set bold off			*/
#if COLOR
	{ "c0", NULL },			/* Foreground color #0		*/
	{ "c1", NULL },			/* Foreground color #1		*/
	{ "c2", NULL },			/* Foreground color #2		*/
	{ "c3", NULL },			/* Foreground color #3		*/
	{ "c4", NULL },			/* Foreground color #4		*/
	{ "c5", NULL },			/* Foreground color #5		*/
	{ "c6", NULL },			/* Foreground color #6		*/
	{ "c7", NULL },			/* Foreground color #7		*/
	{ "d0", NULL },			/* Background color #0		*/
	{ "d1", NULL },			/* Background color #1		*/
	{ "d2", NULL },			/* Background color #2		*/
	{ "d3", NULL },			/* Background color #3		*/
	{ "d4", NULL },			/* Background color #4		*/
	{ "d5", NULL },			/* Background color #5		*/
	{ "d6", NULL },			/* Background color #6		*/
	{ "d7", NULL },			/* Background color #7		*/
#if USG || AIX || AUX
	{ "Sf", NULL },			/* Set foreground color		*/
	{ "Sb", NULL },			/* Set background color		*/
#endif /* USG || AIX || AUX */
#if LINUX
	{ "AF", NULL },			/* Set foreground color		*/
	{ "AB", NULL },			/* Set background color		*/
#endif
#endif /* COLOR */
};
static struct keybind keybind[] = {	/* Keybinding list		*/
	{ "bt", SHFT|CTRL|'i' },	/* Back-tab key			*/
#if	LINUX
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
	{ "F1", SPEC|'a' },		/* F11 key			*/
	{ "F2", SPEC|'b' },		/* F12 key			*/
	{ "F3", SHFT|SPEC|'1' },	/* Shift-F1 or F11 key		*/
	{ "F4", SHFT|SPEC|'2' },	/* Shift-F2 or F12 key		*/
	{ "F5", SHFT|SPEC|'3' },	/* Shift-F3 or F13 key		*/
	{ "F6", SHFT|SPEC|'4' },	/* Shift-F4 or F14 key		*/
	{ "F7", SHFT|SPEC|'5' },	/* Shift-F5 or F15 key		*/
	{ "F8", SHFT|SPEC|'6' },	/* Shift-F6 or F16 key		*/
	{ "F9", SHFT|SPEC|'7' },	/* Shift-F7 or F17 key		*/
	{ "FA", SHFT|SPEC|'8' },	/* Shift-F8 or F18 key		*/
	{ "FB", SHFT|SPEC|'9' },	/* Shift-F9 or F19 key		*/
	{ "FC", SHFT|SPEC|'0' },	/* Shift-F0 or F20 key		*/
	{ "FD", SHFT|SPEC|'a' },	/* Shift-F0 or F20 key		*/
	{ "FE", SHFT|SPEC|'b' },	/* Shift-F0 or F20 key		*/
	{ "kA", CTRL|'O' },		/* Insert line key		*/
	{ "kb", CTRL|'H' },		/* Backspace key		*/
	{ "kC", CTRL|'L' },		/* Clear screen key		*/
	{ "kD", SPEC|'D' },		/* Delete character key		*/
	{ "kd", SPEC|'N' },		/* Down arrow key		*/
	{ "kE", CTRL|'K' },		/* Clear to end of line key	*/
	{ "kF", CTRL|'V' },		/* Scroll forward key		*/
	{ "kH", CTRL|'E' },		/* Home down key		*/
 	{ "@7", CTRL|'E' },		/* Home down key	(kjc)	*/
	{ "kh", CTRL|'A' },		/* Home key			*/
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
#else
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
	{ "l1", SHFT|SPEC|'1' },	/* F1 key			*/
	{ "l2", SHFT|SPEC|'2' },	/* F2 key			*/
	{ "l3", SHFT|SPEC|'3' },	/* F3 key			*/
	{ "l4", SHFT|SPEC|'4' },	/* F4 key			*/
	{ "l5", SHFT|SPEC|'5' },	/* F5 key			*/
	{ "l6", SHFT|SPEC|'6' },	/* F6 key			*/
	{ "l7", SHFT|SPEC|'7' },	/* F7 key			*/
	{ "l8", SHFT|SPEC|'8' },	/* F8 key			*/
	{ "l9", SHFT|SPEC|'9' },	/* F9 key			*/
	{ "l0", SHFT|SPEC|'0' },	/* F0 or F10 key		*/
 	{ "l;", SPEC|'0' },		/* F0 or F10 key	(kjc)	*/
	{ "kA", CTRL|'O' },		/* Insert line key		*/
	{ "kb", CTRL|'H' },		/* Backspace key		*/
	{ "kC", CTRL|'L' },		/* Clear screen key		*/
	{ "kD", SPEC|'D' },		/* Delete character key		*/
	{ "kd", SPEC|'N' },		/* Down arrow key		*/
	{ "kE", CTRL|'K' },		/* Clear to end of line key	*/
	{ "kF", CTRL|'V' },		/* Scroll forward key		*/
	{ "EN", SPEC|'>' },		/* Home down key		*/
	{ "kh", SPEC|'<' },		/* Home key			*/
	{ "kI", SPEC|'C' },		/* Insert character key		*/
	{ "kL", CTRL|'K' },		/* Delete line key		*/
	{ "kl", SPEC|'B' },		/* Left arrow key		*/
	{ "kN", SPEC|'V' },		/* Next page key		*/
	{ "kP", SPEC|'Z' },		/* Previous page key		*/
	{ "PD", SPEC|'V' },		/* Next page key		*/
	{ "PU", SPEC|'Z' },		/* Previous page key		*/
	{ "kR", CTRL|'Z' },		/* Scroll backward key		*/
	{ "kr", SPEC|'F' },		/* Right arrow key		*/
	{ "ku", SPEC|'P' }		/* Up arrow key			*/
#endif
};

#if COLOR
static int cfcolor = -1;		/* Current foreground color	*/
static int cbcolor = -1;		/* Current background color	*/
#endif /* COLOR */
static int inbuf[NINCHAR];		/* Input buffer			*/
static int * inbufh =			/* Head of input buffer		*/
	inbuf;
static int * inbuft =			/* Tail of input buffer		*/
	inbuf;
#if	UTF8
static unsigned outbuf[NOUTCHAR];		/* Output buffer		*/
static unsigned * outbuft = 		/* Output buffer tail		*/
	outbuf;
#else
static unsigned char outbuf[NOUTCHAR];	/* Output buffer		*/
static unsigned char * outbuft = 	/* Output buffer tail		*/
	outbuf;
#endif /* UTF8 */

/** Terminal definition block **/
int scopen(), scclose(), ttgetc(), ttputc(), ttflush();
int scmove(), sceeol(), sceeop(), scbeep(), screv();
int sckopen(), sckclose();
#if COLOR
int scfcol(), scbcol();
#endif /* COLOR */

#if	FLABEL
static void dis_sfk(), dis_ufk();
#endif

TERM term = {
	512,				/* Maximum number of rows	*/
	25,				/* Current number of rows	*/
	512,				/* Maximum number of columns	*/
	80,				/* Current number of columns	*/
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
#if LINUX
	strcpy(os, "LINUX");
#else
	strcpy(os, "UNIX");
#endif
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
	int len;

	/* Compute length of write */
	len = outbuft - outbuf;
	if (len == 0)
		return(0);

	/* Reset buffer position */
	outbuft = outbuf;
#if	UTF8
        char utf8[6];
        unsigned int bytes;

	while (len--) {
	        bytes = unicode_to_utf8(*outbuft, utf8);
        	write(1, utf8, bytes);
		outbuft++;
	}

	outbuft = outbuf;
#else
	/* Perform write to screen */
	return(write(1, outbuf, len) != len);
#endif /* UTF8 */
	return(0);
}

/** Put character onto display **/
int ttputc(ch)
int ch;				/* Character to display		*/
{
	/* Check for buffer full */
	if (outbuft == &outbuf[NOUTCHAR])
		ttflush();
	/* Add to buffer */
#if	UTF8
	*outbuft++ = ((unsigned) ch) & CMSK;
#else
	*outbuft++ = ch;
#endif
	return(0);
}

#if	UTF8
static char buffer[32];
static int pending = 0;

int grabwait(void)
{
	unsigned c;
	int count, bytes = 1, expected;

	count = pending;

	if (!count) {
		curterm.c_cc[VMIN] = 1;
		curterm.c_cc[VTIME] = 0;
		ioctl(0, TCSETA, &curterm);
		count = read(0, buffer, sizeof(buffer));
		if (count <= 0)
			return 0;
		pending = count;
	}

	c = (unsigned char) buffer[0];

	if (c == 27 || (c >= 32 && c < 128))
		goto done;

	/*
	 * Lazy. We don't bother calculating the exact
	 * expected length. We want at least two characters
	 * for the special character case (ESC+[) and for
	 * the normal short UTF8 sequence that starts with
	 * the 110xxxxx pattern.
	 *
	 * But if we have any of the other patterns, just
	 * try to get more characters. At worst, that will
	 * just result in a barely perceptible 0.1 second
	 * delay for some *very* unusual utf8 character
	 * input.
	 */
	expected = ((c & 0xe2) == 0xe0 || (c & 0xe2) == 0xe2) ? 6 : 2;

	/* Special character - try to fill buffer */
	if (count < expected) {
		int n;
		curterm.c_cc[VMIN] = 0;
		curterm.c_cc[VTIME] = 1;		/* A .1 second lag */
		ioctl(0, TCSETA, &curterm);
		n = read(0, buffer + count, sizeof(buffer) - count);

		if (n > 0)
			pending += n;
	}

	bytes = utf8_to_unicode(buffer, 0, pending, &c);

	/* Hackety hack! Turn no-break space into regular space */
	if (c == 0xa0)
		c = ' ';
done:
	if ((pending - bytes) > 0 && bytes < 32) {
		pending -= bytes;
		memmove(buffer, buffer+bytes, pending);
	} else
		pending = 0;
	return c;
}

int grabnowait(void)
{
	unsigned c;
	int count, bytes = 1, expected;

	count = pending;
	if (!count) {

		curterm.c_cc[VMIN] = 0;
		curterm.c_cc[VTIME] = 1;
		ioctl(0, TCSETA, &curterm);
		count = read(0, buffer, sizeof(buffer));

		if (count <= 0)
			return TIMEOUT;
		pending = count;
	}

	c = (unsigned char) buffer[0];
	if (c == 27 || (c >= 32 && c < 128))
		goto done;

	/*
	 * Lazy. We don't bother calculating the exact
	 * expected length. We want at least two characters
	 * for the special character case (ESC+[) and for
	 * the normal short UTF8 sequence that starts with
	 * the 110xxxxx pattern.
	 *
	 * But if we have any of the other patterns, just
	 * try to get more characters. At worst, that will
	 * just result in a barely perceptible 0.1 second
	 * delay for some *very* unusual utf8 character
	 * input.
	 */
	expected = ((c & 0xe2) == 0xe0 || (c & 0xe2) == 0xe2) ? 6 : 2;

	/* Special character - try to fill buffer */
	if (count < expected) {
		int n;
		curterm.c_cc[VMIN] = 0;
		curterm.c_cc[VTIME] = 1;		/* A .1 second lag */
		ioctl(0, TCSETA, &curterm);
		n = read(0, buffer + count, sizeof(buffer) - count);

		if (n > 0)
			pending += n;
	}

	bytes = utf8_to_unicode(buffer, 0, pending, &c);

	/* Hackety hack! Turn no-break space into regular space */
	if (c == 0xa0)
		c = ' ';
done:
	pending -= bytes;
	memmove(buffer, buffer+bytes, pending);
	return c;
}

#else	/* UTF8 */

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
#if	JMDEXT
		curterm.c_cc[VTIME] = 2;
#else
		curterm.c_cc[VTIME] = 5;
#endif
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
		puts("** Horrible read error occurred **");
		exit(1);
	}
#endif
	if (count == 0)
		return(TIMEOUT);

	/* Return new character */
	return(ch);
#endif /* USG || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || (AVVION || TERMIOS) */
}

#endif	/* UTF8 */

/*
 * qin - queue in a character to the input buffer.
 */
VOID qin(int ch)
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
VOID qrep(int ch)
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

/** Initialize screen package **/
int scopen()
{
	char * cp, tcbuf[NCAPBUF];
	int status;
	struct capbind * cb;
	struct keybind * kp;
	char err_str[NSTRING];

#ifndef VAT
#define TGETSTR(a,b)	tgetstr((a), (b))
#else
#define TGETSTR(a,b)	tgetstr((a), *(b))
#endif

#if (HPUX8 || HPUX9 || VAT || AUX || AVVION || TERMIOS || AIX)

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

	/* Set size of screen */
	/* Get default size from termcap */
	term.t_nrow = tgetnum("li") - 1;
	term.t_ncol = tgetnum("co");
	/* Override size if environment variables exists */
	cp = getenv("LINES");
	if (cp)
		term.t_nrow = atoi(cp) - 1;

	cp = getenv("COLUMNS");
	if (cp)
		term.t_ncol = atoi(cp);
	/* Get real size from screen driver */
#ifdef TIOCGWINSZ
	{
		struct winsize w;
		if (ioctl(2, TIOCGWINSZ, &w) == 0)
		{
			if (w.ws_row > 0)
				term.t_nrow = w.ws_row - 1;
			if (w.ws_col > 0)
				term.t_ncol = w.ws_col;
		}
	}
#else
#ifdef WIOCGETD
	{
		struct uwdata w;
		if (ioctl(2, WIOCGETD, &w) == 0)
		{
			if (w.uw_height > 0)
				term.t_nrow = (w.uw_height / w.uw_vs) - 1;
			if (w.uw_width > 0)
				term.t_ncol = w.uw_width / w.uw_hs;
		}
	}
#endif
#endif

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

	/* Get keybindings */
	kp = keybind;
	while (kp < &keybind[sizeof(keybind)/sizeof(*keybind)]) {
		addkey(TGETSTR(kp->name, &cp), kp->value);
		kp++;
	}
#if LINUX
	//* numeric keypad without caps lock */
	addkey("\x1bOo", '/');
	addkey("\x1bOj", '*');
	addkey("\x1bOm", '-');
	addkey("\x1bOk", '+');
	addkey("\x1bOM", '\n');
	addkey("\x1b[1;5A", CTLX|CTRL|'P');	// move-window-up
	addkey("\x1b[1;5B", CTLX|CTRL|'N');	// move-window-down
	addkey("\x1b[1;5C", META|'F');		// forward-word
	addkey("\x1b[1;5D", META|'B');		// backward-word
	addkey("\x1b[1;5F", META|'>');		// end-of-file
	addkey("\x1b[1;5H", META|'<');		// beginning-of-file
	addkey("\x1b[3;5~", META|CTRL|'Y');	// delete-kill-ring
	addkey("\x1b[5;5~", META|CTRL|'Z');	// scroll-next-up
	addkey("\x1b[6;5~", META|CTRL|'V');	// scroll-next-down
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
/*
	if (capbind[CAP_MD].store)
		putpad(capbind[CAP_MD].store);
*/
	/* Success */
	return(0);
}

/** Close screen package **/
int scclose()
{
	/* Turn off keypad mode */
	putpad(capbind[CAP_KE].store);
	sckclose();
	/* Close terminal device */
	ttflush();
	ttclose();
	/* Success */
	return(0);
}

/* open keyboard -hm */
int sckopen()
{
	putpad(capbind[CAP_KS].store);
	ttflush();
#if	FLABEL
	dis_ufk();
#endif
	return(TRUE);
}

/* close keyboard -hm */
int sckclose()
{
	putpad(capbind[CAP_KE].store);
	ttflush();
#if	FLABEL
	dis_sfk();
#endif
	return(TRUE);
}

/** Move cursor **/
int scmove(row, col)
int row;				/* Row number			*/
int col;				/* Column number		*/
{
	char *tgoto();

	/* Call on termcap to create move sequence */
	putpad(tgoto(capbind[CAP_CM].store, col, row));
	/* Success */
	return(0);
}

/** Erase to end of line **/
int sceeol()
{
#if COLOR
//	scfcol(gfcolor);
//	scbcol(gbcolor);
#endif /* COLOR */
	/* Send erase sequence */
	putpad(capbind[CAP_CE].store);
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
	putpad(capbind[CAP_CL].store);
	/* Success */
	return(0);
}

/** Set reverse video state **/
int screv(state)
int state;			/* New state			*/
{
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
	/* Success */
	return(0);
}

/** Beep **/
int scbeep()
{
#if !NOISY
	/* Send out visible bell, if it exists */
	if (capbind[CAP_VB].store)
		putpad(capbind[CAP_VB].store);
	else
#endif /* not NOISY */
		/* The old standby method */
		ttputc('\7');
	/* Success */
	return(0);
}

#if COLOR
static char cmap[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };

/** Set foreground color **/
int scfcol(color)
int color;		/* Color to set			*/
{
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
#ifdef	SCO
		char s[20];
		sprintf(s, capbind[CAP_SF].store, color & 7);
		putpad(s);
#else	/* SCO */
		putpad(tparm(capbind[CAP_SF].store, cmap[color & 7]));
#endif	/* SCO */
		cfcolor = color;
	}
#endif /* USG || AUX */
	return(0);
}

/** Set background color **/
int scbcol(color)
int color;			/* Color to set			*/
{
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
#ifdef	SCO
		char s[20];
		sprintf(s, capbind[CAP_SB].store, color & 7);
		putpad(s);
#else	/* SCO */
		putpad(tparm(capbind[CAP_SB].store, cmap[color & 7]));
#endif	/* SCO */
		cbcolor = color;
	}
#endif /* USG || AUX */
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
#if COLOR
	if (strncmp(cmd, "CLRMAP ", 7) == 0)
		dokeymap = 0;
	else
#endif /* COLOR */
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
		code = stock((unsigned char*) cmd);

		/* Add to tree */
		addkey(cp, code);
	}
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
	return(0);
}

#if HANDLE_WINCH
/*
 * Window size changes handled via signals.
 */
void winch_changed(int status)
{
	signal(SIGWINCH,winch_changed);
	winch_flag = 1;
}

void winch_new_size()
{
	struct winsize win;

	winch_flag = 0;
	ioctl(fileno(stdin),TIOCGWINSZ,&win);
	winch_vtresize(win.ws_row,win.ws_col);
	onlywind(0,0);
	TTmove(0,0);
	TTeeop();
}

#endif	/* HANDLE_WINCH */

#endif	/* TERMCAP */

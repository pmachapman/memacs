/*
 * curses.c
 *
 *  Created on: 10 mars 2020
 *      Author: Jean-Michel Dubois
 *
 *      curses terminal with Unicode support
 */

#include "estruct.h"			/* Emacs definitions		*/

#if	CURSES
/** Problem with CTRL */
#undef	CTRL

#include <stdio.h>			/* Standard I/O definitions	*/
#include <signal.h>
#include <time.h>
#include <sys/types.h>			/* System type definitions	*/
#include <sys/stat.h>			/* File status definitions	*/
#include <sys/ioctl.h>			/* I/O control definitions	*/
#if	LINUX
#include <term.h>
#undef TERMIOS
#endif
#if __CYGWIN__
#include <asm/socket.h>
#endif

/** Include files **/
#include "eproto.h"			/* Function definitions		*/
#include "edef.h"			/* Global variable definitions	*/
#include "elang.h"			/* Language definitions		*/

/** Do nothing routine **/
int scnothing()
{
	return(0);
}

/** Parameters **/
#define NINCHAR		64		/* Input buffer size		*/
#define NOUTCHAR	256		/* Output buffer size		*/
#define MARGIN		8		/* Margin size			*/
#define SCRSIZ		64		/* Scroll for margin		*/
#define NPAUSE		500		/* Pause in milliseconds	*/
#define TIMEOUT		255		/* No character available	*/

#if COLOR

static int cfcolor = -1;		/* Current foreground color	*/
static int cbcolor = -1;		/* Current background color	*/
#endif /* COLOR */
static attr_t attrib = 0;		/* Current character attribute	*/
/** Terminal definition block **/
int scopen(), scclose(), ttgetc(), ttputc(), ttflush();
int scmove(), sceeol(), sceeop(), scbeep(), screv();
int sckopen(), sckclose();
#if COLOR
int scfcol(), scbcol();
#endif /* COLOR */

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

#if	MOUSE
#if NCURSES_MOUSE_VERSION > 1
// button 4 and 5 report mouse wheel
#define _ALL_MOUSE_EVENTS (BUTTON1_PRESSED|BUTTON1_RELEASED|BUTTON1_CLICKED|\
			BUTTON2_PRESSED|BUTTON2_RELEASED|BUTTON2_CLICKED|\
			BUTTON3_PRESSED|BUTTON3_RELEASED|BUTTON3_CLICKED|\
			BUTTON4_PRESSED|BUTTON5_PRESSED|\
			REPORT_MOUSE_POSITION)
#else
#define _ALL_MOUSE_EVENTS (BUTTON1_PRESSED|BUTTON1_RELEASED|BUTTON1_CLICKED|\
			BUTTON2_PRESSED|BUTTON2_RELEASED|BUTTON2_CLICKED|\
			BUTTON3_PRESSED|BUTTON3_RELEASED|BUTTON3_CLICKED|\
			REPORT_MOUSE_POSITION)
#endif
#endif

#define FILEMENU	0
#define EDITMENU	1
#define SEARCHMENU	2
#define EXECMENU	3
#define TOOLSMENU	4

typedef struct menuitem {
	char*		i_text;
	short		i_type;
	union EPOINTER	i_ptr;
	int		(*i_markfunc)();	// status function for menu item mark
	int		(*i_checkdisabled)();	// status function for menu item show
	short		i_disabled;
} MENUITEM;

typedef struct menubar {
	char*		m_text;
	short		m_nitems;
	short		m_width;
	MENUITEM*	m_items;
	short		m_left;
	short		m_right;
	unsigned int	m_shortcut;
} MENUBAR;

static int issline(void)
{
	return curbp->b_mode & MDSLINE;
}

static int sline(int n, int f)
{
	if (curbp->b_mode & MDSLINE)
		curbp->b_mode &= ~MDSLINE;
	else {
		curbp->b_mode |= MDSLINE;
		curbp->b_mode &= ~MDDLINE;
	}

	return(TRUE);
}

static int isdline(void)
{
	return curbp->b_mode & MDDLINE;
}

static int dline(int n, int f)
{
	if (curbp->b_mode & MDDLINE)
		curbp->b_mode &= ~MDDLINE;
	else {
		curbp->b_mode |= MDDLINE;
		curbp->b_mode &= ~MDSLINE;
	}

	return(TRUE);
}

static int isnumeric(void)
{
	return ! (curbp->b_mode & (MDSLINE | MDDLINE));
}

static int numeric(int n, int f)
{
	curbp->b_mode &= ~(MDSLINE|MDDLINE);
	return (TRUE);
}

static int isexact(void)
{
	return curbp->b_mode & MDEXACT;
}

static int exact(int n, int f)
{
	if (curbp->b_mode & MDEXACT)
		curbp->b_mode &= ~MDEXACT;
	else
		curbp->b_mode |= MDEXACT;

	return(TRUE);
}

#if	MAGIC
static int ismagic(void)
{
	return curbp->b_mode & MDMAGIC;
}

static int magic(int n, int f)
{
	if (curbp->b_mode & MDMAGIC)
		curbp->b_mode &= ~MDMAGIC;
	else
		curbp->b_mode |= MDMAGIC;

	return(TRUE);
}
#endif

#if	THEOX
static int isutf8(void)
{
	return ! (curbp->b_mode & MDTHEOX);
}

static int istheox(void)
{
	return curbp->b_mode & MDTHEOX;
}
#endif

static int isdebug(void)
{
	return macbug;
}

static int setdebug(int n, int f)
{
	return (macbug = ! macbug);
}

static int isautosave(void)
{
	return curbp->b_mode & MDASAVE;
}

static int autosave(int n, int f)
{
	if (curbp->b_mode & MDASAVE)
		curbp->b_mode &= ~MDASAVE;
	else
		curbp->b_mode |= MDASAVE;

	return(TRUE);
}

static int isnobackup(void)
{
	return curbp->b_mode & MDNOBAK;
}

static int nobackup(int n, int f)
{
	if (curbp->b_mode & MDNOBAK)
		curbp->b_mode &= ~MDNOBAK;
	else
		curbp->b_mode |= MDNOBAK;

	return(TRUE);
}

static MENUITEM filemenu[] = {
	{ "++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " Open...           ^X-^F", BINDFNC, { filefind }, NULL, NULL },
	{ " Insert...         ^X-^I", BINDFNC, { insfile }, NULL, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " View...           ^X-^V", BINDFNC, { viewfile }, NULL, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Append to...      ^X-^A", BINDFNC, { fileapp }, NULL, NULL },
	{ " Save              ^X-^S", BINDFNC, { filesave }, NULL, NULL },
	{ " Save as...        ^X-^W", BINDFNC, { filewrite }, NULL, NULL },
	{ " Save all and quit M-Z  ", BINDFNC, { quickexit }, NULL, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Auto save mode         ", BINDFNC, { autosave }, isautosave, NULL },
	{ " Don't save backup      ", BINDFNC, { nobackup }, isnobackup, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Quit              ^X-^C", BINDFNC, { quit }, NULL, NULL },
	{ "++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};

static MENUITEM editmenu[] = {
	{ "++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " Cut end of line ^K ", BINDFNC, { killtext }, NULL, NULL },
	{ " Cut region      ^W ", BINDFNC, { killregion }, NULL, NULL },
	{ " Copy region     M-W", BINDFNC, { copyregion }, NULL, NULL },
	{ " Paste           ^Y ", BINDFNC, { yank }, NULL, NULL },
	{ "--------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Undo            ^_ ", BINDFNC, { undo }, NULL, NULL },
	{ "++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};

static MENUITEM modifmenu[] = {
	{ "++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " Word lower      M-L  ", BINDFNC, { lowerword }, NULL, NULL },
	{ " Word upper      M-U  ", BINDFNC, { upperword }, NULL, NULL },
	{ " Word capitalize M-C  ", BINDFNC, { capword }, NULL, NULL },
	{ "----------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Region lower    ^X-^L", BINDFNC, { lowerregion }, NULL, NULL },
	{ " Region upper    ^X-^U", BINDFNC, { upperregion }, NULL, NULL },
	{ "----------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Entab region    ^X-E ", BINDFNC, { entab }, NULL, NULL },
	{ " Detab region    ^X-D ", BINDFNC, { detab }, NULL, NULL },
	{ " Trim region     ^X-T ", BINDFNC, { trim }, NULL, NULL },
	{ " Indent to right M-)  ", BINDFNC, { indent_region }, NULL, NULL },
	{ " Indent to left  M-(  ", BINDFNC, { undent_region }, NULL, NULL },
	{ "----------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Swap characters ^T   ", BINDFNC, { twiddle }, NULL, NULL },
#if	MDSLINE
	{ "----------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Single line graphics ", BINDFNC, { sline }, issline, NULL },
	{ " Double line graphics ", BINDFNC, { dline }, isdline, NULL },
	{ " Numeric keypad       ", BINDFNC, { numeric }, isnumeric, NULL },
#endif
#if	THEOX
	{ "----------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Convert to UTF-8     ", BINDFNC, { nullproc }, NULL, isutf8 },	/* bound to b-to-utf8 */
	{ " Convert to TheoX     ", BINDFNC, { nullproc }, NULL, istheox },	/* bound to b-to-theox */
#endif
	{ "++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};

static MENUITEM searchmenu[] = {
	{ "++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " Search forward  f1 ^S  ", BINDFNC, { forwsearch }, NULL, NULL },
	{ " Search backward f2 ^R  ", BINDFNC, { backsearch }, NULL, NULL },
	{ " Next forward    f3     ", BINDFNC, { forwhunt }, NULL, NULL },
	{ " Next backward   f4     ", BINDFNC, { backhunt }, NULL, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
#if	ISRCH
	{ " I-Search forward   ^X-S", BINDFNC, { isearch }, NULL, NULL },
	{ " I-Search backward  ^X-R", BINDFNC, { risearch }, NULL, NULL },
#endif
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Query replace      M-^R", BINDFNC, { qreplace }, NULL, NULL },
	{ " Replace all        M-R ", BINDFNC, { sreplace }, NULL, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Case sensitive         ", BINDFNC, { exact }, isexact, NULL },
#if	MAGIC
	{ " Regular expressions    ", BINDFNC, { magic }, ismagic, NULL },
#endif
	{ "++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};

static MENUITEM windowmenu[] = {
	{ "++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " Grow window      ^X-^  ", BINDFNC, { enlargewind }, NULL, NULL },
	{ " Shrink window    ^X-^Z ", BINDFNC, { shrinkwind }, NULL, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Previous window  ^X-P  ", BINDFNC, { prevwind }, NULL, NULL },
	{ " Next window      ^X-N  ", BINDFNC, { nextwind }, NULL, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Split window     ^X-2  ", BINDFNC, { splitwind }, NULL, NULL },
	{ " Delete window    ^X-0  ", BINDFNC, { delwind }, NULL, NULL },
	{ " Keep one window  ^X-1  ", BINDFNC, { onlywind }, NULL, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Scroll next up   M-PgUp", BINDFNC, { nextup }, NULL, NULL },
	{ " Scroll next down M-PgDn", BINDFNC, { nextdown }, NULL, NULL },
	{ "++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};

static MENUITEM macromenu[] = {
	{ "+++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " Start recording     ^X-(", BINDFNC, { ctlxlp }, NULL, NULL },
	{ " Stop recording      ^X-)", BINDFNC, { ctlxrp }, NULL, NULL },
	{ " Play                ^X-E", BINDFNC, { ctlxe }, NULL, NULL },
	{ "-------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Run uemacs command  M-X ", BINDFNC, { namedcmd }, NULL, NULL },
	{ " Run procedure       M-^E", BINDFNC, { execproc }, NULL, NULL },
	{ " Run file            M-^S", BINDFNC, { execfile }, NULL, NULL },
	{ "-------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Debug mode              ", BINDFNC, { setdebug }, isdebug, NULL },
	{ "+++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};

static MENUITEM shellmenu[] = {
	{ "++++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " Shell command...     ^X-!", BINDFNC, { spawn }, NULL, NULL },
	{ " Interactive shell... ^X-C", BINDFNC, { spawncli }, NULL, NULL },
	{ " Pipe command...      ^X-@", BINDFNC, { pipecmd }, NULL, NULL },
	{ " Filter buffer...     ^X-#", BINDFNC, { uefilter }, NULL, NULL },
	{ "++++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};

static int ishardtab(void)
{
	return tabsize && ! stabsize;
}

static int hardtab(int n, int f)
{
	int status;		/* status return */
	VDESC vd;		/* variable num/type */
	char value[NSTRING];	/* value to set variable to */

	/* check the legality and find the var */
	findvar("hardtab", &vd, NVSIZE + 1, VT_GLOBAL);
	status = mlreply(TEXT53, &value[0], NSTRING);
/*				 "Value: " */
	if (status == ABORT)
		return(status);

	/* and set the appropriate value */
	return svar(&vd, value);
}

static int issofttab(void)
{
	return stabsize;
}

static int softtab(int n, int f)
{
	int status;		/* status return */
	VDESC vd;		/* variable num/type */
	char value[NSTRING];	/* value to set variable to */

	/* check the legality and find the var */
	findvar("softtab", &vd, NVSIZE + 1, VT_GLOBAL);
	status = mlreply(TEXT53, &value[0], NSTRING);
/*				 "Value: " */
	if (status == ABORT)
		return(status);

	/* and set the appropriate value */
	return svar(&vd, value);
}

#if	COLOR && 0
static int setfcol(int n, int f)
{

	return(TRUE);
}

static int setbcol(int n, int f)
{

	return(TRUE);
}
#endif

static MENUITEM setupmenu[] = {
	{ "++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " Hard tab width...      ", BINDFNC, { hardtab }, ishardtab, NULL },
	{ " Soft tab width...      ", BINDFNC, { softtab }, issofttab, NULL },
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Show variable...       ", BINDFNC, { dispvar }, NULL, NULL },
	{ " Set variable...        ", BINDFNC, { setvar }, NULL, NULL },
#if	COLOR && 0
	{ "------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Set foreground color...", BINDFNC, { setfcol }, NULL, NULL },
	{ " Set background color...", BINDFNC, { setbcol }, NULL, NULL },
#endif
	{ "++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};

#if TXBASIC
static MENUITEM langmenu[] = {
	{ "++++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " Format buffer       S-F1 ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Format with braces  S-F2 ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Check syntax        S-F3 ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Compile release     S-F4 ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Compile for speed   S-F5 ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Compile for TxDebug S-F6 ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Compile for gdb     S-F7 ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Run                 S-F8 ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Run through gdb          ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Previous error      S-F9 ", BINDFNC, { nullproc }, NULL, NULL },
	{ " Next error          S-F10", BINDFNC, { nullproc }, NULL, NULL },
	{ "--------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " IF...ELSE...ENDIF        ", BINDFNC, { nullproc }, NULL, NULL },
	{ " SELECT...CASE...CEND     ", BINDFNC, { nullproc }, NULL, NULL },
	{ " FOR...NEXT               ", BINDFNC, { nullproc }, NULL, NULL },
	{ " WHILE...WEND             ", BINDFNC, { nullproc }, NULL, NULL },
	{ "++++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};
#endif

static MENUITEM helpmenu[] = {
	{ "++++++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ " MicroEmacs help        f6  ", BINDFNC, { nullproc }, NULL, NULL },	/* bound to get-help */
#if	LIBHELP
	{ " Language help index    M-I ", BINDFNC, { hlpindex }, NULL, NULL },
	{ " Language help on word  M-H ", BINDFNC, { lookupword }, NULL, NULL },
	{ " Language help on topic M-J ", BINDFNC, { hlpindex }, NULL, NULL },
#endif
	{ "----------------------------", BINDFNC, { nullproc }, NULL, NULL },
	{ " Describe abbreviations     ", BINDFNC, { desc_abbrevs }, NULL, NULL },
	{ " Describe functions         ", BINDFNC, { desfunc }, NULL, NULL },
	{ " Describe key               ", BINDFNC, { deskey }, NULL, NULL },
	{ " Describe key bindings      ", BINDFNC, { desbind }, NULL, NULL },
	{ " Describe variables         ", BINDFNC, { desvars }, NULL, NULL },
	{ "++++++++++++++++++++++++++++", BINDFNC, { nullproc }, NULL, NULL },
	{ NULL, 0, { NULL }, NULL }
};

static MENUBAR menubar[] = {
	{ "File", 0, 0, filemenu },
	{ "Edit", 0, 0, editmenu },
	{ "Modify", 0, 0, modifmenu },
	{ "Search", 0, 0, searchmenu },
	{ "Window", 0, 0, windowmenu },
	{ "Macro", 0, 0, macromenu },
	{ "Shell", 0, 0, shellmenu },
	{ "Setup", 0, 0, setupmenu },
#if TXBASIC
	{ "TxBasic", 0, 0, langmenu },
#endif
	{ "Help", 0, 0, helpmenu },
	{ NULL, 0, 0, NULL }
};

static MENUITEM* dropmenu;

/** Open terminal device **/
int ttopen()
{
#if LINUX
	strcpy(os, "LINUX");
#else
	strcpy(os, "UNIX");
#endif
	reset_prog_mode();
	update_panels();
	doupdate();
	/* Success */
	return(0);
}

/** Close terminal device **/
int ttclose()
{
	endwin();
	/* Success */
	return(0);
}

/** Flush output buffer to display **/
int ttflush()
{
	wrefresh(wmain);
	return(0);
}

/** Put character onto display **/
int ttputc(ch)
int ch;				/* Character to display		*/
{
	/* Put character on screen */
	if (ch == '\b') {
		int y, x;
		getyx(wmain, y, x);
		mvwdelch(wmain, y, x -1);
	} else if (ch == 7) {
		beep();
	} else {
#if	UTF8
		cchar_t	cch;
		wchar_t wch = ch;
#if COLOR
		setcchar(&cch, &wch, attrib, PAIR_NUMBER(cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor), NULL);
#else
		setcchar(&cch, &wch, attrib, 0, NULL);
#endif
		wadd_wch(wmain, &cch);
#else
#if COLOR
		waddch(ch|COLOR_PAIR(wmain, cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor));
#else
		waddch(ch);
#endif
#endif
	}
	return(0);
}

#if	MOUSE

typedef struct mouse {
	MEVENT		m_mousePrevEvent;
	int32_t		m_iMouseMask;
	int32_t		m_iButtonMask;
} mouse;

static mouse Mouse;
static int inbuf[100];			/* Mouse input buffer		*/
static int * inbufh =			/* Head of input buffer		*/
	inbuf;
static int * inbuft =			/* Tail of input buffer		*/
	inbuf;

/*
 * qin - queue in a character to the input buffer.
 */
VOID qin(int ch)
{
	/* Check for overflow */
	if (inbuft >= &inbuf[sizeof(inbuf) / sizeof(int)]) {
		/* Annoy user */
		beep();
		wrefresh(wmain);
		return;
	}

	/* Add character */
	*inbuft++ = ch;
}

void checkmouse(void)
{
	MEVENT mouseEvent;

	if (getmouse(&mouseEvent) == OK) {
		// ncurses 5.9 doesn't report button releases

		if (mouseEvent.bstate & REPORT_MOUSE_POSITION) {
			if ((Mouse.m_mousePrevEvent.bstate & BUTTON1_PRESSED) && ! (mouseEvent.bstate & BUTTON1_PRESSED))
				mouseEvent.bstate |= BUTTON1_PRESSED;

			if ((Mouse.m_mousePrevEvent.bstate & BUTTON2_PRESSED) && ! (mouseEvent.bstate & BUTTON2_PRESSED))
				mouseEvent.bstate |= BUTTON2_PRESSED;

			if ((Mouse.m_mousePrevEvent.bstate & BUTTON3_PRESSED) && ! (mouseEvent.bstate & BUTTON3_PRESSED))
				mouseEvent.bstate |= BUTTON3_PRESSED;
		} else {
			if ((Mouse.m_mousePrevEvent.bstate & BUTTON1_PRESSED) && ! (mouseEvent.bstate & BUTTON1_PRESSED))
				mouseEvent.bstate |= BUTTON1_RELEASED;

			if ((Mouse.m_mousePrevEvent.bstate & BUTTON2_PRESSED) && ! (mouseEvent.bstate & BUTTON2_PRESSED))
				mouseEvent.bstate |= BUTTON2_RELEASED;

			if ((Mouse.m_mousePrevEvent.bstate & BUTTON3_PRESSED) && ! (mouseEvent.bstate & BUTTON3_PRESSED))
				mouseEvent.bstate |= BUTTON3_RELEASED;
		}

		if (mouseEvent.bstate & BUTTON1_RELEASED) {
			Mouse.m_mousePrevEvent.bstate &= ~BUTTON1_PRESSED;
			mouseEvent.bstate &= ~BUTTON1_PRESSED;
			qin(0);
			qin(MOUS >> SHIFTPFX);
			qin(mouseEvent.x);
			qin(mouseEvent.y);
			qin('b');
		}

		if (mouseEvent.bstate & BUTTON2_RELEASED) {
			Mouse.m_mousePrevEvent.bstate &= ~BUTTON2_PRESSED;
			mouseEvent.bstate &= ~BUTTON2_PRESSED;
			qin(0);
			qin(MOUS >> SHIFTPFX);
			qin(mouseEvent.x);
			qin(mouseEvent.y);
			qin('d');
		}

		if (mouseEvent.bstate & BUTTON3_RELEASED) {
			Mouse.m_mousePrevEvent.bstate &= ~BUTTON3_PRESSED;
			mouseEvent.bstate &= ~BUTTON3_PRESSED;
			qin(0);
			qin(MOUS >> SHIFTPFX);
			qin(mouseEvent.x);
			qin(mouseEvent.y);
			qin('f');
		}

		if (mouseEvent.bstate & (BUTTON1_PRESSED | BUTTON1_CLICKED)) {
			qin(0);
			qin(MOUS >> SHIFTPFX);
			qin(mouseEvent.x);
			qin(mouseEvent.y);
			qin('a');
		}

		if (mouseEvent.bstate & (BUTTON2_PRESSED | BUTTON2_CLICKED)) {
			qin(0);
			qin(MOUS >> SHIFTPFX);
			qin(mouseEvent.x);
			qin(mouseEvent.y);
			qin('c');
		}

		if (mouseEvent.bstate & (BUTTON3_PRESSED | BUTTON3_CLICKED)) {
			qin(0);
			qin(MOUS >> SHIFTPFX);
			qin(mouseEvent.x);
			qin(mouseEvent.y);
			qin('e');
		}

		// is mouse dragged ?
		if (mouseEvent.bstate & REPORT_MOUSE_POSITION) {
			qin(0);
			qin(MOUS >> SHIFTPFX);
			qin(mouseEvent.x);
			qin(mouseEvent.y);
			qin('m');
		}

		Mouse.m_mousePrevEvent = mouseEvent;
	}
}

#endif	/* MOUSE */

/* Cook special characters */

int extcode(int ch)
{
	switch (ch) {
		case KEY_DOWN:		/* down-arrow key */
			return (CTRL|'N');
		case KEY_UP:		/* up-arrow key */
			return (CTRL|'P');
		case KEY_LEFT:		/* left-arrow key */
			return (CTRL|'B');
		case KEY_RIGHT:		/* right-arrow key */
			return (CTRL|'F');
		case KEY_HOME:		/* home key */
			return (CTRL|'A');
		case KEY_BACKSPACE:	/* backspace key */
			return (CTRL|'?');
		case KEY_F0+1:		/* Function keys */
			return (SPEC|'1');
		case KEY_F0+2:
			return (SPEC|'2');
		case KEY_F0+3:
			return (SPEC|'3');
		case KEY_F0+4:
			return (SPEC|'4');
		case KEY_F0+5:
			return (SPEC|'5');
		case KEY_F0+6:
			return (SPEC|'6');
		case KEY_F0+7:
			return (SPEC|'7');
		case KEY_F0+8:
			return (SPEC|'8');
		case KEY_F0+9:
			return (SPEC|'9');
		case KEY_F0+10:
			return (SPEC|'0');
		case KEY_F0+11:
			return (SPEC|'a');
		case KEY_F0+12:
			return (SPEC|'b');
		case KEY_F0+13:
			return (SHFT|SPEC|'1');
		case KEY_F0+14:
			return (SHFT|SPEC|'2');
		case KEY_F0+15:
			return (SHFT|SPEC|'3');
		case KEY_F0+16:
			return (SHFT|SPEC|'4');
		case KEY_F0+17:
			return (SHFT|SPEC|'5');
		case KEY_F0+18:
			return (SHFT|SPEC|'6');
		case KEY_F0+19:
			return (SHFT|SPEC|'7');
		case KEY_F0+20:
			return (SHFT|SPEC|'8');
		case KEY_F0+21:
			return (SHFT|SPEC|'9');
		case KEY_F0+22:
			return (SHFT|SPEC|'0');
		case KEY_F0+23:
			return (SHFT|SPEC|'a');
		case KEY_F0+24:
			return (SHFT|SPEC|'b');
		case KEY_DL:		/* delete-line key */
			return (CTRL|'K');
		case KEY_IL:		/* insert-line key */
			return (CTRL|'O');
		case KEY_DC:		/* delete-character key */
			return (SPEC|'D');
		case KEY_IC:		/* insert-character key */
			return (SPEC|'C');
//		case KEY_EIC:		/* sent by rmir or smir in insert mode */
		case KEY_CLEAR:		/* clear-screen or erase key */
			return (SPEC|'L');
//		case KEY_EOS:		/* clear-to-end-of-screen key */
		case KEY_EOL:		/* clear-to-end-of-line key */
			return (CTRL|'K');
		case KEY_SF:		/* scroll-forward key */
			return (CTRL|'V');
		case KEY_SR:		/* scroll-backward key */
			return (CTRL|'Z');
		case KEY_NPAGE:		/* next-page key */
			return (SPEC|'V');
		case KEY_PPAGE:		/* previous-page key */
			return (SPEC|'Z');
//		case KEY_STAB:		/* set-tab key */
//		case KEY_CTAB:		/* clear-tab key */
//		case KEY_CATAB:		/* clear-all-tabs key */
		case KEY_ENTER:		/* enter/send key */
			return (CTRL|'M');
//		case KEY_PRINT:		/* print key */
//		case KEY_LL:		/* lower-left key (home down) */
		case KEY_A1:		/* upper left of keypad */
			return (SPEC|'<');
		case KEY_A3:		/* upper right of keypad */
			return (SPEC|'V');
		case KEY_B2:		/* center of keypad */
			return ' ';
		case KEY_C1:		/* lower left of keypad */
		case KEY_C3:		/* lower right of keypad */
			return (CTRL|'V');
		case KEY_BTAB:		/* back-tab key */
			return (SHFT|CTRL|'I');
		case KEY_BEG:		/* begin key */
			return (META|'<');
		case KEY_CANCEL:	/* cancel key */
			return (CTRL|'G');
		case KEY_CLOSE:		/* close key */
			return (CTLX|CTRL|'C');
		case KEY_COMMAND:	/* command key */
			return (CTLX|'$');
//		case KEY_COPY:		/* copy key */
		case KEY_CREATE:	/* create key */
			return (CTLX|CTRL|'F');
		case KEY_END:		/* end key */
			return (META|'<');
		case KEY_EXIT:		/* exit key */
			return (CTLX|CTRL|'C');
		case KEY_FIND:		/* find key */
			return (CTRL|'S');
		case KEY_HELP:		/* help key */
			return (SPEC|'H');
		case KEY_MARK:		/* mark key */
			return (SPEC|' ');
//		case KEY_MESSAGE:	/* message key */
		case KEY_MOVE:		/* move key */
			return (SPEC|'G');
		case KEY_NEXT:		/* next key */
			return (ALTD|'S');
		case KEY_OPEN:		/* open key */
		case KEY_OPTIONS:	/* options key */
		case KEY_PREVIOUS:	/* previous key */
			return (ALTD|'R');
//		case KEY_REDO:		/* redo key */
//		case KEY_REFERENCE:	/* reference key */
		case KEY_REFRESH:	/* refresh key */
			return (CTRL|'L');
		case KEY_REPLACE:	/* replace key */
			return (SPEC|'R');
//		case KEY_RESTART:	/* restart key */
//		case KEY_RESUME:	/* resume key */
		case KEY_SAVE:		/* save key */
			return (CTLX|CTRL|'W');
		case KEY_SBEG:		/* shifted begin key */
			return (SPEC|'B');
//		case KEY_SCANCEL:	/* shifted cancel key */
		case KEY_SCOMMAND:	/* shifted command key */
			return (CTLX|'#');
//		case KEY_SCOPY:		/* shifted copy key */
//		case KEY_SCREATE:	/* shifted create key */
		case KEY_SDC:		/* shifted delete-character key */
			return (CTRL|'D');
		case KEY_SDL:		/* shifted delete-line key */
			return (CTRL|'K');
		case KEY_SELECT:	/* select key */
			return (SPEC|'W');
		case KEY_SEND:		/* shifted end key */
			return (SPEC|'F');
		case KEY_SEOL:		/* shifted clear-to-end-of-line key */
			return (CTRL|'K');
#if	0
		case KEY_SEXIT:		/* shifted exit key */
		case KEY_SFIND:		/* shifted find key */
		case KEY_SHELP:		/* shifted help key */
		case KEY_SHOME:		/* shifted home key */
		case KEY_SIC:		/* shifted insert-character key */
		case KEY_SLEFT:		/* shifted left-arrow key */
		case KEY_SMESSAGE:	/* shifted message key */
		case KEY_SMOVE:		/* shifted move key */
		case KEY_SNEXT:		/* shifted next key */
		case KEY_SOPTIONS:	/* shifted options key */
		case KEY_SPREVIOUS:	/* shifted previous key */
		case KEY_SPRINT:	/* shifted print key */
		case KEY_SREDO:		/* shifted redo key */
		case KEY_SREPLACE:	/* shifted replace key */
		case KEY_SRIGHT:	/* shifted right-arrow key */
		case KEY_SRSUME:	/* shifted resume key */
		case KEY_SSAVE:		/* shifted save key */
		case KEY_SSUSPEND:	/* shifted suspend key */
		case KEY_SUNDO:		/* shifted undo key */
		case KEY_SUSPEND:	/* suspend key */
#endif
		case KEY_UNDO:		/* undo key */
			return (CTRL|'_');
#if MOUSE
		case KEY_MOUSE:		/* Mouse event has occurred */
			checkmouse();
			return *inbufh++;
#endif
		case KEY_RESIZE:	/* Terminal resize event */
			return (CTRL|'L');
//		case KEY_EVENT:		/* We were interrupted by an event */
		case 0x0209:		/* ^Suppr */
			return (CTRL|'K');
		case 0x02e1:		/* ^Down */
			return (CTLX|CTRL|'N');
		case 0x0214:		/* ^End */
			return (CTRL|'G');
		case 0x0219:		/* ^Home */
			return (MOUS|'x');
		case 0x0223:		/* ^ <- */
			return (SPEC|CTRL|'B');
		case 0x0228:		/* ^Page Down */
			return (SPEC|CTRL|'V');
		case 0x022d:		/* ^Page Up */
			return (SPEC|CTRL|'Z');
		case 0x0232:		/* ^ -> */
			return (SPEC|CTRL|'F');
		case 0x0238:		/* ^Up */
			return (CTLX|CTRL|'P');
	}

	return 0;
}

/** Return cooked characters **/
int ttgetc()
{
	int ch = 0;

	ttflush();
#if	MOUSE
	if (inbufh != inbuft) {
		/* Get mouse input from buffer */
		ch = *inbufh++;

		/* reset us to the beginning of the buffer if there are no more
		   pending characters */
		if (inbufh >= inbuft)
			inbufh = inbuft = inbuf;

		return(ch);
	}
#endif
	/* Return next character */
#if	UTF8
	if (wget_wch(stdscr, (wint_t*) &ch) == KEY_CODE_YES)
#else
	if (((ch = wgetch(stdscr)) & KEY_CODE_YES))
#endif
		ch = extcode(ch);
/* No way to distinguish between ALT-A and ESC A
	else if (ch == 0x1b) {
		wint_t wch;
		update_panels();
		wtimeout(stdscr, 10);

		if (wget_wch(stdscr, (wint_t*) &wch) != ERR)
			ch = ALTD|wch;

		wtimeout(stdscr, -1);
	}
*/
	return(ch);
}

#if TYPEAH
int typahead()
{
	int count;

#if	MOUSE
	/* See if internal buffer is non-empty */
	if (inbufh != inbuft)
		return(1);
#endif
	/* Get number of pending characters */
	if (ioctl(STDIN_FILENO, FIONREAD, &count))
		return(0);

	return(count);
}
#endif /* TYPEAH */

void initmenu(void)
{
	MENUBAR* pbar;
	MENUITEM* pitem;
	int xpos = 0;
	size_t l;
#if REVSTA
	wstandout(wmenubar);
#endif
	for (pbar = menubar, pbar->m_nitems = 0; pbar->m_text; ++pbar) {
		pbar->m_left = xpos;
		pbar->m_right = pbar->m_left + strlen(pbar->m_text);
		waddstr(wmenubar, pbar->m_text);
		waddstr(wmenubar, "  ");
		xpos = pbar->m_right + 2;

		for (pitem = pbar->m_items; pitem->i_text; ++pitem) {
			pitem->i_disabled = 0;
			++pbar->m_nitems;

			if ((l = strlen(pitem->i_text)) > pbar->m_width)
				pbar->m_width = l;
		}

		pbar->m_width += 2;
	}
#if REVSTA
	wclrtoeol(wmenubar);
	wstandend(wmenubar);
	update_panels();
#endif
}

/** Initialize screen package **/
int scopen()
{
	const char* cp;
	/* Override size if environment variables exists */
	cp = getenv("LINES");
	if (cp)
		term.t_nrow = atoi(cp) - 2;

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
				term.t_nrow = w.ws_row - 2;
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
				term.t_nrow = (w.uw_height / w.uw_vs) - 2;
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

	/* Initialize screen */
	initscr();
#if	COLOR
	start_color();
#endif
	cbreak();
	raw();
	noecho();
	nonl();
	intrflush(0, 0);
	keypad(stdscr, 1);
	idlok(stdscr, 1);
	idcok(stdscr, 1);
#if	COLOR
	if (has_colors()) {
		use_default_colors();
		assume_default_colors(COLOR_WHITE, COLOR_BLACK);

		if (COLOR_PAIRS >= 256) {
			for (int fg = 0; fg < 16; ++fg)
				for (int bg = 0; bg < 16; ++bg)
					init_pair(fg * 16 + bg + 1, fg, bg);
		} else if (COLOR_PAIRS >= 64) {
			for (int fg = 0; fg < 8; ++fg)
				for (int bg = 0; bg < 8; ++bg)
					init_pair(fg * 8 + bg + 1, fg, bg);
		}
	}
#endif

	/* Open terminal device */
	if (ttopen()) {
		puts("Cannot open terminal");
		exit(1);
	}

	wmenubar = newwin(1, term.t_mcol, 0, 0);
	scrollok(wmenubar, 0);
	wattrset(wmenubar, COLOR_PAIR(COLOR_BLACK * ((COLORS == 256) ? 16 : 8) + COLOR_CYAN + 9));
	wbkgdset(wmenubar, COLOR_PAIR(COLOR_BLACK * ((COLORS == 256) ? 16 : 8) + COLOR_CYAN + 9));
	wclear(wmenubar);
	initmenu();
	keypad(wmenubar, 1);
	pmenu = new_panel(wmenubar);
	show_panel(pmenu);
	wmain = newwin(term.t_mrow + 1, term.t_mcol, 1, 0);
	scrollok(wmain, 0);
	keypad(wmain, 1);
	pmain = new_panel(wmain);
	show_panel(pmain);
	update_panels();
	/* Success */
	return(0);
}

/** Close screen package **/
int scclose()
{
	/* Turn off curses */
	update_panels();
	del_panel(pmain);
	del_panel(pmenu);
	delwin(wmain);
	delwin(wmenubar);
	endwin();
	/* Success */
	return(0);
}

/* open keyboard -hm */
int sckopen()
{
#if MOUSE
	mmask_t oldmask;
	mousemask(_ALL_MOUSE_EVENTS, &oldmask);
#endif
	return(TRUE);
}

/* close keyboard -hm */
int sckclose()
{
	return(TRUE);
}

/** Move cursor **/
int scmove(row, col)
int row;				/* Row number			*/
int col;				/* Column number		*/
{
	wmove(wmain, row, col);
	update_panels();
	/* Success */
	return(0);
}

/** Erase to end of line **/
int sceeol()
{
#if	COLOR
	wattrset(wmain, COLOR_PAIR(cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor + 1) | attrib);
	wbkgdset(wmain, COLOR_PAIR(cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor + 1) | attrib);
#endif
	wclrtoeol(wmain);
	update_panels();
	/* Success */
	return(0);
}

/** Clear screen **/
int sceeop()
{
#if	COLOR
	wattrset(wmain, COLOR_PAIR(cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor + 1) | attrib);
	wbkgdset(wmain, COLOR_PAIR(cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor + 1) | attrib);
#endif
	werase(wmain);
	update_panels();
	/* Success */
	return(0);
}

/** Set reverse video state **/
int screv(state)
int state;				/* New state			*/
{
	if (state)
		wstandout(wmain);
	else
		wstandend(wmain);

	update_panels();
	/* Success */
	return(0);
}

/** Beep **/
int scbeep()
{
	beep();
	/* Success */
	return(0);
}

#if COLOR

/** Set foreground color (upper case color name)  **/
int scfcol(color)
int color;		/* Color to set			*/
{
	/* Skip if already the correct color */
	color &= (COLORS == 256) ? 0x0f : 0x07;

	if (color == cfcolor)
		return(0);

	if (color & 8)
		attrib |= A_BOLD;
	else
		attrib &= ~A_BOLD;

	cfcolor = color;
	wattrset(wmain, COLOR_PAIR(cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor + 1) | attrib);
	wbkgdset(wmain, COLOR_PAIR(cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor + 1) | attrib);
	update_panels();
	return(0);
}

/** Set background color (lower case color name) **/
int scbcol(color)
int color;			/* Color to set			*/
{
	/* Skip if already the correct color */
	color &= (COLORS == 256) ? 0x0f : 0x07;

	if (color == cbcolor)
		return(0);

	cbcolor = color;
	wattrset(wmain, COLOR_PAIR(cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor + 1) | attrib);
	wbkgdset(wmain, COLOR_PAIR(cfcolor * ((COLORS == 256) ? 16 : 8) + cbcolor + 1) | attrib);
	update_panels();
	return(0);
}
#endif /* COLOR */

/** Set palette **/
int spal(cmd)
char * cmd;			/* Palette command		*/
{
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
	/* Clear window size changed flag */
	winch_flag = 0;
	/* Get new size */
	ioctl(fileno(stdin), TIOCGWINSZ, &win);
	winch_vtresize(win.ws_row, win.ws_col);
	/* Resize curses screen */
	resizeterm(win.ws_row, win.ws_col);
	wresize(wmain, term.t_nrow + 1, term.t_ncol);
	/* Reset display */
	onlywind(0,0);
	TTmove(0,0);
	TTeeop();
	update_panels();
}

#endif	/* HANDLE_WINCH */

static int mdrop = -1;
static int mline = 0;

static void showitem(MENUITEM* mitem, int ypos, int selected)
{
	int disabled = (mitem->i_checkdisabled && mitem->i_checkdisabled());

	if (selected)
		wstandout(wdrop);

	if (disabled)
		wattron(wdrop, A_BOLD);
	else
		wattron(wdrop, A_DIM);

	wmove(wdrop, ypos, 1);
	waddstr(wdrop, mitem->i_text);

	if (mitem->i_markfunc && mitem->i_markfunc()) {
		wmove(wdrop, ypos, 1);
		waddch(wdrop, '*');
	}

	if (selected)
		wstandend(wdrop);

	if (disabled)
		wattroff(wdrop, A_BOLD);
	else
		wattroff(wdrop, A_DIM);
}

void closedrop(void)
{
	if (wdrop != NULL) {
		del_panel(pdrop);
		delwin(wdrop);
		wdrop = NULL;

		for (MENUBAR* pbar = menubar; pbar->m_text; ++pbar) {
			wmove(wmenubar, 0, pbar->m_left);
			waddstr(wmenubar, pbar->m_text);
		}
	}
}

int handlemenu(WINDOW* wmouse, int ypos, int xpos)
{
	MENUBAR* pbar;
	int status = TRUE;

	if (wmouse == wmenubar) {
		closedrop();

		for (pbar = menubar; pbar->m_text; ++pbar) {
			if (xpos >= pbar->m_left && xpos <= pbar->m_right) {
				if (mdrop == pbar - menubar)
					return handlemenu(wmain, 0, 0);

				curs_set(0);
#if REVSTA
				wstandend(wmenubar);
#else
				wstandout(wmenubar);
#endif
				wmove(wmenubar, 0, pbar->m_left);
				waddstr(wmenubar, pbar->m_text);
#if REVSTA
				wstandout(wmenubar);
#else
				wstandend(wmenubar);
#endif
				dropmenu = pbar->m_items;
				wdrop = newwin(pbar->m_nitems, pbar->m_width, 1, pbar->m_left);
				pdrop = new_panel(wdrop);
				scrollok(wdrop, false);
				wattrset(wdrop, COLOR_PAIR(COLOR_BLACK * ((COLORS == 256) ? 16 : 8) + COLOR_CYAN + 9));
				wbkgdset(wdrop, COLOR_PAIR(COLOR_BLACK * ((COLORS == 256) ? 16 : 8) + COLOR_CYAN + 9));

				MENUITEM* mitem;
				int ypos = 0;
				mdrop = pbar - menubar;
				mline = 0;

				for (mitem = pbar->m_items; mitem->i_text; ++mitem, ++mline) {
					if (*mitem->i_text == '-')
						mvwhline(wdrop, ypos, 1, 0, pbar->m_width - 2);
					else if (*mitem->i_text != '+')
						showitem(mitem, ypos, mline == 1);

					++ypos;
				}

				box(wdrop, 0, 0);
				mline = 1;
				break;
			}
		}

		update_panels();
	} else if (wmouse == wdrop) {
		int savecl = clexec;

		clexec = FALSE;
		closedrop();
		mdrop = -1;
		curs_set(1);
		update_panels();

		if (dropmenu[ypos].i_type == BINDFNC)
			status = (*(dropmenu[ypos].i_ptr.fp))(0, 0);
		else
			status = dobuf(dropmenu[ypos].i_ptr.buf);

		clexec = savecl;
	} else if (wmouse == wmain) {
		closedrop();
		mdrop = -1;
		curs_set(2);
		update_panels();
	}

	return status;
}

int menudrop(int f, int n)
{
	return handlemenu(wmenubar, 0, 0);
}

void menukey(int c)
{
	char begin;
	int nline = mline;

	switch (c) {
		case CTRL|'N':
		 	begin = *dropmenu[mline + 1].i_text;

			if (begin == '-')
				nline += 2;
			else if (begin != '+')
				++nline;

			if (mline != nline) {
				showitem(&dropmenu[mline], mline, FALSE);
				mline = nline;
				showitem(&dropmenu[mline], mline, TRUE);
			}

			break;
		case CTRL|'P':
			begin = *dropmenu[mline - 1].i_text;

			if (begin == '-')
				nline -= 2;
			else if (begin != '+')
				nline--;

			if (mline != nline) {
				showitem(&dropmenu[mline], mline, FALSE);
				mline = nline;
				showitem(&dropmenu[mline], mline, TRUE);
			}

			break;
		case CTRL|'G':
			closedrop();
			break;
		case CTRL|'M':
			handlemenu(wdrop, mline, 1);
			break;
		case CTRL|'F':
			if (menubar[mdrop + 1].m_items)
				handlemenu(wmenubar, 0, menubar[mdrop + 1].m_left);
			break;
		case CTRL|'B':
			if (mdrop > 0)
				handlemenu(wmenubar, 0, menubar[mdrop - 1].m_left);
			break;
		case CTRL|'[':
			handlemenu(wmain, 0, 0);
			break;
	}
}

#define MB_FNC  0
#define MB_BUF  1

/* AddMenuBinding:  bind a menu item to an EPOINTER */
/* ==============                                   */
/* called by bindtomenu and macrotomenu, once the function or macro name
   has been parsed */


static int  AddMenuBinding(ETYPE EPOINTER eptr, int type)
{
	char    menutext[NSTRING];
	char    *itemtext;
	MENUBAR*	mbar;
	MENUITEM*	mitem;

	/* Parse the menu text */
	itemtext = &menutext[0];

	if (mlreply(TEXT306, itemtext, NSTRING) != TRUE)	/* "Menu: " */
		return FALSE;

	if ((itemtext = strchr(menutext, '>'))) {
		*itemtext++ = '\0';

		for (mbar = menubar; mbar->m_text; ++mbar) {
			if (! strcmp(menutext, mbar->m_text)) {
				for (mitem = mbar->m_items; mitem->i_text; ++mitem) {
					if (! strncmp(itemtext, mitem->i_text + 1, strlen(itemtext))) {
						mitem->i_ptr = eptr;
						mitem->i_type = (type == MB_BUF) ? BINDBUF : BINDFNC;
						return (TRUE);
					}
				}
				break;
			}
		}
	}

	mlwrite(TEXT300);  /* "[Incorrect menu]" */
	return(FALSE);
}

/* Bind a function to a menu item
 *
 * Syntax : bind-to-menu function "Menu>Item"
 */

int bindtomenu(int n, int f)
{
	ETYPE EPOINTER  e;

	e.fp = getname(TEXT304);   /* "Function: " */

	if (e.fp == NULL) {
		mlwrite (TEXT16);       /* "[No such function]" */
		return FALSE;
	}

	return AddMenuBinding(e, MB_FNC);
}

/* Bind a macro buffer to a menu item
 *
 * Syntax : macro-to-menu buffer "Menu>Item"
 */

int macrotomenu(int n, int f)
{
	ETYPE EPOINTER  e;
	char		bname[NBUFN];

	if (mlreply(TEXT305, &bname[1], NBUFN-2) != TRUE)	/* "Macro: " */
		return(FALSE);

	bname[0] = '[';
	strcat(bname, "]");

	if ((e.buf = bfind(bname, FALSE, 0)) == NULL) {
		mlwrite (TEXT130);
		/* "Macro not defined" */
		return(FALSE);
	}

	return AddMenuBinding(e, MB_BUF);
}

/* unbindmenu:  remove a menu entry */
/* ==========                       */

int unbindmenu(int f, int n)
{
	char    menutext[NSTRING];
	char    *itemtext;
	MENUBAR*	mbar;
	MENUITEM*	mitem;

	/* Parse the menu text */
	itemtext = &menutext[0];

	if (mlreply(TEXT306, itemtext, NSTRING) != TRUE)	/* "Menu: " */
		return(FALSE);

	if ((itemtext = strchr(menutext, '>'))) {
		*itemtext++ = '\0';

		for (mbar = menubar; mbar->m_text; ++mbar) {
			if (! strcmp(menutext, mbar->m_text)) {
				for (mitem = mbar->m_items; mitem->i_text; ++mitem) {
					if (! strncmp(itemtext, mitem->i_text + 1, strlen(itemtext))) {
						mitem->i_ptr.fp = nullproc;
						mitem->i_type = BINDFNC;
						return(TRUE);
					}
				}
				break;
			}
		}
	}

	mlwrite(TEXT300);  /* "[Incorrect menu]" */
	return(FALSE);
}

#endif	/* CURSES */


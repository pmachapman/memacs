/*      XPCONIO.C       Operating specific video & keyboard functions
 *                      for the Window XP operating system (console mode)
 *                      for MicroEMACS 5.00
 *                      (C)Copyright 2008 by Daniel M. Lawrence
 *
 *                      Unicode support by Jean-Michel Dubois
 *
 * The routines in this file provide video and keyboard support using the
 * Windows XP/Visual Studio 2008 console functions.
 *
 */

#define termdef 1                       /* don't define "term" external */

#undef  PASCAL
#undef  NEAR
#undef  HIBYTE

#include <windows.h>
#include <stdio.h>

#include        "estruct.h"
#include        "eproto.h"
#include        "edef.h"
#include        "elang.h"

#if     XPCON
#define NROW    256              /* Max Screen size.         */
#define NCOL    256             /* Edit if you want to.         */
#define MARGIN  8               /* size of minimim margin and   */
#define SCRSIZ  64              /* scroll size for extended lines */
#define NPAUSE  5               /* # times thru update to pause */

/* Forward references.          */

PASCAL NEAR ntmove();
PASCAL NEAR nteeol();
PASCAL NEAR nteeop();
PASCAL NEAR ntbeep();
PASCAL NEAR ntopen();
PASCAL NEAR ntclose();
PASCAL NEAR ntgetc();
PASCAL NEAR ntputc();
PASCAL NEAR ntflush();
PASCAL NEAR ntrev();
PASCAL NEAR ntkclose();
PASCAL NEAR ntkopen();
PASCAL NEAR ntcres();
PASCAL NEAR ntparm();
#if     COLOR
PASCAL NEAR ntfcol();
PASCAL NEAR ntbcol();
#endif
PASCAL NEAR fnclabel();
static WORD NEAR ntAttribute(void);

/* Screen buffer to write to. */
static CHAR_INFO ciScreenBuffer[NROW * NCOL];
static int      cfcolor = 0;    /* current foreground color     */
static int      cbcolor = 15;   /* current background color     */
static int      ctrans[] =      /* ansi to ibm color translation table  */
        { 0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15 };

static WORD ntrow = 0;         /* current cursor row   */
static WORD ntcol = 0;         /* current cursor col   */
static WORD ntMax = 0;
static WORD ntMin = (WORD)-1;
static WORD ntColMax = 0;
static WORD ntColMin = (WORD)-1;

static int lastwrow;		/* last window's row size */
static int lastwcol;

int revflag = FALSE;                    /* are we currently in rev video? */

/* Windows NT Console screen buffer handle.  All I/O is done through this
 * handle.
 */
static HANDLE hInput, hOutput;
static char chConsoleTitle[256];    // Preserve the title of the console.
static long ConsoleMode, OldConsoleMode;

static INPUT_RECORD ir;
static WORD wKeyEvent;

/*
 * Standard terminal interface dispatch table.
 */
TERM    term    = {
        NROW-1,
        NROW-1,
        NCOL,
        NCOL,
        0, 0,
        MARGIN,
        SCRSIZ,
        NPAUSE,
        ntopen,
        ntclose,
        ntkopen,
        ntkclose,
        ntgetc,
        ntputc,
        ntflush,
        ntmove,
        nteeol,
        nteeop,
        nteeop,
        ntbeep,
        ntrev,
        ntcres
#if     COLOR
        , ntfcol,
        ntbcol
#endif
};

/*	Mousing global variable	*/
static int mexist;	/* is the mouse driver installed? */
static int nbuttons;	/* number of buttons on the mouse */
static int oldbut;	/* Previous state of mouse buttons */
static int oldcol;	/* previous x position of mouse */
static int oldrow;	/* previous y position of mouse */

/*	input buffers and pointers	*/

#define	IBUFSIZE	64	/* this must be a power of 2 */
#if	UTF8
unsigned int in_buf[IBUFSIZE];	/* input character buffer */
#else
unsigned char in_buf[IBUFSIZE];	/* input character buffer */
#endif
int in_next = 0;		/* pos to retrieve next input character */
int in_last = 0;		/* pos to place most recent input character */

void in_init()	/* initialize the input buffer */

{
	in_next = in_last = 0;
}

int in_check()	/* is the input buffer non-empty? */

{
	if (in_next == in_last)
		return(FALSE);
	else
		return(TRUE);
}

void in_put(event)

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

#if COLOR
/*----------------------------------------------------------------------*/
/*	ntfcol()							*/
/* Set the current foreground color.					*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntfcol(
	int color)			/* color to set */
{
	cfcolor = ctrans[color];
}

/*----------------------------------------------------------------------*/
/*	ntbcol()							*/
/* Set the current background color.					*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntbcol(
	int color)		/* color to set */
{
	cbcolor = ctrans[color];
}
#endif

static void near ntSetUpdateValues(void)
{
	if (ntrow < ntMin)
		ntMin = ntrow;
	if (ntrow > ntMax)
		ntMax = ntrow;
	if (ntMax == ntMin) {
		if (ntcol < ntColMin)
			ntColMin = ntcol;
		if (ntcol > ntColMax)
			ntColMax = ntcol;
	}
}

/*----------------------------------------------------------------------*/
/*	ntmove()							*/
/* Move the cursor. 						*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntmove(
	int row,
	int col)
{
	COORD dwCursorPosition;

	ntcol = dwCursorPosition.X = col;
	ntrow = dwCursorPosition.Y = row;
	SetConsoleCursorPosition(hOutput, dwCursorPosition);
}


/*----------------------------------------------------------------------*/
/*	ntflush()							*/
/* Update the physical video buffer from the logical video buffer.	*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntflush(void)
{
	SMALL_RECT srWriteRegion;
	COORD coordUpdateBegin, coordBufferSize;

	if (ntMin <= ntMax) {

		if (ntMin == ntMax) {

			/* Fri Feb 14 1992 WaltW - Same line bud. */
			srWriteRegion.Right = term.t_ncol - 1 /*ntColMax*/;
			srWriteRegion.Left = 0; //ntColMin;

			coordUpdateBegin.X = 0; //ntColMin;
			coordUpdateBegin.Y = ntMin;
		} else {
			srWriteRegion.Right = term.t_ncol - 1;
			srWriteRegion.Left = 0;

			coordUpdateBegin.X = 0;
			coordUpdateBegin.Y = ntMin;
		}

		srWriteRegion.Bottom = ntMax;
		srWriteRegion.Top = ntMin;

		coordBufferSize.X = term.t_ncol;
		coordBufferSize.Y = term.t_nrow + 1;

		WriteConsoleOutput(hOutput, (PCHAR_INFO)ciScreenBuffer,
			coordBufferSize, coordUpdateBegin, &srWriteRegion);
		ntColMax = ntMax = 0;
		ntColMin = ntMin = (WORD) -1;
	}
	ntmove(ttrow, ttcol);
	return(TRUE);
}

static void near MouseEvent(void)

{
	register int k;		/* current bit/button of mouse */
	register int event;	/* encoded mouse event */
	register int etype;	/* event type byte */
	MOUSE_EVENT_RECORD *m_event;	/* mouse event to decode */
	int mousecol;		/* current mouse column */
	int mouserow;		/* current mouse row */
	int sstate;		/* current shift key status */
	int newbut;		/* new state of the mouse buttons */

	m_event = &(ir.Event.MouseEvent);

	/* check to see if any mouse buttons are different */
	newbut = m_event->dwButtonState;
	mousecol = m_event->dwMousePosition.X;
	mouserow = m_event->dwMousePosition.Y;

	/* only notice changes */
	if ((oldbut == newbut) && (mousecol == oldcol)
	    && (mouserow == oldrow))
		return(FALSE);

#if	0
	printf("<%d,%d> B%d C%d F%d O%d N%d\n", m_event->dwMousePosition.Y,
		m_event->dwMousePosition.Y, m_event->dwButtonState,
		m_event->dwControlKeyState, m_event->dwEventFlags,
		oldbut, newbut);
#endif

	/* get the shift key status as well */
	etype = MOUS >> SHIFTPFX;
	sstate = m_event->dwControlKeyState;
	if (sstate & SHIFT_PRESSED)		/* shifted? */
		etype |= (SHFT >> SHIFTPFX);
	if ((sstate & RIGHT_CTRL_PRESSED) ||
	    (sstate & LEFT_CTRL_PRESSED))	/* controled? */
		etype |= (CTRL >> SHIFTPFX);

	/* no buttons changes */
	if (oldbut == newbut) {

		/* generate a mouse movement */
		if (((mouse_move == 1) && (mmove_flag == TRUE)) ||
		    (mouse_move == 2)) {
			in_put(0);
			in_put(etype);
			in_put(mousecol);
			in_put(mouserow);
			in_put('m');
		}
		oldcol = mousecol;
		oldrow = mouserow;
		return(TRUE);
	}

	for (k=1; k != (1 << nbuttons); k = k<<1) {

		/* For each button on the mouse */
		if ((oldbut&k) != (newbut&k)) {
			/* This button changed, generate an event */
			in_put(0);
			in_put(etype);
			in_put(mousecol);
			in_put(mouserow);

			event = ((newbut&k) ? 0 : 1);	/* up or down? */
			if (k == 2)			/* center button? */
				event += 4;
			if (k == 4)			/* right button? */
				event += 2;
			event += 'a';
			in_put(event);
			oldbut = newbut;
			oldcol = mousecol;
			oldrow = mouserow;
			return(TRUE);
		}
	}
	return(FALSE);
}

static void near WindowSizeEvent(void)
{
	CONSOLE_SCREEN_BUFFER_INFO Console;

	GetConsoleScreenBufferInfo(hOutput, &Console);

	in_put(0);
	in_put(MOUS >> SHIFTPFX);
	in_put(Console.srWindow.Right+1);
	in_put(Console.srWindow.Bottom);
	in_put('2');

	/* remeber this size */
	lastwrow = Console.srWindow.Bottom;
	lastwcol = Console.srWindow.Right+1;
}

/* handle the current keyboard event */

static void near KeyboardEvent()

{
	int c;		/* ascii character to examine */
	int vscan;	/* virtual scan code */
	int prefix;	/* character prefix */
	int state;	/* control key state from console device */

	/* ignore key up events */
	if (ir.Event.KeyEvent.bKeyDown == FALSE)
		return(FALSE);

	/* If this is an extended character, process it */
#if	UTF8
	c = ir.Event.KeyEvent.uChar.UnicodeChar;
#else
	c = ir.Event.KeyEvent.uChar.AsciiChar;
#endif
	state = ir.Event.KeyEvent.dwControlKeyState;
	prefix = 0;

	if (c == 0) {

		/* grab the virtual scan code */
		vscan = ir.Event.KeyEvent.wVirtualScanCode;

		/* function keys are special! */
		if (vscan > 58 && vscan < 68) {
			c = '1' + vscan - 59;
			prefix = SPEC;
			goto pastothers;
		}

		/* interpret code by keyscan */
		switch (vscan) {

			/* ignore these key down events */
			case 29:	/* control */
			case 42:	/* left shift */
			case 54:	/* left shift */
			case 56:	/* ALT key */
				return;

			case 68:	/* F10 */
				prefix = SPEC; c = '0'; break;

			case 69:	/* PAUSE */
				prefix = SPEC; c = ':'; break;

			case 70:	/* SCROLL LOCK */
				return;

			case 71:	/* HOME */
				prefix = SPEC; c = '<'; break;

			case 72:	/* Cursor Up */
				prefix = SPEC; c = 'P'; break;

			case 73:	/* Page Up */
				prefix = SPEC; c = 'Z'; break;

			case 75:	/* Cursor left */
				prefix = SPEC; c = 'B'; break;

			case 76:	/* keypad 5 */
				prefix = SPEC; c = 'L'; break;

			case 77:	/* Cursor Right */
				prefix = SPEC; c = 'F'; break;

			case 79:	/* END */
				prefix = SPEC; c = '>'; break;

			case 80:	/* Cursor Down */
				prefix = SPEC; c = 'N'; break;

			case 81:	/* Page Down */
				prefix = SPEC; c = 'V'; break;

			case 82:	/* insert key */
				prefix = SPEC; c = 'C'; break;

			case 83:	/* delete key */
				prefix = SPEC; c = 'D'; break;

			case 87:	/* F11 */
				prefix = SPEC; c = '-'; break;

			case 88:	/* F12 */
				prefix = SPEC; c = '='; break;

			default:
#if	1
				/* tell us about a key we do net yet map! */
				printf("<%d:%d/%d> ", ir.EventType,
					ir.Event.KeyEvent.uChar.AsciiChar,
					  ir.Event.KeyEvent.wVirtualScanCode);
#endif
				return;
		}

pastothers:	/* shifted special key? */
		if (state & SHIFT_PRESSED)
			prefix |= SHFT;
	}

	/* decode the various modifiers to the character */
	if (state & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED)) {
		prefix |= ALTD;
		if (islower(c))
			c = c - 'a' + 'A';
	}
	if ((state & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) && c > 31)
		prefix |= CTRL;

	/* if there is a prefix, insert it in the input stream */
	if (prefix != 0) {
		in_put(0);
		in_put(prefix >> SHIFTPFX);
	}

	/* place the ascii character in the input queue */
	in_put(c);
	return;

}

int PendingScreenResize()

{
	CONSOLE_SCREEN_BUFFER_INFO Console;

	GetConsoleScreenBufferInfo(hOutput, &Console);

	if ((lastwrow != Console.srWindow.Bottom) || (lastwcol != Console.srWindow.Right + 1)) {
		return(TRUE);
	} else {
		return(FALSE);
	}
}
/*----------------------------------------------------------------------*/
/*	ntgetc()							*/
/* Get a character from the keyboard.					*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntgetc()
{

	long dw;

ttc:	ntflush();

	/* return any keystrokes waiting in the
	   type ahead buffer */
	if (in_check())
		return(in_get());

	/* wait for input for a short time */
	while (WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), 10) == WAIT_TIMEOUT) {

		/* check to see if the console window has been resized */
		if (PendingScreenResize()) {
			WindowSizeEvent();
			goto ttc;
		}
	}

	/* get the next keyboard/mouse/resize event */
	ReadConsoleInput(hInput, &ir, 1, &dw);

	/* let the proper event handler field this event */
	switch (ir.EventType) {

		case KEY_EVENT:
			KeyboardEvent();
			goto ttc;

		case MOUSE_EVENT:
			MouseEvent();
			goto ttc;

		case WINDOW_BUFFER_SIZE_EVENT:
			/* resize max windows sizes here.... */
			goto ttc;

		case FOCUS_EVENT:			/* ignore this one */
			goto ttc;
	}

	/* we should never arrive here, ignore this event */
	goto ttc;
}

#if TYPEAH
/*----------------------------------------------------------------------*/
/*	typahead()							*/
/* Returns true if a key has been pressed.				*/
/*----------------------------------------------------------------------*/

PASCAL NEAR typahead()

{
	/* anything waiting in the input queue? */
	if (in_check())
		return(TRUE);

	return(FALSE);
}
#endif

static WORD near ntAttribute(void)
{
	return(revflag ? (cbcolor | (cfcolor << 4)) : ((cbcolor << 4) | cfcolor));
}

/*----------------------------------------------------------------------*/
/*	ntputc()							*/
/* Put a character at the current position in the current colors.	*/
/* Note that this does not behave the same as putc() or VioWrtTTy().	*/
/* This routine does nothing with returns and linefeeds.  For backspace */
/* it puts a space in the previous column and moves the cursor to the	*/
/* previous column.  For all other characters, it will display the	*/
/* graphic representation of the character and put the cursor in the	*/
/* next column (even if that is off the screen.  In practice this isn't */
/* a problem.								*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntputc(int c)
{
	WORD wScreenPos;

	/* only characters on screen!!! */
	if (ntcol < 0 || ntcol > 32000)
			return;

	if (c == '\n' || c == '\r') { 		/* returns and linefeeds */
		ntrow++;
		ntcol = 0;
		return;
	}

	if (c == '\b') {			/* backspace */
		--ntcol;
		ntputc(' ');
		--ntcol;
		return;
	}

	wScreenPos = (ntrow * term.t_ncol) + ntcol++;
#if	UTF8
	ciScreenBuffer[wScreenPos].Char.UnicodeChar = c;
#else
	ciScreenBuffer[wScreenPos].Char.AsciiChar = c;
#endif
	ciScreenBuffer[wScreenPos].Attributes = ntAttribute();
	ntSetUpdateValues();
}


/*----------------------------------------------------------------------*/
/*	nteeol()							*/
/* Erase to end of line.						*/
/*----------------------------------------------------------------------*/

PASCAL NEAR nteeol()
{
	WORD wNum;
	WORD wScreenPos;
	WORD wAttribute;

	/* only characters on screen!!! */
	if (ntcol < 0 || ntcol > 32000)
			return;

	wNum = term.t_ncol - ntcol;
	wScreenPos = ntrow * term.t_ncol + ntcol;
	wAttribute = ntAttribute();
	for (; wNum; wNum--) {
#if	UTF8
		ciScreenBuffer[wScreenPos].Char.UnicodeChar = ' ';
#else
		ciScreenBuffer[wScreenPos].Char.AsciiChar = ' ';
#endif
		ciScreenBuffer[wScreenPos].Attributes = wAttribute;
		wScreenPos++, ntcol++;
	}
	ntSetUpdateValues();
}


/*----------------------------------------------------------------------*/
/*	nteeop()							*/
/* Erase to end of page.						*/
/*----------------------------------------------------------------------*/

PASCAL NEAR nteeop()
{
	WORD wNum;
	WORD wScreenPos;
	WORD wAttribute;

#if	COLOR
	ntfcol(gfcolor);
	ntbcol(gbcolor);
#endif

	wNum = (term.t_ncol - ntcol) + ((term.t_ncol * term.t_nrow) - ntrow);
	wScreenPos = ntrow * term.t_ncol + ntcol;
	wAttribute = ntAttribute();
	for (; wNum; wNum--) {
#if	UTF8
		ciScreenBuffer[wScreenPos].Char.UnicodeChar = ' ';
#else
		ciScreenBuffer[wScreenPos].Char.AsciiChar = ' ';
#endif
		ciScreenBuffer[wScreenPos].Attributes = wAttribute;
		wScreenPos++, ntcol++;
	}
	ntSetUpdateValues();
}

/*----------------------------------------------------------------------*/
/*	ntrev() 						*/
/* Change reverse video state.						*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntrev(state)

int state;	/* TRUE = reverse, FALSE = normal */

{
	revflag = state;
}

/*----------------------------------------------------------------------*/
/*	ntcres()							*/
/* Change the screen resolution.					*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntcres(char *res)		/* name of desired video mode	*/
{
	return TRUE;
}


/*----------------------------------------------------------------------*/
/*	spal()								*/
/* Change pallette settings.  (Does nothing.)				*/
/*----------------------------------------------------------------------*/

PASCAL NEAR spal(char *dummy)
{
	return(TRUE);
}


/*----------------------------------------------------------------------*/
/*	ntbeep()							*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntbeep()
{
	Beep(750, 300);
	return(TRUE);
}

/*----------------------------------------------------------------------*/
/*	ntopen()							*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntopen()
{
	CONSOLE_SCREEN_BUFFER_INFO Console;

	/* initialize the input queue */
	in_init();
	strcpy(os, "WINXP");

	/* This will allocate a console if started from
	 * the windows NT program manager. */
	AllocConsole();

	/* Save the titlebar of the window so we can
	 * restore it when we leave. */
	GetConsoleTitleA(&chConsoleTitle, sizeof(chConsoleTitle));

	/* Set Window Title to MicroEMACS */
	SetConsoleTitleA(PROGNAME);

	/* Get our standard handles */
	hInput = GetStdHandle(STD_INPUT_HANDLE);
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	/* get a ptr to the output screen buffer */
	GetConsoleScreenBufferInfo(hOutput, &Console);
	SetConsoleMode(hInput, ENABLE_WINDOW_INPUT);

	/* let MicroEMACS know our starting screen size */
	term.t_nrow = Console.srWindow.Bottom;
	term.t_ncol = Console.srWindow.Right + 1;
	term.t_mrow = NROW - 1;
	term.t_mcol = NCOL;
	if (term.t_nrow > term.t_mrow)
		term.t_nrow = term.t_mrow;
	if (term.t_ncol > term.t_mcol)
		term.t_ncol = term.t_mcol;

	/* remeber this size */
	lastwrow = term.t_nrow;
	lastwcol = term.t_ncol;

	ntColMin = ntMin = (WORD)-1;
	ntColMax = ntMax = 0;

	/* we always have a mouse under NT */
	mexist = GetNumberOfConsoleMouseButtons(&nbuttons);
	oldcol = -1;
	oldrow = -1;
	oldbut = 0;

	/* initialize some attributes about the output screen */
	revexist = TRUE;
	revflag = FALSE;
	eolexist = TRUE;
/*	gfcolor = 15;
	gbcolor = 0;*/
	cfcolor = 7;
	cbcolor = 0;

	SetConsoleOutputCP(437);	/* this does not seem to give me the old IBMPC character set.... */

	return(TRUE);
}

/*----------------------------------------------------------------------*/
/*	ntclose()							*/
/* Restore the original video settings. 				*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntclose()
{
	/* reset the title on the window */
	SetConsoleTitleA(chConsoleTitle);

	FreeConsole();
	return(TRUE);
}

/*----------------------------------------------------------------------*/
/*	ntkopen()							*/
/* Open the keyboard.							*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntkopen()
{
	/* save the original console mode to restore on exit */
	GetConsoleMode(hInput, &OldConsoleMode);

	/* and reset this to what MicroEMACS needs */
	ConsoleMode = OldConsoleMode;
	ConsoleMode &= ~(ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT |
		ENABLE_ECHO_INPUT | ENABLE_WINDOW_INPUT);
	ConsoleMode |= ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	SetConsoleMode(hInput, ConsoleMode);

	return(TRUE);
}


/*----------------------------------------------------------------------*/
/*	ntkclose()							*/
/* Close the keyboard.							*/
/*----------------------------------------------------------------------*/

PASCAL NEAR ntkclose()
{
	/* restore the console mode from entry */
	SetConsoleMode(hInput, OldConsoleMode);
	return(TRUE);
}

#if FLABEL
PASCAL NEAR fnclabel(f, n)	/* label a function key */

int f,n;	/* default flag, numeric argument [unused] */

{
	/* on machines with no function keys...don't bother */
	return(TRUE);
}
#endif
#endif

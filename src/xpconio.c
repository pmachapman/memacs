/*      XPCONIO.C       Operating specific video & keyboard functions
 *                      for the Window XP operating system (console mode)
 *                      for MicroEMACS 5.00
 *                      (C)Copyright 2008 by Daniel M. Lawrence
 *
 * The routines in this file provide video and keyboard support using the
 * Windows XP/Visual Studio 2008 console functions.
 *
 */

#define termdef 1                       /* don't define "term" external */

#undef  PASCAL
#undef  NEAR
#undef  HIBYTE

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

int PASCAL NEAR ntmove(int row, int col);
int PASCAL NEAR nteeol(void);
int PASCAL NEAR nteeop(void);
int PASCAL NEAR ntbeep(void);
int PASCAL NEAR ntopen(void);
int PASCAL NEAR ntclose(void);
int PASCAL NEAR ntgetc(void);
int PASCAL NEAR ntputc(int c);
int PASCAL NEAR ntflush(void);
int PASCAL NEAR ntrev(int state);
int PASCAL NEAR ntkclose(void);
int PASCAL NEAR ntkopen(void);
int PASCAL NEAR ntcres(char *res);
int PASCAL NEAR ntparm();
#if     COLOR
int PASCAL NEAR ntfcol(int color);
int PASCAL NEAR ntbcol(int color);
#endif
int PASCAL NEAR fnclabel();
static WORD near ntAttribute(void);

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
static DWORD ConsoleMode, OldConsoleMode;
CONSOLE_SCREEN_BUFFER_INFOEX OldConsoleInfo = { 0 };

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
static DWORD nbuttons;	/* number of buttons on the mouse */
static int oldbut;	/* Previous state of mouse buttons */
static int oldcol;	/* previous x position of mouse */
static int oldrow;	/* previous y position of mouse */

/*	input buffers and pointers	*/

#define	IBUFSIZE	64	/* this must be a power of 2 */

unsigned char in_buf[IBUFSIZE];	/* input character buffer */
static int in_next = 0;		/* pos to retrieve next input character */
static int in_last = 0;		/* pos to place most recent input character */

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

void in_put(int event)
{
	in_buf[in_last++] = (unsigned char)event;
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

int PASCAL NEAR ntfcol(
	int color)			/* color to set */
{
	cfcolor = ctrans[color];
	return 0;
}

/*----------------------------------------------------------------------*/
/*	ntbcol()							*/
/* Set the current background color.					*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR ntbcol(
	int color)		/* color to set */
{
	cbcolor = ctrans[color];
	return 0;
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

int PASCAL NEAR ntmove(
	int row,
	int col)
{
	COORD dwCursorPosition;

	ntcol = dwCursorPosition.X = (SHORT)col;
	ntrow = dwCursorPosition.Y = (SHORT)row;
	SetConsoleCursorPosition(hOutput, dwCursorPosition);
	return 0;
}


/*----------------------------------------------------------------------*/
/*	ntflush()							*/
/* Update the physical video buffer from the logical video buffer.	*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR ntflush(void)
{
	SMALL_RECT srWriteRegion;
	COORD coordUpdateBegin, coordBufferSize;

	if (ntMin <= ntMax) {


		srWriteRegion.Right = term.t_ncol - 1;
		srWriteRegion.Left = 0;

		coordUpdateBegin.X = 0;
		coordUpdateBegin.Y = ntMin;

#if 0
		if (ntMin == ntMax) {

			/* Fri Feb 14 1992 WaltW - Same fuckin' line bud. */
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
#endif // 0

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

static int near MouseEvent(INPUT_RECORD *pIr)

{
	MOUSE_EVENT_RECORD *m_event;	/* mouse event to decode */
	register int k;		/* current bit/button of mouse */
	register int event;	/* encoded mouse event */
	register int etype;	/* event type byte */
	int mousecol;		/* current mouse column */
	int mouserow;		/* current mouse row */
	int sstate;		/* current shift key status */
	int newbut;		/* new state of the mouse buttons */

	m_event = &(pIr->Event.MouseEvent);

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
	etype = MOUS >> 8;
	sstate = m_event->dwControlKeyState;
	if (sstate & SHIFT_PRESSED)		/* shifted? */
		etype |= (SHFT >> 8);
	if ((sstate & RIGHT_CTRL_PRESSED) ||
	    (sstate & LEFT_CTRL_PRESSED))	/* controled? */
		etype |= (CTRL >> 8);

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

static void near WindowSizeEvent()
{
	CONSOLE_SCREEN_BUFFER_INFO Console;

	GetConsoleScreenBufferInfo(hOutput, &Console);

	in_put(0);
	in_put(MOUS >> 8);
	in_put(Console.srWindow.Right+1);
	in_put(Console.srWindow.Bottom);
	in_put('2');

	/* remeber this size */
	lastwrow = Console.srWindow.Bottom;
	lastwcol = Console.srWindow.Right+1;
}

/* handle the current keyboard event */

static void near KeyboardEvent(INPUT_RECORD *pIr)

{
	int c;		/* ascii character to examine */
	int vscan;	/* virtual scan code */
	int lprefix;	/* character lprefix */
	int state;	/* control key state from console device */

	/* ignore key up events */
	if (pIr->Event.KeyEvent.bKeyDown == FALSE)
		return;

	/* If this is an extended character, process it */
	c = pIr->Event.KeyEvent.uChar.AsciiChar;
	state = pIr->Event.KeyEvent.dwControlKeyState;
	lprefix = 0;

	if (c == 0) {

		/* grab the virtual scan code */
		vscan = pIr->Event.KeyEvent.wVirtualScanCode;

		/* function keys are special! */
		if (vscan > 58 && vscan < 68) {
			c = '1' + vscan - 59;
			lprefix = SPEC;
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
				lprefix = SPEC; c = '0'; break;

			case 69:	/* PAUSE */
				lprefix = SPEC; c = ':'; break;

			case 70:	/* SCROLL LOCK */
				return;

			case 71:	/* HOME */
				lprefix = SPEC; c = '<'; break;

			case 72:	/* Cursor Up */
				lprefix = SPEC; c = 'P'; break;

			case 73:	/* Page Up */
				lprefix = SPEC; c = 'Z'; break;

			case 75:	/* Cursor left */
				lprefix = SPEC; c = 'B'; break;

			case 76:	/* keypad 5 */
				lprefix = SPEC; c = 'L'; break;

			case 77:	/* Cursor Right */
				lprefix = SPEC; c = 'F'; break;

			case 79:	/* END */
				lprefix = SPEC; c = '>'; break;

			case 80:	/* Cursor Down */
				lprefix = SPEC; c = 'N'; break;

			case 81:	/* Page Down */
				lprefix = SPEC; c = 'V'; break;

			case 82:	/* insert key */
				lprefix = SPEC; c = 'C'; break;

			case 83:	/* delete key */
				lprefix = SPEC; c = 'D'; break;

			case 87:	/* F11 */
				lprefix = SPEC; c = '-'; break;

			case 88:	/* F12 */
				lprefix = SPEC; c = '='; break;

			default:
#if	0
				/* tell us about a key we do net yet map! */
				printf("<%d:%d/%d> ", pIr->EventType,
					pIr->Event.KeyEvent.uChar.AsciiChar,
					   pIr->Event.KeyEvent.wVirtualScanCode);
#endif
				return;
		}

pastothers:	/* shifted special key? */
		if (state & SHIFT_PRESSED)
			lprefix |= SHFT;
	}

	/* decode the various modifiers to the character */
	if (state & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED)) {
		lprefix |= ALTD;
		if (islower(c))
			c = c - 'a' + 'A';
	}
	if ((state & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) && c > 31)
		lprefix |= CTRL;

	/* if there is a lprefix, insert it in the input stream */
	if (lprefix != 0) {
		in_put(0);
		in_put(lprefix >> 8);
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
#define MAX_INPUT_EVENTS 128
#define INPUT_WINDOW_SIZE 0
#define INPUT_KEYBOARD_READY 1

static
int WaitForInput()
{
	/* wait for input for a short time */
	while(WaitForSingleObject(hInput, 10) == WAIT_TIMEOUT)
	{
		/* check to see if the console window has been resized */
		if(PendingScreenResize())
		{
			return INPUT_WINDOW_SIZE;
		}
	}
	return INPUT_KEYBOARD_READY;
}

int PASCAL NEAR ntgetc()
{
	DWORD dw = 0;
	INPUT_RECORD ir[MAX_INPUT_EVENTS];
	BOOL success = FALSE;
	DWORD i;
	BOOL doMore = TRUE;


	for(;;)
	{

		ntflush();

		/* return any keystrokes waiting in the
		   type ahead buffer */
		if(in_check())
			return(in_get());

		if(INPUT_WINDOW_SIZE == WaitForInput())
		{
			WindowSizeEvent();
			continue;
		}

		/* get the next keyboard/mouse/resize event */

		if(FALSE == (success = ReadConsoleInput(hInput, ir, MAX_INPUT_EVENTS, &dw)))
		{
			meexit(127);
		}

		for(i = 0; i < dw; ++i)
		{
			INPUT_RECORD *pIr = (ir + i);
			/* let the proper event handler field this event */
			switch(pIr->EventType)
			{

			case KEY_EVENT:
				KeyboardEvent(pIr);
				break;

			case MOUSE_EVENT:
				MouseEvent(pIr);
				break;

			case WINDOW_BUFFER_SIZE_EVENT:
				WindowSizeEvent();
				break;

			case MENU_EVENT:
				break;
			case FOCUS_EVENT:
				break;
			default:
				/* We should never get here */
				break;
			}
		}
	}
}

#if TYPEAH
/*----------------------------------------------------------------------*/
/*	typahead()							*/
/* Returns true if a key has been pressed.				*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR typahead()

{
	/* anything waiting in the input queue? */
	if (in_check())
		return(TRUE);

	return(FALSE);
}
#endif

static WORD near ntAttribute(void)
{
	return(WORD) (revflag ? (cbcolor | (cfcolor << 4)) : ((cbcolor << 4) | cfcolor));
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

int PASCAL NEAR ntputc(int c)
{
	WORD wScreenPos;

	/* only characters on screen!!! */
	if (ntcol < 0 || ntcol > 32000)
			return TRUE;

	if (c == '\n' || c == '\r') { 		/* returns and linefeeds */
		ntrow++;
		ntcol = 0;
		return TRUE;
	}

	if (c == '\b') {			/* backspace */
		--ntcol;
		ntputc(' ');
		--ntcol;
		return TRUE;
	}

	wScreenPos = (ntrow * term.t_ncol) + ntcol++;
	ciScreenBuffer[wScreenPos].Char.AsciiChar = (char)c;
	ciScreenBuffer[wScreenPos].Attributes = ntAttribute();
	ntSetUpdateValues();
	return TRUE;
}


/*----------------------------------------------------------------------*/
/*	nteeol()							*/
/* Erase to end of line.						*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR nteeol(void)
{
	WORD wNum;
	WORD wScreenPos;
	WORD wAttribute;

	/* only characters on screen!!! */
	if (ntcol < 0 || ntcol > 32000)
			return TRUE;

	wNum = term.t_ncol - ntcol;
	wScreenPos = ntrow * term.t_ncol + ntcol;
	wAttribute = ntAttribute();
	for (; wNum; wNum--) {
		ciScreenBuffer[wScreenPos].Char.AsciiChar = ' ';
		ciScreenBuffer[wScreenPos].Attributes = wAttribute;
		wScreenPos++, ntcol++;
	}
	ntSetUpdateValues();
	return TRUE;
}


/*----------------------------------------------------------------------*/
/*	nteeop()							*/
/* Erase to end of page.						*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR nteeop(void)
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
		ciScreenBuffer[wScreenPos].Char.AsciiChar = ' ';
		ciScreenBuffer[wScreenPos].Attributes = wAttribute;
		wScreenPos++, ntcol++;
	}
	ntSetUpdateValues();
	return 0;
}

/*----------------------------------------------------------------------*/
/*	ntrev() 						*/
/* Change reverse video state.						*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR ntrev(int state)
{
	revflag = state;
	return 0;
}

/*----------------------------------------------------------------------*/
/*	ntcres()							*/
/* Change the screen resolution.					*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR ntcres(char *res)		/* name of desired video mode	*/
{
	return TRUE;
}


/*----------------------------------------------------------------------*/
/*	spal()								*/
/* Change pallette settings.  (Does nothing.)				*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR spal(char *dummy)
{
	return(TRUE);
}


/*----------------------------------------------------------------------*/
/*	ntbeep()							*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR ntbeep(void)
{
	Beep(750, 300);
	return(TRUE);
}

/*----------------------------------------------------------------------*/
/*	ntopen()							*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR ntopen()
{
	BOOL success = FALSE;
	CONSOLE_SCREEN_BUFFER_INFOEX Console = { 0 };
	Console.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);

	/* Get our standard handles */
	hInput = GetStdHandle(STD_INPUT_HANDLE);
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	/* Store the original console info */
	success = GetConsoleScreenBufferInfoEx(hOutput, &Console);

	OldConsoleInfo = Console;

	/* Save the titlebar of the window so we can
		* restore it when we leave. */
	success = GetConsoleTitle(chConsoleTitle, sizeof(chConsoleTitle)/sizeof(chConsoleTitle[0]));


	/* initialize the input queue */
	in_init();
	strcpy(os, "WINXP");

	/* This will allocate a console if started from
	/* the windows NT program manager. */
	/* AllocConsole(); */



	/* Set Window Title to MicroEMACS */
	success = SetConsoleTitle(PROGNAME);

	/* get a ptr to the output screen buffer */

	/* success = SetConsoleMode(hInput, ENABLE_WINDOW_INPUT); */

	/* let MicroEMACS know our starting screen size */
	term.t_nrow = Console.srWindow.Bottom;
	term.t_ncol = Console.srWindow.Right + 1;
	term.t_mrow = NROW - 1;
	term.t_mcol = NCOL;
	if (term.t_nrow > term.t_mrow)
		term.t_nrow = term.t_mrow;
	if (term.t_ncol > term.t_mcol)
		term.t_ncol = term.t_mcol;

	Console.wAttributes = (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);


	success = SetConsoleScreenBufferInfoEx(hOutput, &Console);
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

int PASCAL NEAR ntclose(void)
{
	/* reset the title on the window */
	SetConsoleTitle(chConsoleTitle);

	if(OldConsoleInfo.cbSize == sizeof(CONSOLE_SCREEN_BUFFER_INFOEX))
	{
		SetConsoleScreenBufferInfoEx(hOutput, &OldConsoleInfo);
	}

	/* FreeConsole(); */
	return(TRUE);
}

/*----------------------------------------------------------------------*/
/*	ntkopen()							*/
/* Open the keyboard.							*/
/*----------------------------------------------------------------------*/

int PASCAL NEAR ntkopen(void)
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

int PASCAL NEAR ntkclose(void)
{
	/* restore the console mode from entry */
	SetConsoleMode(hInput, OldConsoleMode);
	return(TRUE);
}

#if FLABEL
int PASCAL NEAR fnclabel(f, n)	/* label a function key */

int f,n;	/* default flag, numeric argument [unused] */

{
	/* on machines with no function keys...don't bother */
	return(TRUE);
}
#endif
#endif

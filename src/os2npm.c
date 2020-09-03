/*
 * OS2NONPM.C
 *
 * The routines in this file provide video and keyboard support using the
 * OS/2 Vio and Kbd functions (not the presentation manager).
 *
 * The os2putc, os2eeol and os2eeop routines modify the logical video
 * buffer.  Os2flush calls VioShowBuf to update the physical video buffer.
 * An earlier version used VioWrtTTy with ANSI processing (easy to do, but
 * sloooow).  A later version using VioWrtNCell was better, but not as
 * good as manipulating the logical buffer.
 */

#define INCL_BASE
#include	<os2.h>

#define	termdef	1			/* don't define "term" external */

#include	<stdio.h>

#undef	PASCAL
#undef	NEAR
#undef	HIBYTE

#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"

#if     OS2NPM
/*
 * Os2def.h defines COLOR, but no MSC stuff needs it.
 * We need COLOR as defined in estruct.h, so edit it out of os2def.h.
 */
#include	<conio.h>

#define	NROW    102		/* Screen size.                 */
#define	NCOL    80		/* Edit if you want to.         */
#define	MARGIN	8		/* size of minimim margin and	*/
#define	SCRSIZ	64		/* scroll size for extended lines */
#define	NPAUSE	100		/* # times thru update to pause */

#define CDCGA	0		/* color graphics adapter	*/
#define CDMONO	1		/* monochrome display adapter	*/
#define CDEGA	2		/* EGA				*/
#define CDVGA	3		/* VGA				*/

#define NDRIVE	4		/* number of video modes	*/

int dtype = -1;				/* current video mode	*/
char drvname[][8] = {			/* names of video modes	*/
	"CGA", "MONO", "EGA", "VGA"
};

/* Forward references.          */

os2move();
os2eeol();
os2eeop();
os2beep();
os2open();
os2close();
os2getc();
os2putc();
os2flush();
os2rev();
os2kclose();
os2kopen();
os2cres();
os2parm();
#if	COLOR
os2fcol();
os2bcol();
#endif

struct {			/* Current screen attribute for ORing	*/
	BYTE	filler;		/* with character to be displayed.	*/
	BYTE	attr;
} os2cell = {0, 0x07};

struct {			/* Current reverse screen attribute for	*/
	BYTE	filler;		/* ORing with character to be displayed.*/
	BYTE	attr;
} os2rcell = {0, 0x07};

static struct {				/* initial states		*/
	USHORT		ansiState;	/* ANSI translation		*/
	VIOCONFIGINFO	vioConfigInfo;	/* video configuration		*/
	VIOMODEINFO	vioModeInfo;	/* video mode			*/
	KBDINFO		kbdInfo;	/* keyboard info		*/
} initial;

static int	cfcolor = -1;		/* current foreground color	*/
static int	cbcolor = -1;		/* current background color	*/
static int	ctrans[] =	/* ansi to ibm color translation table	*/
	{0, 4, 2, 6, 1, 5, 3, 7,
	 8, 12, 10, 14, 9, 13, 11, 15};

static short	 os2row;		/* current cursor row	*/
static short	 os2col;		/* current cursor col	*/

int revflag = FALSE;			/* are we currently in rev video? */

/*
 * To minimize the amount of buffer that VioShowBuf has to update, we
 * keep track of the lowest and highest bytes in the logical video
 * buffer which have been modified.
 */
static USHORT	*lvb;			/* logical video buffer	*/
static USHORT	 lvbLen;		/* length of buffer	*/
static USHORT	 lvbMin;		/* min index of modified byte	*/
static USHORT	 lvbMax;		/* max index of modified byte	*/

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
	os2open,
	os2close,
	os2kopen,
	os2kclose,
	os2getc,
	os2putc,
	os2flush,
	os2move,
	os2eeol,
	os2eeop,
	os2eeop,
	os2beep,
	os2rev,
	os2cres
#if	COLOR
	, os2fcol,
	os2bcol
#endif
};

#if	MOUSE
/*	Mousing global variable	*/
static int mexist;	/* is the mouse driver installed? */
static int nbuttons;	/* number of buttons on the mouse */
static int oldbut;	/* Previous state of mouse buttons */
static int oldcol;	/* previous x position of mouse */
static int oldrow;	/* previous y position of mouse */
HMOU mouse_handle;	/* handle to opened mouse */
#endif

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

#if	COLOR
/*----------------------------------------------------------------------*/
/*	os2fcol()							*/
/* Set the current foreground color.					*/
/*----------------------------------------------------------------------*/

os2fcol(
	int color)			/* color to set */
{
	if (dtype != CDMONO)
		cfcolor = ctrans[color];
	else
		cfcolor = 7;

	/* set the normal attribute */
	os2cell.attr &= 0xF0;
	os2cell.attr |= cfcolor;

	/* set the reverse attribute */
	os2rcell.attr &= 0x07;
	os2rcell.attr |= cfcolor << 4;	
}

/*----------------------------------------------------------------------*/
/*	os2bcol()							*/
/* Set the current background color.					*/
/*----------------------------------------------------------------------*/

os2bcol(
	int color)		/* color to set */
{
	if (dtype != CDMONO)
		cbcolor = ctrans[color];
	else
		cbcolor = 0;

	/* set normal background attribute */
	os2cell.attr &= 0x0F;
	os2cell.attr |= cbcolor << 4;

	/* set reverse background attribute */
	os2rcell.attr &= 0x70;
	os2rcell.attr |= cbcolor;
}
#endif


/*----------------------------------------------------------------------*/
/*	os2move()							*/
/* Move the cursor.							*/
/*----------------------------------------------------------------------*/

os2move(
	int row,
	int col)
{
	os2row = row;
	os2col = col;
	VioSetCurPos(os2row, os2col, 0);
}


/*----------------------------------------------------------------------*/
/*	os2flush()							*/
/* Update the physical video buffer from the logical video buffer.	*/
/*----------------------------------------------------------------------*/

os2flush(void)
{
	if (lvbMin <= lvbMax) {		/* did anything change?	*/
		VioShowBuf(lvbMin * 2, (lvbMax - lvbMin + 1) * 2, 0);
		VioSetCurPos(os2row, os2col, 0);
	}
	lvbMin = lvbLen;
	lvbMax = 0;
}


/*----------------------------------------------------------------------*/
/*	os2char()							*/
/* Get a character from the keyboard.					*/
/* Function keys, editing keys and alt- keys queue extra bytes into     */
/* input queue.
/*----------------------------------------------------------------------*/

os2char()
{
	register c;		/* extended key code for special keys */
	KBDKEYINFO keyInfo;

	KbdCharIn(&keyInfo, IO_WAIT, 0);	/* get a character	*/

	/* Function, edit or alt- key?	*/
	if (keyInfo.chChar == 0  ||  keyInfo.chChar == 0xE0) {
		c = extcode(keyInfo.chScan); /* hold on to scan code */
		in_put(c >> 8);		/* queue up prefix byte */
		in_put(c & 0xFF);	/* queue up event byte */
		return(0);
	}
	return(keyInfo.chChar & 255);
}

/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all. Also mouse events are forced into the input stream here.
 */
os2getc()

{
	register int c;		/* character read */
#if	MOUSE
	NOPTRRECT mouse_rect;
#endif

	os2flush();

ttc:	/* return any keystrokes waiting in the
	   type ahead buffer */
	if (in_check())
		return(in_get());

#if	TYPEAH
	if (typahead())
		return(os2char());

	/* with no mouse, this is a simple get char routine */
	if (mexist == FALSE || mouseflag == FALSE)
		return(os2char());

#if	MOUSE
	/* turn the mouse cursor on */
	MouDrawPtr(mouse_handle);

	/* loop waiting for something to happen */
	while (TRUE) {
		if (typahead())
			break;
		if (checkmouse())
			break;
		DosSleep(10L);	/* let others have some CPU! */
	}

	/* turn the mouse cursor back off */
	mouse_rect.row = 0;
	mouse_rect.col = 0;
	mouse_rect.cRow = 24;
	mouse_rect.cCol = 79;
	MouRemovePtr(&mouse_rect, mouse_handle);

	goto ttc;
#endif	/* MOUSE */
#else	/* TYPEAH */
	return(os2char());
#endif	/* TYPEAH */
}

#if	MOUSE
checkmouse()

{
	register int k;		/* current bit/button of mouse */
	register int etype;	/* event type byte */
	register int event;	/* encoded mouse event */
	int mousecol;		/* current mouse column */
	int mouserow;		/* current mouse row */
	int sstate;		/* current shift key status */
	int newbut;		/* new state of the mouse buttons */
	MOUEVENTINFO mouse_event;	/* info about a mouse event */
	MOUQUEINFO mouse_queue;	/* holds info about the mouse queue */
	USHORT wait_flag;	/* wait flag for read mouse queue call */
	KBDINFO kbd_info;	/* keyboard information */

	/* is there a mouse event queued up? */
	MouGetNumQueEl(&mouse_queue, mouse_handle);
	if (mouse_queue.cEvents == 0)
		return(FALSE);	

	/* get the current mouse event */
	wait_flag = MOU_WAIT;
	MouReadEventQue(&mouse_event, &wait_flag, mouse_handle);

	/* build a simple bit field out of OS/2's rather complex one */
	newbut = 0;
	if (mouse_event.fs & MOUSE_BN1_DOWN)
		newbut |= 1;
	if (mouse_event.fs & MOUSE_MOTION_WITH_BN1_DOWN)
		newbut |= 1;
	if (mouse_event.fs & MOUSE_BN2_DOWN)
		newbut |= 2;
	if (mouse_event.fs & MOUSE_MOTION_WITH_BN2_DOWN)
		newbut |= 2;
	if (mouse_event.fs & MOUSE_BN3_DOWN)
		newbut |= 4;
	if (mouse_event.fs & MOUSE_MOTION_WITH_BN3_DOWN)
		newbut |= 4;

	/* check to see if any mouse buttons are different */
	mousecol = mouse_event.col;
	mouserow = mouse_event.row;

	/* only notice changes */
	if ((oldbut == newbut) && (mousecol == oldcol)
	    && (mouserow == oldrow))
		return(FALSE);

	/* get the shift key status as well */
	KbdGetStatus(&kbd_info, 0);
	etype = MOUS >> 8;
	sstate = kbd_info.fsState;
	if (sstate & (RIGHTSHIFT | LEFTSHIFT)) /* shifted */
		etype |= (SHFT >> 8);
	else if (sstate & CONTROL)	/* controled? */
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

	/* only on screen presses are legit! */
	if (mousecol < 0)
		mousecol = 0;
	if (mouserow < 0)
		mouserow = 0;

	for (k = 1; k != (1 << nbuttons); k = k << 1) {

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
			event += 'a';		/* plain */
			in_put(event);
			oldbut = newbut;
			oldcol = mousecol;
			oldrow = mouserow;
			return(TRUE);
		}
	}
	return(TRUE);
}
#endif

#if	TYPEAH
/*----------------------------------------------------------------------*/
/*	typahead()							*/
/* Returns true if a key has been pressed.				*/
/*----------------------------------------------------------------------*/

typahead()

{
	KBDKEYINFO kbdinfo;

	KbdPeek(&kbdinfo, 0);
	return(kbdinfo.fbStatus != 0);
}
#endif


/*----------------------------------------------------------------------*/
/*	os2putc()							*/
/* Put a character at the current position in the current colors.	*/
/* Note that this does not behave the same as putc() or VioWrtTTy().	*/
/* This routine does nothing with returns and linefeeds.  For backspace */
/* it puts a space in the previous column and moves the cursor to the	*/
/* previous column.  For all other characters, it will display the	*/
/* graphic representation of the character and put the cursor in the	*/
/* next column (even if that is off the screen.  In practice this isn't	*/
/* a problem.								*/
/*----------------------------------------------------------------------*/

os2putc(int c)
{
	USHORT	cell;
	USHORT	i;

	if (c == '\n' || c == '\r') {		/* returns and linefeeds */
		return;
	}
	if (c == '\b') {			/* backspace		*/
		cell = ' ' | (revflag ? *(USHORT *)&os2rcell : *(USHORT *)&os2cell);
		--os2col;			/* move cursor back	*/
		i = os2row * term.t_ncol + os2col;
	}
	else {
		cell = (0x00ff & c) | (revflag ? *(USHORT *)&os2rcell : *(USHORT *)&os2cell);
		i = os2row * term.t_ncol + os2col;
		++os2col;			/* move cursor forward	*/
	}
	lvb[i] = cell;
	if (i < lvbMin)
		lvbMin = i;
	if (i > lvbMax)
		lvbMax = i;
}


/*----------------------------------------------------------------------*/
/*	os2eeol()							*/
/* Erase to end of line.						*/
/*----------------------------------------------------------------------*/

os2eeol()
{
	USHORT	cell = ' ';
	USHORT  i;

	cell |= (revflag ? *(USHORT *)&os2rcell : *(USHORT *)&os2cell);
	
	i = os2row * term.t_ncol + os2col;  /* current cursor position	*/
	if (i < lvbMin)
		lvbMin = i;
	while (i < os2row * term.t_ncol + term.t_ncol)
		lvb[ i++] = cell;
	if (--i > lvbMax)
		lvbMax = i;
}


/*----------------------------------------------------------------------*/
/*	os2eeop()							*/
/* Erase to end of page.						*/
/*----------------------------------------------------------------------*/

os2eeop()
{
	USHORT	cell = ' ';
	USHORT  i;

#if COLOR
	if (dtype != CDMONO)
		cell |= (ctrans[gbcolor] << 4 | ctrans[gfcolor]) << 8;
	else
		cell |= 0x0700;
#else
	cell |= 0x0700;
#endif

	i = os2row * term.t_ncol + os2col;  /* current cursor position	*/
	if (i < lvbMin)
		lvbMin = i;
	while (i < term.t_nrow * term.t_ncol + term.t_ncol)
		lvb[ i++] = cell;
	if (--i > lvbMax)
		lvbMax = i;
}

/*----------------------------------------------------------------------*/
/*	os2rev()							*/
/* Change reverse video state.  					*/
/*----------------------------------------------------------------------*/

os2rev(state)

int state;	/* TRUE = reverse, FALSE = normal */

{
	revflag = state;
}

/*----------------------------------------------------------------------*/
/*	os2cres()							*/
/* Change the screen resolution.					*/
/*----------------------------------------------------------------------*/

os2cres(char *res)		/* name of desired video mode	*/
{
	USHORT	err;
	int	type;			/* video mode type	*/
	VIOMODEINFO vioModeInfo;

	vioModeInfo = initial.vioModeInfo;
	
	/* From the name, find the type of video mode.	*/
	for (type = 0; type < NDRIVE; type++) {
		if (strcmp(res, drvname[type]) == 0)
			break;
	}
	if (type == NDRIVE)
		return(FALSE);	/* not a mode we know about	*/

		
	switch (type) {
		case CDMONO:
		case CDCGA:
			vioModeInfo.row = 25;
			break;
		case CDEGA:
			vioModeInfo.row = 43;
			break;
		case CDVGA:
			vioModeInfo.row = 50;
			break;
	}
	
	if (VioSetMode(&vioModeInfo, 0))	/* change modes 	*/
		return(FALSE);			/* couldn't do it	*/

	newsize(TRUE, vioModeInfo.row);

	/* reset the $sres environment variable */
	strcpy(sres, drvname[type]);
	dtype = type;				/* set the current mode	*/
	
	return TRUE;
}


/*----------------------------------------------------------------------*/
/*	spal()								*/
/* Change pallette settings.  (Does nothing.)				*/
/*----------------------------------------------------------------------*/

spal(char *dummy)
{
}


/*----------------------------------------------------------------------*/
/*	os2beep()							*/
/*----------------------------------------------------------------------*/

os2beep()
{
	DosBeep(1200, 175);
}


/*----------------------------------------------------------------------*/
/*	os2open()							*/
/* Find out what kind of video adapter we have and the current video	*/
/* mode.  Even if the adapter supports a higher resolution mode than is */
/* in use, we still use the current mode.				*/
/*----------------------------------------------------------------------*/

os2open()
{
#if	MOUSE
	PTRLOC mouse_pos;	/* position to place mouse */
	USHORT event_mask;		/* event mask for mouse handling */
#endif

	initial.vioConfigInfo.cb = 0x0A;
	VioGetConfig(0, &initial.vioConfigInfo, 0);
	switch (initial.vioConfigInfo.adapter) {
		case DISPLAY_VGA:
			dtype = CDVGA;
			break;
		case DISPLAY_EGA:
			dtype = CDEGA;
			break;
		case DISPLAY_CGA:
		case 8:	/* some new mode in OS/2 2.0 */
			dtype = CDCGA;
			break;
		case DISPLAY_MONOCHROME:
		default:
			dtype = CDMONO;
	}
	strcpy(sres, drvname[dtype]);

	initial.vioModeInfo.cb = 0x0E;
	VioGetMode(&initial.vioModeInfo, 0);
	newsize(TRUE, initial.vioModeInfo.row);
			
	VioGetAnsi(&initial.ansiState, 0);
	VioGetBuf((PULONG)&lvb, &lvbLen, 0);
	lvbMin = lvbLen;
	lvbMax = 0;

	revexist = TRUE;
	revflag = FALSE;

	/* initialize our character input queue */
	in_init();
	strcpy(os, "OS2");

#if	MOUSE
	/* find out if we have a mouse */
	if (MouOpen((PSZ)0, &mouse_handle) != 0) {
		mexist = FALSE;
		return;
	}

	/* find out some info about the mouse */
	mexist = TRUE;
	MouGetNumButtons(&nbuttons, mouse_handle);

	/* tell it what events were interested in */
/*	event_mask = MOUSE_BN1_DOWN | MOUSE_BN2_DOWN | MOUSE_BN3_DOWN;*/
	event_mask = 0xff;
	MouSetEventMask(&event_mask, mouse_handle);

	/* get the mouse in the upper right corner */
	oldrow = mouse_pos.row = 0;	/* top row of display */
	oldcol = mouse_pos.col = (term.t_ncol - 1); /* last col of display */
	MouSetPtrPos(&mouse_pos, mouse_handle);
#else
	mexist = FALSE;
#endif
}

/*----------------------------------------------------------------------*/
/*	os2close()							*/
/* Restore the original video settings.					*/
/*----------------------------------------------------------------------*/

os2close()
{
#if	MOUSE
	/* close our use of the mouse */
	if (mexist)
		MouClose(mouse_handle);
#endif

	VioSetAnsi(initial.ansiState, 0);
	VioSetMode(&initial.vioModeInfo, 0);
	VioSetCurPos(initial.vioModeInfo.row - 1,
			 initial.vioModeInfo.col - 1, 0);
}

/*----------------------------------------------------------------------*/
/*	os2kopen()							*/
/* Open the keyboard.							*/
/*----------------------------------------------------------------------*/

os2kopen()
{
	KBDINFO	kbdInfo;

	initial.kbdInfo.cb = 0x000A;
	KbdGetStatus(&initial.kbdInfo, 0);	
	kbdInfo = initial.kbdInfo;
	kbdInfo.fsMask &= ~0x0001;		/* not echo on		*/
	kbdInfo.fsMask |= 0x0002;		/* echo off		*/
	kbdInfo.fsMask &= ~0x0008;		/* cooked mode off	*/
	kbdInfo.fsMask |= 0x0004;		/* raw mode		*/
	kbdInfo.fsMask &= ~0x0100;		/* shift report	off	*/
	KbdSetStatus(&kbdInfo, 0);
}


/*----------------------------------------------------------------------*/
/*	os2kclose()							*/
/* Close the keyboard.							*/
/*----------------------------------------------------------------------*/

os2kclose()
{
	KbdSetStatus(&initial.kbdInfo, 0); /* restore original state	*/
}

#if	FLABEL
fnclabel(f, n)	/* label a function key */

int f,n;	/* default flag, numeric argument [unused] */

{
	/* on machines with no function keys...don't bother */
	return(TRUE);
}
#endif

#if 0
/*----------------------------------------------------------------------*/
/*	scwrite()							*/
/* Write a line to the screen.
/* I tried using this routine with MEMMAP = 1, but there were too many	*/
/* problems with the cursor and flushing the buffer.			*/
/*----------------------------------------------------------------------*/

scwrite(
	int	 row,	/* row of screen to place outstr on */
	char	*outstr,/* string to write out (must be term.t_ncol long) */
	int	 forg,	/* foreground color of string to write */
	int	 bacg,	/* background color */
{
	USHORT	 attr;
	int	 i;

	attr = (((ctrans[bacg] & 15) << 4) | (forg & 15)) << 8;

	for (i = row * term.t_ncol;  i < (row + 1) * term.t_ncol;  i++)
		lvb[i] = attr | *(outstr++);

	if (i < lvbMin)
		lvbMin = i;
	if (i > lvbMax)
		lvbMax = i;
}
#endif
#endif


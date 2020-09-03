/*
 *	MicroEMACS 4.00 	XVT.C
 *		written by Daniel M. Lawrence
 *
 *	(C)Copyright 1995 by Daniel M. Lawrence
 *	MicroEMACS 4.00 can be copied and distributed freely for any
 *	non-commercial purposes. MicroEMACS 4.00 can only be incorporated
 *	into commercial software with the permission of the current author.
 *
 *	The routines in this file provide support for the XVT windowing system
 *
 *	Notice that the xvt header files force stdlib.h into the compile
 *	even though it conflicts with some of our declarations......
 *	This forced us to use the standard definitions and ditch ours.
 */

#define termdef 1			/* don't define term external */
#define XVTDRIVER	1		/* this is the XVT driver */
#define DXVT(a) xvt_dbg(a); xvt_dbg(NULL)

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"

#if	XVT
#undef	COLOR

#include	<xvt.h>
#include	<xvtmenu.h>
#include	"uemacs.h"

int fnclabel(int f, int n);

#define NROW	25			/* Screen size. 		*/
#define NCOL	80			/* Edit if you want to. 	*/
#define NPAUSE	100			/* # times thru update to pause */
#define MARGIN	8			/* size of minimim margin and	*/
#define SCRSIZ	64			/* scroll size for extended lines */
#define BEL	0x07			/* BEL character.		*/
#define ESC	0x1B			/* ESC character.		*/

/* Forward references.		*/
extern int xvtmove();
extern int xvteeol();
extern int xvteeop();
extern int xvtbeep();
extern int xvtopen();
extern int xvtrev();
extern int xvtclose();
extern int xvtkopen();
extern int xvtkclose();
extern int xvtcres();
extern int xvtgetc();
extern int xvtputc();
extern int xvtflush();
extern int xvtfcol();
extern int xvtbcol();
static int rev_state = FALSE;

static int cfcolor = -1;	/* current foreground color */
static int cbcolor = -1;	/* current background color */

/* mouse status information */
static int oldcol;	/* previous x position of mouse */
static int oldrow;	/* previous y position of mouse */

/* Info held for XVT! */

#define	MAXOBUF	256	/* maximum number of output characters buffered */

EWINDOW xvt_win;		/* current window being displayed by XVT */
EWINDOW init_win;	/* the initial window we are to toss */
int xvt_char_width;	/* width of characters in pixels */
int xvt_char_height;	/* height of characters in pixels */
int xvt_descent;	/* height of descenders (character offset) */
int xvt_row = 0;	/* current output row in window */
int xvt_col = 0;	/* current column in window */
int xvt_fcolor = COLOR_WHITE;	/* current xvt forground color */
int xvt_bcolor = COLOR_BLACK;	/* current xvt background color */
COLOR xvt_colors[] = {	/* xvt color translation table */
	COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE,
	COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE,
	COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE,
	COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
};
char xvt_obuf[MAXOBUF];		/* output characters buffer */
char *xvt_next = xvt_obuf;	/* next output character position */
char *xvt_end = &xvt_obuf[MAXOBUF-1];	/* end of the buffer */

/*
 * Standard terminal interface dispatch table. Most of the fields point into
 * "termio" code.
 */
NOSHARE TERM term = {
	NROW-1,
	NROW-1,
	NCOL,
	NCOL,
	0, 0,
	MARGIN,
	SCRSIZ,
	NPAUSE,
	xvtopen,
	xvtclose,
	xvtkopen,
	xvtkclose,
	xvtgetc,
	xvtputc,
	xvtflush,
	xvtmove,
	xvteeol,
	xvteeop,
	xvteeop,
	xvtbeep,
	xvtrev,
	xvtcres,
	xvtfcol,
	xvtbcol
};

/*	Some XVT specific globals	*/

int xvt_argc;		/* save the command line arguments to */
char **xvt_argv;	/* pass to the emacs mainline */

dump_event(char *typ, EVENT *ep)

{
	char m[128];	/* message buffer */

	DXVT(typ);
	switch (ep->type) {
		case E_CREATE:
			DXVT("E_CREATE:");
			break;
		case E_FOCUS:
			DXVT("E_FOCUS:");
			break;
		case E_CLOSE:
			DXVT("E_CLOSE:");
			break;
		case E_DESTROY:
			DXVT("E_DESTROY:");
			break;
		case E_UPDATE:
			DXVT("E_UPDATE:");
			break;
		case E_COMMAND:
			DXVT("E_COMMAND:");
			break;
		case E_SIZE:
			DXVT("E_SIZE:");
			break;
		case E_CHAR:
			sprintf(m, "E_CHAR: %d S%d C%d", ep->v.chr.ch,
				ep->v.chr.shift, ep->v.chr.control);
			DXVT(m);
			break;
		case E_MOUSE_DOWN:
			sprintf(m, "E_MOUSE_DOWN: [%d] %d/%d S%d C%d",
				ep->v.mouse.button,
				ep->v.mouse.where.v,ep->v.mouse.where.h,
				ep->v.mouse.shift, ep->v.mouse.control);
			DXVT(m);
			break;
		case E_MOUSE_UP:
			sprintf(m, "E_MOUSE_UP: [%d] %d/%d S%d C%d",
				ep->v.mouse.button,
				ep->v.mouse.where.v,ep->v.mouse.where.h,
				ep->v.mouse.shift, ep->v.mouse.control);
			DXVT(m);
			break;
		case E_MOUSE_MOVE:
			sprintf(m, "E_MOUSE_MOVE: [%d] %d/%d S%d C%d",
				ep->v.mouse.button,
				ep->v.mouse.where.v,ep->v.mouse.where.h,
				ep->v.mouse.shift, ep->v.mouse.control);
			DXVT(m);
			break;
		default:
			sprintf(m, "E_UNKNOWN: %d", ep->type);
			DXVT(m);
			break;
	}
}

static long windowHandler(EWINDOW win, EVENT *ep)

{
	register int etype;	/* event type byte */
	register int event;	/* encoded mouse event */
	int mouserow, mousecol;	/* mouse row/column position */

/*dump_event("win", ep);*/

	switch (ep->type) {
		case E_CREATE:
			clear_window(win, COLOR_RED);

		case E_CHAR:	/* a character has been typed */

			/* if it was a simple one */
			if (ep->v.chr.ch < UCHAR_MAX) {
				in_put(ep->v.chr.ch);
				break;
			}

			/* otherwise, its a function key! */
			in_fkey(ep->v.chr.ch);
			break;

		case E_FOCUS:
			break;
		case E_UPDATE:
/*			sgarbf = TRUE;*/
			break;

		case E_MOUSE_MOVE:	/* the mouse has moved */
		case E_MOUSE_DOWN:	/* a button has gone down */
		case E_MOUSE_UP:	/* a button has been released */

			/* locate the new charactor position */
			mouserow = ep->v.mouse.where.v / xvt_char_height;
			mousecol = ep->v.mouse.where.h / xvt_char_width;

			/* if its in the same character cell... ignore it */
			if ((ep->type == E_MOUSE_MOVE) &&
			    (mouserow == oldrow) && (mousecol == oldcol))
				return;

			/* get the event type */
			etype = MOUS >> 8;
			if (ep->v.mouse.shift == TRUE)
				etype |= (SHFT >> 8);
			if (ep->v.mouse.control == TRUE)
				etype |= (CTRL >> 8);

			/* no buttons changes */
			if (ep->type == E_MOUSE_MOVE) {
		
				/* generate a mouse movement */
				if (((mouse_move == 1) && (mmove_flag == TRUE)) ||
				    (mouse_move == 2)) {
					in_put(0);
					in_put(etype);
					in_put(mousecol);
					in_put(mouserow);
					in_put('m');
				}

				/* save the new mouse location */
				oldcol = mousecol;
				oldrow = mouserow;
				break;
			}


			/* encode the mouse button press */
			in_put(0);
			in_put(etype);
			in_put(mousecol);
			in_put(mouserow);
			
			/* direction of the button press */
			event = ((ep->type == E_MOUSE_DOWN) ? 0 : 1);
			if (ep->v.mouse.button == 1)	/* right button */
				event += 4;
			if (ep->v.mouse.button == 2)	/* center button */
				event += 2;
			event += 'a';
			in_put(event);

			/* save the new mouse location */
			oldcol = mousecol;
			oldrow = mouserow;
			break;			

		case E_CLOSE:
			xvt_terminate();
	}
	return(0L);
}

in_fkey(xvt_code)	/* input an xvt key code into the MicroEMACS
			   input stream */

int xvt_code;

{
	unsigned int code;	/* resulting MicroEMACS extended key code */

	switch (xvt_code) {
		case K_F1:     /* function key 1 */
			code = SPEC | '1'; break;
		case K_F2:
			code = SPEC | '2'; break;
		case K_F3:
			code = SPEC | '3'; break;
		case K_F4:
			code = SPEC | '4'; break;
		case K_F5:
			code = SPEC | '5'; break;
		case K_F6:
			code = SPEC | '6'; break;
		case K_F7:
			code = SPEC | '7'; break;
		case K_F8:
			code = SPEC | '8'; break;
		case K_F9:
			code = SPEC | '9'; break;
		case K_F10:
			code = SPEC | '0'; break;
		case K_F11:
			code = SPEC | '-'; break;
		case K_F12:
			code = SPEC | '='; break;
#if	0
		case K_F13:
			code = SPEC | ''; break;
		case K_F14:
			code = SPEC | ''; break;
		case K_F15:    /* function key 15 */
			code = SPEC | ''; break;
#endif
		case K_UP:     /* up arrow */
			code = SPEC | 'P'; break;
		case K_DOWN:   /* down arrow */
			code = SPEC | 'N'; break;
		case K_RIGHT:  /* right arrow */
			code = SPEC | 'F'; break;
		case K_LEFT:   /* left arrow */
			code = SPEC | 'B'; break;
		case K_PREV:   /* previous screen */
			code = SPEC | 'Z'; break;
		case K_NEXT:   /* next screen */
			code = SPEC | 'V'; break;
		case K_LHOME:  /* line home */
			code = SPEC | '<'; break;
		case K_LEND:   /* line end */
			code = SPEC | '>'; break;
		case K_HOME:   /* home */
			code = SPEC | SHFT | 'B'; break;
		case K_END:    /* end */
			code = SPEC | SHFT | 'F'; break;
		case K_INS:    /* insert */
			code = SPEC | 'C'; break;
		case K_WLEFT:  /* word left */
			code = SPEC | CTRL | 'B'; break;
		case K_WRIGHT: /* word right */
			code = SPEC | CTRL | 'F'; break;
		case K_BTAB:   /* back tab */
			code = SPEC | CTRL | 'I'; break;
		case K_HELP:   /* help */
			code = SPEC | CTRL | '>'; break;
		case K_CLEAR:  /* clear */
			code = SPEC | 'k'; break;
	}

	/* put the key in the input stream */
	in_put(0);
	in_put(code >> 8);
	in_put(code & 0xFF);

}

static long taskHandler(EWINDOW win, EVENT *ep)
{
	RCT rct;
	char aname[80]; 	/* application name */
	DRAW_CTOOLS dtool;
	CBRUSH brush;	/* brushed used for the clearing */
	FONT fptr;	/* font to select */
	int leading, ascent, descent;


/* dump_event("tsk", ep); */

	switch (ep->type) {
		case E_CREATE:

			/* set the drawing mode to opaque */
			win_get_draw_ctools(win, &dtool);
			dtool.opaque_text = TRUE;
			win_set_draw_ctools(win, &dtool);
		
			/* select an appropriate non-proportional font */
			select_font(FF_FIXED, 0, 10, &fptr);
			win_set_font(win, &fptr, FALSE);
		
			/* set the current brush color! */
			brush.pat = PAT_SOLID;
			brush.color = xvt_colors[cbcolor];
			win_set_cbrush(win, &brush);
		
			/* discover the size of the current font */
			win_get_font_metrics(win, &leading, &ascent, &descent);
			xvt_char_height = leading + ascent + descent;
			xvt_descent = descent;
			xvt_char_width  = win_get_text_width(win, "M", 1);
		
			/* hide the task window... we aren't going to use it */
			show_window(win, FALSE);

			/* set up a rect to get an 25 x 80 window */
			set_rect(&rct, xvt_char_width * 1, xvt_char_height * 2,
				 xvt_char_width * 82, xvt_char_height * 27);
			strcpy(aname, PROGNAME);
			strcat(aname, " ");
			strcat(aname, VERSION);

			/* create the first screen window */
			xvt_win = create_window(W_DOC, &rct, aname, 0,
				SCREEN_WIN, WSF_SIZE|WSF_CLOSE|/*WSF_HSCROLL|
				WSF_VSCROLL|*/WSF_NO_MENUBAR,
				EM_ALL, windowHandler, 0L);
			called_main(xvt_argc, xvt_argv);
		case E_CLOSE:
			close_window(win);
			break;
	}
	return(0L);
}

xvt_main(argc, argv)

int argc;
char **argv;

{
	XVT_CONFIG config;
	char aname[80]; 	/* application name */

	/* save the command line arguments to pass to emacs */
	xvt_argc = argc;
	xvt_argv = argv;

	/* set up the structure to let XVT know how to start */
	config.menu_bar_ID = MAIN_MENUBAR;
	config.about_box_ID = 0;
	config.base_appl_name = "uemacs";
	strcpy(aname, PROGNAME);
	strcat(aname, " ");
	strcat(aname, VERSION);
	config.appl_name = aname;
	config.taskwin_title = aname;

	xvt_system(argc, argv, 0L, taskHandler, &config);
}

xvtfcol(color)		/* set the current output color */

int color;	/* color to set */

{
	if (color == cfcolor)
		return;
	xvtflush();
	win_set_fore_color(xvt_win, xvt_colors[color]);
	cfcolor = color;
}

xvtbcol(color)		/* set the current background color */

int color;	/* color to set */

{
	if (color == cbcolor)
		return;
	xvtflush();
	win_set_back_color(xvt_win, xvt_colors[color]);
	cbcolor = color;
}

xvtmove(row, col)
{
	xvtflush();
	xvt_row = row;
	xvt_col = col;
}

xvteeol()
{
	RCT rect;	/* rectangle to use to clear to end of this line */
	CBRUSH brush;	/* brushed used for the clearing */

	xvtflush();

	/* set the rectangles limits */
	set_rect(&rect, xvt_col * xvt_char_width,
			xvt_row * xvt_char_height - 1,
			term.t_ncol * xvt_char_width,
			(xvt_row + 1) * xvt_char_height - 1);

	/* set the current brush color! */
	brush.pat = PAT_SOLID;
	brush.color = xvt_colors[cbcolor];
	win_set_cbrush(xvt_win, &brush);
	win_set_std_cpen(xvt_win, TL_PEN_HOLLOW);

	/* and clear it */
	win_draw_rect(xvt_win, &rect);
}

xvteeop()
{
	RCT rect;	/* rectangle to use to clear to end of this line */
	CBRUSH brush;	/* brushed used for the clearing */

	xvtflush();

	/* set the current brush color! */
	brush.pat = PAT_SOLID;
	brush.color = xvt_colors[gbcolor];
	win_set_cbrush(xvt_win, &brush);
	win_set_std_cpen(xvt_win, TL_PEN_HOLLOW);

	/* first erase to the end of this line... */
	/* set the rectangles limits */
	set_rect(&rect, xvt_col * xvt_char_width,
			xvt_row * xvt_char_height - 1,
			term.t_ncol * xvt_char_width,
			(xvt_row + 1) * xvt_char_height + 1);

	/* and clear it */
	win_draw_rect(xvt_win, &rect);

	/* and then the rest of the window downward */
	/* set the rectangles limits */
	set_rect(&rect, 0,
			(xvt_row + 1) * xvt_char_height - 1,
			term.t_ncol * xvt_char_width,
			term.t_nrow * xvt_char_height);

	/* and clear it */
	win_draw_rect(xvt_win, &rect);
}

xvtrev(state)		/* change reverse video state */

int state;	/* TRUE = reverse, FALSE = normal */

{
	int ftmp, btmp; 	/* temporaries for colors */

	if (state != rev_state) {
		ftmp = cfcolor;
		btmp = cbcolor;
		cfcolor = -1;
		cbcolor = -1;
		xvtfcol(btmp);
		xvtbcol(ftmp);
		rev_state = state;
	}
}

xvtcres()	/* change screen resolution */

{
	return(TRUE);
}

spal(char *dummy)		/* change pallette settings */

{
	/* none for now */
}

xvtbeep()
{
	xvt_beep();
}

xvtopen()

{
	DRAW_CTOOLS dtool;
	CBRUSH brush;	/* brushed used for the clearing */
	FONT fptr;	/* font to select */
	int leading, ascent, descent;

	/* set the drawing mode to opaque */
	win_get_draw_ctools(xvt_win, &dtool);
	dtool.opaque_text = TRUE;
	win_set_draw_ctools(xvt_win, &dtool);

	/* select an appropriate non-proportional font */
	select_font(FF_FIXED, 0, 10, &fptr);
	win_set_font(xvt_win, &fptr, FALSE);

	/* set the current brush color! */
	brush.pat = PAT_SOLID;
	brush.color = xvt_colors[cbcolor];
	win_set_cbrush(xvt_win, &brush);

	/* discover the size of the current font */
	win_get_font_metrics(xvt_win, &leading, &ascent, &descent);
	xvt_char_height = leading + ascent + descent;
	xvt_descent = descent;
	xvt_char_width  = win_get_text_width(xvt_win, "M", 1);

	/* initialize the mouse status */
	oldcol = -1;
	oldrow = -1;

	/* initialize the output character queue */
	xvt_next = xvt_obuf;

	/* let emacs know the screen is open */
	strcpy(sres, "XVT");
	revexist = TRUE;
	ttopen();
}

xvtclose()

{
	xvtfcol(7);
	xvtbcol(0);
	ttclose();
}

xvtkopen()	/* open the keyboard (a noop here) */

{

}

xvtkclose() /* close the keyboard (a noop here) */

{
	xvtflush();
}

/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all. Also mouse events are forced into the input stream here.
 */
int xvtgetc()

{
	xvtflush();

	/* try first.... */
	if (in_check())
		return(in_get());

	/* turn on the text cursor! */
	caret_on(xvt_win, xvt_col * xvt_char_width, (xvt_row + 1) * xvt_char_height - 1);

	/* loop waiting for something to happen */
	while (TRUE) {
		process_events();
		if (in_check()) {
			/* turn off the text cursor */
			caret_off(xvt_win);

			/* and return the event */
			return(in_get());
		}
	}
}

int xvtflush()

{
	/* if there's nothing to flush */
	if (xvt_next == xvt_obuf)
		return;

	/* draw the characters out */
	win_draw_text(xvt_win, xvt_col * xvt_char_width,
			(xvt_row + 1) * xvt_char_height - 1 - xvt_descent,
			&xvt_obuf[0], (xvt_next - xvt_obuf));

	xvt_col += (xvt_next - xvt_obuf);
	xvt_next = xvt_obuf;
}

int xvtputc(c)

char c;	/* character to write to the current xvt window */

{
	/* backspaces just backup the pointers */
	if (c == '\b') {
		xvtflush();
		xvt_col--;
		if (xvt_col < 0)
			xvt_col = 0;
		return;
	}

	/* don't go too far! */
	if (xvt_next == xvt_end)
		xvtflush();

	*xvt_next++ = c;
}

#if	FLABEL
int fnclabel(f, n)		/* label a function key */

int f,n;	/* default flag, numeric argument [unused] */

{
	/* on machines with no function keys...don't bother */
	return(TRUE);
}
#endif
#else
xvthello()
{
}
#endif

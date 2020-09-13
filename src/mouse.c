/*	MOUSE.C:	Mouse functionality commands
			for MicroEMACS 4.00
			originally written by Dave G. Conroy
			modified by Jeff Lomicka and Daniel Lawrence
			Unicode support by Jean-Michel Dubois
*/

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include        "edef.h"
#include	"elang.h"

#define	MNONE	0			/* Mouse commands.		*/
#define	MMOVE	1
#define	MREGDWN	2
#define	MREGUP	3
#define	MCREATE	4
#define MDELETE	5

#if	MOUSE
NOSHARE int	lastypos = HUGENUM;	/* Last mouse event row.	*/
NOSHARE int	lastxpos = HUGENUM;	/* Last mouse event column.	*/
NOSHARE int	lastmcmd = MNONE;	/* Last mouse command.		*/

/*
 * Move mouse button, down. The window that the
 * mouse is in is always selected (this lets you select a
 * window by clicking anyplace in it, even off the end
 * of the text). If the mouse points at text then dot is
 * moved to that location.
 */
int PASCAL NEAR movemd(f, n)

int f,n;	/* prefix flag and argument */

{
	register EWINDOW *wp;
	register EWINDOW *lastwp;
	register LINE	*lp;
#if	CURSES
	WINDOW* wmouse = NULL;

	if (wdrop != NULL && wenclose(wdrop, ypos, xpos))
		wmouse_trafo(wmouse = wdrop, &ypos, &xpos, 0);
	else if (wenclose(wmain, ypos, xpos))
		wmouse_trafo(wmouse = wmain, &ypos, &xpos, 0);
	else if (wenclose(wmenubar, ypos, xpos))
		wmouse_trafo(wmouse = wmenubar, &ypos, &xpos, 0);

	if (wmouse == wmain) {
		handlemenu(wmouse, ypos, xpos);
#endif
		/* make sure we are on the proper screen */
		mouse_screen();

		/* adjust position by screen offset */
		ypos -= term.t_roworg;
		xpos -= term.t_colorg;

		/* if anything has changed, reset the click count */
		if (lastmcmd != MMOVE || lastypos!=ypos || lastxpos!=xpos)
			nclicks = 0;
		++nclicks;
		lastwp = mousewindow(lastypos);		/* remember last window */

		/* reset the last position */
		lastypos = ypos;
		lastxpos = xpos;
		lastmcmd = MMOVE;

		/* if we move the mouse off the windows, don't do anything with it */
		if ((wp=mousewindow(ypos)) == NULL)
			return(FALSE);

		/* if we are on the line with the point, adjust for extended lines */
		if (wp == curwp && (lp = mouseline(wp, ypos)) == curwp->w_dotp)
			xpos += lbound;

		/* make the window the mouse points to current */
		curwp = wp;
		curbp = wp->w_bufp;

		/* if we changed windows, update the modelines */
		if (wp != lastwp)
			upmode();

		/* if we aren't off the end of the text, move the point to the mouse */
		if ((lp=mouseline(wp, ypos)) != NULL) {
			curwp->w_dotp = lp;
			curwp->w_doto = mouseoffset(wp, lp, xpos);
		}
#if	CURSES
	} else if (wmouse)
		return handlemenu(wmouse, ypos, xpos);
#endif

	return(TRUE);
}


/*	mouse-move:	adjust the hilited region by setting mark(hilite+1)
			only if we are holding down the proper button
*/

int PASCAL NEAR mmove(f, n)

int f,n;	/* prefix flag and argument */

{
	register EWINDOW *wp;
	register EWINDOW *lastwp;
	register LINE *lp;
	register int lastmodeline;	/* was the down click on a modeline? */
	register int lastcmdline;	/* was the down click on the command line? */

	/* ignore this if not hilighting */
	if (hilite >= NMARKS)
		return(TRUE);

	/* ignore this if we did not just do a mouse region down */
	if (lastmcmd != MREGDWN)
		return(TRUE);

	/* adjust position by screen offset */
	ypos -= term.t_roworg;
	xpos -= term.t_colorg;

	/* Just where was the last click? */
	lastwp = mousewindow(lastypos);
	lastmodeline = ismodeline(lastwp, lastypos);
	lastcmdline = (lastypos == term.t_nrow);

	/* if it was on a modeline or the command line, don't do things here */
	if (lastmodeline || lastcmdline)
		return(FALSE);

	/* if we move the mouse off the window, abort this */
	if ((wp = mousewindow(ypos)) == NULL || wp != lastwp)
		return(FALSE);

	/* if we are on the line with the point, adjust for extended lines */
	if (wp == curwp && (lp = mouseline(wp, ypos)) == curwp->w_dotp)
		xpos += lbound;

	/* if we aren't off the end of the text, set mark $hilight+1 */
	if ((lp=mouseline(wp, ypos)) != NULL) {
		wp->w_markp[hilite+1] = lp;
		wp->w_marko[hilite+1] = mouseoffset(wp, lp, xpos);
	}

	return(TRUE);
}

/*	mouse-region-down:	mouse region operations

	nclicks = 0:	move cursor to mouse
			if hiliting then
				hilite set-mark
			set-mark

		  1:	move cursor to mouse
			kill-region
*/

int PASCAL NEAR mregdown(f, n)

int f,n;	/* prefix flag and argument */

{
	register EWINDOW *wp;
	register EWINDOW *lastwp;
	register LINE	*lp;
	ESCREEN *sp;
	char scr_name[12];		/* constructed temp screen name */
	static int temp_count = 0;	/* next temp screen number! */

	/* make sure we are on the proper screen */
	mouse_screen();

	/* adjust position by screen offset */
	ypos -= term.t_roworg;
	xpos -= term.t_colorg;

	/* if anything has changed, reset the click count */
	if (((lastmcmd != MREGUP) && (lastmcmd != MREGDWN))
	    || lastypos != ypos || lastxpos != xpos)
		nclicks = 0;
	++nclicks;
	lastwp = mousewindow(lastypos);		/* remember last window */

	/* reset the last position */
	lastypos = ypos;
	lastxpos = xpos;
	lastmcmd = MREGDWN;

#if	!WINDOW_MSWIN
	/* if we are in the upper left corner, create a new window */
	if (xpos == 0 && ypos == 0) {

		/* get the name of the screen to create */
		strcpy(scr_name, "SCREEN");
		strcat(scr_name, int_asc(temp_count++));
		while (lookup_screen(scr_name) != (ESCREEN *)NULL) {
			strcpy(scr_name, "SCREEN");
			strcat(scr_name, int_asc(temp_count++));
		}

		/* save the current dot position in the buffer info
		   so the new screen will start there! */
		curbp->b_dotp = curwp->w_dotp;
		curbp->b_doto = curwp->w_doto;

		/* screen does not exist, create it */
		sp = init_screen(scr_name, curbp);

		/* and make this screen current */
		return(select_screen(sp, TRUE));
	}
#endif

	/* if we move the mouse off the windows, don't move anything */
	if ((wp=mousewindow(ypos)) == NULL)
		return(FALSE);

	/* if we are on the line with the point, adjust for extended lines */
	if (wp == curwp && (lp = mouseline(wp, ypos)) == curwp->w_dotp)
		xpos += lbound;

	/* make the window the mouse points to current */
	curwp = wp;
	curbp = wp->w_bufp;

	/* if we changed windows, update the modelines */
	if (wp != lastwp)
		upmode();

	/* if we aren't off the end of the text, move the point to the mouse */
	if ((lp=mouseline(wp, ypos)) != NULL) {
		curwp->w_dotp = lp;
		curwp->w_doto = mouseoffset(wp, lp, xpos);
	}

	/* perform the region function */
	if (nclicks == 1) {
		if (hilite < NMARKS) {
			curwp->w_markp[hilite+1] = NULL;
			curwp->w_marko[hilite+1] = 0;
			curwp->w_markp[hilite] = curwp->w_dotp;
			curwp->w_marko[hilite] = curwp->w_doto;
		}
		return(setmark(FALSE, 0));
	} else {
		lastflag &= ~CFKILL;
		return(killregion(FALSE, 0));
	}
}

/*	mouse-region-up:	mouse region operations

	If the corresponding downclick was on a modeline, then we
	wish to delete the indicated window. Otherwise we are using
	this button to copy/paste.

	nclicks = 0:	move cursor to mouse
			copy-region

		  1:	move cursor to mouse
			yank

		  3:	reset nclicks to 0
*/

int PASCAL NEAR mregup(f, n)

int f,n;	/* prefix flag and argument */

{
	register EWINDOW *wp;
	register EWINDOW *lastwp;
	register LINE *lp;
	register ESCREEN *sp;		/* ptr to screen to delete */
	register int lastmodeline;	/* was the dowbclick on a modeline? */
	register int lastcmdline;	/* was the downclick on the command line? */

	/* adjust position by screen offset */
	ypos -= term.t_roworg;
	xpos -= term.t_colorg;

	/* if anything has changed, reset the click count */
	if (((lastmcmd != MREGUP) && (lastmcmd != MREGDWN))
	    || lastypos != ypos || lastxpos != xpos)
		nclicks = 0;
	++nclicks;
	lastwp = mousewindow(lastypos);		/* remember last window */

#if	!WINDOW_MSWIN
	/* if the down click was in the upper left corner...
	   then we are moving a just created screen */
	if (lastypos == 0 && lastxpos == 0) {
		new_row_org(TRUE, ypos + term.t_roworg);
		new_col_org(TRUE, xpos + term.t_colorg);
#if	WINDOW_TEXT
		refresh_screen(first_screen);
#endif
		return(TRUE);
	}
#endif

	/* Just where was the last click? */
	lastmodeline = ismodeline(lastwp, lastypos);
	lastcmdline = (lastypos == term.t_nrow);

	/* reset the last position */
	lastypos = ypos;
	lastxpos = xpos;
	lastmcmd = MREGUP;

	/* if we started on a modeline.... */
	if (lastmodeline)
		return(delwind(TRUE, 0));

	/* if we are on a command line */
	if (lastcmdline) {
		if (ypos != term.t_nrow)

			/* ABORT ABORT ABORT screen deletion */
			return(TRUE);
		else {

			/* delete the screen last screen means exiting */
			if (first_screen->s_next_screen == (ESCREEN *)NULL)
				return(quit(FALSE, 0));

			/* bring the second last screen to front*/
			sp = first_screen;
			select_screen(sp->s_next_screen, FALSE);

			/* and dump the front screen */
			first_screen->s_next_screen = sp->s_next_screen;
			free_screen(sp);
#if	WINDOW_TEXT
			refresh_screen(first_screen);
#endif
			return(TRUE);
		}
	}

	/* if we move the mouse off the windows, don't move anything */
	if ((wp=mousewindow(ypos)) == NULL)
		return(FALSE);

	/* if we are on the line with the point, adjust for extended lines */
	if (wp == curwp && (lp = mouseline(wp, ypos)) == curwp->w_dotp)
		xpos += lbound;

	/* make the window the mouse points to current */
	curwp = wp;
	curbp = wp->w_bufp;

	/* if we aren't off the end of the text, move the point to the mouse */
	if ((lp=mouseline(wp, ypos)) != NULL && nclicks < 3) {
		curwp->w_dotp = lp;
		curwp->w_doto = mouseoffset(wp, lp, xpos);
	}

	/* if we changed windows, update the modelines, abort the new op */
	if (wp != lastwp) {
		upmode();
		return(TRUE);
	}

	/* perform the region function */
	if (nclicks == 1) {
		return(copyregion(FALSE, 0));
	} else if (nclicks == 2) {
		return(yank(FALSE, 1));
	} else {
		nclicks = 0;
		return(TRUE);
	}
}

/*
 * Move mouse button, up. The up click must be
 * in the text region of a window. If the old click was in a
 * mode line then the mode line moves to the row of the
 * up click. If the old click is not in a mode line then the
 * window scrolls. The code in this function is just
 * too complex!
 */
int PASCAL NEAR movemu(f, n)

int f,n;	/* prefix flag and argument */

{
	register EWINDOW *lastwp;
	register EWINDOW *wp;
	register int lastmodeline;	/* was the downclick on a modeline? */
	register int deltay;
	register int deltax;
#if	WINDOW_TEXT
	register int redraw_needed;	/* is a screen redraw required */
#endif

	/* adjust position by screen offset */
	ypos -= term.t_roworg;
	xpos -= term.t_colorg;

	/* no movement... fail the command */
	if (lastypos==ypos && lastxpos==xpos)
		return(FALSE);

#if	!WINDOW_MSWIN
	/* if the down click was in the bottom right corner...
	   then we are resizing */
	if (lastypos == term.t_nrow && lastxpos + 1 == term.t_ncol) {
#if	WINDOW_TEXT
		if (xpos < term.t_ncol || ypos < term.t_nrow)
			redraw_needed = TRUE;
		else
			redraw_needed = FALSE;
#endif
		newwidth(TRUE, xpos + 1);
		newsize(TRUE, ypos + 1);
#if	WINDOW_TEXT
		if (redraw_needed) {
			refresh_screen(first_screen);
		}
#endif
		return(TRUE);
	}

	/* if the down click was in the upper left corner...
	   then we are moving the screen */
	if (lastypos == 0 && lastxpos == 0) {
		new_row_org(TRUE, ypos + term.t_roworg);
		new_col_org(TRUE, xpos + term.t_colorg);
#if	WINDOW_TEXT
		refresh_screen(first_screen);
#endif
		return(TRUE);
	}
#endif

	/* if the down click was not in a window.. fail the command
	   (for example, if we click on the command line) */
	if ((lastwp=mousewindow(lastypos)) == NULL)
		return(FALSE);

	/* did we down click on a modeline? */
	lastmodeline = ismodeline(lastwp, lastypos);

	/* are we not in a window? fail it then */
	if ((wp=mousewindow(ypos)) == NULL)
		return(FALSE);

	/* how far did we move? */
	deltay = lastypos-ypos;
	deltax = lastxpos-xpos;
	lastypos = ypos;
	lastxpos = xpos;

	/* if we started on a modeline.... */
	if (lastmodeline) {

		/* move the window horizontally */
		if (deltax != 0 && (diagflag || deltay == 0)) {
			lastwp->w_fcol += deltax;
			if (lastwp->w_fcol < 0)
				lastwp->w_fcol = 0;
			lastwp->w_flag |= WFMODE|WFHARD;
			if (deltay == 0)
				return(TRUE);
		}

		/* don't allow the bottom modeline to move */
		if (lastwp->w_wndp == NULL)
			return(FALSE);

		/* shrink the current window */
		if (deltay > 0) {
			if (lastwp != wp)
				return(FALSE);
			curwp = wp;
			curbp = wp->w_bufp;
			return(shrinkwind(TRUE, deltay));
		}

		/* or grow it */
		if (deltay < 0) {
			if (wp != lastwp->w_wndp)
				return(FALSE);
			curwp = lastwp;
			curbp = lastwp->w_bufp;
			return(enlargewind(TRUE, -deltay));
		}
	}

	/* did we up click in a modeline? fail it them */
	if (ismodeline(wp, ypos) != FALSE)
		return(FALSE);

	/* we can not move outside the current window */
	if (lastwp != wp)
		return(FALSE);

	/* move horizontally as well? */
	if (deltax != 0 && (diagflag || deltay == 0)) {
		wp->w_fcol += deltax;
		if (wp->w_fcol < 0)
			wp->w_fcol = 0;
		wp->w_flag |= WFMODE;
	}

	/* and move the screen */
	return(mvdnwind(TRUE, deltay));
}

/*
 * Return a pointer to the EWINDOW structure
 * for the window in which "row" is located, or NULL
 * if "row" isn't in any window. The mode line is
 * considered to be part of the window.
 */

EWINDOW *PASCAL NEAR mousewindow(row)

register int	row;

{
	register EWINDOW *wp;

	/* must be a positiove row! */
	if (row < 0)
		return(NULL);

	/* step through each window on the active screen */
	wp = wheadp;
	while (wp != NULL) {

		/* is this row within the current window? */
		if (row < wp->w_ntrows+1)
			return(wp);

		/* advance to the next window */
		row -= wp->w_ntrows+1;
		wp = wp->w_wndp;
	}
	return(NULL);
}

/*
 * The row "row" is a row within the window
 * whose EWINDOW structure is pointed to by the "wp"
 * argument. Find the associated line, and return a pointer
 * to it. Return NULL if the mouse is on the mode line,
 * or if the mouse is pointed off the end of the
 * text in the buffer.
 */

LINE *PASCAL NEAR mouseline(wp, row)

register EWINDOW *wp;
register int	row;

{
	register LINE	*lp;

	row -= wp->w_toprow;
	if (row >= wp->w_ntrows + (modeflag ? 0 : 1))
		return(NULL);
	lp = wp->w_linep;
	while (row--) {
		if (lp == wp->w_bufp->b_linep)	/* Hit the end.		*/
			return(NULL);
		lp = lforw(lp);
	}
	return(lp);
}

/*
 * Return the best character offset to use
 * to describe column "col", as viewed from the line whose
 * LINE structure is pointed to by "lp".
 */

int PASCAL NEAR mouseoffset(wp, lp, col)

register EWINDOW *wp;
register LINE	*lp;
register int	col;

{
	register int	c;
	register int	offset;
	register int	curcol;
	register int	newcol;

	offset = 0;
	curcol = 0;
	col += wp->w_fcol;	/* adjust for extended lines */
	while (offset != lused(lp)) {
		newcol = curcol;
		c = lgetc(lp, offset);
#if	UTF8
		if (is_multibyte_utf8(c)) {
			unsigned int wc;

			offset += utf8_to_unicode(ltext(lp), offset, lused(lp) - offset, &wc) - 1;
		} else
#endif
		if (c == '\t' && tabsize > 0)
			newcol += -(newcol % tabsize) + (tabsize - 1);
		else if (disphigh && c > 0x7f) {
			newcol += 2;
			c -= 0x80;
		} else if (c < 0x20 || c == 0x7f)	/* ISCTRL */
			++newcol;

		++newcol;
		if (newcol > col)
			break;
		curcol = newcol;
		++offset;
	}
	return(offset);
}

VOID PASCAL NEAR mouse_screen()

{
	register ESCREEN *screen_ptr;	/* screen to test mouse in */

	/* if we move the mouse off the windows, check for other windows */
	if ((ypos < term.t_roworg) || (xpos < term.t_colorg) ||
	    (ypos > term.t_roworg + term.t_nrow) ||
	    (xpos >= term.t_colorg + term.t_ncol)) {

		/* scan through the other windows */
		screen_ptr = first_screen->s_next_screen;
		while (screen_ptr != (ESCREEN *)NULL) {

			/* is the mouse in this window? */
			if ((ypos >= screen_ptr->s_roworg) &&
			    (xpos >= screen_ptr->s_colorg) &&
			    (ypos <= screen_ptr->s_roworg + screen_ptr->s_nrow) &&
			    (xpos <= screen_ptr->s_colorg + screen_ptr->s_ncol)) {

				/* select this screen */
				select_screen(screen_ptr, FALSE);
				lastxpos = -1;
				lastypos = -1;
				break;
			}

			/* on to the next screen */
			screen_ptr = screen_ptr->s_next_screen;
		}
	}
}

int PASCAL NEAR ismodeline(wp, row)

EWINDOW *wp;
int row;

{
	/* not on a legal window, say we aren't on a mode line either */
	if (wp == (EWINDOW *)NULL)
		return(FALSE);

	/* are we on a modeline? */
	if (row == wp->w_toprow+wp->w_ntrows && modeflag)
		return(TRUE);

	/* no */
	return(FALSE);
}

/* The mouse has been used to resize the physical window. Now we need to
   let emacs know about the newsize, and have him force a re-draw
*/

int PASCAL NEAR resizm(f, n)

int f, n;	/* these are ignored... we get the new size info from
		   the mouse driver */
{
#if	WINDOW_TEXT
	register int redraw_needed;	/* is a screen redraw required */
#endif

	/* make sure we are on the proper screen */
	mouse_screen();

	/* adjust position by screen offset */
	ypos -= term.t_roworg;
	xpos -= term.t_colorg;

#if	WINDOW_TEXT
		if (xpos < term.t_ncol || ypos < term.t_nrow)
			redraw_needed = TRUE;
		else
			redraw_needed = FALSE;
#endif

	/* change to the new size */
	newwidth(TRUE, xpos + 1);
	newsize(TRUE, ypos + 1);

#if	WINDOW_TEXT
	if (redraw_needed) {
		refresh_screen(first_screen);
	}
#endif
	return(TRUE);
}

int PASCAL NEAR resizm2(f, n)

int f, n;	/* these are ignored... we get the new size info from
		   the mouse driver */
{
	/* make sure we are on the proper screen */
	mouse_screen();

	/* change to the new size */
	newwidth(TRUE, xpos + 1);
	newsize(TRUE, ypos + 1);

#if	WINDOW_TEXT
	refresh_screen(first_screen);
#endif
	return(TRUE);
}

#else
VOID mousehello()
{
}
#endif

/*	SCREEN.C:	Screen manipulation commands
			for MicroEMACS 4.00
			written by Daniel Lawrence
*/

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include        "edef.h"
#include	"elang.h"

#if	0
dumpscreens(msg)

char *msg;

{
	ESCREEN *sp;

	printf("<%s>\n", msg);
	sp = first_screen;
	while (sp != (ESCREEN *)NULL) {
		printf("%lu - [%s] %d/%d to %d/%d \n", sp, sp->s_screen_name,
		sp->s_roworg, sp->s_colorg, sp->s_nrow, sp->s_ncol);
		sp = sp->s_next_screen;
	}
	printf("   0     -   [EOL]\n");
	tgetc();
}
#endif

#if	WINDOW_TEXT
/* Redraw given screen and all screens behind it */

VOID PASCAL NEAR refresh_screen(sp)

ESCREEN *sp;	/* screen image to refresh */

{
	/* if we are suppressing redraws */
	if (gflags & GFSDRAW)
		return;

	/* at end of list, do nothing */
	if (sp == (ESCREEN *)NULL)
		return;

	/* if first refresh, erase the page */
	if (sp == first_screen) {
		(*term.t_clrdesk)();
		if (sp->s_next_screen == (ESCREEN *)NULL)
			sgarbf = TRUE;
	}

	/* if there are others below, defer to them first */
	if (sp->s_next_screen)
		refresh_screen(sp->s_next_screen);

	select_screen(sp, FALSE);
	update(TRUE);
}
#endif

/*	This command takes the last window in the linked window list,
	which is visibly rearmost, and brings it to front. It is bound
	to A-N on machines with an ALT key
*/

int PASCAL NEAR cycle_screens(f, n)

int f,n;	/* prefix flag and argument */

{
	ESCREEN *sp;		/* ptr to screen to switch to */

	/* find the last screen */
	sp = first_screen;
	while (sp->s_next_screen != (ESCREEN *)NULL)
		sp = sp->s_next_screen;

	/* and make this screen current */
	return(select_screen(sp, TRUE));
}

int PASCAL NEAR find_screen(f, n)

int f,n;	/* prefix flag and argument */

{
	char scr_name[NSTRING];	/* buffer to hold screen name */
	ESCREEN *sp;		/* ptr to screen to switch to */
	int result;

	/* get the name of the screen to switch to */
	if ((result = mlreply(TEXT242, scr_name, NSTRING)) != TRUE) {
	    /* "Find Screen: " */
            return result;
        }
	sp = lookup_screen(scr_name);

	if (sp == (ESCREEN *)NULL) {

		/* save the current dot position in the buffer info
		   so the new screen will start there! */
		curbp->b_dotp = curwp->w_dotp;
		curbp->b_doto = curwp->w_doto;

		/* screen does not exist, create it */
		sp = init_screen(scr_name, curbp);
	}

	/* and make this screen current */
	return(select_screen(sp, TRUE));
}

VOID PASCAL NEAR free_screen(sp)	/* free all resouces associated with a screen */

ESCREEN *sp;	/* screen to dump */

{
	register int cmark;	/* mark ordinal index */
	register EWINDOW *wp;	/* ptr to window to free */
	register EWINDOW *tp;	/* temp window pointer */

	/* first, free the screen's windows */
	wp = sp->s_first_window;
	while (wp) {
		if (--wp->w_bufp->b_nwnd == 0) {
			wp->w_bufp->b_dotp  = wp->w_dotp;
			wp->w_bufp->b_doto  = wp->w_doto;
			for (cmark = 0; cmark < NMARKS; cmark++) {
				wp->w_bufp->b_markp[cmark] = wp->w_markp[cmark];
				wp->w_bufp->b_marko[cmark] = wp->w_marko[cmark];
			}
			wp->w_bufp->b_fcol  = wp->w_fcol;
		}

		/* on to the next window, free this one */
		tp = wp->w_wndp;
		free((char *) wp);
		wp = tp;
	}

#if	WINDOW_MSWIN
	term.t_delscr(sp);
#endif
	/* and now, free the screen struct itself */
	free(sp->s_screen_name);
	free((char *) sp);
}

VOID PASCAL NEAR unlist_screen(sp)

ESCREEN *sp;         /* screen to remove from the list */
{
	ESCREEN *last_scr;	/* screen previous to one to delete */

	last_scr = first_screen;
	while (last_scr) {
		if (last_scr->s_next_screen == sp)
			break;
		last_scr = last_scr->s_next_screen;
	}
	last_scr->s_next_screen = sp->s_next_screen;
}

int PASCAL NEAR delete_screen(f, n)

int f,n;	/* prefix flag and argument */

{
	char scr_name[NSTRING];	/* buffer to hold screen name */
	ESCREEN *sp;		/* ptr to screen to switch to */
	int result;

	/* get the name of the screen to delete */
	if ((result = mlreply(TEXT243, scr_name, NSTRING)) != TRUE) {
	    /* "Delete Screen: " */
            return result;
	} 
	sp = lookup_screen(scr_name);

	/* make sure it exists... */
	if (sp == (ESCREEN *)NULL) {
		mlwrite(TEXT240);   /* "[No such screen]" */
		return(FALSE);
	}

	/* it can't be current... */
	if (sp == first_screen) {
		mlwrite(TEXT241);   /* "%%Can't delete current screen" */
		return(FALSE);
	}

	unlist_screen(sp);
	free_screen(sp);
#if	WINDOW_TEXT
	refresh_screen(first_screen);
#endif
	return(TRUE);
}

/* this function initializes a new screen.... */

ESCREEN *PASCAL NEAR init_screen(scr_name, scr_buf)

char *scr_name;		/* screen name */
BUFFER *scr_buf;	/* buffer to place in first window of screen */

{
	int cmark;		/* current mark to initialize */
	ESCREEN *sp;		/* pointer to allocated screen */
	ESCREEN *last_sp;	/* pointer to last screen */
	EWINDOW *wp;		/* ptr to first window of new screen */

	/* allocate memory for this screen */
	sp = (ESCREEN *)room(sizeof(ESCREEN));
	if (sp == (ESCREEN *)NULL)
		return(sp);

	/* set up this new screens fields! */
	sp->s_next_screen = (ESCREEN *)NULL;
	sp->s_screen_name = copystr(scr_name);
#if     WINDOW_MSWIN
	if (term.t_newscr (sp) != TRUE) {       /* failed */
	    free ((void *)sp);
	    return ((ESCREEN *)NULL);
	}
	/* ... in MSWIN, the s_nrow/ncol etc... values are kept up to
	   date by vtinitscr; besides, term entries may actually match
	   the first_screen instead of the new screen */
	term.t_roworg = sp->s_roworg;
	term.t_colorg = sp->s_colorg;
	term.t_nrow = sp->s_nrow;
	term.t_ncol = sp->s_ncol;
#else
	sp->s_roworg = term.t_roworg;
	sp->s_colorg = term.t_colorg;
	sp->s_nrow = term.t_nrow;
	sp->s_ncol = term.t_ncol;
#endif

	/* allocate its first window */
	wp = (EWINDOW *)room(sizeof(EWINDOW));
	if (wp == (EWINDOW *)NULL) {
		free((char *)sp);
		return((ESCREEN *)NULL);
	}
	sp->s_first_window = sp->s_cur_window = wp;

	/* and setup the windows info */
	wp->w_wndp = NULL;
	wp->w_bufp = scr_buf;
	scr_buf->b_nwnd += 1;	
	wp->w_linep = scr_buf->b_linep;

	/* position us at the buffers dot */
	wp->w_dotp  = scr_buf->b_dotp;
	wp->w_doto  = scr_buf->b_doto;

	/* set all the marks to UNSET */
	for (cmark = 0; cmark < NMARKS; cmark++) {
		wp->w_markp[cmark] = NULL;
		wp->w_marko[cmark] = 0;
	}
	wp->w_toprow = 0;
#if	COLOR
	/* initalize colors to global defaults */
	wp->w_fcolor = gfcolor;
	wp->w_bcolor = gbcolor;
#endif
	wp->w_fcol = 0;
#if WINDOW_MSWIN
	wp->w_ntrows = sp->s_nrow-1;
#else
	wp->w_ntrows = term.t_nrow-1;		/* "-1" for mode line.	*/
#endif
	wp->w_force = 0;
	wp->w_flag  = WFMODE|WFHARD;		/* Full.		*/

	/* first screen? */
	if (first_screen == (ESCREEN *)NULL) {
		first_screen = sp;
		return(sp);
	}

	/* insert it at the tail of the screen list */
	last_sp = first_screen;
	while (last_sp->s_next_screen != (ESCREEN *)NULL)
		last_sp = last_sp->s_next_screen;
	last_sp->s_next_screen = sp;

	/* and return the new screen pointer */
	return(sp);
}

ESCREEN *PASCAL NEAR lookup_screen(scr_name)

char *scr_name;		/* named screen to find */

{
	ESCREEN *result;

	/* scan the screen list */
	result = first_screen;
	while (result) {

		/* if this is it, return its handle! */
		if (strcmp(scr_name, result->s_screen_name) == 0)
			return(result);

		/* on to the next screen */
		result = result->s_next_screen;
	}

	/* we didn't find it..... */
	return((ESCREEN *)NULL);
}

int PASCAL NEAR select_screen(sp, announce)

ESCREEN *sp;	/* ptr to screen to switch to */
int announce;	/* announce the selection? */

{
	EWINDOW *temp_wp;	/* backup of current window ptr (curwp) */
	ESCREEN *temp_screen;	/* temp ptr into screen list */

	/* make sure there is something here to set to! */
	if (sp == (ESCREEN *)NULL)
		return(FALSE);

	/* nothing to do, it is already current */
	if (sp == first_screen)
		return(TRUE);

	/* deselect the current window */
#if     WINDOW_MSWIN
        curwp->w_flag |= WFMODE;
#else
	temp_wp = curwp;
	curwp = (EWINDOW *)NULL;
	modeline(temp_wp);
	updupd(TRUE);
	curwp = temp_wp;
#endif

	/* save the current screens concept of current window */
	first_screen->s_cur_window = curwp;
#if     WINDOW_MSWIN
        /* in MSWIN, the term entries may (but not always) already
	   reflect the new screen's size and the s_n... stuff is always
	   kept up to date by vtinitscr */
	vtscreen (sp);
#else
	first_screen->s_roworg = term.t_roworg;
	first_screen->s_colorg = term.t_colorg;
	first_screen->s_nrow = term.t_nrow;
	first_screen->s_ncol = term.t_ncol;
#endif

	/* re-order the screen list */
	temp_screen = first_screen;
	while (temp_screen->s_next_screen != (ESCREEN *)NULL) {
		if (temp_screen->s_next_screen == sp) {
			temp_screen->s_next_screen = sp->s_next_screen;
			break;
		}
		temp_screen = temp_screen->s_next_screen;
	}
	sp->s_next_screen = first_screen;
	first_screen = sp;

	/* reset the current screen, window and buffer */
	wheadp = first_screen->s_first_window;
	curwp = first_screen->s_cur_window;
	curbp = curwp->w_bufp;

	/* let the display driver know we need a full screen update */
#if     WINDOW_MSWIN
        term.t_topscr (first_screen);
        curwp->w_flag |= WFMODE;
        update(FALSE);
#else
	update_size();
	upwind();
#endif
	if (announce) {
		mlwrite(TEXT225, first_screen->s_screen_name);
/*			"[Switched to screen %s]" */
	}
	return(TRUE);
}

/*	Build and popup a buffer containing the list of all screens.
	Bound to "A-B".
*/

int PASCAL NEAR list_screens(f, n)

int f,n;	/* prefix flag and argument */

{
	register int status;	/* stutus return */

	if ((status = screenlist(f)) != TRUE)
		return(status);
	return(wpopup(slistp));
}


/*
 * This routine rebuilds the
 * text in the special secret buffer
 * that holds the screen list. It is called
 * by the list screens command. Return TRUE
 * if everything works. Return FALSE if there
 * is an error (if there is no memory). Iflag
 * indicates whether to list hidden screens.
 */
int PASCAL NEAR screenlist(iflag)

int iflag;	/* list hidden screen flag */

{
	ESCREEN *sp;		/* ptr to current screen to list */
	EWINDOW *wp;		/* ptr into current screens window list */
	int status;		/* return status from functions */
	char line[NSTRING];	/* buffer to construct list lines */
	char bname[NSTRING];	/* name of next buffer */

	/* mark this buffer as unchanged so... */
	slistp->b_flag &= ~BFCHG;

	/* we can dump it's old contents without complaint */
	if ((status = bclear(slistp)) != TRUE)
		return(status);

	/* there is no file connected with this buffer */
	strcpy(slistp->b_fname, "");

	/* construct the header of this list */
	if (addline(slistp, "Screen         Buffers") == FALSE
	 || addline(slistp, "------         -------") == FALSE)
		return(FALSE);

	/* starting from the first screen */
	sp = first_screen;

	/* scan all the screens */
	while (sp) {

		/* construct the screen name */
		strcpy(line, sp->s_screen_name);
		strcat(line, "                ");
		line[15] = 0;

		/* list this screens windows's buffer names */
		wp = sp->s_first_window;
		while (wp) {

			/* grab this window's buffer name */
			strcpy(bname, wp->w_bufp->b_bname);

			/* handle full lines */
			if (strlen(line) + strlen(bname) + 1 > 78) {
				if (addline(slistp, line) == FALSE)
					return(FALSE);
				strcpy(line, "               ");
			}

			/* append this buffer name */
			if (strlen(line) > 15)
				strcat(line, " ");
			strcat(line, bname);

			/* on to the next window */
			wp = wp->w_wndp;
		}

		/* and add the line to the buffer */
		if (addline(slistp, line) == FALSE)
			return(FALSE);

		/* on to the next screen */
		sp = sp->s_next_screen;
	}

	/* all constructed! */
	return(TRUE);
}

/* rename_screen:    change the current screen's name	*/

int PASCAL NEAR rename_screen(f, n)

int f, n;	/* default number and arguments */

{
	char scr_name[NSTRING];  /* buffer to hold screen name */
	int result;

	/* get the new name of the screen */
	if ((result = mlreply(TEXT335, scr_name, NSTRING)) != TRUE) {
/*			      "Change screen name to: " */
		return(result);
	} 

	if (lookup_screen(scr_name) != (ESCREEN*)NULL) {
		mlwrite(TEXT336);
/*			"[Screen name already in use]" */
		return(FALSE);
	}

	/* replace the old screen name with the new */
	free(first_screen->s_screen_name);
	first_screen->s_screen_name = copystr(scr_name);
#if	WINDOW_MSWIN
	SetWindowText(first_screen->s_drvhandle, scr_name);
#endif
	return(TRUE);

}



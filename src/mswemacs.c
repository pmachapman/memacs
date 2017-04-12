/* The routines in this file provide extra emacs functions available
   under the Microsoft Windows environment on an IBM-PC or compatible
   computer. The following functions are supplied: cutregion,
   clipregion, insertclip and helpengine.
   Also implemented here is the support for scroll bars

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

#include    "estruct.h"
#include    <stdio.h>
#include    "eproto.h"
#include    "edef.h"
#include    "elang.h"

#include    "mswin.h"

#include    "mswmenu.h"
/* to invoke the commands corresponding to scroll bar actions */

#define MAXSCROLL 32767 /* maximum position for scrollbars */

static HANDLE   hClipData = NULL;   /* used by insertclip and
				       ClipboardCleanup */

/* CopyToClipboard: internal function to copy region to clipboard */
/* ===============                                                */

static BOOL PASCAL CopyToClipboard (REGION *Region)
{
    long    Size = 0L;
    HANDLE  hData;
#if WINDOW_MSWIN32
    char    *Data;
#else
    char huge *Data;
#endif
    BOOL    Result = TRUE;
    register LINE *lp;
    register int Offset;
    register int lcnt;       /* used to reduce longop() overhead */
    
    /*-figure out the size of the clipboard data (end of lines have to
       be turned into CR-LF) */
    Size = Region->r_size;
    if (curwp->w_dotp != curwp->w_markp[0]) {   /* multiple lines */
        lp = Region->r_linep;
        do {
            ++Size;
	    lp = lforw(lp);
	} while ((lp != curwp->w_dotp) && (lp != curwp->w_markp[0]));
    }
    if (Size == 0L) return TRUE;

    /*-copy the buffer data into a block of global memory */
    if (hData = GlobalAlloc (GMEM_MOVEABLE, Size + 1)) {
        if (!(Data = GlobalLock (hData))) goto NoClipboardMemory;
	lp = Region->r_linep;
	Offset = Region->r_offset;
	lcnt = 0;
	while (Size-- > 0) {
	    if (Offset != lused(lp)) {    /* middle of line */
	        *Data++ = lgetc(lp, Offset);
	        ++Offset;
	    }
	    else {                          /* end of line */
	        *Data++ = '\r';
	        *Data++ = '\n';
	        Size--;
	        lp = lforw(lp);
	        Offset = 0;
	        if (--lcnt < 0) {
		    longop (TRUE);
		    lcnt = 10;  /* reduce longop calls overhead */
		}
	    }
	}
	*Data = '\0';
	/*-pass the text to the clipboard */
	GlobalUnlock (hData);
	if (OpenClipboard (hFrameWnd)) {
	    if (EmptyClipboard ()) {
	        SetClipboardData (CF_TEXT, hData);
	    }
	    else Result = FALSE;
	    CloseClipboard ();
	}
	else Result = FALSE;
	if (Result == FALSE) GlobalFree (hData);
    }
    else {
NoClipboardMemory:
	mlabort (TEXT94);   /* out of memory */
        Result = FALSE;
    }
    return Result;
} /* CopyToClipboard */

/* cutregion:   move the current region to the clipboard */
/* =========                                             */

PASCAL cutregion (int f, int n)
{
    REGION  Region;
    int     Result;

    /*-don't allow command if read-only mode */
    if (curbp->b_mode & MDVIEW) return rdonly();

    if ((Result = getregion (&Region)) != TRUE) return Result;

    if ((Result = CopyToClipboard (&Region)) != TRUE) return Result;
    curwp->w_dotp = Region.r_linep;
    curwp->w_doto = Region.r_offset;
    return ldelete (Region.r_size, FALSE);
} /* cutregion */

/* clipregion:  copy the current region into the clipboard */
/* ==========                                              */

PASCAL clipregion (int f, int n)
{
    REGION  Region;
    int     Result;
    
    if ((Result = getregion (&Region)) != TRUE) return Result;

    return CopyToClipboard (&Region);
} /* clipregion */

/* insertclip:  insert the clipboard contents at dot */
/* ==========                                        */

PASCAL insertclip (int f, int n)
{
    BOOL    Result = TRUE;
    char    *Text, *TextHead;
    short int curoff;
    LINE    *curline;

    /*-don't allow command if read-only mode */
    if (curbp->b_mode & MDVIEW) return rdonly();

    if (OpenClipboard (hFrameWnd)) {
	if ((hClipData = GetClipboardData (CF_TEXT)) != NULL) {
	    /* Save the local pointers to hold global "." */
	    if (yankflag == SRBEGIN) {
		/* Find the *previous* line, since the line we are on
		   may disappear due to re-allocation.  This works even
		   if we are on the first line of the file. */
		curline = lback(curwp->w_dotp);
		curoff = curwp->w_doto;
	    }
	    if ((TextHead = GlobalLock (hClipData)) != NULL) {
		while (n--) {
		    Text = TextHead;
		    while (*Text != '\0') {
			if (*Text == '\n') {
			    if (lnewline () == FALSE) {
				Result = FALSE;
				goto bail_out;
			    }
			}
			else {
			    if (*Text != '\r') if (linsert (1, *Text) == FALSE) {
				Result = FALSE;
				goto bail_out;
			    }
			}
			++Text;
		    }
		}
bail_out:
                GlobalUnlock (hClipData);
                hClipData = NULL;   /* for ClipboardCleanup */
                /* If requested, set global "." back to the beginning of
		   the yanked text. */
		if (yankflag == SRBEGIN) {
		    curwp->w_dotp = lforw(curline);
		    curwp->w_doto = curoff;
		}
	    }
	}
	else Result = FALSE;
	CloseClipboard ();
    }
    else Result = FALSE;
    return Result;
} /* insertclip */

/* ClipboardCleanup:    to be called if the user aborts during a longop */
/* ================                                                     */

void FAR PASCAL ClipboardCleanup (void)
{
    if (hClipData) {
        GlobalUnlock (hClipData);
        CloseClipboard ();
    }
} /* ClipboardCleanup */

/* helpengine:  invoke the MS-Windows help engine */
/* ==========                                     */

PASCAL helpengine (int f, int n)
{
    char    OldHelpFile [NFILEN];
    char    HelpKey [NLINE];
    BOOL    Result;

    strcpy (OldHelpFile, HelpEngineFile);
    SetWorkingDir ();
    if ((Result = FILENAMEREPLY (TEXT307, HelpEngineFile, NFILEN)) != TRUE) return Result;
        /* "Help file: " */
    if (HelpEngineFile[0] == '\0') {
        strcpy (HelpEngineFile, OldHelpFile);
        return FALSE;
    }
    else {
        Result = mlreply (TEXT308, HelpKey, NLINE);
	if ((Result != TRUE) && (Result != FALSE)) return Result;
	    /* "Help key: " */
	if (HelpKey[0] == '\0') {
	    WinHelp (hFrameWnd, HelpEngineFile, HELP_INDEX, NULL);
	}
	else {
	    WinHelp (hFrameWnd, HelpEngineFile, HELP_KEY,
                     (DWORD)(LPSTR)&HelpKey[0]);
	}
    }
    return TRUE;
} /* helpengine */

/* minimizescreen:  turn the current screen into an icon */
/* ==============                                        */

PASCAL  minimizescreen (int f, int n)
{
    BOOL    nq;

    nq = notquiescent;
    notquiescent = 0;
    ShowWindow (first_screen->s_drvhandle, SW_MINIMIZE);
    notquiescent = nq;
    return TRUE;
} /* minimizescreen */

/* ForceMessage:    do a SendMessage, forcing quiescent mode */
/* ============                                              */

static PASCAL ForceMessage (HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
    BOOL    nq;

    nq = notquiescent;
    notquiescent = 0;
    SendMessage (hWnd, wMsg, wParam, lParam);
    notquiescent = nq;
} /* ForceMessage */

/* maximizescreen:  maximize the current screen */
/* ==============                               */

PASCAL  maximizescreen (int f, int n)
{
    ForceMessage (hMDIClientWnd, WM_MDIMAXIMIZE,
                  (UINT)first_screen->s_drvhandle, 0L);
    return TRUE;
} /* maximizescreen */

/* restorescreen:   restore the current screen from maximized/minimized state */
/* =============                                                              */

PASCAL  restorescreen (int f, int n)
{
    ForceMessage (hMDIClientWnd, WM_MDIRESTORE,
                  (UINT)first_screen->s_drvhandle, 0L);
    return TRUE;
} /* restorescreen */

/* tilescreens: tile the non-iconized screens */
/* ===========                                */

PASCAL  tilescreens (int f, int n)

/* without a numeric argument, tile horizontally. With a numeric argument
   of 1, tile vertically */
{
    if (f && (n == 1)) {
        ForceMessage (hMDIClientWnd, WM_MDITILE, MDITILE_HORIZONTAL, 0L);
    }
    else ForceMessage (hMDIClientWnd, WM_MDITILE, MDITILE_VERTICAL, 0L);
    return TRUE;
} /* tilescreens */

/* cascadescreens:  position the non-iconized screens in cascade */
/* ==============                                                */

PASCAL  cascadescreens (int f, int n)
{
    ForceMessage (hMDIClientWnd, WM_MDICASCADE, 0, 0L);
    return TRUE;
} /* cascadescreens */

/* ScrollMessage:   handle WM_HSCROLL and WM_VSCROLL */
/* =============                                     */
void FAR PASCAL ScrollMessage (HWND hWnd, UINT wMsg, WORD ScrlCode, int Pos)

{
    int     Delta;
    
    if (notquiescent) return;
    
    if (wMsg == WM_VSCROLL) {
        switch (ScrlCode) {
            case SB_LINEUP:
                mvupwind (FALSE, 1);
                break;
            case SB_LINEDOWN:
                mvdnwind (FALSE, 1);
                break;
            case SB_PAGEUP:
                backpage (FALSE, 1);
                break;
            case SB_PAGEDOWN:
                forwpage (FALSE, 1);
                break;
            case SB_THUMBTRACK:
            case SB_THUMBPOSITION:
                if (Win31API) {
                    if (ScrlCode == SB_THUMBPOSITION) return;
                }
                else {
                    if (ScrlCode == SB_THUMBTRACK) return;
                    /* there is something wrong with thumb tracking in
                       Windows 3.0 */
                }
                Delta = Pos - GetScrollPos (hWnd, SB_VERT);
                if (Delta) mvdnwind (TRUE, Delta);
                break;
            default:
                return;
        }
        curwp->w_flag |= WFMODE;
    }
    else {
        switch (ScrlCode) {
            case SB_LINEUP:
                Delta = -1;
                break;
            case SB_LINEDOWN:
                Delta = 1;
                break;
            case SB_PAGEUP:
                Delta = -term.t_ncol;
                break;
            case SB_PAGEDOWN:
                Delta = term.t_ncol;
                break;
            case SB_THUMBTRACK:
                Delta = Pos - GetScrollPos (hWnd, SB_HORZ);
                break;
            default:
                return;
        }
        curwp->w_fcol += Delta;
        if (curwp->w_fcol < 0) curwp->w_fcol = 0;
        if (curwp->w_doto < curwp->w_fcol) {
            /* reframe dot if it was left past the left of the screen */
            curwp->w_doto = min(curwp->w_fcol,lused(curwp->w_dotp));
        }
        if (curwp->w_doto > (curwp->w_fcol + term.t_ncol - 2)) {
            /* reframe dot if it was left past the right of the screen */
            curwp->w_doto = curwp->w_fcol + term.t_ncol - 2;
	}
        curwp->w_flag |= WFMODE | WFHARD;
    }
    if (in_check()) GenerateMenuSeq (IDM_NULLPROC);
        /* this ensures we go through the editloop(), updating the
           modeline display and running the cmdhook, among other things */
    ShowEmacsCaret (FALSE);
    update (TRUE);
    ShowEmacsCaret (TRUE);
} /* ScrollMessage */

/* ScrollBars:      shows/hides, enables/disables scroll bars for all screens */
/* ==========                                                                 */
void FAR PASCAL ScrollBars (void)

{
    static int VScroll = TRUE;
    static int HScroll = TRUE;
    static int Enabled = TRUE;
    int     Quiescence;
    SCREEN  *sp;

    if (vscrollbar) vscrollbar = TRUE;  /* normalize... */
    if (hscrollbar) hscrollbar = TRUE;
    Quiescence = (notquiescent == 0);

    for (sp = first_screen; sp != (SCREEN*)NULL; sp = sp->s_next_screen) {
        if (vscrollbar != VScroll) {
            ShowScrollBar ((HWND)sp->s_drvhandle, SB_VERT, vscrollbar);
        }
        if (hscrollbar != HScroll) {
            ShowScrollBar ((HWND)sp->s_drvhandle, SB_HORZ, hscrollbar);
        }
#if !WIN30SDK
        if ((Enabled != Quiescence) && Win31API && !TakingANap) {
            /* note: no disabling of scroll bars during naps (i.e. fence
               matching), to avoid annoying blinking */
            EnableScrollBar ((HWND)sp->s_drvhandle, SB_BOTH,
                             Quiescence ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
	}
#endif
    }

    if ((Enabled != Quiescence) && Win31API && !TakingANap) {
        Enabled = Quiescence;
    }
    VScroll = vscrollbar;
    HScroll = hscrollbar;
} /* ScrollBars */

/* updscrollbars:      updates the scroll bars for a screen */
/* =============                                            */
PASCAL  updscrollbars (SCREEN *sp, char w_flag)

/* the w_flag is used to determine what needs updating: if the WFHARD
   bit is set, both scroll bars need an update. If the WFMOVE bit
   is set, the horizontal scroll bar needs an update */
/* this function assumes s_cur_window matches curwp for the first_screen */
{
    int ScrollMax, ScrollMin, ScrollPos;
    
    if (vscrollbar && (w_flag & WFHARD)) {
        int    lastline;
        int     topline;

        {   /*-figure-out where we are at vertically */
            register LINE   *lp;
            register LINE   *linep; /* header (= last) line of buffer */
            LINE    *toplp;         /* top line of window */

            linep = sp->s_cur_window->w_bufp->b_linep;
            toplp = sp->s_cur_window->w_linep;
            topline = lastline = 0;
            lp = linep;
            do {
                lp = lforw(lp);
                lastline++;
                if (lp == toplp) topline = lastline;
            } while (lp != linep);
        }
        lastline += 1 - sp->s_cur_window->w_ntrows;
        if (lastline <= 1) lastline = 2;    /* to avoid scrollbar hiding */
        lastline = min(lastline, MAXSCROLL);
        topline = min(topline, MAXSCROLL);
        GetScrollRange ((HWND)sp->s_drvhandle, SB_VERT,
                        &ScrollMin, &ScrollMax);
        if ((ScrollMax != lastline) || (ScrollMin != 1)) {
            SetScrollRange ((HWND)sp->s_drvhandle, SB_VERT, 1,
                            lastline, FALSE);
            ScrollPos = -1; /* makes sure the scroll display is updated */
        }
        else ScrollPos = GetScrollPos ((HWND)sp->s_drvhandle, SB_VERT);
        if (topline != ScrollPos) {
            SetScrollPos ((HWND)sp->s_drvhandle, SB_VERT,
                          topline, TRUE);
        }
    }
    if (hscrollbar && (w_flag & (WFMOVE | WFHARD))) {
        /*-figure-out where we stand horizontally */
        int     row;
        LINE    *lp;
        WINDOW  *wp = sp->s_cur_window;
        int     maxlength = 0;

        lp = wp->w_linep;
        for (row = 0; (row < wp->w_ntrows) && (lp != wp->w_bufp->b_linep);
             row++) {
            maxlength = max(maxlength, lused(lp));
            lp = lforw(lp);
        }
        if (maxlength <= 0) maxlength = 1;
        maxlength = min(maxlength, MAXSCROLL);
        row = min(wp->w_fcol, MAXSCROLL);
        GetScrollRange ((HWND)sp->s_drvhandle, SB_HORZ,
                        &ScrollMin, &ScrollMax);
        if ((ScrollMax != maxlength) || (ScrollMin != 0)) {
            SetScrollRange ((HWND)sp->s_drvhandle, SB_HORZ, 0,
                            maxlength, FALSE);
            ScrollPos = -1; /* makes sure the scroll display is updated */
        }
        else ScrollPos = GetScrollPos ((HWND)sp->s_drvhandle, SB_HORZ);
        if (row != ScrollPos) {
            SetScrollPos ((HWND)sp->s_drvhandle, SB_HORZ,
                          row, TRUE);
        }
    }
} /* updscrollbars */

/* The routines in this file provide drivers for display and input under
   the Microsoft Windows environment on an IBM-PC or compatible
   computer.

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions.

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

#define termdef 1		/* don't define term external */

#include    "estruct.h"
#include    "elang.h"
#include    <stdio.h>
#include    <time.h>
#include    "eproto.h"
#include    "edef.h"

#include    "intuiwin.h"

#define     NPAUSE  1000	/* pause for fence-showing, 1 second */
#define     MARGIN  8		/* size of minimal margin and */
#define     SCRSIZ  20		/* scroll size for extended lines */

/* terminal interface table entries */

static int PASCAL aminop ();
static int PASCAL amiopen ();
static int PASCAL amigetc ();
static int PASCAL amiputc ();
static int PASCAL amiflush ();
static int PASCAL amibeep ();
static int PASCAL amimove ();
static int PASCAL amieeol ();
static int PASCAL amieeop ();
static int PASCAL amibeep ();
static int PASCAL amirev ();
static int PASCAL amirez ();

#if COLOR
static int PASCAL amisetfor ();
static int PASCAL amisetback ();

#endif
static int PASCAL amisleep ();
static int PASCAL aminewscr ();
static int PASCAL amidelscr ();
static int PASCAL amiselscr ();
static int PASCAL amisizscr ();
static int PASCAL amitopscr ();

/* Standard terminal interface dispatch table.
 */
NOSHARE TERM term =
{
    50,
    50,
    HUGE,
    HUGE,
    0,
    0,
    MARGIN,
    SCRSIZ,
    NPAUSE,
    amiopen,			/* t_open: Open terminal at the start */
    aminop,			/* t_close: Close terminal at end */
    aminop,			/* t_kopen: Open keyboard */
    aminop,			/* t_kclose: close keyboard */
    amigetc,			/* t_getchar: Get character from keyboard */
    amiputc,			/* t_putchar: Put character to display */
    amiflush,			/* t_flush: Flush output buffers */
    amimove,			/* t_move: Move the cursor, origin 0 */
    amieeol,			/* t_eeol: Erase to end of line */
    amieeop,			/* t_eeop: Erase to end of page */
    aminop,			/* t_clrdesk: Clear the page totally */
    amibeep,			/* t_beep: Sound beep */
    amirev,			/* t_rev: set reverse video state */
    amirez,			/* t_rez: change screen resolution */
#if	COLOR
    amisetfor,			/* t_setfor: set forground color */
    amisetback,			/* t_setback: set background color */
#endif
    amisleep,			/* t_sleep: wait a while */
    aminewscr,			/* t_newscr: create new "screen" display */
    amidelscr,			/* t_delscr: destroy "screen" display */
    amiselscr,			/* t_selscr: select "screen" display for IO */
    amisizscr,			/* t_sizscr: resize "screen" display */
    amitopscr			/* t_topscr: bring "screen" to top */
};

#define MAXMLHIST   8		/* must be a power of 2 */
#define MLHISTMASK  0x7		/* corresponding bit mask */
static char *MLHistory[MAXMLHIST] =
{NULL};

    /* circular history of the last few message line entries */
static int MLHistOld = 0;	/* index of the oldest message */
static int MLHistNew = 0;	/* index of the first empty slot */

static struct Window *IOWindow;	/* current window where I/Os take place */

static int UpdateRow;		/* Row being updated (-1 for no update) */

static struct
{
    int leftmost;
    int rightmost;		/* -1 indicates the range is empty */
}
UpdateCol;			/* Columns to invalidate on UpdateRow */

static SCREEN *IOScreen;	/* Screen affected by the I/Os */

#if     COLOR
static int ForgColor;		/* current selection of foreground and */
static int BackColor;		/* background colors */

#endif

/* PushMLHist:  store the last message line contents in the history */
/* ==========                                                       */

static void PASCAL 
PushMLHist (void)
{
    char *ml;
    int i, e;

    for (e = MLSIZE - 1; e >= 0; e--)
	if (MLBuf[e] != ' ')
	    break;
    /* e = index of the last non-space char */
    if (e < 0)
	return;
    if ((ml = MLHistory[MLHistNew] = malloc (e + 2)) != NULL)
    {
	for (i = 0; i <= e; i++)
	    *ml++ = MLBuf[i];
	*ml = '\0';		/* terminate the string */
	MLHistNew = (MLHistNew + 1) & MLHISTMASK;
	if (MLHistOld == MLHistNew)
	{
	    /* the history is full. This old entry needs to be discarded */
	    free (MLHistory[MLHistOld]);
	    MLHistOld = (MLHistOld + 1) & MLHISTMASK;
	}
    }
}				/* PushMLHist */

/* MLHistDlgProc:   dialog proc for the Message Line History */
/* =============                                             */

int EXPORT 
MLHistDlgProc (struct Window *hDlg, UINT wMsg,
	       UINT wParam, LONG lParam)
{
    switch (wMsg)
    {

	case WM_INITDIALOG:
	    {
		char s[50];

		strcpy (s, PROGNAME);
		strcat (s, TEXT330);
		SetWindowText (hDlg, s);
	    }
/*-fill the list box */
	    while (MLHistOld != MLHistNew)
	    {
		SendDlgItemMessage (hDlg, ID_HIST, LB_ADDSTRING, 0,
				    (DWORD) MLHistory[MLHistOld]);
		free (MLHistory[MLHistOld]);
		MLHistOld = (MLHistOld + 1) & MLHISTMASK;
	    }
/*-scroll the last message into view */
	    {
		int i;

		i = SendDlgItemMessage (hDlg, ID_HIST, LB_GETCOUNT, 0, 0L);
		SendDlgItemMessage (hDlg, ID_HIST, LB_SETCURSEL, i - 1, 0L);
		SendDlgItemMessage (hDlg, ID_HIST, LB_SETCURSEL, -1, 0L);
	    }
	    return TRUE;

	case WM_COMMAND:
	    if (NOTIFICATION_CODE != BN_CLICKED)
		break;
	    if (LOWORD (wParam) == 1)
	    {
		EndDialog (hDlg, 0);
		return TRUE;
	    }
	    break;
    }
    return FALSE;
}				/* MLHistDlgProc */

/* mlhistory:   displays the message line history box */
/* =========                                          */

/* This function is needed because the message line only shows the last
   message. If a macro fails, it usually generates a slew of errors
   (each beginning by a \n). Eventually, calling this function in the
   core editor loop will pop up a dialog box showing up to MAXMLHIST-1
   past messages */

PASCAL 
mlhistory (void)
{
    FARPROC ProcInstance;

    PushMLHist ();
    mlferase ();
    ProcInstance = MakeProcInstance ((FARPROC) MLHistDlgProc,
				     hEmacsInstance);
    DialogBox (hEmacsInstance, "MLHIST", hFrameWnd, ProcInstance);
    FreeProcInstance (ProcInstance);
}				/* mlhistory */

static void PASCAL 
ChangeUpdateRow (int newrow)
/* Set UpdateRow to newrow, invalidating the changed cells in the
   previous update row */
{
    if ((UpdateCol.rightmost >= 0) && (UpdateRow >= 0))
    {
	InvalidateCells (IOWindow, UpdateCol.leftmost, UpdateRow,
			 UpdateCol.rightmost, UpdateRow);
    }
    UpdateRow = newrow;
}				/* ChangeUpdateRow */

/*****************************************/
/* amiopen: initialize windows interface */
/* =======                               */

static int PASCAL 
amiopen ()
{
    static BOOL FirstTime = TRUE;

    if (FirstTime)
    {
	int x;

	revexist = TRUE;
	eolexist = TRUE;
	strcpy (sres, "MSWIN");	/* $SRES emacs variable */
	strcpy (os, "MSWIN");	/* $OS emacs variable */
	MLBuf = (char *)malloc (MLSIZE);
	for (x = 0; x < MLSIZE;)
	    MLBuf[x++] = ' ';
	FirstTime = FALSE;
    }
    UpdateRow = -1;
    UpdateCol.rightmost = -1;
    UpdateCol.leftmost = HUGE;	/* empty range */
    return 0;
}				/* amiopen */

/***************************************************/
/* amigetc: get character from keyboard (or mouse) */
/* =======                                         */

static int PASCAL 
amigetc ()
{
    return GetInput ();
}				/* amigetc */

/*************************************/
/* amiputc: put character to display */
/* =======                           */

static int PASCAL 
amiputc (c)
int c;				/* character to write (or 0 for dummy write) */
{
    if (UpdateRow != CurrentRow)
    {
	ChangeUpdateRow (CurrentRow);
	UpdateCol.rightmost = -1;
	UpdateCol.leftmost = HUGE;	/* empty range */
    }

/*-set the colors and reverse status in the display buffer */
    if (IOWindow != hFrameWnd)
    {				/* not supported on message line */
	register VIDEO *vp;

	vp = IOScreen->s_physical[UpdateRow];
	vp->v_fcolor = ForgColor;
	vp->v_bcolor = BackColor;
    }

    /*-update the cursor's position and the UpdateCol range. Update the
       message line buffer if that's where the fun is */
    if (c != 0)
    {
	if (CurrentCol < UpdateCol.leftmost)
	    UpdateCol.leftmost = CurrentCol;
	if (CurrentCol > UpdateCol.rightmost)
	    UpdateCol.rightmost = CurrentCol;
	if (IOWindow == hFrameWnd)
	{			/* message line */
	    if (c == '\b')
	    {
		if (CurrentCol > 0)
		    MLBuf[--CurrentCol] = ' ';
	    }
	    else
	    {
		if (c == '\n')
		    PushMLHist ();
		else
		{
		    MLBuf[CurrentCol] = c;
		    if (CurrentCol < MLSIZE - 1)
			++CurrentCol;
		}
	    }
	}
	else
	    ++CurrentCol;
    }
    return 0;
}				/* amiputc */

/**************************************************/
/* amiflush:    update display from output buffer */
/* ========                                       */

static int PASCAL 
amiflush ()
{
    ChangeUpdateRow (-1);
    if (!InternalRequest && (IOWindow == hFrameWnd))
	UpdateWindow (IOWindow);
    /* note: we do not update screens in real time */
    MoveEmacsCaret (IOWindow, CurrentCol, CurrentRow);
    /* some caret movement is directly handled by the driver (see
       amiputc) */
    return 0;
}				/* amiflush */

/****************************/
/* amimove: move the cursor */
/* =======                  */

static int PASCAL 
amimove (int newrow, int newcol)
{
    /*-perform IO windows switching depending on the addressed row: if
       beyond the IO screen, the message line is addressed*/
    if (!IOScreen || (newrow >= IOScreen->s_nrow))
    {
	if (IOWindow != hFrameWnd)
	{
	    ChangeUpdateRow (-1);
	    IOWindow = hFrameWnd;
	}
	CurrentRow = 0;
    }
    else
    {
	if (IOWindow != IOScreen->s_drvhandle)
	{
	    ChangeUpdateRow (-1);
	    IOWindow = IOScreen->s_drvhandle;
	}
	CurrentRow = newrow;
    }

    CurrentCol = newcol;

/*-update the caret */
    MoveEmacsCaret (IOWindow, CurrentCol, CurrentRow);
    return 0;
}				/* amimove */

/*********************************/
/* amieeol: erase to end of line */
/* =======                       */

static int PASCAL 
amieeol ()
{
    amiputc (0);		/* ensure change of row is properly handled */
    UpdateCol.leftmost = min (UpdateCol.leftmost, CurrentCol);
    if (IOWindow == hFrameWnd)
    {				/* message line */
	int x;

	for (x = CurrentCol; x < MLSIZE;)
	    MLBuf[x++] = ' ';
	UpdateCol.rightmost = MLSIZE - 1;
    }
    else
    {
	UpdateCol.rightmost = IOScreen->s_ncol;
    }
    amiflush ();
    return 0;
}				/* amieeol */

/*********************************/
/* amieeop: erase to end of page */
/* =======                       */

static int PASCAL 
amieeop ()
{
    if (IOWindow == hFrameWnd)
    {				/* message line */
	amieeol ();		/* only one line here */
	return 0;
    }
    amiputc (0);		/* ensure change of row is properly handled */
    UpdateCol.leftmost = min (UpdateCol.leftmost, CurrentCol);
    InvalidateCells (IOWindow, UpdateCol.leftmost, UpdateRow,
		     IOScreen->s_ncol, UpdateRow);
    if (CurrentRow + 1 < IOScreen->s_nrow)
    {
	register int r;
	register VIDEO *vp;

/*-propagate the colors and reverse status */
	for (r = CurrentRow + 1; r < IOScreen->s_nrow; r++)
	{
	    vp = IOScreen->s_physical[r];
	    vp->v_fcolor = ForgColor;
	    vp->v_bcolor = BackColor;
	}
/*-flag the area for repaint */
	InvalidateCells (IOWindow, 0, CurrentRow + 1,
			 IOScreen->s_ncol, IOScreen->s_nrow);
    }
    amiflush ();
    return 0;
}				/* amieeop */

/*************************/
/* amibeep: sound a beep */
/* =======               */

static int PASCAL 
amibeep ()
{
    MessageBeep (0);
    return 0;
}				/* amibeep */

/************************************/
/* amirev:  set reverse video state */
/* ======                           */

static int PASCAL 
amirev (state)
int state;			/* TRUE = reverse, FALSE = normal */
{
    /* nothing to do: the description of reverse video fields is kept
       up to date in the physical screen by the display.c code */
    return 0;
}				/* amirev */

/************************************/
/* amirez:  change "resolution"     */
/* ======                           */

static int PASCAL 
amirez (res)
char *res;
{
    return TRUE;		/* $SRES is read-only in MSWIN, but returning an
				   error may break existing macros */
}				/* amirez */

#if COLOR
/* amisetfor:   set foreground color */
/* =========                         */

static int PASCAL 
amisetfor (color)
int color;
{
    ForgColor = color;
    return 0;
}				/* amisetfor */

/* amisetback:  set background color */
/* ==========                        */

static int PASCAL 
amisetback (color)
int color;
{
    BackColor = color;
    return 0;
}				/* amisetback */
#endif

/* amisleep:    wait a specified time (in ms) in an unobtrusive way */
/* ========                                                         */

static int PASCAL 
amisleep (int t)
{
    return TakeANap (t);
}				/* amisleep */

/* aminewscr:   create new MDI window for new screen */
/* =========                                         */

static int PASCAL 
aminewscr (SCREEN * sp)
/* called by screen.c after the screen structure has been allocated. The
   size of the screen will be determined by the window's size. returns
   TRUE if successful. */
{
    MDICREATESTRUCT cs;
    BOOL Maximized;

#if WINDOW_MSWIN32
    Maximized = (GetWindowLong ((struct Window *)SendMessage (hMDIClientWnd,
							      WM_MDIGETACTIVE,
							      0, 0L),
				GWL_STYLE) & WS_MAXIMIZE);
#else
    Maximized = HIWORD (SendMessage (hMDIClientWnd, WM_MDIGETACTIVE, 0, 0L));
#endif
    if (Maximized)
    {
	restorescreen (FALSE, 0);	/* de-maximize previous one */
    }
    if (first_screen == NULL)
	Maximized = TRUE;
    cs.szClass = ScreenClassName;
    cs.szTitle = sp->s_screen_name;
    cs.hOwner = hEmacsInstance;
    cs.x = CW_USEDEFAULT;
    cs.y = CW_USEDEFAULT;
    cs.cx = CW_USEDEFAULT;
    cs.cy = CW_USEDEFAULT;
    if (Maximized)
	cs.style = WS_MAXIMIZE;
    else
	cs.style = 0;
    cs.style |= WS_HSCROLL | WS_VSCROLL;
    cs.lParam = (DWORD) sp;
    sp->s_virtual = NULL;	/* not NULL will testify of success */
    InternalRequest = TRUE;
    SendMessage (hMDIClientWnd, WM_MDICREATE, 0, (DWORD) (LPSTR) & cs);
    if (sp->s_virtual)
    {
	if (hFrameWnd == GetActiveWindow ())
	    SetFocus (sp->s_drvhandle);
    }
    InternalRequest = FALSE;
    if (!sp->s_virtual)
    {
	amidelscr (sp);		/* failed, cleanup! */
	return FALSE;
    }
    return TRUE;
}				/* aminewscr */

/* amidelscr:   destroys an MDI window for a disappearing screen */
/* =========                                                     */

static int PASCAL 
amidelscr (SCREEN * sp)
/* called by screen.c before the screen structure is deallocated */
{
    if (sp->s_drvhandle == IOWindow)
	amiflush ();
    if (sp == IOScreen)
	amiselscr (first_screen);
    /* hopefully, at this time, sp!=first_screen */
    SendMessage (hMDIClientWnd, WM_MDIDESTROY, (UINT) sp->s_drvhandle, 0L);
}				/* amidelscr */

/* amiselscr:   select a window/screen combination for the next IOs */
/* =========                                                        */

static int PASCAL 
amiselscr (SCREEN * sp)
{
    IOWindow = sp->s_drvhandle;
    IOScreen = sp;
}				/* amiselscr */

/* amisizscr:   resize an MDI window to fit the associated screen */
/* =========                                                      */

static int PASCAL 
amisizscr (SCREEN * sp)
/* called by Emacs when the screen's dimensions have been changed. A resize
   through the MS-Windows interface is handled by the ReSize function in
   amidisp.c */
{
    struct Window *hScrWnd;
    BOOL Maximized;
    int RetryCount = -1;	/* prevents infinite loop if frame

				   window can't be resized properly */
    BOOL FrameIconic;
    WINDOWPLACEMENT WindowPlacement;

    if (InternalRequest)
	return;
    InternalRequest = TRUE;
    hScrWnd = sp->s_drvhandle;

    {	/*-If this is iconized, we have to restore it first to be
	   allowed to change its size. If it is the active MDI child and
	   is maximized, we will actually resize the frame window so, if
	   the later is maximized, we restore it first. */

#if WINDOW_MSWIN32
	struct Window *hActiveScr;

	hActiveScr = (struct Window *)SendMessage (hMDIClientWnd, WM_MDIGETACTIVE, 0, 0L);
	Maximized = (hScrWnd == hActiveScr) &&
		(GetWindowLong (hScrWnd, GWL_STYLE) & WS_MAXIMIZE);
#else
	DWORD ActiveScr;

	ActiveScr = SendMessage (hMDIClientWnd, WM_MDIGETACTIVE, 0, 0L);
	Maximized = (hScrWnd == LOWORD (ActiveScr)) && HIWORD (ActiveScr);
#endif
	if (IsIconic (hScrWnd))
	{
	    SendMessage (hMDIClientWnd, WM_MDIRESTORE, (UINT) hScrWnd, 0L);
	}
	if (Maximized && IsZoomed (hFrameWnd))
	{
	    ShowWindow (hFrameWnd, SW_RESTORE);
	}
    }
    FrameIconic = IsIconic (hFrameWnd);
    do
    {
/*-Now, proceed with resizing... */
	int Delta;
	RECT TargetRect, BoundingRect;	/* in MDIClient coordinates */
	RECT OldClientRect;
	POINT ClientSize;

	GetClientRect (hScrWnd, &OldClientRect);
	MinimumClientSize (hScrWnd, sp->s_ncol, sp->s_nrow,
			   &ClientSize.x, &ClientSize.y);
	if (Maximized)
	{
	    if (((OldClientRect.bottom == ClientSize.y)
		 && (OldClientRect.right == ClientSize.x))
		|| (RetryCount > 0))
		break;		/* exit the resize loop */
	    ++RetryCount;
	    BoundingRect.left = BoundingRect.top = 0;
	    BoundingRect.right = GetSystemMetrics (SM_CXSCREEN);
	    BoundingRect.bottom = GetSystemMetrics (SM_CYSCREEN);
	    if (FrameIconic && Win31API)
	    {
		GetWindowPlacement (hFrameWnd, &WindowPlacement);
		TargetRect = WindowPlacement.rcNormalPosition;
	    }
	    else
	    {
		GetWindowRect (hFrameWnd, &TargetRect);
	    }
	}
	else
	{
	    GetClientRect (hMDIClientWnd, &BoundingRect);
	    GetWindowRect (hScrWnd, &TargetRect);
	    ScreenToClient (hMDIClientWnd, (LPPOINT) & TargetRect.right);
	    ScreenToClient (hMDIClientWnd, (LPPOINT) & TargetRect.left);
	}

	/*-process change of width: we move the right edge. If it would
	   go off bounds, we move the left edge back to compensate, but
	   never below 0 */
	Delta = ClientSize.x - OldClientRect.right;
	if ((TargetRect.right + Delta) > BoundingRect.right)
	{
	    TargetRect.left += (BoundingRect.right - TargetRect.right)
		    - Delta;
	    TargetRect.right = BoundingRect.right;
	    if (TargetRect.left < 0)
	    {
		TargetRect.right -= TargetRect.left;
		TargetRect.left = 0;
	    }
	}
	else
	    TargetRect.right += Delta;

	/*-process change of height: we move the bottom edge. If it
	   would go off screen, we move the top edge up to compensate,
	   but never beyond 0 */
	Delta = ClientSize.y - OldClientRect.bottom;
	if ((TargetRect.bottom + Delta) > BoundingRect.bottom)
	{
	    TargetRect.top += (BoundingRect.bottom - TargetRect.bottom)
		    - Delta;
	    TargetRect.bottom = BoundingRect.bottom;
	    if (TargetRect.top < 0)
	    {
		TargetRect.bottom -= TargetRect.top;
		TargetRect.top = 0;
	    }
	}
	else
	    TargetRect.bottom += Delta;

	if (Maximized && FrameIconic && Win31API)
	{
	    WindowPlacement.rcNormalPosition = TargetRect;
	    SetWindowPlacement (hFrameWnd, &WindowPlacement);
	}
	else
	{
	    SetWindowPos (Maximized ? hFrameWnd : hScrWnd, 0,
			  TargetRect.left, TargetRect.top,
			  TargetRect.right - TargetRect.left,
			  TargetRect.bottom - TargetRect.top,
			  SWP_DRAWFRAME | SWP_NOZORDER);
	}
    }
    while (Maximized);		/* if Maximized, we loop once in case a
				   change of menu bar height has prevented
				   us from getting the expected client size
				 */
    InternalRequest = FALSE;
}				/* amisizscr */

/* amitopscr:   bring a screen's window to top. */
/* =========                                    */

static int PASCAL 
amitopscr (SCREEN * sp)

/* called by screen.c when selecting a screen for current */
{
    if (!InternalRequest)
    {
	InternalRequest = TRUE;
	SendMessage (hMDIClientWnd, WM_MDIACTIVATE,
		     (UINT) sp->s_drvhandle, 0L);
#if WINDOW_MSWIN32
	if (GetWindowLong (sp->s_drvhandle, GWL_STYLE) & WS_MAXIMIZE)
	{
#else
	if (HIWORD (SendMessage (hMDIClientWnd, WM_MDIGETACTIVE, 0, 0L)))
	{
#endif
	    /* we have a maximized screen. Since we activated it under
	       InternalRequest, the new size has not yet been processed
	       by emacs, so we have to fix that */
	    newwidth (TRUE, DisplayableColumns (sp->s_drvhandle, 0, &EmacsCM));
	    newsize (TRUE, DisplayableRows (sp->s_drvhandle, 0, &EmacsCM));
	}
	InternalRequest = FALSE;
    }
}				/* amitopscr */

/*************************/
/* aminop:  No Operation */
/* ======                */

static int PASCAL 
aminop ()
{
    return 0;
}

/* The routines in this file provide support for various OS-related
   functions under the Microsoft Windows environment on an IBM-PC or
   compatible computer.

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions.

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

#include    "estruct.h"
#include    "elang.h"
#include    <stdio.h>
#include    <time.h>
#include    "eproto.h"
#include    "edef.h"

#include    "mswin.h"

#include <setjmp.h>

#define MAXPARAM    10		/* max command line parameters */
#define TXTSIZ      NFILEN	/* all purpose string length */
#define T_SLEEP      1		/* Sleep timer ID */

/* message codes for EmacsBroadcastMsg */
#define EMACS_PROCESS   1
#define EMACS_STARTING  1
#define EMACS_ENDING    2

/* variables */
static char *argv[MAXPARAM];
static int argc;

static char FrameClassName[] = PROGNAME ":frame";

/* The Catch/Throw API is replaced by setjmp/longjmp */
static jmp_buf ExitCatchBuf;

#define Throw(buf,n) longjmp(buf,n)
#define Catch(buf)   setjmp(buf)

static int Sleeping = 0;	/* flag for TakeANap() */
static int TimeSlice;
static BOOL LongOperation = FALSE;	/* for longop() and GetCharacter() */
static BOOL HourglassOn = FALSE;	/* between SetHourglass & UpdateCursor */
static BOOL SystemModal = FALSE;	/* indicates that the Yes/No message

					   box should be system-modal */

static UINT EmacsBroadcastMsg;
static DWORD BroadcastVal;	/* used by the EmacsBroadcast function */

static WNDPROC MDIClientProc;

#if GRINDERS != 0
static HCURSOR hRealHourglass;

#endif

/* prototypes */
static void  MessageLoop (BOOL WaitMode);
static BOOL  UpdateCursor (struct Window * window, UINT wParam, LONG lParam);
static void  SetHourglass (BOOL hg);

/* timeset: return a system-dependant time string */
/* =======                                        */
char * 
timeset ()

{
    register char *sp;		/* temp string pointer */
    time_t buf;			/* time data buffer */

    time (&buf);
    sp = ctime (&buf);
    sp[strlen (sp) - 1] = 0;
    return (sp);
}

/* longop:    to be called regularly while a long operation is in progress */
/* ========                                                                */

void  
longop (int f)

/* f is TRUE to set long operation status and FALSE to reset that status */
/* when a long operation is signaled at least twice, the hourglass
   cursor comes up */
/* While the long operation is in progress, this function runs the
   message loop approximately every 100 ms to yield to other
   applications and allow a reduced set of commands (among which quit)
   to be input by the user */
{
    static DWORD LastYield;	/* time of last yield in milliseconds */

    if (f)
    {
	if (!LongOperation)
	{
	    LongOperation = TRUE;
	    LastYield = GetCurrentTime ();
	}
	else
	{			/* 2nd (or more) call in a row, let's yield if enough
				   time has elapsed */
	    DWORD Time;

	    if ((Time = GetCurrentTime ()) - LastYield >= TimeSlice)
	    {
#if GRINDERS != 0
		if (++GrinderIndex >= GRINDERS)
		    GrinderIndex = 0;
#endif
		SetHourglass (TRUE);
		LastYield = Time;
		MessageLoop (FALSE);
	    }
	}
    }
    else
    {
	if (LongOperation)
	{
	    LongOperation = FALSE;
	    SetHourglass (FALSE);
	}
    }
}				/* longop */

/* mlyesno: ask a yes/no question */
/* =======                        */

 
mlyesno (char *prompt)

/* This function replaces the mlyesno from input.c. Instead of asking a
   question on the message line, it pops up a message box */
{
    struct EasyStruct es =
    {
    	sizeof(struct EasyRequest),
    	0L,
    	ProgName,
    	prompt,
    	"Yes|No"
    };
    if (EasyRequest (FrameWnd, &es, NULL, NULL) == 1)
	return TRUE;
    return FALSE;
}				/* mlyesno */

/* mlabort: display a serious error message (proposes abort) */
/* =======                                                   */

VOID  NEAR 
mlabort (char *s)
{
    char text[NSTRING];		/* hopefully sufficient! */

    while (*s == '%')
	s++;			/* remove those strange % signs in some messages */
    strcpy (text, s);
    strcat (text, "\tAbort ");
    strcat (text, ProgName);
    strcat (text, " ?");
    struct EasyStruct es =
    {
    	sizeof(struct EasyRequest),
    	0L,
    	ProgName,
    	text,
    	"Yes|No"
    };
    if (EasyRequest (FrameWnd, &es, NULL, NULL) == 1)
    {
	eexitval = -1;
	Throw (ExitCatchBuf, ABORT);
    }
}				/* mlabort */

struct Screen * UEOpenScreen(WORD *scrWidth, WORD *scrHeight)
{
    struct NewScreen newScr;
    struct Screen *screen;

    ULONG screenModeId;
    struct Screen *pubscr = LockPubScreen ("Workbench");

    if (pubscr)
    {
	screenModeId = GetVPModeID (&(pubscr->ViewPort));

	screen = OpenScreenTags (0L,
			      SA_Width, pubscr->Width,
			      SA_Height, pubscr->Height,
			      SA_Depth, 1,
			      SA_DisplayID, screenModeId,
			      SA_Overscan, OSCAN_TEXT,
			      SA_Autoscroll, TRUE,
	   SA_Title, (ULONG) ((UBYTE *) "MicroEMACS 3.12g/Amiga"),
			      SA_SysFont, 0,
			      SA_Type, CUSTOMSCREEN,
			      SA_BlockPen, 1,
			      SA_DetailPen, 0,
			      TAG_END);

	UnLockPubScreen (NULL, pubscr);
    }
    else
	screen = NULL;
    if (screen == NULL)
    {
	printf ("%%Can not open a screen\n");
    }
    else
    {
    	*scrWidth = screen->Width;
    	*scrHeight = screen->Height;
    }
    return screen;
}

/*************************************************************************
 *
 *	Name:		UEOpenWindow
 *
 *	Purpose:	Open a window on the specified screen; if the
 *		screen ptr is NULL the screen used is Workbench.
 *
 *************************************************************************/

struct Window * UEOpenWindow(struct Screen *screen)
{
    struct NewWindow newWin;
    struct Screen *pubScreen;
    APTR VisualInfo = NULL;

    memset (&newWin, 0, sizeof (struct NewWindow));

    /* initialize the new windows attributes */
    newWin.LeftEdge = 0;
    newWin.TopEdge = 0;
    newWin.Height = screenHeight;

    if (screen)
	newWin.Width = screenWidth;
    else
	newWin.Width = screenWidth;

    newWin.DetailPen = 0;
    newWin.BlockPen = 1;
    newWin.Title = (unsigned char *)"MicroEMACS 3.12g/Amiga";
    newWin.Flags = WFLG_CLOSEGADGET | WFLG_SMART_REFRESH | WFLG_ACTIVATE |
	    WFLG_SIZEBRIGHT | WFLG_NOCAREREFRESH;

   if (screen)
	newWin.Flags |= BACKDROP | BORDERLESS ;
    else
    	newWin.Flags |=  WFLG_SIZEGADGET | WFLG_DEPTHGADGET | WFLG_DRAGBAR;

    newWin.IDCMPFlags = IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS |
	    IDCMP_RAWKEY | IDCMP_NEWSIZE | IDCMP_MENUPICK | IDCMP_MENUHELP;

    if (screen)
    {
	newWin.Type = CUSTOMSCREEN;
	newWin.Screen = scr;
    }
    else
    {
	newWin.Type = WBENCHSCREEN;
	newWin.Screen = NULL;
    }

    newWin.FirstGadget = NULL;
    newWin.CheckMark = NULL;
    newWin.BitMap = NULL;
    newWin.MinWidth = 100;
    newWin.MinHeight = 25;
    newWin.MaxWidth = screenWidth;
    newWin.MaxHeight = screenHeight;

    /* open the window! */
    win = (struct Window *)OpenWindow (&newWin);

    return win;
}


/* WinInit: all the window initialization crap... */
/* =======                                        */

BOOL FAR  
WinInit (LPSTR lpCmdLine, int nCmdShow)

/* returns FALSE if failed init */
{
    char text[TXTSIZ];
    Point InitPos;
    int Colors;
    int i;
    static char HomeDir[NFILEN] = "HOME=";
    WORD w;

    MainHelpUsed = FALSE;
    HelpEngineFile[0] = '\0';

    strcpy (text, "progdir:mewin.guide");
    MainHelpFile = copystr (text);

    TimeSlice = 50;
    Colors = 4;   /* GetProfileInt (ProgName, "Colors", 0); */

    UEOpenScreen();
    UEOpenWindow();

    if (FrameWnd == 0)
	return FALSE;

    if (Colors == 0)
    {
	/* try to detect monochrome displays */
	HDC hDC;

	hDC = GetDC (FrameWnd);
	ColorDisplay = (GetDeviceCaps (hDC, NUMCOLORS) > 2);
	ReleaseDC (FrameWnd, hDC);
    }
    else
    {
	ColorDisplay = (Colors > 2);
    }

    ShowWindow (FrameWnd, nCmdShow);

    hScreenCursor = LoadCursor (hEmacsInstance, "ScreenCursor");
    hTrackCursor = hScreenCursor;
    hNotQuiescentCursor = LoadCursor (hEmacsInstance,
				      "NotQuiescentCursor");
#if GRINDERS == 0
    hHourglass = LoadCursor (NULL, IDC_WAIT);
#else
    strcpy (text, "Grinder1");
    for (i = 0; i < GRINDERS; i++)
    {
	text[7] = (char)i + '1';	/* this assumes GRINDERS < 10 */
	GrinderCursor[i] = LoadCursor (hEmacsInstance, text);
    }
    hRealHourglass = LoadCursor (NULL, IDC_WAIT);
    GrinderIndex = 0;
    hHourglass = GrinderCursor[0];
#endif

    in_init ();			/* sets up the input stream */

    argv[0] = ProgName;		/* dummy program name */
    {
	register char *s;

	argc = 1;
	s = copystr (lpCmdLine);
	while (*s != '\0')
	{
	    argv[argc] = s;
	    if (++argc >= MAXPARAM)
		goto ParsingDone;
	    while (*++s != ' ')
		if (*s == '\0')
		    goto ParsingDone;
	    *s = '\0';
	    while (*++s == ' ') ;
	}
    }
  ParsingDone:
    return TRUE;
}				/* WinInit */

/* SetFrameCaption: sets the frame window's text according to the app Id */
/* ===============                                                       */

static void  
SetFrameCaption (void)
{
    char text[sizeof (PROGNAME) + 19];
    char *t;
    int Id;

    strcpy (text, ProgName);
    Id = GetWindowWord (FrameWnd, GWW_FRMID);
    if (Id)
    {
	for (t = text; *t != '\0'; t++) ;	/* look for the end of text */
	*t++ = ' ';
	*t++ = '#';
	itoa (Id, t, 10);	/* append the App Id */
    }
    SetWindowText (FrameWnd, text);
    if (IsIconic (FrameWnd))	/* force redrawing of the icon title */
	SendMessage (FrameWnd, WM_NCACTIVATE, TRUE, 0L);
}				/* SetFrameCaption */

/* BroadcastEnumProc:   used by EmacsBroadcast */
/* =================                           */
BOOL EXPORT FAR  
BroadcastEnumProc (struct Window * window, LONG lParam)
{
    char ClassName[sizeof (FrameClassName) + 1];
    UINT RetVal;

    if (window != FrameWnd)
    {
	ClassName[0] = '\0';
	GetClassName (window, (LPSTR) & ClassName[0], sizeof (FrameClassName) + 1);
	if (strcmp (ClassName, FrameClassName) == 0)
	{
	    /* The enumerated window is a MicroEMACS frame */
	    if (lParam != 0)
	    {
/*-compute max of all returned values */
		RetVal = SendMessage (window, EmacsBroadcastMsg,
				      (UINT) FrameWnd, lParam);
		BroadcastVal = max (BroadcastVal, RetVal);
	    }
	    else
	    {
/*-compute number of applications */
		BroadcastVal++;
	    }
	}
    }
    return TRUE;
}				/* BroadcastEnumProc */

/* EmacsBroadcast:  send a broadcast message to all Emacs applications */
/* ==============                                                      */

static DWORD  
EmacsBroadcast (DWORD MsgParam)

/* If MsgParam is not zero, the broadcast is sent as an EmacsBroadcastMsg
   to all the Emacs frame windows, except the one specified by FrameWnd.
   The wParam of that message is FrameWnd, lParam is set to MsgParam.
   The value returned is the highest returned value from the broadcast
   or zero if no other emacs instance was found.

   - If MsgParam is 0, this function simply returns the number of Emacs
   instances found, not counting the one identified by FrameWnd.

 */
{
    FARPROC ProcInstance;

    ProcInstance = MakeProcInstance ((FARPROC) BroadcastEnumProc,
				     hEmacsInstance);
    BroadcastVal = 0;
    EnumWindows (ProcInstance, MsgParam);
    FreeProcInstance (ProcInstance);
    return BroadcastVal;
}				/* EmacsBroadcast */

/* MDIClientSubProc:    Subclassing window proc for the MDI Client window */
/* ================                                                       */

LONG EXPORT FAR  
MDIClientSubProc (struct Window * window, UINT wMsg, UINT wParam,
		  LONG lParam)
{
    switch (wMsg)
    {

#if !WIN30SDK
	case WM_CREATE:
	    if (Win31API)
		DragAcceptFiles (window, TRUE);
	    goto DefaultProc;

	case WM_DROPFILES:
	    DropMessage (window, (HDROP) wParam);
	    break;
#endif

	case WM_SETCURSOR:
	    if (UpdateCursor (window, wParam, lParam))
		return TRUE;
	    goto DefaultProc;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_CHAR:
	case WM_SYSCHAR:
	    if (EatKey (wMsg, wParam, lParam))
		break;
	    goto DefaultProc;

	case WM_PARENTNOTIFY:
	    if (notquiescent)
		break;		/* blocks mouse selection of an MDI
				   child (except on caption clicks!)
				 */
	    goto DefaultProc;

	default:
	  DefaultProc:
	    return CallWindowProc (MDIClientProc, window, wMsg, wParam, lParam);
    }
    return 0L;
}				/* MDIClientSubProc */

/* FrameInit:   Frame window's WM_CREATE */
/* =========                             */

void FAR  
FrameInit (CREATESTRUCT * cs)
{
    RECT Rect;
    CLIENTCREATESTRUCT ccs;
    HMENU hMenu;
    UINT FrameId;

#if !WIN30SDK
    if (Win31API)
	DragAcceptFiles (FrameWnd, TRUE);
#endif

    /*-advertise our birth to other Emacs instances and set the frame's
       caption with an Id if necessary */
    if ((FrameId = (UINT) EmacsBroadcast (MAKELONG (EMACS_STARTING,
						    EMACS_PROCESS))) != 0)
    {
	FrameId++;
    }
    SetWindowWord (FrameWnd, GWW_FRMID, (WORD) FrameId);
    SetFrameCaption ();

/*-add "optimize" to system menu */
    hMenu = GetSystemMenu (FrameWnd, 0);
    InsertMenu (hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_ENABLED, SC_OPTIMIZE,
		"&Optimize");

/*-initialize fonts and Cell Metrics */
    FontInit ();

/*-Create the MDI Client window */
    hMDIClientWnd = NULL;	/* the NULL initial value allows
				   DefFrameProc to behave properly if
				   FrameInit fails */
    ccs.hWindowMenu = GetScreenMenuHandle ();
    ccs.idFirstChild = IDM_FIRSTCHILD;
    GetClientRect (FrameWnd, &Rect);
    hMDIClientWnd = CreateWindow ("MDICLIENT",	/* class */
				  NULL,		/* title */
				  WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL |
				  WS_HSCROLL | WS_VISIBLE,	/* style */
				  0,	/* positions */
				  0,
				  Rect.right,	/* dimensions */
				  Rect.bottom - EmacsCM.MLHeight,
				  FrameWnd,	/* parent handle */
				  NULL,		/* menu */
				  hEmacsInstance,
				  (LPSTR) (LPCLIENTCREATESTRUCT) & ccs);
    if (hMDIClientWnd)
    {
	/* we subclass the MDIClient */
	FARPROC ProcInstance;

	MDIClientProc = (WNDPROC) GetWindowLong (hMDIClientWnd, GWL_WNDPROC);
	ProcInstance = MakeProcInstance ((FARPROC) MDIClientSubProc,
					 hEmacsInstance);
	SetWindowLong (hMDIClientWnd, GWL_WNDPROC, (DWORD) ProcInstance);
    }
}				/* FrameInit */

/* CloseEmacs:   handle WM_CLOSE of WM_QUERYENDSESSION messages */
/* ==========                                                   */

static BOOL  
CloseEmacs (UINT wMsg)

/* returns TRUE if emacs should exit */
{
    if (eexitflag)
	return TRUE;		/* emacs has already quited */
    if (wMsg == WM_QUERYENDSESSION)
	SystemModal = TRUE;
    if (fbusy != FWRITING)
    {				/* no file write in progress */
	quit (FALSE, 0);	/* check if it's ok with emacs to terminate */
    }
    else
    {				/* there is, indeed a file write in progress
				   MessageBeep (0);    /* wake the user up! */
	if (MessageBox (FrameWnd, TEXT333,
	/* "File write in progress. Quit later!" */
			ProgName,
			MB_OKCANCEL | MB_ICONSTOP |
			(SystemModal ? MB_SYSTEMMODAL : 0)) == IDCANCEL)
	{
	    quit (TRUE, 0);	/* give up! */
	}
    }
    if (wMsg == WM_QUERYENDSESSION)
	SystemModal = FALSE;
    if (eexitflag)
    {				/* emacs agrees to quit */
	ClipboardCleanup ();	/* close clipboard if we had it open */
	if (fbusy != FALSE)
	    ffclose ();		/* cleanup file IOs */
	return TRUE;
    }
    update (TRUE);		/* restore the caret that quit() moved into
				   the message line */
    return FALSE;		/* we refuse to die! */
}				/* CloseEmacs */

/* ScrWndProc:  MDI child (screen) window function */
/* ==========                                      */
LONG EXPORT FAR  
ScrWndProc (struct Window * window, UINT wMsg, UINT wParam,
	    LONG lParam)
{
    switch (wMsg)
    {

	case WM_CREATE:
#if !WIN30SDK
	    if (Win31API)
		DragAcceptFiles (window, TRUE);
#endif
	    if (!hscrollbar)
		ShowScrollBar (window, SB_HORZ, FALSE);
	    if (!vscrollbar)
		ShowScrollBar (window, SB_VERT, FALSE);
	    {
/*-init WindowWords for ScrReSize function */
		RECT Rect;

		GetClientRect (window, &Rect);
		SetWindowWord (window, GWW_SCRCX, (WORD) Rect.right);
		SetWindowWord (window, GWW_SCRCY, (WORD) Rect.bottom);
	    }
	    {
/*-setup the stuff for display.c */
		SCREEN *sp;

		sp = (SCREEN *) (((MDICREATESTRUCT *) (((CREATESTRUCT *) lParam)->
						    lpCreateParams))->lParam);
		SetWindowLong (window, GWL_SCRPTR, (LONG) sp);
		sp->s_drvhandle = window;
	    }
	    goto DefaultProc;

	case WM_DESTROY:
	    vtfreescr ((SCREEN *) GetWindowLong (window, GWL_SCRPTR));
	    break;

	case WM_KILLFOCUS:
	    EmacsCaret (FALSE);
	    goto DefaultProc;
	case WM_SETFOCUS:
	    EmacsCaret (TRUE);
	    goto DefaultProc;
	case WM_ACTIVATE:
	    if (LOWORD (wParam))
		EmacsCaret (TRUE);
	    goto DefaultProc;

	case WM_MDIACTIVATE:
	    if ((struct Window *) lParam == window)
	    {
		/* this one is becoming active */
		if (!InternalRequest)
		{
		    InternalRequest = TRUE;
		    select_screen ((SCREEN *) GetWindowLong (window, GWL_SCRPTR),
				   FALSE);
		    InternalRequest = FALSE;
		}
		else
		{
		    SCREEN *sp;

		    sp = (SCREEN *) GetWindowLong (window, GWL_SCRPTR);
		    if (sp->s_virtual == NULL)
		    {
			/* this is initialization time! */
			vtinitscr (sp, DisplayableRows (window, 0, &EmacsCM),
				   DisplayableColumns (window, 0, &EmacsCM));
		    }
		}
	    }
	    goto DefaultProc;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_CHAR:
	case WM_SYSCHAR:
	    if (EatKey (wMsg, wParam, lParam))
		break;
	    goto DefaultProc;

#if !WIN30SDK
	case WM_DROPFILES:
	    DropMessage (window, (HDROP) wParam);
	    break;
#endif

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	    MouseMessage (window, wMsg, wParam, lParam);
	    break;

	case WM_VSCROLL:
	case WM_HSCROLL:
	    ScrollMessage (window, wMsg, LOWORD (wParam),
			   (int)HIWORD (wParam));
	    break;

	case WM_SETCURSOR:
	    if (UpdateCursor (window, wParam, lParam))
		return TRUE;
	    goto DefaultProc;

	case WM_PAINT:
	    ScrPaint (window);
	    break;

	case WM_SIZE:
	    if (!IsIconic (FrameWnd))
	    {
		if (wParam == SIZEICONIC)
		    EmacsCaret (FALSE);
		else
		    EmacsCaret (TRUE);
		ScrReSize (window, wParam, LOWORD (lParam), HIWORD (lParam));
	    }
	    goto DefaultProc;

	case WM_GETMINMAXINFO:
	    GetMinMaxInfo (window, (LPPOINT) lParam);
	    goto DefaultProc;

	case WM_MOUSEACTIVATE:
	    if (notquiescent)
		break;		/* blocks mouse selection of an MDI
				   child's caption */
	    goto DefaultProc;

	case WM_SYSCOMMAND:
	    switch (wParam & 0xFFF0)
	    {
		case SC_RESTORE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_MINIMIZE:
		case SC_NEXTWINDOW:
		case SC_PREVWINDOW:
		    if (notquiescent)
			break;	/* sizing commands disabled */
		    goto DefaultProc;
		case SC_CLOSE:
		    if (!notquiescent)
		    {
			SCREEN *sp;

			/* this must be done here, before any MDI mumbo-jumbo */
			sp = (SCREEN *) GetWindowLong (window, GWL_SCRPTR);
			if (sp == first_screen)
			{
			    cycle_screens (FALSE, 0);
			}
			if (sp != first_screen)
			{
			    unlist_screen (sp);
			    free_screen (sp);
			}
			update (TRUE);	/* to restore the caret */
		    }
		    break;
		case SC_MOVE:
		    if (notquiescent)
		    {
			/* prevent moving an MDI client other than the already
			   active one, since it would transfer the activation */
			if (window != (struct Window *) SendMessage (hMDIClientWnd, WM_MDIGETACTIVE,
							0, 0L))
			    break;
		    }
		    goto DefaultProc;
		default:
		    goto DefaultProc;
	    }
	    break;

	default:
	  DefaultProc:
	    return DefMDIChildProc (window, wMsg, wParam, lParam);
    }
    return 0L;
}				/* ScrWndProc */

/* FrameWndProc:    frame window function */
/* ============                           */
LONG EXPORT FAR  
FrameWndProc (struct Window * window, UINT wMsg, UINT wParam,
	      LONG lParam)
{
    switch (wMsg)
    {

#if !WIN30SDK
	case WM_DROPFILES:
	    DropMessage (window, (HDROP) wParam);
	    break;
#endif

	case WM_INITMENUPOPUP:
	    InitMenuPopup ((HMENU) wParam, lParam);
	    goto DefaultProc;

	case WM_MENUCHAR:
	    {
		LONG Code;

		Code = DefFrameProc (window, hMDIClientWnd, wMsg, wParam, lParam);
		if (HIWORD (Code) != 0)
		    return Code;	/* matches a menu command */
		if (EatKey (wMsg, wParam, lParam))
		    return MAKELONG (0, 1);
		/* MicroEMACS ate that character, close the current menu! */
		else
		    return Code;
	    }

	case WM_COMMAND:
	    if (!MenuCommand (wParam, lParam))
		goto DefaultProc;
	    break;

	case WM_SYSCOMMAND:
	    switch (wParam & 0xFFF0)
	    {
		case SC_RESTORE:
		    if (IsIconic (FrameWnd))
			goto DefaultProc;
		case SC_SIZE:
		case SC_MAXIMIZE:
		    if (notquiescent)
			break;	/* sizing commands disabled */
		    goto DefaultProc;
		case SC_OPTIMIZE:
		    if (notquiescent)
			break;
		    if (IsZoomed (FrameWnd) || IsIconic (FrameWnd))
		    {
			/* must restore it first */
			ShowWindow (FrameWnd, SW_RESTORE);
		    }
		    {
			int IconHeight;

#if WIN30SDK
			{
#else
			if (Win31API)
			{
			    IconHeight = GetSystemMetrics (SM_CYICONSPACING);
			}
			else
			{
#endif
			    IconHeight = GetSystemMetrics (SM_CYICON) +
				    GetSystemMetrics (SM_CYCAPTION) +
				    GetSystemMetrics (SM_CYBORDER);
			}
			MoveWindow (FrameWnd, 0, 0,
				    GetSystemMetrics (SM_CXSCREEN),
				  GetSystemMetrics (SM_CYSCREEN) - IconHeight,
				    TRUE);
		    }
		    break;
		default:
		    goto DefaultProc;
	    }
	    break;

	case WM_SETCURSOR:
	    if (UpdateCursor (window, wParam, lParam))
		return TRUE;
	    goto DefaultProc;

	case WM_TIMER:
	    if (wParam == T_SLEEP)
	    {
		--Sleeping;	/* signal wake up to TakeANap() */
		break;
	    }
	    goto DefaultProc;

	case WM_PAINT:		/* must be the message line */
	    MLPaint ();
	    break;

	case WM_SIZE:		/* must adjust the MDIClient's size to fit the
				   Message Line */
	    MoveWindow (hMDIClientWnd, 0, 0,
		   LOWORD (lParam), HIWORD (lParam) - EmacsCM.MLHeight, TRUE);
	    InvalidateRect (FrameWnd, NULL, FALSE);
	    break;

	case WM_ACTIVATE:
	    EmacsCaret (LOWORD (wParam));
	    /* This call matters only when the active MDI Child is
	       iconic, in which case it never receives the focus and
	       thus fails to create the caret (that should still appear
	       if we are getting input on the message line) */
	    goto DefaultProc;

	case WM_CREATE:
	    FrameWnd = window;
	    FrameInit ((CREATESTRUCT *) lParam);
	    goto DefaultProc;

	case WM_QUERYENDSESSION:
	    if (CloseEmacs (wMsg))
		return 1L;
	    break;

	case WM_CLOSE:
	    if (CloseEmacs (wMsg))
		goto DefaultProc;
	    break;

	case WM_DESTROY:
	    if (MainHelpUsed)
		WinHelp (FrameWnd, MainHelpFile, HELP_QUIT, NULL);
	    if (HelpEngineFile[0] != '\0')
		WinHelp (FrameWnd, HelpEngineFile,
			 HELP_QUIT, NULL);
	    EmacsBroadcast (MAKELONG (EMACS_ENDING, EMACS_PROCESS));
	    PostQuitMessage (0);
	    break;

	default:
	    if ((wMsg == EmacsBroadcastMsg) &&
		(HIWORD (lParam) == EMACS_PROCESS))
	    {
		UINT Id;

		switch (LOWORD (lParam))
		{
		    case EMACS_STARTING:
			/* another instance of emacs is starting. If our ID was
			   0, it should now be 1 */
			if (GetWindowWord (FrameWnd, GWW_FRMID) == 0)
			{
			    SetWindowWord (FrameWnd, GWW_FRMID, 1);
			    SetFrameCaption ();
			}
			break;
		    case EMACS_ENDING:
			/* another instance of emacs is ending. If we are going to be
			   alone, let's switch our Id back to 0 */
			Id = (UINT) EmacsBroadcast (0);
			/* Id here is the number of Emacs applications, except for us */
			if (Id == 1)
			{
			    SetWindowWord (FrameWnd, GWW_FRMID, 0);
			    SetFrameCaption ();
			}
		}
		/* send back our Id */
		return (LONG) GetWindowWord (FrameWnd, GWW_FRMID);
	    }
	  DefaultProc:
	    return DefFrameProc (window, hMDIClientWnd, wMsg, wParam, lParam);
    }
    return 0L;
}				/* FrameWndProc */

/* WinMain: Application entry point */
/* =======                          */

int  
WinMain (HANDLE hInstance, HANDLE hPrevInstance,
	 LPSTR lpCmdLine, int nCmdShow)
{
    hEmacsInstance = hInstance;
    if (!WinInit (lpCmdLine, nCmdShow))
	return -1;

    switch (Catch (ExitCatchBuf))
    {
	case 0:
	    emacs (argc, argv);
	    /* If we exit through an emacs command, we pass here. Otherwise
	       (exit through a close in system menu or a windows session
	       termination), the application terminates from the call to
	       mswgetc that received the signal */
	case ABORT:
	    eexitflag = TRUE;
	    longop (FALSE);
	    PostMessage (FrameWnd, WM_CLOSE, 0, 0L);
	    for (;;)
		MessageLoop (TRUE);	/* go into message loop until WM_QUIT */
/*  case TRUE:
   break; */
    }
    return eexitval;
}				/* WinMain */

/* ModifyCursor:    forces a WM_SETCURSOR */
/* ============                           */

static void  
ModifyCursor (void)
{
    POINT pt;

    GetCursorPos (&pt);
    SetCursorPos (pt.x, pt.y);
}				/* ModifyCursor */

/* MessageLoop: Main message loop */
/* ===========                    */

static void  
MessageLoop (BOOL WaitMode)

/* If WaitMode is TRUE this function uses GetMessage the first time and
   PeekMessage after that, until the input queue is empty and there is
   something in the in_put pipe. Thus, WM_PAINTs are processed and the
   input queue is emptied into the in_put pipe.
   If WaitMode is FALSE, this function uses PeekMessage from the
   beginning and returns only when the input queue is empty. In that
   case, WM_PAINTs are not processed, but the processor still gets
   relinquished. Note that while peeking for messages, this function
   returns if the in_put pipe is about to overflow */
{
    MSG Msg;
    BOOL Peeking;

    ++notquiescent;		/* this restrics some actions in nested calls */

    ScrollBars ();		/* hide or show as needed (may generate WM_SIZE) */
    ModifyCursor ();

    do
    {
	Peeking = !WaitMode;
	for (;;)
	{
	    if (defferupdate)
	    {
		ShowEmacsCaret (FALSE);
		update (TRUE);
		ShowEmacsCaret (TRUE);
	    }
	    if (Peeking)
	    {
		if (!in_room (5))
		    break;	/* input stream about to overflow */
		if (!PeekMessage (&Msg, NULL, 0, 0, PM_REMOVE))
		    break;
	    }
	    else
	    {
		longop (FALSE);	/* we are going to wait for input */
		GetMessage (&Msg, NULL, 0, 0);
		Peeking = TRUE;	/* from now on... */
	    }
	    if (Msg.message == WM_QUIT)
	    {			/* time to leave... */
		JettisonFarStorage ();
		if (hEmacsFont)
		    DeleteObject (hEmacsFont);
		Throw (ExitCatchBuf, TRUE);
		/* we're gone out of business ! */
		/* **************************** */
	    }
	    if (!TranslateMDISysAccel (hMDIClientWnd, &Msg))
	    {
		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	    }
	}
    }
    while (WaitMode && !in_check ());	/* keep waiting if pipe is empty */

    --notquiescent;
    ModifyCursor ();
    return;
}				/* MessageLoop */

/* GetInput: wait for user input (called by mswgetc from mswdrv.c) */
/* ========                                                        */

/* The returned value is the next character from the input stream */

int FAR  
GetInput (void)
{
    if (!in_check ())
    {
	ShowEmacsCaret (TRUE);
	MessageLoop (TRUE);
	ShowEmacsCaret (FALSE);
    }
    return in_get ();		/* return next character */
}				/* GetInput */

/* TakeANap:    put emacs to sleep for a few milliseconds */
/* ========                                               */

int FAR  
TakeANap (int t)
/* this function is used by mswsleep(). It returns TRUE unless the timer
   could not be created. Note that for a null time, it simply
   relinquishes the processor */
{
    TakingANap = TRUE;
    if ((t == 0) ||
	(!SetTimer (FrameWnd, T_SLEEP, t, (FARPROC) NULL)))
    {
	ShowEmacsCaret (TRUE);
	MessageLoop (FALSE);	/* let's do one relinquish anyway */
	ShowEmacsCaret (FALSE);
	TakingANap = FALSE;
	if (t == 0)
	    return TRUE;
	else
	    return FALSE;
    }
    ShowEmacsCaret (TRUE);
    Sleeping = 1;		/* this gets reset by WM_TIMER processing */
    do
    {
	MessageLoop (FALSE);
	WaitMessage ();
    }
    while (Sleeping > 0);
    ShowEmacsCaret (FALSE);
    KillTimer (FrameWnd, T_SLEEP);
    TakingANap = FALSE;
    return TRUE;
}				/* TakeANap */

/* UpdateCursor:    sets the apropriate Emacs cursor shape */
/* ============                                            */

static BOOL  
UpdateCursor (struct Window * window, UINT wParam, LONG lParam)

/* this function should be called on each WM_SETCURSOR message, to
   display the appropriate cursor. It returns TRUE if all processing has
   been done (the calling window function should return TRUE). it
   returns FALSE if the default window proc sould be called. */
{
    HCURSOR hCursor;

    switch (LOWORD (lParam))
    {				/* hit-test area code as in WM_NCHITTEST */
	case HTCLIENT:
	    if (HourglassOn)
		hCursor = hHourglass;
	    else
	    {
		if ((struct Window *) wParam == FrameWnd)
		{
		    if (notquiescent)
			hCursor = hNotQuiescentCursor;
		    else
			hCursor = NULL;
		}
		else
		{		/* a screen window or the MDIClient */
		    if (notquiescent)
			hCursor = hNotQuiescentCursor;
		    else if ((struct Window *) wParam == hMDIClientWnd)
		    {
			hCursor = NULL;
		    }
		    else if (mouseflag)
		    {
			if (MouseTracking)
			    hCursor = hTrackCursor;
			else
			    hCursor = hScreenCursor;
		    }
		    else
			hCursor = 0;
		}
	    }
	    if (!hCursor)
		hCursor = LoadCursor (NULL, IDC_ARROW);
	    break;
	case HTHSCROLL:
	case HTVSCROLL:
	case HTREDUCE:
	    if (((struct Window *) wParam == hMDIClientWnd) ||
		((struct Window *) wParam == FrameWnd))
		return FALSE;
	case HTBOTTOM:
	case HTBOTTOMLEFT:
	case HTBOTTOMRIGHT:
	case HTLEFT:
	case HTRIGHT:
	case HTTOP:
	case HTTOPLEFT:
	case HTTOPRIGHT:
	case HTSIZE:
	case HTZOOM:
	    /* all those are unuseable when notquiescent */
	    if (notquiescent)
	    {
		if (HourglassOn)
		    hCursor = hHourglass;
		else
		    hCursor = hNotQuiescentCursor;
	    }
	    else
		return FALSE;
	    break;
	default:
	    return FALSE;
    }
    SetCursor (hCursor);
    return TRUE;
}				/* UpdateCursor */

/* SetHourglass:    sets or removes the hourglass cursor */
/* ============                                          */

static void  
SetHourglass (BOOL hg)

/* hg = TRUE sets the hourglass, hg = FALSE removes it */
{
    POINT Point;
    struct Window * window;

#if GRINDERS != 0
    static int PrevGrinderIndex = -1;

#endif

    if (hg == HourglassOn)
    {
#if GRINDERS == 0
	return;			/* nothing to do */
#else
	if ((hg == FALSE) || (PrevGrinderIndex == GrinderIndex))
	    return;
	PrevGrinderIndex = GrinderIndex;
#endif
    }
    HourglassOn = hg;
    GetCursorPos (&Point);
    window = WindowFromPoint (Point);
    if (window && ((window == FrameWnd) || IsChild (FrameWnd, window)))
    {
	/* the cursor is above one of MicroEMACS's windows */
#if GRINDERS != 0
	if (flickcode)
	{
	    hHourglass = hRealHourglass;
	}
	else
	{
	    hHourglass = GrinderCursor[GrinderIndex];
	}
#endif
	SetCursorPos (Point.x, Point.y);
	/* this should cause a dummy cursor movement to be processed,
	   ending up in UpdateCursor being invoked */
    }
}				/* SetHourglass */

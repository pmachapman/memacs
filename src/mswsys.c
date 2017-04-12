/* The routines in this file provide support for various OS-related
   functions under the Microsoft Windows environment on an IBM-PC or
   compatible computer.

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions.

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

#define	    OEMRESOURCE	1   /* to have access to OBM_CLOSE from windows.h */
#include    "estruct.h"
#include    "elang.h"
#include    <stdio.h>
#include    <time.h>
#include    "eproto.h"
#include    "edef.h"

#include    "mswin.h"

#if WINDOW_MSWIN32
#include <setjmp.h>
#endif

#define MAXPARAM    10      /* max command line parameters */
#define TXTSIZ      NFILEN  /* all purpose string length */
#define T_SLEEP      1      /* Sleep timer ID */

/* message codes for EmacsBroadcastMsg */
#define EMACS_PROCESS   1
#define EMACS_STARTING  1
#define EMACS_ENDING    2

/* variables */
static char     *argv[MAXPARAM];
static int      argc;

static char FrameClassName [] = PROGNAME ":frame";

#if WINDOW_MSWIN32
/* The Catch/Throw API is replaced by setjmp/longjmp */
static jmp_buf ExitCatchBuf;
#define Throw(buf,n) longjmp(buf,n)
#define Catch(buf)   setjmp(buf)
#else
static CATCHBUF ExitCatchBuf;
#endif

static int  Sleeping = 0;           /* flag for TakeANap() */
static int  TimeSlice;
static BOOL LongOperation = FALSE;  /* for longop() and GetCharacter() */
static BOOL HourglassOn = FALSE;    /* between SetHourglass & UpdateCursor */
static BOOL SystemModal = FALSE;    /* indicates that the Yes/No message
				       box should be system-modal */

static UINT EmacsBroadcastMsg;
static DWORD BroadcastVal;          /* used by the EmacsBroadcast function */

static WNDPROC  MDIClientProc;

#if GRINDERS != 0
static HCURSOR  hRealHourglass;
#endif

/* prototypes */
static void  PASCAL MessageLoop (BOOL WaitMode);
static BOOL  PASCAL UpdateCursor (HWND hWnd, UINT wParam, LONG lParam);
static void  PASCAL SetHourglass (BOOL hg);

/* timeset: return a system-dependant time string */
/* =======                                        */
char *PASCAL timeset()

{
    register char *sp;	/* temp string pointer */
    time_t buf;		/* time data buffer */

    time(&buf);
    sp = ctime(&buf);
    sp[strlen(sp)-1] = 0;
    return(sp);
}

/* longop:    to be called regularly while a long operation is in progress */
/* ========                                                                */

PASCAL longop (int f)

/* f is TRUE to set long operation status and FALSE to reset that status */
/* when a long operation is signaled at least twice, the hourglass
   cursor comes up */
/* While the long operation is in progress, this function runs the
   message loop approximately every 100 ms to yield to other
   applications and allow a reduced set of commands (among which quit)
   to be input by the user */
{
    static DWORD    LastYield;  /* time of last yield in milliseconds */
    
    if (f) {
        if (!LongOperation) {
            LongOperation = TRUE;
            LastYield = GetCurrentTime ();
        }
	else {  /* 2nd (or more) call in a row, let's yield if enough
		   time has elapsed */
	    DWORD   Time;

	    if ((Time = GetCurrentTime ()) - LastYield >= TimeSlice) {
#if GRINDERS != 0
                if (++GrinderIndex >= GRINDERS) GrinderIndex = 0;
#endif
		SetHourglass (TRUE);
	        LastYield = Time;
	        MessageLoop (FALSE);
	    }
	}
    }
    else {
	if (LongOperation) {
	    LongOperation = FALSE;
	    SetHourglass (FALSE);
	}
    }
} /* longop */

/* mlyesno: ask a yes/no question */
/* =======                        */

PASCAL mlyesno (char *prompt)

/* This function replaces the mlyesno from input.c. Instead of asking a
   question on the message line, it pops up a message box */
{
    if (MessageBox (hFrameWnd, prompt, ProgName,
                    MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION |
		    (SystemModal ? MB_SYSTEMMODAL : 0)) == IDYES) return TRUE;
    return FALSE;
} /* mlyesno */

/* mlabort: display a serious error message (proposes abort) */
/* =======                                                   */

VOID PASCAL NEAR mlabort (char *s)
{
    char    text[NSTRING];  /* hopefully sufficient! */
    
    while (*s == '%') s++;  /* remove those strange % signs in some messages */
    strcpy (text, s);
    strcat (text, "\tAbort ");
    strcat (text, ProgName);
    strcat (text, " ?");
    if (MessageBox (hFrameWnd, text, NULL,
                    MB_YESNO | MB_DEFBUTTON2 |
                    MB_ICONHAND | MB_APPLMODAL) == IDYES) {
        eexitval = -1;
        Throw (ExitCatchBuf, ABORT);
     }
} /* mlabort */

/* WinInit: all the window initialization crap... */
/* =======                                        */

BOOL FAR PASCAL WinInit (LPSTR lpCmdLine, int nCmdShow)

/* returns FALSE if failed init */
{
    WNDCLASS    wc;
    HMENU       hSysMenu;
    char        text [TXTSIZ];
    POINT       InitPos;
    int         Colors;
    int         i;
    static char HomeDir [NFILEN] = "HOME=";
    WORD        w;

    InitializeFarStorage ();
#if WINDOW_MSWIN32
    Win386Enhanced = FALSE;
    Win31API = TRUE;
#else
    Win386Enhanced = GetWinFlags () & WF_ENHANCED;
    w = LOWORD(GetVersion());
    Win31API = ((w & 0xFF) > 3) || ((w >> 8) >= 10);
#endif

    GetProfileString (ProgName, "HelpFile", "", text, TXTSIZ);
    if (text[0] != '\0') MainHelpFile = copystr (text);
    else MainHelpFile = NULL;   /* default supplied below... */
    MainHelpUsed = FALSE;
    HelpEngineFile[0] = '\0';

    i = GetModuleFileName (hEmacsInstance, text, NFILEN-9);
    for (; i >= 0; i--) {
        if (text[i] == '\\') {
            break;
        }
    }
    if (!MainHelpFile) {    /* default WinHelp file name */
        if (i > 0) text[i+1] = '\0';
        else text[0] = '\0';
        strcat (text, "mewin.hlp");
        MainHelpFile = copystr (text);
    }
    if (i > 0) {
        text[i] = '\0';
        strcat (HomeDir, text);
        putenv (HomeDir);   /* set HOME environment var to point to
			       MicroEMACS executable directory */
    }

    TimeSlice = GetProfileInt (ProgName, "TimeSlice", 100);

    Colors = GetProfileInt (ProgName, "Colors", 0);

    GetProfileString (ProgName, "InitialSize", "", text, TXTSIZ);
    strlwr (text);
    if (strstr (text, "optimize")) {
	InitPos.x = InitPos.y = 0;
    }
    else {
	InitPos.x = InitPos.y = CW_USEDEFAULT;
    }
    if (nCmdShow == SW_SHOWNORMAL) {
	if (strstr (text, "maximize")) {
	    nCmdShow = SW_SHOWMAXIMIZED;
	}
	else if (strstr (text, "minimize") || strstr (text, "icon")) {
	    nCmdShow = SW_SHOWMINNOACTIVE;
	}
    }

    /*-Register the broadcast message */
    strcpy (text, ProgName);
    strcat (text, ":Broadcast_1.1");
    EmacsBroadcastMsg = RegisterWindowMessage(text);

    /*-Register the frame window class */
    wc.style        = 0;
    wc.lpfnWndProc  = (void*)&FrameWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = FRMWNDEXTRA;
    wc.hInstance    = hEmacsInstance;
    wc.hIcon        = LoadIcon (hEmacsInstance, "AppIcon");
    wc.hCursor      = NULL;
    wc.hbrBackground= (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = "Menu";
    wc.lpszClassName= FrameClassName;
    RegisterClass (&wc);

    /*-Register the MDI child (screen) window class */
    strcpy (text, ProgName);
    strcat (text, "_screen");
    ScreenClassName = copystr(text);
    wc.style        = 0;
    wc.lpfnWndProc  = (void*)&ScrWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = SCRWNDEXTRA;
    wc.hInstance    = hEmacsInstance;
    wc.hIcon        = LoadIcon (hEmacsInstance, "ScreenIcon");
    wc.hCursor      = NULL;
    wc.hbrBackground= (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= ScreenClassName;
    RegisterClass (&wc);

    /*-Create the frame window */
    hFrameWnd = CreateWindow (FrameClassName,       /* class */
			      ProgName,             /* title */
			      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			      InitPos.x,            /* positions */
			      InitPos.y,
			      CW_USEDEFAULT,        /* dimensions */
			      CW_USEDEFAULT,
			      NULL,                 /* parent handle */
			      NULL,                 /* menu */
			      hEmacsInstance,
			      NULL);

    if (hFrameWnd == 0) return FALSE;

    if (Colors == 0) {
        /* try to detect monochrome displays */
	HDC     hDC;

	hDC = GetDC (hFrameWnd);
        ColorDisplay = (GetDeviceCaps (hDC, NUMCOLORS) > 2);
	ReleaseDC (hFrameWnd, hDC);
    }
    else {
        ColorDisplay = (Colors > 2);
    }

    ShowWindow (hFrameWnd, nCmdShow);

    hScreenCursor = LoadCursor (hEmacsInstance, "ScreenCursor");
    hTrackCursor = hScreenCursor;
    hNotQuiescentCursor = LoadCursor (hEmacsInstance,
                                      "NotQuiescentCursor");
#if GRINDERS == 0
    hHourglass = LoadCursor (NULL, IDC_WAIT);
#else
    strcpy (text, "Grinder1");
    for (i = 0; i < GRINDERS; i++) {
        text[7] = (char)i + '1';    /* this assumes GRINDERS < 10 */
        GrinderCursor[i] = LoadCursor (hEmacsInstance, text);
    }
    hRealHourglass = LoadCursor (NULL, IDC_WAIT);
    GrinderIndex = 0;
    hHourglass = GrinderCursor[0];
#endif

    in_init ();                 /* sets up the input stream */

    argv [0] = ProgName;        /* dummy program name */
    {
	register char   *s;

	argc = 1;
	s = copystr (lpCmdLine);
	while (*s != '\0') {
	    argv[argc] = s;
	    if (++argc >= MAXPARAM) goto ParsingDone;
	    while (*++s != ' ') if (*s == '\0') goto ParsingDone;
	    *s = '\0';
	    while (*++s == ' ');
	}
    }
ParsingDone:
    return TRUE;
} /* WinInit */

/* SetFrameCaption: sets the frame window's text according to the app Id */
/* ===============                                                       */

static void PASCAL  SetFrameCaption (void)
{
    char    text[sizeof(PROGNAME)+19];
    char    *t;
    int     Id;

    strcpy (text, ProgName);
    Id = GetWindowWord (hFrameWnd, GWW_FRMID);
    if (Id) {
	for (t = text; *t != '\0'; t++) ;   /* look for the end of text */
	*t++ = ' ';
	*t++ = '#';
	itoa (Id, t, 10);   /* append the App Id */
    }
    SetWindowText (hFrameWnd, text);
    if (IsIconic(hFrameWnd))  /* force redrawing of the icon title */
	SendMessage(hFrameWnd, WM_NCACTIVATE, TRUE, 0L);
} /* SetFrameCaption */

/* BroadcastEnumProc:   used by EmacsBroadcast */
/* =================                           */
BOOL EXPORT FAR PASCAL BroadcastEnumProc (HWND hWnd, LONG lParam)
{
    char    ClassName [sizeof(FrameClassName)+1];
    UINT    RetVal;
    
    if (hWnd != hFrameWnd) {
        ClassName[0] = '\0';
        GetClassName (hWnd, (LPSTR)&ClassName[0], sizeof(FrameClassName)+1);
        if (strcmp (ClassName, FrameClassName) == 0) {
            /* The enumerated window is a MicroEMACS frame */
            if (lParam != 0) {
                /*-compute max of all returned values */
                RetVal = SendMessage (hWnd, EmacsBroadcastMsg,
                                      (UINT)hFrameWnd, lParam);
		BroadcastVal = max(BroadcastVal, RetVal);
	    }
	    else {
		/*-compute number of applications */
		BroadcastVal++;
	    }
	}
    }
    return TRUE;
} /* BroadcastEnumProc */

/* EmacsBroadcast:  send a broadcast message to all Emacs applications */
/* ==============                                                      */

static DWORD PASCAL   EmacsBroadcast (DWORD MsgParam)

/* If MsgParam is not zero, the broadcast is sent as an EmacsBroadcastMsg
   to all the Emacs frame windows, except the one specified by hFrameWnd.
   The wParam of that message is hFrameWnd, lParam is set to MsgParam.
   The value returned is the highest returned value from the broadcast
   or zero if no other emacs instance was found.

 - If MsgParam is 0, this function simply returns the number of Emacs
   instances found, not counting the one identified by hFrameWnd.

*/
{
    FARPROC ProcInstance;

    ProcInstance = MakeProcInstance ((FARPROC)BroadcastEnumProc,
				     hEmacsInstance);
    BroadcastVal = 0;
    EnumWindows (ProcInstance, MsgParam);
    FreeProcInstance (ProcInstance);
    return BroadcastVal;
} /* EmacsBroadcast */

/* MDIClientSubProc:    Subclassing window proc for the MDI Client window */
/* ================                                                       */

LONG EXPORT FAR PASCAL MDIClientSubProc (HWND hWnd, UINT wMsg, UINT wParam,
				         LONG lParam)
{
    switch (wMsg) {

#if !WIN30SDK
    case WM_CREATE:
        if (Win31API) DragAcceptFiles (hWnd, TRUE);
        goto DefaultProc;

    case WM_DROPFILES:
	DropMessage (hWnd, (HDROP)wParam);
	break;
#endif

    case WM_SETCURSOR:
	if (UpdateCursor (hWnd, wParam, lParam)) return TRUE;
	goto DefaultProc;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_CHAR:
    case WM_SYSCHAR:
	if (EatKey (wMsg, wParam, lParam)) break;
        goto DefaultProc;

    case WM_PARENTNOTIFY:
        if (notquiescent) break;    /* blocks mouse selection of an MDI
				       child (except on caption clicks!)
				       */
        goto DefaultProc;
        
    default:
DefaultProc:
	return CallWindowProc (MDIClientProc, hWnd, wMsg, wParam, lParam);
    }
    return 0L;
} /* MDIClientSubProc */

/* FrameInit:   Frame window's WM_CREATE */
/* =========                             */

void FAR PASCAL FrameInit (CREATESTRUCT *cs)
{
    RECT    Rect;
    CLIENTCREATESTRUCT  ccs;
    HMENU   hMenu;
    UINT    FrameId;

#if !WIN30SDK
    if (Win31API) DragAcceptFiles (hFrameWnd, TRUE);
#endif

    /*-advertise our birth to other Emacs instances and set the frame's
       caption with an Id if necessary */
    if ((FrameId = (UINT)EmacsBroadcast (MAKELONG(EMACS_STARTING,
                                                  EMACS_PROCESS))) != 0) {
        FrameId++;
    }
    SetWindowWord (hFrameWnd, GWW_FRMID, (WORD)FrameId);
    SetFrameCaption ();

    /*-add "optimize" to system menu */
    hMenu = GetSystemMenu (hFrameWnd, 0);
    InsertMenu (hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_ENABLED, SC_OPTIMIZE,
                "&Optimize");
    
    /*-initialize fonts and Cell Metrics */
    FontInit ();

    /*-Create the MDI Client window */
    hMDIClientWnd = NULL;   /* the NULL initial value allows
			       DefFrameProc to behave properly if
			       FrameInit fails */
    ccs.hWindowMenu = GetScreenMenuHandle ();
    ccs.idFirstChild = IDM_FIRSTCHILD;
    GetClientRect (hFrameWnd, &Rect);
    hMDIClientWnd = CreateWindow ("MDICLIENT",      /* class */
				  NULL,             /* title */
				  WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL |
				      WS_HSCROLL | WS_VISIBLE,  /* style */
			          0,                   /* positions */
				  0,
				  Rect.right,          /* dimensions */
				  Rect.bottom - EmacsCM.MLHeight,
				  hFrameWnd,           /* parent handle */
				  NULL,                /* menu */
				  hEmacsInstance,
				  (LPSTR) (LPCLIENTCREATESTRUCT) &ccs);
    if (hMDIClientWnd) {
        /* we subclass the MDIClient */
        FARPROC ProcInstance;

        MDIClientProc = (WNDPROC)GetWindowLong (hMDIClientWnd, GWL_WNDPROC);
        ProcInstance = MakeProcInstance ((FARPROC)MDIClientSubProc,
				         hEmacsInstance);
        SetWindowLong (hMDIClientWnd, GWL_WNDPROC, (DWORD)ProcInstance);
    }
} /* FrameInit */

/* CloseEmacs:   handle WM_CLOSE of WM_QUERYENDSESSION messages */
/* ==========                                                   */

static BOOL  PASCAL CloseEmacs (UINT wMsg)

/* returns TRUE if emacs should exit */
{
    if (eexitflag) return TRUE;     /* emacs has already quited */
    if (wMsg == WM_QUERYENDSESSION) SystemModal = TRUE;
    if (fbusy != FWRITING) {   /* no file write in progress */
        quit (FALSE, 0);    /* check if it's ok with emacs to terminate */
    }
    else {                  /* there is, indeed a file write in progress
        MessageBeep (0);    /* wake the user up! */
        if (MessageBox (hFrameWnd, TEXT333,
                            /* "File write in progress. Quit later!" */
                        ProgName,
                        MB_OKCANCEL | MB_ICONSTOP |
		        (SystemModal ? MB_SYSTEMMODAL : 0)) == IDCANCEL) {
            quit (TRUE, 0); /* give up! */
        }
    }
    if (wMsg == WM_QUERYENDSESSION) SystemModal = FALSE;
    if (eexitflag) {        /* emacs agrees to quit */
        ClipboardCleanup ();    /* close clipboard if we had it open */
        if (fbusy != FALSE) ffclose (); /* cleanup file IOs */
        return TRUE;
    }
    update (TRUE);          /* restore the caret that quit() moved into
			       the message line */
    return FALSE;	    /* we refuse to die! */
} /* CloseEmacs */

/* ScrWndProc:  MDI child (screen) window function */
/* ==========                                      */
LONG EXPORT FAR PASCAL ScrWndProc (HWND hWnd, UINT wMsg, UINT wParam,
				   LONG lParam)
{
    switch (wMsg) {

    case WM_CREATE:
#if !WIN30SDK
        if (Win31API) DragAcceptFiles (hWnd, TRUE);
#endif
        if (!hscrollbar) ShowScrollBar (hWnd, SB_HORZ, FALSE);
        if (!vscrollbar) ShowScrollBar (hWnd, SB_VERT, FALSE);
	{   /*-init WindowWords for ScrReSize function */
	    RECT    Rect;

	    GetClientRect (hWnd, &Rect);
	    SetWindowWord (hWnd, GWW_SCRCX, (WORD)Rect.right);
	    SetWindowWord (hWnd, GWW_SCRCY, (WORD)Rect.bottom);
	}
	{   /*-setup the stuff for display.c */
	    SCREEN  *sp;

	    sp = (SCREEN*)(((MDICREATESTRUCT*)(((CREATESTRUCT*)lParam)->
					       lpCreateParams))->lParam);
	    SetWindowLong (hWnd, GWL_SCRPTR, (LONG)sp);
	    sp->s_drvhandle = hWnd;
	}
	goto DefaultProc;
	
    case WM_DESTROY:
	vtfreescr ((SCREEN*)GetWindowLong (hWnd, GWL_SCRPTR));
	break;
	
    case WM_KILLFOCUS:
	EmacsCaret (FALSE);
	goto DefaultProc;
    case WM_SETFOCUS:
	EmacsCaret (TRUE);
	goto DefaultProc;
    case WM_ACTIVATE:
	if (LOWORD(wParam)) EmacsCaret (TRUE);
	goto DefaultProc;

    case WM_MDIACTIVATE:
#if WINDOW_MSWIN32
        if ((HWND)lParam == hWnd) {
#else
	if (wParam) {
#endif
            /* this one is becoming active */
	    if (!InternalRequest) {
	        InternalRequest = TRUE;
	        select_screen ((SCREEN *) GetWindowLong (hWnd, GWL_SCRPTR),
                               FALSE);
		InternalRequest = FALSE;
	    }
	    else {
		SCREEN  *sp;

		sp = (SCREEN*)GetWindowLong (hWnd, GWL_SCRPTR);
		if (sp->s_virtual == NULL) {
		    /* this is initialization time! */
		    vtinitscr (sp, DisplayableRows (hWnd, 0, &EmacsCM),
		               DisplayableColumns (hWnd, 0, &EmacsCM));
		}
	    }
	}
	goto DefaultProc;
	
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_CHAR:
    case WM_SYSCHAR:
	if (EatKey (wMsg, wParam, lParam)) break;
        goto DefaultProc;

#if !WIN30SDK
    case WM_DROPFILES:
        DropMessage (hWnd, (HDROP)wParam);
        break;
#endif

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
	MouseMessage (hWnd, wMsg, wParam, lParam);
	break;

    case WM_VSCROLL:
    case WM_HSCROLL:
        ScrollMessage (hWnd, wMsg, LOWORD(wParam),
#if WINDOW_MSWIN32
                       (int)HIWORD(wParam));
#else
                       (int)LOWORD(lParam));
#endif
        break;

    case WM_SETCURSOR:
	if (UpdateCursor (hWnd, wParam, lParam)) return TRUE;
	goto DefaultProc;

    case WM_PAINT:
	ScrPaint (hWnd);
	break;
	
    case WM_SIZE:
	if (!IsIconic (hFrameWnd)) {
	    if (wParam == SIZEICONIC) EmacsCaret (FALSE);
	    else EmacsCaret (TRUE);
	    ScrReSize (hWnd, wParam, LOWORD(lParam), HIWORD(lParam));
	}
	goto DefaultProc;

    case WM_GETMINMAXINFO:
	GetMinMaxInfo (hWnd, (LPPOINT)lParam);
	goto DefaultProc;
	
    case WM_MOUSEACTIVATE:
	if (notquiescent) break;    /* blocks mouse selection of an MDI
				       child's caption */
	goto DefaultProc;

    case WM_SYSCOMMAND:
	switch (wParam & 0xFFF0) {
	case SC_RESTORE:
	case SC_SIZE:
	case SC_MAXIMIZE:
	case SC_MINIMIZE:
	case SC_NEXTWINDOW:
	case SC_PREVWINDOW:
	    if (notquiescent) break;    /* sizing commands disabled */
	    goto DefaultProc;
	case SC_CLOSE:
	    if (!notquiescent) {
		SCREEN  *sp;

		/* this must be done here, before any MDI mumbo-jumbo */
	        sp = (SCREEN*)GetWindowLong (hWnd, GWL_SCRPTR);
		if (sp == first_screen) {
		    cycle_screens (FALSE, 0);
                }
	        if (sp != first_screen) {
	            unlist_screen (sp);
                    free_screen (sp);
		}
		update (TRUE);  /* to restore the caret */
	    }
	    break;
	case SC_MOVE:
	    if (notquiescent) {
	        /* prevent moving an MDI client other than the already
		   active one, since it would transfer the activation */
	        if (hWnd != (HWND)SendMessage (hMDIClientWnd, WM_MDIGETACTIVE,
				               0, 0L)) break;
	    }
	    goto DefaultProc;
	default:
	    goto DefaultProc;
	}
	break;

    default:
DefaultProc:
	return DefMDIChildProc (hWnd, wMsg, wParam, lParam);
    }
    return 0L;
} /* ScrWndProc */

/* FrameWndProc:    frame window function */
/* ============                           */
LONG EXPORT FAR PASCAL FrameWndProc (HWND hWnd, UINT wMsg, UINT wParam,
				     LONG lParam)
{
    switch (wMsg) {
        
#if !WIN30SDK
    case WM_DROPFILES:
	DropMessage (hWnd, (HDROP)wParam);
	break;
#endif

    case WM_INITMENUPOPUP:
	InitMenuPopup ((HMENU)wParam, lParam);
	goto DefaultProc;

    case WM_MENUCHAR:
	{
	    LONG    Code;
	    
	    Code = DefFrameProc (hWnd, hMDIClientWnd, wMsg, wParam, lParam);
	    if (HIWORD(Code) != 0) return Code; /* matches a menu command */
	    if (EatKey (wMsg, wParam, lParam)) return MAKELONG(0,1);
	        /* MicroEMACS ate that character, close the current menu! */
	    else return Code;
	}

    case WM_COMMAND:
        if (!MenuCommand (wParam, lParam)) goto DefaultProc;
	break;

    case WM_SYSCOMMAND:
	switch (wParam & 0xFFF0) {
	case SC_RESTORE:
	    if (IsIconic (hFrameWnd)) goto DefaultProc;
	case SC_SIZE:
	case SC_MAXIMIZE:
	    if (notquiescent) break;    /* sizing commands disabled */
	    goto DefaultProc;
	case SC_OPTIMIZE:
	    if (notquiescent) break;
	    if (IsZoomed (hFrameWnd) || IsIconic (hFrameWnd)) {
	        /* must restore it first */
	        ShowWindow (hFrameWnd, SW_RESTORE);
	    }
	    {
	        int     IconHeight;
	        
#if WIN30SDK
                {
#else
	        if (Win31API) {
	            IconHeight = GetSystemMetrics (SM_CYICONSPACING);
	        }
	        else {
#endif
	            IconHeight = GetSystemMetrics (SM_CYICON) +
			         GetSystemMetrics (SM_CYCAPTION) +
	                         GetSystemMetrics (SM_CYBORDER);
	        }
	        MoveWindow (hFrameWnd, 0, 0,
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
	if (UpdateCursor (hWnd, wParam, lParam)) return TRUE;
	goto DefaultProc;

    case WM_TIMER:
	if (wParam == T_SLEEP) {
	    --Sleeping;     /* signal wake up to TakeANap() */
	    break;
	}
	goto DefaultProc;
	
    case WM_PAINT:  /* must be the message line */
	MLPaint ();
	break;

    case WM_SIZE:   /* must adjust the MDIClient's size to fit the
		       Message Line */
	MoveWindow (hMDIClientWnd, 0, 0,
                    LOWORD(lParam), HIWORD(lParam) - EmacsCM.MLHeight, TRUE);
        InvalidateRect (hFrameWnd, NULL, FALSE);
        break;

#if !WINDOW_MSWIN32
    case WM_NCLBUTTONDBLCLK:
	if (Win31API) goto DefaultProc;
	else {
           /* This corrects a Windows 3.0 bug that prevents a maximized
	       child from being closed by double-clicking the close-box.
	       See "Windows 3: A Developer's Guide" by Jeffrey M.
	       Richter, page 480 */
	    RECT    Rect;
	    HBITMAP hBmp;
	    BITMAP  Bmp;
	    LONG    MDIGetActivateResult;
	    
	    if (wParam != HTMENU) goto DefaultProc;
	    MDIGetActivateResult = SendMessage (hMDIClientWnd,
						WM_MDIGETACTIVE, 0, 0);
	    if (!HIWORD(MDIGetActivateResult)) goto DefaultProc;
	    GetWindowRect (hWnd, &Rect);
	    Rect.top += GetSystemMetrics(SM_CYCAPTION) +
			GetSystemMetrics (SM_CYFRAME);
	    Rect.left += GetSystemMetrics(SM_CXFRAME);
	    hBmp = LoadBitmap (NULL, MAKEINTRESOURCE(OBM_CLOSE));
	    GetObject (hBmp, sizeof(BITMAP), (LPSTR)(LPBITMAP)&Bmp);
	    Rect.bottom = Rect.top + Bmp.bmHeight;
	    Rect.right = Rect.left + Bmp.bmWidth / 2;
	    if (!PtInRect(&Rect, MAKEPOINT(lParam))) goto DefaultProc;
	    SendMessage(LOWORD(MDIGetActivateResult), WM_SYSCOMMAND,
                        SC_CLOSE, lParam);
	}  
	break;
#endif

    case WM_ACTIVATE:
	EmacsCaret (LOWORD(wParam));
	    /* This call matters only when the active MDI Child is
	       iconic, in which case it never receives the focus and
	       thus fails to create the caret (that should still appear
	       if we are getting input on the message line) */
	goto DefaultProc;

    case WM_CREATE:
	hFrameWnd = hWnd;
	FrameInit ((CREATESTRUCT *)lParam);
	goto DefaultProc;
	
    case WM_QUERYENDSESSION:
	if (CloseEmacs (wMsg)) return 1L;
	break;
	
    case WM_CLOSE:
        if (CloseEmacs (wMsg)) goto DefaultProc;
	break;
	
    case WM_DESTROY:
	if (MainHelpUsed) WinHelp (hFrameWnd, MainHelpFile, HELP_QUIT, NULL);
	if (HelpEngineFile[0] != '\0') WinHelp (hFrameWnd, HelpEngineFile,
                                                HELP_QUIT, NULL);
	EmacsBroadcast (MAKELONG(EMACS_ENDING,EMACS_PROCESS));
	PostQuitMessage (0);
	break;
	
    default:
        if ((wMsg == EmacsBroadcastMsg) &&
	    (HIWORD(lParam) == EMACS_PROCESS)) {
	    UINT    Id;
	    
            switch (LOWORD(lParam)) {
	    case EMACS_STARTING:
	        /* another instance of emacs is starting. If our ID was
	           0, it should now be 1 */
	        if (GetWindowWord (hFrameWnd, GWW_FRMID) == 0) {
	            SetWindowWord (hFrameWnd, GWW_FRMID, 1);
	            SetFrameCaption ();
	        }
	        break;
	    case EMACS_ENDING:
	        /* another instance of emacs is ending. If we are going to be
	           alone, let's switch our Id back to 0 */
	        Id = (UINT)EmacsBroadcast (0);
	        /* Id here is the number of Emacs applications, except for us */
	        if (Id == 1) {
		    SetWindowWord (hFrameWnd, GWW_FRMID, 0);
		    SetFrameCaption ();
	        }
	    }
	    /* send back our Id */
	    return (LONG)GetWindowWord (hFrameWnd, GWW_FRMID);
        }
DefaultProc:
	return DefFrameProc (hWnd, hMDIClientWnd, wMsg, wParam, lParam);
    }
    return 0L;
} /* FrameWndProc */

/* WinMain: Application entry point */
/* =======                          */

int PASCAL  WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    hEmacsInstance = hInstance;
    if (!WinInit (lpCmdLine, nCmdShow)) return -1;

    switch (Catch (ExitCatchBuf)) {
    case 0:
	emacs (argc, argv);
	/* If we exit through an emacs command, we pass here. Otherwise
	   (exit through a close in system menu or a windows session
	   termination), the application terminates from the call to
	   mswgetc that received the signal */
    case ABORT:
        eexitflag = TRUE;
        longop (FALSE);
	PostMessage (hFrameWnd, WM_CLOSE, 0, 0L);
	for (;;) MessageLoop (TRUE);/* go into message loop until WM_QUIT */
/*  case TRUE:
        break; */
    }
    return eexitval;
} /* WinMain */

/* ModifyCursor:    forces a WM_SETCURSOR */
/* ============                           */

static void PASCAL  ModifyCursor (void)
{
    POINT   pt;

    GetCursorPos (&pt);
    SetCursorPos (pt.x, pt.y);
} /* ModifyCursor */

/* MessageLoop: Main message loop */
/* ===========                    */

static void  PASCAL MessageLoop (BOOL WaitMode)

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
    MSG     Msg;
    BOOL    Peeking;

    ++notquiescent; /* this restrics some actions in nested calls */

    ScrollBars ();  /* hide or show as needed (may generate WM_SIZE) */
    ModifyCursor ();

    do {
        Peeking = !WaitMode;
        for (;;) {
            if(defferupdate) {
                ShowEmacsCaret (FALSE);
                update (TRUE);
                ShowEmacsCaret (TRUE);
            }
	    if (Peeking) {
	        if (!in_room (5)) break; /* input stream about to overflow */
	        if (!PeekMessage (&Msg, NULL, 0, 0, PM_REMOVE)) break;
	    }
	    else {
                longop (FALSE); /* we are going to wait for input */
	        GetMessage (&Msg, NULL, 0, 0);
	        Peeking = TRUE; /* from now on... */
	    }
	    if (Msg.message == WM_QUIT) {   /* time to leave... */
	        JettisonFarStorage ();
	        if (hEmacsFont) DeleteObject (hEmacsFont);
	        Throw (ExitCatchBuf, TRUE);
	        /* we're gone out of business ! */
	        /* **************************** */
	    }
	    if (!TranslateMDISysAccel (hMDIClientWnd, &Msg)) {
	        TranslateMessage (&Msg);
	        DispatchMessage (&Msg);
	    }
	}
    } while (WaitMode && !in_check ()); /* keep waiting if pipe is empty */

    --notquiescent;
    ModifyCursor ();
    return;
} /* MessageLoop */

/* GetInput: wait for user input (called by mswgetc from mswdrv.c) */
/* ========                                                        */

/* The returned value is the next character from the input stream */

int FAR PASCAL GetInput (void)
{
    if (!in_check ()) {
	ShowEmacsCaret (TRUE);
	MessageLoop (TRUE);
	ShowEmacsCaret (FALSE);
    }
    return in_get ();       /* return next character */
} /* GetInput */

/* TakeANap:    put emacs to sleep for a few milliseconds */
/* ========                                               */

int FAR PASCAL  TakeANap (int t)
/* this function is used by mswsleep(). It returns TRUE unless the timer
   could not be created. Note that for a null time, it simply
   relinquishes the processor */
{
    TakingANap = TRUE;
    if ((t == 0) ||
        (!SetTimer (hFrameWnd, T_SLEEP, t, (FARPROC)NULL))) {
	ShowEmacsCaret (TRUE);
	MessageLoop (FALSE);    /* let's do one relinquish anyway */
	ShowEmacsCaret (FALSE);
	TakingANap = FALSE;
	if (t == 0) return TRUE;
        else return FALSE;
    }
    ShowEmacsCaret (TRUE);
    Sleeping = 1;   /* this gets reset by WM_TIMER processing */
    do {
        MessageLoop (FALSE);
        WaitMessage ();
    } while (Sleeping > 0);
    ShowEmacsCaret (FALSE);
    KillTimer (hFrameWnd, T_SLEEP);
    TakingANap = FALSE;
    return TRUE;
} /* TakeANap */

/* UpdateCursor:    sets the apropriate Emacs cursor shape */
/* ============                                            */

static BOOL  PASCAL UpdateCursor (HWND hWnd, UINT wParam, LONG lParam)

/* this function should be called on each WM_SETCURSOR message, to
   display the appropriate cursor. It returns TRUE if all processing has
   been done (the calling window function should return TRUE). it
   returns FALSE if the default window proc sould be called. */
{
    HCURSOR hCursor;

    switch (LOWORD(lParam)) {   /* hit-test area code as in WM_NCHITTEST */
    case HTCLIENT:
        if (HourglassOn) hCursor = hHourglass;
	else {
	    if ((HWND)wParam == hFrameWnd) {
	        if (notquiescent) hCursor = hNotQuiescentCursor;
	        else hCursor = NULL;
	    }
	    else {  /* a screen window or the MDIClient */
		if (notquiescent) hCursor = hNotQuiescentCursor;
		else if ((HWND)wParam == hMDIClientWnd) {
		    hCursor = NULL;
		}
		else if (mouseflag) {
		    if (MouseTracking) hCursor = hTrackCursor;
		    else hCursor = hScreenCursor;
		}
		else hCursor = 0;
	    }
	}
	if (!hCursor) hCursor = LoadCursor (NULL, IDC_ARROW);
	break;
    case HTHSCROLL:
    case HTVSCROLL:
    case HTREDUCE:
        if (((HWND)wParam == hMDIClientWnd) ||
            ((HWND)wParam == hFrameWnd)) return FALSE;
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
        if (notquiescent) {
            if (HourglassOn) hCursor = hHourglass;
	    else hCursor = hNotQuiescentCursor;
        }
        else return FALSE;
	break;
    default:
	return FALSE;
    }
    SetCursor (hCursor);
    return TRUE;
} /* UpdateCursor */

/* SetHourglass:    sets or removes the hourglass cursor */
/* ============                                          */

static void  PASCAL SetHourglass (BOOL hg)

/* hg = TRUE sets the hourglass, hg = FALSE removes it */
{
    POINT   Point;
    HWND    hWnd;
#if GRINDERS != 0
    static  int PrevGrinderIndex = -1;
#endif

    if (hg == HourglassOn) {
#if GRINDERS == 0
        return;   /* nothing to do */
#else
        if ((hg == FALSE) || (PrevGrinderIndex == GrinderIndex)) return;
        PrevGrinderIndex = GrinderIndex;
#endif
    }
    HourglassOn = hg;
    GetCursorPos (&Point);
    hWnd = WindowFromPoint (Point);
    if (hWnd && ((hWnd == hFrameWnd) || IsChild (hFrameWnd, hWnd))) {
        /* the cursor is above one of MicroEMACS's windows */
#if GRINDERS != 0
        if (flickcode) {
            hHourglass = hRealHourglass;
	}
        else {
            hHourglass = GrinderCursor[GrinderIndex];
        }
#endif
	SetCursorPos (Point.x, Point.y);
	/* this should cause a dummy cursor movement to be processed,
	   ending up in UpdateCursor being invoked */
    }
} /* SetHourglass */

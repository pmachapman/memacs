/* This file provides definitions for the C source and resource scripts
   for support of operation under the Microsoft Windows environment on
   an IBM-PC or compatible computer.

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

/* compile flags */
#define GRINDERS    8   /* 0 to use the hourglass, 8 to use the animated
                           grinder (8 is the number of animation steps) */
                           
#define WIN30SDK   0   /* 1 to be compatible with the Windows 3.0 SDK */

#define CARETSHAPE  0   /* 2 to have a cell-sized caret when within a
			   screen, 1 for a vertical bar, 0 for an
                           horizontal bar */

#define MEMTRACE    1   /* adds a debugging trace to mswmem.c (used only
                           if SUBALLOC (below) is 1) */

/* The SUBALLOC #define implements malloc/free/realloc by segment
   suballocation routines (needed for Borland C 2.0, but not necessary
   under Borland C 3.0). Not needed under MSC 7.0 either. */
#if WINDOW_MSWIN32
	#define SUBALLOC    0
#else
	#if     TURBO
		#if         defined(__BORLANDC__) && (__TURBOC__<0x300)
			#define SUBALLOC    1   /* BC++ 2.0 */
		#else
			#define SUBALLOC    0   /* TC++ 1.0 or BC++ 3.x */
		#endif	/* defined(__BORLANDC__) && (__TURBOC__<0x300) */
	#endif	/* TURBO */

	#if     MSC
		#if         _MSC_VER < 700
			#define SUBALLOC    1
		#else
			#define SUBALLOC    0
		#endif	/* _MSC_VER < 700 */
	#endif	/* MSC */

#endif	/* WINDOW_MSWIN32 */

#if !WIN30SDK
#include    <shellapi.h>
#endif

#include    "mswrid.h"  /* contains all the Resource IDs */

/* SDK-compatibility (Win3.0, Win3.1, WIN32) */
#if WIN30SDK
#define UINT    WORD
#define MDITILE_VERTICAL    0x0000
#define MDITILE_HORIZONTAL  0x0001
#endif
#if WINDOW_MSWIN32
#define LPDATA(p) ((LPARAM)(LPSTR)(p))
#define HDROP HANDLE
#define NOTIFICATION_CODE HIWORD(wParam)    /* for WM_COMMAND parsing */
#else
#define LPDATA(p) ((LPSTR)(p))
#define NOTIFICATION_CODE HIWORD(lParam)
#define WNDPROC FARPROC
#endif

/* macros */
#define MLSIZE  NSTRING         /* message line buffer size */

/* offsets for the screen windows extra bytes */
#define GWL_SCRPTR  0                   /* SCREEN structure pointer */
#define GWW_SCRCX   (GWL_SCRPTR+sizeof(LONG))   /* client area width */
#define GWW_SCRCY   (GWW_SCRCX+sizeof(WORD))       /* client area height */
#define SCRWNDEXTRA (GWW_SCRCY+sizeof(WORD))

/* offsets for the frame window extra bytes */
#define GWW_FRMID   0                   /* Id seen by other emacs apps */
#define FRMWNDEXTRA (GWW_FRMID+sizeof(WORD))

/* structures */
typedef struct  CellMetrics {   /* coordinate-related parameters of a font */
    short   OffsetX, OffsetY;  /* offset of column 0 or row 0, relative
				  to the client area upper left corner */
    short   LeadingY, HalfLeadingY; /* external leading between rows */
    short   SizeX, SizeY;       /* character cell size */
    short   MLHeight;           /* height of the message line */
} CellMetrics;

#ifdef  termdef     /* in mswdrv.c only */
char    ProgName [] = PROGNAME;
int     CurrentRow = 0;
int     CurrentCol = 0;
BOOL    MouseTracking = FALSE;
BOOL    InternalRequest = FALSE;
BOOL    TakingANap = FALSE;

/* Global uninitialized variables */
HWND    hFrameWnd;          /* main (frame) window */
HWND    hMDIClientWnd;      /* MDI client window */
HANDLE  hEmacsInstance;     /* module instance */
BOOL    Win386Enhanced;     /* Windows 386 enhanced mode */
BOOL    Win31API;           /* Windows 3.1 or later version */
HCURSOR hScreenCursor, hTrackCursor, hNotQuiescentCursor, hHourglass;
char    *ScreenClassName;   /* MDI childs class name */
char    *MLBuf;             /* message line buffer */
HFONT   hEmacsFont;         /* font used for display */
CellMetrics     EmacsCM;    /* cell metrics of that font */


#if GRINDERS != 0
HCURSOR GrinderCursor [GRINDERS];/* handles for the grinder animation */
int     GrinderIndex;
#endif

char    *MainHelpFile;      /* Emac's help file name and useage flag */
BOOL    MainHelpUsed;

char    HelpEngineFile [NFILEN];/* user help file and useage flag */
BOOL    ColorDisplay;       /* TRUE if the display is color-capable */
#else
extern char ProgName [DUMMYSZ];/* used all over the place for captions,
			          etc... */
extern int  CurrentRow;
extern int  CurrentCol;     /* caret positions (in text coordinates) */
extern BOOL MouseTracking;  /* TRUE if mouse in tracking/dragging mode */
extern BOOL InternalRequest;
    /* TRUE for a request that originates from the mswxxx modules. This
       is meant to avoid infinite recursions for requests that could
       come from both MS-Windows or the editor's core (for instance
       resizing, screen activation...) */
extern BOOL TakingANap;     /* TRUE during execution of TakeANap() */

/* Global uninitialized variables */
extern HWND    hFrameWnd;          /* main (frame) window */
extern HWND    hMDIClientWnd;      /* MDI client window */
extern HANDLE  hEmacsInstance;     /* module instance */
extern BOOL    Win386Enhanced;     /* Windows 386 enhanced mode */
extern BOOL    Win31API;           /* Windows 3.1 or later version */
extern HCURSOR hScreenCursor, hTrackCursor, hNotQuiescentCursor, hHourglass;
extern char    *ScreenClassName;   /* MDI childs class name */
extern char    *MLBuf;             /* message line buffer */
extern HFONT   hEmacsFont;         /* font used for display */
extern CellMetrics     EmacsCM;    /* cell metrics of that font */

#if GRINDERS != 0
extern HCURSOR GrinderCursor [GRINDERS];/* handles for the grinder animation */
extern int     GrinderIndex;
#endif

extern char    *MainHelpFile;      /* Emac's help file name and useage flag */
extern BOOL    MainHelpUsed;

extern char    HelpEngineFile [NFILEN];/* user help file and useage flag */
extern BOOL    ColorDisplay;       /* TRUE if the display is color-capable */
#endif

/* input stream access functions */
void in_init (void);
BOOL in_room (int n);
BOOL in_check (void);
void in_put (int event);
int  in_get (void);

/* Windows-implementation specific functions */
LONG EXPORT FAR PASCAL ScrWndProc (HWND hWnd, UINT wMsg, UINT wParam,
				   LONG lParam);
LONG EXPORT FAR PASCAL FrameWndProc (HWND hWnd, UINT wMsg, UINT wParam,
				     LONG lParam);
int FAR PASCAL GetInput (void);
int FAR PASCAL TakeANap (int t);

void FAR PASCAL GenerateMenuSeq (UINT ID);
KEYTAB * FAR PASCAL FindKeyBinding (void *Func);
void FAR PASCAL InitMenuPopup (HMENU hMenu, LONG lParam);
BOOL FAR PASCAL MenuCommand (UINT wParam, LONG lParam);
HMENU FAR PASCAL GetScreenMenuHandle (void);

BOOL FAR PASCAL EatKey (UINT MsgCode, UINT wParam, LONG lParam);
void FAR PASCAL MouseMessage (HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
#if !WIN30SDK
void FAR PASCAL DropMessage (HWND hWnd, HANDLE hDrop);
#endif

void FAR PASCAL BuildCellMetrics (CellMetrics *cm, HFONT hFont);
void FAR PASCAL InvalidateCells (HWND hWnd, int leftcol, int toprow,
                                 int rightcol, int bottomrow);
void FAR PASCAL MinimumClientSize (HWND hWnd, int NCols, int NRows,
				   int *Width, int *Height);
int FAR PASCAL DisplayableRows (HWND hWnd, int Height, CellMetrics *cm);
int FAR PASCAL DisplayableColumns (HWND hWnd, int Width, CellMetrics *cm);
void FAR PASCAL EmacsCaret (BOOL Show);
void FAR PASCAL MoveEmacsCaret (HWND hWnd, int col, int row);
void FAR PASCAL ShowEmacsCaret (BOOL Show);
void FAR PASCAL CellToClient (HWND hWnd, POINT Cell, LPPOINT Client);
void FAR PASCAL ClientToCell (HWND hWnd, POINT Client, LPPOINT Cell);
void FAR PASCAL GetMinMaxInfo (HWND hWnd, LPPOINT rgpt);
BOOL FAR PASCAL ScrReSize (HWND hWnd, UINT wParam, WORD cx, WORD cy);
void FAR PASCAL ScrPaint (HWND hWnd);
void FAR PASCAL MLPaint (void);

BOOL FAR PASCAL InMessageLine (void);

void FAR PASCAL ClipboardCleanup (void);
void FAR PASCAL ScrollMessage (HWND hWnd, UINT wMsg, WORD ScrlCode, int Pos);
void FAR PASCAL ScrollBars (void);

int FAR PASCAL SetWorkingDir (void);

void FAR PASCAL InitializeFarStorage (void);
void FAR PASCAL JettisonFarStorage (void);

HFONT FAR PASCAL SelectFont (HDC hDC, HFONT hFont);
BOOL FAR PASCAL PickEmacsFont (void);
void FAR PASCAL FontInit (void);

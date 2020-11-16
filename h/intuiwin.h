/* This file provides definitions for the C source and resource scripts
   for support of operation under the Amiga Intuition environment

   It should not be compiled if the WINDOW_AMIGA symbol is not set */

/* compile flags */
#define GRINDERS    0   /* 0 to use the hourglass, 8 to use the animated
                           grinder (8 is the number of animation steps) */
                           
#define CARETSHAPE  0   /* 2 to have a cell-sized caret when within a
			   screen, 1 for a vertical bar, 0 for an
                           horizontal bar */

/* #include    "mswrid.h"  /* contains all the Resource IDs */

typedef struct Window *HWND;
typedef struct Menu *HMENU;
typedef char *LPSTR;
typedef ULONG LPARAM;
typedef ULONG COLORREF;
typedef struct
{
	WORD left,top;
	WORD bottom,right;
} Rect;


#define LPDATA(p) ((LPARAM)(LPSTR)(p))
#define HDROP HANDLE
#define NOTIFICATION_CODE HIWORD(wParam)    /* for WM_COMMAND parsing */

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
HWND    FrameWnd;          /* main (frame) window */
HWND    hMDIClientWnd;      /* MDI client window */
HANDLE  hEmacsInstance;     /* module instance */
HCURSOR hScreenCursor, hTrackCursor, hNotQuiescentCursor, hHourglass;
char    *ScreenClassName;   /* MDI childs class name */
char    *MLBuf;             /* message line buffer */
HFONT   hEmacsFont;         /* font used for display */
CellMetrics     EmacsCM;    /* cell metrics of that font */


#if GRINDERS != 0
HCURSOR GrinderCursor [GRINDERS];/* handles for the grinder animation */
int     GrinderIndex;
#endif

char    *MainHelpFile;      /* Emac's help file name and usage flag */
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
extern HWND    FrameWnd;          /* main (frame) window */
extern HWND    hMDIClientWnd;      /* MDI client window */
extern HANDLE  hEmacsInstance;     /* module instance */
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
LONG ScrWndProc (HWND hWnd, UINT wMsg, UINT wParam,
				   LONG lParam);
LONG FrameWndProc (HWND hWnd, UINT wMsg, UINT wParam,
				     LONG lParam);
int GetInput (void);
int TakeANap (int t);

void GenerateMenuSeq (UINT ID);
KEYTAB * FindKeyBinding (void *Func);
void InitMenuPopup (HMENU hMenu, LONG lParam);
BOOL MenuCommand (UINT wParam, LONG lParam);
HMENU GetScreenMenuHandle (void);

BOOL EatKey (UINT MsgCode, UINT wParam, LONG lParam);
void MouseMessage (HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
#if !WIN30SDK
void DropMessage (HWND hWnd, HANDLE hDrop);
#endif

void BuildCellMetrics (CellMetrics *cm, HFONT hFont);
void InvalidateCells (HWND hWnd, int leftcol, int toprow,
                                 int rightcol, int bottomrow);
void MinimumClientSize (HWND hWnd, int NCols, int NRows,
				   int *Width, int *Height);
int DisplayableRows (HWND hWnd, int Height, CellMetrics *cm);
int DisplayableColumns (HWND hWnd, int Width, CellMetrics *cm);
void EmacsCaret (BOOL Show);
void MoveEmacsCaret (HWND hWnd, int col, int row);
void ShowEmacsCaret (BOOL Show);
void CellToClient (HWND hWnd, POINT Cell, LPPOINT Client);
void ClientToCell (HWND hWnd, POINT Client, LPPOINT Cell);
void GetMinMaxInfo (HWND hWnd, LPPOINT rgpt);
BOOL ScrReSize (HWND hWnd, UINT wParam, WORD cx, WORD cy);
void ScrPaint (HWND hWnd);
void MLPaint (void);

BOOL InMessageLine (void);

void ClipboardCleanup (void);
void ScrollMessage (HWND hWnd, UINT wMsg, WORD ScrlCode, int Pos);
void ScrollBars (void);

int SetWorkingDir (void);

void InitializeFarStorage (void);
void JettisonFarStorage (void);

HFONT SelectFont (HDC hDC, HFONT hFont);
BOOL PickEmacsFont (void);
void FontInit (void);


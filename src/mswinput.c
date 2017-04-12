/* The routines in this file provide keyboard and mouse input support
   under the Microsoft Windows environment on an IBM-PC or compatible
   computer.

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions.

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

#include    "estruct.h"
#include    "elang.h"
#include    <stdio.h>
#include    <time.h>
#include    "eproto.h"
#include    "edef.h"

#include    "mswin.h"

#define IBUFSIZE    64      /* this must be a power of 2 */

static unsigned char in_buf[IBUFSIZE];  /* input character buffer */
static int in_next = 0;                 /* pos to retrieve next input character */
static int in_last = 0;                 /* pos to place most recent input character */
static int in_free = 0;                 /* number of unused char entries */

static int vk_at = -1;                  /* VK code for '@' key */

/* in_init: initialize the input stream buffer */
/* =======                                     */

void    in_init (void)
{
    /*-initialize input stream buffer */
    in_next = in_last = 0;
    in_free = IBUFSIZE;
    
    /*-initialize vk_at to try to support the popular ^@ */
    vk_at = VkKeyScan ('@');
    if ((vk_at >> 8) != 1) {
        /* not a shifted key. no cigar! */
        vk_at = -1;
    }
} /* in_init */

/* in_room: is there enough room for n chars ? */
/* =======                                     */

BOOL    in_room (int n)
{
    if (n <= in_free) return TRUE;
    else {
        MessageBeep (0);
	return FALSE;
    }
} /* in_room */

/* in_check:    is the input buffer non empty? */
/* ========                                    */

BOOL    in_check (void)
{
    if (in_next == in_last)
        return(FALSE);
    else
        return(TRUE);
} /* in_check */

/* in_put:  enter an event into the input buffer */
/* ======                                        */

void    in_put (int event)
{
    in_buf[in_last++] = event;
    in_last &= (IBUFSIZE - 1);
    --in_free;
} /* in_put */

/* in_get:  get an event from the input buffer */
/* ======                                      */

int in_get (void)
{
    register int event;	/* event to return */

    event = in_buf[in_next++];
    in_next &= (IBUFSIZE - 1);
    ++in_free;
    return(event);
} /* in_get */

/* typahead:    TRUE if there are typeahead characters in the input stream */
/* ========                                                                */

PASCAL typahead (void)
{
    if (in_check()) return TRUE;
    else return FALSE;
} /* typahead */

/* EatKey: processes WM_(SYS)KEYxxx and WM_(SYS/MENU)CHAR messages */
/* ======                                                          */

BOOL FAR PASCAL EatKey (UINT MsgCode, UINT wParam, LONG lParam)

/* This function must be called for each WM_(SYS)KEYxxx or
   WM_(SYS/MENU)CHAR message. It returns TRUE if it has taken possesion
   of the keyboard action. In that case, the message processing should
   be terminated */
{
    int     evt = -1;       /* -1 means: key not for emacs */
    WORD    prefix = 0;
    WORD    Key;

    if (IsIconic (hFrameWnd)) return FALSE;   /* no input while fully
						 iconic */
    Key = LOWORD(wParam);
    
    switch (MsgCode) {
        
    case WM_KEYDOWN:
KeyDown:
        /*-process the non-ascii keys (Page-up, Page-down, End, Home,
	   Arrows, Insert, Delete, function keys) */
	prefix |= SPEC;
	if (GetKeyState (VK_CONTROL) < 0) prefix |= CTRL;
	if (GetKeyState (VK_SHIFT) < 0) prefix |= SHFT;
	switch (Key) {

	case VK_HOME:
	    evt = '<';
	    break;
	case VK_UP:
	    evt = 'P';
	    break;
	case VK_PRIOR:  /* Page-up */
	    evt = 'Z';
	    break;
	case VK_LEFT:
	    evt = 'B';
	    break;
	case VK_RIGHT:
	    evt = 'F';
	    break;
	case VK_END:
	    evt = '>';
	    break;
	case VK_DOWN:
	    evt = 'N';
	    break;
	case VK_NEXT:   /* Page-down */
	    evt = 'V';
	    break;
	case VK_INSERT:
	    evt = 'C';
	    break;
	case VK_DELETE:
	    evt = 'D';
	    break;
	default:
	    if ((Key >= VK_F1) && (Key <= VK_F10)) {    /* function key */
		if (Key == VK_F10) evt = '0';
		else evt = Key - VK_F1 + '1';
	    }
	    else if ((vk_at > 0) && (Key == LOBYTE(vk_at)) &&
                     ((prefix & (SHFT | CTRL)) == CTRL)) {
                /* we assume a ^@ or A-^@ */
                prefix &= ALTD;
                evt = 0;
            }
	    break;
	}
	break;

    case WM_SYSKEYDOWN:
        /*-process ALT'ed function keys */
        if (!(lParam & 0x20000000)) goto KeyDown;
            /* for some reason, plain F10 arrives as a SYS message ! */
        if (Key == VK_F4) return FALSE; /* standard accelerator for
					   Frame's SC_CLOSE */
        prefix |= ALTD;
        goto KeyDown;
        
    case WM_CHAR:
        /*-process regular ASCII, with CTRL & SHFT embedded in event */ 
	evt = Key;
	break;

    case WM_SYSCHAR:
        if (lParam & 0x20000000) {  /*-process ALT'ed ASCII char */
	    evt = upperc((char)Key);
	    prefix = ALTD;
	    if (getbind(ALTD | evt) == NULL) {
	        /* that key is not bound, let's ignore it to have
		   Windows check for a menu-bar accelerator */
	        evt = -1;
	    }
	}
	break;

    case WM_MENUCHAR:
#if WINDOW_MSWIN32
	if (!(HIWORD(wParam) & MF_POPUP) && (GetKeyState (VK_MENU) < 0)) {
#else
	if (!(LOWORD(lParam) & MF_POPUP) && (GetKeyState (VK_MENU) < 0)) {
#endif
	    /* it is an ALT'ed char that does not match any accelerator */
	    evt = upperc((char)Key);
	    prefix = ALTD;
	}
	break;
    }
    
    if (evt == -1) return FALSE; /* nothing of interest ! */

    if (in_room (3)) {
	if ((prefix != 0) || (evt == 0)) {
	    in_put (0);
	    in_put (prefix >> 8);
	}
	in_put (evt);
    }
    return TRUE;
} /* EatKey */

/* PutMouseMessage: feeds a mouse message into the in_put queue */
/* ===============                                              */

void PASCAL    PutMouseMessage (UINT wMsg, UINT wParam, POINT Position)

{
    char    c;
    int     prefix;

    if (!mouseflag) return; /* mouse input is disabled */
    
    switch (wMsg) {
    case WM_LBUTTONDOWN:
	c = 'a';
	break;
    case WM_LBUTTONUP:
	c = 'b';
	break;
    case WM_MBUTTONDOWN:
	c = 'c';
	break;
    case WM_MBUTTONUP:
	c = 'd';
	break;
    case WM_RBUTTONDOWN:
	c = 'e';
	break;
    case WM_RBUTTONUP:
	c = 'f';
	break;
    case WM_MOUSEMOVE:
	/* mouse move events being suppressed? */
	if (((mouse_move == 1) &&
	    ((mmove_flag == FALSE) || (!MouseTracking))) ||
	    (mouse_move == 0))
		return;
    	c = 'm';
    	break;
    default:
	return; /* should not happen, but let's be safe! */
    }
    prefix = MOUS;
    if (wParam & MK_SHIFT)
	prefix |= SHFT;
    if (wParam & MK_CONTROL)
	prefix |= CTRL;
    in_put (0);
    in_put (prefix >> 8);
    in_put ((unsigned char)Position.x);
    in_put ((unsigned char)Position.y);
    in_put (c);
} /* PutMouseMessage */

/* MouseMessage:    handles client area mouse messages */
/* ============                                        */

void FAR PASCAL MouseMessage (HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
    POINT           Position;
    static POINT old_Position = {-1, -1};/* last position reported by movement */

    Position.x = (short)LOWORD(lParam);
    Position.y = (short)HIWORD(lParam);
    ClientToCell (hWnd, Position, &Position);
    
    switch (wMsg) {
    case WM_MOUSEMOVE:
#if	0
	if (MouseTracking && !notquiescent) {
            MoveEmacsCaret (hWnd, Position.x, Position.y);
        }
#endif
	if ((old_Position.x != Position.x) || (old_Position.y != Position.y)) {
		if (in_room (5))
			PutMouseMessage (wMsg, wParam, Position);
		old_Position.x = Position.x;
		old_Position.y = Position.y;
	}
	break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        if (in_room (5) && mouseflag) {
	    PutMouseMessage (wMsg, wParam, Position);
	    MouseTracking = TRUE;
	    SetCapture (hWnd);
	    if (!notquiescent) MoveEmacsCaret (hWnd, Position.x, Position.y);
	}
	break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        if (MouseTracking) {
	    if (in_room (5)) PutMouseMessage (wMsg, wParam, Position);
	    MouseTracking = FALSE;
	    ReleaseCapture ();  /* let go of the mouse */
	    if (!notquiescent) {
                MoveEmacsCaret (hWnd, CurrentCol, CurrentRow);
                    /* restore the caret */
            }
	}
	break;

    default:
        break;
    }
} /* MouseMessage */

#if !WIN30SDK
/* DropMessage:    handles WM_DROPFILES messages */
/* ===========                                   */

void FAR PASCAL DropMessage (HWND hWnd, HDROP hDrop)

/* Generates a MS! keystroke. $xpos/$ypos contain the position where
   the files were dropped, or -1 (actually, 255) if the drop occurred
   outside a screen client area. The invisible buffer "Dropped files" is
   filled with the list of files dropped (one per line), except the first
   line which is either empty or contains the name of the screen where the
   drop occurred. */
{
    BUFFER  *DropBuf;
    POINT   Point;

    DropBuf = bfind("Dropped files", TRUE, BFINVS);
    if (DropBuf && bclear (DropBuf)) {
        if ((hWnd == hFrameWnd) || (hWnd == hMDIClientWnd)) {
            Point.x = -1;
            Point.y = -1;
            addline (DropBuf, "");  /* no screen name */
        }
        else {
            /* the drop occured on a screen */
            addline (DropBuf, ((SCREEN*)GetWindowLong (hWnd, GWL_SCRPTR))->s_screen_name);
            if (DragQueryPoint (hDrop, &Point)) {
                ClientToCell (hWnd, Point, &Point);
            }
            else {
                /* not within the client area! */
                Point.x = -1;
                Point.y = -1;
            }
        }
        {   /*-complete the DropBuf with the file name list */
            char    FileName [NFILEN];
            WORD    Number;
            WORD    i;

            Number = DragQueryFile (hDrop, -1, NULL, 0);
            for (i = 0; i < Number; i++) {
                DragQueryFile (hDrop, i, FileName, NFILEN);
                addline (DropBuf, FileName);
            }
            DropBuf->b_dotp = lforw(DropBuf->b_linep);
            DropBuf->b_doto = 0;
        }
        /*-feed the pseudo-keystroke into the in_put stream */
        in_put (0);
        in_put (MOUS >> 8);
        in_put ((unsigned char)Point.x);
        in_put ((unsigned char)Point.y);
        in_put ('!');
    }
    DragFinish (hDrop);
} /* DropMessage */
#endif

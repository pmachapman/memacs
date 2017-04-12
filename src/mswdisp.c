/* The routines in this file provide display support under
   the Microsoft Windows environment on an IBM-PC or compatible
   computer.

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

/* The functions in this module are mostly concerned with the mapping
   between character cells and client coordinates. */

#include    "estruct.h"
#include    "elang.h"
#include    <stdio.h>
#include    <time.h>
#include    "eproto.h"
#include    "edef.h"

#include    "mswin.h"

#if     COLOR
/* palette table. It is initialized for the default colors, but can be
   changed by the spal function */
static COLORREF EmacsPalette [16] = {
    0x00000000,     /* black */
    0x00000080,     /* red */
    0x00008000,     /* green */
    0x0000FFFF,     /* yellow */
    0x00800000,     /* blue */
    0x00800080,     /* magenta */
    0x00808000,     /* cyan */
    0x00C0C0C0,     /* grey (light) */
    0x00808080,     /* gray (dark) */
    0x000000FF,     /* light red */
    0x0000FF00,     /* light green */
    0x0080FFFF,     /* light yellow*/
    0x00FF0000,     /* light  blue */
    0x00800080,     /* light magenta*/
    0x00FFFF00,     /* light cyan */
    0x00FFFFFF      /* white */
};
#endif

static HWND hCaretWnd = 0;          /* window where the caret belongs */
static int  CaretVisible = 0;       /* the caret should be visible if not 0 */
static int  CaretCol, CaretRow;     /* caret position */
/* Text Metrics values (from the CellMetrics structure) are used as follows:

    -------------------------------- Client area upper boundary
    |          ^
    |       OffsetY
    |          v
    |          ^
    |      HalfLeadingY
    |          v
    |       ---------- ----------
    |<---->|          |          |  ^
  OffsetX  |   cell   |   cell   |  |    . . .
    |      |    0,0   |    1,0   | SizeY
    |      |          |          |  |
    |      |          |          |  v
    |       ---------- ----------
    |                         ^
    |       <- SizeX->     LeadingY = 2 * HalfLeadingY
    |                         v
    |       ---------- ----------
    |      |          |          |
    |      |   cell   |   cell   |
    |      |    0,1   |    1,1   |
    |      |          |          |
    |
    |                ...
    |
   Client area left boundary
*/

/* BuildCellMetrics:   fills a CellMetrics structure from a font description */
/* ================                                                          */

void FAR PASCAL BuildCellMetrics (CellMetrics *cm, HFONT hFont)
{
    HDC     hDC;
    TEXTMETRIC Metrics;
    HANDLE  hPrevFont;

    hDC = GetDC (hFrameWnd);
    hPrevFont = SelectFont (hDC, hFont);
    GetTextMetrics (hDC, &Metrics);
    SelectObject (hDC, hPrevFont);
    ReleaseDC (hFrameWnd, hDC);
    cm->SizeX = Metrics.tmAveCharWidth;
    if (cm->SizeX == 0) cm->SizeX = 1;  /* ATM gives 0 sometimes !!! */
    cm->SizeY = Metrics.tmHeight;
    if (cm->SizeY == 0) cm->SizeY = 1;
    cm->HalfLeadingY = Metrics.tmExternalLeading / 2;
    cm->OffsetX = cm->SizeX / 4;
    if ((cm->OffsetY = (cm->SizeY / 8) - cm->HalfLeadingY) < 0) {
	cm->OffsetY = 0;
	/* for a reasonable upper boundary separation, we want
	   (SizeY / 8) <= OffsetY + HalfLeadingY */
    }
    cm->LeadingY = 2 * cm->HalfLeadingY;
    cm->MLHeight = cm->SizeY + cm->LeadingY + (2 * cm->OffsetY) +
                   GetSystemMetrics(SM_CYBORDER);
} /* BuildCellMetrics */

/* InvalidateCells: marks character cells for repaint */
/* ===============                                    */

void FAR PASCAL InvalidateCells (HWND hWnd, int leftcol, int toprow,
                                 int rightcol, int bottomrow)
{
    RECT    Rect;           /* used for Cell and Client coordinates */
    RECT    ClientRect;     /* MDI window client area */
    POINT   MaxBottomRight; /* Cell coordinates of bottom right of MDI
			       window */

    GetClientRect (hWnd, &ClientRect);
    ClientToCell (hWnd, *(POINT *)&ClientRect.right, &MaxBottomRight);

    Rect.left = leftcol;
    Rect.top = toprow;
    CellToClient (hWnd, *(POINT *)&Rect.left, (POINT *)&Rect.left);
    if (leftcol == 0) Rect.left = 0;
    else Rect.left -= EmacsCM.SizeX / 2;    /* see Rect.right below... */
    if (toprow == 0) Rect.top = 0;

    Rect.right = rightcol + 1;
    Rect.bottom = bottomrow + 1;
    CellToClient (hWnd, *(POINT *)&Rect.right, (POINT *)&Rect.right);
    if (rightcol + 1 >= MaxBottomRight.x) Rect.right = ClientRect.right;
    else Rect.right += EmacsCM.SizeX / 2;
        /* this adjustment is done to avoid left-over pixels caused by
           characters which display an overhang outside their cells
           (for instance: w & W in Courier New). The corresponding
           adjustment is done on Rect.left a few lines above */
    if (bottomrow + 1 >= MaxBottomRight.y) Rect.bottom = ClientRect.bottom;

    InvalidateRect (hWnd, &Rect, FALSE);
        /* the background does not need erasing since Paint will
	   completely fill the invalid rectangle */
} /* InvalidateCells */

/* MinimumClientSize:  computes the minimum client area size */
/* =================                                         */

void FAR PASCAL MinimumClientSize (HWND hWnd, int NCols, int NRows,
				   int *Width, int *Height)

/* The values pointed by Height and Width are set to the smallest value
   that allows displaying NRows rows and NCols columns. A NULL pointer
   disables the computing of the associated value */
{
    if (Height) *Height = (2 * EmacsCM.OffsetY) +
                          (NRows * (EmacsCM.SizeY + EmacsCM.LeadingY));
    if (Width) *Width = (2 * EmacsCM.OffsetX) + (NCols * EmacsCM.SizeX);
} /* MinimumClientSize */

/* DisplayableRows: returns the number of rows displayable in the client area */
/* ===============                                                            */

int FAR PASCAL DisplayableRows (HWND hWnd, int Height, CellMetrics *cm)

/* Heigh is the hypothetic heigh of the client area. If this parameter
   is 0, the real client area is measured. If it is negative, the
   maximal client area (maximized child in a maximized frame) is used.
   */
{
    RECT    Rect;
    int     x;

    if (Height == 0) GetClientRect (hWnd, &Rect);
    else {
	if (Height > 0) {
	    Rect.bottom = Height;
	}
	else {
	    Rect.bottom = GetSystemMetrics (SM_CYFULLSCREEN) -
                          (GetSystemMetrics (SM_CYMENU) + cm->MLHeight);
	}
    }
    x = (Rect.bottom - (2 * cm->OffsetY)) / (cm->SizeY + cm->LeadingY);
    if (x < 0) return 0;
    return x;
} /* DisplayableRows */

/* DisplayableColumns:  returns the number of columns displayable in the client area */
/* ==================                                                                */

int FAR PASCAL DisplayableColumns (HWND hWnd, int Width, CellMetrics *cm)

/* Width is the hypothetic width of the client area. If this parameter
   is 0, the real client area is measured. If it is negative, the
   maximal client area (maximized child in a maximized frame) is used.
   */
{
    RECT    Rect;
    int     x;

    if (Width== 0) GetClientRect (hWnd, &Rect);
    else {
	if (Width > 0) {
	    Rect.right = Width;
	}
	else {
	    Rect.right = GetSystemMetrics (SM_CXFULLSCREEN);
	}
    }
    x = (Rect.right - (2 * cm->OffsetX)) / cm->SizeX;
    if (x < 0) return 0;
    return x;
} /* DisplayableColumns */

/* UpdateEmacsCaretPos: position the caret according to CaretCol/CaretRow */
/* ===================                                                    */

static void PASCAL UpdateEmacsCaretPos (void)
{
    POINT   pt;

    pt.x = CaretCol;
    pt.y = CaretRow;
    CellToClient (hCaretWnd, pt, &pt);
#if CARETSHAPE == 0
    if (hCaretWnd != hFrameWnd) {
	pt.y += EmacsCM.SizeY - (EmacsCM.SizeY / 4);
    }
#endif
    SetCaretPos (pt.x, pt.y + EmacsCM.HalfLeadingY);
} /* UpdateEmacsCaretPos */

/* EmacsCaret:  Creates or destroys the caret */
/* ==========                                 */

void FAR PASCAL EmacsCaret (BOOL Show)

/* the Show parameter is TRUE if the caret should be created and FALSE
   if it should be destroyed */
{
    if (Show) {
        if (hCaretWnd == 0) return;
	if (hFrameWnd == GetActiveWindow ()) {
	    if (!IsWindow (hCaretWnd)) {
	        /* this may happen in some transient cases when closing
		   down a screen */
	        hCaretWnd = 0;
	        return;
	    }
            CreateCaret (hCaretWnd, NULL,
			 hCaretWnd == hFrameWnd ?
			 	      GetSystemMetrics (SM_CXBORDER) :
#if CARETSHAPE == 1
				      EmacsCM.SizeX / 4,
#else
				      EmacsCM.SizeX,
#endif
#if CARETSHAPE == 0
			 hCaretWnd == hFrameWnd ?
			 	      EmacsCM.SizeY :
                                      EmacsCM.SizeY / 4);
#else
                         EmacsCM.SizeY);
#endif
            UpdateEmacsCaretPos ();
            if (CaretVisible && !IsIconic (hCaretWnd)) ShowCaret (hCaretWnd);
        }
    }
    else {
	/* destroy the caret */
	DestroyCaret ();
    }
} /* EmacsCaret */

/* MoveEmacsCaret:  updates the caret position */
/* ==============                              */

void FAR PASCAL MoveEmacsCaret (HWND hWnd, int col, int row)
{
    CaretCol = col;
    CaretRow = row;
    if (hWnd != hCaretWnd) {
	hCaretWnd = hWnd;
        EmacsCaret (TRUE);
    }
    else {
	hCaretWnd = hWnd;
	UpdateEmacsCaretPos ();
    }
} /* MoveEmacsCaret */

/* ShowEmacsCaret:  shows or hides the caret used by emacs */
/* ==============                                          */

void FAR PASCAL ShowEmacsCaret (BOOL Show)

/* this function is used to make the caret visible only when waiting for
   user input */
{
    if (Show) {
        if (!CaretVisible) {
            if (!IsIconic (hCaretWnd)) ShowCaret (hCaretWnd);
        }
	++CaretVisible;
    }
    else {
	--CaretVisible;
        if (!CaretVisible) {
            HideCaret (NULL);
        }
    }
} /* ShowEmacsCaret */

/* InMessageLine:   non-zero if caret currently in the message line */
/* =============                                                    */

BOOL FAR PASCAL InMessageLine (void)
{
    return (hCaretWnd == hFrameWnd);
} /* InMessageLine */

/* CellToClient:    converts character cell coordinates into client coordinates */
/* ============                                                                 */

void FAR PASCAL CellToClient (HWND hWnd, POINT Cell, LPPOINT Client)

/* The resulting Client coordinates indicate the upper left pixel one
   HalfLeadingY above the character cell */
{
    Client->x = (Cell.x * EmacsCM.SizeX) + EmacsCM.OffsetX;
    if (hWnd == hFrameWnd ) {
        RECT    Rect;

        GetClientRect (hWnd, &Rect);
        Client->y = (Rect.bottom - EmacsCM.MLHeight) + EmacsCM.OffsetY +
                    GetSystemMetrics (SM_CYBORDER);
    }
    else Client->y = (Cell.y * (EmacsCM.SizeY + EmacsCM.LeadingY)) +
                     EmacsCM.OffsetY;
} /* CellToClient */

/* ClientToCell:    converts client coordinates into character cell coordinates */
/* ============                                                                 */

void FAR PASCAL ClientToCell (HWND hWnd, POINT Client, LPPOINT Cell)

/* The area associated with a Cell is the character cell itself, plus
   the HalfLeadingY-high areas above and under the cell */
{
    int     MaxCol, MaxRow;
    
    if (hWnd == hFrameWnd) {    /* message line case */
        MaxCol = MLSIZE - 1;
	MaxRow = 0;
    }
    else {                      /* screen case */
        register SCREEN *sp;

        sp = (SCREEN*)GetWindowLong (hWnd, GWL_SCRPTR);
        MaxCol = sp->s_ncol - 1;
        MaxRow = sp->s_nrow - 1;
    }
    if ((Cell->x = (Client.x - EmacsCM.OffsetX) /
                   EmacsCM.SizeX) < 0) Cell->x = 0;
    else Cell->x = min (Cell->x, MaxCol);
    if ((Cell->y = (Client.y - EmacsCM.OffsetY) /
                   (EmacsCM.SizeY + EmacsCM.LeadingY)) < 0) Cell->y = 0;
    else Cell->y = min (Cell->y, MaxRow);
} /* ClientToCell */

/* GetMinMaxInfo:  processes the WM_GETMINMAXINFO message for a screen */
/* =============                                                       */

void FAR PASCAL GetMinMaxInfo (HWND hWnd, LPPOINT rgpt)
{
    if (InternalRequest) return;	/* none of our business */

    if (notquiescent) {
	/* forbid all size changes */
	RECT    Rect;

	GetWindowRect (hWnd, &Rect);
	rgpt[1].x = Rect.right - Rect.left;
	rgpt[1].y = Rect.bottom - Rect.top;
	rgpt[2] = *(POINT*)&Rect.left;
	rgpt[3] = rgpt[1];
	rgpt[4] = rgpt[1];
    }
    else {                  /* compute minimum tracking size */
        int     X, Y;

        /* minimum displayed text width = 3 */
        /* minimum displayed text  height = 10 */
        MinimumClientSize (hWnd, term.t_margin, 2, &X, &Y);
        rgpt[3].x = X + (2 * GetSystemMetrics (SM_CXFRAME)) +
                    GetSystemMetrics (SM_CXVSCROLL);
        rgpt[3].y = Y + GetSystemMetrics (SM_CYCAPTION) +
                    (2 * GetSystemMetrics (SM_CYFRAME)) +
                    GetSystemMetrics (SM_CYHSCROLL);
    }
} /* GetMinMaxInfo */

/* ScrReSize:    processes the WM_SIZE message */
/* =========                                   */

BOOL FAR PASCAL ScrReSize (HWND hWnd, UINT wParam, WORD cx, WORD cy)

/* returns TRUE only if real resizing performed */
{
    BOOL    ChgWidth, ChgHeight;

    if ((wParam != SIZENORMAL) && (wParam != SIZEFULLSCREEN)) {
        return FALSE;
    }
    ChgWidth = (cx != GetWindowWord (hWnd, GWW_SCRCX));
    ChgHeight = (cy != GetWindowWord (hWnd, GWW_SCRCY));
    if (!ChgWidth && !ChgHeight) return FALSE;

    SetWindowWord (hWnd, GWW_SCRCX, cx);
    SetWindowWord (hWnd, GWW_SCRCY, cy);
    if (!InternalRequest) {
        SCREEN  *TopScreen;

        InternalRequest = TRUE;
        TopScreen = first_screen;
	select_screen ((SCREEN *)GetWindowLong (hWnd, GWL_SCRPTR), FALSE);
	if (ChgWidth) {
            newwidth (TRUE, DisplayableColumns (hWnd, cx, &EmacsCM));
        }
	if (ChgHeight) {
	    newsize (TRUE, DisplayableRows (hWnd, cy, &EmacsCM));
	}
	select_screen (TopScreen, FALSE);
	update (FALSE);
	InternalRequest = FALSE;
    }
    return TRUE;
} /* ScrReSize */

/* ScrPaint:   processes WM_PAINT messages for emacs screens */
/* ========                                                  */

void FAR PASCAL ScrPaint (HWND hWnd)
{
    SCREEN  *sp;
    PAINTSTRUCT ps;
    HANDLE  hPrevFont;
    RECT    Rect;
    int     Row, BottomRow;
    int     Col;
    int     Length;

    /* note that if the WM_PAINT is not passed through the message loop
       (as would hapen with use of UpdateWindow), it is possible to
       attempt repainting while defferupdate is TRUE which means update
       has been deffered */

    BeginPaint (hWnd, &ps);
    sp = (SCREEN*)GetWindowLong (hWnd, GWL_SCRPTR);

    /*-calculate the row/col loop control variables and normalize the
       coordinates of the first line's rectangle into Rect */
    CopyRect (&Rect, &ps.rcPaint);
    ClientToCell (hWnd, *(POINT *)&Rect.left, (POINT *)&Rect.left);
    Col = Rect.left;
    Row = Rect.top;
    --Rect.right;       /* in rectangle conventions, */
    --Rect.bottom;      /* the lower/right border is excluded */
    ClientToCell (hWnd, *(POINT *)&Rect.right, (POINT *)&Rect.right);
    ++Rect.right;
    Length = Rect.right - Col;
    BottomRow = Rect.bottom;
    Rect.bottom = Rect.top + 1;
    CellToClient (hWnd, *(POINT *)&Rect.left, (POINT *)&Rect.left);
    CellToClient (hWnd, *(POINT *)&Rect.right, (POINT *)&Rect.right);
    /* Rect now contains a bounding rectangle for the 1st row. This will
       be used to paint the row's background and will be moved down one
       row's height at each iteration of the painting loop */

    /*-loop from top to bottom, writing one line at a time */
    hPrevFont = SelectFont (ps.hdc, hEmacsFont);
    do {
	VIDEO   *vp;

	vp = sp->s_physical[Row];
	if (!(vp->v_flag & VFNEW)) {    /* valid contents */
	    COLORREF FColor, BColor;
            POINT   RevPt;
            int     i;
	    RECT    BoundingRect;   /* clipping rectangle */
            int     Boundary[4];    /* horizontal paint boundaries:
                                       [0] to [1] is painted in normal video,
                                       [1] to [2] is painted in reverse video,
                                       [2] to [3] is painted in normal video */

	    CopyRect (&BoundingRect, &ps.rcPaint);
	    if (Row > 0) BoundingRect.top = Rect.top;
	    if (Row < BottomRow) BoundingRect.bottom = Rect.bottom;

	    Boundary[0] = BoundingRect.left;
	    Boundary[3] = BoundingRect.right;
	    if (vp->v_right) {  /* there is a reverse video area */
	        if (vp->v_left <= 0) {
	            /* special case: flush to the border */
                    Boundary[1] = 0;
                }
                else {
                    RevPt.y = 0;
	            RevPt.x = vp->v_left;
	            CellToClient (hWnd, RevPt, &RevPt);
	            Boundary[1] = max(BoundingRect.left,RevPt.x);
	        }
	        RevPt.y = 0;
	        RevPt.x = vp->v_right;
	        CellToClient (hWnd, RevPt, &RevPt);
	        Boundary[2] = min(BoundingRect.right,RevPt.x);
	    }
	    else {  /* only normal video */
	        Boundary[1] = Boundary [0];
	        Boundary[2] = Boundary [0];
	    }

	    /*-Paint the row in up to three parts, between the computed
               boundaries (reverse video is applied between [1] and [2] */
	    for (i = 0; i < 3; i++) if (Boundary[i] < Boundary [i+1]) {
	        BoundingRect.left = Boundary[i];
	        BoundingRect.right = Boundary[i+1];
#if     COLOR
	        if (ColorDisplay) {
	            FColor = EmacsPalette[i == 1 ?
                                          vp->v_bcolor : vp->v_fcolor];
	            BColor = EmacsPalette[i == 1 ?
                                          vp->v_fcolor : vp->v_bcolor];
                }
                else {  /* monochrome display */
#else
                {
#endif
		    if (i == 1) {   /* "reverse" video */
		        FColor = GetSysColor (COLOR_HIGHLIGHTTEXT);
		        BColor = GetSysColor (COLOR_HIGHLIGHT);
		    }
		    else {
		        FColor = GetSysColor (COLOR_WINDOWTEXT);
		        BColor = GetSysColor (COLOR_WINDOW);
		    }
	        }
	        SetBkColor (ps.hdc, BColor);
	        SetTextColor (ps.hdc, FColor);
	        ExtTextOut (ps.hdc,
		            Rect.left, Rect.top + EmacsCM.HalfLeadingY,
			    ETO_OPAQUE | ETO_CLIPPED, &BoundingRect,
			    &vp->v_text[Col],
			    Length, NULL);
	    }
	}
	Rect.top = Rect.bottom;
	Rect.bottom += EmacsCM.SizeY + EmacsCM.LeadingY;
    } while (++Row <= BottomRow);

    SelectObject (ps.hdc, hPrevFont);
EndScrPaint:
    EndPaint (hWnd, &ps);
} /* ScrPaint */

/* MLPaint: processes WM_PAINT messages for the Message Line */
/* =======                                                   */

void FAR PASCAL MLPaint (void)
{
    PAINTSTRUCT ps;
    HANDLE  hPrev, hPen;
    RECT    Rect;
    POINT   Client;
    
    BeginPaint (hFrameWnd, &ps);
    
    /*-draw the text portion targetted for repaint */
    hPrev = SelectFont (ps.hdc, hEmacsFont);
    ClientToCell (hFrameWnd, *(POINT*)&ps.rcPaint.left, (POINT*)&Rect.left);
    ClientToCell (hFrameWnd, *(POINT*)&ps.rcPaint.right, (POINT*)&Rect.right);
    CellToClient (hFrameWnd, *(POINT*)&Rect.left, &Client);
    ExtTextOut (ps.hdc,
                Client.x, Client.y + EmacsCM.HalfLeadingY,
                ETO_OPAQUE, &ps.rcPaint,
                &MLBuf[Rect.left],
                Rect.right - Rect.left + 1, NULL);
    SelectObject (ps.hdc, hPrev);
    
    /*-draw the separation line at top of message line */
    hPen = CreatePen (PS_SOLID, GetSystemMetrics (SM_CYBORDER), RGB(0,0,0));
    hPrev = SelectObject (ps.hdc, hPen);
    GetClientRect (hFrameWnd, &Rect);
    Rect.top = Rect.bottom - EmacsCM.MLHeight;
#if WINDOW_MSWIN32
    MoveToEx (ps.hdc, 0, Rect.top, NULL);
#else
    MoveTo (ps.hdc, 0, Rect.top);
#endif
    LineTo (ps.hdc, Rect.right, Rect.top);
    SelectObject (ps.hdc, hPrev);
    DeleteObject (hPen);

    EndPaint (hFrameWnd, &ps);
} /* MLPaint */

/* spal:    set palette from $palette string */
/* ====                                      */

PASCAL spal (char *pstr)
{
#if     COLOR
    int     pal;	/* current palette position */

    for (pal = 0; pal < 16; pal++) {
	DWORD   clr;	/* current color value */
	int     i;
        unsigned char n;

	if (*pstr== 0) break;
	clr = 0;
	for (i = 0; i < 3; i++) if (*pstr) {
	    n = *pstr++ - '0';
	    if (n >= 8) n = 255;
	    else n *= 32;
	    clr |= (DWORD)n << (i * 8);
	}
	EmacsPalette[pal] = clr;
    }
#endif
    return 0;
} /* spal */

/* The routines in this file provide font control functions under
   the Microsoft Windows environment on an IBM-PC or compatible
   computer.

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions.

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

#include    "estruct.h"
#include    <stdio.h>
#include    "eproto.h"
#include    "edef.h"

#include    "mswin.h"

static TEXTMETRIC   Metrics;
static HFONT        hNewFont;
static char         FaceName[LF_FACESIZE];

/* SelectFont:  Selects the emacs-chosen font in the Device Context */
/* ==========                                                       */

HFONT FAR PASCAL SelectFont (HDC hDC, HFONT hFont)

/* just like SelectObject, this function returns the previously selected
   font handle */
{
    HFONT   hPrevFont;

    SetMapMode (hDC, MM_TEXT);
    hPrevFont= SelectObject (hDC, hFont ?
				  hFont :
                                  GetStockObject (SYSTEM_FIXED_FONT));
    return hPrevFont;
} /* SelectFont */

/* GetFontMetrics:  retrieves the TEXTMETRIC and face name of a given font */
/* ==============                                                          */

static void PASCAL GetFontMetrics (HFONT hFont, TEXTMETRIC *Metrics,
                                        char *FaceName)
/* If either Metrics of FaceName is NULL, the corresponding value is not
   returned. If not NULL, FaceName must point to a string containing at
   least LF_FACESIZE characters */
{
    HDC     hDC;
    HFONT   hPrevFont;

    hDC = GetDC (hFrameWnd);
    hPrevFont = SelectFont (hDC, hFont);
    if (Metrics) GetTextMetrics (hDC, Metrics);
    if (FaceName) GetTextFace (hDC, LF_FACESIZE, FaceName);
    SelectObject (hDC, hPrevFont);
    ReleaseDC (hFrameWnd, hDC);
} /* GetFontMetrics */

/* UpdateMaxRowCol: update the maximas displayed on the dialog box */
/* ===============                                                 */

static void PASCAL UpdateMaxRowCol (HWND hDlg, HFONT hFont)
{
    CellMetrics cm;
    char    text[17];

    BuildCellMetrics (&cm, hFont);
    SetDlgItemText (hDlg, ID_MAXROWS,
		    itoa (DisplayableRows (hFrameWnd, -1, &cm), text, 10));
    SetDlgItemText (hDlg, ID_MAXCOLUMNS,
		    itoa (DisplayableColumns (hFrameWnd, -1, &cm), text, 10));
} /* UpdateMaxRowCol */

/* UpdateSample:    Update the sample text displayed on the dialog box */
/* ============                                                        */

static void PASCAL UpdateSample (HWND hDlg, HFONT hFont,
                                      TEXTMETRIC *m, char *FaceName)
{
#define FONTSAMPLESIZE  LF_FACESIZE+40+(26*3)
    char    SampleText[FONTSAMPLESIZE];
    int     i;
    char    c;

    strcpy (SampleText, FaceName);
    i = strlen (SampleText);
    strcpy (&SampleText[i], " (Height=");
    i += strlen (&SampleText[i]);
    itoa (m->tmHeight, &SampleText[i], 10);
    i += strlen (&SampleText[i]);
    strcpy (&SampleText[i], ", Width=");
    i += strlen (&SampleText[i]);
    itoa (m->tmAveCharWidth, &SampleText[i], 10);
    i += strlen (&SampleText[i]);
    strcpy (&SampleText[i], ") sample:");
    i += strlen (&SampleText[i]);
    for (c = 'A'; c <= 'Z'; c++) {
	SampleText[i++] = ' ';
	SampleText[i++] = c;
	SampleText[i++] = lowerc (c);
    }
    SampleText[i] = '\0';
    SendDlgItemMessage (hDlg, ID_SAMPLE, WM_SETFONT, (UINT)hFont, (long)FALSE);
    SetDlgItemText (hDlg, ID_SAMPLE, SampleText);
} /* UpdateSample */

/* NewFont: creates a font matching the user's selections */
/* =======                                                */

static void PASCAL    NewFont (HWND hDlg, BOOL TrustSizeEdit)
/* setting TrustSizeEdit to FALSE indicates that the contents of the
   ID_FONTSIZE edit box should not be used (this is used when this
   function is called for a size list-selection change, at which time
   the edit box has not been updated yet) */
{
    DWORD   d;
    int     i;
    BOOL    FontSizeOK;
    LOGFONT lf;
    HFONT   hOldFont;

    hOldFont = hNewFont;
    i = SendDlgItemMessage (hDlg, ID_FONT, LB_GETCURSEL, 0, 0L);
    if (i == LB_ERR) lf.lfFaceName[0] = 0;
    else {
	SendDlgItemMessage (hDlg, ID_FONT, LB_GETTEXT, i,
	                    (DWORD)(LPSTR)lf.lfFaceName);
    }
    if (TrustSizeEdit) {
        i = GetDlgItemInt (hDlg, ID_FONTSIZE, &FontSizeOK, FALSE);
    }
    else FontSizeOK = FALSE;
    if (FontSizeOK) {
	lf.lfHeight = i;
	lf.lfWidth = 0;
    }
    else {
	i = SendDlgItemMessage (hDlg, ID_FONTSIZE, CB_GETCURSEL, 0, 0L);
	if (i == CB_ERR) i = 0;
	d = SendDlgItemMessage (hDlg, ID_FONTSIZE, CB_GETITEMDATA, i, 0L);
	lf.lfHeight = HIWORD(d);
	lf.lfWidth = LOWORD(d);
    }
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = IsDlgButtonChecked (hDlg, ID_BOLD) ? 700 : 400;
    lf.lfItalic = 0;
    lf.lfUnderline = 0;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = IsDlgButtonChecked (hDlg, ID_ANSI) ?
                   ANSI_CHARSET : OEM_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;

    hNewFont = CreateFontIndirect (&lf);
    GetFontMetrics (hNewFont, &Metrics, FaceName);
    UpdateMaxRowCol (hDlg, hNewFont);
    UpdateSample (hDlg, hNewFont, &Metrics, FaceName);
    if (hOldFont) DeleteObject (hOldFont);
} /* NewFont */

/* AddSize: Add a font size into the font size list (used by EnumSizesProc) */
/* =======                                                                  */

static void PASCAL AddSize (HWND hDlg, short int Height, short int Width)
{
    char    ItemText[17];
    int     i;

    itoa (Height, ItemText, 10);
    i = SendDlgItemMessage (hDlg, ID_FONTSIZE, CB_ADDSTRING, 0,
			    (DWORD)(LPSTR)ItemText);
    SendDlgItemMessage (hDlg, ID_FONTSIZE, CB_SETITEMDATA, i,
			MAKELONG(Width,Height));
    
} /* AddSize */

/* EnumSizesProc:   font enumeration function used by BuildSizeList */
/* =============                                                    */
int EXPORT FAR PASCAL EnumSizesProc (LPLOGFONT lf, LPTEXTMETRIC tm,
                                     short FontType, LPSTR Data)

/* Data should point to a handle to the dialog box */
{
    if ((lf->lfWeight > 400) || lf->lfItalic || lf->lfUnderline ||
        lf->lfStrikeOut) return 1;
#if WIN30SDK
    if (FontType & 0x04) {
#else
    if (FontType & TRUETYPE_FONTTYPE) {
#endif
        /* make a size list up */
        short int h;
        
        for (h = lf->lfHeight / 4; h <= (3 * lf->lfHeight) / 2; h += 2) {
            AddSize (*(HWND FAR *)Data, h, 0);
        }
        return 0;   /* no need to list this one further */
    }
    else {  /* non-scalable font */
        /* list it only if it has a proper aspect ratio */
        HFONT   hFont;
        LOGFONT Font;
        TEXTMETRIC Metrics; 

	Font = *lf;
	Font.lfWidth = 0;
	hFont = CreateFontIndirect (&Font);
	GetFontMetrics (hFont, &Metrics, NULL);
	DeleteObject (hFont);
	if (tm->tmAveCharWidth == Metrics.tmAveCharWidth) {
	    AddSize (*(HWND FAR *)Data,
                     (short int)lf->lfHeight, (short int)lf->lfWidth);
	}
    }
    return 1;
} /* EnumSizesProc */

/* BuildSizeList:   initializes the FontSize list box */
/* =============                                      */

static void PASCAL BuildSizeList (HWND hDlg, TEXTMETRIC *Metrics)

/* This function initializes the FontSize list box with the sizes
   available for the face name currently selected in the Font list box.
   The sizes are written in the format: CXxCY. Also, the sizes are
   stored in the 32 bit values retrievable by LB_GETITEMDATA: width in
   the low-order word and height in the high order word */
{
    SendDlgItemMessage (hDlg, ID_FONTSIZE, WM_SETREDRAW, FALSE, 0L);
    SendDlgItemMessage (hDlg, ID_FONTSIZE, CB_RESETCONTENT, 0, 0L);
    {
	HDC     hDC;
	FARPROC ProcInstance;
	char    FaceName[LF_FACESIZE];

	SendDlgItemMessage (hDlg, ID_FONT, LB_GETTEXT,
			    (UINT)SendDlgItemMessage (hDlg, ID_FONT,
						      LB_GETCURSEL, 0, 0L),
			    (DWORD)(LPSTR)&FaceName[0]);
	    /* FaceName now contains the currently selected face name */
	hDC = GetDC (hDlg);
	ProcInstance = MakeProcInstance ((FARPROC)EnumSizesProc,
					 hEmacsInstance);
	EnumFonts (hDC, FaceName, ProcInstance, LPDATA(&hDlg));
	FreeProcInstance (ProcInstance);
	ReleaseDC (hDlg, hDC);
	if (SendDlgItemMessage (hDlg, ID_FONTSIZE, CB_GETCOUNT, 0, 0L) == 0) {
	    /* no size at all in the size list (ATM, for instance, does that!).
               Let's fill it with a few sample sizes... */
            short int h;

            for (h = 6; h <= 40; h += 2) AddSize (hDlg, h, 0);
	}
    }
    SendDlgItemMessage (hDlg, ID_FONTSIZE, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect (GetDlgItem (hDlg, ID_FONTSIZE), NULL, TRUE);
    {   /*-Select the larger height that is smaller or equal to the
	   current Metrics (assumed to be the ones of the previous font)
	   */
	int     i;
	int     BestIndex = 0;
	short int h, w, BestHeight = 0, BestWidth = 0;
	DWORD   ItemData;

	for (i = 0;; i++) {
	    ItemData = SendDlgItemMessage (hDlg, ID_FONTSIZE,
					   CB_GETITEMDATA, i, 0L);
	    if (ItemData == CB_ERR) break;  /* end of list hit */
	    if ((h = HIWORD(ItemData)) > Metrics->tmHeight) continue;
	    if (BestHeight > h) continue;
	    w = LOWORD(ItemData);
	    if (BestHeight == h) {  /* use the width to optimize */
		if (w > Metrics->tmAveCharWidth) continue;
		if (BestWidth > w) continue;
	    }
	    BestHeight = h;
	    BestWidth = w;
	    BestIndex = i;
	}
	SendDlgItemMessage (hDlg, ID_FONTSIZE, CB_SETCURSEL, BestIndex, 0L);
    }
    NewFont (hDlg, TRUE);
} /* BuildSizeList */

/* AddFace: Adds a face to the FONT list box if it begets a proper font */
/* =======                                                              */

static void PASCAL AddFace (HWND hDlg, char *CandidateFace)
{
    BYTE    CharSet;
    int     From, At;   /* indexes for list box searches */
    HFONT   hFixedFont;
    char    FaceName [LF_FACESIZE];
    TEXTMETRIC tm;
    
    CharSet = (IsDlgButtonChecked (hDlg, ID_ANSI) ?
               ANSI_CHARSET : OEM_CHARSET);
    /*-try to identify a fixed-pitch font of set CharSet, with the
       specified face name */
    hFixedFont = CreateFont (0, 0, 0, 0, 0, FALSE, FALSE, FALSE,
			     CharSet, OUT_DEFAULT_PRECIS,
			     CLIP_DEFAULT_PRECIS, DRAFT_QUALITY,
			     FF_DONTCARE+FIXED_PITCH,
			     CandidateFace);
    GetFontMetrics (hFixedFont, &tm, FaceName);
    DeleteObject (hFixedFont);
    if ((tm.tmPitchAndFamily & 0x01) || /* =1 if variable pitch */
	(tm.tmCharSet != CharSet) ||
	(strcmp (FaceName, CandidateFace) != 0)) {
	/* flunked the exam! */
	return;     /* skip that face */
    }
    /* the candidate face has at least one fixed-pitch font with the
       right charset */
    /*- scan the list box to make sure this face is not a duplicate */
    At = -1;    /* start at beginning of list box */
    do {
	From = At;
	At = SendDlgItemMessage (hDlg, ID_FONT, LB_FINDSTRING, From,
                                 (DWORD)CandidateFace);
        if (At == LB_ERR) break;    /* no match, implies not duplicate */
	if (SendDlgItemMessage (hDlg, ID_FONT, LB_GETTEXTLEN, At, 0L) ==
            strlen(CandidateFace)) {
            /* the lengths match, that means the strings match so it is
	       indeed a duplicate */
	    return;
	}
    } while (At > From);    /* exit if search has gone through the
			       bottom of the list box */

    /*-it is a brand new face, let's add it to the list */
    SendDlgItemMessage (hDlg, ID_FONT, LB_ADDSTRING, 0, (DWORD)CandidateFace);
    return;
} /* AddFace */

/* EnumFacesProc:   face enumeration function used by BuildFaceList */
/* =============                                                    */
int EXPORT FAR PASCAL EnumFacesProc (LPLOGFONT lf, LPTEXTMETRIC tm,
                                     short FontType, LPSTR Data)

/* Data should point to a handle to the dialog box */
/* lists only fixed pitch fonts that match the selected charset */
{
    AddFace (*(HWND FAR *)Data, lf->lfFaceName);
    return 1;
} /* EnumFacesProc */

/* BuildFaceList:   initialize the FONT list box */
/* =============                                 */

static void PASCAL BuildFaceList (HWND hDlg, char *FaceName)

/* This function initializes the Font list box with fixed fonts matching
   the current charset selection and then selects an item */
{
    SendDlgItemMessage (hDlg, ID_FONTSIZE, WM_SETREDRAW, FALSE, 0L);
    SendDlgItemMessage (hDlg, ID_FONT, LB_RESETCONTENT, 0, 0L);
    {
	HDC     hDC;
	FARPROC ProcInstance;

	hDC = GetDC (hDlg);
	ProcInstance = MakeProcInstance ((FARPROC)EnumFacesProc,
					 hEmacsInstance);
	EnumFonts (hDC, NULL, ProcInstance, LPDATA(&hDlg));
	FreeProcInstance (ProcInstance);
	ReleaseDC (hDlg, hDC);
    }
    SendDlgItemMessage (hDlg, ID_FONT, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect (GetDlgItem (hDlg, ID_FONT), NULL, TRUE);
    /*-select the same facename as before or default to the first item */
    if (SendDlgItemMessage (hDlg, ID_FONT, LB_SELECTSTRING, -1,
                            (DWORD)FaceName) == LB_ERR) {
	SendDlgItemMessage (hDlg, ID_FONT, LB_SETCURSEL, 0, 0L);
    }
    BuildSizeList (hDlg, &Metrics);
} /* BuildFaceList */

/* FontDlgProc: Emacs Font dialog box function */
/* ===========                                 */
int EXPORT FAR PASCAL  FontDlgProc (HWND hDlg, UINT wMsg, UINT wParam,
				    LONG lParam)
{
    switch (wMsg) {
        
    case WM_INITDIALOG:
        hNewFont = NULL;
	{   /*-setup the dialog box's caption */
	    char    s[40];

	    strcpy (s, ProgName);
	    strcat (s, " - Font selection");
	    SetWindowText (hDlg, s);
	}
	/*-set the Bold button */
	GetFontMetrics (hEmacsFont, &Metrics, FaceName);
	CheckDlgButton (hDlg, ID_BOLD, (Metrics.tmWeight > 550));
	{   /*-simulate a mouse click on the appropriate charset
	       radiobutton. This will initialize all the other controls
	       */
	    WORD    id;
	    
	    id = (Metrics.tmCharSet == ANSI_CHARSET ? ID_ANSI : ID_OEM);
	    SendMessage (hDlg, WM_COMMAND,
#if WINDOW_MSWIN32
                         MAKELONG(id, BN_CLICKED), (long)GetDlgItem (hDlg, id));
#else
                         id, MAKELONG(GetDlgItem (hDlg, id), BN_CLICKED));
#endif
	}
        return TRUE;

    case WM_COMMAND:
	switch (LOWORD(wParam)) {
	case 1:     /* OK button */
	    if (NOTIFICATION_CODE == BN_CLICKED) {
AcceptFont:
		if (hEmacsFont) DeleteObject (hEmacsFont);
		hEmacsFont = hNewFont;
		EndDialog (hDlg, TRUE);
		/* no need to unhook the SAMPLE's font, it is not
		   destroyed since it will be used by emacs */
	    }
	    else return FALSE;
	    break;
	case 2:     /* Cancel button */
	    if (NOTIFICATION_CODE == BN_CLICKED) goto CancelFont;
	    else return FALSE;
	case ID_SAVEFONT:
	    if (NOTIFICATION_CODE == BN_CLICKED) {
		/*-save the facename, bold status and size into WIN.INI,
		   then perform as the OK button */
		char    NumText[17];
		    
		GetFontMetrics (hNewFont, &Metrics, FaceName);
		WriteProfileString (ProgName, "FontName", FaceName);
		WriteProfileString (ProgName, "CharSet",
		                    itoa (Metrics.tmCharSet, NumText, 10));
		WriteProfileString (ProgName, "FontHeight",
		                    itoa (Metrics.tmHeight, NumText, 10));
		WriteProfileString (ProgName, "FontWidth",
		                    itoa (Metrics.tmAveCharWidth,
                                          NumText, 10));
		WriteProfileString (ProgName, "FontWeight",
		                    itoa (Metrics.tmWeight, NumText, 10));
            }
	    else return FALSE;
	    goto AcceptFont;
	case ID_ANSI:
	case ID_OEM:
	    if (NOTIFICATION_CODE == BN_CLICKED) {
	        CheckRadioButton (hDlg, ID_1ST_CHARSET, ID_LAST_CHARSET,
                                  LOWORD(wParam));
		BuildFaceList (hDlg, FaceName);
	    }
	    else return FALSE;
	    break;
	case ID_BOLD:
	    if (NOTIFICATION_CODE == BN_CLICKED) NewFont (hDlg, TRUE);
	    else return FALSE;
	    break;
	case ID_FONT:
	    switch (NOTIFICATION_CODE) {
	    case LBN_SELCHANGE:
		BuildSizeList (hDlg, &Metrics);
		break;
	    case LBN_DBLCLK:    /* treated as OK */
		goto AcceptFont;
	    default:
		return FALSE;
	    }
	    break;
	case ID_FONTSIZE:
	    switch (NOTIFICATION_CODE) {
	    case CBN_SELCHANGE:
		NewFont (hDlg, FALSE);
		break;
	    case CBN_EDITCHANGE:
	        {
		    BOOL    FontSizeOK;

		    GetDlgItemInt (hDlg, ID_FONTSIZE, &FontSizeOK, FALSE);
		    if (FontSizeOK) NewFont (hDlg, TRUE);
		    else MessageBeep (0);
		}
		break;
	    case CBN_DBLCLK:    /* treated as OK */
		goto AcceptFont;
	    default:
		return FALSE;
	    }
	    break;
	default:
	    return FALSE;
        }
	break;
    case WM_SYSCOMMAND:
	if ((wParam & 0xFFF0) == SC_CLOSE) {
CancelFont:
	    EndDialog (hDlg, FALSE);
	    if (hNewFont) {
		SendDlgItemMessage (hDlg, ID_SAMPLE, WM_SETFONT, 0, FALSE);
		    /* reset to SYSTEM_FONT before deleting the font */
		DeleteObject (hNewFont);
	    }
	    return TRUE;
	}
	return FALSE;
    default:
	return FALSE;
    }
    return FALSE;
} /* FontDlgProc */

/* ChangeFont:  effects the font change on the screen & message line */
/* ==========                                                         */
static void PASCAL ChangeFont (void)

{
    SCREEN  *sp, *fsp;
    RECT    Rect;

    /*-loop through all the screens, resizing the vision that emacs has
       of them, processing the current ("first") screen last */
    InternalRequest = TRUE;
    fsp = first_screen;
    do {
	sp = first_screen;
	while (sp->s_next_screen != (SCREEN *)NULL) sp = sp->s_next_screen;
        select_screen (sp, FALSE);
        GetClientRect (sp->s_drvhandle, &Rect);
        newwidth (TRUE, DisplayableColumns (sp->s_drvhandle,
                                            Rect.right, &EmacsCM));
        newsize (TRUE, DisplayableRows (sp->s_drvhandle,
                                        Rect.bottom, &EmacsCM));
    } while (sp != fsp);
    InternalRequest = FALSE;

    /*-update the frame's client area and the MDIClient's size */
    InvalidateRect (hFrameWnd, NULL, TRUE);
    GetClientRect (hFrameWnd, &Rect);
    MoveWindow (hMDIClientWnd, 0, 0,
                Rect.right, Rect.bottom - EmacsCM.MLHeight,
                TRUE);
} /* ChangeFont */

/* PickEmacsFont:   calls-up the FONTS dialog box */
/* =============                                  */

BOOL FAR PASCAL PickEmacsFont (void)

/* returns TRUE is a new font has been picked */
{
    BOOL    FontChanged;
    FARPROC ProcInstance;

    ProcInstance = MakeProcInstance ((FARPROC)FontDlgProc,
				     hEmacsInstance);
    FontChanged = DialogBox (hEmacsInstance, "FONTS",
			     hFrameWnd, ProcInstance);
    FreeProcInstance (ProcInstance);

    if (FontChanged) {
	BuildCellMetrics (&EmacsCM, hEmacsFont);
        ChangeFont ();
	return TRUE;
    }
    else return FALSE;
} /* PickEmacsFont */

/* FontInit:    initialize a font description from WIN.INI */
/* ========                                                */

void FAR PASCAL FontInit (void)
{
    LOGFONT lf;
    char    text[20];

    if (GetProfileString (ProgName, "CharSet", "", text, 20)) {
        if (isdigit(text[0])) { /* pure numeric value */
            lf.lfCharSet = atoi (text);
        }
        else {  /* menmonic value */
	    if (strncmp (text, "OEM", 3) == 0) {
		lf.lfCharSet = OEM_CHARSET;
	    }
	    else lf.lfCharSet = ANSI_CHARSET;
	}
	lf.lfHeight = GetProfileInt (ProgName, "FontHeight", 0);
	lf.lfWidth = GetProfileInt (ProgName, "FontWidth", 0);
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = GetProfileInt (ProgName, "FontWeight", 400);
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
        GetProfileString (ProgName, "FontName", "",
                          lf.lfFaceName, LF_FACESIZE);

        hEmacsFont = CreateFontIndirect (&lf);
    }
    else {  /* no CharSet entry, default to SYSTEM_FIXED_FONT */
	hEmacsFont = 0;
    }
    BuildCellMetrics (&EmacsCM, hEmacsFont);
} /* FontInit */

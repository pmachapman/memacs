/* The routines in this file provide menu-related functions under the
   Microsoft Windows environment on an IBM-PC or compatible computer.

   This module also contains the code for the About and Modes dialog
   boxes.

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions.

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

#include    "estruct.h"
#include    "elang.h"
#include    <stdio.h>
#include    "eproto.h"
#include    "edef.h"

#include    "mswin.h"
#include    "mswmenu.h"
#include    "mswhelp.h"

#define MAXMENUTITLE 50

/* codes for the MenuType parameter from AddMenuEntry () */
#define MT_DUMMY    1
#define MT_MENUBAR  2

typedef struct   {
    WORD    m_word;
    ETYPE EPOINTER m_ptr;
} MENUTAB;

#define MB_FNC  0x4000
#define MB_BUF  0x8000  /* static IDs should not look like they carry
			   this flag */

#define MAXDYNMENU  256

static MENUTAB MenuDynBind [MAXDYNMENU] = {0, NULL};

static MENUTAB MenuStaticBind [] = {
    {IDM_NULLPROC,      nullproc},
    {IDM_FILEFIND,      filefind},
    {IDM_VIEWFILE,      viewfile},
    {IDM_INSFILE,	insfile},
    {IDM_FILEREAD,	fileread},
    {IDM_FILENAME,	filename},
    {IDM_FILESAVE,	filesave},
    {IDM_FILEWRITE,	filewrite},
    {IDM_FILEAPP,	fileapp},
    {IDM_SETEKEY,	setekey},
    {IDM_NEXTBUFFER,	nextbuffer},
    {IDM_USEBUFFER,	usebuffer},
    {IDM_UNMARK,	unmark},
    {IDM_NAMEBUFFER,	namebuffer},
    {IDM_KILLBUFFER,	killbuffer},
    {IDM_NARROW,	narrow},
    {IDM_WIDEN,		widen},
    {IDM_LISTBUFFERS,	listbuffers},
    {IDM_SPLITWIND,	splitwind},
    {IDM_DELWIND,	delwind},
    {IDM_ONLYWIND,	onlywind},
    {IDM_NEXTWIND,	nextwind},
    {IDM_PREVWIND,	prevwind},
    {IDM_MVUPWIND,	mvupwind},
    {IDM_MVDNWIND,	mvdnwind},
    {IDM_NEXTUP,	nextup},
    {IDM_NEXTDOWN,	nextdown},
    {IDM_ENLARGEWIND,	enlargewind},
    {IDM_SHRINKWIND,	shrinkwind},
    {IDM_RESIZE,	resize},
    {IDM_QUICKEXIT,	quickexit},
    {IDM_QUIT,		quit},
        /* here just to compute the key binding: the actual IDM_QUIT
	   selection is processed as a special */
    {IDM_CUTREGION,	cutregion},
    {IDM_CLIPREGION,	clipregion},
    {IDM_INSERTCLIP,	insertclip},
    {IDM_SETMARK,	setmark},
    {IDM_REMMARK,	remmark},
    {IDM_SWAPMARK,	swapmark},
    {IDM_YANK,		yank},
    {IDM_KILLREGION,	killregion},
    {IDM_COPYREGION,	copyregion},
    {IDM_UPPERREGION,	upperregion},
    {IDM_LOWERREGION,	lowerregion},
    {IDM_ENTAB,		entab},
    {IDM_DETAB,		detab},
    {IDM_TRIM,		trim},
    {IDM_INDENT_REGION,	indent_region},
    {IDM_UNDENT_REGION,	undent_region},
    {IDM_WORDCOUNT,	wordcount},
    {IDM_FILLPARA,	fillpara},
    {IDM_KILLPARA,	killpara},
    {IDM_KILLTEXT,	killtext},
    {IDM_OPENLINE,	openline},
    {IDM_DELFWORD,	delfword},
    {IDM_DELBWORD,	delbword},
    {IDM_CAPWORD,	capword},
    {IDM_LOWERWORD,	lowerword},
    {IDM_UPPERWORD,	upperword},
    {IDM_DEBLANK,	deblank},
    {IDM_TWIDDLE,	twiddle},
    {IDM_TAB,		tab},
    {IDM_QUOTE,		quote},
    {IDM_SETFILLCOL,	setfillcol},
    {IDM_FORWSEARCH,	forwsearch},
    {IDM_BACKSEARCH,	backsearch},
    {IDM_FORWHUNT,	forwhunt},
    {IDM_BACKHUNT,	backhunt},
    {IDM_FISEARCH,	fisearch},
    {IDM_RISEARCH,	risearch},
    {IDM_SREPLACE,	sreplace},
    {IDM_QREPLACE,	qreplace},
    {IDM_GOTOMARK,	gotomark},
    {IDM_GOTOLINE,	gotoline},
    {IDM_GETFENCE,	getfence},
    {IDM_GOTOBOB,	gotobob},
    {IDM_GOTOEOB,	gotoeob},
    {IDM_FORWPAGE,	forwpage},
    {IDM_BACKPAGE,	backpage},
    {IDM_GOTOEOP,	gotoeop},
    {IDM_GOTOBOP,	gotobop},
    {IDM_FORWLINE,	forwline},
    {IDM_BACKLINE,	backline},
    {IDM_GOTOBOL,	gotobol},
    {IDM_GOTOEOL,	gotoeol},
    {IDM_FORWWORD,	forwword},
    {IDM_BACKWORD,	backword},
    {IDM_ENDWORD,	endword},
    {IDM_EXECPRG,	execprg},
    {IDM_SPAWNCLI,	spawncli},
    {IDM_SPAWN,		spawn},
    {IDM_PIPECMD,	pipecmd},
    {IDM_FILTER,	filter},
    {IDM_CTLXE,		ctlxe},
    {IDM_CTLXLP,	ctlxlp},
    {IDM_CTLXRP,	ctlxrp},
    {IDM_NAMEDCMD,	namedcmd},
    {IDM_EXECCMD,	execcmd},
    {IDM_EXECPROC,	execproc},
    {IDM_EXECBUF,	execbuf},
    {IDM_EXECFILE,	execfile},
    {IDM_BINDTOKEY,	bindtokey},
    {IDM_MACROTOKEY,	macrotokey},
    {IDM_BINDTOMENU,	bindtomenu},
    {IDM_MACROTOMENU,	macrotomenu},
    {IDM_UNBINDKEY,	unbindkey},
    {IDM_UNBINDMENU,    unbindmenu},
    {IDM_DESKEY,	deskey},
    {IDM_SETVAR,	setvar},
    {IDM_DISPVAR,	dispvar},
    {IDM_DESVARS,	desvars},
    {IDM_SHOWCPOS,	showcpos},
    {IDM_CTRLG,		ctrlg},
    {IDM_FIND_SCREEN,	find_screen},
    {IDM_RENAME_SCREEN,	rename_screen},
    {IDM_NEWSIZE,	newsize},
    {IDM_NEWWIDTH,	newwidth},
    {IDM_DESBIND,	desbind},
    {IDM_DESFUNC,	desfunc},
    {IDM_APRO,		apro},
    {0,                 NULL}
};

#define MAXMENUPARENT   7

typedef struct {    /* current menu description */
    int     cm_pos;     /* entry position */
    int     cm_x;       /* index of the last parent handle, -1 if entry
			   located in main menu bar */
    HMENU   cm_parent[MAXMENUPARENT];
                        /* parent handle, main popup menu at index 0 */
} CURMENU;

static CURMENU  CurrentMenu = {-1, -1, {NULL}};

static unsigned int ctrlx_key = 0;
static unsigned int meta_key = 0;       /* for GetKeyText */

/* execmenu:    execute a function bound to a menu */
/* ========                                        */

/* This function is called by the edit loop in main.c, when a MENU
   extended character is detected. */

PASCAL execmenu (int f, int n)
/* f, n: arguments to target function */
{
    register UINT    ID;
    register MENUTAB *MTp;

    ID = (xpos << 8) + ypos;    /* getkey sees MENU sequences as MOUS */
    if (ID >= IDM_DYNAMIC) {
        MTp = &MenuDynBind[ID - IDM_DYNAMIC];
        if (MTp->m_word & MB_FNC) return (*(MTp->m_ptr.fp)) (f, n);
        else if (MTp->m_word & MB_BUF) {
            BOOL    status;

            if (f == FALSE) n = 1;
            while (n--) {
                status = dobuf (MTp->m_ptr.buf);
                if (status != TRUE) return status;
            }
        }
        else return FAILED;
        return TRUE;
    }
    else for (MTp = &MenuStaticBind[0]; MTp->m_word != 0; MTp++) {
        if (ID == MTp->m_word) return (*(MTp->m_ptr.fp)) (f, n);
    }
    return FAILED;      /* not found !!! */
} /* execmenu */

/* GenerateMenuSeq: send a menu sequence into the input stream */
/* ===============                                             */

void FAR PASCAL GenerateMenuSeq (UINT ID)
{
    if (!in_room (5)) return;
    in_put (0);             /* escape indicator */
    in_put (MENU >> 8);     /* MENU prefix */
    in_put (ID >> 8);       /* menu ID high byte (--> xpos) */
    in_put (ID & 0xFF);     /* menu ID low byte (--> ypos) */
    in_put ('m');           /* dummy event */
} /* GenerateMenuSeq */

/* AboutDlgProc:  About box dialog function */
/* ============                             */
int EXPORT FAR PASCAL  AboutDlgProc (HWND hDlg, UINT wMsg, UINT wParam,
                                     LONG lParam)
{
    char    s [50];
    static RECT FullBox;
    
    switch (wMsg) {
        
    case WM_INITDIALOG:
        strcpy (s, PROGNAME);
        strcat (s, " ");
        strcat (s, VERSION);
        SetDlgItemText (hDlg, ID_PROGVER, s);
        {
            RECT    Bar;

	    GetWindowRect (hDlg, &FullBox);
            GetWindowRect (GetDlgItem (hDlg, ID_ABOUTBAR), &Bar);
            MoveWindow (hDlg, FullBox.left, FullBox.top,
                        Bar.left + GetSystemMetrics (SM_CXDLGFRAME)
                                 - FullBox.left,
                        FullBox.bottom - FullBox.top,
                        FALSE);     /* shrink dialog box, do not repaint */
        }
	return TRUE;
	
    case WM_COMMAND:
	if (NOTIFICATION_CODE != BN_CLICKED) break;
	if (LOWORD(wParam) == 1) {
	    EndDialog (hDlg, 0);
	    return TRUE;
	}
	else if (LOWORD(wParam) == ID_MOREABOUT) {
	    EnableWindow ((HWND)lParam, FALSE);   /* disable "More >>" */
	    SetFocus (GetDlgItem (hDlg, 1));         /* make "OK" default */
	    MoveWindow (hDlg, FullBox.left, FullBox.top,
	                FullBox.right - FullBox.left,
                        FullBox.bottom - FullBox.top,
                        TRUE);      /* enlarge dialog box and repaint */
	    return TRUE;
	}
	break;
    default:
	return FALSE;
    }
    return FALSE;
} /* AboutDlgProc */

/* SetCheck:    puts a check mark in a check box */
/* ========                                      */

void  PASCAL    SetCheck (HWND hDlg, int BoxID)
{
    SendMessage (GetDlgItem (hDlg, BoxID), BM_SETCHECK, 1, 0L);
} /* SetCheck */

/* GetCheck:    TRUE is the check box is checked */
/* ========                                      */

BOOL  PASCAL    GetCheck (HWND hDlg, int BoxID)
{
    return (SendMessage (GetDlgItem (hDlg, BoxID), BM_GETCHECK, 0, 0L) != 0);
} /* GetCheck */

/* ModeDlgProc: Modes dialog box function */
/* ===========                            */

/* must be invoked through DialogBoxParam, with LOWORD(dwInitParam) set
   to TRUE for global modes and FALSE for current buffer modes */

int EXPORT FAR PASCAL  ModeDlgProc (HWND hDlg, UINT wMsg, UINT wParam,
                                    LONG lParam)
{
    char    s[40+NBUFN];
    static int *modep;      /* mode flags pointer */
    
    switch (wMsg) {
        
    case WM_INITDIALOG:
        if (LOWORD(lParam)) {
	    strcpy (s, TEXT331);    /* "Global modes" */
	    modep = &gmode;
	}
        else {
	    strcpy (s, TEXT332);    /* "Modes for buffer: " */
	    strcat (s, curbp->b_bname);
	    modep = &(curbp->b_mode);
	}
        SetWindowText (hDlg, s);
        if (*modep & MDWRAP) SetCheck (hDlg, ID_WRAP);
	if (*modep & MDCMOD) SetCheck (hDlg, ID_CMODE);
        if (*modep & MDEXACT) SetCheck (hDlg, ID_EXACT);
        if (*modep & MDVIEW) SetCheck (hDlg, ID_VIEW);
        if (*modep & MDOVER) SetCheck (hDlg, ID_OVER);
        if (*modep & MDREPL) SetCheck (hDlg, ID_REP);
        if (*modep & MDMAGIC) SetCheck (hDlg, ID_MAGIC);
        if (*modep & MDCRYPT) SetCheck (hDlg, ID_CRYPT);
        if (*modep & MDASAVE) SetCheck (hDlg, ID_ASAVE);
	return TRUE;
	
    case WM_COMMAND:
	if (NOTIFICATION_CODE != BN_CLICKED) break;
	switch (LOWORD(wParam)) {
	    
	case 1:		/* OK */
	    *modep = 0;
	    if (GetCheck (hDlg, ID_WRAP)) *modep |= MDWRAP;
	    if (GetCheck (hDlg, ID_CMODE)) *modep |= MDCMOD;
	    if (GetCheck (hDlg, ID_EXACT)) *modep |= MDEXACT;
	    if (GetCheck (hDlg, ID_VIEW)) *modep |= MDVIEW;
	    if (GetCheck (hDlg, ID_OVER)) *modep |= MDOVER;
	    if (GetCheck (hDlg, ID_REP)) *modep |= MDREPL;
	    if (GetCheck (hDlg, ID_MAGIC)) *modep |= MDMAGIC;
	    if (GetCheck (hDlg, ID_CRYPT)) *modep |= MDCRYPT;
	    if (GetCheck (hDlg, ID_ASAVE)) *modep |= MDASAVE;
	    EndDialog (hDlg, 0);
	    break;
	    
	case 2:		/* Cancel */
	    EndDialog (hDlg, -1);
	    break;
	
	case ID_OVER:
	    /* if OVER set, clear the REP mode check-box */
	    if (GetCheck (hDlg, ID_OVER)) {
		SendMessage (GetDlgItem (hDlg, ID_REP), BM_SETCHECK, 0, 0L);
	    }
	    break;

	case ID_REP:
	    /* if REP set, clear the OVER mode check-box */
	    if (GetCheck (hDlg, ID_REP)) {
		SendMessage (GetDlgItem (hDlg, ID_OVER), BM_SETCHECK, 0, 0L);
	    }
	    break;

	}
	break;

    default:
	return FALSE;
    }
    return FALSE;
} /* ModeDlgProc */

/* IsMenuSeparator:  TRUE if the item is a separator */
/* ===============                                   */
static BOOL PASCAL IsMenuSeparator (HMENU hMenu, int Position)
{
    DWORD state;
    
    if (GetSubMenu (hMenu, Position) != 0) return FALSE;
    if ((state = GetMenuState (hMenu, Position, MF_BYPOSITION)) != -1) {
        if (state & MF_SEPARATOR) return TRUE;
    }
    return FALSE;
} /* IsMenuSeparator */

/* GetMenuEntryID:   give back ID of MenuItem or popup */
/* ==============                                      */

/* if the entry is a menu item (not a popup), its ID is returned. If the
   entry is a popup, the ID of the first entry in this popup is
   returned, added to n*IDM_POPUP where n is the number of popup levels.

   If the entry is neither an item nor a popup, an item id of 0 is
   returned */

UINT  PASCAL    GetMenuEntryID (HMENU hMenu, int Position)
{
    UINT    id;
    HMENU   hSubMenu;

    if (IsMenuSeparator (hMenu, Position)) {
        id = 0;
    }
    else if ((hSubMenu = GetSubMenu (hMenu, Position)) != 0) {
	id = IDM_POPUP + GetMenuEntryID (hSubMenu, 0);
    }
    else id = GetMenuItemID (hMenu, Position);
    return id;
} /* GetMenuEntryID */

/* FindKeyBinding:  scan the key binding table, return first match */
/* ==============                                                  */

/* If there is no match, the returned value point to the BINDNUL entry
   */

KEYTAB * FAR PASCAL FindKeyBinding (void *Func)
{
    register KEYTAB *KTp;

    for (KTp = &keytab[0]; KTp->k_type != BINDNUL; ++KTp) {
	if (((KTp->k_type == BINDFNC) && (KTp->k_ptr.fp == Func)) ||
	    ((KTp->k_type == BINDBUF) && (KTp->k_ptr.buf == Func)))
	    if (!(KTp->k_code & MOUS)) break;   /* found it! (we skip
						   mouse bindings) */
    }
    return KTp;
    
} /* FindKeyBinding */

/* GetKeyText:  translates a key code into a CUA-type description */
/* ==========                                                     */

/* Returns the length (excluding the terminating NULL) of the text if it
   fits in the supplied buffer, 0 otherwise (in which case the buffer
   contents are unuseable). */

/* This function uses the ctrlx_key and meta_key static variables to
   avoid scanning the key binding table too often. If these are NULL,
   however, it looks them up. Therefore, these variables can be zeroed
   if there is a doubt about their validity */

int  PASCAL GetKeyText (int Key, char *Text, int TextLength)
{
    int     i;      /* index in Text */
    char    c;

    if (Key & (CTLX | META)) {
        unsigned *prefix_key_ptr;
        int     n;

        prefix_key_ptr = (Key & CTLX) ? &ctrlx_key : &meta_key;
	if (*prefix_key_ptr == 0) {
	    KEYTAB  *KTp;

	    KTp = FindKeyBinding ((Key & CTLX) ? cex : meta);
	    if (KTp->k_type == BINDNUL) return 0;
	    *prefix_key_ptr = KTp->k_code;
	}
#define NBSPACES    3   /* # of spaces after prefix */
	i = GetKeyText (*prefix_key_ptr, Text, TextLength - NBSPACES);
	if (i == 0)  return 0;
	for (n = 0; n < NBSPACES; ++n) Text[i++] = ' ';
    }
    else {
	i = 0;
    }

#define APPENDTEXT(s)   {if (TextLength - i < sizeof(s)) return 0;\
                         strcpy (&Text[i], s); i += sizeof(s) - 1;}

    c = (char)Key;
    
    if (Key & ALTD) APPENDTEXT(TEXT310) /* "Alt+" */
    
    if (Key & SHFT) APPENDTEXT(TEXT311) /* "Shift+" */
    
    if (Key & CTRL) {
        switch (c) {
	case 'H':
	    APPENDTEXT(TEXT312) /* "BkSp" */
	    goto all_done;
	case 'I':
	    APPENDTEXT(TEXT313) /* "Tab" */
	    goto all_done;
	case 'M':
	    APPENDTEXT(TEXT314) /* "Enter" */
	    goto all_done;
	case '[':
	    APPENDTEXT(TEXT315)  /* "Esc" */
	    goto all_done;
	default:
	    APPENDTEXT(TEXT316) /* "Ctrl+" */
	    break;
	}
    }
    
    if (Key & SPEC) {
        switch (c) {
	case '<':
	    APPENDTEXT(TEXT317) /* "Home" */
	    break;
	case 'N':
	    APPENDTEXT(TEXT318) /* "DownArrow" */
	    break;
	case 'P':
	    APPENDTEXT(TEXT319) /* "UpArrow" */
	    break;
	case 'B':
	    APPENDTEXT(TEXT320) /* "LeftArrow" */
	    break;
	case 'F':
	    APPENDTEXT(TEXT321) /* "RightArrow" */
	    break;
	case '>':
	    APPENDTEXT(TEXT322) /* "End" */
	    break;
	case 'Z':
	    APPENDTEXT(TEXT323) /* "PageUp" */
	    break;
	case 'V':
	    APPENDTEXT(TEXT324) /* "PageDown" */
	    break;
	case 'C':
	    APPENDTEXT(TEXT325) /* "Ins" */
	    break;
	case 'D':	    
	    APPENDTEXT(TEXT326) /* "Del" */
	    break;
	case '0':
	    APPENDTEXT(TEXT327) /* "F10" */
	    break;
	default:    /* single digit function key */
	    if ((c < '1') || (c > '9')) return 0; /* unknown! */
	    if (TextLength - i < 3) return 0;
	    Text[i++] = CHAR328;    /* 'F' */
	    Text[i++] = c;
	    break;
        }
    }
    else if (c == ' ') APPENDTEXT(TEXT329)  /* "SpaceBar" */
    else {
        /* normal char */
        if (TextLength - i < 2) return 0;
        Text[i++] = c;
    }
all_done:    
    Text[i] = '\0';
    return i;
} /* GetKeyText */

/* UpdateMenuItemText:  sets the key binding info in a menu item */
/* ==================                                            */

void  PASCAL    UpdateMenuItemText (HMENU hMenu, int Position,
                                    MENUTAB *MTp)
{
    KEYTAB  *KTp;
    char    NewText[MAXMENUTITLE];
    char    OldText[MAXMENUTITLE];
    register int i;

    GetMenuString (hMenu, Position, OldText, MAXMENUTITLE, MF_BYPOSITION);
    strcpy (NewText, OldText);
    for (i = 0; (NewText[i] != '\0') && (NewText[i] != '\t'); ++i) ;
        /* find the first tab char or the string's end */

    KTp = FindKeyBinding (MTp->m_word & MB_BUF ? (void*)MTp->m_ptr.buf :
						 (void*)MTp->m_ptr.fp);
    if (KTp->k_type != BINDNUL) {
	NewText[i] = '\t';
	if (!GetKeyText (KTp->k_code, &NewText[i+1], MAXMENUTITLE + 1 - i))
	    goto no_binding;    /* if out of room, no binding text */
    }
    else {
no_binding:
        /* let's erase the binding text */
	NewText[i] = '\0';
    }
    if (strcmp (NewText, OldText) == 0) return;
    ModifyMenu (hMenu, Position, MF_BYPOSITION | MF_STRING,
                GetMenuItemID (hMenu, Position), NewText); 
} /* UpdateMenuItemText */

/* InitMenuPopup:   perform popup menu init before display (WM_INITMENUPOPUP) */
/* =============                                                              */

/* here, we may gray menu entries that are invalid. We also try to
   display a key binding after each menu item */

void FAR PASCAL InitMenuPopup (HMENU hMenu, LONG lParam)
{
    int     Position;
    int     ItemCount;
    BOOL    Enable;
    UINT    EntryID;
    register MENUTAB *MTp; /* points the appropriate entry in MenuStaticBind */
    MENUTAB *PrevMTp;      /* to control MenuStaticBind scanning */

    if (HIWORD(lParam)) return;     /* do not process the system menu */

    ctrlx_key = meta_key = 0;       /* these may have changed since the
				       last call to GetKeyText */
    
    /*-Scan the menu's items */
    ItemCount = GetMenuItemCount (hMenu);
    PrevMTp = &MenuStaticBind[0];
    for (Position =  0; Position < ItemCount; Position++) {
        EntryID = GetMenuEntryID (hMenu, Position);

        if (EntryID == 0) continue; /* skip separators */

        /*-lookup the item in the menu binding table */
        /* since we assume the items' order follows the binding table,
	   we try to optimize the lookup by starting from the last
	   found entry */
        if (EntryID >= IDM_POPUP) {
	    /* for popup menus MTp yields the nullproc entry */
            MTp = &MenuStaticBind[0];
        }
        else if (EntryID >= IDM_DYNAMIC) {
	    /* for dynamic items, MTp is directly set to the proper
	       entry in the MenuDynBind */
	    MTp = &MenuDynBind[EntryID - IDM_DYNAMIC];
	}
        else {
            MTp = PrevMTp + 1;
	    for (;;) {
	        if (MTp->m_word == EntryID) {
	            /* found it! */
		    PrevMTp = MTp;  /* prepare for next item */
		    break;
	        }
	        if (MTp == PrevMTp) {
	            /* scan completed, item not found! */
	            MTp = &MenuStaticBind[0]; /* use default */
	            break;
	        }
	        if (MTp->m_word == 0) {
                    MTp = &MenuStaticBind[0];   /* cycle through */
                }
                else ++MTp;
	    }
	}

 	/*-for each item found in the binding table, display a key binding */
	if (MTp != &MenuStaticBind[0]) UpdateMenuItemText (hMenu, Position, MTp);

	/*-if not quiescent, gray most items */
        if (notquiescent) {
            switch (EntryID) {
#ifdef  IDM_DEBUG
	    case IDM_DEBUG:
#endif
	    case IDM_QUIT:
	    case IDM_CTRLG:
	    case IDM_ABOUT:
	    case IDM_WHELPINDEX:
	    case IDM_WHELPKEYBOARD:
	    case IDM_WHELPCOMMANDS:
	    case IDM_WHELPPROCEDURES:
		Enable = TRUE;
		break;
	    default:
		Enable = FALSE;
		break;
	    }
	}
	else {
	    /*-if quiescent, gray the invalid items, depending on
	       appropriate conditions */
	    Enable = TRUE;
            switch (EntryID) {
	    case IDM_FILESAVE:
	    case IDM_UNMARK:
		/* grayed if current buffer is unmarked */
		if (!(curbp->b_flag & BFCHG)) Enable = FALSE;
		break;
            case IDM_NEXTBUFFER:
	    case IDM_USEBUFFER:
	    case IDM_KILLBUFFER:
		/* grayed if sole visible buffer */
		if (getdefb () == NULL) Enable = FALSE;
		break;
	    case IDM_NARROW:
	        /* grayed if already narrowed */
		if (curbp->b_flag & BFNAROW) Enable = FALSE;
	        /* and if no mark... */
            case IDM_CUTREGION:
            case IDM_CLIPREGION:
            case IDM_EDIT_REGION_POPUP:
		/* grayed if no mark0 in current window */
		if (curwp->w_markp[0] == NULL) Enable = FALSE;
		break;
            case IDM_REMMARK:
            case IDM_SWAPMARK:
            case IDM_GOTOMARK:
                /* grayed if no mark at all in current window */
                {
                    int     i;
                    Enable = FALSE;
                    for (i = 0; i <= 9; i++) if (curwp->w_markp[i]) {
                        Enable = TRUE;
                        break;
                    }
                }
                break;
	    case IDM_WIDEN:
		/* grayed if not narrowed */
		if (!(curbp->b_flag & BFNAROW)) Enable = FALSE;
		break;
	    case IDM_DELWIND:
	    case IDM_ONLYWIND:
	    case IDM_NEXTWIND:
	    case IDM_PREVWIND:
	    case IDM_NEXTUP:
	    case IDM_NEXTDOWN:
	    case IDM_FILE_WINDOW_SIZE_POPUP:
		/* grayed if only one window in current screen */
		if (wheadp->w_wndp == NULL) Enable = FALSE;
		break;
	    case IDM_EDIT_CLIPBOARD_POPUP:
		/* grayed if clipboard empty and no mark set */
		if ((curwp->w_markp[0] == NULL) &&
		    !IsClipboardFormatAvailable (CF_TEXT)) Enable = FALSE;
		break;
	    case IDM_INSERTCLIP:
		/* grayed if clipboard empty */
		if (!IsClipboardFormatAvailable (CF_TEXT)) Enable = FALSE;
		break;
	    case IDM_YANK:
		/* grayed if no kill buffer */
		if (kbufh[kill_index] == NULL) Enable = FALSE;
		break;
	    case IDM_FORWHUNT:
	    case IDM_BACKHUNT:
		/* grayed if no search string */
		if (pat[0] == '\0') Enable = FALSE;
		break;
	    case IDM_CTLXE:
		/* grayed if no keyboard macro */
		break;
	    case IDM_TILE_VERTICALLY:
	        if (!Win31API) Enable = FALSE;
	        break;
	    }

	    /*-If view mode, gray the items flagged IDM_NOTVIEW */
	    if (curbp->b_mode & MDVIEW) {
	        if (!(EntryID & (IDM_DYNAMIC | IDM_POPUP)) &&
                    (MTp->m_word & IDM_NOTVIEW)) Enable = FALSE;
	    }
        }

        EnableMenuItem (hMenu, Position,
                        MF_BYPOSITION | (Enable ? MF_ENABLED : MF_GRAYED));
    }
} /* InitMenuPopup */

/* SimulateExtendedKey:    feed an extended key into the input stream */
/* ===================                                                */

static void PASCAL  SimulateExtendedKey (int ec)
{
    char    prefix;

    if (in_room(5)) {
	prefix = ec >> 8;
	if (prefix != 0) {
	    in_put (0);
	    in_put (prefix);
	    if (prefix & (MOUS >> 8)) {
		/* in case the key is a mouse action, supply
		   dummy mouse position info */
		in_put (1);
		in_put (1);
	    }
	}
	in_put (ec & 0xFF);
    }
} /* SimulateExtendedKey */

/* MenuCommand: WM_COMMAND message handling */
/* ===========                              */

/* returns TRUE if the command has been recognized and FALSE otherwise
   */

BOOL FAR PASCAL MenuCommand (UINT wParam, LONG lParam)
{
    FARPROC     ProcInstance;
    DWORD       HelpContext;

    switch (LOWORD(wParam)) {
        /* the menu choices from here down to the default statement are
           valid even in the not-quiescent case */

    case IDM_ABOUT:
	ProcInstance = MakeProcInstance ((FARPROC)AboutDlgProc,
					 hEmacsInstance);
	DialogBox (hEmacsInstance, "ABOUT", hFrameWnd, ProcInstance);
	FreeProcInstance (ProcInstance);
	break;
	
#ifdef  IDM_DEBUG
    case IDM_DEBUG:     /* places a call to the debugger */
	DebugBreak ();
	break;
#endif
	
    case IDM_QUIT:
	PostMessage (hFrameWnd, WM_CLOSE, 0, 0L);
	break;
	
    case IDM_CTRLG:
	/* this is a special case: we feed the abort key into the input
	   stream */
	SimulateExtendedKey (abortc);
	break;
	
    case IDM_WHELPINDEX:
	HelpContext = 0;
	goto InvokeHelp;
    case IDM_WHELPKEYBOARD:
	HelpContext = HELPID_KEYBOARD;
	goto InvokeHelp;
    case IDM_WHELPCOMMANDS:
	HelpContext = HELPID_COMMANDS;
	goto InvokeHelp;
    case IDM_WHELPPROCEDURES:
	HelpContext = HELPID_PROCEDURES;
InvokeHelp:
	WinHelp (hFrameWnd, MainHelpFile,
                 HelpContext ? HELP_CONTEXT : HELP_INDEX, HelpContext);
	MainHelpUsed = TRUE;
	break;
	
    default:
	if (notquiescent) return TRUE;  /* abort processing */

	/* the following IDs are processed only if emacs is quiescent */
	switch (LOWORD(wParam)) {

	case IDM_GLOBMODE:
	case IDM_MODE:
	    ProcInstance = MakeProcInstance ((FARPROC)ModeDlgProc,
					     hEmacsInstance);
	    DialogBoxParam (hEmacsInstance, "MODES",
			    hFrameWnd, ProcInstance,
			    (DWORD)(wParam == IDM_GLOBMODE));
	    FreeProcInstance (ProcInstance);
	    if (wParam = IDM_MODE) {
		upmode ();
		update (FALSE);
	    }
	    break;

	case IDM_FONT:
	    PickEmacsFont ();
	    break;

	case IDM_CASCADE:
	    SendMessage (hMDIClientWnd, WM_MDICASCADE, 0, 0L);
	    break;
	case IDM_TILE_HORIZONTALLY:
	    SendMessage (hMDIClientWnd, WM_MDITILE, MDITILE_VERTICAL, 0L);
	    break;
	case IDM_TILE_VERTICALLY:
	    SendMessage (hMDIClientWnd, WM_MDITILE, MDITILE_HORIZONTAL, 0L);
	    break;
	case IDM_ARRANGEICONS:
	    SendMessage (hMDIClientWnd, WM_MDIICONARRANGE, 0, 0L);
	    break;

	case IDM_NORMALIZE:
	    {
		SCREEN  *sp;

		sp = (SCREEN*)GetWindowLong ((HWND)(SendMessage (hMDIClientWnd,
					            WM_MDIGETACTIVE, 0, 0L)),
		                             GWL_SCRPTR);
		newsize (TRUE, sp->s_nrow);
		newwidth (TRUE, sp->s_ncol);
		update (FALSE);
	    }
	    break;
	    
	default:
	    if (wParam >= IDM_FIRSTCHILD) return FALSE;
	    GenerateMenuSeq (wParam);
	    return TRUE;
	}
    }
    
    /* we have processed an internal menu command */
    if (!notquiescent) GenerateMenuSeq (IDM_NULLPROC);
        /* this flushes a possible numeric argument */
    return TRUE;
} /* MenuCommand */

/* GetScreenMenuHandle: returns the handle to the 'Screen' menu (for MDI mgt) */
/* ===================                                                        */

HMENU FAR PASCAL GetScreenMenuHandle (void)
{
    HMENU   hBaseMenu;
    int     Pos;

    hBaseMenu = GetMenu (hFrameWnd);
    for (Pos = GetMenuItemCount (hBaseMenu) - 1; Pos >= 0; Pos--) {
        if (GetMenuEntryID (hBaseMenu, Pos) == IDM_SCREEN_POPUP) {
            return GetSubMenu (hBaseMenu, Pos);
        }
    }
    return NULL;
} /* GetScreenMenuHandle */

/* MenuEntryCount:  count of menu entries, excluding the MDI buttons */
/* ==============                                                    */

static int PASCAL   MenuEntryCount (HMENU hMenu)
{
    int     Count;

    Count = GetMenuItemCount (hMenu);
    if (hMenu == GetMenu (hFrameWnd)) {
#if WINDOW_MSWIN32
        if (GetWindowLong((HWND)SendMessage (hMDIClientWnd, WM_MDIGETACTIVE,
                                             0, 0L), GWL_STYLE) &
            WS_MAXIMIZE) {
#else
        if (HIWORD(SendMessage (hMDIClientWnd, WM_MDIGETACTIVE, 0, 0L))) {
#endif
            /* an MDI child is maximized ==> we have MDI buttons in the
	       menu bar */
	    Count -= 2;
	}
    }
    return Count;
} /* MenuEntryCount */

/* MenuEntryOffset:  position of the first non-MDI button menu entry */
/* ===============                                                   */

static int PASCAL   MenuEntryOffset (HMENU hMenu)
{
    if (hMenu == GetMenu (hFrameWnd)) {
#if WINDOW_MSWIN32
        if (GetWindowLong((HWND)SendMessage (hMDIClientWnd, WM_MDIGETACTIVE,
                                             0, 0L), GWL_STYLE) &
            WS_MAXIMIZE) {
#else
        if (HIWORD(SendMessage (hMDIClientWnd, WM_MDIGETACTIVE, 0, 0L))) {
#endif
            /* an MDI child is maximized ==> we have MDI buttons in the
	       menu bar */
	    return 1;
	}
    }
    return 0;
    
} /* MenuEntryOffset */

/* ParseMenu:   parse a piece of menu path */
/* =========                               */

static BOOL PASCAL  ParseMenu (char *Name, char *Title, int *Posp)

/* Puts the text part of the menu name in Title (at most MAXMENUTITLE
   characters including the \0) and the position number in *Posp. If no
   position is specified, *Posp is set to -1. The returned BOOL is TRUE
   if no error occured and FAILED if a syntax problem was diagnosed */
{
    register int i;

    *Posp = -1;
    for (i = 0; Name[i] != '\0'; i++) {
        if (i >= MAXMENUTITLE - 1) {
            mlwrite (TEXT300);  /* "[Incorrect menu]" */
            return FAILED;
        }
        if (Name[i] == '>') break;
        if (Name[i] == '@') {   /* must parse a position index */
            unsigned char *s;

            *Posp = 0;
            for (s = &Name[i+1]; (*s != '>') && (*s != '\0'); s++) {
                if ((*s >= '0') && (*s <= '9')) {
                    *Posp = (*Posp * 10) + (*s - '0');
                }
                else {      /* non numerical character! */
                    mlwrite (TEXT300);  /* "[Incorrect menu]" */
                    return FAILED;
                }
            }
            break;
        }
        Title[i] = Name[i];
    }
    Title[i] = '\0';
    return TRUE;
} /* ParseMenu */

/* LocateMenu:  locate a menu entry matching a piece of menu path */
/* ==========                                                     */

static BOOL PASCAL  LocateMenu (char *Name, CURMENU *CM)

/* The returned BOOL is TRUE if a matching menu entry was found, FALSE
   if no such entry was found and FAILED if a syntax problem was
   diagnosed. The CURMENU structure is updated if the entry was found.
   */
{
    HMENU   hMenu;
    int     ItemCount;
    int     Pos, StartPos, PosOffset;
    BOOL    Result, DoScan;
    char    Target[MAXMENUTITLE];
    char    EntryName[MAXMENUTITLE];

    while (*Name == '<') {
	++Name;
	--CM->cm_x;
	CM->cm_pos = -1;
    }
    if ((Result = ParseMenu (Name, Target, &Pos)) != TRUE) {
	return Result;
    }
    if (CM->cm_x < 0) {
	hMenu = GetMenu (hFrameWnd);
    }
    else hMenu = CM->cm_parent[CM->cm_x];
    PosOffset = MenuEntryOffset (hMenu);
    ItemCount = GetMenuItemCount (hMenu);
    if (DoScan = (Pos < 0)) Pos = CM->cm_pos;
    if (Pos < 0) Pos = 0;
    Pos += PosOffset;
    StartPos = Pos;
    do {
	char    *s;

	if (IsMenuSeparator (hMenu, Pos)) {
	    EntryName[0] = '\0';
	}
	else {
            GetMenuString (hMenu, Pos, (LPSTR)&EntryName[0],
		           MAXMENUTITLE, MF_BYPOSITION);
	    if ((s = strchr (EntryName, '\t')) != NULL) *s = '\0';
	        /* discard the accelerator/key-binding info */
	}
	if (strcmp (EntryName, Target) == 0) goto MenuLocated;
        if (!DoScan) {
            if (*Target == '\0') goto MenuLocated;
            else break; /* no matching target at indicated position */
        }
	if (++Pos >= ItemCount) Pos = 0;
    } while (Pos != StartPos);
    return FALSE;   /* not found */
MenuLocated:
    CM->cm_pos = Pos - PosOffset;
    return TRUE;
} /* LocateMenu */

/* AddMenuEntry:    add a menu entry (or, recursively, a cascade of entries) */
/* ============                                                              */

static BOOL PASCAL  AddMenuEntry (char *Name, UINT ID, CURMENU *CM,
                                      WORD *MenuType)
/* the MenuType is a set of flags set returned by this function:
   MT_DUMMY indicates that the menu item is a mere separator, MT_MENUBAR
   indicates that the menu bar has been modified */
{
    HMENU   hMenu, hPopup;
    int     Pos;
    BOOL    Result;
    char    EntryName[MAXMENUTITLE];
    WORD    MenuFlags;

    if ((Result = ParseMenu (Name, EntryName, &Pos)) != TRUE) {
        return Result;
    }
    if (CM->cm_x < 0) {
	hMenu = GetMenu (hFrameWnd);
	*MenuType |= MT_MENUBAR;
    }
    else hMenu = CM->cm_parent[CM->cm_x];
    Name = strchr(Name, '>');
    if (Name != NULL) {     /* our task is to set up a popup menu */
	if (CM->cm_x >= MAXMENUPARENT - 1) {
	    mlwrite (TEXT301);  /* "[Too many nested popup menus]" */
	    return FALSE;
	}
	if ((hPopup = CreatePopupMenu ()) == NULL) {
	    mlwrite (TEXT302);  /* "[lack of resources]" */
	    return ABORT;
	}
	MenuFlags = MF_POPUP | MF_STRING;
    }
    else {  /* this is a menu entry, not a popup */
	if (strcmp (EntryName, "-") == 0) {    /* only a separator */
	    MenuFlags = MF_SEPARATOR;
	    *MenuType |= MT_DUMMY;
	    ID = 0;     /* contrary to SDK documentation, InsertMenu
			   does not ignore the ID for a separator in 3.0! */
	}
	else {
	    MenuFlags = MF_STRING;
	}
    }
    if (Pos < 0) {
        Pos = CM->cm_pos + 1;       /* to insert after current entry */
	if (Pos <= 0) Pos = MenuEntryCount (hMenu); /* to append at end */
    }
    Result = InsertMenu (hMenu, Pos + MenuEntryOffset (hMenu),
                         MenuFlags | MF_BYPOSITION,
                         Name ? (UINT)hPopup : ID,
                         (LPSTR)&EntryName[0]);
    if (!Result) {
        mlwrite (TEXT302);  /* "[lack of resources]" */
        Result = FALSE;
    }
    else Result = TRUE;
    if (Name == NULL) {
        CM->cm_pos = Pos;
    }
    else {
        if (Result != TRUE) {
            DestroyMenu (hPopup);
        }
        else {
	    CM->cm_parent[++CM->cm_x] = hPopup;
	    CM->cm_pos = -1;
	    ++Name;     /* skip the '>' */
	    if ((Result = AddMenuEntry (Name, ID, CM, MenuType)) != TRUE) {
		/* Menu creation failed at some point, we must undo our own */
		DeleteMenu (hMenu, Pos + MenuEntryOffset (hMenu),
                            MF_BYPOSITION);
	    }
	}
    }
    return Result;
} /* AddMenuEntry */

/* AddMenuBinding:  bind a menu item to an EPOINTER */
/* ==============                                   */

static BOOL PASCAL  AddMenuBinding (ETYPE EPOINTER eptr, WORD type)

/* called by bindtomenu and macrotomenu, once the function or macro name
   has been parsed */
{
    BOOL    Result;
    WORD    MenuType = 0;   /* info returned by AddMenuEntry */
    char    FullName[NSTRING];
    char    *Name;
    UINT    DynIndex;
    CURMENU CM;

    /*-Let's make sure we have a free entry in the binding table */
    for (DynIndex = 0; MenuDynBind[DynIndex].m_word != 0; DynIndex++) {
        if (DynIndex >= MAXDYNMENU) {
            mlwrite (TEXT17);
            /* "Binding table FULL!" */
            return FALSE;
        }
    }

    /*-Parse the menu Name */
    Name = &FullName[0];
    if ((Result = mlreply (TEXT306, Name, NSTRING)) != TRUE) {
            /* "Menu: " */
        return Result;
    }
    if (*Name == '>') {
        CM.cm_pos = -1;
        CM.cm_x = -1;
        ++Name;
    }
    else CM = CurrentMenu;
    while ((Result = LocateMenu (Name, &CM)) == TRUE) {
        /* ...looping until we hit a piece of the menu path that does
	   not yet exist */
	HMENU   hMenu;
	
	if (CM.cm_x < 0) hMenu = GetMenu (hFrameWnd);
	else hMenu = CM.cm_parent[CM.cm_x];
	hMenu = GetSubMenu (hMenu, CM.cm_pos + MenuEntryOffset (hMenu));
	if (hMenu == NULL) {
	    /* this is not a popup menu, we loose! */
	    mlwrite (TEXT300);  /* "[Incorrect menu]" */
	    return FALSE;
	}
	if (CM.cm_x >= MAXMENUPARENT - 1) {
	    mlwrite (TEXT301);  /* "[Too many nested popup menus]" */
	    return FALSE;
	}
	CM.cm_parent[++CM.cm_x] = hMenu;
	CM.cm_pos = -1;
	Name = strchr (Name, '>');
	if (Name == NULL) {
	    /* premature end of menu path! */
	    mlwrite (TEXT300);  /* "[Incorrect menu]" */
	    return FALSE;
	}
	++Name;     /* skip the '>' */
    }
    if (Result != FALSE) return Result;  /* syntax error, no cigar! */

    /*-Create the menu entry (or cascade of entries) */
    if ((Result = AddMenuEntry (Name, DynIndex + IDM_DYNAMIC, &CM,
                                &MenuType)) != TRUE) {
        return Result;
    }
    CurrentMenu = CM;

    if (!(MenuType & MT_DUMMY)) {
	/*-complete the binding */
	MenuDynBind[DynIndex].m_word = type;
	MenuDynBind[DynIndex].m_ptr = eptr;
    }
    
    if (MenuType & MT_MENUBAR) DrawMenuBar (hFrameWnd);
    return TRUE;
} /* AddMenuBinding */

/* bindtomenu:  bind a menu item to an emacs function */
/* ==========                                         */

PASCAL bindtomenu (int f, int n)
/* command arguments IGNORED */
{
    ETYPE EPOINTER  e;

    e.fp = getname (TEXT304);   /* "Function: " */
    if (e.fp == NULL) {
        mlwrite (TEXT16);       /* "[No such function]" */
        return FALSE;
    }
    return AddMenuBinding (e, MB_FNC);
} /* bindtomenu */

/* macrotomenu: bind a menu item to a macro (i.e. a buffer) */
/* ===========                                              */

PASCAL macrotomenu (int f, int n)
/* command arguments IGNORED */
{
    ETYPE EPOINTER  e;
    char    Name[NBUFN];
    BOOL    Result;

    if ((Result = mlreply(TEXT305, &Name[1], NBUFN-2)) != TRUE) {
            /* "Macro: " */
        return Result;
    }
    Name[0] = '[';
    strcat (Name, "]");
    if ((e.buf = bfind (Name, FALSE, 0)) == NULL) {
        mlwrite (TEXT130);
        /* "Macro not defined" */
        return FALSE;
    }
    return AddMenuBinding (e, MB_BUF);
} /* macrotomenu */

/* DeleteMenuBinding:   remove a menu entry and its binding or sub-entries */
/* =================                                                       */

static BOOL PASCAL  DeleteMenuBinding (HMENU hMenu, int Pos)
/* returns TRUE except when an attempt is made to delete the 'Screen'
   menu */
{
    BOOL    Result;
    UINT    ID;
    HMENU   hPopup;

    Pos += MenuEntryOffset (hMenu);
    if (!IsMenuSeparator (hMenu, Pos)) {
        if ((ID = GetMenuItemID (hMenu, Pos)) != -1) {
            /* this is a pure menu item */
            if (ID >= IDM_DYNAMIC) {
                /* let's free the dynamic binding table entry */
                MenuDynBind[ID - IDM_DYNAMIC].m_word = 0;
            }
        }
        else if ((hPopup = GetSubMenu (hMenu, Pos)) != NULL) {
            /* this is a popup, we must delete all the contained entries */
            int     i;

            if (ID == IDM_SCREEN_POPUP) {
                /* this menu is used by the system's MDI manager to list the
	           MDI childs. We cannot remove it */
                return FALSE;
            }
            for (i = MenuEntryCount (hPopup); i > 0;) {
                DeleteMenuBinding (hPopup, --i);
            }
        }
    }
    DeleteMenu (hMenu, Pos, MF_BYPOSITION);
    if (hMenu == GetMenu (hFrameWnd)) DrawMenuBar (hFrameWnd);
    return TRUE;
} /* DeleteMenuBinding */

/* unbindmenu:  remove a menu entry */
/* ==========                       */

PASCAL unbindmenu (int f, int n)
/* command arguments IGNORED */
{
    BOOL    Result;
    char    FullName[NSTRING];
    char    *Name;
    CURMENU CM;

    Name = &FullName[0];
    if ((Result = mlreply (TEXT303, Name, NSTRING)) != TRUE) {
            /* "Menu: " */
        return Result;
    }
    if (*Name == '>') {
        CM.cm_pos = -1;
        CM.cm_x = -1;
        ++Name;
    }
    else CM = CurrentMenu;
    while ((Result = LocateMenu (Name, &CM)) == TRUE) {
	HMENU   hMenu;
	
	if (CM.cm_x < 0) hMenu = GetMenu (hFrameWnd);
	else hMenu = CM.cm_parent[CM.cm_x];
	Name = strchr (Name, '>');
	if (Name != NULL) {     /* the menu path continues beyond this... */
	    hMenu = GetSubMenu (hMenu, CM.cm_pos + MenuEntryOffset (hMenu));
	    if (hMenu == NULL) {
	        /* this is not a popup menu, we loose! */
	        Result = FALSE;
	        break;
	    }
	    if (CM.cm_x >= MAXMENUPARENT - 1) {
		mlwrite (TEXT301);  /* "[Too many nested popup menus]" */
		return FALSE;
	    }
	    CM.cm_parent[++CM.cm_x] = hMenu;
	    CM.cm_pos = 0;
	    ++Name;     /* skip the '>' */
	    /* and loop once again... */
	}
	else {                  /* this is it: the end of the menu path */
	    int     Pos;

	    if ((Result = DeleteMenuBinding (hMenu, CM.cm_pos)) != TRUE) {
	        return FALSE;
	    }
	    if ((Pos = MenuEntryCount (hMenu) - 1) < CM.cm_pos) {
		/* we just deleted the item at the menu's end. This
		   position is no longer valid */
		CM.cm_pos = Pos;
	    }
	    CurrentMenu = CM;
	    break;      /* we are done */
	}
    }
    /* we arrive here if the menu was not found or a syntax error occured */
    if (Result == FALSE) mlwrite (TEXT300); /* "[Incorrect menu]" */
    return Result;
} /* unbindmenu */

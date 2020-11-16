/*      AMIGADOS.C:     Operating specific I/O and Spawning functions
   for MicroEMACS 3.12
   (C)Copyright 1993 by Daniel M. Lawrence
   Significant changes for GCC (c) 1996 Ruth Ivimey-Cook
 */

#include        <stdio.h>
#if defined(__SASC) || defined(__GNUC__)
#include	<string.h>
#else
#define __aligned
#endif

#include	"estruct.h"
#if	AMIGA
#include	<exec/types.h>
#include	<exec/io.h>
#include	<exec/memory.h>
#include	<exec/libraries.h>
#include	<devices/inputevent.h>
#include	<graphics/text.h>
#include	<graphics/gfxbase.h>
#include	<graphics/view.h>
#include	<graphics/displayinfo.h>
#include	<intuition/intuition.h>
#include	<intuition/intuitionbase.h>
#include	<intuition/screens.h>
#include	<utility/tagitem.h>
#include	<libraries/asl.h>
#include	<devices/console.h>
#include	<dos/dos.h>

#if	GCC
#include	<sys/stat.h>
#include	<dirent.h>
#endif

#if		SASC | GCC
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/console_protos.h>
#include <clib/asl_protos.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/console_pragmas.h>
#include <pragmas/asl_pragmas.h>

void in_init (void);
int in_check (void);
void in_put (int event);
int in_get (void);
void UEGetScreenInfo(WORD *scrWidth, WORD *scrHeight, UWORD *scrMode);
struct Screen * UEOpenScreen(WORD *scrWidth, WORD *scrHeight);
struct Window * UEOpenWindow(struct Screen *screen);

int mod (int);
void sendcon (char *buf);
void doevent (void);
void dokey (struct InputEvent *);
void stuffibuf (int, int, int);
int spawncli (int, int);
int spawn (int, int);
int execprg (int, int);
int pipecmd (int, int);

#ifdef WINDOW_AMIGA
int FileReq (char *, char *, unsigned);

#endif
#endif /* SASC | GCC */

#ifdef AZTEC_C
#define memset(a,b,c) setmem(a,c,b)
#endif

#if	GCC
static DIR *dirptr = NULL;	/* Current directory stream     */
static char *nameptr;		/* Ptr past end of path in rbuf */

#endif

static char path[NFILEN];	/* Path of file to find         */
static char rbuf[NFILEN];	/* Return file buffer           */

int amiga_winflag = FALSE;
int amiga_scrflag = FALSE;
char *amiga_pubscrnm = NULL;
WORD screenWidth;
WORD screenHeight;

#include	"eproto.h"
#include        "edef.h"
#include	"elang.h"

/* set these to the version of the libs being compiled with (or earlier). */
#define INTUITION_REV	37L
#define ASL_REV		37L
#define GFX_REV		37L

#define	NEW 		1006L

#ifdef FORCE_TOPAZ80		/* FORCE_TOPAZ80 is obsolete, now we're font sensitive */
#define	CRWIDTH		8	/* constant values for TOPAZ80 */
#define	CRHEIGHT	8
#else
#define	CRWIDTH		theFont->tf_XSize	/* now font sensitive, */
#define	CRHEIGHT	theFont->tf_YSize	/* use variable values */
#endif

struct IntuitionBase *IntuitionBase = 0L;
struct GfxBase *GfxBase = 0L;
struct Library *ConsoleDevice = 0L;
struct Library *AslBase = 0L;

struct Screen *scr;
struct Window *win;
struct IOStdReq con;		/* ptr to console device driver handle */
static struct TextFont *theFont;
struct Menu   *EmacsMenus = NULL;

#ifdef FORCE_TOPAZ80		/* FORCE_TOPAZ80 is obsolete, now we're font sensitive */
static struct TextAttr *theTextAttr;

#endif

/*      Intuition Function type declarations    */

#if	!defined(__SASC) && !defined(__GNUC__)
struct Library *OpenLibrary ();
struct Window *OpenWindow ();
struct Message *GetMsg ();

#endif

#ifdef AMIGA
extern NOSHARE TERM term;

#endif

typedef struct
{
    short rw_code;		/* normal keycode to generate */
    short rw_scode;		/* shifted  "  */
    short rw_ccode;		/* control  "  */
}
RKEY;

/* raw keycode scan code to emacs keycode translation table */

RKEY keytrans[0x60] =
{

/*      CODE    NORM    SHIFT   CTRL */
/*      0x00, */ '`', '~', 0,
/*      0x01, */ '1', '!', 0,
/*      0x02, */ '2', '@', 0,
/*      0x03, */ '3', '#', 0,
/*      0x04, */ '4', '$', 0,
/*      0x05, */ '5', '%', 0,
/*      0x06, */ '6', '^', 0,
/*      0x07, */ '7', '&', 0,
/*      0x08, */ '8', '*', 0,
/*      0x09, */ '9', '(', 0,
/*      0x0a, */ '0', ')', 0,
/*      0x0b, */ '-', '_', 0,
/*      0x0c, */ '=', '+', 0,
/*      0x0d, */ '\\', '|', 0,
/*      0x0e, */ 0, 0, 0,
/*      0x0f, */ 0, 0, 0,
/*      0x10, */ 'q', 'Q', CTRL | 'Q',
/*      0x11, */ 'w', 'W', CTRL | 'W',
/*      0x12, */ 'e', 'E', CTRL | 'E',
/*      0x13, */ 'r', 'R', CTRL | 'R',
/*      0x14, */ 't', 'T', CTRL | 'T',
/*      0x15, */ 'y', 'Y', CTRL | 'Y',
/*      0x16, */ 'u', 'U', CTRL | 'U',
/*      0x17, */ 'i', 'I', CTRL | 'I',
/*      0x18, */ 'o', 'O', CTRL | 'O',
/*      0x19, */ 'p', 'P', CTRL | 'P',
/*      0x1a, */ '[', '{', 0,
/*      0x1b, */ ']', '}', 0,
/*      0x1c, */ 0, 0, 0,
/*      0x1d, */ '1', SPEC | '>', 0,
/*      0x1e, */ '2', SPEC | 'N', 0,
/*      0x1f, */ '3', SPEC | 'V', 0,
/*      0x20, */ 'a', 'A', CTRL | 'A',
/*      0x21, */ 's', 'S', CTRL | 'S',
/*      0x22, */ 'd', 'D', CTRL | 'D',
/*      0x23, */ 'f', 'F', CTRL | 'F',
  /*      0x24, */ 'g', 'G', CTRL | 'G', /*     0x25, */ 'h', 'H', CTRL | 'H',
/*      0x26, */ 'j', 'J', CTRL | 'J',
/*      0x27, */ 'k', 'K', CTRL | 'K',
/*      0x28, */ 'l', 'L', CTRL | 'L',
/*      0x29, */ ';', ':', 0,
/*      0x2a, */ 39, 34, 0,
/*      0x2b, */ 0, 0, 0,
/*      0x2c, */ 0, 0, 0,
/*      0x2d, */ '4', SPEC | 'B', 0,
/*      0x2e, */ '5', 0, 0,
/*      0x2f, */ '6', SPEC | 'F', 0,
	/* this key is probably mapped on forign AIMIGA keyboards */
/*      0x30, */ 0, 0, 0,
/*      0x31, */ 'z', 'Z', CTRL | 'Z',
/*      0x32, */ 'x', 'X', CTRL | 'X',
/*      0x33, */ 'c', 'C', CTRL | 'C',
/*      0x34, */ 'v', 'V', CTRL | 'V',
/*      0x35, */ 'b', 'B', CTRL | 'B',
/*      0x36, */ 'n', 'N', CTRL | 'N',
/*      0x37, */ 'm', 'M', CTRL | 'M',
/*      0x38, */ ',', '<', 0,
/*      0x39, */ '.', '>', 0,
/*      0x3a, */ '/', '?', 0,
/*      0x3b, */ 0, 0, 0,
/*      0x3c, */ '.', SPEC | 'D', 0,
/*      0x3d, */ '7', SPEC | '<', 0,
/*      0x3e, */ '8', SPEC | 'P', 0,
/*      0x3f, */ '9', SPEC | 'Z', 0,
/*      0x40, */ ' ', SHFT | ' ', 0,
/*      0x41, */ CTRL | 'H', SHFT | 'D', 0,
/*      0x42, */ CTRL | 'I', SHFT | 'I', 0,
/*      0x43, */ CTRL | 'M', CTRL | 'M', CTRL | 'M',
/*      0x44, */ CTRL | 'M', CTRL | 'M', CTRL | 'M',
/*      0x45, */ CTRL | '[', 0, 0,
/*      0x46, */ SPEC | 'D', 0, 0,
/*      0x47, */ 0, 0, 0,
/*      0x48, */ 0, 0, 0,
/*      0x49, */ 0, 0, 0,
/*      0x4a, */ '-', 0, 0,
/*      0x4b, */ 0, 0, 0,
/*      0x4c, */ SPEC | 'P', SHFT | SPEC | 'P', CTRL | SPEC | 'P',
/*      0x4d, */ SPEC | 'N', SHFT | SPEC | 'N', CTRL | SPEC | 'N',
/*      0x4e, */ SPEC | 'F', SHFT | SPEC | 'F', CTRL | SPEC | 'F',
/*      0x4f, */ SPEC | 'B', SHFT | SPEC | 'B', CTRL | SPEC | 'B',
/*      0x50, */ SPEC | '1', SHFT | SPEC | '1', CTRL | SPEC | '1',
/*      0x51, */ SPEC | '2', SHFT | SPEC | '2', CTRL | SPEC | '2',
/*      0x52, */ SPEC | '3', SHFT | SPEC | '3', CTRL | SPEC | '3',
/*      0x53, */ SPEC | '4', SHFT | SPEC | '4', CTRL | SPEC | '4',
/*      0x54, */ SPEC | '5', SHFT | SPEC | '5', CTRL | SPEC | '5',
/*      0x55, */ SPEC | '6', SHFT | SPEC | '6', CTRL | SPEC | '6',
/*      0x56, */ SPEC | '7', SHFT | SPEC | '7', CTRL | SPEC | '7',
/*      0x57, */ SPEC | '8', SHFT | SPEC | '8', CTRL | SPEC | '8',
/*      0x58, */ SPEC | '9', SHFT | SPEC | '9', CTRL | SPEC | '9',
/*      0x59, */ SPEC | '0', SHFT | SPEC | '0', CTRL | SPEC | '0',
/*      0x5a, */ '(', 0, 0,
/*      0x5b, */ ')', 0, 0,
/*      0x5c, */ '/', 0, 0,
/*      0x5d, */ '*', 0, 0,
/*      0x5e, */ 0, 0, 0,
/*      0x5f, */ SPEC | '?', 0, 0,
};

/* some keyboard keys current states */

int r_shiftflag;		/* right shift key */
int l_shiftflag;		/* left shift key */
int r_altflag;			/* right alt key */
int l_altflag;			/* left alt key */
int r_amiflag;			/* right amiga key */
int l_amiflag;			/* left amiga key */
int ctrlflag;			/* control key */
int lockflag;			/* shift lock key */

/*      output buffers and pointers     */

#define OBUFSIZE	256L
#define	IBUFSIZE	64	/* this must be a power of 2 */

char out_buf[OBUFSIZE + 1];	/* output character buffer */
int out_ptr = 0;		/* index to next char to put in buffer */

/*      input buffers and pointers      */

#define	IBUFSIZE	64	/* this must be a power of 2 */

unsigned char in_buf[IBUFSIZE];	/* input character buffer */
int in_next = 0;		/* pos to retrieve next input character */
int in_last = 0;		/* pos to place most recent input character */

/*************************************************************************
 *
 *	Name:		in_init
 *
 *	Purpose:	initialise the input buffer pointers.
 *
 *************************************************************************/

void amiga_init (int argc, char *argv[])
{
    amiga_scrflag = TRUE;
    amiga_winflag = TRUE;
    dbprintf(("amiga_init: scr: n, win: y\n"));
}

/*************************************************************************
 *
 *	Name:		in_init
 *
 *	Purpose:	initialise the input buffer pointers.
 *
 *************************************************************************/

void in_init ()
{
    in_next = in_last = 0;
}

/*************************************************************************
 *
 *	Name:		in_check
 *
 *	Purpose:	Return TRUE if the input buffer contains waiting
 *			characters.
 *
 *************************************************************************/

int in_check ()
{
    if (in_next == in_last)
	return (FALSE);
    else
	return (TRUE);
}

/*************************************************************************
 *
 *	Name:		in_put
 *
 *	Purpose:	to write a new character (event) into the input
 *			buffer for processing.
 *
 *		int event;	event to enter into the input buffer 
 *
 *************************************************************************/

void in_put (int event)
{
    dbprintf(("in_put: %d\n", event));
    in_buf[in_last++] = event;
    in_last &= (IBUFSIZE - 1);
}

/*************************************************************************
 *
 *	Name:		in_get
 *
 *	Purpose:	get an event from the input buffer.
 *
 *************************************************************************/

int in_get ()
{
    register int event;		/* event to return */

    event = in_buf[in_next++];
    in_next &= (IBUFSIZE - 1);
    dbprintf(("in_get: %d\n", event));
    return (event);
}

/*************************************************************************
 *
 *	Name:		UEGetScreenInfo
 *
 *	Purpose:	get the current WB screen width, height & mode
 *
 *************************************************************************/

void UEGetScreenInfo(WORD *scrWidth, WORD *scrHeight, UWORD *scrMode)
{
    struct Screen *defaultScr;
    int success = FALSE;

    defaultScr = AllocMem (sizeof (struct Screen), MEMF_PUBLIC | MEMF_CLEAR);
    if (defaultScr)
    {
	success = GetScreenData (defaultScr, sizeof (struct Screen),
				 WBENCHSCREEN, NULL);
    }

    if (success)
    {
	*scrWidth = defaultScr->Width;
	*scrHeight = defaultScr->Height;
	*scrMode = defaultScr->ViewPort.Modes;
    }
    else
    {
	*scrWidth = 640;
	*scrHeight = 200;
	*scrMode = HIRES;
    }
    FreeMem (defaultScr, sizeof (struct Screen));

    dbprintf(("screeninfo: %d x %d, m %d\n", *scrWidth, *scrHeight, *scrMode));
}

/*************************************************************************
 *
 *	Name:		UEOpenScreen
 *
 *	Purpose:	Open a screen based on the spec of the current
 *		WB screen; pre OS 2.04 we use the OpenScreen interface,
 *		for OS 2.04 we use the tags interface, with a mod for
 *		OS 3, which can ask for WB to be duplicated, whatever
 *		it really is.
 *
 *************************************************************************/

struct Screen * UEOpenScreen(WORD *scrWidth, WORD *scrHeight)
{
    struct NewScreen newScr;
    struct Screen *screen;

#if	INTUI_REV >= 36
    if (IntuitionBase->LibNode.lib_Version >= 36)
    {
#if	INTUI_REV >= 39
	if (IntuitionBase->LibNode.lib_Version >= 39)
	{
	    screen = OpenScreenTags (0L,
				  SA_LikeWorkbench, TRUE,
		   SA_Title, (ULONG) ((UBYTE *) "MicroEMACS 3.12g/Amiga"),
				  SA_Depth, 1,
				  SA_SysFont, 0,
				  SA_Type, CUSTOMSCREEN,
				  SA_BlockPen, 1,
				  SA_DetailPen, 0
				  TAG_END);
	}
	else
#endif
	{
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
	}
    }
    else
#endif
    {
	memset(&newScr, 0, sizeof newScr);

	UEGetScreenInfo(&newScr.Width, &newScr.Height, &newScr.ViewModes);

	newScr.LeftEdge = 0;
	newScr.TopEdge = 0;
	newScr.Depth = 1;
	newScr.DetailPen = 0;
	newScr.BlockPen = 1;

	newScr.Type = CUSTOMSCREEN;
#ifdef FORCE_TOPAZ80
	newScr.Font = theTextAttr;	/* force use of TOPAZ80 */
#else
	newScr.Font = 0L;	/* Use system default font */
#endif
	newScr.DefaultTitle = (unsigned char *)"MicroEMACS 3.12g/Amiga";
	screen = OpenScreen (&newScr);

    }			/* end of OS1.2/1.3 code */
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

#if 0
    EmacsMenus = CreateMenus( MicroEmacsMenu, GTMN_FrontPen, 0L, TAG_DONE );

    if (screen == NULL && EmacsMenus != NULL)
    {
    	pubScreen = LockPubScreen( PubScreenName );
	if (pubScreen == NULL)
	{
	    FreeMenus(EmacsMenus);
	    EmacsMenus = NULL;
	}
    }
    else
    	pubScreen = screen;

    if ( ! ( VisualInfo = GetVisualInfo( pubScreen, TAG_DONE )))
    {
	FreeMenus(EmacsMenus);
	EmacsMenus = NULL;
    }

    if (EmacsMenus != NULL)
	LayoutMenus( EmacsMenus, VisualInfo, TAG_DONE );

    if (screen == NULL && pubScreen != NULL)
	UnlockPubScreen(NULL, pubScreen);
    if (VisualInfo != NULL)
    	FreeVisualInfo(VisualInfo);
#endif
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

    if (EmacsMenus != NULL && win != NULL )
    	SetMenuStrip(win, EmacsMenus);

    return win;
}

/*************************************************************************
 *
 *	Name:		ttopen
 *
 *	Purpose:	
 *		This function is called once to set up the terminal 
 *	device streams. It sets up the 'os' variable, opens the needed
 *	system libraries, opens up a screen (if requested), a window and
 *	an console driver for the window.
 *
 *************************************************************************/

int ttopen()
{
    int i, width, height;

#ifdef FORCE_TOPAZ80
    static char fontname[] = "topaz.font";
#endif
#if	AZTEC
    extern int Enable_Abort;	/* Turn off ctrl-C interrupt */

    Enable_Abort = 0;		/* for the Manx compiler */
#endif

    strcpy (os, "AMIGADOS");

    GfxBase = (struct GfxBase *)OpenLibrary ("graphics.library", GFX_REV);
    if (GfxBase == NULL)
    {
	printf ("%%Can not open graphics.library version %ld\n", GFX_REV);
	exit (1);
    }

    /* open the intuition library */
    IntuitionBase = (struct IntuitionBase *)
	    OpenLibrary ("intuition.library", INTUITION_REV);
    if (IntuitionBase == NULL)
    {
	if (GfxBase)
	    CloseLibrary ((struct Library *)GfxBase);
	printf ("%%Can not open Intuition version %ld\n", INTUITION_REV);
	exit (1);
    }
    /* open the ASL library */
    AslBase = (struct Library *) OpenLibrary ("asl.library", ASL_REV);
    if (AslBase == NULL)
    {
	if (GfxBase)
	    CloseLibrary ((struct Library *)GfxBase);
	if (IntuitionBase)
	    CloseLibrary ((struct Library *)IntuitionBase);
	printf ("%%Can not open asl.library version %ld\n", ASL_REV);
	exit (1);
    }

#ifdef FORCE_TOPAZ80
    theTextAttr = AllocMem (sizeof (struct TextAttr), MEMF_PUBLIC | MEMF_CLEAR);

    if (theTextAttr == NULL)
    {
	printf ("%%Not enough memory\n");
	ttclose();
	exit (2);
    }
    theTextAttr->ta_Name = fontname;
    theTextAttr->ta_YSize = 8;
    theTextAttr->ta_Style = FS_NORMAL;
    theTextAttr->ta_Flags = FPF_ROMFONT;

    theFont = OpenFont (theTextAttr);
#else
    theFont = GfxBase->DefaultFont;
#endif

    if (amiga_scrflag)
    {
    	scr = UEOpenScreen(&screenWidth, &screenHeight);
    	if (scr == NULL)
    	{
	    /* have a go at opening on the workbench */
	    amiga_scrflag = FALSE;
    	}
    }

    if (NOT amiga_scrflag)
    {
    	UWORD mode;
    	UEGetScreenInfo(&screenWidth, &screenHeight, &mode);
    	scr = NULL;
    }

    win = UEOpenWindow(scr);
    if (win == NULL)
    {
	printf ("%%Can not open a window\n");
    	ttclose();
	exit (2);
    }

#ifdef FORCE_TOPAZ80
    if (theFont != NULL)
	SetFont (win->RPort, theFont);
#endif

    /* and open up the console for output */
    con.io_Data = (APTR) win;
    OpenDevice ("console.device", 0, (struct IORequest *)(&con), 0);
    ConsoleDevice = (struct Library *)con.io_Device;

    /* and init all the keyboard flags */
    r_shiftflag = FALSE;
    l_shiftflag = FALSE;
    r_altflag = FALSE;
    l_altflag = FALSE;
    r_amiflag = FALSE;
    l_amiflag = FALSE;
    ctrlflag = FALSE;
    lockflag = FALSE;

    /* initialize our private event queue */
    in_init ();

    /* font sensitive height and width calculation */
    width = win->Width - (win->BorderLeft + win->BorderRight);
    height = win->Height - (win->BorderTop + win->BorderBottom);

    /*
     * maximum screen size; fairly arbitrary, but it's very rare for
     * a font to have a spacing less than 6x6
     */
    term.t_mcol = screenWidth / 6;
    term.t_mrow = screenHeight / 6;

    /*
     * current screen size
     */
    term.t_ncol = (width / theFont->tf_XSize);
    term.t_nrow = (height / theFont->tf_YSize);

    printf("screen: %d x %d, term: %d cols %d rows\n",
    		width, height, term.t_ncol, term.t_nrow);

    /* set the current sizes */
    newwidth (TRUE, term.t_mcol);
    newsize (TRUE, term.t_mrow);

    /* on all screens we are not sure of the initial position
       of the cursor                                        */
    ttrow = 999;
    ttcol = 999;
}

/*************************************************************************
 *
 *	Name:		ttclose
 *
 *	Purpose:	Close down the Amiga-sepcific bits, including
 *	the window and screen, the console device and the libraries.
 *
 *************************************************************************/

int ttclose ()
{
    dbprintf(("ttclose()\n"));
    /* make sure there is no pending output */
    ttflush ();

    /* and now close up shop */
    if (ConsoleDevice)
    {
    	CloseDevice ((struct IORequest *)(&con));
	ConsoleDevice = 0L;
    }
    if (win)
    {
	if (EmacsMenus)
		SetMenuStrip(win, NULL);
	CloseWindow (win);
    }
    if (scr)
	CloseScreen (scr);
#ifdef FORCE_TOPAZ80
    if (theFont)
	CloseFont (theFont);
    if (theTextAttr)
	FreeMem (theTextAttr, (ULONG) (sizeof (struct TextAttr)));

#endif
    OpenWorkBench ();
    if (AslBase)
	CloseLibrary (AslBase);
    if (IntuitionBase)
	CloseLibrary ((struct Library *)IntuitionBase);
    if (GfxBase)
	CloseLibrary ((struct Library *)GfxBase);
}


/*************************************************************************
 *
 *	Name:		ttputc
 *
 *	Purpose:	
 *		Write a character to the display. Terminal output is
 *	buffered, and we just put the characters in the big array,
 *	checking for overflow.
 *
 *************************************************************************/

int ttputc (int c)
{
    dbprintf(("ttputc(%d)\n", c));

    /* add the character to the output buffer */
    out_buf[out_ptr++] = c;

    /* send the buffer out if we are at the limit */
    if (out_ptr >= OBUFSIZE)
	ttflush ();
}


/*************************************************************************
 *
 *	Name:		ttflush
 *
 *	Purpose:	Flush terminal buffer. Uses sendcon() to do the
 *		real work.
 *
 *************************************************************************/

int ttflush ()
{
    dbprintf(("ttflush\n"));
    /* if there are any characters waiting to display... */
    if (out_ptr)
    {
        dbprintf(("ttflush, send\n"));
	out_buf[out_ptr] = 0;	/* terminate the buffer string */
	sendcon (out_buf);	/* send them out */
	out_ptr = 0;		/* and reset the buffer */
    }
}

/*************************************************************************
 *
 *	Name:		ttgetc
 *
 *	Purpose:	read a character from the terminal. 'characters'
 *		may also be (part of) an 'event' - e.g. a mouse press or
 *		menu selection.
 *
 *************************************************************************/

int ttgetc ()
{
    dbprintf(("ttgetc\n"));
    /* make sure there is no pending output */
nxtchr:
    ttflush ();

    /* if it is already buffered up, get it */
    if (in_check())
	return(in_get());

    /* process an INTUITION event (possibly loading the input buffer) */
    doevent ();
    goto nxtchr;
}

#if	TYPEAH

/*************************************************************************
 *
 *	Name:		typahead
 *
 *	Purpose:	Check to see if any characters are already in the
 *		keyboard buffer. Returns TRUE if more characters available,
 *		FALSE if not.
 *
 *************************************************************************/

int typahead ()
{
    ULONG sigMask = 1L << win->UserPort->mp_SigBit;

  tcheck:			/* if type ahead is already pending... */
    if (in_check ())
    {
	dbprintf(("typahead: Buffered event!\n"));
	return (TRUE);
    }

    /* check the signal for IDCMP events pending */
    /* bad:    if ((SetSignal(0L, 0L) & sigMask) != 0)*/
#if 0
    if (IsMsgPortEmpty(win->UserPort))
    {
	dbprintf(("typahead: IDCMP event!\n"));
	return (TRUE);
    }
#endif
    /* no event in queue... no typeahead ready */
    return (FALSE);
}
#endif

void 
doevent ()
{
    register int eventX, eventY;	/* local copies of the event info */
    struct IntuiMessage *event;	/* current event to repond to */
    struct InputEvent ievent =
    {
	NULL, IECLASS_RAWKEY, 0, 0, 0
    };
    ULONG class;		/* class of event */
    USHORT code;		/* data code */
    SHORT x, y;			/* mouse x/y position at time of event */
    char buf[128];		/*temp buff */

    dbprintf (("doevent: Wait\n"));

    /* wait for an event to occur */
    Wait (1 << win->UserPort->mp_SigBit);

    /* get the event and parse it up */
    while (event = (struct IntuiMessage *)GetMsg (win->UserPort))
    {
	class = event->Class;
	code = event->Code;
	eventX = event->MouseX;
	eventY = event->MouseY;
	ievent.ie_Code = code;
	ievent.ie_Qualifier = event->Qualifier;
	ievent.ie_position.ie_addr = *((APTR *) event->IAddress);

	ReplyMsg ((struct Message *)event);
	dbprintf (("doevent: ReplyMsg\n"));

	/* a normal keystroke? */
	switch (class)
	{
	    case IDCMP_RAWKEY:
		dbprintf (("doevent: RAWKEY\n"));
		dokey (&ievent);
		break;

	    case IDCMP_CLOSEWINDOW:
		/* User clicked on the close gadget! */
		dbprintf (("doevent: CLOSEWINDOW\n"));
		quit (FALSE, 0);
		stuffibuf (255, 0, 0);	/* fake a char to force quit to work */
		break;

	    case IDCMP_NEWSIZE:
		/* resolve the 'mouse' address (border adjusted) */
		x = (win->Width) / CRWIDTH;
		y = (win->Height - scr->BarHeight) / CRHEIGHT;

		dbprintf (("doevent: NEWSIZE: %d x %d\n", x, y));

		if (x > term.t_mcol)
		    x = term.t_mcol;
		if (y > term.t_mrow)
		    y = term.t_mrow;

		stuffibuf (MOUS | '1', x, y);
		break;

	    case IDCMP_MENUPICK:
		dbprintf (("doevent: MENUPICK: %d\n", code));
	    	break;

	    case IDCMP_MENUHELP:
		dbprintf (("doevent: MENUHELP: %d\n", code));
	    	break;

	    case IDCMP_MOUSEBUTTONS:
		/* and lastly, a mouse button press */
		x = (eventX) / CRWIDTH;
		y = (eventY - scr->BarHeight) / CRHEIGHT;

		dbprintf (("doevent: MOUSEBUTTONS: %d x %d\n", x, y));

		if (x > term.t_mcol)
		    x = term.t_mcol;
		if (y > term.t_mrow)
		    y = term.t_mrow;

		switch (code)
		{
		    case 104:
			stuffibuf (MOUS | mod ('a'), x, y);
			break;
		    case 232:
			stuffibuf (MOUS | mod ('b'), x, y);
			break;
		    case 105:
			stuffibuf (MOUS | mod ('e'), x, y);
			break;
		    case 233:
			stuffibuf (MOUS | mod ('f'), x, y);
			break;
		}
		break;
	}
    }
    return;
}

/*************************************************************************
 *
 *	Name:		mod
 *
 *	Purpose:	modify a character by the current shift and
 *		control flags. Returns modified character.
 *
 *	c -- original character.
 *
 *************************************************************************/

int 
mod (int c)		
{
    /* first apply the shift and control modifiers */
    if (l_shiftflag || r_shiftflag || lockflag)
	c -= 32;
    if (ctrlflag)
	c |= CTRL;
    return (c);
}

/*************************************************************************
 *
 *	Name:		sendcon
 *
 *	Purpose:	send a string to the console.
 *
 *	char *buf --  buffer to write out
 *
 *************************************************************************/

void sendcon (char *buf)
{

    /* initialize the IO request */
    con.io_Data = (APTR) buf;
    con.io_Length = strlen (buf);
    con.io_Command = CMD_WRITE;

    dbprintf(("Sendcon %d bytes\n", con.io_Length));

    /* and perform the I/O */
    SendIO ((struct IORequest *)(&con));
}

/*************************************************************************
 *
 *	Name:		dokey
 *
 *	Purpose:	process an incomming keyboard code
 *
 *************************************************************************/

void dokey (struct InputEvent *IEvent)
{
    register int ekey;		/* translate emacs key */
    register int dir;		/* key direction (up/down) */
    char buf[NSTRING];
    char cvbuf[16];
    int numchars;
    int code = IEvent->ie_Code;

    /* decode the direction of the key */
    dir = TRUE;
    if (code > 127)
    {
	code = code & 127;
	dir = FALSE;		/* Key released */
    }
    numchars = RawKeyConvert (IEvent, cvbuf, 16, 0L);

    if (numchars == 1)
    {
	ekey = cvbuf[0];
	/* up keystrokes are ignored for the rest of these */
    }
    else
    {
	if (code >= 0x60)
	{
	    switch (code)
	    {

		case 0x60:
		    l_shiftflag = dir;
		    break;
		case 0x61:
		    r_shiftflag = dir;
		    break;
		case 0x62:
		    lockflag = dir;
		    break;
		case 0x63:
		    ctrlflag = dir;
		    break;
		case 0x64:
		    l_altflag = dir;
		    break;
		case 0x65:
		    r_altflag = dir;
		    break;
		case 0x66:
		    l_amiflag = dir;
		    break;
		case 0x67:
		    r_amiflag = dir;
		    break;

	    }
	    return;
	}
	/* first apply the shift and control modifiers */
	if (ctrlflag)
	    ekey = keytrans[code].rw_ccode;
	else if (l_shiftflag || r_shiftflag || lockflag)
	    ekey = keytrans[code].rw_scode;
	else
	    ekey = keytrans[code].rw_code;
    }
    if (dir == FALSE)
	return;

    if ((numchars != 1) || ((ekey != '@') && (ekey < 0x7F)))
	/* now apply the ALTD modifier */
	if (r_altflag || l_altflag)
	    ekey |= ALTD;

    /* apply the META prefix */
    if (r_amiflag || l_amiflag)
    {
	if ('a' <= ekey && ekey <= 'z')
	    ekey -= 32;
	ekey |= META;
    }

    /* and place it in the input buffer */
    stuffibuf (ekey, 0, 0);
}

/*************************************************************************
 *
 *	Name:		stuffibuf
 *
 *	Purpose:	stuff a key in the input buffer
 *
 *	key --	 extended keystroke to remember 
 *	x, y --	 mouse position to record
 *
 *************************************************************************/

void stuffibuf (int key, int x, int y)
{
    register int upper;		/* upper extended bits of key */

    /* split the extended keystroke */
    upper = key >> 8;
    key = key & 255;

    /* if it is JUST control... encode it in! */
    if (upper == (CTRL >> 8))
    {
	in_put (key - 64);
	return;
    }

    /* if it is normal, just place it inqueue */
    if (upper == 0)
    {
	in_put (key);
	return;
    }

    /* queue up an extended escape sequence */
    in_put (0);			/* escape indicator */
    in_put (upper);		/* event type */
    if (upper & (MOUS >> 8))
    {
	in_put (x);		/* x position */
	in_put (y);		/* y position */
    }
    in_put (key);		/* event code */
    return;
}

/** Callout to system to perform command **/
int 
callout (char *cmd)
/* char *cmd;			* Command to execute           */
{
    int status;

    /* Close down */
    ansimove (term.t_nrow, 0);
    ttflush ();
    ansikclose ();
    ttclose ();

    /* Do command */
    status = SystemTags (cmd, TAG_DONE) == 0;

    /* Restart system */
    sgarbf = TRUE;
    ansikopen ();
    if (ttopen ())
    {
	puts ("** Error reopening terminal device **");
	exit (1);
    }

    /* Success */
    return (status);
}

int 
spawncli (int f, int n)
/* int f;				* Flags                        */
/* int n;				* Argument count               */
{
    /* don't allow this command if restricted */
    if (restflag)
	return (resterr ());

    mlwrite (TEXT1);
/*              "[Starting new CLI]" */
    Execute ("NEWCLI \"CON:0/0/640/200/MicroEMACS Subprocess/CLOSE\"", 0L, 0L);

    return (TRUE);
}

/** Spawn a command **/
int 
spawn (f, n)
int f;				/* Flags                        */
int n;				/* Argument count               */
{
    char line[NLINE];
    int s;

    /* Don't allow this command if restricted */
    if (restflag)
	return (resterr ());

    /* Get command line */
    s = mlreply ("!", line, NLINE);
    if (!s)
	return (s);

    /* Perform the command */
    s = callout (line);

    /* if we are interactive, pause here */
    if (clexec == FALSE)
    {
	mlwrite ("[End]");
	ttflush ();
	ttgetc ();
    }
    return (s);
}

/** Execute program **/
int 
execprg (f, n)
int f;				/* Flags                        */
int n;				/* Argument count               */
{
    /* Same as spawn */
    return (spawn (f, n));
}

/*
 * Pipe a one line command into a window
 * Bound to ^X @
 */
pipecmd (f, n)
{
    register int s;		/* return status from CLI */
    register WINDOW *wp;	/* pointer to new window */
    register BUFFER *bp;	/* pointer to buffer to zot */
    char line[NLINE];		/* command line send to shell */
    static char bname[] = "command";
    static char filnam[] = "t:command";

    /* don't allow this command if restricted */
    if (restflag)
	return (resterr ());

    /* get the command to pipe in */
    if ((s = mlreply ("@", line, NLINE)) != TRUE)
	return (s);

    /* get rid of the command output buffer if it exists */
    if ((bp = bfind (bname, FALSE, 0)) != FALSE)
    {
	/* try to make sure we are off screen */
	wp = wheadp;
	while (wp != NULL)
	{
	    if (wp->w_bufp == bp)
	    {
		onlywind (FALSE, 1);
		break;
	    }
	    wp = wp->w_wndp;
	}
	if (zotbuf (bp) != TRUE)

	    return (FALSE);
    }

    strcat (line, " >");
    strcat (line, filnam);
    s = callout (line);
    if (!s)
	return (s);

    sgarbf = TRUE;

    /* split the current window to make room for the command output */
    if (splitwind (FALSE, 1) == FALSE)
	return (FALSE);

    /* and read the stuff in */
    if (getfile (filnam, FALSE) == FALSE)
	return (FALSE);

    /* make this window in VIEW mode, update all mode lines */
    curwp->w_bufp->b_mode |= MDVIEW;
    wp = wheadp;
    while (wp != NULL)
    {
	wp->w_flag |= WFMODE;
	wp = wp->w_wndp;
    }

    /* and get rid of the temporary file */
    unlink (filnam);
    return (TRUE);
}

/*
 * filter a buffer through an external DOS program
 * Bound to ^X #
 */
filter (f, n)

{
    register int s;		/* return status from CLI */
    register BUFFER *bp;	/* pointer to buffer to zot */
    char line[NLINE];		/* command line send to shell */
    char tmpnam[NFILEN];	/* place to store real file name */
    static char bname1[] = "fltinp";
    static char filnam1[] = "t:fltinp";
    static char filnam2[] = "t:fltout";
    long newcli;

    /* don't allow this command if restricted */
    if (restflag)
	return (resterr ());

    /* Don't allow filtering of VIEW mode buffer */
    if (curbp->b_mode & MDVIEW)
	return (rdonly ());

    /* get the filter name and its args */
    if ((s = mlreply ("#", line, NLINE)) != TRUE)
	return (s);

    /* setup the proper file names */
    bp = curbp;
    strcpy (tmpnam, bp->b_fname);	/* save the original name */
    strcpy (bp->b_fname, bname1);	/* set it to our new one */

    /* write it out, checking for errors */
    if (writeout (filnam1, "w") != TRUE)
    {
	mlwrite (TEXT2);
/*                      "[Cannot write filter file]" */
	strcpy (bp->b_fname, tmpnam);
	return (FALSE);
    }

    /* Setup input and output */
    strcat (line, " <fltinp >fltout");

    /* Perform command */
    s = callout (line);

    /* If successful, read in file */
    if (s)
    {
	s = readin (filnam2, FALSE);
	if (s)
	{
	    /* Mark buffer as changed */
	    bp->b_flag |= BFCHG;
	}
	sgarbf = TRUE;
    }

    /* reset file name */
    strcpy (bp->b_fname, tmpnam);	/* restore name */

    /* and get rid of the temporary file */
    unlink (filnam1);
    unlink (filnam2);

    /* Show status */
    if (!s)
	mlwrite ("[Execution failed]");
    return (TRUE);
}

#if	AZTEC
/* return a system dependant string with the current time */

char *PASCAL NEAR 
timeset ()

{
    return (errorm);
}

#elif	GCC

/** Get time of day **/
char *
timeset ()
{
    time_t buf;
    char *sp, *cp;

    /* Get system time */
    time (&buf);

    /* Pass system time to converter */
    sp = ctime (&buf);

    /* Eat newline character */
    for (cp = sp; *cp; cp++)
	if (*cp == '\n')
	{
	    *cp = '\0';
	    break;
	}
    return (sp);
}

#endif

#if	AZTEC
/*      FILE Directory routines         */

extern char *scdir ();

/*      do a wild card directory search (for file name completion) */

char *PASCAL NEAR 
getffile (fspec)

char *fspec;			/* pattern to match */

{
    register int index;		/* index into various strings */
    char fname[NFILEN];		/* file/path for DOS call */

    /* first parse the file path off the file spec */
    strcpy (path, fspec);
    index = strlen (path) - 1;
    while (index >= 0 && (path[index] != '/' &&
			  path[index] != '\\' && path[index] != ':'))
	--index;
    path[index + 1] = 0;

    /* construct the composite wild card spec */
    strcpy (fname, path);
    strcat (fname, &fspec[index + 1]);
    strcat (fname, "*.*");

    /* save the path/wildcard off */
    strcpy (path, fname);

    /* and call for the first file */
    return (getnfile ());
}

char *PASCAL NEAR 
getnfile ()

{
    register char *sp;		/* return from scdir */

    /* and call for the next file */
    sp = scdir (path);
    if (sp == NULL)
	return (NULL);

    /* return the next file name! */
    strcpy (rbuf, sp);
    return (rbuf);
}

#elif	GCC

/* UNIX like environment, with standard opendir/readdir/closedir calls */

/** Get first filename from pattern **/

char *
getffile (fspec)
char *fspec;			/* Filename specification       */
{
    int index, point, extflag;

    /* First parse the file path off the file spec */
    strcpy (path, fspec);
    index = strlen (path) - 1;
    while (index >= 0 && (path[index] != '/' &&
			  path[index] != '\\' && path[index] != ':'))
	--index;
    path[index + 1] = '\0';

    /* Check for an extension */
    point = strlen (fspec) - 1;
    extflag = FALSE;
    while (point >= 0)
    {
	if (fspec[point] == '.')
	{
	    extflag = TRUE;
	    break;
	}
	point--;
    }

    /* Open the directory pointer */
    if (dirptr)
    {
	closedir (dirptr);
	dirptr = NULL;
    }

    dirptr = opendir ((path[0] == '\0') ? "" : path);

    if (!dirptr)
	return (NULL);

    strcpy (rbuf, path);
    nameptr = &rbuf[strlen (rbuf)];

    /* ...and call for the first file */
    return (getnfile ());
}

/** Get next filename from pattern **/
char *
getnfile ()
{
    int index;
    struct dirent *dp;
    struct stat fstat;

    /* ...and call for the next file */
    do
    {
	dp = readdir (dirptr);
	if (!dp)
	    return (NULL);

	/* Check to make sure we skip all weird entries except directories */
	strcpy (nameptr, dp->d_name);

    }
    while (stat (rbuf, &fstat) ||
	   ((fstat.st_mode & S_IFMT) & (S_IFREG | S_IFDIR)) == 0);

    /* if this entry is a directory name, say so */
    if ((fstat.st_mode & S_IFMT) == S_IFDIR)
	strcat (rbuf, DIRSEPSTR);

    /* Return the next file name! */
    return (rbuf);
}

#else /* NOT GCC or AZTEC envir */

char *PASCAL NEAR 
getffile (fspec)
char *fspec;			/* file to match */
{
    return (NULL);
}

char *PASCAL NEAR 
getnfile ()
{
    return (NULL);
}
#endif

#if	AZTEC

/*      Big flame..... AZTEC C documents, but seems to be missing the
   standard library realloc() function.  A quick replacement is
   below.  Note that this ALWAYS copies to a new block, and is
   thus inherently less efficient than a native one would be.
 */

char *
realloc (ptr, size)

char *ptr;			/* original pointer */
int size;			/* # of bytes for newly allocated block */

{
    char *src, *dest;		/* ptrs for byte copying */
    char *nptr;			/* newly allocated pointer */

    /* allocate the new memory block */
    nptr = malloc (size);
    if (nptr == NULL)
	return (NULL);

    /* copy the bytes from the old one */
    src = ptr;
    dest = nptr;
    while (size--)
	*dest++ = *src++;

    /* and free the old one */
    free (ptr);
    return (nptr);
}

#endif

#if	WINDOW_AMIGA
#if	ASL_REV <= 37

int 
FileReq (char *rtitle, char *filename, unsigned maxlen)
{
    struct FileRequester *frp;
    static char DirStr[256];
    int rc;

    frp = AllocAslRequestTags (ASL_FileRequest,
			       ASL_Window, (ULONG) win,
			       ASL_Hail, (ULONG) rtitle,
			       ASL_FuncFlags, (ULONG) (FILB_PATGAD),
			       ASL_Pattern, (ULONG) ((UBYTE *) "~(#?.info)"),
			       TAG_DONE);
    if (frp != 0L)
    {
	if (AslRequest (frp, 0L) != 0)
	{
	    if ((strlen (frp->rf_Dir) + 1 + strlen (frp->rf_File)) >= maxlen)
	    {
		rc = -1;
	    }
	    else
	    {
		strncpy (filename, frp->rf_Dir, maxlen);
		filename[maxlen] = '\0';
		AddPart (filename, frp->rf_File, maxlen);
		rc = 0;
	    }
	}
	else
	    rc = -1;
	FreeAslRequest (frp);
    }
    else
	rc = -1;
    return rc;
}				/* FileReq */

#else /* ASL_REV >= 38 */

int 
FileReq (char *rtitle, char *filename, unsigned maxlen)
{
    struct FileRequester *frp;
    int rc;

    frp = AllocAslRequestTags (ASL_FileRequest, ASLFR_Screen, (ULONG) scr,
			       ASLFR_TitleText, (ULONG) rtitle,
			       ASLFR_DoPatterns, TRUE,
		       ASLFR_InitialPattern, (ULONG) ((UBYTE *) "~(#?.info)"),
			       ASLFR_InitialHeight, (screenHeight * 11L) >> 4,
			       TAG_DONE);
    if (frp != 0L)
    {
	if (RequestFile (frp) != 0)
	{
	    strncpy (filename, frp->rf_Dir, maxlen);
	    filename[maxlen] = '\0';
	    AddPart (filename, frp->rf_File, maxlen);
	    rc = 0;
	}
	else
	    rc = -1;
	FreeFileRequest (frp);
    }
    else
	rc = -1;
    return rc;
}				/* FileReq */

#endif /* ASL_REV */

#endif /* WINDOW_AMIGA */

#else /* AMIGA */
void adoshello (void);

void 
adoshello (void)
{
}

#endif /* AMIGA */

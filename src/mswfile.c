/* The routines in this file provide support for file access under the
   Microsoft Windows environment on an IBM-PC or compatible computer.

   Must be compiled with Borland C++ 2.0 or MSC 6.0 or later versions

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

#include    "estruct.h"
#include    <stdio.h>
#include    "eproto.h"
#include    "edef.h"

#if WINNT
#define FNAMELEN NFILEN
#else
#define FNAMELEN 13
#endif

#if  TURBO
#include    <dir.h>

struct ffblk fileblock;	/* structure for directory searches */
#endif
#if	MSC | ZTC | IC
#include <dos.h>

#if WINDOW_MSWIN32
WIN32_FIND_DATA fileblock;
HANDLE  filehandle;
#else
struct find_t fileblock;	/* structure for directory searches */
#endif
#endif
#if     MSC
#include <direct.h>
#define getcwd _getcwd
#endif

#include    "mswin.h"

/* macros */
#define ATTR_DIR    (0x4010 | 0x8000)   /* attributes for dir listbox */
#define ATTR_FIL    (0x0021)            /* attributes for file listbox */

/* structures */
typedef struct PARAMS {     /* parameters between filenamedlg and
			       FileDlgProc (pointed by Par) */
    char    Name [FNAMELEN];    /* file name */
    char    *Prompt;            /* prompt text for the caption */
} PARAMS;

/* static variables */
static  char    Path [NFILEN] = "";    /* directory path */
static  char    StarName [FNAMELEN] = "*.*";    /* starname */
static  PARAMS  *Par;

/* function prototypes */
int EXPORT FAR PASCAL FileDlgProc (HWND hDlg, UINT wMsg, UINT wParam,
                                   LONG lParam);
static void    CompletePath (char *s, char *FileName);
static void    UpdateAll (HWND hDlg, char *s);

/* ChangeWorkingDir:    sets the working dir to match the supplied path */
/* ================                                                     */

static int ChangeWorkingDir (char * FilePath)

/* returns 0 if successful, -1 otherwise */
{
    char    *WorkPath;
    char    *Backslash; /* will find the last backslash in the path */
    char    Crushed;
    int     Result;

    WorkPath = FilePath;
    if (*WorkPath == '\0') return 0;    /* empty path! */
    if (WorkPath[1] == ':') {   /* drive specification */
        int     disk;

	_chdrive(disk = (lowerc(*WorkPath) - 'a') + 1);
	if (disk != _getdrive ()) return -1;
	WorkPath += 2;  /* skip that drive spec */
    }
    
    for (Backslash = WorkPath; *Backslash != '\0'; Backslash++) ;
    while (*Backslash != '\\') {
        if (Backslash == WorkPath) break;
	--Backslash;
    }
    /* Backslash now points at the last backslash in the file path. That
       is the end of the directory path */
    if ((Backslash == WorkPath) && (*Backslash == '\\')) ++Backslash;
    Crushed = *Backslash;
    *Backslash = '\0';      /* temporarily terminate the path there */
    if (*WorkPath == '\0') Result = TRUE;
    else Result = chdir (WorkPath);
    *Backslash = Crushed;   /* restore the file path before returning */
    return Result;
} /* ChangeWorkingDir */

/* SetWorkingDir:   sets the working dir to the current window's path */
/* =============                                                      */

int FAR PASCAL SetWorkingDir (void)

/* returns 0 if successful, -1 otherwise */
/* this function also sets the text of the Path displayed in the FILE
   dialog */
{
    int     Result;

    Result = ChangeWorkingDir (curbp->b_fname);
    if (Result == 0) getcwd (Path, NFILEN);
    return Result;
} /* SetWorkingDir */

/* fullpathname:    fully qualifies the given pathname */
/* ============                                        */

char * PASCAL   fullpathname (char *PathName, int Nbuf)

/* the PathName argument is assumed to be at least Nbuf characters
   long. It is modified to contain the corresponding full pathname. The
   returned address is the PathName argument. */
{
    char    FullName [_MAX_PATH];

    if (_fullpath(FullName, PathName, Nbuf) != NULL) {
        strcpy (PathName, FullName);
    }
    return PathName;
} /* fullpathname */

/* filenamedlg: equivalent of mlreply, but specifically to get a filename */
/* ===========                                                            */

PASCAL  filenamedlg (char *prompt, char *buf, int nbuf, int fullpath)
{
    PARAMS  Parameters;
    FARPROC ProcInstance;
    BOOL    Result;

    SetWorkingDir ();
    if (clexec || (kbdmode != STOP)) {  /* not interactive */
        Result = mlreply (prompt, buf, nbuf);
        if (Result == TRUE) {
	    if (fullpath) fullpathname (buf, nbuf);
        }
        return Result;
    }
    Parameters.Prompt = prompt;
    Par = &Parameters;
    ProcInstance = MakeProcInstance ((FARPROC)FileDlgProc, hEmacsInstance);
    if (Result = (DialogBox (hEmacsInstance, "FILE", hFrameWnd,
                             ProcInstance) >= 0)) {
        CompletePath (buf, Parameters.Name);
    }
    FreeProcInstance (ProcInstance);
    SetWorkingDir ();
    return Result;
} /* filenamedlg */

/* FileDlgOK:   process OK in File Dialog */
/* =========                              */

static BOOL PASCAL FileDlgOK (HWND hDlg)

/* this is a service function for FileDlgProc. It processes the OK case.
   The returned value is TRUE if the dialog box is ending, FALSE
   otherwise */
{
    char    s [NFILEN];

    GetDlgItemText (hDlg, ID_FILENAME, s, NFILEN);
    if (*s == 0) return FALSE;  /* empty name, ignore it! */
    if (strchr (s, '*') || strchr (s, '?')) {
	/* there is a starname here! */
	UpdateAll (hDlg, s);
    }
    else {
	int     l;
	char    *n;

	l = strlen (s);
	n = &s[l - 1];
	if ((*n == '\\') || (*n == ':')) {
	    /* it is a directory or drive */
	    if (l < NFILEN - 1 - strlen(StarName)) {
		strcat (s, StarName);
		UpdateAll (hDlg, s);
	    }
	}
	else {
	    /* it looks like a bonafide file name ! */
	    int     nl = 1;

	    /* first, we extract the filename portion...*/
	    do {
		if (n-- == &s[0]) goto ExtractedOK;
		if ((*n == ':') || (*n == '\\')) goto ExtractedOK;
	    } while (++nl < FNAMELEN);
	    return FALSE;
ExtractedOK:
	    strcpy (Par->Name, ++n);
	    if (n - &s[0] < NFILEN - 1 - strlen(StarName)) {
		strcpy (n, StarName);
		/* now, we use DlgDirList to generate the full directory
		   path */
		if (DlgDirList (hDlg, s, NULL, ID_PATH, ATTR_FIL)) {
		    getcwd (Path, NFILEN);
		    EndDialog (hDlg, 0);
		    return TRUE;
		}
	    }
	}
    }
    return FALSE;
} /* FileDlgOK */

/* FileNameCompletion:  process filename edit box for name completion */
/* ==================                                                 */

/* scrolls the file list box to bring the first match into view and
   attempt filename completion if a space is placed at the end of the
   edit field. Returns TRUE if filename completion was attempted and
   successful, FALSE otherwise. */
static BOOL PASCAL FileNameCompletion (HWND hDlg)
{
    char    s [NFILEN];
    int     i;
    BOOL    PleaseComplete = FALSE;

    i = GetDlgItemText (hDlg, ID_FILENAME, s, NFILEN);
    while ((i > 0) && (s[--i] == ' ')) {
	PleaseComplete = TRUE;
	s[i] = '\0';
    }
    if (PleaseComplete) {
	DWORD   LastSel;

	LastSel = SendDlgItemMessage (hDlg, ID_FILENAME, EM_GETSEL, 0, 0L);
        SetDlgItemText (hDlg, ID_FILENAME, s);  /* remove the spaces */
#if WINDOW_MSWIN32
        SendDlgItemMessage (hDlg, ID_FILENAME, EM_SETSEL,
                            (UINT)LOWORD(LastSel), (DWORD)HIWORD(LastSel));
#else
        SendDlgItemMessage (hDlg, ID_FILENAME, EM_SETSEL, 0, LastSel);
#endif
    }
    if (strchr (s, '\\') || strchr (s, ':') || strchr (s, '[')) {
        return FALSE;   /* contains more than a plain file name. The
			   file list box will not be appropriate for
			   completion so we do not attempt anything */
    }
    i = SendDlgItemMessage (hDlg, ID_FILES, LB_SELECTSTRING,
                            -1, (DWORD)(LPSTR)&s[0]);
    if (i == LB_ERR) {
        /* no match, give up! */
        return FALSE;
    }
    if (GetFocus () != GetDlgItem (hDlg, ID_FILES)) {
        SendDlgItemMessage (hDlg, ID_FILES, LB_SETTOPINDEX, i, 0L);
    }
    if (PleaseComplete) {
        if (i != SendDlgItemMessage (hDlg, ID_FILES, LB_FINDSTRING,
                                     i, (DWORD)(LPSTR)&s[0])) {
            return FALSE;   /* not unique ==> completion fails */
        }
        else {
            SendDlgItemMessage (hDlg, ID_FILES, LB_GETTEXT,
                                i, (DWORD)(LPSTR)&s[0]);
            SetDlgItemText (hDlg, ID_FILENAME, s);
            return TRUE;
        }
    }
    return FALSE;
} /* FileNameCompletion */

/* FileDlgProc: Open file dialog function */
/* ===========                            */
int EXPORT FAR PASCAL  FileDlgProc (HWND hDlg, UINT wMsg, UINT wParam,
                                    LONG lParam)
{
    char    s [NFILEN];    /* all purpose */
    int     i;
    
    switch (wMsg) {
        
    case WM_INITDIALOG:
	{   /* let's build the caption */
	    char    DlgTitle [sizeof(PROGNAME) + 3 + 30];

	    strcpy (DlgTitle, ProgName);
	    strcat (DlgTitle, " - ");
	    strcat (DlgTitle, Par->Prompt); /* hopefully, the prompt is
					       under 30 char! */
	    i = strlen (DlgTitle) - 1;
	    while (DlgTitle[i] == ' ') i--;
	    if (DlgTitle[i] == ':') DlgTitle[i] = 0;
	        /* we remove the colon+spaces at the end of the prompt */
	    SetWindowText (hDlg, DlgTitle);
	}
	SetFocus (GetDlgItem (hDlg, ID_FILENAME));
        CompletePath (s, StarName);
        UpdateAll (hDlg, s);
        i = 0;
        while (in_check()) {
            /* we need to send to the dialog box the characters stored
	       into the in_put() buffer. For instance, if the user typed
	       ^X^F while the startup script was running (to specify the
	       first file to read in) and quickly followed this by
	       typing a file name. The Find file dialog box would not
	       receive those characters which would already have been
	       absorbed into the in_put() pipe and would later end up
	       inserted at the beginning of the buffer! */
	    int     c;

	    c = in_get();
	    switch (c) {
	    case 0: /* escape sequence, discard it... */
	        if (in_get() & (MOUS >> 8)) {
	            in_get();
	            in_get();
	        }
	        in_get();
	        break;
	    case '\b':  /* backspace */
		if (i > 0) i--;
		break;
	    case '\r':  /* Enter */
		s[i] = '\0';
		SetDlgItemText (hDlg, ID_FILENAME, s);
		if (FileDlgOK (hDlg)) goto NoMoreTypeAhead;
		i = 0;
		break;
	    case 0x1B:  /* Escape */
	        EndDialog (hDlg, -1);
		goto NoMoreTypeAhead;
	    default:
	        if ((c > 0x1F) && (c < 0x7F)) {
	            /* regular ASCII char, stuff it into the filename */
	            s[i++] = c;
	        }
	        /* else, discard it */
	        break;
	    }
	}
        if (i > 0) {
            s[i] = '\0';
	    SetDlgItemText (hDlg, ID_FILENAME, s);
	}
#if WINDOW_MSWIN32
	SendDlgItemMessage (hDlg, ID_FILENAME, EM_SETSEL, i, -1);
#else
	SendDlgItemMessage (hDlg, ID_FILENAME, EM_SETSEL,
	                    0, MAKELONG(i, -1));
#endif
	if (FileNameCompletion (hDlg)) FileDlgOK (hDlg);
NoMoreTypeAhead:
	return FALSE;
	
    case WM_COMMAND:
	switch (LOWORD(wParam)) {

	case ID_FILENAME:
	    if (NOTIFICATION_CODE == EN_CHANGE) {
	        if (FileNameCompletion (hDlg)) FileDlgOK (hDlg);
	    }
	    break;
	    
	case ID_DIRECTORIES:
	    switch (NOTIFICATION_CODE) {
	    case LBN_SELCHANGE:
#if WINDOW_MSWIN32
		DlgDirSelectEx (hDlg, s, NFILEN -1 - strlen (StarName),
                                ID_DIRECTORIES);
#else
		DlgDirSelect (hDlg, s, ID_DIRECTORIES);
#endif
		strcat (s, StarName);
		SetDlgItemText (hDlg, ID_FILENAME, s);
		break;
	    case LBN_DBLCLK:
		FileDlgOK (hDlg);   /* same as OK */
	    }
	    break;
	    
	case ID_FILES:
	    switch (NOTIFICATION_CODE) {
	    case LBN_SELCHANGE:
#if WINDOW_MSWIN32
		DlgDirSelectEx (hDlg, s, NFILEN -1, ID_FILES);
#else
		DlgDirSelect (hDlg, s, ID_FILES);
#endif
		i = strlen (s) - 1;
		if (s[i] == '.') s[i] = 0;  /* zap dot at end of file
					       name */
	        SetDlgItemText (hDlg, ID_FILENAME, s);
		break;
	    case LBN_DBLCLK:
		FileDlgOK (hDlg);   /* same as OK */
	    }
	    break;
	    
	case IDOK:
            FileDlgOK (hDlg);
	    break;
	    
	case IDCANCEL:
	    EndDialog (hDlg, -1);
	    break;
	}
	break;
	
    default:
	return FALSE;
    }
    return FALSE;
} /* FileDlgProc */

/* CompletePath:  prepend Path to the FileName, result in s */
/* ============                                             */

static void    CompletePath (char *s, char *FileName)

/* s must be at least NFILEN characters long, while the length of Path +
   the length of FileName must be < NFILEN */
{
    strcpy (s, Path);
    if ((*s != 0) && (s[strlen (s) - 1] != '\\')) strcat (s, "\\");
    strcat (s, FileName);
} /* CompletePath */

/* UpdateAll:   updates all the controls from the path in s */
/* =========                                                */

static void    UpdateAll (HWND hDlg, char *s)

/* this function also keeps the static variables Path and StarName up to
   date */
{
    if (DlgDirList (hDlg, s, ID_DIRECTORIES, ID_PATH, ATTR_DIR)) {
        getcwd (Path, NFILEN);
        strcpy (StarName, s);
	DlgDirList (hDlg, s, ID_FILES, NULL, ATTR_FIL);
        SetDlgItemText (hDlg, ID_FILENAME, StarName);
#if WINDOW_MSWIN32
	SendDlgItemMessage (hDlg, ID_FILENAME, EM_SETSEL, 0, -1);
#else
	SendDlgItemMessage (hDlg, ID_FILENAME, EM_SETSEL, 0, MAKELONG(0, -1));
#endif
    }
} /* UpdateAll */

#if	TURBO | IC
/*	FILE Directory routines		*/
/* all borrowed from MSDOS.C */

char path[NFILEN];	/* path of file to find */
char rbuf[NFILEN];	/* return file buffer */

/*	do a wild card directory search (for file name completion) */

char *PASCAL getffile(fspec)

char *fspec;	/* pattern to match */

{
	register int index;		/* index into various strings */
	register int point;		/* index into other strings */
	register int extflag;		/* does the file have an extention? */
	char fname[NFILEN];		/* file/path for DOS call */

	/* first parse the file path off the file spec */
	strcpy(path, fspec);
	index = strlen(path) - 1;
	while (index >= 0 && (path[index] != '/' &&
				path[index] != '\\' && path[index] != ':'))
		--index;
	path[index+1] = 0;

	/* check for an extension */
	point = strlen(fspec) - 1;
	extflag = FALSE;
	while (point > index) {
		if (fspec[point] == '.') {
			extflag = TRUE;
			break;
		}
		point--;
	}

	/* construct the composite wild card spec */
	strcpy(fname, path);
	strcat(fname, &fspec[index+1]);
	strcat(fname, "*");
	if (extflag == FALSE)
		strcat(fname, ".*");

	/* and call for the first file */
	if (findfirst(fname, &fileblock, FA_DIREC) == -1)
		return(NULL);

	/* return the first file name! */
	strcpy(rbuf, path);
	strcat(rbuf, fileblock.ff_name);
	mklower(rbuf);
	if (fileblock.ff_attrib == 16)
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}

char *PASCAL getnfile()

{
	register int index;		/* index into various strings */
	register int point;		/* index into other strings */
	register int extflag;		/* does the file have an extention? */
	char fname[NFILEN];		/* file/path for DOS call */

	/* and call for the first file */
	if (findnext(&fileblock) == -1)
		return(NULL);

	/* return the first file name! */
	strcpy(rbuf, path);
	strcat(rbuf, fileblock.ff_name);
	mklower(rbuf);
	if (fileblock.ff_attrib == 16)
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}
#else
#if	(MSC || ZTC)
/*	FILE Directory routines		*/

char path[NFILEN];	/* path of file to find */
char rbuf[NFILEN];	/* return file buffer */

/*	do a wild card directory search (for file name completion) */

char *PASCAL getffile(fspec)

char *fspec;	/* pattern to match */

{
	register int index;		/* index into various strings */
	register int point;		/* index into other strings */
	register int extflag;		/* does the file have an extention? */
	char fname[NFILEN];		/* file/path for DOS call */

	/* first parse the file path off the file spec */
	strcpy(path, fspec);
	index = strlen(path) - 1;
	while (index >= 0 && (path[index] != '/' &&
				path[index] != '\\' && path[index] != ':'))
		--index;
	path[index+1] = 0;

	/* check for an extension */
	point = strlen(fspec) - 1;
	extflag = FALSE;
	while (point > index) {
		if (fspec[point] == '.') {
			extflag = TRUE;
			break;
		}
		point--;
	}

	/* construct the composite wild card spec */
	strcpy(fname, path);
	strcat(fname, &fspec[index+1]);
	strcat(fname, "*");
	if (extflag == FALSE)
		strcat(fname, ".*");

	/* and call for the first file */
#if WINDOW_MSWIN32
        if ((filehandle = FindFirstFile (fname, &fileblock)) ==
            INVALID_HANDLE_VALUE)
#else
	if (_dos_findfirst(fname, _A_NORMAL|_A_SUBDIR, &fileblock) != 0)
#endif
		return(NULL);

	/* return the first file name! */
	strcpy(rbuf, path);
#if WINDOW_MSWIN32
        strcat(rbuf, fileblock.cFileName);
        mklower(rbuf);
        if (fileblock.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
#else
	strcat(rbuf, fileblock.name);
	mklower(rbuf);
	if (fileblock.attrib == 16)
#endif
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}

char *PASCAL getnfile()

{
	register int index;		/* index into various strings */
	register int point;		/* index into other strings */
	register int extflag;		/* does the file have an extention? */
	char fname[NFILEN];		/* file/path for DOS call */

	/* and call for the next file */
#if WINDOW_MSWIN32
        if (!FindNextFile (filehandle, &fileblock)) {
            FindClose(filehandle);
            return (NULL);
        }
#else
	if (_dos_findnext(&fileblock) != 0)
		return(NULL);
#endif

	/* return the first file name! */
	strcpy(rbuf, path);
#if WINDOW_MSWIN32
        strcat(rbuf, fileblock.cFileName);
        mklower(rbuf);
        if (fileblock.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
#else
	strcat(rbuf, fileblock.name);
	mklower(rbuf);
	if (fileblock.attrib == 16)
#endif
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}
#else
char *PASCAL getffile(fspec)

char *fspec;	/* file to match */

{
	return(NULL);
}

char *PASCAL getnfile()

{
	return(NULL);
}
#endif
#endif



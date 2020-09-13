/*	FILE.C:   for MicroEMACS

	The routines in this file handle the reading, writing
	and lookup of disk files.  All of details about the
	reading and writing of the disk are in "fileio.c".

*/

#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"
#if	BSD | FREEBSD | SUN | USG | AIX
#include	<sys/types.h>
#include	<sys/stat.h>
# if	BACKUP
#  include <errno.h>
# endif
#endif
#if	THEOS
# include <sys/stat.h>
# include <unistd.h>
#endif
#if	(MSDOS|TOS)
#  include <io.h>
#  include <errno.h>
#  include <sys\types.h>
#  include <sys\stat.h>
#endif
/*
 * Read a file into the current
 * buffer. This is really easy; all you do is
 * find the name of the file, and call the standard
 * "read a file into the current buffer" code.
 * Bound to "C-X C-R".
 */
int PASCAL NEAR fileread(f, n)

int f, n;	/* default and numeric arguments (unused) */

{
	char *fname;	/* file name to read */

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());

	if ((fname = gtfilename(TEXT131)) == NULL)
/*                              "Read file" */
		return(FALSE);
#if	THEOS4
	if (! addlname(fname))
		return (FALSE);
#endif
	return(readin(fname, TRUE));
}

/*
 * Insert a file into the current
 * buffer. This is really easy; all you do it
 * find the name of the file, and call the standard
 * "insert a file into the current buffer" code.
 * Bound to "C-X C-I".
 */
int PASCAL NEAR insfile(f, n)

int f,n;	/* prefix flag and argument */

{
	register int	s;
	char *fname;	/* file name */
	LINE *curline = NULL;

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
	if (curbp->b_mode&MDVIEW)      /* don't allow this command if  */
		return(rdonly());	/* we are in read only mode	*/

	if ((fname = gtfilename(TEXT132)) == NULL)
/*                              "Insert file" */
		return(FALSE);
#if	THEOS4
	if (! addlname(fname))
		return (FALSE);
#endif
	/*
	 * Save the local pointers to hold global ".", in case
	 * $yankflag is set to 1.  Insert-file always places the
	 * starting offset point at 0.  Hold *previous* line
	 * position, since the current line may be re-allocated.
	 */
	if (yankflag)
		curline = lback(curwp->w_dotp);

	s = ifile(fname);

	if (yankflag)
		curwp->w_dotp = lforw(curline);

	return (s);
}

/*
 * Select a file for editing.
 * Look around to see if you can find the
 * fine in another buffer; if you can find it
 * just switch to the buffer. If you cannot find
 * the file, create a new buffer, read in the
 * text, and switch to the new buffer.
 * Bound to C-X C-F.
 */
int PASCAL NEAR filefind(f, n)

int f,n;	/* prefix flag and argument */

{
	char *fname;	/* file user wishes to find */	/* file name */

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());

	if ((fname = gtfilename(TEXT133)) == NULL)
/*                              "Find file" */
		return(FALSE);
#if	THEOS4
	if (! addlname(fname))
		return (FALSE);
#endif
	return(getfile(fname, TRUE));
}

int PASCAL NEAR viewfile(f, n)	/* visit a file in VIEW mode */

int f,n;	/* prefix flag and argument */

{
	char *fname;	/* file user wishes to find */	/* file name */
	register int s;	/* status return */

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());

	if ((fname = gtfilename(TEXT134)) == NULL)
/*                              "View file" */
		return(FALSE);
#if	THEOS4
	if (! addlname(fname))
		return (FALSE);
#endif
	s = getfile(fname, FALSE);
	if (s) {	/* if we succeed, put it in view mode */
		curwp->w_bufp->b_mode |= MDVIEW;
		upmode();
	}
	return(s);
}

#if	CRYPT
int PASCAL NEAR resetkey()	/* reset the encryption key if needed */

{
	register int s; /* return status */

	/* turn off the encryption flag */
	cryptflag = FALSE;

	/* if we are in crypt mode */
	if (curbp->b_mode & MDCRYPT) {
		if (curbp->b_key[0] == 0) {
			s = setekey(FALSE, 0);
			if (s != TRUE)
				return(s);
		}

		/* let others know... */
		cryptflag = TRUE;

		/* and set up the key to be used! */
		/* de-encrypt it */
		ecrypt((char *)NULL, 0);
		ecrypt(curbp->b_key, strlen(curbp->b_key));

		/* re-encrypt it...seeding it to start */
		ecrypt((char *)NULL, 0);
		ecrypt(curbp->b_key, strlen(curbp->b_key));
	}

	return(TRUE);
}
#endif

int PASCAL NEAR getfile(fname, lockfl)

char fname[];		/* file name to find */
int lockfl;		/* check the file for locks? */

{
	register BUFFER *bp;
	register LINE	*lp;
	register int	i;
	register int	s;
	ESCREEN		*sp;	/* screen pointer, if we need it */
	char bname[NBUFN];	/* buffer name to put file */
	char prompt[NSTRING];	/* string for collisions prompt */

#if	MSDOS | WINNT | WINXP | AOSVS | VMS | TOS | THEOS4
	mklower(fname); 	       /* msdos isn't case sensitive */
#endif
	for (bp=bheadp; bp!=NULL; bp=bp->b_bufp) {
		if ((bp->b_flag&BFINVS)==0 && strcmp(bp->b_fname, fname)==0) {
			swbuffer(bp);
			lp = curwp->w_dotp;
			i = curwp->w_ntrows/2;
			while (i-- && lback(lp)!=curbp->b_linep)
				lp = lback(lp);
			curwp->w_linep = lp;
			curwp->w_flag |= WFMODE|WFHARD;
			mlwrite(TEXT135);
/*                              "[Old buffer]" */
			return(TRUE);
		}
	}
	makename(bname, fname); 		/* New buffer name.	*/

	/* prevent buffer name conflicts */
	while ((bp=bfind(bname, FALSE, 0)) != NULL) {

		/* first, come up with our own name */
		unqname(bname);

		/* if interactive, let em change it if they dislike our names */
		if (clexec == FALSE) {

			strcpy(prompt, TEXT136);
/*				       "Buffer name: " */
			strcpy(&prompt[strlen(prompt) - 2], "[");
			strcat(prompt, bname);
			strcat(prompt, "]: ");
			s = mlreply(prompt, bname, NBUFN);

			if (s == ABORT) 	/* ^G to just quit	*/
				return(s);
			if (s == FALSE) {	/* CR to let the computer pick */
				makename(bname, fname);	/* New buffer name. */
				unqname(bname);		/* which is unique */
			}
		}
	}

	/* create the new buffer */
	if (bp==NULL && (bp=bfind(bname, TRUE, 0))==NULL) {
		mlwrite(TEXT137);
/*                      "Cannot create buffer" */
		return(FALSE);
	}

	/*
	 * Check $newscreen, see if we make a
	 * new screen for the new file.
	 */
	if (newscreenflag) {
		sp = lookup_screen(bname);
		if (sp == (ESCREEN *)NULL) {
			/* screen does not exist, create it */
			sp = init_screen(bname, bp);
		}
		select_screen(sp, FALSE);
	}

	swbuffer(bp);			/* switch to the new buffer */
	return(readin(fname, lockfl));	/* Read it in.		*/
}

/*
	Read file "fname" into the current buffer, blowing away any text
	found there.  Called by both the read and find commands.  Return
	the final status of the read.  Also called by the mainline, to
	read in a file specified on the command line as an argument.
	The command in $readhook is called after the buffer is set up
	and before it is read.
*/

int PASCAL NEAR readin(fname, lockfl)

CONST char*	fname;	/* name of file to read */
int	lockfl;		/* check for file locks? */

{
	register LINE *lp1;
	register LINE *lp2;
	register int i;
	register EWINDOW *wp;
	register BUFFER *bp;
	register int s;
	register long nline;
	register int cmark;	/* current mark */
	int nbytes;
	char mesg[NSTRING];

#if	FILOCK
	int force_read = FALSE;

	if (lockfl && lockchk(fname) == ABORT)
		force_read = TRUE;
#endif

	bp = curbp;				/* Cheap.		*/
	if ((s=bclear(bp)) != TRUE)		/* Might be old.	*/
		return(s);
	bp->b_flag &= ~(BFINVS|BFCHG);
	strcpy(bp->b_fname, fname);

	/* let a user macro get hold of things...if he wants */
	execkey(&readhook, FALSE, 1);

#if	CRYPT
	/* set up for decryption */
	s = resetkey();
	if (s != TRUE)
		return(s);
#endif

	/* turn off ALL keyboard translation in case we get a dos error */
	TTkclose();

	if ((s=ffropen(fname)) == FIOERR)	/* Hard file open.	*/
		goto out;

	if (s == FIOFNF) {			/* File not found.	*/
		mlwrite(TEXT138);
/*                      "[New file]" */
		goto out;
	}

	/* read the file in */
	mlwrite(TEXT139);
/*              "[Reading file]" */
	nline = 0L;
	while ((s=ffgetline(&nbytes)) == FIOSUC) {
		if ((lp1=lalloc(nbytes)) == NULL) {
			s = FIOMEM;		/* Keep message on the	*/
			break;			/* display.		*/
		}
		lp2 = lback(curbp->b_linep);
		lp2->l_fp = lp1;
		lp1->l_fp = curbp->b_linep;
		lp1->l_bp = lp2;
		curbp->b_linep->l_bp = lp1;
		for (i=0; i<nbytes; ++i)
			lputc(lp1, i, fline[i]);
		++nline;
	}
	ffclose();				/* Ignore errors.	*/

#if	BSD || FREEBSD || USG || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX || AVION
	/* if we don't have write priviledges, make this in VIEW mode */
	if (s !=FIOERR && s != FIOFNF) {
		if (access(fname, 2 /* W_OK*/) != 0)
			curbp->b_mode |= MDVIEW;
	}
#endif

	strcpy(mesg, "[");
	if (s==FIOERR) {
		strcat(mesg, TEXT141);
/*                           "I/O ERROR, " */
		curbp->b_flag |= BFTRUNC;
	}
	if (s == FIOMEM) {
		strcat(mesg, TEXT142);
/*                           "OUT OF MEMORY, " */
		curbp->b_flag |= BFTRUNC;
	}
	strcat(mesg, TEXT140);
/*                   "Read " */
	strcat(mesg, long_asc(nline));
	strcat(mesg, TEXT143);
/*                   " line" */
	if (nline > 1L)
		strcat(mesg, "s");
	strcat(mesg, "]");
	mlwrite(mesg);

out:
	TTkopen();	/* open the keyboard again */
	for (wp=wheadp; wp!=NULL; wp=wp->w_wndp) {
		if (wp->w_bufp == curbp) {
			wp->w_linep = lforw(curbp->b_linep);
			wp->w_dotp  = lforw(curbp->b_linep);
			wp->w_doto  = 0;
			for (cmark = 0; cmark < NMARKS; cmark++) {
				wp->w_markp[cmark] = NULL;
				wp->w_marko[cmark] = 0;
			}
			wp->w_flag |= WFMODE|WFHARD;
		}
	}
#if	FILOCK
	if (force_read == TRUE) {
		curbp->b_mode |= MDVIEW;
		upmode();
	}
#endif
	if (s == FIOERR || s == FIOFNF) 	/* False if error.	*/
		return(FALSE);
	return(TRUE);
}

/*
 * Take a file name, and from it
 * fabricate a buffer name. This routine knows
 * about the syntax of file names on the target system.
 * I suppose that this information could be put in
 * a better place than a line of code.
 * Returns a pointer into fname indicating the end of the file path; i.e.,
 * 1 character BEYOND the path name.
 */
CONST char *PASCAL NEAR makename(bname, fname)

char *bname;
CONST char *fname;

{
	register CONST char *cp1;
	register char *cp2;
	register CONST char *pathp;

#if	THEOS
#if	THEOS4
	register int np = 0;

	cp1 = fname;
	while (*cp1) {
		if (*cp1 == '.')
			np++;
		++cp1;
	}
	if (np != 2) {
		while (cp1 != fname && cp1[-1] != '/' && *cp1 != '\\')
			--cp1;
	} else
		cp1 = strrchr(fname,'.')+1;

	/* cp1 is pointing to the first real filename char */
	pathp = cp1;

	cp2 = bname;
	while (cp2!=&bname[NBUFN-1] && *cp1 && *cp1!=':' && *cp1!='.')
		*cp2++ = *cp1++;
	*cp2 = 0;
#else
	cp1 = fname;
	while (*cp1 != 0)
		++cp1;

	while (cp1 != fname && cp1[-1] != '/' && *cp1 != '\\')
		--cp1;

	/* cp1 is pointing to the first real filename char */
	pathp = cp1;

	cp2 = bname;
	while (cp2!=&bname[NBUFN-1] && *cp1 && *cp1!=':')
		*cp2++ = *cp1++;
	*cp2 = 0;
#endif
#else	/* ?THEOS */
#if     AOSVS | MV_UX
        resolve_full_pathname(fname, fname);
        mklower(fname);   /* aos/vs not case sensitive */
#endif
	cp1 = &fname[0];
	while (*cp1 != 0)
		++cp1;

#if	AMIGA
	while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!='/')
		--cp1;
#endif
#if     AOSVS | MV_UX
        while (cp1!=&fname[0] && cp1[-1]!=':')
                --cp1;
#endif
#if	VMS
	while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!=']')
		--cp1;
#endif
#if	MSDOS | OS2 | WINNT | WINXP
	while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!='\\'&&cp1[-1]!='/')
		--cp1;
#endif
#if	TOS
	while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!='\\')
		--cp1;
#endif
#if	FINDER
	while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!='\\'&&cp1[-1]!='/')
		--cp1;
#endif
#if	USG | LINUX | AIX | AUX | SMOS | HPUX8 | HPUX9 | BSD | FREEBSD | SUN | XENIX | AVIION
	while (cp1!=&fname[0] && cp1[-1]!='/')
		--cp1;
#endif
#if WMCS
	while (cp1!=&fname[0] && cp1[-1]!='_' && cp1[-1]!='/')
		--cp1;
#endif
	/* cp1 is pointing to the first real filename char */
	pathp = cp1;

	cp2 = &bname[0];
	while (cp2!=&bname[NBUFN-1] && *cp1!=0 && *cp1!=';')
		*cp2++ = *cp1++;
	*cp2 = 0;

#endif	/* !THEOS */
	return(pathp);
}

VOID PASCAL NEAR unqname(name)	/* make sure a buffer name is unique */

char *name;	/* name to check on */

{
	register char *sp;

	/* check to see if it is in the buffer list */
	while (bfind(name, 0, FALSE) != NULL) {

		/* go to the end of the name */
		sp = name;
		while (*sp)
			++sp;
		if (sp == name || (*(sp-1) <'0' || *(sp-1) > '8')) {
			*sp++ = '0';
			*sp = 0;
		} else
		      *(--sp) += 1;
	}
}

/*
 * Ask for a file name, and write the
 * contents of the current buffer to that file.
 * Update the remembered file name and clear the
 * buffer changed flag. This handling of file names
 * is different from the earlier versions, and
 * is more compatable with Gosling EMACS than
 * with ITS EMACS. Bound to "C-X C-W" for writing
 * and ^X^A for appending.
 */

int PASCAL NEAR filewrite(f, n)

int f, n;	/* emacs arguments */

{
	register int s;
	char *fname;

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());

	if ((fname = gtfilename(TEXT144)) == NULL)
/*                     "Write file: " */
		return(FALSE);
#if	THEOS4
	if (! addlname(fname))
		return (FALSE);
#endif
	if ((s=writeout(fname, "w")) == TRUE) {
		strcpy(curbp->b_fname, fname);
		curbp->b_flag &= ~BFCHG;
		/* Update mode lines.	*/
		upmode();
	}
	return(s);
}

int PASCAL NEAR fileapp(f, n)	/* append file */

int f, n;	/* emacs arguments */

{
	register int s;
	char *fname;

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
	if ((fname = gtfilename(TEXT218)) == NULL)
/*                     "Append file: " */
		return(FALSE);
	if ((s=writeout(fname, "a")) == TRUE) {
		curbp->b_flag &= ~BFCHG;
		/* Update mode lines.	*/
		upmode();
	}
	return(s);
}

/*
 * Save the contents of the current
 * buffer in its associatd file. Do nothing
 * if nothing has changed (this may be a bug, not a
 * feature). Error if there is no remembered file
 * name for the buffer. Bound to "C-X C-S". May
 * get called by "C-Z".
 */
int PASCAL NEAR filesave(f, n)

int f,n;	/* prefix flag and argument */

{
	register int s;

	if (curbp->b_mode&MDVIEW)	/* don't allow this command if	*/
		return(rdonly());	/* we are in read only mode	*/
	if ((curbp->b_flag&BFCHG) == 0) 	/* Return, no changes.	*/
		return(TRUE);
	if (curbp->b_fname[0] == 0) {		/* Must have a name.	*/
		mlwrite(TEXT145);
/*                      "No file name" */
		return(FALSE);
	}

	/* complain about truncated files */
	if ((curbp->b_flag&BFTRUNC) != 0) {
		if (mlyesno(TEXT146) == FALSE) {
/*                          "Truncated file..write it out" */
			mlwrite(TEXT8);
/*                              "[Aborted]" */
			return(FALSE);
		}
	}

	/* complain about narrowed buffers */
	if ((curbp->b_flag&BFNAROW) != 0) {
		if (mlyesno(TEXT147) == FALSE) {
/*                          "Narrowed Buffer..write it out" */
			mlwrite(TEXT8);
/*                              "[Aborted]" */
			return(FALSE);
		}
	}

	if ((s=writeout(curbp->b_fname, "w")) == TRUE) {
		curbp->b_flag &= ~BFCHG;
		/* Update mode lines.	*/
		upmode();
	}
	return(s);
}

#if	(MSDOS|TOS)
char* backname(char* tname, char* fn)
{
	char* p;

	strcpy(tname, fn);
	if ((p = strchr(tname, '.') != NULL)
		*p = '\0';

	return strcat(tname, ".bak");
}

#elif	BSD | SUN | V7 | USG | LINUX

char* backname(char* tname, CONST char* fn)
{
	strcpy(tname, fn);
	return strcat(tname, "~");
}

#elif THEOS4

extern char* backname(char* tname, CONST char* fn)
{
	char fname[NFILEN];
	char rname[NFILEN];
	char dname[NFILEN];
	char* p;
	char* q;
	char* d;

	strcpy(fname, fn);

	if ((p = strchr(fname, ':')) != NULL) {
		strcpy(dname, p);
		*p = '\0';
	} else
		dname[0] = '\0';

	if ((q = strrchr(fname, '/')) == NULL)
		q = fname;

	if ((p = strchr(q, '.')) != NULL) {
		strcpy(tname, fname);

		if ((q = strchr(p + 1, '.')) == NULL) {
			strcpy(strrchr(tname, '.'), ".BACKUP");
		} else {
			if ((p = strrchr(tname, '/')) == NULL)
				p = tname;
			else
				p++;

			strcpy(p, q + 1);
			strcat(p, ".BACKUP");
		}
	} else
		strcat(strcpy(tname, fname), ".BACKUP");

	strcat(tname, dname);
	return tname;
}

#elif THEOS

extern char* backname(char* tname, CONST char* fn)
{
	char dname[NFILEN];
	char* p;

	strcpy(tname, fn);

	if ((p = strchr(tname, ':')) != NULL) {
		strcpy(dname, p);
		*p = '\0';
	} else
		dname[0] = '\0';

	strcat(tbame, ".bak");
	strcat(tname, dname);
	return tname;
}

#endif
/*
 * This function performs the details of file writing. It uses
 * the file management routines in the "fileio.c" package. The
 * number of lines written is displayed. Several errors are
 * posible, and cause writeout to return a FALSE result. When
 * $ssave is TRUE,  the buffer is written out to a temporary
 * file, and then the old file is unlinked and the temporary
 * renamed to the original name.  Before the file is written,
 * a user specifyable routine (in $writehook) can be run.
 */

int PASCAL NEAR writeout(fn, mode)

char *fn;	/* name of file to write current buffer to */
char *mode;	/* mode to open file (w = write a = append) */
{
	register LINE *lp;	/* line to scan while writing */
	register char *sp;	/* temporary string pointer */
	register long nline;	/* number of lines written */
	int status;		/* return status */
	int sflag;		/* are we safe saving? */
	char tname[NFILEN];	/* temporary file name */
	char bname[NFILEN];	/* backup file name */
	char buf[NSTRING];	/* message buffer */
#if	BSD | FREEBSD | SUN | XENIX | USG | AIX | THEOS
	struct stat st;		/* we need info about the file permisions */
#endif
#if	BACKUP
# if	(MSDOS|TOS)
#  if	TOS
#   define EACCES AEACCDN
#  endif	/* TOS */

	struct	stat st;
	extern	char *strchr();
	char	*p;
# endif	/* (MSDOS|TOS) */
#endif	/* BACKUP */

	/* let a user macro get hold of things...if he wants */
	execkey(&writehook, FALSE, 1);

	/* determine if we will use the save method */
    sflag = ((ssave
#if BACKUP
              || (curbp->b_mode & MDNOBAK) == 0
#endif
              )
		&& fexist(fn) && *mode == 'w') ? TRUE : FALSE;

#if	CRYPT
	/* set up for file encryption */
	status = resetkey();
	if (status != TRUE)
		return(status);
#endif

	/* turn off ALL keyboard translation in case we get a dos error */
	TTkclose();

#if	BACKUP
	backname(bname, fn);

	if (sflag && access(bname, 0) == 0 && unlink(bname) == EOF) {
		TTkopen();
		mlwrite(TEXT49, bname);
		return(FALSE);
	}
#endif /* BACKUP */

	/* Perform Safe Save..... */
	if (sflag) {
		/* duplicate original file name, and find where to trunc it */
		sp = tname + (makename(tname, fn) - fn) + 1;
		strcpy(tname, fn);

		/* create a unique name, using random numbers */
		do {
			*sp = 0;
			strcat(tname, int_asc(ernd() & 0xffff));
		} while(fexist(tname));

		/* open the temporary file */
#if     AOSVS
                status = ffwopen(fn, "w", tname);
#else
		status = ffwopen(tname, "w");
#endif
	} else
#if     AOSVS
                status = ffwopen(fn, mode, NULL);
#else
		status = ffwopen(fn, mode);
#endif

	/* if the open failed.. clean up and abort */
	if (status != FIOSUC) {
		TTkopen();
		return(FALSE);
	}

	/* write the current buffer's lines to the open disk file */
	mlwrite(TEXT148);	/* tell us that we're writing */
/*              "[Writing...]" */
	lp = lforw(curbp->b_linep);	/* start at the first line.	*/
	nline = 0L;			/* track the Number of lines	*/
	while (lp != curbp->b_linep) {
		if ((status = ffputline(&lp->l_text[0], lused(lp))) != FIOSUC)
			break;
		++nline;
		lp = lforw(lp);
	}


	/* report on status of file write */
	*buf = 0;
	status |= ffclose();
	if (status == FIOSUC) {
		/* report on success (or lack therof) */
		strcpy(buf, TEXT149);
/*                          "[Wrote " */
		strcat(buf, long_asc(nline));
		strcat(buf, TEXT143);
/*                          " line" */
		if (nline > 1L)
			strcat(buf, "s");

		if (sflag) {
#if	BSD | FREEBSD | SUN | XENIX | USG | AIX | THEOS
			/* get the permisions on the original file */
#if defined(S_ISLNK) && ! THEOS
#if defined(UW7) || defined(LINUX)
			struct stat stl;
			stat(fn, &st);
			lstat(fn, &stl);
#else
			statlstat(fn, &st);
#endif

			if (S_ISLNK(stl.st_mode)) {
				char ln[NFILEN];
				int l;
				l = readlink(fn, ln, sizeof(ln));
				if (l > -1) {
					ln[l] = '\0';
					strcpy(fn, ln);
				}
			}
#endif
			stat(fn, &st);
#endif
#if BACKUP
			/* move original file to backup */
			status = rename(fn, bname);
#else
			/* remove original file */
			status = unlink(fn);
#endif
			/* rename temporary file to original name */
			if (status == 0 && rename(tname, fn) == 0) {
#if	BSD | FREEBSD | SUN | XENIX | USG | AIX
				chown(fn, (int)st.st_uid, (int)st.st_gid);
				chmod(fn, (int)st.st_mode);
#else
#if	THEOS
				_chstat(fn, st.st_protect);
#else
				;
#endif
#endif
			} else {
				strcat(buf, TEXT150);
/*                                          ", saved as " */
				strcat(buf, tname);
				status = FIODEL;	    /* failed */
			}
		}
		strcat(buf, "]");
		mlwrite(buf);
	}

#if BACKUP
	if (curbp->b_mode & MDNOBAK)
		unlink(bname);
#endif

	/* reopen the keyboard, and return our status */
	TTkopen();
	return(status == FIOSUC);
}

/*
 * The command allows the user
 * to modify the file name associated with
 * the current buffer. It is like the "f" command
 * in UNIX "ed". The operation is simple; just zap
 * the name in the BUFFER structure, and mark the windows
 * as needing an update. You can type a blank line at the
 * prompt if you wish.
 */

int PASCAL NEAR filename(f, n)

int f,n;	/* prefix flag and argument */

{
	register int	s;
	char		fname[NFILEN];

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
	if ((s=FILENAMEREPLY(TEXT151, fname, NFILEN)) == ABORT)
/*                     "Name: " */
		return(s);
	if (s == FALSE)
		strcpy(curbp->b_fname, "");
	else {
#if	THEOS4
		if (! addlname(fname))
			return (FALSE);
#endif
		strcpy(curbp->b_fname, fname);
	}
	/* Update mode lines.	*/
	upmode();
	curbp->b_mode &= ~MDVIEW;      /* no longer read only mode */
	return(TRUE);
}

/*
 * Insert file "fname" into the current
 * buffer, Called by insert file command. Return the final
 * status of the read.
 */
int PASCAL NEAR ifile(fname)
char	fname[];
{
	register LINE *lp0;
	register LINE *lp1;
	register LINE *lp2;
	register int i;
	register BUFFER *bp;
	register int s;
	register long nline;
	int nbytes;
	int cmark;	/* current mark */
	char mesg[NSTRING];

	bp = curbp;				/* Cheap.		*/
	bp->b_flag |= BFCHG;			/* we have changed	*/
	bp->b_flag &= ~BFINVS;			/* and are not temporary*/
	if ((s=ffropen(fname)) == FIOERR)	/* Hard file open.	*/
		goto out;
	if (s == FIOFNF) {			/* File not found.	*/
		mlwrite(TEXT152);
/*                      "[No such file]" */
		return(FALSE);
	}
	mlwrite(TEXT153);
/*              "[Inserting file]" */

#if	CRYPT
	s = resetkey();
	if (s != TRUE)
		return(s);
#endif
	/* back up a line and save the mark here */
	curwp->w_dotp = lback(curwp->w_dotp);
	curwp->w_doto = 0;
	for (cmark = 0; cmark < NMARKS; cmark++) {
		if (curwp->w_markp[cmark] == lforw(curwp->w_dotp)) {
			curwp->w_markp[cmark] = curwp->w_dotp;
			curwp->w_marko[cmark] = 0;
		}
	}

	nline = 0L;
	while ((s=ffgetline(&nbytes)) == FIOSUC) {
		if ((lp1=lalloc(nbytes)) == NULL) {
			s = FIOMEM;		/* Keep message on the	*/
			break;			/* display.		*/
		}
		lp0 = curwp->w_dotp;  /* line previous to insert */
		lp2 = lp0->l_fp;	/* line after insert */

		/* re-link new line between lp0 and lp2 */
		lp2->l_bp = lp1;
		lp0->l_fp = lp1;
		lp1->l_bp = lp0;
		lp1->l_fp = lp2;

		/* and advance and write out the current line */
		curwp->w_dotp = lp1;
		for (i=0; i<nbytes; ++i)
			lputc(lp1, i, fline[i]);
		++nline;
	}
	ffclose();				/* Ignore errors.	*/
	for (cmark = 0; cmark < NMARKS; cmark++)
		if (curwp->w_markp[cmark] == lback(curwp->w_dotp))
			curwp->w_markp[cmark] = lforw(curwp->w_markp[cmark]);

	strcpy(mesg, "[");
	if (s==FIOERR) {
		strcat(mesg, TEXT141);
/*                           "I/O ERROR, " */
		curbp->b_flag |= BFTRUNC;
	}
	if (s == FIOMEM) {
		strcat(mesg, TEXT142);
/*                           "OUT OF MEMORY, " */
		curbp->b_flag |= BFTRUNC;
	}
	strcat(mesg, TEXT154);
/*                   "Inserted " */
	strcat(mesg, long_asc(nline));
	strcat(mesg, TEXT143);
/*                   " line" */
	if (nline > 1L)
		strcat(mesg, "s");
	strcat(mesg, "]");
	mlwrite(mesg);

out:
	/* advance to the next line and mark the window for changes */
	curwp->w_dotp = lforw(curwp->w_dotp);
	curwp->w_flag |= WFHARD | WFMODE;

	/* copy window parameters back to the buffer structure */
	curbp->b_dotp = curwp->w_dotp;
	curbp->b_doto = curwp->w_doto;
	for (cmark = 0; cmark < NMARKS; cmark++) {
		curbp->b_markp[cmark] = curwp->w_markp[cmark];
		curbp->b_marko[cmark] = curwp->w_marko[cmark];
	}
	curbp->b_fcol = curwp->w_fcol;

	if (s == FIOERR)			/* False if error.	*/
		return(FALSE);
	return(TRUE);
}

/*	show-files	Bring up a fake buffer and list the
			names of all the files in a given directory
*/

int PASCAL NEAR showfiles(f, n)

int f,n;	/* prefix flag and argument */

{
	register BUFFER *dirbuf;/* buffer to put file list into */
	char outseq[NSTRING];	/* output buffer for file names */
	char *sp;		/* output ptr for file names */
	char mstring[NSTRING];	/* string to match cmd names to */
	int status;		/* status return */

	/* ask what directory mask to search */
	status = mlreply("Directory to show: ", mstring, NSTRING - 1);
	if (status == ABORT)
		return(status);

	/* get a buffer for the file list */
	dirbuf = bfind("File List", TRUE, BFINVS);
	if (dirbuf == NULL || bclear(dirbuf) == FALSE) {
		mlwrite("Can not display file list");
/*			"Can not display function list" */
		return(FALSE);
	}

	/* let us know this is in progress */
	mlwrite("[Building File List]");

	/* get the first file name */
	sp = getffile(mstring);

	while (sp) {

		/* add a name to the buffer */
		strcpy(outseq, sp);
		if (addline(dirbuf, outseq) != TRUE)
			return(FALSE);

		/* and get the next name */
		sp = getnfile();
	}

	/* display the list */
	wpopup(dirbuf);
	mlerase();	/* clear the mode line */
	return(TRUE);
}

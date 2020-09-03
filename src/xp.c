/*      XP.C:           Operating specific I/O and Spawning functions
 *                      for the Window XP operating system (console mode)
 *                      for MicroEMACS 5.00
 *                      (C)Copyright 2008 by Daniel M. Lawrence
 *
 * Note:  don't try to compile this on non Windows NT systems....  The header
 *        files won't exist.
 *
 * Modifications needed: check that we don't construct command lines and
 * temporary filenames which are too large for their buffers.
 *
 */


#undef  PASCAL
#undef  NEAR
#undef  HIBYTE

#include <windows.h>

#include        "estruct.h"
#include        "eproto.h"

#if     WINXP

#include "elang.h"
#include "edef.h"
#include <process.h>
#include <io.h>

/*
 * Create a subjob with a copy of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as garbage so that you do a full
 * repaint. Bound to "^X C". The message at the start in VMS puts out a newline.
 * Under some (unknown) condition, you don't get one free when DCL starts up.
 */
spawncli(f, n)
{
        /* don't allow this command if restricted */
        if (restflag)
                return(resterr());

        movecursor(term.t_nrow, 0);             /* Seek to last line.   */
        TTflush();
        TTkclose();
        shell();
        TTkopen();
        sgarbf = TRUE;
        return(TRUE);
}


/*
 * Run a one-liner in a subjob. When the command returns, wait for a single
 * character to be typed, then mark the screen as garbage so a full repaint is
 * done. Bound to "C-X !".
 */
spawn(f, n)
{
        register int s;
        char line[NLINE];

        /* don't allow this command if restricted */
        if (restflag)
                return(resterr());

        if ((s=mlreply("!", line, NLINE)) != TRUE)
                return(s);
        movecursor(term.t_nrow - 1, 0);
        TTkclose();
        system(line);
        TTkopen();
        /* if we are interactive, pause here */
        if (clexec == FALSE) {
                puts(TEXT6);
/*                     "\r\n\n[End]" */
                tgetc();
        }
        sgarbf = TRUE;
        return (TRUE);
}


/*
 * Run an external program with arguments. When it returns, wait for a single
 * character to be typed, then mark the screen as garbage so a full repaint is
 * done. Bound to "C-X $".
 */

execprg(f, n)
{
        register int s;
        char line[NLINE];

        /* don't allow this command if restricted */
        if (restflag)
                return(resterr());

        if ((s=mlreply("$", line, NLINE)) != TRUE)
                return(s);
        movecursor(term.t_nrow - 1, 0);
        TTkclose();
        execprog(line);
        TTkopen();
        /* if we are interactive, pause here */
        if (clexec == FALSE) {
                puts(TEXT6);
/*                     "\r\n\n[End]" */
                tgetc();
        }
        sgarbf = TRUE;
        return (TRUE);
}

/*
 * Pipe a one line command into a window
 * Bound to ^X @
 * We use a unique temporary file name so that multiple instances of
 * MicroEMACS don't try to use the same file.
 */
pipecmd(f, n)
{
        register EWINDOW *wp;    /* pointer to new window */
        register BUFFER *bp;    /* pointer to buffer to zot */
        register char *tmp;     /* ptr to TMP DOS environment variable */
        char line[NLINE];       /* command line send to shell */
        static char bname[] = "command";
        static char filnam[NSTRING];

        /* don't allow this command if restricted */
        if (restflag)
                return(resterr());

        /* get rid of the command output buffer if it exists */
        if ((bp=bfind(bname, FALSE, 0)) != FALSE) {
                /* try to make sure we are off screen */
                wp = wheadp;
                while (wp != NULL) {
                        if (wp->w_bufp == bp) {
                                onlywind(FALSE, 1);
                                break;
                        }
                        wp = wp->w_wndp;
                }
                /* get rid of the existing command buffer */
                if (zotbuf(bp) != TRUE)
                        return(FALSE);
        }

        /* get the command to pipe in */
        if (mlreply("@", line, NLINE) != TRUE)
                return(FALSE);

        /* Call mktemp() to get a unique filename in the tmp directory. */
        if ((tmp = getenv("TMP")) == NULL)
                filnam[0] = 0;
        else {
                strcpy(filnam, tmp);
                if (filnam[strlen(filnam) - 1] != '\\')
                        strcat(filnam, "\\");
        }
        strcat(filnam,"eXXXXXX");
        mktemp(filnam);

        /* redirect the command output to the output file */
        strcat(line, " >>");
        strcat(line, filnam);
        movecursor(term.t_nrow - 1, 0);

        /* execute the command */
        TTkclose();
        system(line);
        TTkopen();
        sgarbf = TRUE;

        /* did the output file get generated? */
        if (access( filnam, 0) != 0)
                return(FALSE);

        /* split the current window to make room for the command output */
        if (splitwind(FALSE, 1) == FALSE)
                        return(FALSE);

        /* and read the stuff in */
        if (getfile(filnam, FALSE) == FALSE)
                return(FALSE);

        /* rename the buffer */
        strcpy( curwp->w_bufp->b_bname, "command");
        /* make this window in VIEW mode, update all mode lines */
        curwp->w_bufp->b_mode |= MDVIEW;
        wp = wheadp;
        while (wp != NULL) {
                wp->w_flag |= WFMODE;
                wp = wp->w_wndp;
        }

        /* and get rid of the temporary file */
        unlink(filnam);
        return(TRUE);
}


/*
 * filter a buffer through an external DOS program
 * Bound to ^X #
 * We use unique temporary file names so that multiple instances of
 * MicroEMACS don't try to use the same file.
 */
uefilter(f, n)

{
        register int    s;      /* return status from CLI */
        register BUFFER *bp;    /* pointer to buffer to zot */
        char line[NLINE];       /* command line send to shell */
        char tmpnam[NFILEN];    /* place to store real file name */
        char *tmp;              /* ptr to TMP DOS environment variable */

        static char filnam1[NSTRING];
        static char filnam2[NSTRING];

        /* don't allow this command if restricted */
        if (restflag)
                return(resterr());

        if (curbp->b_mode&MDVIEW)       /* don't allow this command if  */
                return(rdonly());       /* we are in read only mode     */

        /* get the filter name and its args */
        if ((s=mlreply("#", line, NLINE)) != TRUE)
                return(s);

        /* Call mktemp() to get unique filenames in the tmp directory. */
        if ((tmp = getenv("TMP")) == NULL)
                filnam1[0] = filnam2[0] = 0;
        else {
                strcpy(filnam1, tmp);
                strcpy(filnam2, tmp);
                if (filnam1[strlen(filnam1) - 1] != '\\') {
                        strcat(filnam1, "\\");
                        strcat(filnam2, "\\");
                }
        }
        strcat(filnam1,"eXXXXXX");
        strcat(filnam2,"eXXXXXX");
        mktemp(filnam1);

        /* setup the proper file names */
        bp = curbp;
        strcpy(tmpnam, bp->b_fname);    /* save the original name */
        strcpy(bp->b_fname, filnam1);   /* set it to our new one */

        /* write it out, checking for errors */
        if (writeout(filnam1, "w") != TRUE) {
                mlwrite(TEXT2);
/*                      "[Cannot write filter file]" */
                strcpy(bp->b_fname, tmpnam);
                return(FALSE);
        }

        mktemp(filnam2);
        strcat(line, " <");             /* construct the command line */
        strcat(line, filnam1);
        strcat(line, " >");
        strcat(line, filnam2);

        movecursor(term.t_nrow - 1, 0);
        TTkclose();
        system(line);
        TTkopen();
        sgarbf = TRUE;
        s = TRUE;

        /* on failure, escape gracefully */
        if (s != TRUE || (readin(filnam2,FALSE) == FALSE)) {
                mlwrite(TEXT3);
/*                      "[Execution failed]" */
                strcpy(bp->b_fname, tmpnam);
                unlink(filnam1);
                unlink(filnam2);
                return(s);
        }

        /* reset file name */
        strcpy(bp->b_fname, tmpnam);    /* restore name */
        bp->b_flag |= BFCHG;            /* flag it as changed */

        /* and get rid of the temporary file */
        unlink(filnam1);
        unlink(filnam2);
        return(TRUE);
}



/*      SHELL: Bring up a shell. */

shell(void)
{
        char *shell;            /* Name of system command processor */


        /*  get name of system shell  */
        if ((shell = getenv("COMSPEC")) == NULL) {
                return(FALSE);          /*  No shell located  */
        }
#ifdef __BORLANDC__
        return (spawnlp(_P_WAIT, shell, shell, NULL));
#else
        return (_spawnlp(_P_WAIT, shell, NULL));
#endif
}



execprog( char *cmd)
{
        char             args[NSTRING];         /* args passed to program */
        char            *sp;
        char             prog[NSTRING];         /* name of program */
        USHORT           i;

        /*
         * Parse the command name from the command line and copy it
         * into the prog and args arrays.
         */
        i = 0;
        while (cmd[i]  &&  (cmd[i] != ' ')  &&  (cmd[i] != '\t')) {
                prog[i] = args[i] = cmd[i];
                i++;
        }
        prog[i] = args[i] = 0;          /* terminate with a null */

        /* skip whitespace and copy the args */
        while (cmd[i]  &&  ((cmd[i] == ' ')  ||  (cmd[i] == '\t')))
                i++;
        while (cmd[i]) {
                args[i] = cmd[i];
                i++;
        }
        args[i] = args[i + 1] = 0;      /* terminate with 2 nulls */


        /* look up the program on the path, trying various extensions */
        if ((sp = flook(prog, TRUE)) == NULL)
                if ((sp = flook(strcat(prog, ".exe"), TRUE)) == NULL) {
                        return(FALSE);
                }
        strcpy(prog, sp);

        /*
         * Execute the program synchronously.  We wait for child
         * to return.
         */
        return (0 == spawnl(P_WAIT, prog, args, NULL));
}


/*      FILE Directory routines         */

struct _finddata_t pBuf; /* buffer to hold file information */
long lDir;              /* directory handle */
int num_found;          /* number of directory entries found/to find */

char path[NFILEN];      /* path of file to find */
char rbuf[NFILEN];      /* return file buffer */

/*      do a wild card directory search (for file name completion) */

char *getffile(fspec)

char *fspec;    /* pattern to match */

{
        register int index;             /* index into various strings */
        register int point;             /* index into other strings */
        register int extflag;           /* does the file have an extension? */
        char fname[NFILEN];             /* file/path for DOS call */

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
        num_found = 1;
        if ((lDir = _findfirst(fname, &pBuf)) == -1)
                return(NULL);

        /* return the first file name! */
        strcpy(rbuf, path);
        strcat(rbuf, pBuf.name);
        mklower(rbuf);
        if (pBuf.attrib == _A_SUBDIR)
                strcat(rbuf, DIRSEPSTR);
        return(rbuf);
}

char *getnfile()

{
        /* and call for the next file */
        if (_findnext(lDir, &pBuf) != 0)
                return(NULL);

        /* return the first file name! */
        strcpy(rbuf, path);
        strcat(rbuf, pBuf.name);
        mklower(rbuf);
        if (pBuf.attrib == _A_SUBDIR)
                strcat(rbuf, DIRSEPSTR);
        return(rbuf);
}

/* return a system dependent string with the current time */

char *timeset()

{
        register char *sp;      /* temp string pointer */
        char buf[16];           /* time data buffer */
        extern char *ctime();

        time(buf);
        sp = ctime(buf);
        sp[strlen(sp)-1] = 0;
        return(sp);
}

#endif

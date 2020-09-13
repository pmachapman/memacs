/*
 * __setargv.c - command argument expander
 *
 * Author  : Jean-Michel Dubois
 * Date	   : 09/26/92
 *
 * Function: Expands the command line arguments by replacing any filename
 *	     including wilcards by the sorted list of matching files name.
 *	     Strings beginning by a dash are considered as options and left
 *	     unchanged.
 *
 * Syntax  : void _setargv(int *argc, char ***argv);
 *
 * Returns : new argc. Caller's argc and argv are updated.
 *	     If a insufficient memory condition occurs, return 0 and errno
 *	     is set to ENOMEM.
 *
 * Example :
 *		main(int argc, char **argv)
 *		{
 *			if (_setargv(&argc, &argv)) {
 *				...
 */ 
#pragma library

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <scr.h>
#include <peek.h>

/* Allocate argv array in 16 entries chunks */

static int allocarg(int n, int l, char ***nargv, char *s)
{
    if ((n+1) > l) {    /* If array full */
        l += 16;        /* increase size and reallocate */
        if (!(*nargv = (char **) realloc(*nargv,l * sizeof (void *)))) {
            errno = _errnum = ENOMEM;    /* Not enough memory */
            return 0; 
        }
    }
    (*nargv)[n] = strdup(s);    /* Save argument */
    return l;            /* Return new maxsize */
}

/* Comparison function for qsort */

static int sortcmp(char **p, char **q)
{
    return stricmp(*p,*q);
}

/* Main body of the function */

int _setargv(int *argc, char ***argv)
{
    register int nargc;     /* New arguments counter */
    char **nargv;           /* New arguments pointers */
    register int i, l, base;
    char *p, *q, *r;
    char path[_MAX_DIR+_MAX_DRIVE];
    char deflib[_MAX_DIR+_MAX_DRIVE];
    char* namecpy;
    _errnum = 0;
    nargc = 0;          /* Initialise counter, size counter */
    l = *argc;          /* and new argument vector to the */
                        /* current argv array size */

    if (getenv("LIBRARY")) {
    	strcat(strcpy(deflib, getenv("LIBRARY")), ".");
    	namecpy = strend(deflib);
    } else
    	*deflib = '\0';
    	
    if ((nargv = (char **) calloc((size_t) *argc, sizeof (void *))) != NULL) {
        /* For each initial argument */
        for (i = 0; i < *argc; i++) {
            q = (*argv)[i];
            if (q[0] == '-' || ! testwild(q)) {
                /* if it begins with a dash or doesnt include
                 * wildcard simply add it to the new array
                 */
                if (! (l = allocarg(nargc, l, &nargv, q)))
                    return 0;    /* Not enough memory */
                nargc++;
            } else {
                /* else keep current counter for qsort */
                base = nargc;
                /* use default library ? */
                if (strchr(q, '.') == NULL && *deflib != '\0') {
                    strcpy(namecpy, q);
                    diropen(deflib);
                } else {
                    /* open directory with argument */
                    diropen(q);
                }
                while ((r = dirread()) != NULL) {
                    /* reduce path to given one */
                    if ((p = strrchr(q, '/')) != NULL) {
                        strncpy(path, q, p-q+1);
                        path[p-q+1] = '\0';
                    } else
                        path[0] = '\0';
                        
                    if ((p = strrchr(r, '/')) != NULL && *deflib == '\0')
                        strcat(path, p+1);
                    else
                        strcat(path, r);
                        
                    if (peekscr(&SCR->searchseq[1]) == 255
                     && strchr(q, ':') == NULL) {
                        *strchr(path, ':') = '\0';
                    }
                    /* and add each matching filename. */
                    if (! (l = allocarg(nargc,l,&nargv,path)))
                        return 0;/* Not enough memory */
                    nargc++;
                }
                if (nargc == base) {
                    /* if no match found include wild card name */
                    if (! (l = allocarg(nargc, l, &nargv, q)))
                        return 0;    /* Not enough memory */
                    nargc++;
                } else if ((nargc - base) > 1)
                    /* If more than one file name matchs */
                    /* sort arguments. */
                    qsort(&(nargv[base]),(size_t)nargc-base,
                        sizeof(void *),sortcmp);
                dirclose();
            }
        }
        /* Update caller's parameters */
        *argc = nargc;
        *argv = nargv;
        /* and sign on success */
        return nargc;
    }

    /* If it is not possible to allocate initial array, sign on error */
    _errnum = ENOMEM;
    return 0;
}

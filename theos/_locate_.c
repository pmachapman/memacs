#include "estruct.h"

#if	THEOS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sc.h>

/* locate fails when stating a file in root dir from a directory with a
 * relative path. Workaround by setting directory to root dir
 * getting the file directory block, then restoring the current directory.
 */

struct fdb* _locate_(const char* fn, char* buf, short* drv)
{
    struct fdb* fdb;
    char buf2[256];
    char cwd[256];
    char* p;
    char* q;

    /* return if file found */
    if (fdb = _locate(fn, buf, drv))
    	return fdb;

    /* if file name does not contain a path delimiter it really does not exist.
     */
    strcpy(buf2, fn);

    if ((p = strrchr(buf2, '/')) == NULL)
        return NULL;

    /* get drive name from file path */
    q = strrchr(buf2, ':');

    /* cat drive name if any to directory path */
    if (q)
        strcat(buf2, q);
    else
        *p = '\0';
    /* save current directory */
    getcwd(cwd, 256);
    /* chdir to directory path */
    chdir(buf2);
    /* get File Directory Block */
    p = strrchr(fn, '/');
    fdb = _locate(p + 1, buf, drv);
    /* restore current directory */
    chdir(cwd);
    return fdb;
}

#endif

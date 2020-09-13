/*	LOCK:	File locking command routines for MicroEMACS
		written by Daniel Lawrence
								*/

#include <stdio.h>
#include "estruct.h"
#include "eproto.h"
#include "edef.h"
#include "elang.h"

#if	FILOCK

#if	BSD || FREEBSD || WMCS || SUN || XENIX || HPUX8 || HPUX9 || AVIION || USG || AIX || AUX
#include <sys/errno.h>
extern int sys_nerr;		/* number of system error messages defined */
extern CONST char * CONST sys_errlist[];	/* list of message texts */
#endif

#if	MSC
#include <errno.h>
#else
extern int errno;		/* current error */
#endif

char *lname[NLOCKS];	/* names of all locked files */
int numlocks;		/* # of current locks active */

/* lockchk:	check a file for locking and add it to the list */

int lockchk(fname)

CONST char *fname;	/* file to check for a lock */

{
	register int i;		/* loop indexes */
	register int status;	/* return status */

	/* check to see if that file is already locked here */
	if (numlocks > 0)
		for (i=0; i < numlocks; ++i)
			if (strcmp(fname, lname[i]) == 0)
				return(TRUE);

	/* if we have a full locking table, bitch and leave */
	if (numlocks == NLOCKS) {
		mlwrite(TEXT173);
/*                      "LOCK ERROR: Lock table full" */
		return(ABORT);
	}

	/* next, try to lock it */
	status = xlock(fname);
	if (status == ABORT)	/* file is locked, no override */
		return(ABORT);
	if (status == FALSE)	/* locked, overriden, dont add to table */
		return(TRUE);

	/* we have now locked it, add it to our table */
	lname[++numlocks - 1] = (char *)room(strlen(fname) + 1);
	if (lname[numlocks - 1] == NULL) {	/* room failure */
		undolock(fname);		/* free the lock */
		mlwrite(TEXT174);
/*                      "Cannot lock, out of memory" */
		--numlocks;
		return(ABORT);
	}

	/* everthing is cool, add it to the table */
	strcpy(lname[numlocks-1], fname);
	return(TRUE);
}

/*	lockrel:	release all the file locks so others may edit */

int lockrel()

{
	register int status;	/* status of locks */
	register int s;		/* status of one unlock */

	status = TRUE;
	while (numlocks-- > 0) {
		if ((s = xunlock(lname[numlocks])) != TRUE)
			status = s;
		free(lname[numlocks]);
	}
	return(status);
}

/* lock:	Check and lock a file from access by others
		returns	TRUE = files was not locked and now is
			FALSE = file was locked and overridden
			ABORT = file was locked, abort command
*/

int xlock(fname)

CONST char *fname;	/* file name to lock */

{
	register char *locker;	/* lock error message */
	register int status;	/* return status */
	char msg[NSTRING];	/* message string */

	/* attempt to lock the file */
	locker = dolock(fname);
	if (locker == NULL)	/* we win */
		return(TRUE);

	/* file failed...abort */
	if (strncmp(locker, TEXT175, 4) == 0) {
/*                          "LOCK" */
		lckerror(locker);
		return(ABORT);
	}

	/* someone else has it....override? */
	strcpy(msg, TEXT176);
/*                  "File in use by " */
	strcat(msg, locker);
	strcat(msg, TEXT177);
/*                  ", overide?" */
	status = mlyesno(msg);		/* ask them */
	if (status == TRUE)
		return(FALSE);
	else
		return(ABORT);
}

/*	xunlock: Unlock a file
		this only warns the user if it fails
							*/

int xunlock(fname)

char *fname;	/* file to unlock */

{
	register char *locker;	/* undolock return string */

	/* unclock and return */
	locker = undolock(fname);
	if (locker == NULL)
		return(TRUE);

	/* report the error and come back */
	lckerror(locker);
	return(FALSE);
}

VOID lckerror(errstr)	/* report a lock error */

char *errstr;		/* lock error string to print out */

{
	char obuf[NSTRING];	/* output buffer for error message */

	strcpy(obuf, errstr);
	strcat(obuf, " - ");
#if	BSD || FREEBSD || WMCS || SUN || XENIX || HPUX8 || HPUX9 || AVIION || USG || AIX || AUX
	strcat(obuf, strerror(errno));
#else
	strcat(obuf, "Error # ");
	strcat(obuf, int_asc(errno));
#endif
	mlwrite(obuf);
	update(TRUE);
}
#else
VOID lckhello()	/* dummy function */
{
}
#endif

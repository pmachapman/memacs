/*	UNDO.C:		Undo commands and functionality for
 *			MicroEMACS
 *			(C)Copyright 1995 by Daniel Lawrence
 *
 *	The functions in this file record and allow the playback
 *	of basic editing changes. For each buffer, a stack of these
 *	changes is maintained. The beginning of each command which
 *	can change the buffer is flaged with a command entry. The
 *	undo command then can back out of these changes one command
 *	at a time. The UNDO stack is flushed by any command that
 *	writes the buffer to disk, or clears the buffer.
 */
 
#include	<stdio.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"

/* insert an editing operation at the top of the current buffer's
   undo stack. */

#if	PROTO
VOID undo_insert(OPTYPE op_type, long count, OBJECT op_erand)
#else
VOID undo_insert(op_type, count, op_erand)

OPTYPE op_type;		/* type of operation being recorded */
long count;		/* operand count */
OBJECT op_erand;	/* the operand of the operation */
#endif
{
	int undo_size;	/* size of the undo object */
	UNDO_OBJ *up;	/* ptr to a newly allocated undo object */

	/* don't bother if we are not keeping track! */
	if (undoflag == FALSE)
		return;

	/* or don't do it if we are admist undoing something else */
	if (undoing == TRUE)
		return;

	/* if it is a command object, and the last one pushed
	   was one as well, don't bother */
	if ((curbp->undo_head != (UNDO_OBJ *)NULL) &&
	    (op_type == OP_CMND) &&
	    (curbp->undo_head->type == OP_CMND))
		return;

	/* if it is a insert char, and the last one pushed
	   was one as well, just increment its count */
	if ((curbp->undo_head != (UNDO_OBJ *)NULL) &&
	    (op_type == OP_INSC) &&
	    (curbp->undo_head->type == OP_INSC)) {
	    	curbp->undo_head->count++;
		return;
	}

	/* get the correct size for this entry */
	undo_size = sizeof(UNDO_OBJ);
	if (op_type == OP_DSTR)
		undo_size += (int)count;

	/* and allocate the memory */
	up = (UNDO_OBJ *)malloc(undo_size);
	if (up == (UNDO_OBJ *)NULL)
		return;

	/* update the buffer undo count */
	curwp->w_bufp->undo_count++;

	/* record the buffer position and undo object type */
	up->line_num = getlinenum(curbp, curwp->w_dotp);
	up->offset = curwp->w_doto;
	up->type = op_type;
	up->count = count;

	/* save the actual object... */
	switch (op_type) {

		case OP_CMND:
		case OP_ISTR:
		case OP_CPOS:
			break;

		case OP_DSTR:
			strncpy(up->undo_obj.obj_string,
				op_erand.obj_sptr, (int)count);
			up->undo_obj.obj_string[(int)count] = 0;
			break;

		case OP_REPC:
		case OP_DELC:
		case OP_INSC:
			up->undo_obj.obj_char = op_erand.obj_char;
			break;
	}

	/* and add it to the head of the current buffer's undo list */
	up->next = curbp->undo_head;
	curbp->undo_head = up;
	return;
}

/* reverse the editing operation at the top of the current buffer's
   undo stack. */

#if	PROTO
int undo_op(void)
#else
int undo_op()
#endif
{
	OPTYPE op_type;		/* type of operation being recorded */
	UNDO_OBJ *up;		/* ptr to the undo object */
	int status;		/* return status from undone operation */
	long count;		/* repeat count for some operations */

	/* don't bother if we are not keeping track! */
	if (undoflag == FALSE)
		return(FALSE);

	/* make sure we flag we are doing an undo.... [avoid recursion!] */
	undoing = TRUE;

	/* grab the operation to undo */
	up = curwp->w_bufp->undo_head;

	/* restore the buffer position */
	gotoline(TRUE, up->line_num);
	curwp->w_doto = up->offset;
	op_type = up->type;

	/* undo the actual operation */
	status = FALSE;
	switch (op_type) {

		case OP_CMND:
		case OP_CPOS:
			status = TRUE;
			break;

		case OP_REPC:
			ldelete(1L, FALSE);
			/* fall into the next case! */

		case OP_DELC:
			if (up->undo_obj.obj_char == '\r') {
				count = up->count;
				while (count--)
					status = lnewline();
			} else
				status = linsert(up->count, up->undo_obj.obj_char);
			break;

		case OP_DSTR:
			status = linstr(up->undo_obj.obj_string);
			break;

		case OP_INSC:
		case OP_ISTR:
			status = ldelete(up->count, FALSE);
			break;
	}

	/* update the buffer undo count */
	curwp->w_bufp->undo_count--;

	/* and discard the undo entry */
	curwp->w_bufp->undo_head = up->next;
	free((char *)up);
	undoing = FALSE;
	return(status);
}

/* clear and deallocate the contents of a buffer's undo stack */

VOID undo_zot(bp)

BUFFER *bp;

{
	UNDO_OBJ *up;	/* current undo object being cleared */
	UNDO_OBJ *np;	/* next undo object to be cleared */

	bp->undo_count = 0L;
	np = bp->undo_head;
	while (np != (UNDO_OBJ *)NULL) {

		/* advance to the next undo object */
		up = np;
		np = up->next;

		/* and clear it */
		free((char *)up);
	}

	/* and tell the buffer it's gone */
	bp->undo_head = (UNDO_OBJ *)NULL;
}

/* Undo last done command */

int PASCAL NEAR undo(f, n)

int f,n;	/* prefix flag and argument */

{
	int status;	/* status return from undone operations */
	UNDO_OBJ *up;	/* ptr to the undo object */

	/* handle the default argument */
	if (f == FALSE)
		n = 1;

	/* non-positive arguments loose */
	if (n < 1)
		return(FALSE);

	/* repeat a single undo for n iterations */
	status = TRUE;
	up = curwp->w_bufp->undo_head;
	while ((status == TRUE) && (n > 0) && up != (UNDO_OBJ *)NULL) {

		/* undo individual operations until the next statement boundry */
		do {
			status = undo_op();
			up = curwp->w_bufp->undo_head;
		} while ((status == TRUE) &&
			 (up != (UNDO_OBJ *)NULL) &&
			 (up->type != OP_CMND));

		/* one less rep to do... */
		n--;
	}
	return(status);
}

/* delete current buffer's undo stack */

int PASCAL NEAR undo_delete(f, n)

int f,n;	/* prefix flag and argument */

{
	/* well, do that..... and we know it will work.... */
	undo_zot(curbp);
	return(TRUE);   
}

/* pop up a list of the current buffer's undo stack */

int PASCAL NEAR undo_list(f, n)

int f,n;	/* prefix flag and argument */

{
	register int status;	/* stutus return */

	if ((status = undolist()) != TRUE)
		return(status);
	return(wpopup(ulistp));
}

PASCAL NEAR undolist()

{
	register char *cp1;	/* scanning pointer into line to build */
	register char *cp2;
	long count;		/* number of remaining undo elements in stack */
	UNDO_OBJ *up;		/* current undo object being listed */
	char b[8];		/* place to build ascii version of longs */
	char line[128];		/* text buffer to hold current line */
	int status;		/* return status from subcommands */

	/* prepare and clear the buffer holding the undo list */
	ulistp->b_flag &= ~BFCHG;		/* Don't complain!	*/
	if ((status = bclear(ulistp)) != TRUE) 	/* Blow old text away	*/
		return(status);
	strcpy(ulistp->b_fname, "");

	/* add in the header text */
	if (addline(ulistp,
	    "           Line/Pos  REP   Type  Data"
	    ) == FALSE
	|| addline(ulistp, "           --------  ---   ----  ----") == FALSE)
		return(FALSE);

	/* scan through the undo stack, starting at the top! */
	up    = curwp->w_bufp->undo_head;
	count = curwp->w_bufp->undo_count;
	while (up != NULL) {

		/* Starting at the beginning of the line */
		cp1 = &line[0];

		/* add in the undo stack ordinal number */
		flong_asc(b, 7, count);
			cp2 = &b[0];
		while (*cp2)
			*cp1++ = *cp2++;
		*cp1++ = ' ';

		/* and then the line number */
		flong_asc(b, 7, up->line_num);
		cp2 = &b[0];
		while (*cp2)
			*cp1++ = *cp2++;
		*cp1++ = '/';

		/* and the offset into the line */
		strcpy(b, int_asc(up->offset));
		while (strlen(b) < 6)
			strcat(b, " ");
		cp2 = &b[0];
		while (*cp2)
			*cp1++ = *cp2++;
		*cp1++ = ' ';

		/* and the count */
		strcpy(b, int_asc(up->count));
		while (strlen(b) < 3)
			strcat(b, " ");
		cp2 = &b[0];
		while (*cp2)
			*cp1++ = *cp2++;
		*cp1++ = ' ';			/* Gap. 		*/

		/* next, the undo type */
		switch (up->type) {
	
			case OP_CMND:
				strcpy(cp1, "CMND  ");
				break;		
	
			case OP_CPOS:
				strcpy(cp1, "CPOS  ");
				break;		
	
			case OP_DELC:
				strcpy(cp1, "DELC  ");
				cmdstr(up->undo_obj.obj_char, cp1 + 6);
				break;

			case OP_DSTR:
				strcpy(cp1, "DSTR  ");
				strcat(cp1, "\"");
				strncat(cp1, up->undo_obj.obj_string, 40);
				cp1[26] = '+';
				cp1[27] = 0;
				strcat(cp1, "\"");
				break;

			case OP_INSC:
				strcpy(cp1, "INSC  ");
				cmdstr(up->undo_obj.obj_char, cp1 + 6);
				break;

			case OP_ISTR:
				strcpy(cp1, "ISTR  ");
				break;		

			case OP_REPC:
				strcpy(cp1, "REPC  ");
				cmdstr(up->undo_obj.obj_char, cp1 + 6);
				break;		
		}

		/* terminate and add the built line into the buffer */
		if (addline(ulistp, line) == FALSE)
			return(FALSE);

		/* on to the next undo! */
		count--;
		up = up->next;
	}
	return(TRUE);
}

/* clear ALL the undo stacks */

#if	PROTO
VOID undo_dump(void)
#else
VOID undo_dump()
#endif
{
	BUFFER *bp;

	/* clear all the buffer's undo stacks */
	bp = bheadp;
	while (bp) {
		undo_zot(bp);
		bp = bp->b_bufp;
	}
}

/*	ROOM:	Allocate memory using malloc()
		on failure, discard oldest undo information
		and retry
*/

#if	PROTO
VOID *room(int nbytes)
#else
VOID *room(nbytes)

int nbytes;	/* number of bytes to malloc() */
#endif
{
	void *ptr;	/* temporary pointer */
	BUFFER *bp;	/* buffer to dealloc memory from */
	UNDO_OBJ *up;	/* ptr to undo struct to free */
	UNDO_OBJ *lp;	/* last undo struct before up */

	ptr = (char *)NULL;
	while (ptr == (char *)NULL) {

		/* attempt to allocate the memory */
		ptr = malloc(nbytes);
		if (ptr != (char *)NULL)
			return(ptr);

		/* find the oldest visited buffer */
nextbuf:	bp = getoldb();

		/* no buffers left to check? */
		if (bp == (BUFFER *)NULL)
			return((char *)NULL);

		/* any undo info to discard? */
		if (bp->undo_count == 0) {
			bp->last_access = 0;
			goto nextbuf;
		}

		/* dump the last undo structure */
		lp = (UNDO_OBJ *)NULL;
		up = bp->undo_head;
		while (up->next != (UNDO_OBJ *)NULL) {
			lp = up;
			up = up->next;
		}

		/* dump the oldest undo */
		free((char *)up);
		lp->next = (UNDO_OBJ *)NULL;
		bp->undo_count--;
	}
}

/*	RE-ROOM: Allocate memory using realloc()
		on failure, discard oldest undo information
		and retry
*/

#if	PROTO
VOID *reroom(void *orig_ptr, int nbytes)
#else
VOID *reroom(orig_ptr, nbytes)

int nbytes;	/* number of bytes to malloc() */
void *orig_ptr;
#endif
{
	void *ptr;	/* temporary pointer */
	BUFFER *bp;	/* buffer to dealloc memory from */
	UNDO_OBJ *up;	/* ptr to undo struct to free */
	UNDO_OBJ *lp;	/* last undo struct before up */

	/*
	 * Avoid the whole problem of non-ANSI realloc() functions
	 * that don't handle NULL pointers correctly by calling
	 * malloc() (by way of room()) directly if orig_ptr is NULL.
	 */
	if (orig_ptr == NULL)
		return(room(nbytes));

	ptr = (char *)NULL;
	while (ptr == (char *)NULL) {

		/* attempt to allocate the memory */
		ptr = realloc(orig_ptr, nbytes);
		if (ptr != (char *)NULL)
			return(ptr);

		/* find the oldest visited buffer */
nxtbuf:		bp = getoldb();

		/* no buffers left to check? */
		if (bp == (BUFFER *)NULL)
			return((char *)NULL);

		/* any undo info to discard? */
		if (bp->undo_count == 0) {
			bp->last_access = 0;
			goto nxtbuf;
		}

		/* dump the last undo structure */
		lp = (UNDO_OBJ *)NULL;
		up = bp->undo_head;
		while (up->next != (UNDO_OBJ *)NULL) {
			lp = up;
			up = up->next;
		}

		/* dump the oldest undo */
		free((char *)up);
		lp->next = (UNDO_OBJ *)NULL;
		bp->undo_count--;
	}
}


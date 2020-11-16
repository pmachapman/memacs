/*      AMIGADOS.C:     Operating specific I/O and Spawning functions
   for MicroEMACS 3.12
   (C)Copyright 1993 by Daniel M. Lawrence
   Significant changes for GCC (c) 1996 Ruth Ivimey-Cook
 */

#include        <stdio.h>

#ifdef DEBUG
#include <stdarg.h>

static FILE *debugfile = 0;

void _dprintf(char *str, ...)
{
	va_list ap;

	va_start(ap, str);
	if (debug)
	{
		if (debugfile == 0)
			debugfile = fopen(DEBUGFILE, "w");
		
		vfprintf(debugfile, str, ap);
		fflush(debugfile);
	}
	va_end(ap);
}

#endif


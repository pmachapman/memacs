/*      AMIGADOS.C:     Operating specific I/O and Spawning functions
   for MicroEMACS 3.12
   (C)Copyright 1993 by Daniel M. Lawrence
   Significant changes for GCC (c) 1996 Ruth Ivimey-Cook
 */

#ifndef _EDEBUG_H
#define _EDEBUG_H

#ifdef DEBUG
#define dbprintf(stuff)			_dprintf stuff
#define dbprintf_note(stuff)	_dprintf stuff ; printf stuff
#else
#define dbprintf(stuff)
#define dbprintf_note(stuff)
#endif

#endif


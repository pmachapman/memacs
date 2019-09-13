#ifndef ESTRUCT_H_INCLUDED
#define ESTRUCT_H_INCLUDED

#ifdef BUILD_MSWIN
#include "mswin/estruct.h"
#else
#ifdef BUILD_NTCONSOLE
#include "nt/estruct.h"
#elif defined(BUILD_XPCONSOLE)
#include "xp/estruct.h"
#else
#include "estruct_orig.h"
#endif
#endif


#endif            

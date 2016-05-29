#ifndef __StringParsing_H__
#define __StringParsing_H__


//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions
		
char *String_Parsing ( char *szSource , char *szFrom , char *szTo , char **pNextChar );

char *StringReplaceAll( char *szSourceString , char *szFindWhat , char *szReplaceWith , int *pCount );

#endif  /* ndef __StringParsing_H__ */

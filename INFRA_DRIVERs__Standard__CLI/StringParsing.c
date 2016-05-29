//==============================================================================
// Include files
#include "Windows.h"
#include "toolbox.h"
#include <ansi_c.h>
#include "GlobalDefines.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions


char *String_Parsing ( char *szSource , char *szFrom , char *szTo , char **pNextChar )
{
	char		*pSearchFrom	=	NULL,
				*pSearchTo		=	NULL,
				*pSource		=	NULL,
				*pLocalNext		=	NULL,
				*pReturnResult	=	NULL;

	int			iOffSet			=	0,
				iLength			=	0;
	
	if ( pNextChar )										// pNextChar exists
		*pNextChar = NULL;
	
	if ( szSource == NULL )									// szSource not exists
		return NULL;
	
	pSource = calloc( strlen( szSource ) + 1 , 1 );
	
	if ( pSource == NULL )									// No memory
		return NULL;
	
	strcpy( pSource , szSource );							// source copy
	
	if ( strlen(pSource) == 0 )								// No memory
		return NULL;
	
	do
	{
		
		if (( szFrom == NULL ) || ( strlen(szFrom) == 0 ))	// szFrom from begin
			pSearchFrom = pSource;
	
		else												// find szFrom
		{
			pSearchFrom = strstr ( pSource, szFrom );
		
			if ( pSearchFrom == NULL )						// szFrom not found
				break;
		
			pSearchFrom += strlen (szFrom);					// pSearchFrom points after szFrom
		
			if ( pSearchFrom == NULL )						// szFrom is the last string
				break; 
		}
	
		if (( szTo == NULL ) || ( strlen(szTo) == 0 ))	// szTo to end
		{
			if ( strlen(pSearchFrom) )
				pSearchTo = pSearchFrom + strlen(pSearchFrom) - 1;
			else
				pSearchTo = pSource + strlen(pSource) - 1;  
		}
		else
		{
			pSearchTo = strstr ( pSearchFrom, szTo );
		
			if ( pSearchTo == NULL )						// szTo not found
				break;		
			
			pLocalNext = pSearchTo + strlen ( szTo );		// pointer to the next char after szTo
		
			if ( pSearchTo && pSource )
				iOffSet = pSearchTo - pSource;
			else
				iOffSet = 0;
			
			if ( pNextChar )								// if requared return next pointer
				*pNextChar = strstr( szSource + iOffSet , pLocalNext );
			
			if ( pSearchFrom == pSearchTo )					// o bytes between szFrom and szTo
				break;
		
			pSearchTo--;
		}
		
		while (( pSearchFrom ) && ( *pSearchFrom == ' '))	// delete spaces from begin 
		{
			if ( pSearchFrom == pSearchTo )					// only spaces
				break;
		
			pSearchFrom++;
		}
	
		while (( pSearchTo ) && ( *pSearchTo == ' ' )) 		// delete spases from end
		{
			if ( pSearchFrom == pSearchTo )					// only spaces
				break;
		
			pSearchTo--;
		}	
	
		if (( pSearchTo ) && ( pSearchFrom ))
		{
			pSearchTo++;									
			*pSearchTo = 0;									// "close" the result string
			
			iLength = strlen( pSearchFrom );
			
			pReturnResult = calloc( iLength + 1024 , 1 ); 
			
			if ( pReturnResult )
				memcpy( pReturnResult , pSearchFrom , iLength );
			
			break;			
		}
		
	} while(0);
	
	if ( pSource ) 				// memory free
		free ( pSource );
	
    return pReturnResult;
}



char		*StringReplaceAll( char *szSourceString , char *szFindWhat , char *szReplaceWith , int *pCount )
{
	STD_ERROR				StdError							=	{0};
	
	char				*pOutputString				=	NULL,
						*pInputString				=	NULL,
						*pPointer					=	NULL;
	
	int					count						=	0,
						index						=	0,
						iCountIndex					=	0,
						next_index					=	0,
						newLength					=	0,
						dynamicSize					=	0;
	
	int					*piLocationList				=	NULL,
						*piTemp						=	NULL;
	
	IF (( szSourceString == NULL ) , "Pointer can't be null.");
	IF (( szFindWhat == NULL ) , "Pointer can't be null.");
	IF (( szReplaceWith == NULL ) , "Pointer can't be null.");
	
	FREE_CALLOC_COPY_STRING_ERR( pInputString , szSourceString );
	dynamicSize = 1024;
	CALLOC_ERR( piLocationList , dynamicSize + 1 , sizeof(int));
	
	do 
	{
		pPointer = strstr( pInputString + index , szFindWhat ); 
		
		if ( pPointer )
		{
			index = pPointer - pInputString + strlen(szFindWhat);
			count++;
			*pPointer = 0;
			
			if ( count > dynamicSize )
			{
				dynamicSize *= 2;
				
				if ( dynamicSize < count)
					dynamicSize += count;
				
				CALLOC_COPY_ERR( piTemp , dynamicSize + 1 , sizeof(int) , piLocationList , count ); 
				FREE( piLocationList );
				piLocationList = piTemp;
			}
			
			piLocationList[count] = index;
		}
		
	}while(pPointer);
	
	if ( pCount )
		*pCount = count;
	
	newLength = strlen(szSourceString) + (( count + 1 ) * strlen(szReplaceWith));
	
	if ( count )
	{
		CALLOC_ERR( pOutputString , newLength , sizeof(char));
	
		for ( iCountIndex = 0 ; iCountIndex < count ; iCountIndex++ )
		{
			index = piLocationList[iCountIndex];
			next_index += sprintf ( pOutputString + next_index , "%s%s" , pInputString + index , szReplaceWith );
		}

		index = piLocationList[iCountIndex];
		sprintf ( pOutputString + next_index , "%s" , pInputString + index );   
	}
	
Error:
	
	if ( pOutputString == NULL )
	{
		CALLOC_COPY_STRING(pOutputString,szSourceString);
	}
	
	FREE(pInputString);
	FREE(piLocationList);
	
	return pOutputString;
}

/*============================================================================*/
/*                        L a b W i n d o w s / C V I                         */
/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 1987-2013.  All Rights Reserved.     */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Title:       inifile.h                                                     */
/* Purpose:     provides generic interface for reading and writing            */
/*              .ini style files.                                             */
/*                                                                            */
/*============================================================================*/


#ifndef INIFILE_HEADER
#define INIFILE_HEADER

#include "toolbox.h"

#ifdef __cplusplus
	extern "C" {
#endif


  /******************/
  /* Macros & Types */
  /******************/

typedef struct IniStructForwardReferenceTag *IniText;
typedef int (CVICALLBACK * Ini_OutputFunc)(void *outputDest, char *outputString);
typedef int (CVICALLBACK * Ini_InputFunc)(void *inputSource, char *inputBuffer, int maxLineLength, int *endOfSource, int *lineNumRead);
typedef int (CVICALLBACK * Ini_SectionFilterFunc)(IniText theIniText, void *callbackData, char *sectionName);

  /********************/
  /* Public Functions */
  /********************/

IniText CVIFUNC Ini_New (int sorted);
void    CVIFUNC Ini_Dispose (IniText theIniText);

int     CVIFUNC Ini_WriteToFile (IniText theIniText, const char pathName[]);
int     CVIFUNC Ini_ReadFromFile (IniText theIniText, const char pathName[]);

int     CVIFUNC Ini_WriteGeneric(IniText theIniText, Ini_OutputFunc outputFunc, void *outputDest);
int     CVIFUNC Ini_ReadGeneric(IniText theIniText, Ini_InputFunc inputFunc, void *inputSource);

#if _NI_mswin32_
int     CVIFUNC Ini_WriteToRegistry(IniText theIniText, int rootKey, const char *baseKeyName);
int     CVIFUNC Ini_ReadFromRegistry(IniText theIniText, int rootKey, const char *baseKeyName);
#endif /* _NI_mswin32_ */

int 	CVIFUNC Ini_SetAddQuotesToStrings(IniText iniText, int addQuotesToStrings);
int 	CVIFUNC Ini_SetMaxValueLineLength(IniText iniText, int maxValueLineLength);
int     CVIFUNC Ini_SetTokens(IniText theIniText, char sectionNameStartCharacter, char sectionNameEndCharacter, const char *valueSeparator, const char *commentMarker);
int     CVIFUNC Ini_SetSectionFilter(IniText theIniText, Ini_SectionFilterFunc sectionFilter, void *callbackData);
int     CVIFUNC Ini_SetDuplicateChecking(IniText iniText, int checkForDuplicates);
int     CVIFUNC Ini_CopySection(IniText source, const char sectionToCopy[], IniText destination, int overwriteDuplicateSections);
int		CVIFUNC Ini_CopySectionEx(IniText source, const char sectionToCopy[], IniText destination, const char destinationSectionToCopyTo[], int overwriteDuplicateSection);

int     CVIFUNC Ini_NumberOfSections (IniText theIniText);
int     CVIFUNC Ini_NthSectionName (IniText theIniText, int index, char **sectionName);
int     CVIFUNC Ini_SectionExists (IniText theIniText, const char *sectionName);
int     CVIFUNC Ini_NumberOfItems (IniText theIniText, const char *sectionName);
int     CVIFUNC Ini_NthItemName (IniText theIniText, const char *sectionName, int index,
								 char **itemName);
int     CVIFUNC Ini_ItemExists (IniText theIniText, const char *section, const char *itemName);

		/* The following function is obsolete.  Use Ini_ItemExists() instead. */
int     CVIFUNC Ini_ItemOrMultiLineItemExists (IniText theIniText, const char *section,
											   const char *itemName);

void    CVIFUNC Ini_Sort (IniText theIniText);
void    CVIFUNC Ini_DisableSorting (IniText theIniText);
int     CVIFUNC Ini_SortInternally(IniText theIniText);
void    CVIFUNC Ini_DisableInternalSorting (IniText theIniText);
int     CVIFUNC Ini_LineOfLastAccess (IniText theIniText);
int     CVIFUNC Ini_RemoveSection(IniText theIniText, const char *sectionName);
int     CVIFUNC Ini_RemoveItem(IniText theIniText, const char *sectionName, const char *itemName);


int     CVIFUNC Ini_PutString (IniText theIniText, const char *section, const char *itemName,
							   const char *value);
int     CVIFUNC Ini_PutRawString (IniText theIniText, const char *section, const char *itemName,
								  const char *value);
int     CVIFUNC Ini_PutInt (IniText dest, const char *section, const char *itemName, int value);
int     CVIFUNC Ini_PutUInt (IniText dest, const char *section, const char *itemName,
							 unsigned int value);
int     CVIFUNC Ini_PutDouble (IniText dest, const char *section, const char *itemName,
							   double value);
int     CVIFUNC Ini_PutBoolean (IniText dest, const char *section, const char *itemName,
								int boolValue);
int     CVIFUNC Ini_PutData(IniText iniText, const char *section, const char *itemName, const unsigned char *data, long dataSize);

int     CVIFUNC Ini_GetPointerToString (IniText theIniText, const char *section, const char *itemName,
										char **value);
int     CVIFUNC Ini_GetStringCopy (IniText theIniText, const char *section, const char *itemName,
								   char **value);
int     CVIFUNC Ini_GetStringIntoBuffer (IniText theIniText, const char *section, const char *itemName,
										 char buffer[], size_t bufSize);
int     CVIFUNC Ini_GetPointerToRawString (IniText theIniText, const char *section, const char *itemName,
										   char **value);
int     CVIFUNC Ini_GetRawStringCopy (IniText theIniText, const char *section, const char *itemName,
									  char **value);
int     CVIFUNC Ini_GetRawStringIntoBuffer (IniText theIniText, const char *section, const char *itemName,
											char buffer[], size_t bufSize);
int     CVIFUNC Ini_GetInt (IniText src, const char *section, const char *itemName, int *value);
int     CVIFUNC Ini_GetInt64 (IniText src, const char *section, const char *itemName, __int64 *value);
int     CVIFUNC Ini_GetUInt (IniText src, const char *section, const char *itemName,
							 unsigned int *value);
int     CVIFUNC Ini_GetUInt64 (IniText src, const char *section, const char *itemName,
							 unsigned __int64 *value);
int     CVIFUNC Ini_GetDouble (IniText src, const char *section, const char *itemName, double *value);
int     CVIFUNC Ini_GetBoolean (IniText src, const char *section, const char *itemName,
								int *boolValue);
int     CVIFUNC Ini_GetData(IniText iniText, const char *section, const char *itemName, unsigned char **data, size_t *dataSize);


				/**********************/
				/* LineFile Functions */
				/**********************/

typedef struct LineFileForwardReferenceTag *LineFile;
typedef int (CVICALLBACK * LineFileReadFunc)(void *inputSource, int numBytesToRead, int *numBytesRead, void *dest);   /* numBytesRead == 0 if at end of input */

LineFile    CVIFUNC LineFile_New (LineFileReadFunc inputFunc, void *inputSource);
void        CVIFUNC LineFile_Dispose (LineFile lineFile);
int         CVIFUNC LineFile_Read (LineFile lineFile, char *destBuf, size_t maxNumBytes,
								   ssize_t *bytesRead, int *endOfFile);
int         CVIFUNC LineFile_ReadNonEmpty (LineFile lineFile, char *destBuf,
										   size_t maxNumBytes, ssize_t *bytesRead,
										   int *endOfFile);
int         CVIFUNC LineFile_NumLinesRead (LineFile lineFile);
int         CVIFUNC LineFile_OffsetOfLastLine (LineFile lineFile);
int         CVIFUNC LineFile_NumExtraLineTermChars (LineFile lineFile);
int         CVIFUNC LineFile_NumCharsInEmptyLines (LineFile lineFile);
int         CVICALLBACK LineFile_ReadFromFile(void *inputSource, int numBytesToRead, int *numBytesRead, void *dest);



#ifdef __cplusplus
	}
#endif

#endif /* INIFILE_HEADER */



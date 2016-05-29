/*============================================================================*/
/*                        L a b W i n d o w s / C V I                         */
/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 1987-2013.  All Rights Reserved.     */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Title:       inifile.c                                                     */
/* Purpose:     provides functions to read and write .ini style files         */
/*                                                                            */
/*============================================================================*/


#include <userint.h>
#ifdef _NI_mswin_
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <ansi_c.h>
#include <winreg.h>
#endif /* _NI_mswin_ */
#include <utility.h>
#include "toolbox.h"
#include "inifile.h"
#include <errno.h>
#include <limits.h>

  /******************/
  /* Macros & Types */
  /******************/

#define FastTokenCmp(string, tokenString) ((*(string) != *(tokenString)) ? (*(string) - *(tokenString)) : CmbStrNCmp((string), (tokenString), strlen((char *)(tokenString))))
#define kMaxSpecialTokenLength          8           /* way longer than reasonably needed */

#define kMaxSectionOrTagNameSize		255
#define kMaxIniLineSize                 1024   /* long lines should be split into multi-line entries */

#define kLineTag                        " Line"
#define kDefaultMaxValueLineLength      80

#define kStringCacheChunkSize           1024 /* the cache size is always the smallest multiple of this value that will hold the requested number of bytes */

#define kIniWriteBufSize                16384

typedef enum
	{
	kIni_CommentLine,
	kIni_SectionLine,
	kIni_TagAndValueLine,
	kIni_NotAnIniLine
	} IniLineType;


typedef struct _iniEntryStruct
	{
	char    *name;
	char    *value;
	int     sourceLine;
	int     addMarkerQuotes;    /* add quotes when writing to file, because item was put as a string */
	char	*comment;
	} *IniEntry;


typedef struct
	{
	char        *sectionName;
	ListType    entryList;
	ListType    internallySortedEntryList;
	char        sorted;
	int         sourceLine;
	char		*comment;
	} *IniSection;


struct IniStructForwardReferenceTag /* needed to allow declaring IniText in the header file */
	{
	ListType                sectionList;
	ListType                internallySortedList;
	IniSection              lastSectionAccessed;  /* optimization */
	char                    sorted;
	int                     lastLineAccessed;
	int                     nameValueInputSeparatorLength;
	char                    nameValueInputSeparatorToken[kMaxSpecialTokenLength];
	int                     nameValueOutputSeparatorLength;
	char                    nameValueOutputSeparatorToken[kMaxSpecialTokenLength + 2];
	char                    sectionNameStartChar;
	char                    sectionNameEndChar;
	char                    commentMarker[kMaxSpecialTokenLength];
	int                     checkForDuplicatesWhenPutting;
	Ini_SectionFilterFunc   sectionFilter;
	void                    *sectionFilterCallbackData;
	char                    **stringCache;
	char					*topComment;
	char					*bottomComment;
	int						addQuotesToStrings;
	int						maxValueLineLength;
	};

	/*******************************/
	/* Static Functions Prototypes */
	/*******************************/

static int          Ini_FindSectionIndex(IniText theIniText, const char *sectionName);
static IniSection   Ini_FindSection(IniText theIniText, const char *sectionName);
static int          Ini_InsertSection(IniText theIniText, IniSection section);
static void         Ini_SetDefaultTokens(IniText iniText);


/*  IniSection members */
static IniSection   Ini_NewSection(const char *sectionName, int sorted);
static void         Ini_DisposeSection(IniSection section);
static char         *Ini_SectionName(IniSection section);
static int          CVICALLBACK Ini_CompareSectionNames(void *ptrToSection1, void *ptrToSection2);
static int          CVICALLBACK Ini_CompareSectionToName(void *sectionName, void *ptrToSection);
static IniEntry     Ini_FindEntry(IniSection section, const char *itemName);
static int          Ini_InsertEntry(IniSection section, IniEntry entry, int index);
static int          Ini_WriteSection(IniText iniText, IniSection theSection, Ini_OutputFunc outputFunc, void *outputDest);
static int          Ini_RemoveFromSection(IniSection section, const char *itemName, int *indexRef);


/*  IniEntry members */
static IniEntry     Ini_NewEntry(const char *name, const char *value, int stripMarkerQuotes);
static void         Ini_DisposeEntry(IniEntry theEntry);
static char         *Ini_EntryName(IniEntry entry);
static int          CVICALLBACK Ini_CompareEntryNames(void *ptrToEntry1, void *ptrToEntry2);
static int          CVICALLBACK Ini_CompareEntryToName(void *entryName, void *ptrToEntry);
static int          Ini_WriteEntry(IniText iniText, IniEntry theEntry, Ini_OutputFunc outputFunc, void *outputDest);
static int          Ini_SetEntryValue(IniEntry entry, const char *value, int stripMarkerQuotes);


/*  Helper functions */
static void         Ini_DisposeSectionList(ListType sectionList);
static void         Ini_DisposeEntryList(ListType entryList);
static int          ParseTagTextSectionTitle(IniText iniText, const char *firstCharAfterOpenBracket, char *sectionTitle);
static int          ParseTagTextLine(IniText iniText, const char buf[], char *section, char *item, char *value);
static int          CVICALLBACK Ini_LineFileInputFunction(void *inputSource, char *inputBuffer, int maxLineLength, int *endOfSource, int *lineNumRead);
static int          CVICALLBACK Ini_FileOutputFunc(void *file, char *outputString);
static int          Ini_GetErrCodeFromWriteErrno(int errnoVal, int forceError);
static int          SetStringCacheSize(char **strHandle, int size);
static int          Ini_PutSingleLineString(IniText theIniText, const char *section, const char *itemName, const char *value);
static int          CoalesceMultilineItemsInSection(IniSection  section);
static int          CoalesceMultiLineItems(IniSection section, int startIndex, int endIndex);
static int          FindMultiLineItem(IniSection section, int startOfSearchIndex, int *startIndex, int *endIndex);
static int          CoalesceMultilineItemsAfterRead(IniText iniText);

static int          Ini_Put(IniText theIniText, const char *section, const char *itemName, const char *value, int addMarkerQuotes);
static int          Ini_Get(IniText theIniText, const char *section, const char *itemName, char **value);
static void         Ini_AnalyzeItemName(const char *itemName, int *isMultiLine, int *whichLine, char singleLineItemName[]);

static int          EscapeText(const char *srcTextPtr, char **destTextPtr);
static int          UnescapeText(const char *srcTextPtr, char *destTextPtr);

static int          GetHexCharValue (int hexChar);

static char *       StrDupWithoutQuotes(const char *string, int *hadQuotes);

// Disable warnings in clang.
#if defined (__clang__)
#pragma clang diagnostic ignored "-Wunused-variable"
#endif // defined __clang__

	/****************************/
	/* Resource tracking        */
	/****************************/

#if HAVE_CVI_RTE
#ifndef _WIN64
#define POINTER		"0x%08p"
#else
#define POINTER		"0x%016p"
#endif

extern void * CVIFUNC_C	__CVI_Resource_Acquire (void *resource,
							char *type, char *description, ...);
extern void * CVIFUNC	__CVI_Resource_Release (void *resource);

static char *INIFILE_RESOURCE_TYPE		= "Files";
static char *INIFILE_FORMAT_FILE		= "INI file "POINTER;
static char *INIFILE_FORMAT_LINEFILE	= "Line file "POINTER;
#endif /* HAVE_CVI_RTE */

/*********************************************************************/

	/****************************/
	/* Function Implementations */
	/****************************/

IniText CVIFUNC Ini_New(int sorted)
{
	IniText newIniText = NULL;
#if defined(_NI_mswin_) && _NI_mswin_
	int error = UIENoError,tracking = SetEnableResourceTracking (0);
#else
	int error = UIENoError;
#endif

	nullChk(newIniText = calloc(sizeof(*newIniText), 1));
	nullChk(newIniText->sectionList = ListCreate(sizeof(IniSection)));
	newIniText->internallySortedList = NULL;
	newIniText->sorted = sorted;
	newIniText->lastLineAccessed = -1;
	newIniText->lastSectionAccessed = NULL;
	newIniText->sectionFilter = NULL;
	newIniText->addQuotesToStrings = 1;
	newIniText->maxValueLineLength = kDefaultMaxValueLineLength;
	Ini_SetDefaultTokens(newIniText);

	newIniText->checkForDuplicatesWhenPutting = TRUE;
	nullChk(newIniText->stringCache = (char **)NewHandle(0));

Error:
#if defined(_NI_mswin_) && _NI_mswin_
	SetEnableResourceTracking (tracking);
#endif
	if (error < 0)
		{
		Ini_Dispose(newIniText);
		newIniText = NULL;
		}
#if HAVE_CVI_RTE && defined (_NI_mswin_) && _NI_mswin_
	else
		__CVI_Resource_Acquire (newIniText, INIFILE_RESOURCE_TYPE,
			INIFILE_FORMAT_FILE, newIniText);
#endif
	return newIniText;
}

/*********************************************************************/

void    CVIFUNC Ini_Dispose(IniText iniText)
{
	if (iniText)
		{
		Ini_DisposeSectionList(iniText->sectionList);
		ListDispose(iniText->internallySortedList);
		DisposeHandle((Handle)iniText->stringCache);
		if (iniText->topComment)
			free(iniText->topComment);
		if (iniText->bottomComment)
			free(iniText->bottomComment);
#if HAVE_CVI_RTE && defined (_NI_mswin_) && _NI_mswin_
		__CVI_Resource_Release (iniText);
#endif
		free(iniText);
		}
}

/*********************************************************************/

int CVIFUNC Ini_SectionExists(IniText iniText, const char *sectionName)
{
	return Ini_FindSection(iniText, sectionName) != NULL;
}

/*********************************************************************/

	/* returns 0 if the section doesn't exist */
int  CVIFUNC Ini_NumberOfItems(IniText theIniText, const char *sectionName)
{
	IniSection  section;

	section = Ini_FindSection(theIniText, sectionName);

	if (section)
		return (int)ListNumItems(section->entryList);
	else
		return 0;
}

/*********************************************************************/

	/* returns FALSE if the section or item doesn't exist */
int  CVIFUNC Ini_NthItemName(IniText theIniText, const char *sectionName, int index, char **itemName)
{
	IniSection  section;
	IniEntry    entry = NULL;
	int         numItems;
	int         result = FALSE;

	if( !itemName )
		return UIENullPointerPassed;

	section = Ini_FindSection(theIniText, sectionName);

	if (section)
		{
		numItems = (int)ListNumItems(section->entryList);
		if (index > 0 && index <= numItems)
			{
			ListGetItem(section->entryList, &entry, index);
			*itemName = entry->name;
			result = TRUE;
			}
		}

	return result;
}

/*********************************************************************/

int  CVIFUNC Ini_NumberOfSections(IniText theIniText)
{
	return theIniText ? (int)ListNumItems(theIniText->sectionList) : UIENullPointerPassed;
}

/*********************************************************************/

int CVIFUNC Ini_NthSectionName(IniText theIniText, int index, char **sectionName)
{
	IniSection  section;
	int     numItems;
	int     result = FALSE;

	if( !theIniText || !sectionName )
		return UIENullPointerPassed;

	numItems = (int)ListNumItems(theIniText->sectionList);
	if (index > 0 && index <= numItems)
		{
		ListGetItem(theIniText->sectionList, &section, index);
		*sectionName = section->sectionName;
		result = TRUE;
		}

	return result;
}

/*********************************************************************/

int CVIFUNC Ini_LineOfLastAccess(IniText theIniText)
{
	return theIniText ? theIniText->lastLineAccessed : UIENullPointerPassed;
}

/*********************************************************************/

void    CVIFUNC Ini_Sort(IniText theIniText)
{
	int index;
	int numSections;

	if( !theIniText )
		return;

	numSections = (int)ListNumItems(theIniText->sectionList);

	for (index = 1; index <= numSections; index++)
		{
		IniSection  currentSection;

		ListGetItem(theIniText->sectionList, &currentSection, index);
		ListQuickSort(currentSection->entryList, Ini_CompareEntryNames);
		currentSection->sorted = TRUE;
		}

	ListQuickSort(theIniText->sectionList, Ini_CompareSectionNames);
	theIniText->sorted = TRUE;
}

/*********************************************************************/

int  CVIFUNC Ini_SortInternally(IniText theIniText)
{
	int error = UIENoError;
	int index;
	int numSections;

	if( !theIniText )
		return UIENullPointerPassed;

	ListDispose(theIniText->internallySortedList);
	nullChk( theIniText->internallySortedList = ListCopy(theIniText->sectionList));

	ListQuickSort(theIniText->internallySortedList, Ini_CompareSectionNames);

	numSections = (int)ListNumItems(theIniText->internallySortedList);

	for (index = 1; index <= numSections; index++)
		{
		IniSection  currentSection;

		ListGetItem(theIniText->internallySortedList, &currentSection, index);

		ListDispose(currentSection->internallySortedEntryList);
		nullChk( currentSection->internallySortedEntryList = ListCopy(currentSection->entryList));
		ListQuickSort(currentSection->internallySortedEntryList, Ini_CompareEntryNames);
		}

Error:
	if (error < 0)
		Ini_DisableInternalSorting(theIniText);

	return error;
}

/*********************************************************************/

void    CVIFUNC Ini_DisableInternalSorting (IniText theIniText)
{
	int index;
	int numSections;

	if( !theIniText )
		return;

	ListDispose(theIniText->internallySortedList);
	theIniText->internallySortedList = NULL;

	numSections = (int)ListNumItems(theIniText->sectionList);

	for (index = 1; index <= numSections; index++)
		{
		IniSection  currentSection;

		ListGetItem(theIniText->sectionList, &currentSection, index);
		ListDispose(currentSection->internallySortedEntryList);
		currentSection->internallySortedEntryList = NULL;
		}
}

/*********************************************************************/

void    CVIFUNC Ini_DisableSorting (IniText theIniText)
{
	int index;
	int numSections;

	if( !theIniText )
		return;

	numSections = (int)ListNumItems(theIniText->sectionList);

	for (index = 1; index <= numSections; index++)
		{
		IniSection  currentSection;

		ListGetItem(theIniText->sectionList, &currentSection, index);
		currentSection->sorted = FALSE;
		}
	theIniText->sorted = FALSE;
}

/*********************************************************************/

static void Ini_DisposeSectionList(ListType sectionList)
{
	size_t index;

	if (sectionList)
		{
		for (index = 1; index <= ListNumItems(sectionList); index++)
			Ini_DisposeSection(*(IniSection *)ListGetPtrToItem(sectionList, index));

		ListDispose(sectionList);
		}
}

/*********************************************************************/

	/*  After an section is inserted into a IniText, the IniText owns it,
		is responsible for disposing it, and should be the only one to access it.
	*/
static int Ini_InsertSection(IniText theIniText, IniSection section)
{
	if( !theIniText )
		return UIENullPointerPassed;

	if (theIniText->internallySortedList)
		if (!ListInsertInOrder(theIniText->internallySortedList, &section, Ini_CompareSectionNames))
			{  /* if memory allocation failure, abandon the optimization */
			ListDispose(theIniText->internallySortedList);
			theIniText->internallySortedList = NULL;
			}

	if (theIniText->sorted)
		return ListInsertInOrder(theIniText->sectionList, &section, Ini_CompareSectionNames);
	else
		return ListInsertItem(theIniText->sectionList, &section, END_OF_LIST);
}

/*********************************************************************/

static IniSection Ini_NewSection(const char *sectionName, int sorted)
{
	IniSection  newSection = NULL;
#if defined(_NI_mswin_) && _NI_mswin_
	int     error = UIENoError, tracking = SetEnableResourceTracking (0);
#else
	int     error = UIENoError;
#endif

	nullChk(newSection = calloc(sizeof(*newSection), 1));
	nullChk(newSection->entryList = ListCreate(sizeof(IniEntry)));
	newSection->internallySortedEntryList = NULL;
	nullChk(newSection->sectionName = StrDupWithoutSurrWhiteSpace((char *)sectionName));
	newSection->sorted = sorted;

Error:
#if defined(_NI_mswin_) && _NI_mswin_
	SetEnableResourceTracking (tracking);
#endif
	if (error < 0)
		{
		Ini_DisposeSection(newSection);
		newSection = NULL;
		}

	return newSection;
}

/*********************************************************************/

static void Ini_DisposeSection(IniSection section)
{
	if (section)
		{
		Ini_DisposeEntryList(section->entryList);
		ListDispose(section->internallySortedEntryList);
		free(section->sectionName);
		if (section->comment)
			free(section->comment);
		free(section);
		}
}

/*********************************************************************/

	/* Returns the index into the sectionList, the internallySortedList is not looked at */
static int Ini_FindSectionIndex(IniText theIniText, const char *sectionName)
{
	int index = 0;

	if (theIniText)
		{
		if (theIniText->sorted)
			index = (int)ListBinSearch(theIniText->sectionList, (void *)sectionName, Ini_CompareSectionToName);
		else
			index = (int)ListFindItem(theIniText->sectionList, (void *)sectionName, FRONT_OF_LIST, Ini_CompareSectionToName);
		}

	return index;
}

/*********************************************************************/

int CVIFUNC Ini_RemoveSection(IniText theIniText, const char *sectionName)
{
	int         index;
	IniSection  section;
	int         removed = FALSE;

	if( !theIniText )
		return removed;

	if (theIniText->internallySortedList)
		{
		index = (int)ListBinSearch(theIniText->internallySortedList, (void *)sectionName, Ini_CompareSectionToName);
		if (index != 0)
			ListRemoveItem(theIniText->internallySortedList, NULL, index);
		}

	index = Ini_FindSectionIndex(theIniText, sectionName);

	if (index != 0)
		{
		ListRemoveItem(theIniText->sectionList, &section, index);
		if (theIniText->lastSectionAccessed == section)
			theIniText->lastSectionAccessed = NULL;

		Ini_DisposeSection(section);
		removed = TRUE;
		}

	return removed;
}

/*********************************************************************/

static void Ini_DisposeEntryList(ListType entryList)
{
	size_t index;

	if (entryList)
		{
		for (index = 1; index <= ListNumItems(entryList); index++)
			Ini_DisposeEntry(*(IniEntry *)ListGetPtrToItem(entryList, index));

		ListDispose(entryList);
		}
}

/*********************************************************************/

static char *Ini_SectionName(IniSection section)
{
	return section ? section->sectionName : NULL;
}

/*********************************************************************/

static int CVICALLBACK Ini_CompareSectionToName(void *sectionName, void *ptrToSection)
{
	return StrICmpWithoutSurrWhiteSpace((char *)sectionName, Ini_SectionName(*(IniSection*)ptrToSection));
}

/*********************************************************************/

static int CVICALLBACK Ini_CompareSectionNames(void *ptrToSection1, void *ptrToSection2)
{
	return StrICmp(Ini_SectionName(*(IniSection*)ptrToSection1), Ini_SectionName(*(IniSection*)ptrToSection2));
}

/*********************************************************************/

	/*
	Creates a dynamically allocated copy of a string while stripping
	off matching quotes if present.  Returns whether quotes were present.
	*/
static char * StrDupWithoutQuotes(const char *string, int *hadQuotes)
{
	char        *copy;
	int         len;

	if (hadQuotes)
		*hadQuotes = FALSE;

	len = (int)strlen(string);

		/* look for and remove matching quotes */
	if ((len > 1) && (string[0] == '\"') && CmbStrByteIs((unsigned char*)string, len - 1, '\"'))
		{
		string++;
		len -= 2;
		if (hadQuotes)
			*hadQuotes = TRUE;
		}

	copy = malloc (len+1);
	if (copy)
		{
		memcpy (copy, string, len);
		copy[len] = '\0';
		}

	return copy;
}

/*********************************************************************/

static int Ini_SetEntryValue(IniEntry entry, const char *value, int stripMarkerQuotes)
{
	int     error = UIENoError;
	char    *newValue = 0;
	int     hadMarkerQuotes;

	if( !entry )
		return UIENullPointerPassed;

	if (stripMarkerQuotes)
		{
		nullChk(newValue = StrDupWithoutQuotes(value, &hadMarkerQuotes));
		}
	else
		{
		hadMarkerQuotes = FALSE;
		nullChk(newValue = StrDup((char *)value));
		}

	free(entry->value);
	entry->value = newValue;
	entry->addMarkerQuotes = hadMarkerQuotes;

Error:
	return error;
}

/*********************************************************************/

static IniEntry Ini_NewEntry(const char *name, const char *value, int stripMarkerQuotes)
{
	IniEntry    newEntry = NULL;
#if defined(_NI_mswin_) && _NI_mswin_
	int     error = UIENoError, tracking = SetEnableResourceTracking (0);
#else
	int     error = UIENoError;
#endif

	nullChk(newEntry = calloc(sizeof(*newEntry), 1));
	nullChk(newEntry->name = StrDupWithoutSurrWhiteSpace((char *)name));
	errChk( Ini_SetEntryValue(newEntry, value, stripMarkerQuotes));

Error:
#if defined(_NI_mswin_) && _NI_mswin_
	SetEnableResourceTracking (tracking);
#endif
	if (error < 0)
		{
		Ini_DisposeEntry(newEntry);
		newEntry = NULL;
		}

	return newEntry;
}

/*********************************************************************/

static void Ini_DisposeEntry(IniEntry theEntry)
{
	if (theEntry)
		{
		free(theEntry->name);
		free(theEntry->value);
		if (theEntry->comment)
			free(theEntry->comment);
		free(theEntry);
		}
}

/*********************************************************************/

static int CVICALLBACK Ini_CompareEntryNames(void *ptrToEntry1, void *ptrToEntry2)
{
	return StrICmp(Ini_EntryName(*(IniEntry*)ptrToEntry1), Ini_EntryName(*(IniEntry*)ptrToEntry2));
}

/*********************************************************************/

static int CVICALLBACK Ini_CompareEntryToName(void *entryName, void *ptrToEntry)
{
	return StrICmpWithoutSurrWhiteSpace((char *)entryName, Ini_EntryName(*(IniEntry*)ptrToEntry));
}

/*********************************************************************/

static char *Ini_EntryName(IniEntry entry)
{
	return entry ? entry->name : NULL;
//	return entry->name;
}

/*********************************************************************/

/*  After an entry is inserted into a section, the section owns it, is responsible
	for disposing it, and should be the only one to access it.
*/
static int Ini_InsertEntry(IniSection section, IniEntry entry, int index)
			/* ignores index if list is sorted;  use END_OF_LIST for index to add at end */
{
	if( !section || !entry )
		return UIENullPointerPassed;

	if (section->internallySortedEntryList)
		if (!ListInsertInOrder(section->internallySortedEntryList, &entry, Ini_CompareEntryNames))
			{
			/* if an memory allocation error occurs, simply abandon the optimization */
			ListDispose(section->internallySortedEntryList);
			section->internallySortedEntryList = NULL;
			}

	if (section->sorted)
		return ListInsertInOrder(section->entryList, &entry, Ini_CompareEntryNames);
	else
		return ListInsertItem(section->entryList, &entry, index);
}

/*********************************************************************/

static IniSection Ini_FindSection(IniText theIniText, const char *sectionName)
{
	int         index;
	IniSection  section = NULL;

	if( !theIniText )
		return section;

		/* optimization */
	if (theIniText->lastSectionAccessed
			&& (StrICmpWithoutSurrWhiteSpace((char *)sectionName,
							 theIniText->lastSectionAccessed->sectionName) == 0))
		return theIniText->lastSectionAccessed;

	if (theIniText->internallySortedList)  /* optimization */
		{
		index = (int)ListBinSearch(theIniText->internallySortedList, (void *)sectionName, Ini_CompareSectionToName);
		if (index != 0)
			ListGetItem(theIniText->internallySortedList, &section, index);
		}
	else
		{
		index = Ini_FindSectionIndex(theIniText, sectionName);

		if (index != 0)
			ListGetItem(theIniText->sectionList, &section, index);
		}

	if (section)
		theIniText->lastSectionAccessed = section;

	return section;
}

/*********************************************************************/

static int Ini_FindEntryIndex(IniSection section, const char *itemName)
{
	int index = 0;

	if (section)
		{
		if (section->sorted)
			index = (int)ListBinSearch(section->entryList, (void *)itemName, Ini_CompareEntryToName);
		else
			index = (int)ListFindItem(section->entryList, (void *)itemName, FRONT_OF_LIST, Ini_CompareEntryToName);
		}

	return index;
}

/*********************************************************************/

static IniEntry Ini_FindEntry(IniSection section, const char *itemName)
{
	int         index;
	IniEntry    entry = NULL;

	if( !section )
		return NULL;

	if (section->internallySortedEntryList)  /* optimization */
		{
		index = (int)ListBinSearch(section->internallySortedEntryList, (void *)itemName, Ini_CompareEntryToName);
		if (index != 0)
			ListGetItem(section->internallySortedEntryList, &entry, index);
		}
	else
		{
		index = Ini_FindEntryIndex(section, itemName);

		if (index != 0)
			ListGetItem(section->entryList, &entry, index);
		}

	return entry;
}

/*********************************************************************/

int CVIFUNC Ini_RemoveItem(IniText theIniText, const char *sectionName, const char *itemName)
{
	IniSection  section;
	int         removed=FALSE;

	if( (section=Ini_FindSection(theIniText,sectionName))
	 && (removed=Ini_RemoveFromSection(section,itemName,NULL)) )
		while (Ini_RemoveFromSection(section, itemName, NULL));   /* make sure to remove any duplicates as well */

	return removed;
}

/*********************************************************************/

static int Ini_RemoveFromSection(IniSection section, const char *itemName, int *indexRef)
			/* sets *indexRef to a positive number if it finds and removes the item */
{
	int         index = 0;
	IniEntry    entry;
	int         removed = FALSE;

	if (section)
		{
		if (section->internallySortedEntryList)
			{
			index = (int)ListBinSearch(section->internallySortedEntryList, (void *)itemName, Ini_CompareEntryToName);
			if (index != 0)
				ListRemoveItem(section->internallySortedEntryList, NULL, index);
			}

		index = Ini_FindEntryIndex(section, itemName);

		if (index != 0)
			{
			ListRemoveItem(section->entryList, &entry, index);
			Ini_DisposeEntry(entry);
			removed = TRUE;
			}
		}

	if (indexRef)
		*indexRef = index;
	return removed;
}

/*********************************************************************/

static int Ini_WriteSection(IniText iniText, IniSection theSection, Ini_OutputFunc outputFunc, void *outputDest)
{
	int     error = UIENoError;
	char    *outputBuf = NULL;
	int     outputLength;
	size_t  index;

	if( !iniText || !theSection )
		return UIENullPointerPassed;

	if (theSection->comment)
		errChk((*outputFunc)(outputDest, theSection->comment));
	
	outputLength = (int)strlen(theSection->sectionName) + 2 + 1;
					/* +2 for section name brackets, +1 for the nul byte */
	nullChk (outputBuf = malloc(outputLength));
	sprintf (outputBuf, "%c%s%c", iniText->sectionNameStartChar, theSection->sectionName,
								  iniText->sectionNameEndChar);
	errChk ((*outputFunc) (outputDest, outputBuf));

	if (theSection->entryList)
		for (index=1; index <= ListNumItems(theSection->entryList); index++)
			errChk (Ini_WriteEntry (iniText, *(IniEntry*)ListGetPtrToItem(theSection->entryList, index),
								   outputFunc, outputDest));

Error:
	free(outputBuf);
	return error;
}

/*********************************************************************/

static int Ini_WriteEntry(IniText iniText, IniEntry entry, Ini_OutputFunc outputFunc, void *outputDest)
{
	int     error = UIENoError;
	char    itemNameBuff[kMaxSectionOrTagNameSize + 1];
	char    outputBuffer[kMaxIniLineSize + 1];
	int     len, i, maxValueLen;
	char    *srcTextPtr;

	if( !iniText || !entry || !outputFunc )
		return UIENullPointerPassed;

	if (entry->comment)
		errChk((*outputFunc)(outputDest, entry->comment));
	
	len = (int)strlen(entry->value);

	// iniText->maxValueLineLength == 0 indicates no limit on the value line length, so write everything to a single line.
	if (iniText->maxValueLineLength == 0 || len <= iniText->maxValueLineLength)
		{
		maxValueLen = (int)(sizeof(outputBuffer) - min(strlen(entry->name),kMaxSectionOrTagNameSize) - strlen(iniText->nameValueOutputSeparatorToken) - (entry->addMarkerQuotes ? 2 : 0) - 1 /* terminating NULL */);
		StringCopyMax(outputBuffer, entry->name, min(strlen(entry->name),kMaxSectionOrTagNameSize)+1);
		strcat (outputBuffer, iniText->nameValueOutputSeparatorToken);
		if (entry->addMarkerQuotes)
			strcat (outputBuffer, "\"");
		strncat (outputBuffer, entry->value, maxValueLen);
		if (entry->addMarkerQuotes)
			strcat (outputBuffer, "\"");
		errChk ((*outputFunc)(outputDest, outputBuffer));
		}
	else
		{
		srcTextPtr = entry->value;
		i = 1;

		while (*srcTextPtr != 0)
			{
			StringCopyMax(itemNameBuff, entry->name, sizeof(itemNameBuff) - strlen(kLineTag) - 4 /* width of line number */);
			sprintf (itemNameBuff+strlen(itemNameBuff), "%s%04d", kLineTag, i++);
			
			maxValueLen = (int)(sizeof(outputBuffer) - strlen(itemNameBuff) - strlen(iniText->nameValueOutputSeparatorToken) - (entry->addMarkerQuotes ? 2 : 0) - 1 /* terminating NULL */);
			if (iniText->maxValueLineLength < maxValueLen)
				maxValueLen = iniText->maxValueLineLength;
			if ((int)strlen(srcTextPtr) < maxValueLen)
				maxValueLen = (int)strlen(srcTextPtr);
			
			strcpy (outputBuffer, itemNameBuff);
			strcat (outputBuffer, iniText->nameValueOutputSeparatorToken);
			if (entry->addMarkerQuotes)
				strcat (outputBuffer, "\"");
			strncat (outputBuffer, srcTextPtr, maxValueLen);
			if (entry->addMarkerQuotes)
				strcat (outputBuffer, "\"");

			srcTextPtr += maxValueLen;
			errChk ((*outputFunc)(outputDest, outputBuffer));
			
			}
		}

Error:
	return error;
}

/*********************************************************************/

static int  Ini_Put(IniText theIniText, const char *sectionName, const char *itemName, const char *value, int addMarkerQuotes)
{
	IniSection  section = NULL;
	IniSection  newSection = NULL;
	IniEntry    newEntry = NULL;
	int         error = UIENoError, tracking;

	if( !theIniText )
		return UIENullPointerPassed;

#if defined(_NI_mswin_) && _NI_mswin_
	tracking = SetEnableResourceTracking (0);
#endif
	nullChk(newEntry = Ini_NewEntry(itemName, value, FALSE));
	newEntry->addMarkerQuotes = addMarkerQuotes;

	section = Ini_FindSection(theIniText, sectionName);

	if (!section) /* if no current section with this name, create one and use it */
		{
		nullChk(newSection = Ini_NewSection(sectionName, theIniText->sorted));
		nullChk(Ini_InsertEntry(newSection, newEntry, END_OF_LIST));
		nullChk(Ini_InsertSection(theIniText, newSection));
		}
	else
		{
		int index, firstIndex=END_OF_LIST;   /* 1-based */

		if (theIniText->checkForDuplicatesWhenPutting)
			{
			while (Ini_RemoveFromSection(section, itemName, &index)) /* remove any items with the same name */
				if (firstIndex == END_OF_LIST)
					firstIndex = index;
			}
		nullChk(Ini_InsertEntry(section, newEntry, firstIndex));  /* ignores firstIndex parameter if list is sorted */
		}

Error:
#if defined(_NI_mswin_) && _NI_mswin_
	SetEnableResourceTracking (tracking);
#endif
	if (error < 0)
		{
		Ini_DisposeSection(newSection);
		Ini_DisposeEntry(newEntry);
		}

	return error;
}

/*********************************************************************/

/*  If not found, sets *value = NULL and returns FALSE if not found.
	If found, sets *value = ptr to value string and returns TRUE
	(the caller must not modify the returned string).
	The string in *value is valid until the next operation on theIniText.
*/
static int  Ini_Get(IniText theIniText, const char *sectionName, const char *itemName, char **value)
{
	IniSection  section = NULL;
	IniEntry    entry = NULL;
	int         found = FALSE;

	if( !theIniText )
		return UIENullPointerPassed;

	if  (value)
		*value = NULL;

	section = Ini_FindSection(theIniText, sectionName);

	if (section)
		{
		entry = Ini_FindEntry(section, itemName);
		if (entry)
			{
			if (value)
				*value = entry->value;
			found = TRUE;
			theIniText->lastLineAccessed = entry->sourceLine;
			}
		}

	return found;
}

/*********************************************************************/

int CVIFUNC Ini_WriteGeneric(IniText theIniText, Ini_OutputFunc outputFunc, void *outputDest)
{
	int		error = UIENoError;
	size_t	index;

	if( !theIniText || !outputFunc )
		return UIENullPointerPassed;

	if (theIniText->topComment)
		{
		errChk((*outputFunc)(outputDest, theIniText->topComment));
		errChk((*outputFunc)(outputDest, ""));
		}
	
	if (theIniText->sectionList)
		{
		for (index = 1; index <= ListNumItems(theIniText->sectionList); index++)
			{
			errChk(Ini_WriteSection(theIniText, *(IniSection *)ListGetPtrToItem(theIniText->sectionList, index), outputFunc, outputDest));
			errChk((*outputFunc)(outputDest, ""));
			}
		}
	
	if (theIniText->bottomComment)
		{
		errChk((*outputFunc)(outputDest, theIniText->bottomComment));
		}
Error:
	return error;
}

/*********************************************************************/

static int CVICALLBACK Ini_FileOutputFunc(void *file, char *outputString)
{
	int error = UIENoError;
	int len = 0, numWritten = 0;

	errno = 0;
	len = (int)strlen(outputString);
	if (len)
		numWritten = (int)fwrite (outputString, 1, len, (FILE *)file);
	if (numWritten == len)
		{
		len = LINE_TERMINATOR_LEN;
		numWritten = (int)fwrite (LINE_TERMINATOR, 1, len, (FILE *)file);
		}
	if (numWritten != len)
		{
		errChk( Ini_GetErrCodeFromWriteErrno(errno, TRUE));
		}

Error:
	return error;
}

/*********************************************************************/

static int Ini_GetErrCodeFromWriteErrno(int errnoVal, int forceError)
{
	switch (errnoVal)
		{
		case 0:
			return forceError ? UIEIOError : UIENoError;
		case EPERM:
		case EACCES:
			return UIEAccessDenied;
		case ENOMEM:
			return UIEOutOfMemory;
		case ENOSPC:
			return UIEDiskFull;
		case EBUSY:
		case EIO:
		case ENXIO:
			return UIEIOError;
		default:
			return UIEIOError;
		}
}

/*********************************************************************/
/*********************************************************************/

int CVIFUNC Ini_WriteToFile(IniText theIniText, const char pathName[])
{
	char tempPathName[MAX_PATHNAME_LEN] = {0};
	FILE *tempFile=NULL;    /* a non-NULL value indicates the file is open */
	int tempFileCreated=FALSE;
	int isWritable;
	int error = UIENoError;
	char *iobuf = NULL;

	if( !theIniText )
		return UIENullPointerPassed;

	if (pathName)
		{
		/*******************************************************************/
		/* Check the file identified by pathName for writeability.         */
		/* Create a temporary file in the same directory.                  */
		/*******************************************************************/
		errChk (GetFileWritability ((char *)pathName, &isWritable));
		if (!isWritable)
			errChk (UIEAccessDenied);
		errChk (CreateAndOpenTemporaryFile ((char *)pathName, "ini$$", "CVI", "wb",
											tempPathName, &tempFile))
		tempFileCreated = TRUE;

		nullChk (iobuf = malloc(kIniWriteBufSize));
		setvbuf (tempFile, iobuf, _IOFBF, kIniWriteBufSize);

		/*******************************************************************/
		/* Write to the temporary file so that the original file will not  */
		/* be wiped out in case an error occurs.                           */
		/* Remember to flush the file before closing it.                   */
		/*******************************************************************/
		errChk (Ini_WriteGeneric (theIniText, Ini_FileOutputFunc, (void *)tempFile));
		errno = 0;
		if (fflush (tempFile) != 0)
			errChk( Ini_GetErrCodeFromWriteErrno(errno, TRUE));

		fclose (tempFile);
		tempFile = NULL;     /* so won't close it twice */

		/**********************************************************************/
		/* Remove the original file, and rename the temporary file.           */
		/* Since the two files are in the same directory, we should not       */
		/* get a disk full error.                                             */
		/* We should not get a persmission error, unless the original file    */
		/* is writable but not deletable.                                     */
		/**********************************************************************/
		errChk (DeleteAndRename ((char *)tempPathName, (char *)pathName));
		}
	else
		{
		errChk (Ini_WriteGeneric (theIniText, Ini_FileOutputFunc, (void *)stdout));
		fflush(stdout);
		}

Error:
	if (error < 0)
		if (tempFileCreated)
			{
			if (tempFile)
				fclose (tempFile);
			RemoveFileIfExists (tempPathName);
			}

	if (iobuf)
		free(iobuf);

	return error;
}

/*********************************************************************/

int CVIFUNC Ini_ReadFromFile(IniText theIniText, const char pathName[])
{
	int         error = UIENoError;
	FILE        *file = NULL;
	LineFile    lineFile = NULL;

	if( !theIniText )
		return UIENullPointerPassed;

	file = fopen (pathName, "rb");
	if (file == NULL)
		errChk (ToolErr_CouldNotOpenFileForReading);

	nullChk (lineFile = LineFile_New(LineFile_ReadFromFile, (void *)file));

	errChk (Ini_ReadGeneric (theIniText, Ini_LineFileInputFunction, (void *)lineFile));

Error:
	LineFile_Dispose(lineFile);
	if (file)
		fclose (file);
	return error;
}

/*********************************************************************/

/* This function is only available on Win32 platforms */
#if defined(_NI_mswin32_) && !defined(_LINK_CVI_LVRT_)

int CVIFUNC Ini_ReadFromRegistry (IniText theIniText, int rootKey,
								  const char *baseKeyName)
{
	int   error = UIENoError;
	DWORD subKeyCount;
	DWORD tagCount;
	char  *subKeyName = NULL;
	char  *tagName = NULL;
	char  *tagValue = NULL;
	DWORD sdkErr;
	DWORD numSubKeys;
	DWORD numValues;
	DWORD maxSubKeyNameLen;
	DWORD subKeyNameSize;
	DWORD maxValueNameLen;
	DWORD maxValueSize;
	DWORD tagNameSize;
	DWORD tagValueSize;
	DWORD keyType;
	HKEY  hrootKey = 0;
	HKEY  hbaseKey = 0;
	HKEY  hsubKey = 0;

	if( !theIniText )
		return UIENullPointerPassed;

	switch (rootKey)
		{
		case 0:
			hrootKey = HKEY_CLASSES_ROOT;
			break;
		case 1:
			hrootKey = HKEY_CURRENT_USER;
			break;
		case 2:
			hrootKey = HKEY_LOCAL_MACHINE;
			break;
		case 3:
			hrootKey = HKEY_USERS;
			break;
		default:
			errChk(UIEFileWasNotFound);
			break;
		}

	/* Open user's base Key and enumerate through all Subkeys */
	if ((sdkErr = RegOpenKeyEx (hrootKey, (LPCTSTR)baseKeyName, 0,
								KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
								&hbaseKey))
		!= ERROR_SUCCESS)
		{
		if (sdkErr == ERROR_FILE_NOT_FOUND)
			errChk(UIEFileWasNotFound);
		else if (sdkErr == ERROR_ACCESS_DENIED)
			errChk(UIEAccessDenied);
		else
			errChk(ToolErr_CouldNotOpenFileForReading);
		}
	if (RegQueryInfoKey (hbaseKey, NULL, NULL, NULL, &numSubKeys,
						 &maxSubKeyNameLen, NULL, NULL, NULL, NULL, NULL,
						 NULL)
		!= ERROR_SUCCESS)
		errChk(ToolErr_ErrorReadingFile);
	if (numSubKeys > 0)
		{
		numSubKeys = (numSubKeys > INT_MAX) ? INT_MAX : numSubKeys;

		/* On NT, maxSubKeyNameLen will not include NULL terminatior */
		if (!(subKeyName = (char *)malloc (maxSubKeyNameLen + 1)))
			errChk(UIEOutOfMemory);
		subKeyName[0] = 0;
		for (subKeyCount = 1; subKeyCount <= numSubKeys; subKeyCount++)
			{
			subKeyNameSize = (DWORD)maxSubKeyNameLen + 1;
			if (RegEnumKeyEx (hbaseKey, (DWORD)(subKeyCount - 1),
							  subKeyName, &subKeyNameSize, NULL, NULL,
							  NULL, NULL)
				!= ERROR_SUCCESS)
				errChk(ToolErr_ErrorReadingFile);
			if (RegOpenKey (hbaseKey, (LPCTSTR)subKeyName, &hsubKey)
				!= ERROR_SUCCESS)
				errChk(ToolErr_ErrorReadingFile);

			/* For this current Subkey, get the number of values */
			if (RegQueryInfoKey (hsubKey, NULL, NULL, NULL, NULL, NULL,
								 NULL, &numValues, &maxValueNameLen,
								 &maxValueSize, NULL, NULL)
				!= ERROR_SUCCESS)
				errChk(ToolErr_ErrorReadingFile);

			/* If there are Values, create a new Section for this subkey */
			/* and enumerate all values, bringing them into INI          */
			/* tag-value pairs.                                          */
			if (numValues > 0)
				{
				numValues = (numValues > INT_MAX) ? INT_MAX : numValues;

				/* maxValueNameLen does not include end NULL */
				if (!(tagName = (char *)malloc (maxValueNameLen + 1)))
					errChk(UIEOutOfMemory);
				if (!(tagValue = (char *)malloc (maxValueSize)))
					errChk(UIEOutOfMemory);
				tagName[0] = 0;
				tagValue[0] = 0;
				for (tagCount = 1; tagCount <= numValues; tagCount++)
					{
					tagNameSize = (DWORD) maxValueNameLen + 1;
					tagValueSize = (DWORD) maxValueSize;
					if (RegEnumValue (hsubKey, (DWORD)(tagCount-1),
									  tagName, &tagNameSize, NULL,
									  &keyType, (LPBYTE)tagValue,
									  &tagValueSize)
						!= ERROR_SUCCESS)
						errChk(ToolErr_ErrorReadingFile);
					if (keyType == REG_SZ)
						errChk(Ini_PutRawString (theIniText, subKeyName, tagName, tagValue));
					}
				if (tagName)
					{
					free (tagName);
					tagName = NULL;
					}
				if (tagValue)
					{
					free (tagValue);
					tagValue = NULL;
					}
				}
			if (hsubKey)
				{
				RegCloseKey (hsubKey);
				hsubKey = NULL;
				}
			}
		if (subKeyName)
			{
			free (subKeyName);
			subKeyName = NULL;
			}
		}
	if (hbaseKey)
		{
		RegCloseKey (hbaseKey);
		hbaseKey = NULL;
		}

Error:
	if (subKeyName)
		free (subKeyName);
	if (tagName)
		free(tagName);
	if (tagValue)
		free(tagValue);
	if (hsubKey)
		RegCloseKey(hsubKey);
	if (hbaseKey)
		RegCloseKey(hbaseKey);
	return error;
}

#endif /* _NI_mswin32_ && !defined(_LINK_CVI_LVRT_) */

/*********************************************************************/

/* This function is only available on Win32 platforms */
#if defined(_NI_mswin32_) && !defined(_LINK_CVI_LVRT_)

static int CVIFUNC Ini_ClearRegistry (HKEY hrootKey, const char *baseKeyName)
{
	int   error = UIENoError;
	DWORD subKeyCount;
	char  *subKeyName = NULL;
	char  *tagName = NULL;
	char  *tagValue = NULL;
	DWORD sdkErr;
	DWORD numSubKeys;
	DWORD maxSubKeyNameLen;
	DWORD subKeyNameSize;
	HKEY  hbaseKey = 0;
	HKEY  hsubKey = 0;


	/* Open user's base Key and enumerate through all Subkeys */
	if ((sdkErr = RegOpenKeyEx (hrootKey, (LPCTSTR)baseKeyName, 0,
								KEY_WRITE,
								&hbaseKey))
		!= ERROR_SUCCESS)
		{
		if (sdkErr == ERROR_FILE_NOT_FOUND)
			errChk(UIEFileWasNotFound);
		else if (sdkErr == ERROR_ACCESS_DENIED)
			errChk(UIEAccessDenied);
		else
			errChk(ToolErr_CouldNotOpenFileForReading);
		}
	if (RegQueryInfoKey (hbaseKey, NULL, NULL, NULL, &numSubKeys,
						 &maxSubKeyNameLen, NULL, NULL, NULL, NULL, NULL,
						 NULL)
		!= ERROR_SUCCESS)
		errChk(ToolErr_ErrorReadingFile);
	if (numSubKeys > 0)
		{
		numSubKeys = (numSubKeys > INT_MAX) ? INT_MAX : numSubKeys;

		/* On NT, maxSubKeyNameLen will not include NULL terminatior */
		if (!(subKeyName = (char *)malloc (maxSubKeyNameLen + 1)))
			errChk(UIEOutOfMemory);
		subKeyName[0] = 0;
		for (subKeyCount = 1; subKeyCount <= numSubKeys; subKeyCount++)
			{
			subKeyNameSize = (DWORD)maxSubKeyNameLen + 1;
			if (RegEnumKeyEx (hbaseKey, (DWORD)0,
							  subKeyName, &subKeyNameSize, NULL, NULL,
							  NULL, NULL)
				!= ERROR_SUCCESS)
				errChk(ToolErr_ErrorReadingFile);

			if ((sdkErr = RegDeleteKey(hbaseKey, subKeyName)) != ERROR_SUCCESS)
				if (sdkErr == ERROR_ACCESS_DENIED) {
					errChk(UIEAccessDenied);
				}
				else {
					errChk(UIEErrorWritingFile);
				}

			}
		if (subKeyName)
			{
			free (subKeyName);
			subKeyName = NULL;
			}
		}
	if (hbaseKey)
		{
		RegCloseKey (hbaseKey);
		hbaseKey = NULL;
		}


Error:
	if (subKeyName)
		free (subKeyName);
	if (tagName)
		free(tagName);
	if (tagValue)
		free(tagValue);
	if (hsubKey)
		RegCloseKey(hsubKey);
	if (hbaseKey)
		RegCloseKey(hbaseKey);
	return error;
}

#endif /* _NI_mswin32_ && !defined(_LINK_CVI_LVRT_) */

/*********************************************************************/

/* This function is only available on Win32 platforms */
#if defined(_NI_mswin32_) && !defined(_LINK_CVI_LVRT_)

int CVIFUNC Ini_WriteToRegistry (IniText theIniText, int rootKey,
								 const char *baseKeyName)
{
	int   error = UIENoError;
	int   sectionCount;
	int   numSections;
	int   itemCount;
	char  *sectionName;
	char  *tagName;
	char  *tagValue;
	DWORD sdkErr;
	DWORD keyDisp;
	HKEY  hrootKey = 0;
	HKEY  hbaseKey = 0;
	HKEY  hsubKey = 0;

	if( !theIniText )
		return UIENullPointerPassed;

	switch (rootKey)
		{
		case 0:
			hrootKey = HKEY_CLASSES_ROOT;
			break;
		case 1:
			hrootKey = HKEY_CURRENT_USER;
			break;
		case 2:
			hrootKey = HKEY_LOCAL_MACHINE;
			break;
		case 3:
			hrootKey = HKEY_USERS;
			break;
		default:
			errChk(UIEFileWasNotFound);
			break;
		}

	errChk(Ini_ClearRegistry(hrootKey, baseKeyName));

	/* Open or create the user's base Key */
	if ((sdkErr = RegCreateKeyEx (hrootKey, (LPCTSTR)baseKeyName, 0, "",
								  REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL,
								  &hbaseKey, &keyDisp))
		!= ERROR_SUCCESS)
		{
		if (sdkErr == ERROR_ACCESS_DENIED)
			errChk(UIEAccessDenied);
		else
			errChk(UIEErrorWritingFile);
		}

	/* Loop through all Sections in the INI object */
	numSections = Ini_NumberOfSections (theIniText);
	for (sectionCount = 1; sectionCount <= numSections; sectionCount++)
		{
		errChk(Ini_NthSectionName (theIniText, sectionCount, &sectionName));

		/* Open or create this Section as a Subkey of the user's base Key */
		if ((sdkErr = RegCreateKeyEx (hbaseKey, (LPCTSTR) sectionName, 0,
									  "", REG_OPTION_NON_VOLATILE,
									  KEY_WRITE, NULL, &hsubKey,
									  &keyDisp))
			!= ERROR_SUCCESS)
			{
			if (sdkErr == ERROR_ACCESS_DENIED)
				errChk(UIEAccessDenied);
			else
				errChk(UIEErrorWritingFile);
			}

		/* Loop through all Items in this Section, creating Values */
		for (itemCount = 1;
			 itemCount <= Ini_NumberOfItems (theIniText, sectionName);
			 itemCount++)
			{
			errChk(Ini_NthItemName (theIniText, sectionName, itemCount,
				   &tagName));
			errChk(Ini_GetPointerToRawString (theIniText, sectionName,
				   tagName, &tagValue));

			if ((sdkErr = RegSetValueEx (hsubKey, tagName, 0, REG_SZ,
										 (CONST BYTE *)tagValue,
										 strlen (tagValue) + 1))
				!= ERROR_SUCCESS)
				{
				if (sdkErr == ERROR_ACCESS_DENIED)
					errChk(UIEAccessDenied);
				else
					errChk(UIEErrorWritingFile);
				}
			}
		if (hsubKey)
			{
			RegCloseKey (hsubKey);
			hsubKey = NULL;
			}
		}

Error:
	if (hsubKey)
		RegCloseKey (hsubKey);
	if (hbaseKey)
		RegCloseKey (hbaseKey);
	if (error > 0)
		error = UIENoError;
	return error;
}

#endif /* _NI_mswin32_ && !defined(_LINK_CVI_LVRT_) */

/*********************************************************************/

static int CVICALLBACK Ini_LineFileInputFunction(void *inputSource, char *inputBuffer, int maxLineLength, int *endOfSource, int *lineNumRead)
{
	int error = UIENoError;

	errChk (LineFile_Read((LineFile)inputSource, inputBuffer, maxLineLength, NULL, endOfSource));
	if (lineNumRead)
		*lineNumRead = LineFile_NumLinesRead((LineFile)inputSource);

Error:
	return error;
}

/***********************************************************************************/

int  CVIFUNC Ini_ReadGeneric(IniText theIniText, Ini_InputFunc inputFunc, void *inputSource)
{
	char        lineBuf[kMaxIniLineSize+1];
	char        sectionBuf[kMaxSectionOrTagNameSize+1];
	char        itemNameBuf[kMaxSectionOrTagNameSize+1];
	char        valueBuf[kMaxIniLineSize+1];
	int         error = UIENoError;
	int         lineType;
	IniSection  currentSection = NULL, newSection = NULL;
	int         endOfSource = FALSE;
	IniEntry    newEntry = NULL;
	int         lineRead, tracking;
	int         internalSortingEnabled = theIniText->internallySortedList != NULL;
	char		*currentComment = NULL;

	if( !theIniText || !inputFunc )
		return UIENullPointerPassed;

#if defined(_NI_mswin_) && _NI_mswin_
	tracking = SetEnableResourceTracking (0);
#endif
	while (!endOfSource)
		{
		errChk((*inputFunc)(inputSource, lineBuf, kMaxIniLineSize, &endOfSource, &lineRead));

		if (!endOfSource)
			{
			sectionBuf[0] = 0;
			itemNameBuf[0] = 0;
			valueBuf[0] = 0;

			errChk(lineType = ParseTagTextLine(theIniText, lineBuf, sectionBuf, itemNameBuf, valueBuf));

			switch (lineType)
				{
				case kIni_SectionLine:
					if (theIniText->sectionFilter && !theIniText->sectionFilter(theIniText, theIniText->sectionFilterCallbackData, sectionBuf))
						{
						currentSection = NULL;
						if (currentComment)
							{
							free(currentComment);
							currentComment = NULL;
							}
						}
					else
						{
						nullChk(newSection = Ini_NewSection(sectionBuf, theIniText->sorted));
						newSection->sourceLine = lineRead;
						newSection->comment = currentComment;
						currentComment = NULL;
						errChk(ListInsertItem(theIniText->sectionList, &newSection, END_OF_LIST));
						currentSection = newSection;
						newSection = NULL;
						}
					break;
				case kIni_TagAndValueLine:
					if (currentSection)
						{
						nullChk(newEntry = Ini_NewEntry(itemNameBuf, valueBuf, TRUE));
						newEntry->sourceLine = lineRead;
						newEntry->comment = currentComment;
						currentComment = NULL;
						errChk(ListInsertItem(currentSection->entryList, &newEntry, END_OF_LIST));
						newEntry = NULL;
						}
					else if (currentComment)
						{
						free(currentComment);
						currentComment = NULL;
						}
					break;
				case kIni_CommentLine:
					if (currentComment)
						nullChk(AppendString(&currentComment, LINE_TERMINATOR, LINE_TERMINATOR_LEN));
					nullChk(AppendString(&currentComment, lineBuf, -1));
					break;
				default:
					if (currentComment && currentSection == NULL && theIniText->topComment == NULL)
						{
						theIniText->topComment = currentComment;
						currentComment = NULL;
						}
					break;
				}
			}
		else if (currentComment && theIniText->bottomComment == NULL)
			{
			theIniText->bottomComment = currentComment;
			currentComment = NULL;
			}
		}

	errChk( CoalesceMultilineItemsAfterRead(theIniText));

	if (theIniText->sorted)
		Ini_Sort(theIniText);

Error:
#if defined(_NI_mswin_) && _NI_mswin_
	SetEnableResourceTracking (tracking);
#endif
	Ini_DisposeSection(newSection);
	Ini_DisposeEntry(newEntry);
	if (internalSortingEnabled)
		 Ini_SortInternally(theIniText);
	if (currentComment)
		free(currentComment);
	return error;
}

/*********************************************************************/

static int CoalesceMultilineItemsAfterRead(IniText iniText)
{
	int         error = UIENoError;
	int         numSections;
	int         index;
	IniSection  section;

	if( !iniText )
		return UIENullPointerPassed;

	numSections = (int)ListNumItems(iniText->sectionList);

	for (index = 1; index <= numSections; index++)
		{
		ListGetItem(iniText->sectionList, &section, index);
		errChk( CoalesceMultilineItemsInSection(section));
		}

Error:
	return error;
}

/*********************************************************************/

	/*  Searches for a multiline entry starting from <startOfSearchIndex>.  Returns the
		start and ending indices of the multiline entry if found.  Function returns TRUE
		if a multiline item was found, FALSE otherwise.
	*/

static int FindMultiLineItem(IniSection section, int startOfSearchIndex, int *startIndex, int *endIndex)
{
	int         numItems;
	int         index;
	IniEntry    entry;
	int         lookingForEndOfMultiLine = FALSE;
	int         isMultiLineItem;
	int         whichLine;

	if( !section )
		return lookingForEndOfMultiLine;

	numItems = (int)ListNumItems(section->entryList);

	for (index = startOfSearchIndex; index <= numItems; index++)
		{
		ListGetItem(section->entryList, &entry, index);
		Ini_AnalyzeItemName(entry->name, &isMultiLineItem, &whichLine, 0);

			/* if we found the first line of a multiline entry */
		if (isMultiLineItem && !lookingForEndOfMultiLine && whichLine == 1)
				{
				*startIndex = index;
				*endIndex = index;
				lookingForEndOfMultiLine = TRUE;
				}
		else
		if (lookingForEndOfMultiLine)
			{
			if (!isMultiLineItem || whichLine == 1)  /* found the end of this multiline entry */
				break;
			else
				*endIndex = index;  /* found another item in this multiline entry */
			}
		}

	return lookingForEndOfMultiLine;
}

/*********************************************************************/

static int CoalesceMultiLineItems(IniSection section, int startIndex, int endIndex)
{
	IniEntry    entry;
	char        singleLineItemNameBuf[kMaxIniLineSize+1];
	char        *newName = 0;
	int         error = UIENoError;
	int         length = 0;
	char        *coalescedValue = 0;
	char        *appendPtr;
	int         index;

	if( !section )
		return UIENullPointerPassed;

	for (index = startIndex; index <= endIndex; index++)
		{
		ListGetItem(section->entryList, &entry, index);
		length += (int)strlen(entry->value);
		}

	nullChk( coalescedValue = (char *)malloc(length+1));

	appendPtr = coalescedValue;

		/* coalesce the values of all multiline entries */
	for (index = startIndex; index <= endIndex; index++)
		{
		ListGetItem(section->entryList, &entry, index);
		strcpy(appendPtr, entry->value);
		appendPtr += strlen(entry->value);
		}

	ListGetItem(section->entryList, &entry, startIndex);
	Ini_AnalyzeItemName(entry->name, 0, 0, singleLineItemNameBuf); /* get item base name */
	nullChk( newName = StrDup(singleLineItemNameBuf));

		/* alter first entry to contain the coalesced value */
	free(entry->value);
	entry->value = coalescedValue;
	free(entry->name);
	entry->name = newName;
	/* keep the same value of entry->addMarkerQuotes */

		/* dispose all multiline entries except the first */
	for (index = startIndex + 1; index <= endIndex; index++)
		{
		ListRemoveItem(section->entryList, &entry, startIndex + 1);
		Ini_DisposeEntry(entry);
		}

Error:
	if (error < 0)
		{
		free(newName);
		free(coalescedValue);
		}

	return error;
}

/*********************************************************************/

static int CoalesceMultilineItemsInSection(IniSection   section)
{
	int         error = UIENoError;
	int         index;
	int         start;
	int         end;

	for (index = 1; FindMultiLineItem(section, index, &start, &end); index = start + 1)
		{
		errChk( CoalesceMultiLineItems(section, start, end));
		}

Error:
	return error;
}

/*********************************************************************/


	 /* returns an IniLineType enum or negative int error,
		buf must be NULL terminated
	*/
static int ParseTagTextLine(IniText iniText, const char buf[], char *section, char *item, char *value)
{
	unsigned char	*tagStart, *tagEnd, *valStart, *valEnd, *sep, *p;
	int				tagLen, valLen;

	/********************************************************************/
	/* NOTE:  Comment lines begin with ';'.                             */
	/*        Section titles are in square brackets.                    */
	/*        Extraneous characters after "<tag>=<val>" are ignored.    */
	/*        If the value can include embedded blanks (particularly    */
	/*        trailing blanks) then it should be surrounded by double   */
	/*        quotation marks.  If the value can begin and end with     */
	/*        double qoutation marks, then it should be surrounded      */
	/*        with an extra set of double quotation marks.              */
	/*        We will consider an all blank value as no value at all.   */
	/*        However, "" and "   " will be considered as significant   */
	/*        values.                                                   */
	/*        NOTE:  Leading and trailing blanks (outside of the double */
	/*               quotes) are always ignored.                        */
	/********************************************************************/

	if( !iniText )
		return UIENullPointerPassed;

	tagStart = (unsigned char *)SkipWhiteSpace(buf);

	if (FastTokenCmp(tagStart,(unsigned char *)iniText->commentMarker) == 0)
		return kIni_CommentLine;                /* is a comment */

	if (*tagStart == iniText->sectionNameStartChar)
		return ParseTagTextSectionTitle(iniText, (char *)tagStart+1, section);

	for (p=tagStart; FastTokenCmp(p, (unsigned char *)iniText->nameValueInputSeparatorToken); CmbStrInc(p))
		if (!*p)
			return kIni_NotAnIniLine;               /* no separator */

	sep = p;

	/* find end of tag, removing white space from trailing end of the tag */
	for (tagEnd = CmbStrPrev(tagStart, sep); (tagEnd != NULL) && isspace(*tagEnd); CmbStrDec(tagStart, tagEnd));

	if (tagEnd == NULL)
		return kIni_NotAnIniLine;               /* no tag */

	/* point to first character after the last character in the tag */
	CmbStrInc(tagEnd);

	valStart = (unsigned char *)SkipWhiteSpace((char *)sep + iniText->nameValueInputSeparatorLength);

	if (valStart && valStart[0] == '\0')		/* if valStart pointing to the end of the line */
		return kIni_NotAnIniLine;               /* no value string at all */

	p = CmbStrLastChar(valStart);
	while (p && isspace (*p))                /* look backwards for last non white space */
		CmbStrDec(valStart, p);

	if (p == NULL)
		return kIni_NotAnIniLine;               /* no value string at all */

	valEnd = CmbStrNext(p); /* valEnd points to the byte after the last non white space of the value */


	/*****************************************************************/
	/* At this point, we know that there is a tag, a separator       */
	/* and a value.  So we can dynamically allocate the strings.     */
	/*****************************************************************/

	tagLen = (int)(tagEnd - tagStart);

	memcpy (item, tagStart, tagLen);
	(item)[tagLen] = 0;

	valLen = (int)(valEnd - valStart);
	memcpy (value, valStart, valLen);
	(value)[valLen] = 0;

	return kIni_TagAndValueLine;
}

/*********************************************************************/

/*
	Returns an IniLineType enum or negative int error
	modified to still ignore leading and trailing white space after the start
	bracket and before the close bracket while keeping white space between multiple
	words in the section title (before it stopped after the first word.
*/
static int ParseTagTextSectionTitle(IniText iniText, const char *firstCharAfterOpenBracket, char *sectionTitle)
{
	unsigned char	*begChar, *closeBracket, *stopChar;
	int				nChars;

	if( !iniText )
		return UIENullPointerPassed;

	begChar = (unsigned char *)SkipWhiteSpace (firstCharAfterOpenBracket);

	if ((closeBracket = CmbStrRChr (begChar, iniText->sectionNameEndChar)) == NULL)     /* JAG 8/23/96 PATCH 4.0.1 */
		return kIni_NotAnIniLine;

		/* make sure the end bracket is the last non-whitespace character on the line */
	if (*SkipWhiteSpace((char *)closeBracket + 1) != '\0')    /* JAG 8/23/96 PATCH 4.0.1 */
		return kIni_NotAnIniLine;

		/* find end of section title (minus trailing whitespace) */
	for (stopChar = CmbStrPrev(begChar, closeBracket);
		 (stopChar != NULL) && isspace(*stopChar);
		 CmbStrDec(begChar, stopChar));

	if (stopChar == NULL)
		return kIni_NotAnIniLine;

	CmbStrInc(stopChar);

	nChars = (int)(stopChar - begChar);

	memcpy (sectionTitle, begChar, nChars);
	(sectionTitle)[nChars] = 0;

	return kIni_SectionLine;
}

/*********************************************************************/

#define kBooleanTrueValueString   "True"
#define kBooleanFalseValueString  "False"

/**********************************************************************/

int CVIFUNC Ini_PutInt(IniText dest, const char *section, const char *itemName, int value)
{
	char buffer[256];

	sprintf(buffer, "%d", value);
	return Ini_Put(dest, section, itemName, buffer, FALSE);
}

/**********************************************************************/

int CVIFUNC Ini_PutUInt(IniText dest, const char *section, const char *itemName, unsigned int value)
{
	char buffer[256];

	sprintf(buffer, "%u", value);
	return Ini_Put(dest, section, itemName, buffer, FALSE);
}

/**********************************************************************************/

int CVIFUNC Ini_PutDouble(IniText dest, const char *section, const char *itemName, double value)
{
	char buffer[256];

	sprintf(buffer, "%.16g", value);   /* JAG 8/13/96 PATCH 4.0.1 */
	return Ini_Put(dest, section, itemName, buffer, FALSE);
}

/**********************************************************************************/

int CVIFUNC Ini_PutBoolean(IniText dest, const char *section, const char *itemName, int boolValue)
{
	return Ini_Put (dest, section, itemName, boolValue
												? kBooleanTrueValueString
												: kBooleanFalseValueString,
											 FALSE);
}

/**********************************************************************/

int CVIFUNC Ini_PutString(IniText dest, const char *section, const char *itemName, const char *srcText)
{
	int     error = UIENoError;
	char    *escText = NULL;
#if defined(_NI_mswin_) && _NI_mswin_
	int	tracking = SetEnableResourceTracking (0);
#endif

	if (!srcText)
		srcText = "";

	errChk( EscapeText(srcText, &escText));
	errChk( Ini_PutRawString(dest, section, itemName, escText));

Error:
#if defined(_NI_mswin_) && _NI_mswin_
	SetEnableResourceTracking (tracking);
#endif
	free(escText);
	return error;
}

/**********************************************************************/

int CVIFUNC Ini_PutRawString(IniText dest, const char *section, const char *itemName, const char *srcText)
{
	return Ini_PutSingleLineString(dest, section, itemName, srcText);  /* add quotes around string */
}

/*****************************************************************************/

/*  Ini_GetPointerToString:
	If itemName is NOT found, sets *value to NULL and returns FALSE.
	If itemName IS found, sets *value to the pointer to string, and returns TRUE.
	The string in *value is valid until the next call to Ini_GetPointerToString,
	Ini_GetStringCopy, or Ini_GetStringIntoBuffer..  (or their "Raw" equivalents).
	If an error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetPointerToString(IniText dest, const char *section, const char *itemName, char **destStr)
{
	int     error = UIENoError;
	int     found;
	char    *rawString;

	if( !destStr )
		return UIENullPointerPassed;

	*destStr = NULL;

	errChk( found = Ini_GetPointerToRawString(dest, section, itemName, &rawString));

	if (found)
		{
		errChk(SetStringCacheSize(dest->stringCache, (int)strlen(rawString)+1));
		errChk(UnescapeText(rawString, *dest->stringCache));
		*destStr = *dest->stringCache;
		}

Error:
	if (error < 0)
		return error;
	else
		return found;
}

/*****************************************************************************/

/*  Ini_GetPointerToRawString:
	If itemName is NOT found, sets *value to NULL and returns FALSE.
	If itemName IS found, sets *value to the pointer to string, and returns TRUE.
	The string in *value is the string in the in-memory list.  Do not modify its contents.
	Ini_GetStringCopy, or Ini_GetStringIntoBuffer (or their "raw" equivalents)
	If an error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetPointerToRawString(IniText dest, const char *section, const char *itemName, char **destStr)
{
	return  Ini_Get(dest, section, itemName, destStr);
}

/************************************************************************************/

	/* Allocated in chunk sized increments to minimize the need to reallocate when the cache grows or shrinks */
static int SetStringCacheSize(char **strHandle, int sizeNeeded)
{
	int error = UIENoError;
	int numCacheChunks;

	numCacheChunks  = (sizeNeeded / kStringCacheChunkSize) + 1;  /* minumum number of chunks needed to hold <sizeNeeded> bytes */
	if (!SetHandleSize((void **)strHandle, numCacheChunks * kStringCacheChunkSize))
		errChk (UIEOutOfMemory);

Error:
	return error;
}

/*****************************************************************************/

static int  Ini_PutSingleLineString(IniText theIniText, const char *section, const char *itemName, const char *value)
{
	int     error = UIENoError;

	if (value == NULL)
		value = "";

	errChk(Ini_Put(theIniText, section, itemName, value, theIniText->addQuotesToStrings));

Error:
	return error;
}

/************************************************************************************/

/*
	Ini_GetStringCopy
	If itemName is NOT found, sets *value to NULL and returns FALSE.
	If itemName IS found, dynamically allocates a copy of the string,
	places the pointer to the copy in *value, and returns TRUE.
	The user is responsible for freeing the string.
	If an error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetStringCopy(IniText theIniText, const char *section, const char *itemName, char **value)
{
	int     error = UIENoError;
	char    *string;
	int     found;

	if( !value )
		return UIENullPointerPassed;

	*value = NULL;
	errChk(found = Ini_GetPointerToString(theIniText, section, itemName, &string));
	if (found)
		nullChk(*value = StrDup(string));

Error:
	if (error < 0)
		{
		free(*value);
		*value = NULL;
		return error;
		}
	else
		return found;
}

/************************************************************************************/

/*
	Ini_GetRawStringCopy
	If itemName is NOT found, sets *value to NULL and returns FALSE.
	If itemName IS found, dynamically allocates a copy of the string,
	places the pointer to the copy in *value, and returns TRUE.
	The user is responsible for freeing the string.
	If an error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetRawStringCopy(IniText theIniText, const char *section, const char *itemName, char **value)
{
	int     error = UIENoError;
	char    *string;
	int     found;

	if( !value )
		return UIENullPointerPassed;

	*value = NULL;
	errChk(found = Ini_GetPointerToRawString(theIniText, section, itemName, &string));
	if (found)
		nullChk(*value = StrDup(string));

Error:
	if (error < 0)
		{
		free(*value);
		*value = NULL;
		return error;
		}
	else
		return found;
}

/************************************************************************************/

/*
	Ini_GetStringIntoBuffer
	If itemName is NOT found, sets buffer[0] to 0, and returns FALSE.
	If itemName IS found, copies the string into buffer, and returns TRUE.
	If an error occurs, returns a negative error code.
	(Note: bufSize includes the space for the nul byte.)
*/
int CVIFUNC Ini_GetStringIntoBuffer(IniText theIniText, const char *section, const char *itemName,
							 char buffer[], size_t bufSize)
{
	int     error = UIENoError;
	char    *temp;
	int     found;

	buffer[0] = 0;
	errChk(found = Ini_GetPointerToString(theIniText, section, itemName, &temp));
	if (found)
		{
		StringCopyMax(buffer, temp, bufSize);
		}

Error:
	if (error < 0)
		return error;
	else
		return found;
}

/************************************************************************************/

/*
	Ini_GetRawStringIntoBuffer
	If itemName is NOT found, sets buffer[0] to 0, and returns FALSE.
	If itemName IS found, copies the string into buffer, and returns TRUE.
	If an error occurs, returns a negative error code.
	(Note: bufSize includes the space for the nul byte.)
*/
int CVIFUNC Ini_GetRawStringIntoBuffer(IniText theIniText, const char *section, const char *itemName,
							 char buffer[], size_t bufSize)
{
	int     error = UIENoError;
	char    *temp;
	int     found;

	buffer[0] = 0;
	errChk(found = Ini_GetPointerToRawString(theIniText, section, itemName, &temp));
	if (found)
		{
		StringCopyMax(buffer, temp, bufSize);
		}

Error:
	if (error < 0)
		return error;
	else
		return found;
}

/**********************************************************************/

/*
	Ini_GetInt:
	If the itemName is NOT found or has an empty value, returns FALSE.
	If itemName IS found and has a valid ASCII representation of an integer,
	sets *value to the integer value, and returns TRUE.
	If the value is not a valid ASCII representation of an integer, or any
	other error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetInt(IniText src, const char *section, const char *itemName, int *value)
	/* returns negative error, or FALSE (tag or value is missing), or TRUE */
{
	char *temp=0;
	int found;
	int error = UIENoError;

	errChk (found = Ini_Get(src,section,itemName, &temp));
	found = found && HasNonWhiteSpace(temp);
	if (found)
		if (!StrToInt (temp, value))
			errChk (ToolErr_InvalidIntNumber);

Error:
	if (error < 0)
		return error;
	else
		return found;
}

/**********************************************************************/

/*
	Ini_GetInt64:
	If the itemName is NOT found or has an empty value, returns FALSE.
	If itemName IS found and has a valid ASCII representation of an integer,
	sets *value to the integer value, and returns TRUE.
	If the value is not a valid ASCII representation of an integer, or any
	other error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetInt64(IniText src, const char *section, const char *itemName, long long *value)
	/* returns negative error, or FALSE (tag or value is missing), or TRUE */
{
	char *temp=0;
	int found;
	int error = UIENoError;

	errChk (found = Ini_Get(src,section,itemName, &temp));
	found = found && HasNonWhiteSpace(temp);
	if (found)
		if (!StrToLongLong (temp, value))
			errChk (ToolErr_InvalidIntNumber);
Error:
	if (error < 0)
		return error;
	else
		return found;
}

/***************************************************************************/

/*
	Ini_GetUInt:
	If the itemName is NOT found or has an empty value, returns FALSE.
	If itemName IS found and has a valid ASCII representation of an unsigned
	integer, sets *value to the integer value, and returns TRUE.
	If the value is not a valid ASCII representation of an unsigned integer,
	or any other error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetUInt(IniText src, const char *section, const char *itemName, unsigned int *value)
	/* returns negative error, or FALSE (tag or value is missing), or TRUE */
{
	char *temp;
	int found;
	int error = UIENoError;

	errChk (found = Ini_Get (src, section, itemName, &temp));
	found = found && HasNonWhiteSpace (temp);
	if (found)
		if (!StrToUInt (temp, value))
			errChk (ToolErr_InvalidUIntNumber);

Error:
	if (error < 0)
		return error;
	else
		return found;
}

/***************************************************************************/

/*
	Ini_GetUInt64:
	If the itemName is NOT found or has an empty value, returns FALSE.
	If itemName IS found and has a valid ASCII representation of an unsigned
	integer, sets *value to the integer value, and returns TRUE.
	If the value is not a valid ASCII representation of an unsigned integer,
	or any other error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetUInt64(IniText src, const char *section, const char *itemName, unsigned long long *value)
	/* returns negative error, or FALSE (tag or value is missing), or TRUE */
{
	char *temp;
	int found;
	int error = UIENoError;

	errChk (found = Ini_Get (src, section, itemName, &temp));
	found = found && HasNonWhiteSpace (temp);
	if (found && value)
		if (!StrToULongLong (temp, value))
			errChk (ToolErr_InvalidIntNumber);
Error:
	if (error < 0)
		return error;
	else
		return found;
}

/***************************************************************************/

/*
	Ini_GetDouble:
	If the itemName is NOT found or has an empty value, returns FALSE.
	If itemName IS found and has a valid ASCII representation of a double,
	sets *value to the double value, and returns TRUE.
	If the value is not a valid ASCII representation of a double, or any
	other error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetDouble(IniText src, const char *section, const char *itemName, double *value)
	/* returns negative error, or FALSE (tag or value is missing), or TRUE */
{
	char *temp = NULL, *endPtr = NULL;
	int found, len;
	int error = UIENoError;

	errChk (found = Ini_Get(src,section,itemName, &temp));
	found = found && HasNonWhiteSpace (temp);
	if (found)
		{
		if ((StrICmp(temp, "+Inf") == 0)
					||  (StrICmp(temp, "Inf") == 0))
			*value = HUGE_VAL;
		else
			{
			if (StrICmp(temp, "-Inf") == 0)
				*value = -HUGE_VAL;
			else {
				errno = 0;
				len = (int)strlen(temp);
				*value = strtod(temp, (char **)&endPtr);
				if (errno || (endPtr != temp + len))
					errChk (ToolErr_InvalidDoubleNumber);
				}
			}
		}

Error:
	if (error < 0)
		return error;
	else
		return found;
}

/***************************************************************************/

/*
	Ini_GetBoolean:
	If the itemName is NOT found or has an empty value, returns FALSE.
	If itemName IS found and has a valid ASCII representation of a boolean,
	sets *value to TRUE or FALSE, and returns TRUE.
	If the value is not a valid ASCII representation of a boolean,
	or any other error occurs, returns a negative error code.
*/
int CVIFUNC Ini_GetBoolean(IniText src, const char *section, const char *itemName, int *boolValue)
{
	char     *temp;
	int     error = UIENoError;
	int     found;

	errChk (found = Ini_Get(src,section,itemName, &temp));
	found = found && HasNonWhiteSpace (temp);
	if (found)
		{
		if (!StrICmp (temp, kBooleanTrueValueString))
			*boolValue = TRUE;
		else if (!StrICmp (temp, kBooleanFalseValueString))
			*boolValue = FALSE;
		else
			errChk (ToolErr_InvalidBooleanValue);
		}

Error:
	if (error < 0)
		return error;
	else
		return found;
}

/*********************************************************************/

int CVIFUNC Ini_ItemOrMultiLineItemExists(IniText theIniText, const char *section, const char *itemName)
{
	return Ini_ItemExists(theIniText, section, itemName);
}

/*********************************************************************/

int CVIFUNC Ini_ItemExists(IniText theIniText, const char *section, const char *itemName)
{
	/******************************************************************/
	/* NOTE:  This function only indicates that there was a tag,      */
	/*        not that there was a value.  Functions like             */
	/*        Ini_GetInt may return FALSE (not found) even though    */
	/*        this function returns TRUE.                             */
	/******************************************************************/

	return Ini_Get(theIniText, section, itemName, 0);
}

/*********************************************************************/

static int EscapeText (const char *srcTextPtr, char **destTextPtr)
{
	int         error = UIENoError;
	const unsigned char  *s = (unsigned char *) srcTextPtr;
	char        *d;
	int         srcLen = 0;
	char        buffer[3];
	int         i;

	if( !destTextPtr )
		return UIENullPointerPassed;

	*destTextPtr = NULL;
	srcLen = (int)strlen (srcTextPtr);

	/***********************************************************************/
	/* since a single character can become as large as \xHH, we want the   */
	/* destination string to be 4 times as large as the original plus NULL */
	/***********************************************************************/
	nullChk (*destTextPtr = malloc (4*srcLen + 1));
	d = *destTextPtr;
	while (*s)
		{
		if (CmbIsLeadByte(*s))
			{
			*d++ = *s++;
			*d++ = *s++;
			continue;
			}
		switch(*s)
			{
			case '\r':
				*d++ = '\\';
				*d++ = 'r';
				break;
			case '\n':
				*d++ = '\\';
				*d++ = 'n';
				break;
			case '\t':
				*d++ = '\\';
				*d++ = 't';
				break;
			case '"':
				*d++ = '\\';
				*d++ = '"';
				break;
			case '\\':
				*d++ = '\\';
				*d++ = '\\';
				break;
			default:
				if (iscntrl((unsigned char)*s))
					{
					*d++ = '\\';
					*d++ = 'x';
					sprintf (buffer, "%02x", *s);
					for (i=0; buffer[i]; i++)
						*d++ = buffer[i];
					}
				else
					*d++ = *s;
				break;
			}
		s++;
		}

	*d = '\0';

Error:
	return error;
}

/***************************************************************************/
/*
   UnescapeText expects the source string to come from EscapeText, and as
   expects there will be no '"', only '\"'.
*/
static int UnescapeText (const char *srcTextPtr, char *destTextPtr)
{
	int			error = UIENoError;
	const char	*s = srcTextPtr;
	char		*d = destTextPtr;
	int			hexVal1, hexVal2, slash = FALSE;

	while (*s)
		{
		if (CmbIsLeadByte(*s))
			{
			*d++ = *s++;
			*d++ = *s++;
			continue;
			}
		if (slash)
			{
			switch(*s)
				{
				case 'r':
					*d++ = '\r';
					break;
				case 'n':
					*d++ = '\n';
					break;
				case 't':
					*d++ = '\t';
					break;
				case 'x':
					hexVal1 = GetHexCharValue((unsigned char)*++s);
					hexVal2 = GetHexCharValue((unsigned char)*++s);
					*d++ = 16 * hexVal1 + hexVal2;
					break;
				default:
					*d++ = *s;
					break;
				}
			slash = FALSE;
			}
		else
			{
			if (*s == '\\')
				slash = TRUE;       /* hasEscapeText = TRUE */
			else
				*d++ = *s;
			}
		s++;
		}

	*d = '\0';

/*Error:*/
	return error;
}

/**************************************************************************/

static int GetHexCharValue (int hexChar)
{
	switch (hexChar)
		{
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'a':
		case 'A':
			return 10;
		case 'b':
		case 'B':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'd':
		case 'D':
			return 13;
		case 'e':
		case 'E':
			return 14;
		case 'f':
		case 'F':
			return 15;
		default:
		   return -1;
		}
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

#define kLineFileReadBufSize   2048

struct LineFileForwardReferenceTag
	{
	void  *inputSource;
	LineFileReadFunc inputFunc;
	char  *fileBuf;
	int   numBytesLeftInBuf;
	char  *currBufLoc;
	int   numLinesRead;
	int   numBytesRead;
	int   prevLineTermChar;
	int   offsetOfLastLine;    /* byte offset within file of the line most recently read */
	int   numExtraLineTermChars;   /* number of extra line term chars thrown away at beginning of most recently read line */
	int   numCharsInEmptyLines;    /* includes whitespace chars and line term chars in empty lines thrown away by LineFileReadNonEmpty */
	};


static void     LineFile_ResetStateVariables (LineFile lineFile);
static int      LineFile_RefillBuf(LineFile lineFile);

/*************************************************************************************************/

	/*  Creates a new LineFile object which is useful for line oriented file io.
		file should be an open file.
	*/
LineFile CVIFUNC LineFile_New(LineFileReadFunc inputFunc, void *inputSource)
{
	LineFile newLineFile = NULL;
#if defined(_NI_mswin_) && _NI_mswin_
	int 	error = UIENoError,tracking = SetEnableResourceTracking (0);
#else
	int 	error = UIENoError;
#endif

	nullChk(newLineFile = calloc(sizeof(*newLineFile), 1));
	nullChk(newLineFile->fileBuf = malloc(kLineFileReadBufSize));
	newLineFile->inputSource = inputSource;
	if (!inputFunc)
		inputFunc = LineFile_ReadFromFile;      /* default to reading from a file */

	newLineFile->inputFunc = inputFunc;
	LineFile_ResetStateVariables(newLineFile);

Error:
#if defined(_NI_mswin_) && _NI_mswin_
	SetEnableResourceTracking (tracking);
#endif
	if (error < 0)
		{
		LineFile_Dispose(newLineFile);
		newLineFile = NULL;
		}
#if HAVE_CVI_RTE && defined (_NI_mswin_) && _NI_mswin_
	else
		__CVI_Resource_Acquire (newLineFile, INIFILE_RESOURCE_TYPE,
			INIFILE_FORMAT_LINEFILE, newLineFile);
#endif
	return newLineFile;
}

/*******************************************************************************/

static void LineFile_ResetStateVariables (LineFile lineFile)
{
	if( lineFile ) {
		lineFile->currBufLoc = lineFile->fileBuf;
		lineFile->numBytesLeftInBuf = 0;
		lineFile->prevLineTermChar = 0;
		lineFile->numLinesRead = 0;
		lineFile->numBytesRead = 0;
		lineFile->offsetOfLastLine = -1L;   /* no line has yet been read */
		lineFile->numExtraLineTermChars = 0;
		lineFile->numCharsInEmptyLines = 0;
	}
}

/*******************************************************************************/

	/*  Disposes a LineFile object, does not close the file.
	*/
void CVIFUNC LineFile_Dispose(LineFile lineFile)
{
	if (lineFile)
		{
#if HAVE_CVI_RTE && defined (_NI_mswin_) && _NI_mswin_
		__CVI_Resource_Release (lineFile);
#endif
		free(lineFile->fileBuf);
		free(lineFile);
		}
}

/*******************************************************************************/

	/*  Reads a line from a LineFile object into destbuf.
		maxNumBytes is the max number of bytes to read into destBuf (excluding the NULL byte).
		NULL may be passed for bytesRead if the value isn't needed
		NOTE:  if *endOfFile==TRUE, then *bytesRead==0.
	*/
int CVIFUNC LineFile_Read (LineFile lineFile, char *destBuf, size_t maxNumBytes,
						  ssize_t *bytesRead, int *endOfFile)
{
	int    endOfLine = FALSE;
	int    firstByteForLine = TRUE;
	size_t tempBytesRead = 0;
	int    ch;
	int    error = UIENoError;

	if( !lineFile || !endOfFile )
		return UIENullPointerPassed;

	tempBytesRead = 0;
	*endOfFile = FALSE;

	lineFile->offsetOfLastLine = lineFile->numBytesRead - lineFile->numBytesLeftInBuf;
	lineFile->numExtraLineTermChars = 0;

	while (!*endOfFile && !endOfLine)
		{
		if (lineFile->numBytesLeftInBuf == 0)
			errChk(LineFile_RefillBuf(lineFile));

		if (lineFile->numBytesLeftInBuf)
			{
			ch = *lineFile->currBufLoc;
			if ((ch == '\n') || (ch == '\r') || (ch == '\0'))
				{
				if (firstByteForLine && (ch != lineFile->prevLineTermChar))
					{
					lineFile->offsetOfLastLine++;
					lineFile->numExtraLineTermChars++;
					}
				else
					{
					lineFile->prevLineTermChar = ch;
					endOfLine = TRUE;
					}
				}
			else
				{
				if (tempBytesRead < maxNumBytes)
					{
					destBuf[tempBytesRead] = ch;        /* fill user buffer if not full yet */
					tempBytesRead++;
					}
				}

			lineFile->numBytesLeftInBuf--;
			lineFile->currBufLoc++;
			firstByteForLine = FALSE;
			}
		else
			{
			if (tempBytesRead > 0)
				endOfLine = TRUE;    /* to handle case where last line does not have a newline or carriage return */
			else
				*endOfFile = TRUE;
			}
		}

Error:
	destBuf[tempBytesRead] = 0;
	if (bytesRead)
		*bytesRead = (ssize_t)tempBytesRead;

	if (endOfLine)
		lineFile->numLinesRead++;

	return error;
}

/*********************************************************************/

	/*  Exactly like LineFile_Read(), except it skips empty lines.
		Either *endOfFile==TRUE && *bytesRead==0
			or *endOfFile==FALSE && *bytesRead > 0
	*/
int CVIFUNC LineFile_ReadNonEmpty (LineFile lineFile, char *destBuf, size_t maxNumBytes,
								  ssize_t *bytesRead, int *endOfFile)
{
	int   numCharsInEmptyLines=0;
	int   error = UIENoError;

	if( !lineFile )
		return UIENullPointerPassed;

	for (;;)
		{
		errChk(LineFile_Read(lineFile, destBuf, maxNumBytes, bytesRead, endOfFile));
		if (*endOfFile || HasNonWhiteSpace(destBuf))
			break;
		numCharsInEmptyLines += (int)strlen(destBuf) + lineFile->numExtraLineTermChars + 1;   /* add 1 for the regular line term char */
		}

Error:
	lineFile->numCharsInEmptyLines = numCharsInEmptyLines;
	return error;
}

/*******************************************************************************/

int CVIFUNC LineFile_NumLinesRead (LineFile lineFile)
{
	return lineFile ? lineFile->numLinesRead : 0;
}

/*******************************************************************************/

int CVIFUNC LineFile_OffsetOfLastLine (LineFile lineFile)
{
	return lineFile ? lineFile->offsetOfLastLine : 0;
}

/*******************************************************************************/

int CVIFUNC LineFile_NumExtraLineTermChars (LineFile lineFile)
{
	return lineFile ? lineFile->numExtraLineTermChars : 0;
}

/*******************************************************************************/

int CVIFUNC LineFile_NumCharsInEmptyLines (LineFile lineFile)
{
	return lineFile ? lineFile->numCharsInEmptyLines : 0;
}

/*******************************************************************************/

	/*  Refills the contents of a LineFile objects internal IO buffer if it has no more
		unused characters.
	*/
static int LineFile_RefillBuf(LineFile lineFile)
{
	int   numRead;
	int   error = UIENoError;

	if( !lineFile )
		return UIENullPointerPassed;

	if (lineFile->numBytesLeftInBuf == 0)
		{
		errChk( lineFile->inputFunc(lineFile->inputSource, kLineFileReadBufSize, &numRead, lineFile->fileBuf));

		lineFile->currBufLoc = lineFile->fileBuf;
		lineFile->numBytesLeftInBuf = numRead;
		lineFile->numBytesRead += numRead;
		}

Error:
	return error;
}

/*******************************************************************************/

	/*  Can be passed to LineFile_New() to read lines from a FILE * */
int CVICALLBACK LineFile_ReadFromFile(void *inputSource, int numBytesToRead, int *numBytesRead, void *dest)
{
	if( numBytesRead )
		*numBytesRead = (int)fread(dest, 1, numBytesToRead, (FILE *)inputSource);

	if (ferror ((FILE *)inputSource))
		return ToolErr_ErrorReadingFile;
	else
		return 0;
}

/*******************************************************************************/

#define kNameValueInputSeparatorToken   "="
#define kSectionNameStartChar           '['
#define kSectionNameEndChar             ']'
#define kCommentMarker                  ";"

static void Ini_SetDefaultTokens(IniText iniText)
{
	Ini_SetTokens(iniText, kSectionNameStartChar, kSectionNameEndChar, kNameValueInputSeparatorToken, kCommentMarker);
}

/*******************************************************************************/

int CVIFUNC Ini_SetTokens(IniText iniText, char sectionNameStartCharacter, char sectionNameEndCharacter, const char *valueSeparator, const char *commentMarker)
{
	if( !iniText )
		return UIENullPointerPassed;

	if (sectionNameStartCharacter)
		iniText->sectionNameStartChar = sectionNameStartCharacter;

	if (sectionNameEndCharacter)
		iniText->sectionNameEndChar = sectionNameEndCharacter;

	if (valueSeparator && valueSeparator[0])
		{
		StringCopyMax(iniText->nameValueInputSeparatorToken, (char *)valueSeparator, kMaxSpecialTokenLength);
		sprintf(iniText->nameValueOutputSeparatorToken, " %s ", iniText->nameValueInputSeparatorToken);
		iniText->nameValueInputSeparatorLength = (int)strlen(iniText->nameValueInputSeparatorToken);
		iniText->nameValueOutputSeparatorLength = (int)strlen(iniText->nameValueOutputSeparatorToken);
		}

	if (commentMarker && commentMarker[0])
		StringCopyMax(iniText->commentMarker, (char *)commentMarker, kMaxSpecialTokenLength);

	return 0;
}

/*******************************************************************************/

int CVIFUNC Ini_SetSectionFilter(IniText iniText, Ini_SectionFilterFunc sectionFilter, void *callbackData)
{
	if( !iniText )
		return UIENullPointerPassed;

	iniText->sectionFilter = sectionFilter;
	iniText->sectionFilterCallbackData = callbackData;
	return 0;
}

/*******************************************************************************/

int CVIFUNC Ini_SetDuplicateChecking(IniText iniText, int checkForDuplicates)
{
	int	oldState;

	if( !iniText )
		return UIENullPointerPassed;

	oldState = iniText->checkForDuplicatesWhenPutting;
	iniText->checkForDuplicatesWhenPutting = (checkForDuplicates != 0);

	return oldState;
}

/*******************************************************************************/

int CVIFUNC Ini_SetAddQuotesToStrings(IniText iniText, int addQuotesToStrings)
{
	int	oldState;

	if( !iniText )
		return UIENullPointerPassed;

	oldState = iniText->addQuotesToStrings;
	iniText->addQuotesToStrings = (addQuotesToStrings != 0);

	return oldState;
}

/*******************************************************************************/

int CVIFUNC Ini_SetMaxValueLineLength(IniText iniText, int maxValueLineLength)
{
	int	oldState;

	if( !iniText )
		return UIENullPointerPassed;
	if(maxValueLineLength < 0 || maxValueLineLength > kMaxIniLineSize)
		return UIEValueIsInvalidOrOutOfRange;

	oldState = iniText->maxValueLineLength;
	iniText->maxValueLineLength = maxValueLineLength;

	return oldState;
}

/*******************************************************************************/

int CVIFUNC Ini_PutData(IniText iniText, const char *section, const char *itemName, const unsigned char *data, long dataSize)
{
	unsigned char   *textPtr = 0;
	int             error = UIENoError;
	int             result = 0;

	nullChk( ConvertDataToText((unsigned char *)data, dataSize, &textPtr));
	errChk( result = Ini_PutRawString(iniText, section, itemName, (const char *)textPtr));

Error:
	free(textPtr);
	if (error < 0)
		return error;
	else
		return result;
}

/*******************************************************************************/

int CVIFUNC Ini_GetData(IniText iniText, const char *section, const char *itemName, unsigned char **data, size_t *dataSize)
{
	int             error = UIENoError;
	unsigned char   *rawString;
	int             found;
	size_t			size = 0;

	if( !data )
		return UIENullPointerPassed;

	*data = 0;
	errChk(found = Ini_GetPointerToRawString(iniText, section, itemName, (char **)&rawString));
	if (found)
		nullChk( ConvertTextToData(rawString, &size, (unsigned char **)data));

Error:
	if (dataSize)
		*dataSize = size;

	if (error < 0)
		{
		free(*data);
		return error;
		}
	else
		return found;
}

/*******************************************************************************/

int CVIFUNC Ini_CopySection(IniText source, const char sectionToCopy[], IniText destination, int overwriteDuplicateSections)
{
	int     sectionIndex;
	int     numSectionsToMerge;
	char    *sectionName;
	int     itemIndex;
	int     numItemsToMerge;
	char    *itemName;
	char    *valuePtr;
	int     error = UIENoError;

	if (source == destination)
		goto Error;

	if( !source || !destination )
		return UIENullPointerPassed;

	numSectionsToMerge  = Ini_NumberOfSections(source);

	for (sectionIndex = 1; sectionIndex <= numSectionsToMerge; sectionIndex++)
		{
		Ini_NthSectionName (source, sectionIndex, &sectionName);

		if ((sectionToCopy == 0) || (StrICmpWithoutSurrWhiteSpace((char *)sectionToCopy, sectionName) == 0))
			{
			if (overwriteDuplicateSections && Ini_SectionExists (destination, sectionName))
				Ini_RemoveSection (destination, sectionName);

			numItemsToMerge = Ini_NumberOfItems (source, sectionName);
			for (itemIndex = 1; itemIndex <= numItemsToMerge; itemIndex++)
				{
				Ini_NthItemName (source, sectionName, itemIndex, &itemName);

				errChk( Ini_GetPointerToRawString(source, sectionName, itemName, &valuePtr));
				errChk( Ini_PutRawString(destination, sectionName, itemName, valuePtr));
				}   /* for item */
			} /* if */
		}   /* for section */

Error:
	if (error < 0)
		return error;
	else
		return 0;
}

/*******************************************************************************/

int CVIFUNC Ini_CopySectionEx(IniText source, const char sectionToCopy[], IniText destination, const char destinationSectionToCopyTo[], int overwriteDuplicateSection)
{
	int     sectionIndex=1;
	int     numSectionsToMerge;
	char    *sectionName;
	int     itemIndex;
	int     numItemsToMerge;
	char    *itemName;
	char    *valuePtr;
	int     error=UIENoError;

	if( !source || !destination || !sectionToCopy )
		return UIENullPointerPassed;

	numSectionsToMerge = Ini_NumberOfSections(source);

	while( sectionIndex<=numSectionsToMerge ) {
		Ini_NthSectionName(source,sectionIndex++,&sectionName);
		if( StrICmpWithoutSurrWhiteSpace(sectionToCopy,sectionName)==0 ) {
			if( overwriteDuplicateSection && Ini_SectionExists(destination,destinationSectionToCopyTo) )
				Ini_RemoveSection(destination,destinationSectionToCopyTo);
			numItemsToMerge = Ini_NumberOfItems(source,sectionName);
			for(itemIndex=1;itemIndex<=numItemsToMerge;++itemIndex) {
				Ini_NthItemName(source,sectionName,itemIndex,&itemName);
				errChk (Ini_GetPointerToRawString(source,sectionName,itemName,&valuePtr));
				errChk (Ini_PutRawString(destination,destinationSectionToCopyTo,itemName,valuePtr));
			}
			return 0;
		}
	}

Error:
	return error<0?error:0;
}

/*******************************************************************************/

	/*
		whichLine and singleLineItemName are only set if isMultiLine is set to TRUE.
		Each parameter is optional and can be NULL.
	*/
static void Ini_AnalyzeItemName(const char *itemName, int *isMultiLine, int *whichLine, char singleLineItemName[])
{
	unsigned char	*lineTagPtr, *numPtr, *tempNumPtr;
	int				lineNum;

	if (isMultiLine)
		*isMultiLine = FALSE;

	lineTagPtr = CmbStrStr ((unsigned char *)itemName, (unsigned char *)kLineTag);
	if (!lineTagPtr)
		return;

	numPtr = tempNumPtr = lineTagPtr + strlen(kLineTag);

	if (!isdigit(*tempNumPtr))  /* line tag must be followed by a positive integer number */
		return;

	while (*tempNumPtr)
		{
		if (!isdigit(*tempNumPtr))  /* line tag must not be followed by anything but a number */
			return;
		tempNumPtr++;
		}

	errno = 0;
	lineNum = strtol ((char *)numPtr, NULL, 10);
	if (errno == ERANGE)
		return;

	if (isMultiLine)
		*isMultiLine = TRUE;

	if (whichLine)
		*whichLine = lineNum;

	if (singleLineItemName)
		StringCopyMax(singleLineItemName, itemName, (char *)lineTagPtr - itemName + 1);
}

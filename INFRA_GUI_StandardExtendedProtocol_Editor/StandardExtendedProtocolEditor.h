/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: panelCB */
#define  PANEL_COMMAND_NAME_LIST          2       /* control type: listBox, callback function: clbCommandsList */
#define  PANEL_REC_DATA_VAL_STR           3       /* control type: string, callback function: clbValueChanged */
#define  PANEL_REC_DATA_SIZE_STR          4       /* control type: string, callback function: clbValueChanged */
#define  PANEL_REC_HEADER_STR             5       /* control type: string, callback function: clbValueChanged */
#define  PANEL_REC_OP_CODE_STR            6       /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_DATA_VAL_STR          7       /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_DATA_SIZE_STR         8       /* control type: string, callback function: clbValueChanged */
#define  PANEL_DECORATION_2               9       /* control type: deco, callback function: (none) */
#define  PANEL_SEND_HEADER_STR            10      /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_OP_CODE_STR           11      /* control type: string, callback function: clbValueChanged */
#define  PANEL_COMMAND_NAME_STRING        12      /* control type: string, callback function: clbCommandNameChanged */
#define  PANEL_EXIT_BUTTON                13      /* control type: command, callback function: clbExit */
#define  PANEL_DECORATION                 14      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG                    15      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_2                  16      /* control type: textMsg, callback function: (none) */
#define  PANEL_REC_VAR_SIZE               17      /* control type: ring, callback function: clbValueNumericChanged */
#define  PANEL_SEND_VAR_SIZE              18      /* control type: ring, callback function: clbValueNumericChanged */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_FILE                     2
#define  MENUBAR_FILE_NEW                 3       /* callback function: clbMenuNewFile */
#define  MENUBAR_FILE_OPEN                4       /* callback function: clbMenuOpenFile */
#define  MENUBAR_FILE_SAVE                5       /* callback function: clbMenuSaveFile */
#define  MENUBAR_FILE_SEPARATOR           6
#define  MENUBAR_FILE_EXIT                7       /* callback function: clbMenuExit */
#define  MENUBAR_EDIT                     8
#define  MENUBAR_EDIT_SORT_BY_OPCODE      9       /* callback function: clbSortByOpCode */


     /* Callback Prototypes: */

int  CVICALLBACK clbCommandNameChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbCommandsList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK clbMenuExit(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuNewFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuOpenFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuSaveFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbSortByOpCode(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK clbValueChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbValueNumericChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

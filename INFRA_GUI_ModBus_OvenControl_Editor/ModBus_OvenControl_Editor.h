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
#define  PANEL_RECEIVE_NUM_OF             3       /* control type: string, callback function: clbValueChanged */
#define  PANEL_RECEIVE_ADDRESS            4       /* control type: string, callback function: clbValueChanged */
#define  PANEL_RECEIVE_DEVICE             5       /* control type: string, callback function: clbValueChanged */
#define  PANEL_RECEIVE_FUNC               6       /* control type: string, callback function: clbValueChanged */
#define  PANEL_RECEIVE_DATA_MUL           7       /* control type: string, callback function: clbValueChanged */
#define  PANEL_RECEIVE_DATA               8       /* control type: string, callback function: clbValueChanged */
#define  PANEL_RECEIVE_DATA_SIZE          9       /* control type: string, callback function: clbValueChanged */
#define  PANEL_RECEIVE_DATA_LEN           10      /* control type: string, callback function: clbValueChanged */
#define  PANEL_REC_PREFIX_ID              11      /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_PREFIX_ID             12      /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_DATA_MUL              13      /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_DATA                  14      /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_DATA_SIZE             15      /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_DATA_LEN              16      /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_NUM_OF                17      /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_ADDRESS               18      /* control type: string, callback function: clbValueChanged */
#define  PANEL_DECORATION_2               19      /* control type: deco, callback function: (none) */
#define  PANEL_SEND_DEVICE                20      /* control type: string, callback function: clbValueChanged */
#define  PANEL_SEND_FUNC                  21      /* control type: string, callback function: clbValueChanged */
#define  PANEL_COMMAND_NAME_STRING        22      /* control type: string, callback function: clbCommandNameChanged */
#define  PANEL_EXIT_BUTTON                23      /* control type: command, callback function: clbExit */
#define  PANEL_DECORATION                 24      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG                    25      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_2                  26      /* control type: textMsg, callback function: (none) */
#define  PANEL_RECEIVE_TYPE               27      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_RECEIVE_CRC                28      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_RES_SWAP                   29      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_SEND_SWAP                  30      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_RES_BYTE_ORDER             31      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_SEND_BYTE_ORDER            32      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_SEND_CRC                   33      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_RECEIVE_VAL_TYPE           34      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_RECEIVE_PREFIX             35      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_SEND_PREFIX                36      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_SEND_VAL_TYPE              37      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_SEND_TYPE                  38      /* control type: ring, callback function: clbValueChanged */
#define  PANEL_RES_PIC                    39      /* control type: pictRing, callback function: (none) */
#define  PANEL_SEND_PIC                   40      /* control type: pictRing, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_FILE                     2
#define  MENUBAR_FILE_NEW                 3       /* callback function: clbMenuNewFile */
#define  MENUBAR_FILE_OPEN                4       /* callback function: clbMenuOpenFile */
#define  MENUBAR_FILE_SAVE                5       /* callback function: clbMenuSaveFile */
#define  MENUBAR_FILE_SAVE_AS             6       /* callback function: clbMenuSaveAsFile */
#define  MENUBAR_FILE_SEPARATOR           7
#define  MENUBAR_FILE_EXIT                8       /* callback function: clbMenuExit */
#define  MENUBAR_EDIT                     9
#define  MENUBAR_EDIT_DELETE              10      /* callback function: clbMenuDeleteItem */


     /* Callback Prototypes: */

int  CVICALLBACK clbCommandNameChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbCommandsList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK clbMenuDeleteItem(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuExit(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuNewFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuOpenFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuSaveAsFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuSaveFile(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK clbValueChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

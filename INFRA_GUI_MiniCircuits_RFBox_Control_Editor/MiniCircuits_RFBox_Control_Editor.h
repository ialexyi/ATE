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
#define  PANEL_DELAY                      2       /* control type: string, callback function: clbValueChanged */
#define  PANEL_TIMEOUT                    3       /* control type: string, callback function: clbValueChanged */
#define  PANEL_STATE_OFF_VAL              4       /* control type: string, callback function: clbValueChanged */
#define  PANEL_STATE_ON_VAL               5       /* control type: string, callback function: clbValueChanged */
#define  PANEL_SWITCH_TYPE                6       /* control type: ring, callback function: clbValueChanged */
#define  PANEL_VALUE_MODE                 7       /* control type: ring, callback function: clbValueChanged */
#define  PANEL_SLOT                       8       /* control type: ring, callback function: clbValueChanged */
#define  PANEL_COMMAND_NAME_LIST          9       /* control type: listBox, callback function: clbCommandsList */
#define  PANEL_COMMAND_NAME_STRING        10      /* control type: string, callback function: clbCommandNameChanged */
#define  PANEL_EXIT_BUTTON                11      /* control type: command, callback function: clbExit */


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


     /* Callback Prototypes: */

int  CVICALLBACK clbCommandNameChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbCommandsList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK clbMenuExit(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuNewFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuOpenFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuSaveFile(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK clbValueChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

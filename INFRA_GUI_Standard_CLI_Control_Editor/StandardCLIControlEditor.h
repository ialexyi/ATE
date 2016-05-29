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
#define  PANEL_DECORATION_2               3       /* control type: deco, callback function: (none) */
#define  PANEL_RESPONDS                   4       /* control type: string, callback function: clbRespondsChanged */
#define  PANEL_SEND_COMMAND_STR           5       /* control type: string, callback function: clbCommandChanged */
#define  PANEL_COMMAND_NAME_STRING        6       /* control type: string, callback function: clbCommandNameChanged */
#define  PANEL_EXIT_BUTTON                7       /* control type: command, callback function: clbExit */
#define  PANEL_DECORATION                 8       /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG                    9       /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_2                  10      /* control type: textMsg, callback function: (none) */
#define  PANEL_RECEIVE_PARAM              11      /* control type: ring, callback function: clbReceiveParamsChanged */
#define  PANEL_RECEIVE_TAB                12      /* control type: tab, callback function: (none) */
#define  PANEL_SEND_TAB                   13      /* control type: tab, callback function: (none) */
#define  PANEL_SEND_PARAM                 14      /* control type: ring, callback function: clbSendParamsChanged */

     /* tab page panel controls */
#define  REC_PAR_1_TYPE                   2       /* control type: ring, callback function: clbValueChanged */
#define  REC_PAR_1_STOP                   3       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_1_START                  4       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_1_NAME                   5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  REC_PAR_2_TYPE                   2       /* control type: ring, callback function: clbValueChanged */
#define  REC_PAR_2_STOP                   3       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_2_START                  4       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_2_NAME                   5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  REC_PAR_3_TYPE                   2       /* control type: ring, callback function: clbValueChanged */
#define  REC_PAR_3_STOP                   3       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_3_START                  4       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_3_NAME                   5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  REC_PAR_4_TYPE                   2       /* control type: ring, callback function: clbValueChanged */
#define  REC_PAR_4_STOP                   3       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_4_START                  4       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_4_NAME                   5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  REC_PAR_5_TYPE                   2       /* control type: ring, callback function: clbValueChanged */
#define  REC_PAR_5_STOP                   3       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_5_START                  4       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_5_NAME                   5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  REC_PAR_6_TYPE                   2       /* control type: ring, callback function: clbValueChanged */
#define  REC_PAR_6_STOP                   3       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_6_START                  4       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_6_NAME                   5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  REC_PAR_7_TYPE                   2       /* control type: ring, callback function: clbValueChanged */
#define  REC_PAR_7_STOP                   3       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_7_START                  4       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_7_NAME                   5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  REC_PAR_8_TYPE                   2       /* control type: ring, callback function: clbValueChanged */
#define  REC_PAR_8_STOP                   3       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_8_START                  4       /* control type: string, callback function: clbValueChanged */
#define  REC_PAR_8_NAME                   5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  SEND_PAR_1_TYPE                  2       /* control type: ring, callback function: clbValueChanged */
#define  SEND_PAR_1_FORMAT                3       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_1_DEFAULT               4       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_1_NAME                  5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  SEND_PAR_2_TYPE                  2       /* control type: ring, callback function: clbValueChanged */
#define  SEND_PAR_2_FORMAT                3       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_2_DEFAULT               4       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_2_NAME                  5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  SEND_PAR_3_TYPE                  2       /* control type: ring, callback function: clbValueChanged */
#define  SEND_PAR_3_FORMAT                3       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_3_DEFAULT               4       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_3_NAME                  5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  SEND_PAR_4_TYPE                  2       /* control type: ring, callback function: clbValueChanged */
#define  SEND_PAR_4_FORMAT                3       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_4_DEFAULT               4       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_4_NAME                  5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  SEND_PAR_5_TYPE                  2       /* control type: ring, callback function: clbValueChanged */
#define  SEND_PAR_5_FORMAT                3       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_5_DEFAULT               4       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_5_NAME                  5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  SEND_PAR_6_TYPE                  2       /* control type: ring, callback function: clbValueChanged */
#define  SEND_PAR_6_FORMAT                3       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_6_DEFAULT               4       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_6_NAME                  5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  SEND_PAR_7_TYPE                  2       /* control type: ring, callback function: clbValueChanged */
#define  SEND_PAR_7_FORMAT                3       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_7_DEFAULT               4       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_7_NAME                  5       /* control type: string, callback function: clbValueChanged */

     /* tab page panel controls */
#define  SEND_PAR_8_TYPE                  2       /* control type: ring, callback function: clbValueChanged */
#define  SEND_PAR_8_FORMAT                3       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_8_DEFAULT               4       /* control type: string, callback function: clbValueChanged */
#define  SEND_PAR_8_NAME                  5       /* control type: string, callback function: clbValueChanged */


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

int  CVICALLBACK clbCommandChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbCommandNameChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbCommandsList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK clbMenuExit(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuNewFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuOpenFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuSaveFile(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK clbReceiveParamsChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbRespondsChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbSendParamsChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbValueChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

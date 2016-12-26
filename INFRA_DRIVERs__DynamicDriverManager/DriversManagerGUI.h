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

#define  FILES                            1
#define  FILES_DIR                        2       /* control type: ring, callback function: (none) */
#define  FILES_PATH                       3       /* control type: string, callback function: (none) */
#define  FILES_SAVE                       4       /* control type: pictButton, callback function: (none) */
#define  FILES_LOAD                       5       /* control type: pictButton, callback function: (none) */
#define  FILES_DELETE                     6       /* control type: pictButton, callback function: (none) */
#define  FILES_UNLINK                     7       /* control type: pictButton, callback function: filesUnlink */
#define  FILES_PICTURE                    8       /* control type: picture, callback function: (none) */
#define  FILES_LINK                       9       /* control type: string, callback function: (none) */
#define  FILES_QUIT                       10      /* control type: pictButton, callback function: (none) */
#define  FILES_LIST                       11      /* control type: listBox, callback function: clbListTrigger */

#define  HELP_ABOUT                       2
#define  HELP_ABOUT_TEXTMSG               2       /* control type: textMsg, callback function: clbAboutPanleContuctWeb */
#define  HELP_ABOUT_QUIT                  3       /* control type: pictButton, callback function: clbAboutPanleQuit */
#define  HELP_ABOUT_PICTURE               4       /* control type: picture, callback function: clbAboutPanleContuctUs */
#define  HELP_ABOUT_PICTURE_3             5       /* control type: picture, callback function: (none) */
#define  HELP_ABOUT_DATE                  6       /* control type: textMsg, callback function: (none) */
#define  HELP_ABOUT_VERSION               7       /* control type: textMsg, callback function: (none) */

#define  SETUP_MSG                        3
#define  SETUP_MSG_YES                    2       /* control type: pictButton, callback function: (none) */
#define  SETUP_MSG_NO                     3       /* control type: pictButton, callback function: (none) */
#define  SETUP_MSG_PICTURE_2              4       /* control type: picture, callback function: (none) */
#define  SETUP_MSG_CONTINUE               5       /* control type: pictButton, callback function: (none) */
#define  SETUP_MSG_INSTRACTION_TEXT       6       /* control type: textBox, callback function: (none) */
#define  SETUP_MSG_INSTRACTION_PICTURE    7       /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK clbAboutPanleContuctUs(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbAboutPanleContuctWeb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbAboutPanleQuit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbListTrigger(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK filesUnlink(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

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
#define  PANEL_COMMAND_NAME_STRING        3       /* control type: string, callback function: clbCommandNameChanged */
#define  PANEL_EXIT_BUTTON                4       /* control type: command, callback function: clbExit */
#define  PANEL_TAB                        5       /* control type: tab, callback function: clbChangeTab */

     /* tab page panel controls */
#define  DEFAULT_BOUD_RATE                2       /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_BIT_DIR_MASK             3       /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_GET_DATA_SHIFT           4       /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_SET_DATA_SHIFT           5       /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_SET_TIMEOUT              6       /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_SET_DELAY                7       /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_POWER_UP_LEVEL           8       /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_VALUE_COUNT              9       /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_DEFAULT_VALUE            10      /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_USE_BITS_MAP             11      /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_PORTS                    12      /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_PULLING_TYPE             13      /* control type: ring, callback function: clbValueChanged */
#define  DEFAULT_DEVICE_NAME              14      /* control type: string, callback function: clbValueChanged */
#define  DEFAULT_CONFIG_TYPE              15      /* control type: ring, callback function: clbValueChanged */
#define  DEFAULT_INTERFACE_TYPE           16      /* control type: ring, callback function: clbValueChanged */
#define  DEFAULT_MEASURE_TYPE             17      /* control type: ring, callback function: clbValueChanged */
#define  DEFAULT_PILLING_STATE            18      /* control type: radioButton, callback function: clbValueChanged */
#define  DEFAULT_VALUE_TYPE               19      /* control type: ring, callback function: clbValueChanged */

     /* tab page panel controls */
#define  MEAS_CONF_NUMBER_OF_POINTS       2       /* control type: string, callback function: clbMeasureConfigValueChanged */
#define  MEAS_CONF_SIMPLE_RATE            3       /* control type: string, callback function: clbMeasureConfigValueChanged */
#define  MEAS_CONF_MEASURE_FUNCTION       4       /* control type: ring, callback function: clbMeasureConfigValueChanged */

     /* tab page panel controls */
#define  SPI_DOUBLE_ENABLE_PULSE          2       /* control type: ring, callback function: clbSpiConfigValueChanged */
#define  SPI_READ_BIT_PORT                3       /* control type: ring, callback function: clbSpiConfigValueChanged */
#define  SPI_ENABLE_BIT_POLARITY          4       /* control type: ring, callback function: clbSpiConfigValueChanged */
#define  SPI_READ_BIT_INDEX               5       /* control type: ring, callback function: clbSpiConfigValueChanged */
#define  SPI_CLOCK_BIT_PHASE              6       /* control type: ring, callback function: clbSpiConfigValueChanged */
#define  SPI_CLOCK_BIT_POLARITY           7       /* control type: ring, callback function: clbSpiConfigValueChanged */
#define  SPI_ENABLE_BIT_INDEX             8       /* control type: ring, callback function: clbSpiConfigValueChanged */
#define  SPI_DATA_BIT_INDEX               9       /* control type: ring, callback function: clbSpiConfigValueChanged */
#define  SPI_CLOCK_BIT_INDEX              10      /* control type: ring, callback function: clbSpiConfigValueChanged */
#define  SPI_WRITE_BITS_COUNT             11      /* control type: string, callback function: clbSpiConfigValueChanged */
#define  SPI_READ_BITS_COUNT              12      /* control type: string, callback function: clbSpiConfigValueChanged */


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

int  CVICALLBACK clbChangeTab(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbCommandNameChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbCommandsList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbMeasureConfigValueChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK clbMenuExit(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuNewFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuOpenFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK clbMenuSaveFile(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK clbSpiConfigValueChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK clbValueChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

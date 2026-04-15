/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

#ifndef PACOMOUSECYD_CFG_H
#define PACOMOUSECYD_CFG_H

////////////////////////////////////////////////////////////
// ***** USER OPTIONS *****
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// ***** CHEAP YELLOW DISPLAY SELECTION *****
////////////////////////////////////////////////////////////

#define CYD_TFT_28        0                                       // Cheap Yellow Display 2.8" (CYD 2432S028R). The only really supported.
#define CYD_TFT_24        1                                       // Cheap Yellow Display 2.4" (CYD 2432S024R)
#define CYD_TFT_32        2                                       // Cheap Yellow Display 3.2" (CYD 2432S032R)
#define CYD_USER_DEFINED  3                                       // Cheap Yellow Display other type. User defined: Define the pins in CYD_USER_DEFINED.h
#define CYD_ESP32_035     4                                       // Cheap Yellow Display 3.5" (ST7796 320x480)
#define CYD_ESP32_32E     5                                       // Cheap Yellow Display 2.8" with ESP-32E, no free pins for encoder, use LED RGB pins instead. 


// Seleccione la version hardware del CYD (Cheap Yellow Display) de la lista   - Select the hardware version of CYD (Cheap Yellow Display) from the list
// Use el archivo User_Setup.h correcto para la libreria TFT_eSPI - Use the correct User_Setup.h file for library TFT_eSPI

#define CYD_HW_VERSION                CYD_TFT_28


////////////////////////////////////////////////////////////
// ***** LED RGB PINS USE SELECTION *****
////////////////////////////////////////////////////////////

#define PRESENT           0                                       // LED RGB present. Normal use
#define UNUSED            1                                       // LED RGB removed. Using LED RGB pins for encoder
#define FUNC_BUTTONS      2                                       // LED RGB removed. Using LED RGB pins for pushbuttons (install pull-up resistors)


// Seleccione el uso del LED RGB - Select use of the RGB LED: PRESENT / UNUSED / FUNC_BUTTONS

#define USE_RGB_LED                   PRESENT 


////////////////////////////////////////////////////////////
// ***** BATTERY LEVEL READ SELECTION *****
////////////////////////////////////////////////////////////

#define READ_UNUSED       0                                       // Don't read Battery level. Normal use
#define READ_LDR          1                                       // Using LDR pin for battery level. Remove LDR. (max. 3.3V)
#define READ_XPT          2                                       // Using XPT2046 (pin 7). (max. 5V)


// Seleccione el modo de lectura del nivel de bateria - Select mode to read battery level: READ_UNUSED / READ_LDR / READ_XPT

#define BATT_MODE                     READ_UNUSED


////////////////////////////////////////////////////////////
// ***** LOCOMOTIVE STACK OPTIONS *****
////////////////////////////////////////////////////////////

// Max. locomotoras guardadas en stack (hasta 254) - Max. locomotives saved in stack (up to 254):

#define LOCOS_IN_STACK                100


////////////////////////////////////////////////////////////
// ***** CSV FILE OPTIONS *****
////////////////////////////////////////////////////////////

// Delimitador en fichero CSV - CSV file delimiter: ';' / ','

#define CSV_FILE_DELIMITER            ';'


////////////////////////////////////////////////////////////
// ***** ENCODER OPTIONS *****
////////////////////////////////////////////////////////////

// Si el encoder es impreciso pruebe a descomentar la siguiente linea - If the encoder is inaccurate, try uncommenting the following line

//#define  ALTERNATE_ENCODER  1


////////////////////////////////////////////////////////////
// ***** END OF USER OPTIONS *****
////////////////////////////////////////////////////////////














////////////////////////////////////////////////////////////
//         DON'T MODIFY ANY OF THE FOLLOWING LINES        //
//      NO MODIFICAR NINUNA DE LAS SIGUIENTES LINEAS      //
////////////////////////////////////////////////////////////

#define USER_MIN_BL         64                                    // User min backlight
#define SYS_MIN_BL          32                                    // System inactivity backlight


#if  (CYD_HW_VERSION == CYD_TFT_28)
#include "boards/CYD_TFT_28.h"
#endif
#if  (CYD_HW_VERSION == CYD_TFT_24)
#include "boards/CYD_TFT_24.h"
#endif
#if  (CYD_HW_VERSION == CYD_TFT_32)
#include "boards/CYD_TFT_32.h"
#endif
#if  (CYD_HW_VERSION == CYD_USER_DEFINED)
#include "CYD_USER_DEFINED.h"
#endif
#if  (CYD_HW_VERSION == CYD_ESP32_035)
#include "boards/CYD_ESP32_035.h"
#endif
#if  (CYD_HW_VERSION == CYD_ESP32_32E)
#if (USE_RGB_LED != UNUSED)
#error "Esta placa no tiene sufientes pins para el encoder, use los pines del LED RGB - This board hasn't enough pins for encoder, use RGB LED pins"
#endif
#include "boards/CYD_ESP32_32E.h"
#endif


#endif

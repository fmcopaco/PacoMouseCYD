/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

#ifndef PACOMOUSECYD_CFG_H
#define PACOMOUSECYD_CFG_H

#define CYD_TFT_28        0                                       // Cheap Yellow Display 2.8"
#define CYD_TFT_24        1                                       // Cheap Yellow Display 2.4"
#define CYD_TFT_32        2                                       // Cheap Yellow Display 3.2"
#define CYD_USER_DEFINED  3                                       // User defined board

#define UNUSED            0
#define PRESENT           1
#define FUNC_BUTTONS      2

#define MODE_SPI          0
#define MODE_BITBANG      1


////////////////////////////////////////////////////////////
// ***** USER OPTIONS *****
////////////////////////////////////////////////////////////

// Seleccione la version hardware del CYD (Cheap Yellow Display)   - Select the hardware version of CYD (Cheap Yellow Display):   CYD_TFT_28 / CYD_TFT_24 / CYD_TFT_32 / CYD_USER_DEFINED
// Use el archivo User_Setup.h correcto para la libreria TFT_eSPI - Use the correct User_Setup.h file for library TFT_eSPI

#define CYD_HW_VERSION                CYD_TFT_28

// Max. locomotoras guardadas en stack (hasta 254) - Max. locomotives saved in stack (up to 254):

#define LOCOS_IN_STACK                100

// Delimitador en fichero CSV - CSV file delimiter: ';' / ','
#define CSV_FILE_DELIMITER            ';'



#if (CYD_HW_VERSION == CYD_USER_DEFINED)
////////////////////////////////////////////////////////////
// ***** USER DEFINED HARDWARE *****
////////////////////////////////////////////////////////////

// Seleccione el modo de acceso al chip XPT2046 - Select XPT2046 chip access mode : MODE_SPI / MODE_BITBANG
#define XPT_MODE      MODE_SPI

// Seleccione rotacion de la pantalla tactil - Select Touchscreen rotation: 0 / 1 / 2 / 3
#define XPT_ROTATION  3

// Touchscreen 
#define XPT2046_IRQ   36    // T_IRQ
#define XPT2046_MOSI  13    // T_DIN
#define XPT2046_MISO  12    // T_OUT
#define XPT2046_CLK   14    // T_CLK
#define XPT2046_CS    33    // T_CS

// Seleccione si usa el LED RGB - Select if use the RGB LED: PRESENT / UNUSED / FUNC_BUTTONS
#define USE_RGB_LED   PRESENT 

//RGB LED Pins
#define RGB_LED_R     4
#define RGB_LED_G     17
#define RGB_LED_B     16

//SD Pins
#define SD_CS         5

// Encoder Pins
#define ENCODER_A     22
#define ENCODER_B     21
#define ENCODER_SW    35


#endif
////////////////////////////////////////////////////////////
// ***** END OF USER DEFINED HARDWARE *****
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// ***** END OF USER OPTIONS *****
////////////////////////////////////////////////////////////






#if  (CYD_HW_VERSION == CYD_TFT_28)
#define USE_CYD_28               1                           // Cheap Yellow Display 2.8" (2432S028R)
#endif
#if  (CYD_HW_VERSION == CYD_TFT_24)
#define USE_CYD_24               1                           // Cheap Yellow Display 2.4" (2432S024R)
#endif
#if  (CYD_HW_VERSION == CYD_TFT_32)
#define USE_CYD_24               1                           // Cheap Yellow Display 3.2" (2432S032R)
#endif
#if  (CYD_HW_VERSION == CYD_USER_DEFINED)
#if (XPT_MODE == MODE_SPI)                                   // Cheap Yellow Display other type
#define USE_XPT2046_SPI
#endif
#if (XPT_MODE == MODE_BITBANG)
#define USE_XPT2046_BITBANG
#endif
#endif


#if defined(USE_CYD_28) && defined(USE_CYD_24)
#error Seleccione solo un tipo de CYD (Cheap Yellow Display) - Select only one type of CYD (Cheap Yellow Display)
#endif



#define USER_MIN_BL   64                                    // User min backlight
#define SYS_MIN_BL    32                                    // System inactivity backlight

#ifdef USE_CYD_28
// Touchscreen pins
#define XPT2046_IRQ   36    // T_IRQ
#define XPT2046_MOSI  32    // T_DIN
#define XPT2046_MISO  39    // T_OUT
#define XPT2046_CLK   25    // T_CLK
#define XPT2046_CS    33    // T_CS

#define USE_XPT2046_BITBANG

#define XPT_ROTATION  0

/*
// I2C pins
#define I2C_SDA       27
#define I2C_SCL       22
*/
//RGB LED
#define RGB_LED_R     4
#define RGB_LED_G    16
#define RGB_LED_B    17

#define USE_RGB_LED  PRESENT

//SD Pins
#define SD_CS         5

// Encoder
#define ENCODER_A    22
#define ENCODER_B    27
#define ENCODER_SW   35 


#endif


#ifdef USE_CYD_24
// Touchscreen pins
#define XPT2046_IRQ   36    // T_IRQ
#define XPT2046_MOSI  13    // T_DIN
#define XPT2046_MISO  12    // T_OUT
#define XPT2046_CLK   14    // T_CLK
#define XPT2046_CS    33    // T_CS

#define USE_XPT2046_SPI

#define XPT_ROTATION  0
/*
// I2C pins
#define I2C_SDA       21
#define I2C_SCL       22
*/
//RGB LED
#define RGB_LED_R     4
#define RGB_LED_G     17
#define RGB_LED_B     16

#define USE_RGB_LED  PRESENT

//SD Pins
#define SD_CS         5

// Encoder
#define ENCODER_A     22
#define ENCODER_B     21
#define ENCODER_SW    35 


#endif

#endif

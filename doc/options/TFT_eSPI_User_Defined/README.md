# CYD_USER_DEFINED

You need to modify the User_Setup.h file according to your board and copy it into the TFT_eSPI library folder.
In the program's config.h file, select CYD_USER_DEFINED as the hardware and modify the USER DEFINED HARDWARE section according to your board.



Tiene que modificar el archivo User_Setup.h de acuerdo a su placa y copiarlo en la carpeta de la biblioteca TFT_eSPI.
En el archivo config.h del programa seleccionar CYD_USER_DEFINED como hardware y modifique la seccion USER DEFINED HARDWARE de acuerdo a su placa.



Heu de modificar el fitxer User_Setup.h segons la vostra placa i copiar-lo a la carpeta de la biblioteca TFT_eSPI.
Al fitxer config.h del programa, seleccioneu CYD_USER_DEFINED com a hardware i modifiqueu la secció USER DEFINED HARDWARE segons la vostra placa.


Sie müssen die Datei User_Setup.h entsprechend Ihrer Platine ändern und in den TFT_eSPI-Bibliotheksordner kopieren.
Wählen Sie in der Datei config.h des Programms CYD_USER_DEFINED als Hardware aus und passen Sie den Abschnitt USER DEFINED HARDWARE entsprechend Ihrem Board an.

<br>
<br>


# config.h


////////////////////////////////////////////////////////////<br>
// ***** USER OPTIONS *****<br>
////////////////////////////////////////////////////////////

// Seleccione la version hardware del CYD (Cheap Yellow Display)   - Select the hardware version of CYD (Cheap Yellow Display):   CYD_TFT_28 / CYD_TFT_24 / CYD_TFT_32 / CYD_USER_DEFINED
// Use el archivo User_Setup.h correcto para la libreria TFT_eSPI - Use the correct User_Setup.h file for library TFT_eSPI

#define CYD_HW_VERSION                CYD_USER_DEFINED<br>


## Examples

////////////////////////////////////////////////////////////<br>
// ***** USER DEFINED HARDWARE EXAMPLES *****<br>
////////////////////////////////////////////////////////////<br>

### ESP32-32E
---



//================================================================================================ <br>
// CYD 240x320 2.8" - ESP32-32E (using RGB LED pins for encoder connection)<br>
//================================================================================================ <br>

// Seleccione el modo de acceso al chip XPT2046 - Select XPT2046 chip access mode : MODE_SPI / MODE_BITBANG<br>
#define XPT_MODE      MODE_BITBANG<br>

// Seleccione rotacion de la pantalla tactil - Select Touchscreen rotation: 0 / 1 / 2 / 3<br>
#define XPT_ROTATION  0<br>

// Touchscreen <br>
#define XPT2046_IRQ   36    // T_IRQ<br>
#define XPT2046_MOSI  32    // T_DIN<br>
#define XPT2046_MISO  39    // T_OUT<br>
#define XPT2046_CLK   25    // T_CLK<br>
#define XPT2046_CS    33    // T_CS<br>

// Seleccione si usa el LED RGB - Select if use the RGB LED: PRESENT / UNUSED<br>
#define USE_RGB_LED   UNUSED<br>

//RGB LED Pins<br>
#define RGB_LED_R     22<br>
#define RGB_LED_G     16<br>
#define RGB_LED_B     17<br>

//SD Pins<br>
#define SD_CS         5<br>

// Encoder Pins<br>
#define ENCODER_A     16<br>
#define ENCODER_B     17<br>
#define ENCODER_SW    22 <br>


<br>

### ESP32-035
---
//================================================================================================<br>
// CYD 320x480 3.5" - ESP32-035<br>
//================================================================================================<br>

// Seleccione el modo de acceso al chip XPT2046 - Select XPT2046 chip access mode : MODE_SPI / MODE_BITBANG<br>
#define XPT_MODE      MODE_SPI<br>

// Seleccione rotacion de la pantalla tactil - Select Touchscreen rotation: 0 / 1 / 2 / 3<br>
#define XPT_ROTATION  3<br>

// Touchscreen <br>
#define XPT2046_IRQ   36    // T_IRQ<br>
#define XPT2046_MOSI  13    // T_DIN<br>
#define XPT2046_MISO  12    // T_OUT<br>
#define XPT2046_CLK   14    // T_CLK<br>
#define XPT2046_CS    33    // T_CS<br>

// Seleccione si usa el LED RGB - Select if use the RGB LED: PRESENT / UNUSED<br>
#define USE_RGB_LED   PRESENT <br>

//RGB LED Pins<br>
#define RGB_LED_R     4<br>
#define RGB_LED_G     17<br>
#define RGB_LED_B     16<br>

//SD Pins<br>
#define SD_CS         5<br>

// Encoder Pins<br>
#define ENCODER_A     22<br>
#define ENCODER_B     21<br>
#define ENCODER_SW    35<br>



 



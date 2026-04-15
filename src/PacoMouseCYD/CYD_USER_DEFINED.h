/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/
// Cheap Yellow Display defined by user

////////////////////////////////////////////////////////////
// ***** USER DEFINED HARDWARE *****
////////////////////////////////////////////////////////////

// Touchscreen pins
#define XPT2046_IRQ   36    // T_IRQ
#define XPT2046_MOSI  13    // T_DIN
#define XPT2046_MISO  12    // T_OUT
#define XPT2046_CLK   14    // T_CLK
#define XPT2046_CS    33    // T_CS

// Seleccione el modo de acceso al chip XPT2046 - Select XPT2046 chip access mode : USE_XPT2046_SPI / USE_XPT2046_BITBANG
#define USE_XPT2046_SPI

// Seleccione rotacion de la pantalla tactil - Select Touchscreen rotation: 0 / 1 / 2 / 3
#define XPT_ROTATION  3

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

// LDR Pins
#define LDR_INPUT     34

/*
// I2C pins
#define I2C_SDA       21
#define I2C_SCL       22
*/

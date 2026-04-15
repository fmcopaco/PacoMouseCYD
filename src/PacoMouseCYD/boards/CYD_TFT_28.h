/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/
// Cheap Yellow Display 2.8" (CYD 2432S028R)

// Touchscreen pins
#define XPT2046_IRQ   36    // T_IRQ
#define XPT2046_MOSI  32    // T_DIN
#define XPT2046_MISO  39    // T_OUT
#define XPT2046_CLK   25    // T_CLK
#define XPT2046_CS    33    // T_CS

#define USE_XPT2046_BITBANG

#define XPT_ROTATION  0

//RGB LED pins
#define RGB_LED_R     4
#define RGB_LED_G     16
#define RGB_LED_B     17

//SD Pins
#define SD_CS         5

// Encoder pins
#define ENCODER_A     22
#define ENCODER_B     27
#define ENCODER_SW    35 

// LDR Pins
#define LDR_INPUT     34


/*
// I2C pins
#define I2C_SDA       27
#define I2C_SCL       22
*/

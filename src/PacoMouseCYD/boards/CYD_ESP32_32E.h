/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/
// Cheap Yellow Display 2.8" with ESP-32E, no free pins for encoder, using LED RGB pins for encoder. 

// Touchscreen pins
#define XPT2046_IRQ   36    // T_IRQ
#define XPT2046_MOSI  32    // T_DIN
#define XPT2046_MISO  39    // T_OUT
#define XPT2046_CLK   25    // T_CLK
#define XPT2046_CS    33    // T_CS

#define USE_XPT2046_BITBANG

#define XPT_ROTATION  0

//RGB LED Pins
#define RGB_LED_R     22
#define RGB_LED_G     16
#define RGB_LED_B     17

//SD Pins
#define SD_CS         5

// Encoder Pins
#define ENCODER_A     16
#define ENCODER_B     17
#define ENCODER_SW    22 


//no free pins for encoder, using LED RGB pins for encoder. 
#undef USE_RGB_LED
#define USE_RGB_LED   UNUSED

// LDR Pins
#define LDR_INPUT     34

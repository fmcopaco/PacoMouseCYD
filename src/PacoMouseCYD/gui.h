/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
      Very basic Graphical User Interface (GUI) for PacoMouseCYD
      All data in absolute coordinates
*/

#include <TFT_eSPI.h>                                       // Graphics and font library for ILI9341 driver chip  v2.5.43
#include "FreeSans7pt7b.h"
#include "FreeSansBold6pt7b.h"


#define MAX_OBJ_STACK       100
#define MAX_LABEL_LNG       150
#define TMR_RESOLUTION      100

#define NOT_USED            0xFF
#define OBJ_NOT_FOUND       0xFFFF


////////////////////////////////////////////////////////////
// ***** FONTS *****
////////////////////////////////////////////////////////////

#ifndef LOAD_GFXFF
ERROR_Please_enable_LOAD_GFXFF_in_User_Setup!
#endif

#define GFXFF               1
#define FSS7                &FreeSans7pt7b
#define FSS9                &FreeSans9pt7b
#define FSSB6               &FreeSansBold6pt7b
#define FSSB9               &FreeSansBold9pt7b
#define FSSB12              &FreeSansBold12pt7b


////////////////////////////////////////////////////////////
// ***** COLORS *****
////////////////////////////////////////////////////////////

// Colour definitions for 64K colour mode (RGB565)
// Bits 0..4 ->   Blue 0..4
// Bits 5..10 ->  Green 0..5
// Bits 11..15 -> Red 0..4
// Assign human-readable names to some common 16-bit color values: http://rinkydinkelectronics.com/calc_rgb565.php
// Examples: https://github.com/newdigate/rgb565_colors

#define COLOR_WHITE           0xFFFF
#define COLOR_BLACK           0x0000
#define COLOR_BLUE            0x001F
#define COLOR_NAVY            0x000F
#define COLOR_AQUA            0x5D1C
#define COLOR_SKYBLUE         0x867D
#define COLOR_RED             0xF882
#define COLOR_DARKRED         0x8800
#define COLOR_CANDY_RED       0xF840
#define COLOR_PINK            0xF97F
#define COLOR_MAGENTA         0xF81F
#define COLOR_GREEN           0x0780
#define COLOR_GREENYELLOW     0xAFE5
#define COLOR_DARKGREEN       0x03E0
#define COLOR_CYAN            0x07FF
#define COLOR_DARKCYAN        0x03EF
#define COLOR_YELLOW          0xFFE0
#define COLOR_GOLD            0xDD24
#define COLOR_LIGHTGREY       0xC618
#define COLOR_DARKGREY        0x7BEF
#define COLOR_LIGHTBLACK      0x4A49
#define COLOR_SMOKYBLACK      0x1061
#define COLOR_CHARCOAL        0x3A2A
#define COLOR_VIOLET          0x9199
#define COLOR_BROWN           0x8200
#define COLOR_ORANGE          0xFD20
#define COLOR_LIME            0x87E0
#define COLOR_MAROON          0x7800
#define COLOR_PURPLE          0x780F
#define COLOR_OLIVE           0x7BE0
#define COLOR_SILVER          0xA510
#define COLOR_CREAM           0xFFF9
#define COLOR_GHOST_WHITE     0xF7BF

#define COLOR_BACKGROUND      0xB5B6                      // 0xB6B6B6
#define COLOR_TRANSPARENT     TFT_TRANSPARENT

const uint16_t colorDraw[] = {COLOR_BLACK, COLOR_BLUE, COLOR_RED, COLOR_MAGENTA, COLOR_GREEN, COLOR_CYAN, COLOR_YELLOW, COLOR_WHITE,
                              COLOR_BACKGROUND, COLOR_TRANSPARENT, COLOR_GHOST_WHITE, COLOR_AQUA, COLOR_CREAM, COLOR_SMOKYBLACK, COLOR_SKYBLUE,
                              COLOR_GOLD,
                             };


////////////////////////////////////////////////////////////
// ***** OBJECT *****
////////////////////////////////////////////////////////////

enum objTypeGUI {OBJ_UNDEF, OBJ_TIMER, OBJ_WIN, OBJ_BUTTON, OBJ_LABEL, OBJ_TXT, OBJ_BAR, OBJ_DRAWSTR, OBJ_ICON, OBJ_KEYBOARD, OBJ_SWITCH,
                 OBJ_GAUGE, OBJ_LPIC, OBJ_FNC, OBJ_SLIDER, OBJ_RADIO, OBJ_CHAR, OBJ_DATA,
                };

struct wObj {                                           // Graphic objects
  uint16_t  objType;
  uint16_t  objID;
};

struct wObj objStack[MAX_OBJ_STACK];                    // Object stack
uint16_t endObjStack;
uint16_t lastWinStack;


////////////////////////////////////////////////////////////
// ***** EVENT *****
////////////////////////////////////////////////////////////

enum EventGUI {EVNT_CLICK, EVNT_DRAW, EVNT_WOPEN, EVNT_WCLOSE, EVNT_TIMER, EVNT_PRESS};

typedef struct {                                        // Events
  uint16_t  objType;
  uint16_t  objID;
  uint16_t  eventID;
} wEvent;

wEvent eventStack[32];                                  // stack for events (size 32, hardcoded in functions. Don't change!)
uint16_t  eventIn;
uint16_t  eventOut;
uint16_t  eventsPending;
uint16_t  lastClickX;
uint16_t  lastClickY;


////////////////////////////////////////////////////////////
// ***** TIMER *****
////////////////////////////////////////////////////////////

enum timers {TMR_BLIGHT, TMR_END_LOGO, TMR_POWER, TMR_SPEEDO, TMR_INFO, TMR_WAIT, TMR_STEAM, TMR_ACCESSORY, TMR_SCAN, TMR_FNC_ECOS,
             TMR_STA_RUN, TMR_WIFI_CHK,
             MAX_SYS_TIMER
            };

enum timerType {TMR_STOP, TMR_ONESHOT, TMR_PERIODIC};

typedef struct {
  uint16_t  tmrDelay;
  uint16_t  tmrCount;
  uint16_t  type;
} wTimer;

wTimer wTimerStack[MAX_SYS_TIMER];
uint32_t  timerSys;

#if (TFT_WIDTH == 240)
#include "gui240x320.h"
#endif
#if (TFT_WIDTH == 320)
#include "gui320x480.h"
#endif

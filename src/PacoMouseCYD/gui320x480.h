/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
      Very basic Graphical User Interface (GUI) for PacoMouseCYD
      All data in absolute coordinates

      Alternate TFT display 320x480 (Only for testing purposes of CYD_USER_DEFINED types)
*/

////////////////////////////////////////////////////////////
// ***** WINDOW *****
////////////////////////////////////////////////////////////

enum winObj {WIN_DEFAULT, WIN_LOGO, WIN_CALIBRATE, WIN_THROTTLE, WIN_SSID, WIN_WIFI, WIN_WIFI_PWD, WIN_WIFI_NET_NAME, WIN_WIFI_NET, WIN_PROTOCOL,
             WIN_ALERT, WIN_CONFIG, WIN_SET_CLOCK, WIN_LOK_EDIT, WIN_EDIT_NAME, WIN_FUNC, WIN_CHG_FUNC, WIN_VMAX,
             WIN_SEL_LOCO, WIN_ENTER_ADDR, WIN_SEL_IMAGE, WIN_MENU, WIN_SCREEN, WIN_SPEED, WIN_ABOUT, WIN_LOCK,
             WIN_OPTIONS, WIN_SPEEDO, WIN_SPEEDO_LNG, WIN_SPEEDO_SCALE, WIN_READ_CV, WIN_PROG_CV, WIN_PROG_ADDR,
             WIN_PROG_LNCV, WIN_STEAM, WIN_UTIL, WIN_ACCESSORY, WIN_PANELS, WIN_PANEL_NAME, WIN_ACC_CTRL, WIN_ACC_ASPECT,
             WIN_ACC_TYPE, WIN_ACC_EDIT, WIN_ACC_NAME, WIN_ACC_ADDR1, WIN_ACC_ADDR2, WIN_WIFI_SCAN,
             WIN_STA_RUN, WIN_STA_PLAY, WIN_STA_STARS, WIN_STA_EDIT, WIN_STA_KEYB, WIN_DEF_ACTION, WIN_BATT,
             MAX_WIN_OBJ
            };

typedef struct {                                        // Window data
  uint16_t  x;
  uint16_t  y;
  uint16_t  w;
  uint16_t  h;
  bool      modal;
  uint16_t  color;
  uint16_t  backgnd;
} wWinObj;

//bool modalWindow;

wWinObj winData[MAX_WIN_OBJ] = {
  {  0,   0, 320, 480, true, COLOR_NAVY,  COLOR_BACKGROUND},   // WIN_DEFAULT
  {  0,   0, 320, 480, true, COLOR_NAVY,  COLOR_WHITE},        // WIN_LOGO
  {  0,   0, 320, 480, true, COLOR_BLACK, COLOR_BLACK},        // WIN_CALIBRATE
  {  0,   0, 320, 480, true, COLOR_BLACK, COLOR_BACKGROUND},   // WIN_THROTTLE
  { 40,  80, 240, 320, true, COLOR_WHITE, COLOR_DARKGREY},     // WIN_SSID
  {  0,   0, 240, 320, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_WIFI
  {  0, 125, 240, 195, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_WIFI_PWD
  {  0, 125, 240, 195, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_WIFI_NET_NAME
  {  0,   0, 240, 320, true, COLOR_BLUE,  COLOR_WHITE},        // WIN_WIFI_NET
  {  0,   0, 240, 320, true, COLOR_BLACK, COLOR_WHITE},        // WIN_PROTOCOL
  { 15, 120, 210,  80, true, COLOR_BLACK, COLOR_WHITE},        // WIN_ALERT
  {  0,   0, 320, 480, true, COLOR_BLACK, COLOR_WHITE},        // WIN_CONFIG
  { 15, 120, 210, 140, true, COLOR_BLACK, COLOR_BACKGROUND},   // WIN_SET_CLOCK
  {  0,   0, 240, 320, true, COLOR_BLACK, COLOR_BACKGROUND},   // WIN_LOK_EDIT
  {  0, 125, 240, 195, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_EDIT_NAME
  {  0,   0, 240, 320, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_FUNC
  { 60, 100, 120, 100, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_CHG_FUNC
  { 36,  30, 155, 245, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_VMAX
  {  0,   0, 240, 320, true, COLOR_NAVY,  COLOR_WHITE},        // WIN_SEL_LOCO
  { 36,  30, 155, 245, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_ENTER_ADDR
  {  40, 80, 240, 320, true, COLOR_NAVY,  COLOR_BACKGROUND},   // WIN_SEL_IMAGE
  {  0,   0, 320, 480, true, COLOR_AQUA,  COLOR_CHARCOAL},     // WIN_MENU
  { 15,  80, 210, 190, true, COLOR_BLACK, COLOR_WHITE},        // WIN_SCREEN
  { 15, 120, 210, 160, true, COLOR_BLACK, COLOR_WHITE},        // WIN_SPEED
  { 45,  15, 230, 220, true, COLOR_NAVY,  COLOR_WHITE},        // WIN_ABOUT
  { 15, 120, 210, 160, true, COLOR_BLACK, COLOR_WHITE},        // WIN_LOCK
  {  5,  75, 230, 205, true, COLOR_BLACK, COLOR_WHITE},        // WIN_OPTIONS
  {  0,   0, 240, 320, true, COLOR_BLACK,  COLOR_BACKGROUND},  // WIN_SPEEDO
  { 36,  30, 155, 245, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_SPEEDO_LNG
  {  0,   0, 240, 320, true, COLOR_BLACK, COLOR_BACKGROUND},   // WIN_SPEEDO_SCALE
  {  0,   0, 320, 480, true, COLOR_AQUA,  COLOR_WHITE},        // WIN_READ_CV
  {  0,   0, 240, 320, true, COLOR_BLACK, COLOR_BACKGROUND},   // WIN_PROG_CV
  {  0,   0, 240, 320, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_PROG_ADDR
  {  0,   0, 240, 320, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_PROG_LNCV
  {  0,   0, 320, 480, true, COLOR_BLACK, COLOR_BLACK},        // WIN_STEAM
  {  0,   0, 320, 480, true, COLOR_BLACK, COLOR_WHITE},        // WIN_UTIL
  {  0,   0, 320, 480, true, COLOR_BLACK, COLOR_WHITE},        // WIN_ACCESSORY
  {  0,   0, 240, 320, true, COLOR_BLACK, COLOR_WHITE},        // WIN_PANELS
  {  0, 125, 240, 195, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_PANEL_NAME
  { 10,  25, 210, 255, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_ACC_CTRL
  {  5,  95, 230,  80, true, COLOR_AQUA,  COLOR_YELLOW},       // WIN_ACC_ASPECT
  { 40, 100, 160, 100, true, COLOR_WHITE, COLOR_LIGHTGREY},    // WIN_ACC_TYPE
  {  0,   0, 240, 240, true, COLOR_WHITE, COLOR_LIGHTGREY},    // WIN_ACC_EDIT
  {  0, 125, 240, 195, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_ACC_NAME
  { 36,  30, 155, 245, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_ACC_ADDR1
  { 36,  30, 155, 245, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_ACC_ADDR2
  {  0,   0, 320, 480, true, COLOR_BLACK, COLOR_BLACK},        // WIN_WIFI_SCAN
  {  0,   0, 320, 480, true, COLOR_WHITE, COLOR_BLUE},         // WIN_STA_RUN
  {  0,   0, 320, 480, true, COLOR_WHITE, COLOR_BLUE},         // WIN_STA_PLAY
  { 10, 120, 220,  80, true, COLOR_BLACK, COLOR_WHITE},        // WIN_STA_STARS
  {  0,   0, 240, 320, true, COLOR_WHITE, COLOR_BLUE},         // WIN_STA_EDIT
  { 85,  75, 150, 200, true, COLOR_WHITE, COLOR_BACKGROUND},   // WIN_STA_KEYB
#if (USE_RGB_LED == FUNC_BUTTONS)
  {  0,  40, 240, 270, true, COLOR_ORANGE, COLOR_BLUE},        // WIN_DEF_ACTION
#else
  {  0,  40, 240, 150, true, COLOR_ORANGE, COLOR_BLUE},        // WIN_DEF_ACTION
#endif
  { 40,  55, 160, 210, true, COLOR_BLUE,  COLOR_WHITE},        // WIN_BATT
};


////////////////////////////////////////////////////////////
// ***** LABEL *****
////////////////////////////////////////////////////////////

uint8_t currLanguage;

enum labelObj {LBL_PACO_TXT, LBL_INIT, LBL_CONNECT, LBL_PRESS, LBL_CAL, LBL_CAL_DONE,
               LBL_WIFI_NET, LBL_SCAN, LBL_SSID_SCAN, LBL_SSID, LBL_IP, LBL_PWD_HIDE, LBL_PORT, LBL_PROTOCOL,
               LBL_SEL_PROT, LBL_SEL_Z21, LBL_SEL_XNET, LBL_SEL_ECOS, LBL_SEL_LNET, LBL_SEL_LBSERVER, LBL_SEL_BINARY, LBL_OPTIONS,
               LBL_NAME, LBL_ADDR, LBL_IMAGE, LBL_VMAX, LBL_FUNC, LBL_SERVICE, LBL_KMH, LBL_SHUNTING, LBL_RATE, LBL_CHG_WIFI,
               LBL_EDIT_FUNC, LBL_STACK_FULL, LBL_STOP_0, LBL_STOP_E, LBL_SEL_IMAGE,
               LBL_MENU_DRIVE, LBL_MENU_ACC, LBL_MENU_CV, LBL_MENU_CFG, LBL_MENU_UTILS,
               LBL_CFG_LANG, LBL_CFG_SCR, LBL_CFG_SPD, LBL_CFG_WIFI, LBL_CFG_FCLK, LBL_CFG_LOCK, LBL_CFG_ABOUT, LBL_SCR_ROTATE, LBL_PACO_WEB,
               LBL_LOCK_LOK, LBL_LOCK_ACC, LBL_LOCK_PRG, LBL_OPT_ADR, LBL_OPT_IB2, LBL_OPT_UHLI, LBL_OPT_DIG,
               LBL_ESTOP, LBL_SCALE, LBL_MM, LBL_SCALE_H0, LBL_SCALE_N, LBL_SCALE_TT, LBL_SCALE_Z, LBL_SCALE_0, LBL_MEASURE,
               LBL_CV_ADDR, LBL_CV_SPD_L, LBL_CV_SPD_M, LBL_CV_SPD_H, LBL_CV_ACC, LBL_CV_DEC, LBL_CV_CFG, LBL_CV_MAN,
               LBL_CV, LBL_LNCV, LBL_POM, LBL_BITS, LBL_CV_ERROR, LBL_UTIL_SPEED, LBL_UTIL_STEAM, LBL_UTIL_SCAN, LBL_UTIL_STA,
               LBL_ASK_SURE, LBL_OPT_DISCOVER, LBL_LNCV_ART, LBL_LNCV_MOD, LBL_LNCV_NUM, LBL_ACC_TYPE, LBL_ACC_NAME, LBL_ACC_ADDR,
               LBL_STA_RUN, LBL_STA_LEVEL, LBL_STA_START, LBL_STA_INSTR, LBL_STA_EXCEL, LBL_STA_GREAT, LBL_STA_TIMEOUT,
               LBL_STA_STATIONS, LBL_STA_TURNOUTS, LBL_STA_TIME, LBL_STA_DESC, LBL_DEF_ACTION, LBL_ACTIONS, LBL_NOT_FOUND, LBL_UPDATE,
               LBL_BATT,
               MAX_LABEL_OBJ
              };

typedef struct {                                        // Label data
  uint16_t  x;
  uint16_t  y;
  const GFXfont *font;
  uint16_t  color;
  byte      align;
} wLabelObj;
/*
  Normally strings are printed relative to the top left corner but this can be
  changed with the setTextDatum() function. The library has #defines for:

  TL_DATUM = Top left
  TC_DATUM = Top centre
  TR_DATUM = Top right
  ML_DATUM = Middle left
  MC_DATUM = Middle centre
  MR_DATUM = Middle right
  BL_DATUM = Bottom left
  BC_DATUM = Bottom centre
  BR_DATUM = Bottom right
*/
wLabelObj labelData[MAX_LABEL_OBJ] = {
  { 160,  35, FSSB12, COLOR_BLACK,  MC_DATUM},          // LBL_PACO_TXT
  {  20, 120, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_INIT
  {  20, 120, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CONNECT
  { 160, 240, FSSB12, COLOR_YELLOW, MC_DATUM},          // LBL_PRESS
  { 160,   0, FSS9,   COLOR_WHITE,  TC_DATUM},          // LBL_CAL
  { 160,   0, FSS9,   COLOR_GREEN,  TC_DATUM},          // LBL_CAL_DONE
  { 120,  20, FSSB12, COLOR_BLUE,   MC_DATUM},          // LBL_WIFI_NET
  { 160, 120, FSSB12, COLOR_YELLOW, MC_DATUM},          // LBL_SCAN
  { 160, 105, FSSB12, COLOR_YELLOW, MC_DATUM},          // LBL_SSID_SCAN
  {  10,  39, FSSB9,  COLOR_BLACK, TL_DATUM},           // LBL_SSID
  {  10, 138, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_IP
  {  10, 100, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_PWD_HIDE
  {  10, 176, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_PORT
  {  10, 214, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_PROTOCOL
  {  10,  10, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_SEL_PROT
  {  50,  50, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_SEL_Z21
  {  50,  85, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_SEL_XNET
  {  50, 120, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_SEL_ECOS
  {  50, 155, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_SEL_LNET
  {  90, 190, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_SEL_LBSERVER
  {  90, 225, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_SEL_BINARY
  {  70, 280, FSSB9,  COLOR_BLACK,  MC_DATUM},          // LBL_OPTIONS
  {   5, 144, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_NAME       Loco Data & .csv
  {   5,  15, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_ADDR
  {   5,  55, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_IMAGE
  {   5, 184, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_VMAX
  { 120, 232, FSSB9,  COLOR_BLACK,  MC_DATUM},          // LBL_FUNC
  {  65, 160, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_SERVICE
  { 159, 320, FSSB9,  COLOR_BLACK,  MC_DATUM},          // LBL_KMH
  {  85, 133, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_SHUNTING
  {  25, 174, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_RATE
  {  65, 140, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CHG_WIFI
  { 120,  55, FSSB9,  COLOR_BLACK,  MC_DATUM},          // LBL_EDIT_FUNC
  {  65, 140, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_STACK_FULL
  {  85, 174, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_STOP_0
  {  85, 205, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_STOP_E
  { 140, 100, FSSB12, COLOR_YELLOW, MC_DATUM},          // LBL_SEL_IMAGE
  {  85, 120, FSSB9,  COLOR_WHITE,  ML_DATUM},          // LBL_MENU_DRIVE
  {  85, 180, FSSB9,  COLOR_WHITE,  ML_DATUM},          // LBL_MENU_ACC
  {  85, 240, FSSB9,  COLOR_WHITE,  ML_DATUM},          // LBL_MENU_CV
  {  85, 300, FSSB9,  COLOR_WHITE,  ML_DATUM},          // LBL_MENU_CFG
  {  85, 360, FSSB9,  COLOR_WHITE,  ML_DATUM},          // LBL_MENU_UTILS
  {  85,  30, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CFG_LANG
  {  85,  90, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CFG_SCR
  {  85, 150, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CFG_SPD
  {  85, 210, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CFG_WIFI
  {  85, 270, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CFG_FCLK
  {  85, 330, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CFG_LOCK
  {  85, 390, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CFG_ABOUT
  {  75, 138, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_SCR_ROTATE
  { 160, 150, FSS9,   COLOR_NAVY,   MC_DATUM},          // LBL_PACO_WEB
  {  75, 133, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_LOCK_LOK
  {  75, 168, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_LOCK_ACC
  {  75, 203, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_LOCK_PRG
  {  60, 155, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_OPT_ADR
  {  60, 120, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_OPT_IB2
  {  60, 155, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_OPT_UHLI
  {  60, 190, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_OPT_DIG
  {  65, 160, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_ESTOP
  { 100,  74, FSSB9,  COLOR_BLACK,  TR_DATUM},          // LBL_SCALE
  { 165, 148, FSSB9,  COLOR_BLACK,  TL_DATUM},          // LBL_MM
  {  40,  95, FSSB12, COLOR_BLACK,  MC_DATUM},          // LBL_SCALE_H0
  {  40, 135, FSSB12, COLOR_BLACK,  MC_DATUM},          // LBL_SCALE_N
  {  40, 175, FSSB12, COLOR_BLACK,  MC_DATUM},          // LBL_SCALE_TT
  {  40, 215, FSSB12, COLOR_BLACK,  MC_DATUM},          // LBL_SCALE_Z
  {  40, 255, FSSB12, COLOR_BLACK,  MC_DATUM},          // LBL_SCALE_0
  {   0,   0, FSSB12, COLOR_BLACK,  MC_DATUM},          // LBL_MEASURE
  {  85,  30, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CV_ADDR
  {  85,  90, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CV_SPD_L
  {  85, 150, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CV_SPD_M
  {  85, 210, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CV_SPD_H
  {  85, 270, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CV_ACC
  {  85, 330, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CV_DEC
  {  85, 390, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CV_CFG
  {  85, 450, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_CV_MAN
  {  10,  50, FSSB12, COLOR_BLACK,  TL_DATUM},          // LBL_CV
  {  45, 246, FSSB9,  COLOR_BLACK,  MC_DATUM},          // LBL_LNCV
  {  35, 147, FSSB9,  COLOR_BLACK,  TC_DATUM},          // LBL_POM
  {  15,  98, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_BITS
  {   0,   0, FSSB12, COLOR_BLACK,  MC_DATUM},          // LBL_CV_ERROR
  {  85,  30, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_UTIL_SPEED
  {  85,  90, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_UTIL_STEAM
  {  85, 150, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_UTIL_SCAN
  {  85, 210, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_UTIL_STA
  {  65, 140, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_ASK_SURE
  {  60,  85, FSS9,   COLOR_BLACK,  TL_DATUM},          // LBL_OPT_DISCOVER
  {  10,  20, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_LNCV_ART
  {  10,  60, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_LNCV_MOD
  {  10, 100, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_LNCV_NUM
  { 120, 115, FSSB9,  COLOR_NAVY,   MC_DATUM},          // LBL_ACC_TYPE
  {  37,  20, FSSB9,  COLOR_NAVY,   MC_DATUM},          // LBL_ACC_NAME
  {  37,  60, FSSB9,  COLOR_NAVY,   MC_DATUM},          // LBL_ACC_ADDR
  { 160,  40, FSSB12, COLOR_WHITE,  MC_DATUM},          // LBL_STA_RUN
  {  10, 100, FSS9,   COLOR_WHITE,  TL_DATUM},          // LBL_STA_LEVEL
  { 160, 229, FSSB9,  COLOR_WHITE,  MC_DATUM},          // LBL_STA_START
  { 160, 300, FSS9,   COLOR_WHITE,  TC_DATUM},          // LBL_STA_INSTR
  {  75, 160, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_STA_EXCEL
  {  75, 160, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_STA_GREAT
  {  60, 150, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_STA_TIMEOUT
  {  10,  50, FSSB9,  COLOR_WHITE,  TL_DATUM},          // LBL_STA_STATIONS
  {  10,  90, FSSB9,  COLOR_WHITE,  TL_DATUM},          // LBL_STA_TURNOUTS
  {  10,  10, FSSB9,  COLOR_WHITE,  TL_DATUM},          // LBL_STA_TIME
  {  10, 130, FSS9,   COLOR_YELLOW, TL_DATUM},          // LBL_STA_DESC
  { 120,  55, FSSB9,  COLOR_WHITE,  MC_DATUM},          // LBL_DEF_ACTION
  {   0,   0, FSS9,   COLOR_WHITE,  MC_DATUM},          // LBL_ACTIONS
  {  65, 140, FSSB9,  COLOR_BLACK,  ML_DATUM},          // LBL_NOT_FOUND
  { 160, 270, FSS9,   COLOR_WHITE,  MC_DATUM},          // LBL_UPDATE
  { 120,  70, FSSB9,  COLOR_BLUE,   MC_DATUM},          // LBL_BATT

};



////////////////////////////////////////////////////////////
// ***** DRAW STRING *****
////////////////////////////////////////////////////////////

/*  Kn    Color n. From colorDraw[]
    kn    Sprite color depth
    Xn    Cursor X
    xn    Increment x position
    Yn    Cursor Y
    yn    Increment y position
    Rw,h  Fill rectangle width,height
    rw,h  Draw rectangle width,height
    Cr    Fill circle radius r
    cr    Draw circle radius r
    Lx,y  Draw Line to x,y
    lx,y  Draw polyline to x,y (X,Y set to end position: x,y)
    px,y  second point (for triangles)  -> XxYypx,yTx,y
    Tx,y  fill triangle (X,Y),(px,y),(Tx,y)
    tx,y  draw triangle (X,Y),(px,y),(tx,y)
    Sw,h  create sprite and draw string into it. x & y set to 0, background color defined with Kn -> KnkdSw,h
    sn    draw sprite with transparent color n
    Dw,h  Degrade rectangle
    dn    Band height for degrade
*/

const char drwStrInit[]      = {"K6R320,50K1Y50R320,6X0Y200R320,280"};
const char drwStrIniStatus[] = {"K7X1Y105R180,35"};
const char drwStrSpanish[]   = {"K2S32,24K6y6R32,12s9"};
const char drwStrCatalan[]   = {"K6S32,24K2y3R32,3y5R32,3y5R32,3y5R32,3s9"};            // quatre barres
const char drwStrEnglish[]   = {"K1S32,24"                                              // blue background
                                "K7x13R6,24X0y9R32,6"                                   // white cross
                                "X0Y1L11,8y21L11,15X1Y0L12,7y23L12,16"                  // left white lines
                                "X19Y7L30,0y9L30,23X20Y8L31,1y7L31,22"                  // right white lines
                                "K2X0Y10R32,4x14Y0R4,24"                                // red cross
                                "X0Y0L12,8y23L12,15X19Y8L31,0X19Y15L31,23"              // red lines
                                "s9"
                               };
const char drwStrGerman[]    = {"K0S32,24K2y8R32,8K6y8R32,8s9"};
const char drwStrCzech[]     = {"K7S32,24K8r32,24K2Y12R32,12K1Y0p12,12T0,23s9"};
const char drwStrFrench[]    = {"K7S32,24K8r32,24K1R11,24K2X21R11,24s9"};
const char drwStrClock[]     = {"K7C12"};               //{"K7C12K0X97Y138L97,140y6L97,146"};
const char drwStrSelLok[]    = {"K0C21"};               // K10X133Y11R34,26"};
const char drwStrMenu[]      = {""};                    // X0Y0K14d16D320,480"};                    // K11L319,90y60L319,150y60L319,210y60L319,270y60L319,330"};
const char drwStrCfgMenu[]   = {"X1Y0K7d15D318,480"};                    // K11L239,40y40L239,80y40L239,120y40L239,160y40L239,200y40L239,240y40L239,280"};
const char drwStrUtlMenu[]   = {""};                                      // X1Y0K7d15D318,480"};
const char drwStrAbout[]     = {"K6R228,50K1y50R228,6y98R228,70"};
const char drwStrSpdTrk[]    = {"K0L199,125K2X81Y133p87,127T93,133X145Y133p151,127T157,133"};
const char drwStrSpdDel[]    = {"K8k16S32,18s9"};
const char drwStrSteam[]     = {"K14p90,0T0,90X32Y0R270,50X230Y0p319,0T319,90X35Y125C25X285Y125C25"               // Sky
                                "K13X143Y32R34,18X141R38,2"                                                       // Chimney
                                "K7X160Y270c125Y340c40X139Y105C31X250Y305L255,305K8X5Y400R310,4"                  // Cabin
                                "K15X47Y172R23,66K2X63Y179p67,175T67,183X63Y229p67,225T67,233"                    // Water level
                               };
const char drwStrWifiScan[]  = {"K0R320,257K10X40'yY302L279,302X160Y370C27"};
const char drwStrStaPlay[]   = {"K1d15D318,330K7X30Y150L210,150"};
const char drwStrRGB[]       = {"K7S32,32K2X15Y8C7K1X7Y22C7K4X23Y22C7s9"};
const char drwStrBatt[]      = {"K11R6,14"};
const char drwStrBattLow[]   = {"K2p75,200T85,200"};
const char drwStrBattFull[]  = {"K4p171,200T181,200"};


enum drwStrObj {DSTR_INIT, DSTR_INIT_STAT, DSTR_ENGLISH, DSTR_SPANISH, DSTR_CATALAN, DSTR_GERMAN, DSTR_CZECH, DSTR_FRENCH,
                DSTR_CLOCK, DSTR_SELLOK, DSTR_MENU, DSTR_CFG_MENU, DSTR_UTL_MENU, DSTR_ABOUT,
                DSTR_SPEEDO_TRK, DSTR_SPEEDO_BLANK, DSTR_STEAM, DSTR_WIFI_SCAN, DSTR_STATION_PLAY,
                DSTR_RGB, DSTR_BATT, DSTR_BATT_L, DSTR_BATT_F,
                MAX_DRAWSTR_OBJ
               };

typedef struct {                                        // drawStr data
  uint16_t  x;
  uint16_t  y;
  const char *str;
} wDrawStr;

wDrawStr drawStrData[MAX_DRAWSTR_OBJ] = {
  {  0,   0, drwStrInit},                               // DSTR_INIT
  {  0,   0, drwStrIniStatus},                          // DSTR_INIT_STAT
  { 44,  18, drwStrEnglish},                            // DSTR_ENGLISH
  { 44,  18, drwStrSpanish},                            // DSTR_SPANISH
  { 44,  18, drwStrCatalan},                            // DSTR_CATALAN
  { 44,  18, drwStrGerman},                             // DSTR_GERMAN
  { 44,  18, drwStrCzech},                              // DSTR_CZECH
  { 44,  18, drwStrFrench},                             // DSTR_FRENCH
  { 36, 141, drwStrClock},                              // DSTR_CLOCK
  { 58,  24, drwStrSelLok},                             // DSTR_SELLOK
  {  1,  90, drwStrMenu},                               // DSTR_MENU
  {  1,  40, drwStrCfgMenu},                            // DSTR_CFG_MENU
  {  1,  40, drwStrUtlMenu},                            // DSTR_UTL_MENU
  { 46,  16, drwStrAbout},                              // DSTR_ABOUT
  { 40, 125, drwStrSpdTrk},                             // DSTR_SPEEDO_TRK
  { 40, 107, drwStrSpdDel},                             // DSTR_SPEEDO_BLANK
  {  0,   0, drwStrSteam},                              // DSTR_STEAM
  {  0,  80, drwStrWifiScan},                           // DSTR_WIFI_SCAN
  {  1,   0, drwStrStaPlay},                            // DSTR_STATION_PLAY
  { 80, 165, drwStrRGB},                                // DSTR_RGB
  {180, 162, drwStrBatt},                               // DSTR_BATT
  { 80, 195, drwStrBattLow},                            // DSTR_BATT_L
  {176, 195, drwStrBattFull},                           // DSTR_BATT_F
};


////////////////////////////////////////////////////////////
// ***** CHAR *****
////////////////////////////////////////////////////////////

enum charObj {CHAR_CLK_COLON, CHAR_CV_EQUAL, CHAR_CV_0, CHAR_CV_1, CHAR_CV_2, CHAR_CV_3, CHAR_CV_4, CHAR_CV_5, CHAR_CV_6, CHAR_CV_7,
              CHAR_LNCV_EQUAL, CHAR_STA_STAM, CHAR_STA_STAP, CHAR_STA_TURNM, CHAR_STA_TURNP,
              MAX_CHAR_OBJ
             };

typedef struct {                                        // Char data
  uint16_t  x;
  uint16_t  y;
  char chr;
  const GFXfont *font;
  uint16_t  color;
} wCharObj;

wCharObj charData[MAX_CHAR_OBJ] = {
  { 95, 146, ':', FSSB9, COLOR_BLACK},                  // CHAR_CLK_COLON
  {135,  65, '=', FSSB12, COLOR_BLACK},                 // CHAR_CV_EQUAL
  {206, 105, '0', FSSB9, COLOR_BLACK},                  // CHAR_CV_0
  {186, 105, '1', FSSB9, COLOR_BLACK},                  // CHAR_CV_1
  {166, 105, '2', FSSB9, COLOR_BLACK},                  // CHAR_CV_2
  {146, 105, '3', FSSB9, COLOR_BLACK},                  // CHAR_CV_3
  {126, 105, '4', FSSB9, COLOR_BLACK},                  // CHAR_CV_4
  {106, 105, '5', FSSB9, COLOR_BLACK},                  // CHAR_CV_5
  { 86, 105, '6', FSSB9, COLOR_BLACK},                  // CHAR_CV_6
  { 66, 105, '7', FSSB9, COLOR_BLACK},                  // CHAR_CV_7
  {145, 107, '=', FSSB12, COLOR_BLACK},                 // CHAR_LNCV_EQUAL
  {132,  70, '-', FSSB12, COLOR_WHITE},                 // CHAR_STA_STAM
  {210,  70, '+', FSSB12, COLOR_WHITE},                 // CHAR_STA_STAP
  {132, 110, '-', FSSB12, COLOR_WHITE},                 // CHAR_STA_TURNM
  {210, 110, '+', FSSB12, COLOR_WHITE},                 // CHAR_STA_TURNP
};



////////////////////////////////////////////////////////////
// ***** DATA *****
////////////////////////////////////////////////////////////

enum dataObj {DATA_CALIBRATE, DATA_SW_BOOT, DATA_SW_R, DATA_SW_G, DATA_SW_B,
              MAX_DATA_OBJ
             };


////////////////////////////////////////////////////////////
// ***** FUNCTIONS *****
////////////////////////////////////////////////////////////

#define FNC_WIDTH    32
#define FNC_HEIGHT   32
#define FNC_MAX      28
#define FNC_ICON_MAX 40

enum funcIconObj {  FNC_BLANK_OFF, FNC_BLANK_ON, FNC_NO_ICON_OFF, FNC_NO_ICON_ON, FNC_FUNC_OFF, FNC_FUNC_ON, FNC_LIGHT_OFF, FNC_LIGHT_ON, FNC_INT_LIGHT_OFF, FNC_INT_LIGHT_ON,
                    FNC_UNI_LIGHT_OFF, FNC_UNI_LIGHT_ON, FNC_SOUND_OFF, FNC_SOUND_ON, FNC_GEN_SOUND_OFF, FNC_GEN_SOUND_ON, FNC_ANNOUN_OFF, FNC_ANNOUN_ON, FNC_SLOW_OFF, FNC_SLOW_ON,
                    FNC_ABV_OFF, FNC_ABV_ON, FNC_COUPLER_OFF, FNC_COUPLER_ON, FNC_SMOKE_OFF, FNC_SMOKE_ON, FNC_PANTO_OFF, FNC_PANTO_ON, FNC_BEAM_OFF, FNC_BEAM_ON,
                    FNC_BELL_OFF, FNC_BELL_ON, FNC_HORN_OFF, FNC_HORN_ON, FNC_WHISTLE_OFF, FNC_WHISTLE_ON, FNC_DOOR_OFF, FNC_DOOR_ON, FNC_FAN_OFF, FNC_FAN_ON, FNC_KOHLE_OFF, FNC_KOHLE_ON,
                    FNC_SHIFT_OFF, FNC_SHIFT_ON, FNC_PLATE_OFF, FNC_PLATE_ON, FNC_BRAKE_OFF, FNC_BRAKE_ON, FNC_WHEEL_OFF, FNC_WHEEL_ON, FNC_RADIO_OFF, FNC_RADIO_ON, FNC_COUPLERSND_OFF, FNC_COUPLERSND_ON,
                    FNC_TRACK_OFF, FNC_TRACK_ON, FNC_NOTCHP_OFF, FNC_NOTCHP_ON, FNC_NOTCHM_OFF, FNC_NOTCHM_ON, FNC_PFIFF_OFF, FNC_PFIFF_ON, FNC_UNI_LIGHT2_OFF, FNC_UNI_LIGHT2_ON,
                    FNC_CURVE_OFF, FNC_CURVE_ON, FNC_COMPR_OFF, FNC_COMPR_ON, FNC_AIRBLOW_OFF, FNC_AIRBLOW_ON, FNC_FIRBOX_OFF, FNC_FIREBOX_ON, FNC_SAND_OFF, FNC_SAND_ON, FNC_TABLE_OFF, FNC_TABLE_ON,
                    FNC_CABIN_OFF, FNC_CABIN_ON, FNC_MUTE_OFF, FNC_MUTE_ON, FNC_DIESEL_OFF, FNC_DIESEL_ON,
                    FNC_NEXT_OFF, FNC_NEXT_ON, FNC_PREV_OFF, FNC_PREV_ON, FNC_CV_OFF, FNC_CV_ON, FNC_VALVE_OFF, FNC_VALVE_ON, FNC_FIRE_CL_OFF, FNC_FIRE_CL_ON, FNC_FIRE_OP_OFF, FNC_FIRE_OP_ON,
                    FNC_ST_SMOKE_OFF, FNC_ST_SMOKE_ON,
                    FNC_TURNLD_OFF, FNC_TURNLD_ON, FNC_TURNLS_OFF, FNC_TURNLS_ON, FNC_TURNRD_OFF, FNC_TURNRD_ON, FNC_TURNRS_OFF, FNC_TURNRS_ON,
                    FNC_TURN3L_OFF, FNC_TURN3L_ON, FNC_TURN3R_OFF, FNC_TURN3R_ON, FNC_TURN3S_OFF, FNC_TURN3S_ON, FNC_CROSD_OFF, FNC_CROSD_ON, FNC_CROSS_OFF, FNC_CROSS_ON,
                    FNC_SIGRY_OFF, FNC_SIGRY_ON, FNC_SIGGW_OFF, FNC_SIGGW_ON, FNC_SEMR_OFF, FNC_SEMR_ON, FNC_SEMG_OFF, FNC_SEMG_ON, FNC_SEMY_OFF, FNC_SEMY_ON, FNC_PANR_OFF, FNC_PANR_ON,
                    FNC_PANG_OFF, FNC_PANG_ON, FNC_TTL_OFF, FNC_TTL_ON, FNC_TTR_OFF, FNC_TTR_ON, FNC_TTROT_OFF, FNC_TTROT_ON, FNC_TTTRK_OFF, FNC_TTTRK_ON, FNC_POWER_OFF, FNC_POWER_ON,
                    FNC_KEYPAD_OFF, FNC_KEYPAD_ON, FNC_DCROSSS1_OFF, FNC_DCROSSS1_ON, FNC_DCROSSS2_OFF, FNC_DCROSSS2_ON, FNC_DCROSSD1_OFF, FNC_DCROSSD1_ON, FNC_DCROSSD2_OFF, FNC_DCROSSD2_ON,
                    FNC_BRETELLED_OFF, FNC_BRETELLED_ON, FNC_BRETELLE_OFF, FNC_BRETELLE_ON, FNC_ACC_OFF, FNC_ACC_ON, FNC_STAR_OFF, FNC_STAR_ON, FNC_RAYO_OFF, FNC_RAYO_ON,
                 };

const unsigned char* funcIcon[] = {
  blank32,  blank32,
  blank32,  full32,
  func_off,  func_on,
  light_off,  light_on,
  intlight_off,  intlight_on,
  unilight_off,  unilight_on,
  snd_off,  snd_on,
  gensnd_off,  gensnd_on,
  announce_off,  announce_on,
  maniobra_off,  maniobra_on,
  abv_off,  abv_on,
  uncoupler_off,  uncoupler_on,
  smoke_off,  smoke_on,
  panto_off, panto_on,
  beam_off, beam_on,
  bell_off, bell_on,
  horn_off, horn_on,
  whistle_off, whistle_on,
  door_off, door_on,
  fan_off, fan_on,
  kohle_off, kohle_on,
  shift_off, shift_on,
  plate_off, plate_on,
  brake_off, brake_on,
  wheel_off, wheel_on,
  radio_off, radio_on,
  couplersnd_off, couplersnd_on,
  track_off, track_on,
  notchp_off, notchp_on,
  notchm_off, notchm_on,
  pfiff_off, pfiff_on,
  unilight2_off, unilight2_on,
  curve_off, curve_on,
  compr_off, compr_on,
  airblow_off, airblow_on,
  firebox_off, firebox_on,
  sand_off, sand_on,
  table_off, table_on,
  cabin_off, cabin_on,
  mute_off, mute_on,
  diesel_off, diesel_on,
  nextP, nextP_on,
  prevP, prevP_on,
  readCV_off, readCV_on,
  valve_off, valve_on,
  fire_close_off, fire_close_on,
  fire_open_off, fire_open_on,
  steam_smoke_off, steam_smoke_on,

  turnL_off, tripleL_on,
  turnLS_off, tripleS_on,
  turnR_off, tripleR_on,
  turnRS_off, tripleS_on,
  tripleL_off, tripleL_on,
  tripleR_off, tripleR_on,
  tripleS_off, tripleS_on,
  crossS2_off, crossS2_on,
  crossS1_off, crossS1_on,
  sig_off, sigRY_on,
  sig_off, sigG_on,
  semR_off, semR_on,
  semG_off, semG_on,
  semY_off, semY_on,
  panR_off, panR_on,
  panG_off, panG_on,
  movG_off, movG_on,
  movR_off, movR_on,
  rotTT_off, rotTT_on,
  outTT_off, blank32,
  power, blank32,
  keypad_off, keypad_on,
  dcrossS1_off, dcrossS1_on,    //
  dcrossS2_off, dcrossS2_on,    //
  dcrossD1_off, dcrossD1_on,    //
  dcrossD2_off, dcrossD2_on,    //
  bretelleR_off, bretelleR_on,
  bretelleG_off, bretelleG_on,
  accPanel_off, accPanel_on,

  star_off, star_on,
  rayo_off, rayo_on,
};

enum funcObj {FNC_FX0, FNC_FX1, FNC_FX2, FNC_FX3, FNC_FX4, FNC_FX5, FNC_FX6, FNC_FX7, FNC_FX8, FNC_FX9,
              FNC_F0, FNC_F1, FNC_F2, FNC_F3, FNC_F4, FNC_F5, FNC_F6, FNC_F7, FNC_F8, FNC_F9,
              FNC_F10, FNC_F11, FNC_F12, FNC_F13, FNC_F14, FNC_F15, FNC_F16, FNC_F17, FNC_F18, FNC_F19,
              FNC_F20, FNC_F21, FNC_F22, FNC_F23, FNC_F24, FNC_F25, FNC_F26, FNC_F27, FNC_F28,
              FNC_CHG, FNC_SPEEDO_DIR, FNC_CV_READ, FNC_ST_WATER, FNC_ST_WHISTLE, FNC_ST_TENDER, FNC_ST_FIRE,
              FNC_ST_SMOKE, FNC_UTL_STEAM, FNC_ACC0, FNC_ACC1, FNC_ACC2, FNC_ACC3, FNC_ACC4, FNC_ACC5,
              FNC_ACC6, FNC_ACC7, FNC_ACC8, FNC_ACC9, FNC_ACC10, FNC_ACC11, FNC_ACC12, FNC_ACC13, FNC_ACC14,
              FNC_ACC15, FNC_ASPECT0, FNC_ASPECT1, FNC_ASPECT2, FNC_ASPECT3, FNC_ACC_TYPE,
              FNC_EDIT_ASPECT0, FNC_EDIT_ASPECT1, FNC_EDIT_ASPECT2, FNC_EDIT_ASPECT3, FNC_ACC_PANEL, FNC_SEL_KEYPAD,
              FNC_SCAN_RESET, FNC_STA_STARS, FNC_STA_DIR, FNC_STA_ACC0, FNC_STA_ACC1, FNC_STA_ACC2, FNC_STA_ACC3,
              FNC_STA_STARC, FNC_STA_STAR1, FNC_STA_STAR2, FNC_STA_RAYO,
              MAX_FNC_OBJ
             };

typedef struct {                                        // Function data
  uint16_t  x;
  uint16_t  y;
  uint8_t   num;
  bool      state;
  uint16_t  idIcon;
  uint16_t  color;
  uint16_t  colorOn;
  uint16_t  backgnd;
} wFncObj;

wFncObj fncData[MAX_FNC_OBJ] = {
  { 16, 150,  0, false, FNC_LIGHT_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                // FNC_FX0
  { 16, 210,  1, false, FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_FX1
  { 16, 270,  2, false, FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_FX2
  { 16, 330,  3, false, FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_FX3
  { 16, 390,  4, false, FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_FX4
  {272, 150,  5, false, FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_FX5
  {272, 210,  6, false, FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_FX6
  {272, 270,  7, false, FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_FX7
  {272, 330,  8, false, FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_FX8
  {272, 390,  9, false, FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_FX9

  {  4,  80,  0, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F0
  { 44,  80,  1, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F1
  { 84,  80,  2, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F2
  {124,  80,  3, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F3
  {164,  80,  4, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F4
  {204,  80,  5, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F5
  {  4, 120,  6, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F6
  { 44, 120,  7, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F7
  { 84, 120,  8, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F8
  {124, 120,  9, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F9
  {164, 120, 10, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F10
  {204, 120, 11, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F11
  {  4, 160, 12, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F12
  { 44, 160, 13, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F13
  { 84, 160, 14, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F14
  {124, 160, 15, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F15
  {164, 160, 16, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F16
  {204, 160, 17, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F17
  {  4, 200, 18, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F18
  { 44, 200, 19, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F19
  { 84, 200, 20, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F20
  {124, 200, 21, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F21
  {164, 200, 22, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F22
  {204, 200, 23, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F23
  {  4, 240, 24, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F24
  { 44, 240, 25, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F25
  { 84, 240, 26, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F26
  {124, 240, 27, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F27
  {164, 240, 28, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_F28

  {104, 150, 99, true,  FNC_FUNC_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_CHG

  {104, 204, 99, true,  FNC_NEXT_OFF, COLOR_BLACK, COLOR_YELLOW, COLOR_BACKGROUND},                 // FNC_SPEEDO_DIR
  { 29, 179, 99, true,  FNC_CV_OFF,   COLOR_BLACK, COLOR_ORANGE, COLOR_CREAM},                      // FNC_CV_READ

  { 45, 250, 99, true,  FNC_VALVE_OFF, COLOR_WHITE, COLOR_RED,   COLOR_BLACK},                      // FNC_ST_WATER
  {210,  65, 99, false, FNC_WHISTLE_OFF, COLOR_ORANGE, COLOR_YELLOW, COLOR_BLACK},                  // FNC_ST_WHISTLE
  { 58, 310, 99, true,  FNC_VALVE_OFF, COLOR_WHITE, COLOR_RED,   COLOR_BLACK},                      // FNC_ST_TENDER
  {144, 324, 99, true,  FNC_FIRE_CL_OFF, COLOR_SILVER, COLOR_RED,  COLOR_BLACK},                    // FNC_ST_FIRE
  {144,   0, 99, false, FNC_ST_SMOKE_OFF, COLOR_DARKGREY, COLOR_GHOST_WHITE, COLOR_SKYBLUE},        // FNC_ST_SMOKE

  { 44,  74, 99, true,  FNC_SMOKE_OFF, COLOR_BLACK, COLOR_YELLOW,  COLOR_WHITE},                    // FNC_UTL_STEAM

  { 33,  39, 99, true, FNC_TURNLD_OFF, COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ACC0
  {107,  39, 99, true, FNC_TURNLS_OFF, COLOR_BLACK, COLOR_GREEN, COLOR_LIGHTGREY},                  // FNC_ACC1
  {181,  39, 99, true, FNC_TURNRD_OFF, COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ACC2
  {255,  39, 99, true, FNC_TURNRS_OFF, COLOR_BLACK, COLOR_GREEN, COLOR_LIGHTGREY},                  // FNC_ACC3
  { 33, 129, 99, true, FNC_TURN3L_OFF, COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ACC4
  {107, 129, 99, true, FNC_TURN3R_OFF, COLOR_BLACK, COLOR_GREEN, COLOR_LIGHTGREY},                  // FNC_ACC5
  {181, 129, 99, true, FNC_TURN3S_OFF, COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ACC6
  {255, 129, 99, true, FNC_CROSD_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ACC7
  { 33, 219, 99, true, FNC_CROSS_OFF,  COLOR_BLACK, COLOR_GREEN, COLOR_LIGHTGREY},                  // FNC_ACC8
  {107, 219, 99, true, FNC_SIGRY_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ACC9
  {181, 219, 99, true, FNC_SIGGW_OFF,  COLOR_BLACK, COLOR_GREEN, COLOR_LIGHTGREY},                  // FNC_ACC10
  {255, 219,  3, true, FNC_SIGRY_OFF,  COLOR_BLACK, COLOR_YELLOW, COLOR_LIGHTGREY},                 // FNC_ACC11
  { 33, 309,  4, true, FNC_SIGGW_OFF,  COLOR_BLACK, COLOR_WHITE, COLOR_LIGHTGREY},                  // FNC_ACC12
  {107, 309, 99, true, FNC_SEMR_OFF,   COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ACC13
  {181, 309, 99, true, FNC_SEMG_OFF,   COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ACC14
  {255, 309, 99, true, FNC_SEMY_OFF,   COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ACC15

  { 29, 119, 99, true, FNC_BLANK_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ASPECT0
  { 79, 119, 99, true, FNC_BLANK_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ASPECT1
  {129, 119, 99, true, FNC_BLANK_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ASPECT2
  {179, 119, 99, true, FNC_BLANK_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                    // FNC_ASPECT3

  {104, 150, 99, true, FNC_BLANK_OFF,   COLOR_BLACK, COLOR_YELLOW, COLOR_LIGHTGREY},                // FNC_ACC_TYPE

  { 20,  84, 99, true, FNC_NO_ICON_OFF, COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                   // FNC_EDIT_ASPECT0
  { 20, 124, 99, true, FNC_NO_ICON_OFF, COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                   // FNC_EDIT_ASPECT1
  { 20, 164, 99, true, FNC_NO_ICON_OFF, COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                   // FNC_EDIT_ASPECT2
  { 20, 204, 99, true, FNC_NO_ICON_OFF, COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                   // FNC_EDIT_ASPECT3

  { 85, 420, 99, true, FNC_ACC_OFF,     COLOR_BLACK, COLOR_RED,   COLOR_BACKGROUND},                // FNC_ACC_PANEL
  {134,   8, 99, true, FNC_KEYPAD_OFF,  COLOR_BLACK, COLOR_CYAN,  COLOR_WHITE},                     // FNC_SEL_KEYPAD
  {144, 352, 99, false, FNC_POWER_OFF,  COLOR_RED,   COLOR_RED,   COLOR_GHOST_WHITE},               // FNC_SCAN_RESET

  { 10, 150, 99, true, FNC_STAR_OFF,    COLOR_ORANGE, COLOR_YELLOW, COLOR_BLUE},                    // FNC_STA_STARS
  {144, 234, 99, true, FNC_NEXT_OFF,    COLOR_ORANGE, COLOR_WHITE,  COLOR_BLUE - 0x0010},           // FNC_STA_DIR
  { 20, 384,  1, true, FNC_TURNLD_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                   // FNC_STA_ACC0
  { 76, 384,  2, true, FNC_TURNLD_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                   // FNC_STA_ACC1
  {132, 384,  3, true, FNC_TURNLD_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                   // FNC_STA_ACC2
  {188, 384,  4, true, FNC_TURNLD_OFF,  COLOR_BLACK, COLOR_RED, COLOR_LIGHTGREY},                   // FNC_STA_ACC3
  { 25, 210, 99, true, FNC_STAR_OFF,    COLOR_ORANGE, COLOR_YELLOW, COLOR_BLUE - 0x000D},           // FNC_STA_STARC
  { 30, 129, 99, true, FNC_STAR_OFF,    COLOR_ORANGE, COLOR_YELLOW, COLOR_WHITE},                   // FNC_STA_STAR1
  { 42, 161, 99, true, FNC_STAR_OFF,    COLOR_ORANGE, COLOR_YELLOW, COLOR_WHITE},                   // FNC_STA_STAR2
  {270,  15, 99, false, FNC_RAYO_OFF,   COLOR_DARKGREY, COLOR_YELLOW, COLOR_BLUE},                  // FNC_STA_RAYO
};


////////////////////////////////////////////////////////////
// ***** ICON *****
////////////////////////////////////////////////////////////

enum iconObj {ICON_PACO, ICON_SDCARD, ICON_NO_SD, ICON_WIFI, ICON_NO_WIFI, ICON_WIFI_SSID, ICON_WIFI_CLOSE, ICON_WIFI_CFG,
              ICON_WIFI_OK, ICON_PWD_OK, ICON_PWD_CNCL, ICON_PROT_OK,
              ICON_CAL_OK, ICON_MENU, ICON_FNEXT, ICON_FWD, ICON_REV, ICON_POWER, ICON_WARNING, ICON_WARNING_ON,
              ICON_BLIGHT, ICON_SET_CLOCK, ICON_CLOCK_OK, ICON_CLOCK_CNCL, ICON_INFO, ICON_LOK_EDIT, ICON_SEL_LOK,/* ICON_KEYB,*/
              ICON_LAST_UP, ICON_NUM_UP, ICON_NUM_DWN, ICON_NAME_UP, ICON_NAME_DWN, ICON_EDIT_SAVE, ICON_EDIT_DEL, ICON_EDIT_CNCL,
              ICON_FNC_OK, ICON_FNC_CNCL, ICON_PREV_IMAGE, ICON_NEXT_IMAGE, ICON_INIT_LOCO,
              ICON_MENU_DRIVE, ICON_MENU_ACC, ICON_MENU_CV, ICON_MENU_CFG, ICON_MENU_UTILS,
              ICON_CFG_OK, ICON_CFG_CNCL, ICON_CFG_SCR, ICON_CFG_SPD, ICON_CFG_WIFI, ICON_CFG_FCLK, ICON_CFG_LOCK, ICON_CFG_ABOUT, ICON_CFG_EXIT,
              ICON_CFG_TOUCH, ICON_SCR_OK, ICON_SCR_CNCL, ICON_SPD_OK, ICON_STOP, ICON_ABOUT_PACO, ICON_LOCK, ICON_OPT_OK, ICON_ESTOP,
              ICON_SPEEDO_LOK, ICON_SPEEDO_CNCL, ICON_SPEEDO_RADAR, ICON_SPEEDO_CV, ICON_CV_CNCL, ICON_WAIT, ICON_WAIT_CV, ICON_ADDR, ICON_ADDR_CNCL,
              ICON_MANOMETER, ICON_STEAM_CNCL, ICON_STEAM_EDIT, ICON_UTL_SPEED, ICON_UTL_EXIT, ICON_SURE_OK, ICON_SURE_CNCL, ICON_FIND_LNCV,
              ICON_ACC_CNCL, ICON_ACC_EDIT, ICON_TYPE_OK, ICON_TYPE_CNCL, ICON_KEYB_ACC, ICON_PLUS_ONE, ICON_UTL_SCAN, ICON_UTL_STA,
              ICON_STA_CLOCK, ICON_STA_STATION, ICON_STA_EDIT, ICON_STA_CNCL, ICON_STA_TARGET, ICON_STA_TRAIN, ICON_STA_PIN, ICON_STA_TIME, ICON_STA_COUNT,
              ICON_STA_STOP, ICON_STA_TIMEOUT, ICON_STA_OK, ICON_NET_OK, ICON_NET_CFG, ICON_NET_WIFI, ICON_NET_SCR, ICON_CFG_SW,
              ICON_PREV_ACT, ICON_NEXT_ACT, ICON_ACT_OK, ICON_PREV_ACT_R, ICON_NEXT_ACT_R, ICON_PREV_ACT_G, ICON_NEXT_ACT_G, ICON_PREV_ACT_B, ICON_NEXT_ACT_B,
              ICON_BATT_CHARGE, ICON_CFG_BATT, ICON_BATT_L, ICON_BATT_F, ICON_BATT_OK, ICON_BATT_CNCL, ICON_BATT_CFG, ICON_LOW_BATT, ICON_LBATT_STEAM,
              MAX_ICON_OBJ
             };

typedef struct {                                        // icon data
  uint16_t  x;
  uint16_t  y;
  uint16_t  w;
  uint16_t  h;
  uint16_t  color;
  const uint8_t *bitmap;                                // img2cpp: Arduino Code output. Draw Mode set to Horizontal - 1 bit per pixel. Invert image colors
} wIconObj;

wIconObj iconData[MAX_ICON_OBJ] = {
  {225,  65, 44, 64, COLOR_BLACK,       cara_paco44x64},  // ICON_PACO
  { 20,  68, 16, 16, COLOR_BLACK,       sdcard},          // ICON_SDCARD
  { 28,  76, 16, 16, COLOR_RED,         cancel},          // ICON_NO_SD
  { 65,  64, 32, 24, COLOR_BLACK,       wifi},            // ICON_WIFI
  { 85,  76, 16, 16, COLOR_RED,         cancel},          // ICON_NO_WIFI
  { 50,  92, 32, 24, COLOR_BLACK,       wifi},            // ICON_WIFI_SSID
  {242,  95, 16, 16, COLOR_RED,         cancel},          // ICON_WIFI_CLOSE
  { 10,  10, 32, 24, COLOR_BLACK,       wifi},            // ICON_WIFI_CFG
  { 37, 284, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_WIFI_OK
  {117, 294, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_PWD_OK
  {197, 294, 16, 16, COLOR_RED,         cancel},          // ICON_PWD_CNCL
  {187, 274, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_PROT_OK
  {152, 232, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_CAL_OK
  {272,  11, 32, 32, COLOR_BLACK,       menu},            // ICON_MENU
  {223, 424, 32, 24, COLOR_BLACK,       fncnxt},          // ICON_FNEXT
  {194, 160, 16, 16, COLOR_NAVY,        arrowR},          // ICON_FWD
  {110, 160, 16, 16, COLOR_DARKGREY,    arrowL},          // ICON_REV
  { 13,  11, 32, 32, COLOR_DARKGREY,    power},           // ICON_POWER
  { 25, 144, 32, 32, COLOR_BLACK,       warning_off},     // ICON_WARNING
  { 25, 144, 32, 32, COLOR_YELLOW,      warning_on},      // ICON_WARNING_ON
  { 30,  90, 24, 24, COLOR_BLACK,       brillo},          // ICON_BLIGHT
  { 25, 130, 24, 24, COLOR_BLACK,       Clock},           // ICON_SET_CLOCK
  { 42, 224, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_CLOCK_OK (+17,+4)
  { 97, 224, 16, 16, COLOR_RED,         cancel},          // ICON_CLOCK_CNCL
  { 25, 144, 32, 32, COLOR_NAVY,        info},            // ICON_INFO
  {148, 424, 24, 24, COLOR_BLACK,       wrench},          // ICON_LOK_EDIT
  { 42,   8, 32, 32, COLOR_WHITE,       sel_lok},         // ICON_SEL_LOK
  //{134,  12, 32, 24, COLOR_BLACK,       keyb},            // ICON_KEYB    // keyboard
  //{134,   8, 32, 32, COLOR_WHITE,       keypad_off},      // ICON_KEYB      // keypad
  { 10,  12, 16, 24, COLOR_BLACK,       last_up},         // ICON_LAST_UP
  { 10,  12, 16, 24, COLOR_BLACK,       num_up},          // ICON_NUM_UP
  { 10,  12, 16, 24, COLOR_BLACK,       num_dwn},         // ICON_NUM_DWN
  { 10,  12, 16, 24, COLOR_BLACK,       name_up},         // ICON_NAME_UP
  { 10,  12, 16, 24, COLOR_BLACK,       name_dwn},        // ICON_NAME_DWN
  { 37, 294, 16, 16, COLOR_BLACK,       sdcard},          // ICON_EDIT_SAVE
  {117, 294, 16, 16, COLOR_BLACK,       trash},           // ICON_EDIT_DEL
  {197, 294, 16, 16, COLOR_RED,         cancel},          // ICON_EDIT_CNCL
  {117, 294, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_FNC_OK
  {197, 294, 16, 16, COLOR_RED,         cancel},          // ICON_FNC_CNCL
  { 45, 364, 32, 32, COLOR_BLACK,       prevP},           // ICON_PREV_IMAGE
  {244, 364, 32, 32, COLOR_BLACK,       nextP},           // ICON_NEXT_IMAGE
  {125,  64, 32, 24, COLOR_BLACK,       sel_lok},         // ICON_INIT_LOCO
  { 44, 108, 32, 24, COLOR_WHITE,       sel_lok},         // ICON_MENU_DRIVE
  { 44, 164, 32, 32, COLOR_WHITE,       accessory},       // ICON_MENU_ACC
  { 44, 224, 32, 32, COLOR_WHITE,       prgCV},           // ICON_MENU_CV
  { 44, 284, 32, 32, COLOR_WHITE,       configure},       // ICON_MENU_CFG
  { 48, 348, 24, 24, COLOR_WHITE,       wrench},          // ICON_MENU_UTILS
  {  4,   8, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_CFG_OK
  {  4,   8, 16, 16, COLOR_RED,         cancel},          // ICON_CFG_CNCL
  { 44,  74, 32, 32, COLOR_RED,         screen},          // ICON_CFG_SCR
  { 44, 138, 32, 24, COLOR_BLACK,       sel_lok},         // ICON_CFG_SPD
  { 44, 198, 32, 24, COLOR_BLACK,       wifi},            // ICON_CFG_WIFI
  { 48, 258, 24, 24, COLOR_BLACK,       Clock},           // ICON_CFG_FCLK
  { 48, 318, 24, 24, COLOR_BLACK,       padlock},         // ICON_CFG_LOCK
  { 44, 374, 32, 32, COLOR_NAVY,        info},            // ICON_CFG_ABOUT
  { 44, 434, 32, 32, COLOR_WHITE,       prevP},           // ICON_CFG_EXIT
  {170, 228, 24, 24, COLOR_BLACK,       touchscr},        // ICON_CFG_TOUCH
  { 47, 232, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_SCR_OK
  {107, 232, 16, 16, COLOR_RED,         cancel},          // ICON_SCR_CNCL
  {112, 242, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_SPD_OK
  { 20, 180, 32, 32, COLOR_RED,         stop0},           // ICON_STOP
  {205,  75, 44, 64, COLOR_BLACK,       cara_paco44x64},  // ICON_ABOUT_PACO
  {108, 238, 24, 24, COLOR_BLACK,       padlock},         // ICON_LOCK
  {112, 242, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_OPT_OK
  { 25, 144, 32, 32, COLOR_RED,         stop0},           // ICON_ESTOP
  { 40, 100, 32, 24, COLOR_MAROON,      sel_lok},         // ICON_SPEEDO_LOK
  {197, 204, 16, 16, COLOR_RED,         cancel},          // ICON_SPEEDO_CNCL
  { 40, 142, 32, 24, COLOR_BLACK,       radar},           // ICON_SPEEDO_RADAR
  { 19, 195, 32, 32, COLOR_BLACK,       prgCV},           // ICON_SPEEDO_CV
  { 37, 291, 16, 16, COLOR_RED,         cancel},          // ICON_CV_CNCL
  { 49, 148, 24, 24, COLOR_BLACK,       Clock},           // ICON_WAIT
  { 45, 144, 32, 32, COLOR_BLACK,       prgCV},           // ICON_WAIT_CV
  { 45,  40, 32, 24, COLOR_BLACK,       sel_lok},         // ICON_ADDR
  {112, 291, 16, 16, COLOR_RED,         cancel},          // ICON_ADDR_CNCL
  {110,  75, 60, 60, COLOR_BLACK,       manometro_bar},   // ICON_MANOMETER
  {282,  22, 16, 16, COLOR_RED,         cancel},          // ICON_STEAM_CNCL
  {205, 250, 24, 24, COLOR_SILVER,      wrench},          // ICON_STEAM_EDIT
  { 44,  14, 32, 24, COLOR_BLACK,       radar},           // ICON_UTL_SPEED
  { 44, 434, 32, 32, COLOR_BLACK,       prevP},           // ICON_UTL_EXIT
  {102, 169, 16, 16, COLOR_DARKGREEN,   ok},              // ICON_SURE_OK
  {162, 169, 16, 16, COLOR_RED,         cancel},          // ICON_SURE_CNCL
  {184,  29, 32, 32, COLOR_BLACK,       search},          // ICON_FIND_LNCV
  { 42, 407, 16, 16, COLOR_RED,         cancel},          // ICON_ACC_CNCL
  {260, 403, 24, 24, COLOR_BLACK,       wrench},          // ICON_ACC_EDIT
  { 57, 294, 16, 16, COLOR_GREENYELLOW, ok},              // ICON_TYPE_OK
  {167, 294, 16, 16, COLOR_RED,         cancel},          // ICON_TYPE_CNCL
  {152, 242, 16, 16, COLOR_RED,         cancel},          // ICON_KEYB_ACC
  {192,  13, 32, 24, COLOR_BLACK,       plus_one},        // ICON_PLUS_ONE
  { 44, 138, 32, 24, COLOR_BLACK,       wifi},            // ICON_UTL_SCAN
  { 44, 194, 32, 32, COLOR_BLACK,       sel_lok},         // ICON_UTL_STA
  {160, 154, 24, 24, COLOR_WHITE,       gameclock},       // ICON_STA_CLOCK
  {160,  95, 48, 32, COLOR_GREEN,       station},         // ICON_STA_STATION
  {280, 430, 24, 24, COLOR_WHITE,       wrench},          // ICON_STA_EDIT
  { 52, 434, 16, 16, COLOR_YELLOW,      cancel},          // ICON_STA_CNCL
  { 45, 119, 48, 32, COLOR_SILVER,      station},         // ICON_STA_TARGET
  {140, 119, 48, 32, COLOR_WHITE,       train},           // ICON_STA_TRAIN
  { 53,  89, 32, 32, COLOR_ORANGE,      targetpin},       // ICON_STA_PIN
  {160,  15, 24, 24, COLOR_WHITE,       gameclock},       // ICON_STA_TIME
  { 10,  12, 48, 32, COLOR_GREENYELLOW, station},         // ICON_STA_COUNT
  {252, 242, 16, 16, COLOR_YELLOW,      cancel},          // ICON_STA_STOP
  {24,  148, 24, 24, COLOR_BLACK,       gameclock},       // ICON_STA_TIMEOUT
  {203,  18, 16, 16, COLOR_YELLOW,      ok},              // ICON_STA_OK
  { 47, 222, 16, 16, COLOR_RED,         ok},              // ICON_NET_OK
  {174, 218, 24, 24, COLOR_BLACK,       wrench},          // ICON_NET_CFG
  {125, 276, 32, 24, COLOR_BLACK,       wifi},            // ICON_NET_WIFI
  { 85, 272, 32, 32, COLOR_RED,         screen},          // ICON_NET_SCR
  {170, 174, 32, 32, COLOR_BLACK,       pushbutton},      // ICON_CFG_SW
  {  7,  92, 32, 32, COLOR_WHITE,       prevP},           // ICON_PREV_ACT
  {200,  92, 32, 32, COLOR_WHITE,       nextP},           // ICON_NEXT_ACT
#if (USE_RGB_LED == FUNC_BUTTONS)
  {112, 273, 16, 16, COLOR_YELLOW,      ok},              // ICON_ACT_OK
#else
  {112, 153, 16, 16, COLOR_YELLOW,      ok},              // ICON_ACT_OK
#endif
  {  7, 132, 32, 32, COLOR_CANDY_RED,   prevP},           // ICON_PREV_ACT_R
  {200, 132, 32, 32, COLOR_CANDY_RED,   nextP},           // ICON_NEXT_ACT_R
  {  7, 172, 32, 32, COLOR_GREENYELLOW, prevP},           // ICON_PREV_ACT_G
  {200, 172, 32, 32, COLOR_GREENYELLOW, nextP},           // ICON_NEXT_ACT_G
  {  7, 212, 32, 32, COLOR_CYAN,        prevP},           // ICON_PREV_ACT_B
  {200, 212, 32, 32, COLOR_CYAN,        nextP},           // ICON_NEXT_ACT_B
  { 63, 153, 32, 32, COLOR_WHITE,       rayo_on},         // ICON_BATT_CHARGE
  {172, 124, 16, 32, COLOR_RED,         batt_25},         // ICON_CFG_BATT
  { 62,  99, 16, 32, COLOR_RED,         batt_25},         // ICON_BATT_L
  {162,  99, 16, 32, COLOR_GREEN,       batt_100},        // ICON_BATT_F
  { 62, 227, 16, 16, COLOR_YELLOW,      ok},              // ICON_BATT_OK
  {112, 227, 16, 16, COLOR_RED,         cancel},          // ICON_BATT_CNCL
  {158, 223, 24, 24, COLOR_BLACK,       wrench},          // ICON_BATT_CFG
  { 21,  60, 16, 32, COLOR_RED,         batt_25},         // ICON_LOW_BATT
  { 56,  14, 16, 32, COLOR_RED,         batt_25},         // ICON_LBATT_STEAM

};


////////////////////////////////////////////////////////////
// ***** BUTTON *****
////////////////////////////////////////////////////////////

enum buttonObj {BUT_CAL_OK, BUT_SSID_CLOSE, BUT_WIFI_OK, BUT_PWD_OK, BUT_PWD_CNCL, BUT_PROT_OK, BUT_OPTIONS,
                BUT_CLOCK_OK, BUT_CLOCK_CNCL,
                BUT_EDIT_OK, BUT_EDIT_DEL, BUT_EDIT_CNCL, BUT_EDIT_FUNC, BUT_NAME_OK, BUT_NAME_CNCL,
                BUT_FNC_OK, BUT_FNC_CNCL, BUT_IMAGE_CNCL,
                BUT_MENU_I_DRIVE, BUT_MENU_I_ACC, BUT_MENU_I_CV, BUT_MENU_I_CFG, BUT_MENU_I_UTILS,
                BUT_MENU_T_DRIVE, BUT_MENU_T_ACC, BUT_MENU_T_CV, BUT_MENU_T_CFG, BUT_MENU_T_UTILS,
                BUT_CFG_I_LANG, BUT_CFG_I_SCR, BUT_CFG_I_SPD, BUT_CFG_I_WIFI, BUT_CFG_I_FCLK, BUT_CFG_I_LOCK, BUT_CFG_I_ABOUT,
                BUT_CFG_T_LANG, BUT_CFG_T_SCR, BUT_CFG_T_SPD, BUT_CFG_T_WIFI, BUT_CFG_T_FCLK, BUT_CFG_T_LOCK, BUT_CFG_T_ABOUT,
                BUT_CFG_TOUCH, BUT_SCR_OK, BUT_SCR_CNCL, BUT_SPD_OK, BUT_LOCK, BUT_OPT_OK,
                BUT_SPEEDO_CNCL, BUT_SPEEDO_CV, BUT_SPEEDO_H0, BUT_SPEEDO_N, BUT_SPEEDO_TT, BUT_SPEEDO_Z, BUT_SPEEDO_0,
                BUT_CV_ADDR, BUT_CV_SPD_L, BUT_CV_SPD_M, BUT_CV_SPD_H, BUT_CV_ACC, BUT_CV_DEC, BUT_CV_CFG, BUT_CV_MAN,
                BUT_CV_READ, BUT_CV_CNCL, BUT_CV_LNCV, BUT_CV_0, BUT_CV_1, BUT_CV_2, BUT_CV_3, BUT_CV_4, BUT_CV_5, BUT_CV_6, BUT_CV_7, BUT_ADDR_CNCL,
                BUT_UTL_I_SPEEDO, BUT_UTL_I_STEAM, BUT_UTL_I_SCAN, BUT_UTL_I_STA, BUT_UTL_T_SPEEDO, BUT_UTL_T_STEAM, BUT_UTL_T_SCAN, BUT_UTL_T_STA,
                BUT_STEAM_CNCL, BUT_SURE_OK, BUT_SURE_CNCL, BUT_LNCV_FIND, BUT_LNCV_CNCL, BUT_ACC_0, BUT_ACC_1, BUT_ACC_2, BUT_ACC_3, BUT_ACC_4,
                BUT_ACC_5, BUT_ACC_6, BUT_ACC_7, BUT_ACC_8, BUT_ACC_9, BUT_ACC_10, BUT_ACC_11, BUT_ACC_12, BUT_ACC_13, BUT_ACC_14, BUT_ACC_15,
                BUT_ACC_CNCL, BUT_ACC_EDIT, BUT_ACC_RED, BUT_ACC_GREEN, BUT_ACC_ASPECT0, BUT_ACC_ASPECT1, BUT_ACC_ASPECT2, BUT_ACC_ASPECT3,
                BUT_ACC_OUT0, BUT_ACC_OUT1, BUT_ACC_OUT2, BUT_ACC_OUT3, BUT_ACC_OUT4, BUT_ACC_OUT5, BUT_ACC_OUT6, BUT_ACC_OUT7,
                BUT_ACC_OUT8, BUT_ACC_OUT9, BUT_ACC_OUT10, BUT_ACC_OUT11, BUT_ACC_OUT12, BUT_ACC_OUT13, BUT_ACC_OUT14, BUT_ACC_OUT15,
                BUT_TYPE_OK, BUT_TYPE_CNCL, BUT_STA_START, BUT_STA_CNCL, BUT_STA_ACC0, BUT_STA_ACC1, BUT_STA_ACC2, BUT_STA_ACC3, BUT_STA_STOP,
                BUT_STA_EDIT, BUT_STA_STAM, BUT_STA_STAP, BUT_STA_TURNM, BUT_STA_TURNP, BUT_WIFI_SEL, BUT_WIFI_CFG, BUT_CFG_SW, BUT_ACT_OK,
                BUT_UPDATE, BUT_CFG_BATT, BUT_LOW_BATT, BUT_FULL_BATT, BUT_BATT_OK, BUT_BATT_CNCL, BUT_BATT_CFG,
                MAX_BUT_OBJ
               };

typedef struct {                                        // button data
  uint16_t  x;
  uint16_t  y;
  uint16_t  w;
  uint16_t  h;
  uint16_t  border;
  uint16_t  backgnd;
  uint16_t  objType;
  uint16_t  objID;
} wButtonObj;

wButtonObj buttonData[MAX_BUT_OBJ] = {
  {140, 225,  40, 30, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_CAL_OK},      // BUT_CAL_OK
  {230,  90,  40, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_WIFI_CLOSE},  // BUT_SSID_CLOSE
  { 10, 280,  70, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_WIFI_OK},     // BUT_WIFI_OK
  {100, 290,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_PWD_OK},      // BUT_PWD_OK
  {180, 290,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_PWD_CNCL},    // BUT_PWD_CNCL
  {160, 265,  70, 34, COLOR_AQUA,  COLOR_LIGHTBLACK, OBJ_ICON, ICON_PROT_OK},     // BUT_PROT_OK
  { 10, 265, 120, 34, COLOR_AQUA,  COLOR_LIGHTBLACK, OBJ_LABEL, LBL_OPTIONS},     // BUT_OPTIONS
  { 25, 220,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_CLOCK_OK},    // BUT_CLOCK_OK
  { 80, 220,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_CLOCK_CNCL},  // BUT_CLOCK_CNCL
  { 20, 290,  50, 24, COLOR_AQUA,  COLOR_CREAM,      OBJ_ICON, ICON_EDIT_SAVE},   // BUT_EDIT_OK
  {100, 290,  50, 24, COLOR_AQUA,  COLOR_CREAM,      OBJ_ICON, ICON_EDIT_DEL},    // BUT_EDIT_DEL
  {180, 290,  50, 24, COLOR_AQUA,  COLOR_CREAM,      OBJ_ICON, ICON_EDIT_CNCL},   // BUT_EDIT_CNCL
  { 60, 220, 120, 26, COLOR_AQUA,  COLOR_CREAM,      OBJ_LABEL, LBL_FUNC},        // BUT_EDIT_FUNC
  {100, 290,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_PWD_OK},      // BUT_NAME_OK
  {180, 290,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_PWD_CNCL},    // BUT_NAME_CNCL
  {100, 290,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_FNC_OK},      // BUT_FNC_OK
  {180, 290,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_FNC_CNCL},    // BUT_FNC_CNCL
  {230,  90,  40, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON, ICON_WIFI_CLOSE},  // BUT_IMAGE_CNCL
  { 41, 101,  39, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_ICON, ICON_MENU_DRIVE},  // BUT_MENU_I_DRIVE
  { 41, 161,  39, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_ICON, ICON_MENU_ACC},    // BUT_MENU_I_ACC
  { 41, 221,  39, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_ICON, ICON_MENU_CV},     // BUT_MENU_I_CV
  { 41, 281,  39, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_ICON, ICON_MENU_CFG},    // BUT_MENU_I_CFG
  { 41, 341,  39, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_ICON, ICON_MENU_UTILS},  // BUT_MENU_I_UTILS
  { 81, 101, 198, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_LABEL, LBL_MENU_DRIVE},  // BUT_MENU_T_DRIVE
  { 81, 161, 198, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_LABEL, LBL_MENU_ACC},    // BUT_MENU_T_ACC
  { 81, 221, 198, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_LABEL, LBL_MENU_CV},     // BUT_MENU_T_CV
  { 81, 281, 198, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_LABEL, LBL_MENU_CFG},    // BUT_MENU_T_CFG
  { 81, 341, 198, 39, COLOR_BLACK, COLOR_BLACK,      OBJ_LABEL, LBL_MENU_UTILS},  // BUT_MENU_T_UTILS
  { 41,  11,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_DRAWSTR, DSTR_ENGLISH},  // BUT_CFG_I_LANG
  { 41,  71,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_ICON,  ICON_CFG_SCR},    // BUT_CFG_I_SCR
  { 41, 131,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_ICON,  ICON_CFG_SPD},    // BUT_CFG_I_SPD
  { 41, 191,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_ICON,  ICON_CFG_WIFI},   // BUT_CFG_I_WIFI
  { 41, 251,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_ICON,  ICON_CFG_FCLK},   // BUT_CFG_I_FCLK
  { 41, 311,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_ICON,  ICON_CFG_LOCK},   // BUT_CFG_I_LOCK
  { 41, 371,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_ICON,  ICON_CFG_ABOUT},  // BUT_CFG_I_ABOUT
  { 81,  11, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CFG_LANG},    // BUT_CFG_T_LANG
  { 81,  71, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CFG_SCR},     // BUT_CFG_T_SCR
  { 81, 131, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CFG_SPD},     // BUT_CFG_T_SPD
  { 81, 191, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CFG_WIFI},    // BUT_CFG_T_WIFI
  { 81, 251, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CFG_FCLK},    // BUT_CFG_T_FCLK
  { 81, 311, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CFG_LOCK},    // BUT_CFG_T_LOCK
  { 81, 371, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CFG_ABOUT},   // BUT_CFG_T_ABOUT
  {150, 220,  60, 40, COLOR_AQUA,  COLOR_CREAM,      OBJ_ICON,  ICON_CFG_TOUCH},  // BUT_CFG_TOUCH
  { 30, 220,  50, 40, COLOR_AQUA,  COLOR_LIGHTBLACK, OBJ_ICON,  ICON_SCR_OK},     // BUT_SCR_OK
  { 90, 220,  50, 40, COLOR_AQUA,  COLOR_LIGHTBLACK, OBJ_ICON,  ICON_SCR_CNCL},   // BUT_SCR_CNCL
  { 95, 230,  50, 40, COLOR_AQUA,  COLOR_LIGHTBLACK, OBJ_ICON,  ICON_SPD_OK},     // BUT_SPD_OK
  { 95, 230,  50, 40, COLOR_AQUA,  COLOR_CREAM,      OBJ_ICON,  ICON_LOCK},       // BUT_LOCK
  { 95, 230,  50, 40, COLOR_AQUA,  COLOR_LIGHTBLACK, OBJ_ICON,  ICON_OPT_OK},     // BUT_OPT_OK
  {185, 191,  40, 40, COLOR_AQUA,  COLOR_CREAM,      OBJ_ICON,  ICON_SPEEDO_CNCL},// BUT_SPEEDO_CNCL
  { 15, 191,  40, 40, COLOR_BLACK, COLOR_WHITE,      OBJ_ICON,  ICON_SPEEDO_CV},  // BUT_SPEEDO_CV
  { 10,  80,  60, 30, COLOR_AQUA,  COLOR_CREAM,      OBJ_LABEL, LBL_SCALE_H0},    // BUT_SPEEDO_H0
  { 10, 120,  60, 30, COLOR_AQUA,  COLOR_CREAM,      OBJ_LABEL, LBL_SCALE_N},     // BUT_SPEEDO_N
  { 10, 160,  60, 30, COLOR_AQUA,  COLOR_CREAM,      OBJ_LABEL, LBL_SCALE_TT},    // BUT_SPEEDO_TT
  { 10, 200,  60, 30, COLOR_AQUA,  COLOR_CREAM,      OBJ_LABEL, LBL_SCALE_Z},     // BUT_SPEEDO_Z
  { 10, 240,  60, 30, COLOR_AQUA,  COLOR_CREAM,      OBJ_LABEL, LBL_SCALE_0},     // BUT_SPEEDO_0
  { 41,  11, 238, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CV_ADDR},     // BUT_CV_ADDR
  { 41,  71, 238, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CV_SPD_L},    // BUT_CV_SPD_L
  { 41, 131, 238, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CV_SPD_M},    // BUT_CV_SPD_M
  { 41, 191, 238, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CV_SPD_H},    // BUT_CV_SPD_H
  { 41, 251, 238, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CV_ACC},      // BUT_CV_ACC
  { 41, 311, 238, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CV_DEC},      // BUT_CV_DEC
  { 41, 371, 238, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CV_CFG},      // BUT_CV_CFG
  { 41, 431, 238, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_CV_MAN},      // BUT_CV_MAN
  { 15, 175,  60, 40, COLOR_WHITE, COLOR_CREAM,      OBJ_FNC,   FNC_CV_READ},     // BUT_CV_READ
  { 15, 280,  60, 35, COLOR_WHITE, COLOR_CREAM,      OBJ_ICON,  ICON_CV_CNCL},    // BUT_CV_CNCL
  { 15, 230,  60, 35, COLOR_WHITE, COLOR_CREAM,      OBJ_LABEL, LBL_LNCV},        // BUT_CV_LNCV
  {202,  91,  18, 18, COLOR_WHITE, COLOR_BROWN,      OBJ_CHAR, CHAR_CV_0},        // BUT_CV_0
  {182,  91,  18, 18, COLOR_WHITE, COLOR_BROWN,      OBJ_CHAR, CHAR_CV_1},        // BUT_CV_1
  {162,  91,  18, 18, COLOR_WHITE, COLOR_BROWN,      OBJ_CHAR, CHAR_CV_2},        // BUT_CV_2
  {142,  91,  18, 18, COLOR_WHITE, COLOR_BROWN,      OBJ_CHAR, CHAR_CV_3},        // BUT_CV_3
  {122,  91,  18, 18, COLOR_WHITE, COLOR_BROWN,      OBJ_CHAR, CHAR_CV_4},        // BUT_CV_4
  {102,  91,  18, 18, COLOR_WHITE, COLOR_BROWN,      OBJ_CHAR, CHAR_CV_5},        // BUT_CV_5
  { 82,  91,  18, 18, COLOR_WHITE, COLOR_BROWN,      OBJ_CHAR, CHAR_CV_6},        // BUT_CV_6
  { 62,  91,  18, 18, COLOR_WHITE, COLOR_BROWN,      OBJ_CHAR, CHAR_CV_7},        // BUT_CV_7
  { 90, 280,  60, 35, COLOR_WHITE, COLOR_CREAM,      OBJ_ICON, ICON_ADDR_CNCL},   // BUT_ADDR_CNCL

  { 41,  11,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_ICON,  ICON_UTL_SPEED},  // BUT_UTL_I_SPEEDO
  { 41,  71,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_FNC,   FNC_UTL_STEAM},   // BUT_UTL_I_STEAM
  { 41, 131,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_ICON,  ICON_UTL_SCAN},   // BUT_UTL_I_SCAN
  { 41, 191,  39, 39, COLOR_WHITE, COLOR_WHITE,      OBJ_ICON,  ICON_UTL_STA},    // BUT_UTL_I_STA
  { 81,  11, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_UTIL_SPEED},  // BUT_UTL_T_SPEEDO
  { 81,  71, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_UTIL_STEAM},  // BUT_UTL_T_STEAM
  { 81, 131, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_UTIL_SCAN},   // BUT_UTL_T_SCAN
  { 81, 191, 198, 39, COLOR_NAVY,  COLOR_WHITE,      OBJ_LABEL, LBL_UTIL_STA},    // BUT_UTL_T_STA

  {265,  11,  40, 32, COLOR_SKYBLUE, COLOR_SKYBLUE,  OBJ_ICON, ICON_STEAM_CNCL},  // BUT_STEAM_CNCL

  { 85, 165,  50, 24, COLOR_AQUA, COLOR_CREAM,       OBJ_ICON, ICON_SURE_OK},     // BUT_SURE_OK
  {145, 165,  50, 24, COLOR_AQUA, COLOR_CREAM,       OBJ_ICON, ICON_SURE_CNCL},   // BUT_SURE_CNCL

  {180,  25,  40, 40, COLOR_AQUA, COLOR_CREAM,       OBJ_ICON, ICON_FIND_LNCV},   // BUT_LNCV_FIND
  { 15, 280,  60, 35, COLOR_WHITE, COLOR_CREAM,      OBJ_ICON, ICON_CV_CNCL},     // BUT_LNCV_CNCL

  { 24,   30, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC0},         // BUT_ACC_0
  { 98,   30, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC1},         // BUT_ACC_1
  {172,   30, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC2},         // BUT_ACC_2
  {246,   30, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC3},         // BUT_ACC_3
  { 24,  120, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC4},         // BUT_ACC_4
  { 98,  120, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC5},         // BUT_ACC_5
  {172,  120, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC6},         // BUT_ACC_6
  {246,  120, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC7},         // BUT_ACC_7
  { 24,  210, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC8},         // BUT_ACC_8
  { 98,  210, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC9},         // BUT_ACC_9
  {172,  210, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC10},        // BUT_ACC_10
  {246,  210, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC11},        // BUT_ACC_11
  { 24,  300, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC12},        // BUT_ACC_12
  { 98,  300, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC13},        // BUT_ACC_13
  {172,  300, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC14},        // BUT_ACC_14
  {246,  300, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ACC15},        // BUT_ACC_15
  { 24,  390, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_ICON, ICON_ACC_CNCL},    // BUT_ACC_CNCL
  {246,  390, 50, 50, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_ICON, ICON_ACC_EDIT},    // BUT_ACC_EDIT
  {160,  35,  50, 40, COLOR_WHITE, COLOR_RED,        OBJ_UNDEF, OBJ_UNDEF},       // BUT_ACC_RED
  { 20,  35,  50, 40, COLOR_WHITE, COLOR_GREEN,      OBJ_UNDEF, OBJ_UNDEF},       // BUT_ACC_GREEN
  { 25, 115,  40, 40, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ASPECT0},      // BUT_ACC_ASPECT0
  { 75, 115,  40, 40, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ASPECT1},      // BUT_ACC_ASPECT1
  {125, 115,  40, 40, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ASPECT2},      // BUT_ACC_ASPECT2
  {175, 115,  40, 40, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_FNC,  FNC_ASPECT3},      // BUT_ACC_ASPECT3

  { 80,  88,  24, 24, COLOR_WHITE, COLOR_GREEN,      OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT0
  {116,  88,  24, 24, COLOR_WHITE, COLOR_RED,        OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT1
  {160,  88,  24, 24, COLOR_WHITE, COLOR_GREEN,      OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT2
  {196,  88,  24, 24, COLOR_WHITE, COLOR_RED,        OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT3
  { 80, 128,  24, 24, COLOR_WHITE, COLOR_GREEN,      OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT4
  {116, 128,  24, 24, COLOR_WHITE, COLOR_RED,        OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT5
  {160, 128,  24, 24, COLOR_WHITE, COLOR_GREEN,      OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT6
  {196, 128,  24, 24, COLOR_WHITE, COLOR_RED,        OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT7
  { 80, 168,  24, 24, COLOR_WHITE, COLOR_GREEN,      OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT8
  {116, 168,  24, 24, COLOR_WHITE, COLOR_RED,        OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT9
  {160, 168,  24, 24, COLOR_WHITE, COLOR_GREEN,      OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT10
  {196, 168,  24, 24, COLOR_WHITE, COLOR_RED,        OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT11
  { 80, 208,  24, 24, COLOR_WHITE, COLOR_GREEN,      OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT12
  {116, 208,  24, 24, COLOR_WHITE, COLOR_RED,        OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT13
  {160, 208,  24, 24, COLOR_WHITE, COLOR_GREEN,      OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT14
  {196, 208,  24, 24, COLOR_WHITE, COLOR_RED,        OBJ_UNDEF,  OBJ_UNDEF},      // BUT_ACC_OUT15
  { 40, 168,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON,   ICON_TYPE_OK},   // BUT_TYPE_OK
  {150, 168,  50, 24, COLOR_WHITE, COLOR_LIGHTBLACK, OBJ_ICON,   ICON_TYPE_CNCL}, // BUT_TYPE_CNCL

  {100, 210, 120, 40, COLOR_WHITE, COLOR_ORANGE,     OBJ_LABEL, LBL_STA_START},   // BUT_STA_START
  { 30, 426,  60, 32, COLOR_WHITE, COLOR_BLUE,       OBJ_ICON,  ICON_STA_CNCL},   // BUT_STA_CNCL

  { 16, 370,  40, 60, COLOR_WHITE,  COLOR_LIGHTGREY, OBJ_FNC,   FNC_STA_ACC0},    // BUT_STA_ACC0
  { 72, 370,  40, 60, COLOR_WHITE,  COLOR_LIGHTGREY, OBJ_FNC,   FNC_STA_ACC1},    // BUT_STA_ACC1
  {128, 370,  40, 60, COLOR_WHITE,  COLOR_LIGHTGREY, OBJ_FNC,   FNC_STA_ACC2},    // BUT_STA_ACC2
  {184, 370,  40, 60, COLOR_WHITE,  COLOR_LIGHTGREY, OBJ_FNC,   FNC_STA_ACC3},    // BUT_STA_ACC3
  {240, 230,  40, 40, COLOR_WHITE,  COLOR_BLACK,     OBJ_ICON,  ICON_STA_STOP},   // BUT_STA_STOP
  {195,  10,  40, 32, COLOR_WHITE,  COLOR_LIGHTGREY, OBJ_ICON,  ICON_STA_OK},     // BUT_STA_EDIT
  {120,  50,  32, 32, COLOR_WHITE,  COLOR_BLACK,     OBJ_CHAR,  CHAR_STA_STAM},   // BUT_STA_STAM
  {200,  50,  32, 32, COLOR_WHITE,  COLOR_BLACK,     OBJ_CHAR,  CHAR_STA_STAP},   // BUT_STA_STAP
  {120,  90,  32, 32, COLOR_WHITE,  COLOR_BLACK,     OBJ_CHAR,  CHAR_STA_TURNM},  // BUT_STA_TURNM
  {200,  90,  32, 32, COLOR_WHITE,  COLOR_BLACK,     OBJ_CHAR,  CHAR_STA_TURNP},  // BUT_STA_TURNP

  { 30, 210,  50, 40, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_ICON,  ICON_NET_OK},     // BUT_WIFI_SEL
  {160, 210,  50, 40, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_ICON,  ICON_NET_CFG},    // BUT_WIFI_CFG
  {150, 170,  60, 40, COLOR_AQUA,  COLOR_WHITE,      OBJ_ICON,  ICON_CFG_SW},     // BUT_CFG_SW
#if (USE_RGB_LED == FUNC_BUTTONS)
  { 90, 265,  60, 32, COLOR_WHITE, COLOR_LIGHTGREY,  OBJ_ICON,  ICON_ACT_OK},     // BUT_ACT_OK
#else
  { 90, 145,  60, 32, COLOR_WHITE, COLOR_LIGHTGREY,  OBJ_ICON,  ICON_ACT_OK},     // BUT_ACT_OK
#endif
  { 60, 250, 200, 40, COLOR_BLUE,  COLOR_ORANGE,     OBJ_LABEL, LBL_UPDATE},      // BUT_UPDATE
  {150, 120,  60, 40, COLOR_AQUA,  COLOR_WHITE,      OBJ_ICON,  ICON_CFG_BATT},   // BUT_CFG_BATT

  { 50,  95,  40, 40, COLOR_AQUA,  COLOR_WHITE,      OBJ_ICON,  ICON_BATT_L},     // BUT_LOW_BATT
  {150,  95,  40, 40, COLOR_AQUA,  COLOR_WHITE,      OBJ_ICON,  ICON_BATT_F},     // BUT_FULL_BATT
  { 50, 215,  40, 40, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_ICON,  ICON_BATT_OK},    // BUT_BATT_OK
  {100, 215,  40, 40, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_ICON,  ICON_BATT_CNCL},  // BUT_BATT_CNCL
  {150, 215,  40, 40, COLOR_AQUA,  COLOR_LIGHTGREY,  OBJ_ICON,  ICON_BATT_CFG},   // BUT_BATT_CFG

};


////////////////////////////////////////////////////////////
// ***** RADIO BUTTON *****
////////////////////////////////////////////////////////////

enum radioObj {RAD_STOP_MODE, RAD_PROTOCOL, RAD_PROTOCOL_LN, RAD_CSTATION, RAD_NETWORKS,
               MAX_RAD_OBJ
              };

typedef struct {                                        // radio button data
  uint16_t  x;
  uint16_t  y;
  uint16_t  h;
  uint16_t  r;
  uint16_t  num;
  uint16_t  value;
  uint16_t  border;
  uint16_t  backgnd;
} wRadioObj;

wRadioObj radioData[MAX_RAD_OBJ] = {
  { 50, 165, 30, 10, 2, 0, COLOR_BLUE, COLOR_LIGHTGREY},    //  RAD_STOP_MODE
  { 10,  40, 35, 10, 4, 0, COLOR_BLUE, COLOR_LIGHTGREY},    //  RAD_PROTOCOL
  { 50, 180, 35, 10, 2, 0, COLOR_BLUE, COLOR_LIGHTGREY},    //  RAD_PROTOCOL_LN
  { 20, 110, 35, 10, 3, 0, COLOR_BLUE, COLOR_LIGHTGREY},    //  RAD_CSTATION
  { 10,  46, 35, 10, 4, 0, COLOR_BLUE, COLOR_LIGHTGREY},    //  RAD_NETWORKS

};

////////////////////////////////////////////////////////////
// ***** PROGRESS BAR *****
////////////////////////////////////////////////////////////

enum barObj {BAR_INIT, BAR_BLIGHT, BAR_WAIT, BAR_JOHNSON, BAR_WATER, BAR_TENDER, BAR_BRAKE, BAR_UPDATE,
             BAR_BATT,
             MAX_BAR_OBJ
            };

typedef struct {                                        // Progress bar data
  uint16_t  x;
  uint16_t  y;
  uint16_t  w;
  uint16_t  h;
  uint16_t  r;
  uint16_t  colorOn;
  uint16_t  colorOff;
  uint16_t  border;
  uint16_t  backgnd;
  uint16_t  min;
  uint16_t  max;
  uint16_t  value;
} wBarObj;

wBarObj barData[MAX_BAR_OBJ] = {
  { 60, 150, 200,  20,  0, COLOR_DARKCYAN, COLOR_CYAN, COLOR_NAVY, COLOR_WHITE, 0, 100, 0},            // BAR_INIT
  { 80,  95, 128,  12, 10, COLOR_NAVY, COLOR_WHITE, COLOR_AQUA, COLOR_WHITE, USER_MIN_BL, 255, 0},     // BAR_BLIGHT
  { 85, 154, 100,  12,  0, COLOR_DARKCYAN, COLOR_CYAN, COLOR_NAVY, COLOR_WHITE, 0, 100, 0},            // BAR_WAIT
  {270, 240,  10, 130, 10, COLOR_DARKGREY, COLOR_LIGHTGREY, COLOR_RED, COLOR_BLACK, 0, 6, 3},          // BAR_JOHNSON
  { 50, 175,  12,  60,  0, COLOR_BLUE, COLOR_LIGHTGREY, COLOR_WHITE, COLOR_BLACK, 0, 50, 40},          // BAR_WATER
  { 15, 305,  10,  50,  0, COLOR_BLUE, COLOR_LIGHTGREY, COLOR_WHITE, COLOR_BLACK, 0, 500, 400},        // BAR_TENDER
  {255, 165,  50,   8,  8, COLOR_DARKGREY, COLOR_LIGHTGREY, COLOR_RED, COLOR_BLACK, 0, 4, 3},          // BAR_BRAKE
  { 60, 250, 200,  40,  0, COLOR_DARKCYAN, COLOR_CYAN, COLOR_NAVY, COLOR_WHITE, 0, 100, 0},            // BAR_UPDATE
  { 60, 145, 120,  48,  0, COLOR_GREEN, COLOR_DARKGREY, COLOR_AQUA, COLOR_WHITE, 0, 120, 20},          // BAR_BATT
};


////////////////////////////////////////////////////////////
// ***** LOCO PICTURE *****
////////////////////////////////////////////////////////////

#define LPIC_WIDTH    190
#define LPIC_HEIGHT    40

enum locoPic {SYS_NO_LOK, SYS_ELOK, SYS_LOCO_2, SYS_LOCO_3, SYS_LOCO_4, SYS_LOCO_5, SYS_LOCO_6, SYS_LOCO_7, SYS_LOCO_8, SYS_LOCO_9, MAX_SYS_LPIC};

enum locoPicObj {LPIC_MAIN, LPIC_LOK_EDIT, LPIC_SEL_IMG1, LPIC_SEL_IMG2, LPIC_SEL_IMG3, LPIC_SEL_IMG4, LPIC_SEL_IMG5, LPIC_SEL_IMG6,
                 LPIC_SPEEDO,
                 MAX_LPIC_OBJ
                };

const unsigned char* sysLocoPic[] = {
  sysNoLoco, sysLocoPic0, sysLocoPic1, sysLocoPic2, sysLocoPic3, sysLocoPic4,
  sysLocoPic5, sysLocoPic6, sysLocoPic7, sysLocoPic8
};

typedef struct {                                        // Loco picture data (190x40 pixel)
  uint16_t  x;
  uint16_t  y;
  uint16_t  id;
} wLpicObj;

wLpicObj lpicData[MAX_LPIC_OBJ] = {
  { 65,  52, SYS_NO_LOK},                               // LPIC_MAIN
  { 25,  80, SYS_NO_LOK},                               // LPIC_LOK_EDIT
  { 65, 120, SYS_NO_LOK},                               // LPIC_SEL_IMG1
  { 65, 160, SYS_NO_LOK},                               // LPIC_SEL_IMG2
  { 65, 200, SYS_NO_LOK},                               // LPIC_SEL_IMG3
  { 65, 240, SYS_NO_LOK},                               // LPIC_SEL_IMG4
  { 65, 280, SYS_NO_LOK},                               // LPIC_SEL_IMG5
  { 65, 320, SYS_NO_LOK},                               // LPIC_SEL_IMG6
  { 25,  16, SYS_NO_LOK},                               // LPIC_SPEEDO

};

////////////////////////////////////////////////////////////
// ***** GAUGE *****
////////////////////////////////////////////////////////////

enum gaugeObj {GAUGE_SPEED, GAUGE_SPEEDO, GAUGE_STATION,
               MAX_GAUGE_OBJ
              };

typedef struct {                                        // Gauge data
  uint16_t  x;
  uint16_t  y;
  uint16_t  r;                                          // Speed gauge has Radius = 0
  uint16_t  color;
  uint16_t  backgnd;
  uint16_t  value;                                      // 0..255
  uint16_t  oldValue;
} wGaugeObj;

wGaugeObj gaugeData[MAX_GAUGE_OBJ] = {
  { 160, 290, 0, COLOR_BLACK,   COLOR_DARKGREY,  0, 0}, // GAUGE_SPEED
  { 120, 220, 40, COLOR_BLUE,   COLOR_CYAN, 128, 0},    // GAUGE_SPEEDO
  { 160, 250, 46, COLOR_ORANGE, COLOR_CYAN, 128, 0},    // GAUGE_STATION

};


////////////////////////////////////////////////////////////
// ***** TEXTBOX *****
////////////////////////////////////////////////////////////

#define NAME_LNG    16                                  // loco names length
#define ADDR_LNG    4                                   // loco addr length
#define SSID_LNG   24
#define PWD_LNG    32
#define IP_LNG      3
#define PORT_LNG    5
#define PANEL_LNG  12
#define ACC_LNG     6

char ssidName[SSID_LNG + 1];
char ssidName1[SSID_LNG + 1];
char ssidName2[SSID_LNG + 1];
char ssidName3[SSID_LNG + 1];
char ssidName4[SSID_LNG + 1];
char ssidName5[SSID_LNG + 1];
char ssidName6[SSID_LNG + 1];
char keybIP1Buf[IP_LNG + 1];                            // IP keyboard
char keybIP2Buf[IP_LNG + 1];
char keybIP3Buf[IP_LNG + 1];
char keybIP4Buf[IP_LNG + 1];
char keybPwdHideBuf[NAME_LNG + 1];
char keybPortBuf[PORT_LNG + 1];
char keybPwdBuf[PWD_LNG + 1];
char keybProtoBuf[PWD_LNG + 1];
char locoName[NAME_LNG + 1];
char locoAddr[ADDR_LNG + 1];
char clockBuf[NAME_LNG + 1];
char keybHourBuf[3];
char keybMinBuf[3];
char keybRateBuf[4];
char locoEditName[NAME_LNG + 1];
char locoEditAddr[ADDR_LNG + 1];
char locoEditID[ADDR_LNG + 1];
char locoEditVmax[ADDR_LNG + 1];
char keybNameBuf[NAME_LNG + 1];
char locoEditFunc[ADDR_LNG + 1];
char selLocoAddr1[ADDR_LNG + 1];
char selLocoAddr2[ADDR_LNG + 1];
char selLocoAddr3[ADDR_LNG + 1];
char selLocoAddr4[ADDR_LNG + 1];
char selLocoAddr5[ADDR_LNG + 1];
char selLocoAddr6[ADDR_LNG + 1];
char selLocoName1[NAME_LNG + 1];
char selLocoName2[NAME_LNG + 1];
char selLocoName3[NAME_LNG + 1];
char selLocoName4[NAME_LNG + 1];
char selLocoName5[NAME_LNG + 1];
char selLocoName6[NAME_LNG + 1];
char locoKeybAddr[ADDR_LNG + 1];
char aboutPacoMouseCYD[PWD_LNG + 1];
char aboutIP[PWD_LNG + 1];
char aboutMAC[PWD_LNG + 1];
char spdScaleBuf[NAME_LNG + 1];
char spdSelScaleBuf[NAME_LNG + 1];
char spdSelScaleNumBuf[IP_LNG + 1];
char spdLengthBuf[NAME_LNG + 1];
char spdSpeedBuf[NAME_LNG + 1];
char speedoKeybLng[PORT_LNG + 1];
char keybCvBuf[ADDR_LNG + 1];
char keybCvValBuf[IP_LNG + 1];
char cvStatusBuf[PWD_LNG + 1];
char keybLncvArtBuf[PORT_LNG + 1];
char keybLncvModBuf[PORT_LNG + 1];
char keybLncvAdrBuf[PORT_LNG + 1];
char keybLncvValBuf[PORT_LNG + 1];
char accNamesBuf[16][ACC_LNG + 1];
char panelNameBuf[PANEL_LNG + 1];
char panelNamesBuf[16][PANEL_LNG + 1];
char panelLabelBuf[MAX_LABEL_LNG + 1];
char accKeybAddr[ADDR_LNG + 1];
char accKeybAddr1[ADDR_LNG + 1];
char accKeybAddr2[ADDR_LNG + 1];
char accKeybName[ACC_LNG + 1];
char accKeybAdrEdit[ADDR_LNG + 1];
char staLevelBuf[ADDR_LNG + 1];
char staStationsBuf[ACC_LNG + 1];
char staStarsBuf[ADDR_LNG + 1];
char staTimeBuf[ACC_LNG + 1];
char staStartTimeBuf[IP_LNG + 1];
char staStatNumBuf[IP_LNG + 1];
char staTurnNumBuf[IP_LNG + 1];
char staTurnout1Buf[ADDR_LNG + 1];
char staTurnout2Buf[ADDR_LNG + 1];
char staTurnout3Buf[ADDR_LNG + 1];
char staTurnout4Buf[ADDR_LNG + 1];
char networkNamesBuf[4][NAME_LNG + 1];
char networkNameBuf[NAME_LNG + 1];
char keybActionBuf[4][NAME_LNG + 1];
char battBuf[NAME_LNG + 1];

enum textObj {TXT_SSID1, TXT_SSID2, TXT_SSID3, TXT_SSID4, TXT_SSID5, TXT_SSID6,
              TXT_IP1, TXT_IP2, TXT_IP3, TXT_IP4, TXT_PORT, TXT_SSID, TXT_PWD_HIDE, TXT_PWD, TXT_PROTOCOL,
              TXT_LOCO_NAME, TXT_LOCO_ADDR, TXT_CLOCK, TXT_HOUR, TXT_MIN, TXT_RATE,
              TXT_EDIT_ADDR, TXT_EDIT_NAME, TXT_EDIT_IMAGE, TXT_EDIT_VMAX, TXT_NAME, TXT_EDIT_FNC, TXT_KEYB_VMAX,
              TXT_SEL_ADDR1, TXT_SEL_ADDR2, TXT_SEL_ADDR3, TXT_SEL_ADDR4, TXT_SEL_ADDR5, TXT_SEL_ADDR6,
              TXT_SEL_NAME1, TXT_SEL_NAME2, TXT_SEL_NAME3, TXT_SEL_NAME4, TXT_SEL_NAME5, TXT_SEL_NAME6,
              TXT_KEYB_ADDR, TXT_ABOUT, TXT_ABOUT_IP, TXT_ABOUT_MAC,
              TXT_SPEEDO_SCALE, TXT_SPEEDO_LNG, TXT_SPEEDO_SPD, TXT_EDIT_LNG, TXT_EDIT_SCALE, TXT_NUM_SCALE,
              TXT_CV, TXT_CV_VAL, TXT_CV_STATUS, TXT_CV_ADDR, TXT_LNCV_ART, TXT_LNCV_MOD, TXT_LNCV_ADR, TXT_LNCV_VAL,
              TXT_ACC_0, TXT_ACC_1, TXT_ACC_2, TXT_ACC_3, TXT_ACC_4, TXT_ACC_5, TXT_ACC_6, TXT_ACC_7,
              TXT_ACC_8, TXT_ACC_9, TXT_ACC_10, TXT_ACC_11, TXT_ACC_12, TXT_ACC_13, TXT_ACC_14, TXT_ACC_15,
              TXT_PANEL, TXT_PANEL0, TXT_PANEL1, TXT_PANEL2, TXT_PANEL3, TXT_PANEL4, TXT_PANEL5, TXT_PANEL6, TXT_PANEL7,
              TXT_PANEL8, TXT_PANEL9, TXT_PANEL10, TXT_PANEL11, TXT_PANEL12, TXT_PANEL13, TXT_PANEL14, TXT_PANEL15,
              TXT_ACC_ADDR, TXT_ACC_ADDR1, TXT_ACC_ADDR2, TXT_ACC_NAME, TXT_ACC_EDIT, TXT_STA_LEVEL, TXT_STA_STARS,
              TXT_STA_STATION, TXT_STA_CLOCK, TXT_STA_TIME, TXT_STA_COUNT, TXT_STA_STARC, TXT_STA_STARTTIME,
              TXT_STA_STATNUM, TXT_STA_TURNNUM, TXT_STA_TURNOUT1, TXT_STA_TURNOUT2, TXT_STA_TURNOUT3, TXT_STA_TURNOUT4,
              TXT_NETWORK, TXT_NETWORK_0, TXT_NETWORK_1, TXT_NETWORK_2, TXT_NETWORK_3, TXT_ACTION_BOOT, TXT_ACTION_R,
              TXT_ACTION_G, TXT_ACTION_B, TXT_BATT,
              MAX_TXT_OBJ
             };

typedef struct {                                        // Textbox data
  uint16_t  x;
  uint16_t  y;
  uint16_t  w;
  uint16_t  h;
  uint16_t  color;
  uint16_t  backgnd;
  uint16_t  border;
  bool      alignCenter;
  uint16_t  maxLength;
  char      *buf;
  const     GFXfont *font;
} wTxtObj;

wTxtObj txtData[MAX_TXT_OBJ] = {
  { 41, 130, 238, 40, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, false, SSID_LNG, ssidName1, FSS9},                     // TXT_SSID1
  { 41, 170, 238, 40, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, false, SSID_LNG, ssidName2, FSS9},                     // TXT_SSID2
  { 41, 210, 238, 40, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, false, SSID_LNG, ssidName3, FSS9},                     // TXT_SSID3
  { 41, 250, 238, 40, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, false, SSID_LNG, ssidName4, FSS9},                     // TXT_SSID4
  { 41, 290, 238, 40, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, false, SSID_LNG, ssidName5, FSS9},                     // TXT_SSID5
  { 41, 330, 238, 40, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, false, SSID_LNG, ssidName6, FSS9},                     // TXT_SSID6
  { 55, 134,  40, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE, true, IP_LNG, keybIP1Buf, FSS9},                  // TXT_IP1
  {100, 134,  40, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE, true, IP_LNG, keybIP2Buf, FSS9},                  // TXT_IP2
  {145, 134,  40, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE, true, IP_LNG, keybIP3Buf, FSS9},                  // TXT_IP3
  {190, 134,  40, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE, true, IP_LNG, keybIP4Buf, FSS9},                  // TXT_IP4
  { 80, 172,  60, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE, true, PORT_LNG, keybPortBuf, FSS9},               // TXT_PORT
  { 10,  58, 220, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE, false, SSID_LNG, ssidName, FSS9},                 // TXT_SSID
  {140,  96,  90, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE, false, NAME_LNG, keybPwdHideBuf, FSS9},           // TXT_PWD_HIDE
  { 10, 135, 220, 24, COLOR_BLACK, COLOR_YELLOW,     COLOR_WHITE, true, PWD_LNG,   keybPwdBuf, FSS9},               // TXT_PWD
  { 10, 240, 140, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE, false, PWD_LNG,  keybProtoBuf, FSS9},             // TXT_PROTOCOL
  { 80, 102, 160, 24, COLOR_NAVY,  COLOR_BACKGROUND, COLOR_BACKGROUND, true, NAME_LNG, locoName, FSS7},             // TXT_LOCO_NAME
  {130, 156,  60, 24, COLOR_NAVY,  COLOR_BACKGROUND, COLOR_BACKGROUND, true, ADDR_LNG, locoAddr, FSSB9},            // TXT_LOCO_ADDR
  { 80,  11, 160, 31, COLOR_BLACK, COLOR_BACKGROUND, COLOR_BACKGROUND, true, NAME_LNG, clockBuf, FSSB12},           // TXT_CLOCK
  { 55, 130,  40, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE,      true, 2, keybHourBuf, FSS9},                 // TXT_HOUR
  {100, 130,  40, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE,      true, 2, keybMinBuf, FSS9},                  // TXT_MIN
  {100, 170,  40, 24, COLOR_BLACK, COLOR_BACKGROUND, COLOR_WHITE,      true, 3, keybRateBuf, FSS9},                 // TXT_RATE
  {120,  12,  60, 24, COLOR_BLUE,  COLOR_BACKGROUND, COLOR_BACKGROUND, false, ADDR_LNG, locoEditAddr, FSSB9},       // TXT_EDIT_ADDR
  { 80, 140, 150, 24, COLOR_BLACK, COLOR_WHITE,      COLOR_NAVY,       false, NAME_LNG, locoEditName, FSS9},        // TXT_EDIT_NAME
  {120,  52,  60, 24, COLOR_BLUE,  COLOR_BACKGROUND, COLOR_BACKGROUND, false, ADDR_LNG, locoEditID, FSSB9},         // TXT_EDIT_IMAGE
  {125, 180,  50, 24, COLOR_BLACK, COLOR_WHITE,      COLOR_NAVY,       true,  ADDR_LNG, locoEditVmax, FSS9},        // TXT_EDIT_VMAX
  { 10, 135, 220, 24, COLOR_BLACK, COLOR_YELLOW,     COLOR_WHITE,      true,  NAME_LNG, keybNameBuf, FSS9},         // TXT_NAME
  { 80, 115,  80, 31, COLOR_NAVY,  COLOR_BACKGROUND, COLOR_BACKGROUND, true,  ADDR_LNG, locoEditFunc, FSSB12},      // TXT_EDIT_FNC
  { 75,  40,  80, 31, COLOR_BLACK, COLOR_WHITE,      COLOR_NAVY, true, IP_LNG, locoEditVmax, FSSB12},               // TXT_KEYB_VMAX
  {  1,  50,  60, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, true, ADDR_LNG, selLocoAddr1, FSSB12},             // TXT_SEL_ADDR1
  {  1,  90,  60, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, true, ADDR_LNG, selLocoAddr2, FSSB12},             // TXT_SEL_ADDR2
  {  1, 130,  60, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, true, ADDR_LNG, selLocoAddr3, FSSB12},             // TXT_SEL_ADDR3
  {  1, 170,  60, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, true, ADDR_LNG, selLocoAddr4, FSSB12},             // TXT_SEL_ADDR4
  {  1, 210,  60, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, true, ADDR_LNG, selLocoAddr5, FSSB12},             // TXT_SEL_ADDR5
  {  1, 250,  60, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, true, ADDR_LNG, selLocoAddr6, FSSB12},             // TXT_SEL_ADDR6
  { 61,  50, 178, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, false, NAME_LNG, selLocoName1, FSS9},              // TXT_SEL_NAME1
  { 61,  90, 178, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, false, NAME_LNG, selLocoName2, FSS9},              // TXT_SEL_NAME2
  { 61, 130, 178, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, false, NAME_LNG, selLocoName3, FSS9},              // TXT_SEL_NAME3
  { 61, 170, 178, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, false, NAME_LNG, selLocoName4, FSS9},              // TXT_SEL_NAME4
  { 61, 210, 178, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, false, NAME_LNG, selLocoName5, FSS9},              // TXT_SEL_NAME5
  { 61, 250, 178, 40, COLOR_NAVY,  COLOR_WHITE, COLOR_LIGHTGREY, false, NAME_LNG, selLocoName6, FSS9},              // TXT_SEL_NAME6
  { 75,  40,  80, 31, COLOR_BLACK, COLOR_WHITE, COLOR_NAVY,      true,  ADDR_LNG, locoKeybAddr, FSSB12},            // TXT_KEYB_ADDR
  { 70,  90, 120, 31, COLOR_NAVY,  COLOR_WHITE, COLOR_WHITE,     true,  PWD_LNG,  aboutPacoMouseCYD, FSSB12},       // TXT_ABOUT
  { 55, 172, 150, 31, COLOR_WHITE, COLOR_BLUE,  COLOR_BLUE,      false, PWD_LNG,  aboutIP, FSS7},                   // TXT_ABOUT_IP
  { 55, 195, 160, 31, COLOR_WHITE, COLOR_BLUE,  COLOR_BLUE,      false, PWD_LNG,  aboutMAC, FSS7},                  // TXT_ABOUT_MAC
  {115,  67, 100, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  NAME_LNG, spdScaleBuf, FSSB9},              // TXT_SPEEDO_SCALE
  { 87, 140,  70, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  NAME_LNG, spdLengthBuf, FSSB9},             // TXT_SPEEDO_LNG
  { 25, 270, 190, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  NAME_LNG, spdSpeedBuf, FSSB9},              // TXT_SPEEDO_SPD
  { 75,  40,  80, 31, COLOR_BLACK, COLOR_WHITE, COLOR_NAVY,      true,  PORT_LNG, speedoKeybLng, FSSB12},           // TXT_EDIT_LNG
  { 55,  20,  70, 30, COLOR_YELLOW, COLOR_BACKGROUND, COLOR_BACKGROUND, true,  NAME_LNG, spdSelScaleBuf, FSSB12},   // TXT_EDIT_SCALE
  {130,  20,  60, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  IP_LNG,   spdSelScaleNumBuf, FSSB12},       // TXT_NUM_SCALE
  { 50,  45,  70, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  ADDR_LNG, keybCvBuf, FSSB12},               // TXT_CV
  {160,  45,  60, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  IP_LNG,   keybCvValBuf, FSSB12},            // TXT_CV_VAL
  {  1,   5, 238, 30, COLOR_BLACK, COLOR_BACKGROUND, COLOR_BACKGROUND,  true,  PWD_LNG,  cvStatusBuf, FSSB12},      // TXT_CV_STATUS
  {100,  40,  80, 30, COLOR_BLUE,  COLOR_WHITE, COLOR_BLACK,     true,  ADDR_LNG,  locoEditAddr, FSSB12},           // TXT_CV_ADDR
  { 85,   8,  70, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  PORT_LNG,  keybLncvArtBuf, FSSB12},         // TXT_LNCV_ART
  { 85,  48,  70, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  PORT_LNG,  keybLncvModBuf, FSSB12},         // TXT_LNCV_MOD
  { 75,  88,  70, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  PORT_LNG,  keybLncvAdrBuf, FSSB12},         // TXT_LNCV_ADR
  {160,  88,  70, 30, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,     true,  PORT_LNG,  keybLncvValBuf, FSSB12},         // TXT_LNCV_VAL
  { 24,  80,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[0],  FSSB6},          // TXT_ACC_0
  { 98,  80,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[1],  FSSB6},          // TXT_ACC_1
  {172,  80,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[2],  FSSB6},          // TXT_ACC_2
  {246,  80,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[3],  FSSB6},          // TXT_ACC_3
  { 24, 170,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[4],  FSSB6},          // TXT_ACC_4
  { 98, 170,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[5],  FSSB6},          // TXT_ACC_5
  {172, 170,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[6],  FSSB6},          // TXT_ACC_6
  {246, 170,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[7],  FSSB6},          // TXT_ACC_7
  { 24, 260,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[8],  FSSB6},          // TXT_ACC_8
  { 98, 260,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[9],  FSSB6},          // TXT_ACC_9
  {172, 260,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[10], FSSB6},          // TXT_ACC_10
  {246, 260,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[11], FSSB6},          // TXT_ACC_11
  { 24, 350,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[12], FSSB6},          // TXT_ACC_12
  { 98, 350,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[13], FSSB6},          // TXT_ACC_13
  {172, 350,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[14], FSSB6},          // TXT_ACC_14
  {246, 350,  50, 20, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     true,  ACC_LNG,  accNamesBuf[15], FSSB6},          // TXT_ACC_15
  {105, 400, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNameBuf,     FSS7},         // TXT_PANEL
  {  5,   4, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[0], FSS7},         // TXT_PANEL0
  {125,   4, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[1], FSS7},         // TXT_PANEL1
  {  5,  44, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[2], FSS7},         // TXT_PANEL2
  {125,  44, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[3], FSS7},         // TXT_PANEL3
  {  5,  84, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[4], FSS7},         // TXT_PANEL4
  {125,  84, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[5], FSS7},         // TXT_PANEL5
  {  5, 124, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[6], FSS7},         // TXT_PANEL6
  {125, 124, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[7], FSS7},         // TXT_PANEL7
  {  5, 164, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[8], FSS7},         // TXT_PANEL8
  {125, 164, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[9], FSS7},         // TXT_PANEL9
  {  5, 204, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[10], FSS7},        // TXT_PANEL10
  {125, 204, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[11], FSS7},        // TXT_PANEL11
  {  5, 244, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[12], FSS7},        // TXT_PANEL12
  {125, 244, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[13], FSS7},        // TXT_PANEL13
  {  5, 284, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[14], FSS7},        // TXT_PANEL14
  {125, 284, 110, 32, COLOR_NAVY,  COLOR_CREAM, COLOR_BACKGROUND, true, PANEL_LNG, panelNamesBuf[15], FSS7},        // TXT_PANEL15
  { 75,  40,  80, 31, COLOR_BLACK, COLOR_WHITE, COLOR_NAVY,       true, ADDR_LNG,  accKeybAddr,    FSSB12},         // TXT_ACC_ADDR
  { 75,  45,  70, 30, COLOR_BLACK, COLOR_WHITE, COLOR_NAVY,       true, ADDR_LNG,  accKeybAddr1,   FSSB9},          // TXT_ACC_ADDR1
  {155,  45,  70, 30, COLOR_BLACK, COLOR_WHITE, COLOR_NAVY,       true, ADDR_LNG,  accKeybAddr2,   FSSB9},          // TXT_ACC_ADDR2
  { 75,   5, 100, 30, COLOR_BLACK, COLOR_WHITE, COLOR_NAVY,       true, ACC_LNG,   accKeybName,    FSSB9},          // TXT_ACC_NAME
  { 75,  40,  80, 31, COLOR_BLACK, COLOR_WHITE, COLOR_NAVY,       true, ADDR_LNG,  accKeybAdrEdit, FSSB12},         // TXT_ACC_EDIT

  { 65,  90,  40, 31, COLOR_WHITE, COLOR_BLUE, COLOR_BLUE,       false, ADDR_LNG,  staLevelBuf, FSS9},              // TXT_STA_LEVEL
  { 50, 150,  40, 31, COLOR_WHITE, COLOR_BLUE, COLOR_BLUE,       false, ADDR_LNG,  staStarsBuf, FSS9},              // TXT_STA_STARS
  {218,  90,  60, 31, COLOR_WHITE, COLOR_BLUE, COLOR_BLUE,       false, ACC_LNG,   staStationsBuf, FSS9},           // TXT_STA_STATION
  {190, 150,  60, 31, COLOR_WHITE, COLOR_BLUE, COLOR_BLUE,       false, ACC_LNG,   staTimeBuf, FSS9},               // TXT_STA_CLOCK
  {190,  12,  55, 27, COLOR_WHITE, COLOR_BLUE, COLOR_BLUE,       false, ACC_LNG,   staTimeBuf, FSS9},               // TXT_STA_TIME
  { 60,  12,  55, 27, COLOR_WHITE, COLOR_BLUE, COLOR_BLUE,       false, ACC_LNG,   staStationsBuf, FSS9},           // TXT_STA_COUNT
  { 22, 245,  40, 26, COLOR_WHITE, COLOR_BLUE - 0x0011, COLOR_BLUE - 0x0011,  true,  ADDR_LNG,  staStarsBuf, FSS9}, // TXT_STA_STARC

  {120,  10,  60, 32, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,      true, IP_LNG,   staStartTimeBuf, FSS9},            // TXT_STA_STARTTIME
  {160,  50,  32, 32, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,      true, IP_LNG,   staStatNumBuf, FSS9},              // TXT_STA_STATNUM
  {160,  90,  32, 32, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,      true, IP_LNG,   staTurnNumBuf, FSS9},              // TXT_STA_TURNNUM
  { 10, 165,  60, 32, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,      true, ADDR_LNG, staTurnout1Buf, FSS9},             // TXT_STA_TURNOUT1
  { 10, 200,  60, 32, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,      true, ADDR_LNG, staTurnout2Buf, FSS9},             // TXT_STA_TURNOUT2
  { 10, 235,  60, 32, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,      true, ADDR_LNG, staTurnout3Buf, FSS9},             // TXT_STA_TURNOUT3
  { 10, 270,  60, 32, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,      true, ADDR_LNG, staTurnout4Buf, FSS9},             // TXT_STA_TURNOUT4

  { 50,  10, 180, 24, COLOR_BLUE,  COLOR_BACKGROUND, COLOR_WHITE, true, NAME_LNG, networkNameBuf, FSS9},            // TXT_NETWORK
  { 50,  50, 180, 24, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     false, NAME_LNG, networkNamesBuf[0], FSS9},        // TXT_NETWORK_0
  { 50,  85, 180, 24, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     false, NAME_LNG, networkNamesBuf[1], FSS9},        // TXT_NETWORK_1
  { 50, 120, 180, 24, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     false, NAME_LNG, networkNamesBuf[2], FSS9},        // TXT_NETWORK_2
  { 50, 155, 180, 24, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE,     false, NAME_LNG, networkNamesBuf[3], FSS9},        // TXT_NETWORK_3
  { 45,  95, 150, 24, COLOR_WHITE, COLOR_ORANGE, COLOR_WHITE,    true,  NAME_LNG, keybActionBuf[0], FSS9},          // TXT_ACTION_BOOT
  { 45, 135, 150, 24, COLOR_WHITE, COLOR_ORANGE, COLOR_WHITE,    true,  NAME_LNG, keybActionBuf[1], FSS9},          // TXT_ACTION_R
  { 45, 175, 150, 24, COLOR_WHITE, COLOR_ORANGE, COLOR_WHITE,    true,  NAME_LNG, keybActionBuf[2], FSS9},          // TXT_ACTION_G
  { 45, 215, 150, 24, COLOR_WHITE, COLOR_ORANGE, COLOR_WHITE,    true,  NAME_LNG, keybActionBuf[3], FSS9},          // TXT_ACTION_B
  { 88, 193,  81, 20, COLOR_WHITE, COLOR_ORANGE, COLOR_WHITE,    true,  NAME_LNG, battBuf, FSSB6},                  // TXT_BATT

};



////////////////////////////////////////////////////////////
// ***** SWITCH *****
////////////////////////////////////////////////////////////

enum switchObj {  SW_SHUNTING, SW_ROTATE, SW_LOCK_LOK, SW_LOCK_ACC, SW_LOCK_PRG, SW_OPT_ADR, SW_OPT_DISCOVER,
                  SW_POM, SW_STA_OR1, SW_STA_OR2, SW_STA_OR3, SW_STA_OR4, SW_STA_INV1, SW_STA_INV2, SW_STA_INV3, SW_STA_INV4,
                  SW_RGB_LED,
                  MAX_SWITCH_OBJ
               };

typedef struct {                                        // Switch data
  uint16_t  x;
  uint16_t  y;
  uint16_t  h;
  uint16_t  colorOn;
  uint16_t  colorOff;
  uint16_t  colorKnob;
  bool  state;
} wSwitchObj;

wSwitchObj switchData[MAX_SWITCH_OBJ] = {
  { 35, 130, 21, COLOR_BLUE, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_SHUNTING
  { 30, 130, 21, COLOR_BLUE, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_ROTATE
  { 25, 130, 21, COLOR_BLUE, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_LOCK_LOK
  { 25, 165, 21, COLOR_BLUE, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_LOCK_ACC
  { 25, 200, 21, COLOR_BLUE, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_LOCK_PRG
  { 15, 155, 21, COLOR_BLUE, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_OPT_ADR
  { 15,  85, 21, COLOR_BLUE, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_OPT_DISCOVER
  { 15, 125, 21, COLOR_RED,  COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_POM
  { 95, 170, 21, COLOR_CYAN, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_STA_OR1
  { 95, 205, 21, COLOR_CYAN, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_STA_OR2
  { 95, 240, 21, COLOR_CYAN, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_STA_OR3
  { 95, 275, 21, COLOR_CYAN, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_STA_OR4
  {170, 170, 21, COLOR_CYAN, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_STA_INV1
  {170, 205, 21, COLOR_CYAN, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_STA_INV2
  {170, 240, 21, COLOR_CYAN, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_STA_INV3
  {170, 275, 21, COLOR_CYAN, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_STA_INV4
  { 30, 170, 21, COLOR_BLUE, COLOR_DARKCYAN,  COLOR_WHITE, false},        // SW_RGB_LED

};


////////////////////////////////////////////////////////////
// ***** KEYBOARD *****
////////////////////////////////////////////////////////////

#define KEYB_WIDTH          240
#define KEYB_HEIGHT         120
#define KEYPAD_WIDTH        80
#define KEYPAD_HEIGHT       120
#define KEYPAD_BIG_WIDTH    150
#define KEYPAD_BIG_HEIGHT   190
#define CHR_BKSPC           8
#define CHR_ENTER           '\n'
#define CHR_REDRAW          '\r'

const char KeybAlphaCaps[3][12] = {
  { 0, 12, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
  {10, 11, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'},
  {30,  9, 'Z', 'X', 'C', 'V', 'B', 'N', 'M'},
};

const char KeybAlpha[3][12] = {
  { 0, 12, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
  {10, 11, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'},
  {30,  9, 'z', 'x', 'c', 'v', 'b', 'n', 'm'},
};

const char KeybNum[3][12] = {
  { 0, 12, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
  { 0, 12, '-', '/', ':', ';', '(', ')', '$', '&', '@', '"'},
  { 50,  7, '.', ',', '?', '!', '\''}
};

const char KeybSym[3][12] = {
  { 0, 12, '[', ']', '{', '}', '#', '%', '^', '*', '+', '='},
  {40,  8, '_', '\\', '|', '~', '<', '>'},
  {50,  7, '.', ',', '?', '!', '\''}
};

const char KeybKeypad[3][12] = {
  { 0, 5, '7', '8', '9'},
  { 0, 5, '4', '5', '6'},
  { 0, 5, '1', '2', '3'}
};

enum keybType {KEYB_ALPHA, KEYB_CAPS, KEYB_NUM, KEYB_SYM, KEYB_KEYPAD, KEYB_KEYPAD_BIG, KEYB_KEYPAD_OPT};

enum keybObj {KEYB_IP, KEYB_PWD, KEYB_CLOCK, KEYB_NAME, KEYB_VMAX, KEYB_ADDR,
              KEYB_LNG, KEYB_SCALE, KEYB_CV, KEYB_CV_ADDR, KEYB_LNCV, KEYB_ACC, KEYB_ACC_ADDR,
              KEYB_STA,
              MAX_KEYB_OBJ
             };


typedef struct {                                        // Keyboard data
  uint16_t  x;
  uint16_t  y;
  uint16_t  type;
  uint16_t  idTextbox;
} wKeybObj;

wKeybObj keybData[MAX_KEYB_OBJ] = {
  {150, 172, KEYB_KEYPAD, TXT_IP1},                   // KEYB_IP
  {  0, 165, KEYB_ALPHA, TXT_PWD},                    // KEYB_PWD
  {142, 130, KEYB_KEYPAD, TXT_HOUR},                  // KEYB_CLOCK
  {  0, 165, KEYB_CAPS, TXT_NAME},                    // KEYB_NAME
  { 40,  80, KEYB_KEYPAD_BIG, TXT_KEYB_VMAX},         // KEYB_VMAX
  { 40,  80, KEYB_KEYPAD_BIG, TXT_KEYB_ADDR},         // KEYB_ADDR
  { 40,  80, KEYB_KEYPAD_BIG, TXT_EDIT_LNG},          // KEYB_LNG
  { 80,  80, KEYB_KEYPAD_BIG, TXT_NUM_SCALE},         // KEYB_SCALE
  { 80, 125, KEYB_KEYPAD_BIG, TXT_CV_VAL},            // KEYB_CV
  { 40,  80, KEYB_KEYPAD_BIG, TXT_CV_ADDR},           // KEYB_CV_ADDR
  { 85, 125, KEYB_KEYPAD_BIG, TXT_LNCV_VAL},          // KEYB_LNCV
  { 40,  80, KEYB_KEYPAD_OPT, TXT_ACC_ADDR},          // KEYB_ACC
  { 40,  80, KEYB_KEYPAD_BIG, TXT_ACC_EDIT},          // KEYB_ACC_ADDR
  { 85,  80, KEYB_KEYPAD_BIG, TXT_STA_STARTTIME},     // KEYB_STA
};

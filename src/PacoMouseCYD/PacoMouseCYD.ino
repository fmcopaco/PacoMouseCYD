/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/

      This software and associated files are a DIY project that is not intended for commercial use.
      This software uses libraries with different licenses, follow all their different terms included.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.

      Sources are only provided for building and uploading to the device.
      You are not allowed to modify the source code or fork/publish this project.
      Commercial use is forbidden.

      --------------------------------------------------------------------------------------------------

      Use 2.8" Cheap Yellow Display ESP32-2432S028 (CYD)
      - ILI9341 driver chip (320x240)
      - XPT2046 chip for touch screen

      CYD Also available in 2.4" and 3.2" (Use Resistive touch)

      Select ESP32 Dev Module in Arduino IDE

      SD Card. IMPORTANT!!!: use FAT32 SD card (max. 32GB)

      --------------------------------------------------------------------------------------------------

       v0.1     24feb25   Start writting code
       v0.2     07mar25   GUI, SD Pictures, Wifi configuration and loco throttle on Z21 working
       v0.3     21mar25   Added loco list sorting and loco image selection. Added internal file system for loco data.
       v0.4     19apr25   Added configuration menu screen. Corrected touch rotation for CYD 2.4". Changed translations files. Added programming CV. Added speedometer.
       v0.5     02jun25   Added steam loco throttle. Adding more function icons. Added Xpressnet LAN and Loconet over TCP protocols. Added experimental identify command station for Loconet.
       v0.6     08oct25   Added Loconet programming. New LocoEditor for browser on SD.
       v0.7     23nov25   Corrected bugs on loconet steam direction. Added accessory panels. Added WiFi analyzer.
       v0.8     15dec25   Added ECoS/CS1 protocol. Updated user defined CYDs. Changes in modal windows.
       v0.9     03jan26   Added Station Run for kids. Corrected minor bugs on loconet
       v0.10    12feb26   Correct little bugs, clean and made more coherent code. Updating accessories. Status RGB LED. Up to 4 WiFi networks. Control of loco in accessory panel. Define actions for buttons. Added Czech language. Update from SD.
       v0.11    18mar26   Changes in config.h for easy setup. Tapping gauge controls speed. Added French language. Check WLAN connection. Battery level. Changes in encoder.
*/

// PacoMouseCYD program version
#define VER_H "0"
#define VER_L "11"
#define VER_R "c"


//#define DEBUG                                               // Descomentar para mensajes de depuracion
//#define SCREEN_SEND                                         // Reads a screen image off the TFT and send it to a processing client sketch over the serial port https://github.com/Bodmer/TFT_eSPI

// Libraries

#include <Arduino.h>
#include "config.h"                                         // PacoMouseCYD config file
#include "icon.h"                                           // PacoMouseCYD icons
#include "gui.h"                                            // PacoMouseCYD Graphic User Interface
#include "translations.h"                                   // PacoMouseCYD languages
#include <FS.h>                                             // SD Card. IMPORTANT!!!: use FAT32 SD card (max. 32GB)
#include <SD.h>
#include <LittleFS.h>
#include <SPI.h>
#include <TFT_eSPI.h>                                       // Graphics and font library for ILI9341 driver chip  v2.5.43
#include "XPT2046.h"                                        // PacoMouseCYD touch screen XPT2046 (2.8": bit bang, 2.4": SPI)
#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "lnet.h"                                           // PacoMouseCYD LNTCP
#include <Update.h>


#ifdef DEBUG
char output[120];

#define DEBUG_MSG(...)  snprintf(output, 120, __VA_ARGS__ );   \
  Serial.println(output);
#else
#define DEBUG_MSG(...)
#endif


////////////////////////////////////////////////////////////
// ***** SD CARD *****
////////////////////////////////////////////////////////////

bool sdDetected;
File root;
char folderSel[32];
int dirCount = 1;
char DirAndFile[15];
char FileName[32];

#define FORMAT_LITTLEFS_IF_FAILED true


////////////////////////////////////////////////////////////
// ***** TFT *****
////////////////////////////////////////////////////////////

TFT_eSPI tft = TFT_eSPI();                                  // Invoke library, pins defined in User_Setup.h

TFT_eSprite sprite = TFT_eSprite(&tft);

// backlight
#define LEDC_CHANNEL_0     0                                // use first channel of 16 channels (started from zero)
#define LEDC_RESOLUTION    8                                // use 8 (12) bit precission for LEDC timer
#define LEDC_BASE_FREQ     5000                             // 5000 Hz as a LEDC base frequency

#define INACT_TIME         1200                             // 2 minutes inactivity (in 100ms) -> lower bright
uint8_t backlight;
uint8_t currBacklight;

#define USB_UP      2                                       // Display USB up
#define USB_DOWN    0                                       // Display USB down

uint8_t locationUSB;
uint8_t activeRGB;

enum colorRGB {LED_RGB_OFF, LED_RGB_BLUE, LED_RGB_RED, LED_RGB_GREEN = 4, LED_RGB_WHITE = 7};

bool editBatt;
uint16_t fullBatt;
uint16_t emptyBatt;
uint16_t currBatt;


////////////////////////////////////////////////////////////
// ***** TOUCHSCREEN *****
////////////////////////////////////////////////////////////

XPT2046_TS touchscreen(XPT2046_MOSI, XPT2046_MISO, XPT2046_CLK, XPT2046_CS);

bool calibrationPending;
bool clickDetected = false;                                 // pulsacion detectada en panel tactil

#define SW_BOOT_PIN 0                                       // BOOT button used to enter touchscreen calibration window or defined action

enum fncSwitch {SW_BOOT, SW_LED_R, SW_LED_G, SW_LED_B};

byte statusBOOT, statusSW_R, statusSW_G, statusSW_B;

uint8_t actionSW_BOOT, actionSW_R, actionSW_G, actionSW_B;

enum actionCmd {ACT_UNDEF, ACT_CALIBRATE, ACT_STOP, ACT_SHUNTING, ACT_NEXT, ACT_PREV,
                ACT_F0, ACT_F1, ACT_F2, ACT_F3, ACT_F4, ACT_F5, ACT_F6, ACT_F7, ACT_F8, ACT_F9,
                ACT_F10, ACT_F11, ACT_F12, ACT_F13, ACT_F14, ACT_F15, ACT_F16, ACT_F17, ACT_F18, ACT_F19,
                ACT_F20, ACT_F21, ACT_F22, ACT_F23, ACT_F24, ACT_F25, ACT_F26, ACT_F27, ACT_F28,
               };


////////////////////////////////////////////////////////////
// ***** ENCODER *****
////////////////////////////////////////////////////////////

volatile int outA;                                          // encoder input used by ISR
volatile int outB;
volatile int copyOutA;
volatile int copyOutB;
volatile uint32_t  lastTimeEncoder;
volatile byte encoderValue;                                 // encoder shared values (ISR & program)
volatile byte encoderMax;
volatile bool encoderChange;
volatile bool encoderNeedService;

byte statusSwitch;
bool switchOn;
const unsigned long timeoutButtons = 50;                    // temporizador antirebote
unsigned long timeButtons;


////////////////////////////////////////////////////////////
// ***** EEPROM *****
////////////////////////////////////////////////////////////

#define EEPROM_SIZE         1024

enum Settings {
  EE_XMIN_H, EE_XMIN_L, EE_XMAX_H, EE_XMAX_L, EE_YMIN_H, EE_YMIN_L, EE_YMAX_H, EE_YMAX_L, EE_BACKLIGHT, EE_LANGUAGE,
  EE_ADRH, EE_ADRL, EE_STOP_MODE, EE_SHUNTING, EE_ROCO, EE_LOCK, EE_SHORT, EE_USB_LOCATION, EE_CMD_STA, EE_CMD_AUTO,
  EE_STA_ADRH1, EE_STA_ADRL1, EE_STA_ADRH2, EE_STA_ADRL2, EE_STA_ADRH3, EE_STA_ADRL3, EE_STA_ADRH4, EE_STA_ADRL4,
  EE_STA_TRNDEF, EE_STA_TRNNUM, EE_STA_NUM, EE_STA_TIME, EE_RGB_LED, EE_ACT_BOOT, EE_ACT_R, EE_ACT_G, EE_ACT_B,
  EE_FULL_H, EE_FULL_L, EE_EMPTY_H, EE_EMPTY_L, EE_UNDEF4,
  EE_WIFI_NET, EE_WIFI,                                     //  datos WiFi. (Tiene que ser el ultimo)
}; // EEPROM settings

bool eepromChanged;


////////////////////////////////////////////////////////////
// ***** WiFi *****
////////////////////////////////////////////////////////////

struct {
  char network[17];                                         // Network name
  char ssid[33];                                            // SSID
  char password[33];                                        // Password
  IPAddress CS_IP;                                          // IP
  uint16_t port;                                            // Port
  bool serverType;                                          // Server type
  byte protocol;                                            // protocol
  int  ok;
} wifiSetting;

enum typeProto {CLIENT_Z21, CLIENT_XNET, CLIENT_ECOS, CLIENT_LNET, CLIENT_CS2};

WiFiClient Client;
WiFiUDP Udp;

#define z21Port   21105                                     // local port to listen on command station
#define XnetPort   5550
#define ECoSPort  15471
#define CS2Port   15731

uint16_t networks;
uint8_t scrSSID;

enum cmdStation {CMD_DR, CMD_ULI, CMD_DIG, CMD_UNKNOW};
byte typeCmdStation;                                        // tipo de central Loconet para funciones

// RSSI RANGE
#define RSSI_CEILING            -40
#define RSSI_FLOOR              -100
#define NEAR_CHANNEL_RSSI_ALLOW -70
#define CHANNEL_WIDTH           15
#define GRAPH_HEIGHT            188
#if (TFT_WIDTH == 240)
#define GRAPH_OFFSET            0
#define GRAPH_BASELINE          222
#else
#define GRAPH_OFFSET            40
#define GRAPH_BASELINE          302
#endif

// Channel color mapping from channel 1 to 14
uint16_t channel_color[] = {
  COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN, COLOR_MAGENTA, COLOR_RED,
  COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN, COLOR_MAGENTA, COLOR_RED, COLOR_ORANGE,
};

uint8_t scan_count = 0;
uint8_t ap_count[14];
int32_t max_rssi[14];
bool signalLost;

////////////////////////////////////////////////////////////
// ***** GUI *****
////////////////////////////////////////////////////////////

uint16_t posObjStack1;
uint16_t posObjStack2;
uint16_t paramChild;

uint8_t lastLanguage;


////////////////////////////////////////////////////////////
// ***** PACOMOUSE *****
////////////////////////////////////////////////////////////

enum initResult {INIT_OK, INIT_NO_SD, INIT_NO_WIFI, INIT_NO_CONNECT};
enum Err {NO_ERROR, ERR_OFF, ERR_STOP, ERR_SERV, ERR_WAIT, ERR_FULL, ERR_CHG_WIFI, ERR_CV, ERR_ASK_SURE, ERR_FILE};

byte errType;

initResult initStatus;

byte csStatus = 0;

#define DEFAULT_STEPS 4                                     // 128 steps
byte stopMode;
bool shuntingMode;
byte shortAddress;

byte tapSpeedSteps[] = {0, 8, 20, 31, 43, 54, 63,           // 128 steps
                        0, 6, 10, 15, 20, 26, 31,           // 28 steps
                        0, 2,  4,  7, 10, 13, 15,           // 14 steps
                       };

enum tapSteps {TAP_STP0, TAP_STP1, TAP_STP2, TAP_STP3, TAP_STP4, TAP_STP5, TAP_STP6};

byte scrHour, scrMin, scrRate, scrPosTime;
byte clockHour, clockMin, clockRate;
unsigned long clockTimer, clockInterval;                    // Internal fast clock calculation

enum prgCV {PRG_IDLE, PRG_CV, PRG_RD_CV29, PRG_RD_CV1, PRG_RD_CV17, PRG_RD_CV18, PRG_WR_CV1, PRG_WR_CV17, PRG_WR_CV18, PRG_WR_CV29};

unsigned int CVaddress, CVdata, CVmax;                      // programacion CV
bool modeProg;
bool enterCVdata;
bool progFinished;
byte progStepCV, lastCV;
byte cv29, cv17, cv18;
unsigned int decoAddress;

enum lock {LOCK_SEL_LOCO, LOCK_TURNOUT, LOCK_PROG};
byte lockOptions;


////////////////////////////////////////////////////////////
// ***** LOCOMOTIVES *****
////////////////////////////////////////////////////////////

unsigned long infoTimer;
unsigned long pingTimer;

byte myLocoData;                                            // current oco data index

typedef union {                                             // Loco address
  uint8_t   adr[2];
  uint16_t  address;
} lokAddr;

typedef union {
  uint8_t   xFunc[4];                                       // loco functions, F0F4, F5F12, F13F20 y F21F28
  uint32_t  Bits;                                           // long para acceder a los bits
} lokFunc;

typedef struct {
  lokAddr   myAddr;
  lokFunc   myFunc;
  uint8_t   myDir;
  uint8_t   mySpeed;
  uint8_t   mySteps;
  uint16_t  myVmax;
  char      myName[NAME_LNG + 1];
  uint16_t  myLocoID;                                       // ID / picture
  uint8_t   myFuncIcon[30];
} lokData;

lokData   locoData[LOCOS_IN_STACK];                         // Loco data

uint16_t  locoStack[LOCOS_IN_STACK];                        // stack para ultimas locos accedidas
uint16_t  sortedLocoStack[LOCOS_IN_STACK];                  // lista ordenada de locomotoras
uint16_t  locoImages[LOCOS_IN_STACK + 20];                  // lista de imagenes en el sistema y SD
bool useID;

#define MAX_LOCO_IMAGE sizeof(locoImages) / sizeof(locoImages[0])
uint16_t locoImageIndex;

enum locoSort {SORT_LAST, SORT_NUM_UP, SORT_NUM_DWN, SORT_NAME_UP, SORT_NAME_DWN};
uint16_t currOrder;

struct {
  unsigned int id;                                          // stack loco data
  unsigned int locoAddress;
  char locoName[NAME_LNG + 1];
} rosterList[LOCOS_IN_STACK];


////////////////////////////////////////////////////////////
// ***** ACCESSORIES *****
////////////////////////////////////////////////////////////

#define AUTO_INTERVAL       100UL                           // Timer interval (100ms)     
#define TIME_ACC_ON         2                               // accessory activation time accessory (200ms)
#define TIME_ACC_CDU        2                               // time to wait after accessory deactivated for CDU (200ms)

enum fifo {FIFO_EMPTY, FIFO_ACC_ON, FIFO_ACC_CDU};

uint16_t accessoryFIFO[32];                                 // FIFO 32 elements. Hardcoded in functions.
uint16_t lastInFIFO;
uint16_t firstOutFIFO;
uint16_t cntFIFO;
uint16_t lastAccessory;
uint8_t accessoryTimer;
uint8_t stateFIFO;

// DESVIADO: ROJO  - > links  thrown
// RECTO:    VERDE + < rechts closed
//enum posDesvio {NO_MOVIDO, DESVIADO, RECTO, INVALIDO};

bool editAccessory;
uint8_t currPanel;
uint8_t currPanelAcc;
uint8_t currAccAspects;
bool accPanelChanged;
uint16_t myTurnout;

typedef enum accType {ACC_UNDEF, ACC_TURN_L, ACC_TURN_R, ACC_TRIPLE, ACC_CROSSING, ACC_DCROSS, ACC_BETRELLE, ACC_SIGNAL2, ACC_SIGNAL3, ACC_SIGNAL4, ACC_SEM2, ACC_SEM3,
                      ACC_PAN, ACC_TT_L, ACC_TT_R, ACC_TT_TRK, ACC_TT_TURN, ACC_LIGHT, ACC_SOUND, ACC_POWER, ACC_KEYPAD, ACC_MAX,
                     };

typedef struct {
  uint16_t  fncIcon;
  uint16_t  color;
  uint16_t  colorOn;
} accAspect;

typedef struct {
  uint8_t   aspects;
  uint8_t   num;
  accAspect icon[4];
} accObj;

const accObj accDef[ACC_MAX] = {
  { 1, 99, {{FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_UNDEF
  { 2, 99, {{FNC_TURNLD_OFF, COLOR_BLACK, COLOR_RED},          {FNC_TURNLS_OFF, COLOR_BLACK, COLOR_GREEN},        {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_TURN_L
  { 2, 99, {{FNC_TURNRD_OFF, COLOR_BLACK, COLOR_RED},          {FNC_TURNRS_OFF, COLOR_BLACK, COLOR_GREEN},        {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_TURN_R
  { 3, 99, {{FNC_TURN3L_OFF, COLOR_BLACK, COLOR_RED},          {FNC_TURN3S_OFF, COLOR_BLACK, COLOR_GREEN},        {FNC_TURN3R_OFF, COLOR_BLACK, COLOR_RED},          {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_TRIPLE
  { 2, 99, {{FNC_CROSD_OFF, COLOR_BLACK, COLOR_RED},           {FNC_CROSS_OFF, COLOR_BLACK, COLOR_GREEN},         {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_CROSSING
  { 4,  4, {{FNC_DCROSSD1_OFF, COLOR_BLACK, COLOR_RED},        {FNC_DCROSSS1_OFF, COLOR_BLACK, COLOR_GREEN},      {FNC_DCROSSD2_OFF, COLOR_BLACK, COLOR_RED},        {FNC_DCROSSS2_OFF, COLOR_BLACK, COLOR_GREEN}}},      // ACC_DCROSS
  { 2, 99, {{FNC_BRETELLED_OFF, COLOR_BLACK, COLOR_RED},       {FNC_BRETELLE_OFF, COLOR_BLACK, COLOR_GREEN},      {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_BETRELLE
  { 2, 99, {{FNC_SIGRY_OFF, COLOR_BLACK, COLOR_RED},           {FNC_SIGGW_OFF, COLOR_BLACK, COLOR_GREEN},         {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_SIGNAL2
  { 3,  3, {{FNC_SIGRY_OFF, COLOR_BLACK, COLOR_RED},           {FNC_SIGGW_OFF, COLOR_BLACK, COLOR_GREEN},         {FNC_SIGRY_OFF, COLOR_BLACK, COLOR_YELLOW},        {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_SIGNAL3
  { 4,  4, {{FNC_SIGRY_OFF, COLOR_BLACK, COLOR_RED},           {FNC_SIGGW_OFF, COLOR_BLACK, COLOR_GREEN},         {FNC_SIGRY_OFF, COLOR_BLACK, COLOR_YELLOW},        {FNC_SIGGW_OFF, COLOR_BLACK, COLOR_WHITE}}},         // ACC_SIGNAL4
  { 2, 99, {{FNC_SEMR_OFF, COLOR_BLACK, COLOR_RED},            {FNC_SEMG_OFF, COLOR_BLACK, COLOR_RED},            {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_SEM2
  { 3,  3, {{FNC_SEMR_OFF, COLOR_BLACK, COLOR_RED},            {FNC_SEMG_OFF, COLOR_BLACK, COLOR_RED},            {FNC_SEMY_OFF, COLOR_BLACK, COLOR_RED},            {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_SEM3
  { 2, 99, {{FNC_PANR_OFF, COLOR_BLACK, COLOR_RED},            {FNC_PANG_OFF, COLOR_BLACK, COLOR_RED},            {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_PAN
  { 1, 99, {{FNC_TTL_OFF, COLOR_BLACK, COLOR_RED},             {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_TT_L
  { 1, 99, {{FNC_TTR_OFF, COLOR_BLACK, COLOR_GREEN},           {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_TT_R
  { 1, 99, {{FNC_TTTRK_OFF, COLOR_BLACK, COLOR_BLACK},         {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_TT_TRK
  { 1, 99, {{FNC_TTROT_OFF, COLOR_BLACK, COLOR_YELLOW},        {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_TT_TURN
  { 2, 99, {{FNC_LIGHT_OFF, COLOR_BLACK, COLOR_LIGHTGREY},     {FNC_LIGHT_OFF, COLOR_BLACK, COLOR_YELLOW},        {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_LIGHT
  { 2, 99, {{FNC_SOUND_OFF, COLOR_BLACK, COLOR_LIGHTGREY},     {FNC_SOUND_OFF, COLOR_BLACK, COLOR_YELLOW},        {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_SOUND
  { 2, 99, {{FNC_POWER_OFF, COLOR_RED, COLOR_RED},             {FNC_POWER_OFF, COLOR_GREEN, COLOR_GREEN},         {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_POWER
  { 1, 99, {{FNC_KEYPAD_OFF, COLOR_BLACK, COLOR_YELLOW},       {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}, {FNC_BLANK_OFF, COLOR_LIGHTGREY, COLOR_LIGHTGREY}}}, // ACC_KEYPAD
};

typedef struct {
  accType   type;
  uint16_t  addr;
  uint16_t  addr2;
  char      accName[ACC_LNG + 1];
  uint8_t   currAspect;
  uint16_t  activeOutput;                                   // '3A2G 3A2R 3A1G 3A1R  2A2G 2A2R 2A1G 2A1R  1A2G 1A2R 1A1G 1A1R  0A2G 0A2R 0A1G 0A1R'
} panelElement;

const uint16_t accOutDefault[ACC_MAX] = {0x0000, 0x0021, 0x0021, 0x0521, 0x0021, 0x4521, 0x0021, 0x0021, 0x0421, 0x8421, 0x0021, 0x0421, 0x0021, 0x0002, 0x0001, 0x0001, 0x0002, 0x0021, 0x0021, 0x0021, 0x0000};

panelElement accPanel[16];
panelElement currAccEdit;
uint8_t savedAspect[16][16];
uint8_t accPosition[256];                                   // Accessories 1..2047 position (0: RED, 1: GREEN)


////////////////////////////////////////////////////////////
// ***** SPEEDOMETER *****
////////////////////////////////////////////////////////////

uint32_t  speedoStartTime;
uint32_t  speedoEndTime;
uint32_t  speedoSpeed;
uint16_t  speedoScale;
uint16_t  speedoLength;

enum speedo {SPD_WAIT, SPD_BEGIN, SPD_COUNT, SPD_ARRIVE, SPD_END};
uint16_t  speedoPhase;


////////////////////////////////////////////////////////////
// ***** STEAM  *****
////////////////////////////////////////////////////////////

uint16_t oldSteamLoco;                                      // steam loco address
uint16_t oldPressure;                                       // 0..270   manometer needle angle
uint16_t oldSpeedSteam;                                     // 240..305 throttle bar angle
uint16_t oldLevelBoiler;                                    // 0..50    water level bar
uint16_t oldLevelTender;                                    // 0..500   tender level bar

uint8_t  steamDir;                                          // 0x80: Forward, 0x00: Backward
uint16_t targetSpeedSteam;                                  // 0..127   calculated target speed
uint16_t currentSteamSpeed;                                 // 0..127   current speed
uint16_t steamPressure;                                     // 0..100   boiler pressure
uint16_t waterLevelBoiler;                                  // 0..100   boiler water
uint16_t waterLevelTender;                                  // 0..500   tender water

bool fillTender;                                            // Tender water filling
bool waterInjection;                                        // Boiler water injection
bool shovelCoal;                                            // Shoveling coal to firebox


#define STEAM_LOAD_TIME       250                           // Basic Timeout for water & steam
#define STEAM_SMOKE_SLOW      3200                          // Chimney smoke at slow speed
#define STEAM_SMOKE_FAST      600                           // Chimney smoke at fast speed
#define STEAM_JOHNSON_NEUTRAL 3                             // Johnson bar neutral position

uint32_t currentSteamTime;
uint32_t steamTimeSpeed;
uint32_t steamTimeSteam;
uint32_t steamTimeWater;
uint32_t steamTimeLoad;
uint32_t steamTimeSmoke;
uint32_t changeSteam;
uint32_t changeWater;
uint32_t changeSpeed;
uint32_t changeSmoke;

enum steamLimits {LIMIT_THROTTLE, LIMIT_JOHNSON, LIMIT_PRESSURE, LIMIT_WATER, LIMIT_TENDER, LIMIT_BRAKE, MAX_LIMIT};

#define LIMIT_NONE 127

uint16_t steamSpeedLimit[MAX_LIMIT];

////////////////////////////////////////////////////////////
// ***** STATION RUN *****
////////////////////////////////////////////////////////////

uint16_t  staTime;
uint16_t  staCurrTime;
uint16_t  staStartTime;
uint8_t   staLevel;
uint8_t   staStars;
uint8_t   staStations;          // stations target
uint8_t   staCurrStation;       // stations counter
uint8_t   staLastStation;       // last color station
uint8_t   staMaxStations;
uint8_t   staMaxTurnout;

#define MAX_STATIONS  5

const uint16_t staColors[MAX_STATIONS] = {COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_WHITE, COLOR_CYAN};

uint16_t staTurnoutAdr1;
uint16_t staTurnoutAdr2;
uint16_t staTurnoutAdr3;
uint16_t staTurnoutAdr4;
uint8_t staTurnoutDef;
bool staTurnoutPos[4];

////////////////////////////////////////////////////////////
// ***** NEXT TRAIN *****
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
// ***** Z21 *****
////////////////////////////////////////////////////////////

#define csNormalOps                   0x00                  // Normal Operation Resumed
#define csEmergencyStop               0x01                  // Emergency Stop
#define csEmergencyOff                0x02                  // Emergency off. Xpressnet
#define csTrackVoltageOff             0x02                  // Emergency off. Z21
#define csStartMode                   0x04                  // Start mode.  Xpressnet
#define csShortCircuit                0x04                  // ShortCircuit
#define csServiceMode                 0x08                  // Service mode. Xpressnet
#define csReserved                    0x10
#define csProgrammingModeActive       0x20                  // Service Mode. Z21
#define csPowerUp                     0x40                  // Xpressnet
#define csErrorRAM                    0x80                  // Error RAM. Xpressnet

byte packetBuffer[1500];                                    // buffer to hold incoming packet, max. 1472 bytes
byte OutData[80];
byte OutPos = 0;
byte OutXOR;

// Z21 white message header
#define LAN_GET_SERIAL_NUMBER         0x10
#define LAN_GET_CODE                  0x18
#define LAN_GET_HWINFO                0x1A
#define LAN_LOGOFF                    0x30
#define LAN_X_Header                  0x40
#define LAN_SET_BROADCASTFLAGS        0x50
#define LAN_GET_BROADCASTFLAGS        0x51
#define LAN_GET_LOCOMODE              0x60
#define LAN_SET_LOCOMODE              0x61                  // Not implemented
#define LAN_GET_TURNOUTMODE           0x70
#define LAN_SET_TURNOUTMODE           0x71                  // Not implemented
#define LAN_RMBUS_DATACHANGED         0x80
#define LAN_RMBUS_GETDATA             0x81
#define LAN_RMBUS_PROGRAMMODULE       0x82                  // Not implemented
#define LAN_SYSTEMSTATE_DATACHANGED   0x84
#define LAN_SYSTEMSTATE_GETDATA       0x85
#define LAN_RAILCOM_DATACHANGED       0x88
#define LAN_RAILCOM_GETDATA           0x89
#define LAN_LOCONET_Z21_RX            0xA0
#define LAN_LOCONET_Z21_TX            0xA1
#define LAN_LOCONET_FROM_LAN          0xA2
#define LAN_LOCONET_DISPATCH_ADDR     0xA3                  // unused
#define LAN_LOCONET_DETECTOR          0xA4
#define LAN_FAST_CLOCK_CONTROL        0xCC
#define LAN_FAST_CLOCK_DATA           0xCD
#define LAN_FAST_CLOCK_SETTINGS_GET   0xCE                  // unused
#define LAN_FAST_CLOCK_SETTINGS_SET   0xCF                  // unused

enum xAnswer {DATA_LENL, DATA_LENH, DATA_HEADERL, DATA_HEADERH, XHEADER, DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7, DB8};

#define Z21_PING_INTERVAL 5                                 // Prevent automatic LOGOFF (5s)

bool waitResultCV;


////////////////////////////////////////////////////////////
// ***** XPRESSNET LAN *****
////////////////////////////////////////////////////////////

enum xPacket {FRAME1, FRAME2, HEADER, DATA1, DATA2, DATA3, DATA4, DATA5};

#define XNET_PING_INTERVAL 10000UL                          // Prevent closing the connection

volatile byte rxBufferXN[20];                               // Comunicacion Xpressnet
volatile byte txBuffer[14];
volatile byte txBytes;
volatile byte rxBytes;

byte rxXOR, rxIndice, txXOR, rxData;
byte xnetVersion, xnetCS;
bool getInfoLoco, getResultsSM;
bool askMultimaus;                                          // Multimaus
byte highVerMM, lowVerMM;

unsigned long timeoutXnet;

////////////////////////////////////////////////////////////
// ***** LOCONET OVER TCP *****
////////////////////////////////////////////////////////////

lnMsg SendPacket;                                           // Paquete a enviar por Loconet WiFi
lnMsg RecvPacket;                                           // Paquete recibido por Loconet WiFi

char rcvStr[10];
byte rcvStrPos;
enum rcvPhase {WAIT_TOKEN, RECV_TOKEN, RECV_PARAM, SENT_TOKEN, SENT_PARAM};
byte rcvStrPhase;
bool sentOK;
unsigned long timeoutSend;

enum statusSlot {STAT_FREE, STAT_COMMON, STAT_IDLE, STAT_IN_USE};

struct slot {
  byte num;                                                 // slot number: 1..120 (0x01..0x78)
  byte state;                                               // state: in_use/idle/common/free
  byte trk;                                                 // track status
} mySlot;

const byte stepsLN[] = {28, 28, 14, 128, 28, 28, 14, 128};

#define LNET_PING_INTERVAL 55000UL                          // Prevent PURGE of slot (55s)

bool doDispatchGet, doDispatchPut;                          // dispatching
uint8_t lastTrk;                                            // last received global track status

uint8_t autoIdentifyCS;
bool lnetProg;                                              // programando CV o LNCV
byte ulhiProg;                                              // Programming track off (UHLI)

enum lncv {LNCV_ART, LNCV_MOD, LNCV_ADR, LNCV_VAL};
unsigned int artNum;
unsigned int modNum;
unsigned int numLNCV;
unsigned int valLNCV;

byte optLNCV;


////////////////////////////////////////////////////////////
// ***** ECoS *****
////////////////////////////////////////////////////////////

#define ID_ECOS             1                               // Base objects ID
#define ID_PRGMANAGER       5
#define ID_POMMANAGER       7
#define ID_LOKMANAGER       10
#define ID_SWMANAGER        11
#define ID_SNIFFERMANAGER   25
#define ID_S88MANAGER       26
#define ID_BOOSTERMANAGER   27
#define ID_S88FEEDBACK      100
#define ID_INTBOOSTER       65000

char cmd[64];                                               // send buffer

#define NAME_LNG 16                                         // loco names length

#define BUF_LNG 1024
char inputBuffer[BUF_LNG];
unsigned int inputLength;

#define TEXTLEN         32             // Length of symbols in input

char  putBackChr;
int   posFile;

// Tokens
enum {
  T_NULL, T_START, T_ENDB, T_REPLY, T_EVENT, T_END, T_COMMA, T_INTLIT, T_SEMI, T_LPARENT, T_RPARENT,
  T_LBRACKET, T_RBRACKET, T_STRLIT, T_IDENT, T_PRINT, T_INT,
  T_GET, T_SET, T_QOBJ, T_ADDR, T_NAME, T_PROT, T_STATUS, T_STATUS2, T_STATE,
  T_REQ, T_VIEW, T_CONTROL, T_RELEASE, T_FORCE,
  T_GO, T_STOP, T_SHUTDWN, T_OK, T_MSG, T_SPEED, T_STEPS, T_DIR, T_FUNC, T_FSYMBOL, T_FSYMB, T_FICON,
  T_LOST, T_OTHER, T_CV, T_CVOK, T_ERROR, T_SWITCH,
  T_CS1, T_ECOS, T_ECOS2, T_APPV,
};

// Token structure
struct token {
  char token;
  int intvalue;
};

struct token T;
int   tokenType;
char  Text[TEXTLEN + 1];

int errCode;
int idManager;
int idCommand;
int numLoks;
int lastNumValue;
bool requestedCV;
int appVer;

byte msgDecodePhase;
enum {MSG_WAIT, MSG_START, MSG_REPLY, MSG_EVENT, MSG_END, MSG_REPLYBODY, MSG_EVENTBODY};

uint8_t mySpeedStep;

const uint8_t FunktionsTastenSymbole[] = {                  // Conversion table from ECoS v4.2.9 to PacoMouseCYD function icons
  1,  1,   2,  3,  4,  5,  6,  7,
  8,  9,  10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  2,  2,  24,  2,  2, 25, 26, 27,   //31

  28, 29, 30, 2, 31,  2, 32,  2,
  2,  2,  2,  2, 33, 34,  2,  2,   //47
  2,  2,  2,  2,  2,  2,  2, 35,
  2, 36,  2,  2,  2,  2,  2,  2,   //63

  2,  2,  2,  2,  2,  2,  2,  2,
  2,  2,  2,  2,  2,  2,  2,  2,   //79
  2,  2,  2,  2, 37,  2,  2, 38,
  39, 2,  2,  2,  2,  2,  2,  2,   //95

  2,  2,  2,  2,  2, 39,  2,  2,   //103
  2,  2,  2,  2,  2,  2,  2,  2,
  2,  2,  2,  2,  2,  2,  2,  2,
  2,  2,  2,  2, 40,  2,  2,  2,   //127
};


const uint8_t FunktionsTastenSymboleCS1[] = {               // Conversion table from CS1 v2.0.4 to PacoMouseCYD function icons
  2, 10,  3, 14,  5, 31, 15, 16,
  11, 13, 17,  9, 12,  6,  7, 23,
  4,  2, 25, 26, 22,  2, 30, 27,
  2,  2,  2, 28, 29, 20, 21,  8,   //31

  18,  2,  2,  2,  2,  2,  2,  2,
  2,  2,  2,  2,  2,  2,  2,  2,  //47
  2,  2,  2,  2,  2,  2,  2,  2,
  2,  2,  2,  2,  2,  2,  2,  2,  //63
};


////////////////////////////////////////////////////////////
// ***** MAIN PROGRAM *****
////////////////////////////////////////////////////////////

void setup() {
  uint16_t posLabel;
  initPins();
  delay(500);                                               // power-up safety delay
#ifdef DEBUG
  Serial.begin(115200);                                     // Debug messages, serial at 115200b
  DEBUG_MSG("\n\nPacoMouseCYD v%s.%s", VER_H, VER_L);
  DEBUG_MSG(USER_SETUP_INFO);
  DEBUG_MSG("Chip Model: %s \nFlash Chip Size: %lu", ESP.getChipModel(), ESP.getFlashChipSize())
  DEBUG_MSG("ESP_ARDUINO_VERSION: %d.%d.%d", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH)
#endif
#ifdef SCREEN_SEND
  Serial.begin(115200);                                     // Set to a high rate for fast image transfer to a PC
#endif
  initGUI();
  initVariables();
  sdDetected = SD.begin(SD_CS) ? true : false;
  DEBUG_MSG("SD Card %s", sdDetected ? "found" : "begin failed")
  tft.init();
  tft.fillScreen(COLOR_BLACK);
#ifdef DEBUG
  DEBUG_MSG("Display driver: %X %dx%d", TFT_DRIVER, TFT_WIDTH, TFT_HEIGHT)
  DEBUG_MSG("TFT BL pin: %d", TFT_BL)
  DEBUG_MSG("GUI Usage:\n Timers: %d\n Windows: %d\n Labels: %d", MAX_SYS_TIMER, MAX_WIN_OBJ, MAX_LABEL_OBJ)
  DEBUG_MSG(" Draw Strings: %d\n Chars: %d\n Function Icons: %d", MAX_DRAWSTR_OBJ, MAX_CHAR_OBJ, MAX_FNC_OBJ)
  DEBUG_MSG(" Icons: %d\n Buttons: %d\n Radio Buttons: %d", MAX_ICON_OBJ, MAX_BUT_OBJ, MAX_RAD_OBJ)
  DEBUG_MSG(" Progress Bar: %d\n Loco Pictures: %d\n Gauges: %d", MAX_BAR_OBJ, MAX_LPIC_OBJ, MAX_GAUGE_OBJ)
  DEBUG_MSG(" Text Box: %d\n Switch: %d\n Keyboard: %d", MAX_TXT_OBJ, MAX_SWITCH_OBJ, MAX_KEYB_OBJ)
#endif

  // Setting up the LEDC and configuring the Back light pin
  // NOTE: this needs to be done after tft.init()
#if (ESP_ARDUINO_VERSION_MAJOR > 2)
  DEBUG_MSG("PWM LED code for version 3.x")
  ledcAttach(TFT_BL, LEDC_BASE_FREQ, LEDC_RESOLUTION);
#else
  DEBUG_MSG("PWM LED code for version 2.x")
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_RESOLUTION); // backlight PWM
  ledcAttachPin(TFT_BL, LEDC_CHANNEL_0);
#endif
  setBacklight (backlight);

  // setup the touchscreen
  // NOTE: this needs to be done after tft.init()
  touchscreen.begin(TFT_WIDTH, TFT_HEIGHT);
  setRotationDisplay(locationUSB);

  copyOutA = digitalRead (ENCODER_A);                       // init encoder ISR
  copyOutB = 0x80;
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, CHANGE);
#ifdef  ALTERNATE_ENCODER
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderISR, CHANGE);
#endif

  openWindow(WIN_LOGO);                                     // special window, don't use events just draw
  initStatus = initSequence();
  getLastLoco();
}


void loop() {
  timerProcess();
  hidProcess();
  wifiProcess();
  if (isWindow(WIN_STEAM))
    steamProcess();
  if (eventsPending > 0) {                                  // execute pending events
    eventProcess();
    DEBUG_MSG("New Event...")
  }
  if (clickDetected) {                                      // only individual clicks
    if (! touchscreen.touched())
      clickDetected = false;
  }
  else {
    if (touchscreen.touched()) {
      TSPoint p = touchscreen.getTouch();
      if (p.z > 350) {
        DEBUG_MSG("X: %d, Y: %d, Z: %d", p.x, p.y, p.z);
        sendClickEvent(p.x, p.y);
        clickDetected = true;
      }
    }
  }
}


////////////////////////////////////////////////////////////
// ***** SUPPORT *****
////////////////////////////////////////////////////////////

void initVariables() {
  uint16_t xm, xM, ym, yM, value, pos;
  EEPROM.begin(EEPROM_SIZE);
  eepromChanged = false;
  currLanguage = EEPROM.read(EE_LANGUAGE);                  // language
  if (currLanguage >= MAX_LANG)
    currLanguage = LANG_ENGLISH;
  calibrationPending = false;
  signalLost = false;
  for (pos = 0; pos < 4; pos++) {                           // WiFi networks
    value = EE_WIFI + (pos * sizeof(wifiSetting));
    EEPROM.get (value, wifiSetting);                        // read WiFi settings
    if (wifiSetting.ok != 0x464D) {                         // check correct EEPROM signature
      snprintf (wifiSetting.network, 16, "WiFi %d", pos + 1); // init WiFi network name
      snprintf (wifiSetting.ssid, 32, "");                  // init WiFi settings
      snprintf (wifiSetting.password, 64, "12345678");
      wifiSetting.CS_IP = IPAddress(192, 168, 0, 111);
      wifiSetting.port = 1234;
      wifiSetting.serverType = true;
      wifiSetting.protocol = CLIENT_Z21;
      wifiSetting.ok = 0x464D;
      EEPROM.put(value, wifiSetting);                       // also init calibration values
      touchscreen.setCalibration(0, 4095, 0, 4095);         // set default calibration values
      saveCalibrationValues();                              // save all
      DEBUG_MSG("Setting default WiFi %d & calibration values", pos);
    }
    snprintf (networkNamesBuf[pos], NAME_LNG + 1, "%s", wifiSetting.network);
    DEBUG_MSG("%d - %d - %s", pos, value, wifiSetting.network)
  }
  pos = EEPROM.read(EE_WIFI_NET) & 0x03;                    // read current WiFi settings
  radioData[RAD_NETWORKS].value = pos;
  value = EE_WIFI + (pos * sizeof(wifiSetting));
  EEPROM.get (value, wifiSetting);
  xm = (EEPROM.read(EE_XMIN_H) << 8) + EEPROM.read(EE_XMIN_L);
  xM = (EEPROM.read(EE_XMAX_H) << 8) + EEPROM.read(EE_XMAX_L);
  ym = (EEPROM.read(EE_YMIN_H) << 8) + EEPROM.read(EE_YMIN_L);
  yM = (EEPROM.read(EE_YMAX_H) << 8) + EEPROM.read(EE_YMAX_L);
  touchscreen.setCalibration(xm, xM, ym, yM);               // set calibration values
  DEBUG_MSG("xMin: %d, xMax: %d, yMin: %d, yMax: %d", xm, xM, ym, yM);
  backlight = EEPROM.read(EE_BACKLIGHT);                    // LEDs
  if (backlight < USER_MIN_BL)
    backlight = USER_MIN_BL;
  locationUSB = EEPROM.read(EE_USB_LOCATION);
  if (locationUSB > 0)
    locationUSB = USB_UP;
  activeRGB = EEPROM.read(EE_RGB_LED);
  currBatt = 0;                                             // Battery
  loadBatteryRange();
  actionSW_BOOT = ACT_UNDEF;                                // buttons & encoder
  actionSW_R = ACT_UNDEF;
  actionSW_G = ACT_UNDEF;
  actionSW_B = ACT_UNDEF;
  statusBOOT = LOW;
  statusSW_R = LOW;
  statusSW_G = LOW;
  statusSW_B = LOW;
  timeButtons = millis();
  lastTimeEncoder = millis();
  encoderValue = 0;
  encoderMax   = 2;
  encoderNeedService = false;
  initFIFO();                                               // accessories
  editAccessory = false;
  accPanelChanged = false;
  currPanel = 0;
  myTurnout = 1;
  initLastAspects();
  CVdata    = 3;                                            // CV programming
  CVaddress = 1;
  modeProg = false;
  enterCVdata = false;
  progFinished = false;
  progStepCV = PRG_IDLE;
  clockRate = 0;                                            // fast clock internal
  clockHour = 0;
  clockMin = 0;
  updateFastClock();
  initLocos();
  stopMode = EEPROM.read(EE_STOP_MODE);
  shuntingMode = (EEPROM.read(EE_SHUNTING) > 0) ? true : false;
  lockOptions = EEPROM.read(EE_LOCK) & 0x07;
  shortAddress = EEPROM.read(EE_SHORT);
  if ((shortAddress != 99) && (shortAddress != 127))
    shortAddress = 99;
  typeCmdStation = EEPROM.read(EE_CMD_STA);
  if (typeCmdStation > CMD_DIG)
    typeCmdStation = CMD_DR;
  autoIdentifyCS = EEPROM.read(EE_CMD_AUTO);
  speedoPhase  = SPD_WAIT;
  speedoScale  = 87;
  speedoLength = 1000;
  speedoSpeed  = 0;
  oldSteamLoco = 0;
  mySlot.num = 0;                                           // Loconet slots
  mySlot.trk = 0x01;                                        // Power on
  doDispatchGet = false;
  doDispatchPut = false;
  lnetProg = false;
  ulhiProg = UHLI_PRG_END;
  lastTrk = 0x80;
  msgDecodePhase = MSG_WAIT;                                // ECoS
  requestedCV = false;
  appVer = 4;
  staTurnoutAdr1 = staGetTurnoutAdr(EE_STA_ADRH1, 1);       // Station Run
  staTurnoutAdr2 = staGetTurnoutAdr(EE_STA_ADRH2, 2);
  staTurnoutAdr3 = staGetTurnoutAdr(EE_STA_ADRH3, 3);
  staTurnoutAdr4 = staGetTurnoutAdr(EE_STA_ADRH4, 4);
  staTurnoutDef =  EEPROM.read(EE_STA_TRNDEF);
  staMaxTurnout =  EEPROM.read(EE_STA_TRNNUM);
  staMaxTurnout =  constrain(staMaxTurnout, 1, 4);
  staMaxStations = EEPROM.read(EE_STA_NUM);
  staMaxStations = constrain(staMaxStations, 3, 5);
  staStartTime =   EEPROM.read(EE_STA_TIME);

}


void hidProcess() {
  if (encoderNeedService)                                   // detectado cambio en pines del encoder
    encoderService();
  if (encoderChange)                                        // se ha movido el encoder
    controlEncoder();
  if (switchOn)                                             // se ha pulsado el boton del encoder
    controlSwitch();
  if (millis() - timeButtons > timeoutButtons)              // lectura de boton
    readButtons();
}


void updateFastClock() {
  if (clockRate > 0)
    sprintf(clockBuf, "%02d:%02d", clockHour, clockMin);
  else
    sprintf(clockBuf, "");
  if (isWindow(WIN_THROTTLE))
    newEvent(OBJ_TXT, TXT_CLOCK, EVNT_DRAW);
}

void setSpeedoPhase(uint8_t phase) {
  drawStrData[DSTR_SPEEDO_BLANK].x = 40 + (speedoPhase * 32);
  speedoPhase = phase;
  iconData[ICON_SPEEDO_LOK].x = 40 + (speedoPhase * 32);
  drawObject(OBJ_DRAWSTR, DSTR_SPEEDO_BLANK);
  drawObject(OBJ_ICON, ICON_SPEEDO_LOK);
}


////////////////////////////////////////////////////////////
// ***** SOPORTE GUI *****
////////////////////////////////////////////////////////////

void setBitsCV() {
  uint16_t n;
  for (n = 0; n < 8; n++) {
    buttonData[BUT_CV_0 + n].backgnd = bitRead(CVdata, n) ? COLOR_BLUE : COLOR_DARKGREY;
    charData[CHAR_CV_0 + n].color = bitRead(CVdata, n) ? COLOR_WHITE : COLOR_BROWN;
  }
}


void setFieldsCV() {
  txtData[TXT_CV].backgnd = enterCVdata ? COLOR_WHITE : COLOR_YELLOW;
  txtData[TXT_CV_VAL].backgnd = enterCVdata ? COLOR_YELLOW : COLOR_WHITE;
  keybData[KEYB_CV].idTextbox = enterCVdata ? TXT_CV_VAL : TXT_CV;
  snprintf(keybCvBuf, ADDR_LNG + 1, "%d", CVaddress);
  if (CVdata > 255) {
    keybCvValBuf[0] = '\0';
    CVdata = 0;
    txtData[TXT_CV_VAL].backgnd = COLOR_PINK;
  }
  else
    snprintf(keybCvValBuf, IP_LNG + 1, "%d", CVdata);
}

void showFieldsCV() {
  uint16_t n;
  newEvent(OBJ_TXT, TXT_CV, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_CV_VAL, EVNT_DRAW);
  for (n = 0; n < 8; n++)
    newEvent(OBJ_BUTTON, BUT_CV_0 + n, EVNT_DRAW);
}

void setFieldsLNCV() {
  txtData[TXT_LNCV_ART].backgnd = (optLNCV == LNCV_ART) ? COLOR_YELLOW : COLOR_WHITE;
  txtData[TXT_LNCV_MOD].backgnd = (optLNCV == LNCV_MOD) ? COLOR_YELLOW : COLOR_WHITE;
  txtData[TXT_LNCV_ADR].backgnd = (optLNCV == LNCV_ADR) ? COLOR_YELLOW : COLOR_WHITE;
  txtData[TXT_LNCV_VAL].backgnd = (optLNCV == LNCV_VAL) ? COLOR_YELLOW : COLOR_WHITE;
  switch (optLNCV) {
    case LNCV_ART:
      keybData[KEYB_LNCV].idTextbox = TXT_LNCV_ART;
      break;
    case LNCV_MOD:
      keybData[KEYB_LNCV].idTextbox = TXT_LNCV_MOD;
      break;
    case LNCV_ADR:
      keybData[KEYB_LNCV].idTextbox = TXT_LNCV_ADR;
      break;
    case LNCV_VAL:
      keybData[KEYB_LNCV].idTextbox = TXT_LNCV_VAL;
      break;

  }
  snprintf(keybLncvArtBuf, PORT_LNG + 1, "%d", artNum);
  snprintf(keybLncvModBuf, PORT_LNG + 1, "%d", modNum);
  snprintf(keybLncvAdrBuf, PORT_LNG + 1, "%d", numLNCV);
  snprintf(keybLncvValBuf, PORT_LNG + 1, "%d", valLNCV);
}

void showFieldsLNCV() {
  newEvent(OBJ_TXT, TXT_LNCV_ART, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_LNCV_MOD, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_LNCV_ADR, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_LNCV_VAL, EVNT_DRAW);
}


void setStatusCV() {
  uint16_t num;
  char buf[MAX_LABEL_LNG];
  num = 0;
  if (CVdata > 255) {
    num = LBL_CV_ERROR;
    txtData[TXT_CV_STATUS].color = COLOR_RED;
  }
  else {
    txtData[TXT_CV_STATUS].color = COLOR_BLUE;
    switch (CVaddress) {
      case 1:
      case 17:
      case 18:
      case 19:
        num = LBL_CV_ADDR;
        break;
      case 2:
        num = LBL_CV_SPD_L;
        break;
      case 3:
        num = LBL_CV_ACC;
        break;
      case 4:
        num = LBL_CV_DEC;
        break;
      case 5:
        num = LBL_CV_SPD_H;
        break;
      case 6:
        num = LBL_CV_SPD_M;
        break;
      case 541:
      case 29:
        num = LBL_CV_CFG;
        break;
      case 520:
      case 8:
        switch (CVdata) {                                   // imprime el fabricante conocido
          case 13:
            sprintf(buf, "DIY");
            break;
          case 74:
            sprintf(buf, "PpP");
            break;
          case 42:
            sprintf(buf, "Digikeijs");
            break;
          case 151:
            sprintf(buf, "ESU");
            break;
          case 145:
            sprintf(buf, "Zimo");
            break;
          case 99:
            sprintf(buf, "Lenz");
            break;
          case 97:
            sprintf(buf, "D&H");
            break;
          case 157:
            sprintf(buf, "Kuehn");
            break;
          case 62:
            sprintf(buf, "Tams");
            break;
          case 85:
            sprintf(buf, "Uhlenbrock");
            break;
          case 134:
            sprintf(buf, "Lais");
            break;
          case 129:
            sprintf(buf, "Digitrax");
            break;
          case 161:
            sprintf(buf, "Roco");
            break;
          case 109:
            sprintf(buf, "Viessmann");
            break;
          case 78:
            sprintf(buf, "Train-O-matic");
            break;
          case 117:
            sprintf(buf, "CT Elektronik");
            break;
          default:
            num = LBL_CV_MAN;
            break;
        }
        break;
      default:
        num = LBL_MENU_CV;
        break;
    }
  }
  if (num > 0)
    getLabelTxt(num, buf);
  snprintf(cvStatusBuf, PWD_LNG + 1, "%s", buf);
}


void setTextSpeedo() {
  uint8_t index;
  char scaleName[5];
  switch (speedoScale) {
    case 87:
      index = LBL_SCALE_H0;
      break;
    case 160:
      index = LBL_SCALE_N;
      break;
    case 120:
      index = LBL_SCALE_TT;
      break;
    case 220:
      index = LBL_SCALE_Z;
      break;
    case 45:
      index = LBL_SCALE_0;
      break;
    default:
      index = 0;
  }
  scaleName[0] = '\0';
  if (index > 0)
    getLabelTxt(index, scaleName);
  snprintf(spdScaleBuf, NAME_LNG + 1, "%s  1:%d", scaleName, speedoScale);
  snprintf(spdSelScaleBuf, NAME_LNG + 1, "%s  1:", scaleName);
  snprintf(spdSelScaleNumBuf, ADDR_LNG + 1, "%d", speedoScale);
}

void setScaleSpeedo(uint16_t value) {
  speedoScale = value;
  setTextSpeedo();
  drawObject(OBJ_TXT, TXT_EDIT_SCALE);
  drawObject(OBJ_TXT, TXT_NUM_SCALE);
}

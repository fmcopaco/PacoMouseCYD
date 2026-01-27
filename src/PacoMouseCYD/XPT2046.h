/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
      Simple XPT2046 SPI/Bitbang interface for PacoMouseCYD
*/


#ifndef XPT2046_TS_h
#define XPT2046_TS_h

#include "Arduino.h"
#include "config.h"
#ifdef USE_XPT2046_SPI
#include <SPI.h>
#endif

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t z;
} TSPoint;

struct TouchCalibration {
  uint16_t xMin;
  uint16_t xMax;
  uint16_t yMin;
  uint16_t yMax;
  uint16_t rotation;
};

class XPT2046_TS {
  public:
    XPT2046_TS(uint8_t mosiPin, uint8_t misoPin, uint8_t clkPin, uint8_t csPin);
    void begin(uint16_t width = 240, uint16_t height = 320);
    bool touched();
    TSPoint getTouch();
    void setRotation(uint8_t n);
    void setCalibration(uint16_t xMin, uint16_t xMax, uint16_t yMin, uint16_t yMax);
    TouchCalibration getCalibration();
    void readData(uint16_t *x, uint16_t *y, uint16_t *z);

  private:
    uint8_t _mosiPin;
    uint8_t _misoPin;
    uint8_t _clkPin;
    uint8_t _csPin;
    uint8_t _irqPin;
    uint16_t _width;
    uint16_t _height;
    uint16_t _xraw;
    uint16_t _yraw;
    uint16_t _zraw;
    uint32_t _msraw;
    TouchCalibration cal;
#ifdef USE_XPT2046_SPI
    SPIClass *hspi = NULL;
#endif
#ifdef USE_XPT2046_BITBANG
    uint16_t readSPI(byte command);
#endif
    void update();
};

#endif

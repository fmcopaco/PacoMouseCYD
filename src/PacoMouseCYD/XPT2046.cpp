/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
      Simple XPT2046 SPI/Bitbang interface for PacoMouseCYD
*/

#include "config.h"
#include "XPT2046.h"


#define Z_THRESHOLD     300
#define MSEC_THRESHOLD  4

XPT2046_TS::XPT2046_TS(uint8_t mosiPin, uint8_t misoPin, uint8_t clkPin, uint8_t csPin) :
  _mosiPin(mosiPin), _misoPin(misoPin), _clkPin(clkPin), _csPin(csPin) {
  cal = TouchCalibration{0, 4095, 0, 4095, 0};                        // other initializations, if required
  _msraw = millis();
#ifdef USE_XPT2046_SPI
  hspi = new SPIClass(HSPI);                                          // XPT2046 connected to HSPI in CYD 2.4"
  hspi->begin();
#endif
  _readBatt = false;
  _battraw = 0;
}


void XPT2046_TS::begin(uint16_t width, uint16_t height) {
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
#ifdef USE_XPT2046_BITBANG
  pinMode(_clkPin, OUTPUT);                                           // init all pins in bitbang mode only (CYD 2.8")
  digitalWrite(_clkPin, LOW);
  pinMode(_mosiPin, OUTPUT);
  pinMode(_misoPin, INPUT);
#endif
  _width = width;
  _height = height;
}


void XPT2046_TS::setCalibration(uint16_t xMin, uint16_t xMax, uint16_t yMin, uint16_t yMax) {
  cal.xMin = xMin;
  cal.xMax = xMax;
  cal.yMin = yMin;
  cal.yMax = yMax;
}


TouchCalibration XPT2046_TS::getCalibration() {
  return cal;
}


void XPT2046_TS::setRotation(uint8_t n) {
  cal.rotation = n % 4;
}


bool XPT2046_TS::touched() {
  update();
  return (_zraw > Z_THRESHOLD);
}


TSPoint XPT2046_TS::getTouch() {
  update();
  uint16_t x = map(_xraw, cal.xMin, cal.xMax, 0, _width);
  uint16_t y = map(_yraw, cal.yMin, cal.yMax, 0, _height);
  if ((x >= _width) || (x <= 0) || (y >= _height) || (y <= 0))
    _zraw = 0;
  return TSPoint{x, y, _zraw};
}


void XPT2046_TS::readData(uint16_t *x, uint16_t *y, uint16_t *z) {
  update();
  *x = _xraw;                                                         // read raw data
  *y = _yraw;
  *z = _zraw;
}


uint16_t XPT2046_TS::readBattData() {
  _readBatt = true;
  update();
  return (_battraw);                                                  // read raw data
}


#ifdef USE_XPT2046_BITBANG
uint16_t XPT2046_TS::readSPI(byte command) {
  uint16_t result = 0;
  for (int i = 7; i >= 0; i--) {
    digitalWrite(_mosiPin, command & (1 << i));                       // send command
    digitalWrite(_clkPin, HIGH);
    delayMicroseconds(7);
    digitalWrite(_clkPin, LOW);
    delayMicroseconds(7);
  }
  for (int i = 11; i >= 0; i--) {                                     // read data
    digitalWrite(_clkPin, HIGH);
    delayMicroseconds(7);
    digitalWrite(_clkPin, LOW);
    delayMicroseconds(7);
    result |= (digitalRead(_misoPin) << i);
  }
  return result;
}


void XPT2046_TS::update() {
  int t;
  uint32_t now = millis();
  if (now - _msraw < MSEC_THRESHOLD)
    return;
  digitalWrite(_csPin, LOW);
  readSPI(0xB0);
  readSPI(0xB0);
  readSPI(0xB0);
  int z1 = readSPI(0xB0);
  _zraw = z1 + 4095;
  readSPI(0xC0);
  readSPI(0xC0);
  readSPI(0xC0);
  int z2 = readSPI(0xC0);
  _zraw -= z2;
  readSPI(0x90);
  readSPI(0x90);
  readSPI(0x90);
  _xraw = readSPI(0x90);
  readSPI(0xD0);
  readSPI(0xD0);
  readSPI(0xD0);
  _yraw = readSPI(0xD0);
  if (_readBatt) {
    readSPI(0xA6);
    readSPI(0xA6);
    readSPI(0xA6);
    _battraw = readSPI(0xA6);
  }
  digitalWrite(_csPin, HIGH);
  _msraw = now;
  switch (cal.rotation) {
    case 0:
      t = 4095 - _yraw;
      _yraw = _xraw;
      _xraw = t;
      break;
    case 1:
      break;
    case 2:
      t = _xraw;
      _xraw = _yraw;
      _yraw = 4095 - t;
      break;
    default:
      _xraw = 4095 - _xraw;
      _yraw = 4095 - _yraw;
      break;
  }
}
#endif


#ifdef USE_XPT2046_SPI
void XPT2046_TS::update() {
  int t;
  uint32_t now = millis();
  if (now - _msraw < MSEC_THRESHOLD)
    return;
  hspi->beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_csPin, LOW);
  hspi->transfer(0xB0);
  hspi->transfer16(0xB0);
  hspi->transfer16(0xB0);
  hspi->transfer16(0xB0);
  int z1 = hspi->transfer16(0xC0) >> 3;
  _zraw = z1 + 4095;
  hspi->transfer16(0xC0);
  hspi->transfer16(0xC0);
  hspi->transfer16(0xC0);
  int z2 = hspi->transfer16(0x90) >> 3;
  _zraw -= z2;
  hspi->transfer16(0x90);
  hspi->transfer16(0x90);
  hspi->transfer16(0x90);
  _xraw = hspi->transfer16(0xD0) >> 3;
  hspi->transfer16(0xD0);
  hspi->transfer16(0xD0);
  hspi->transfer16(0xD0);
  _yraw = hspi->transfer16(0x0) >> 3;
  if (_readBatt) {
    hspi->transfer16(0xA6);
    hspi->transfer16(0xA6);
    hspi->transfer16(0xA6);
    _battraw = hspi->transfer16(0xA6);
  }
  digitalWrite(_csPin, HIGH);
  hspi->endTransaction();
  _msraw = now;
  switch (cal.rotation) {
    case 0:
      _xraw = 4095 - _xraw;
      break;
    case 1:
      t = _yraw;
      _yraw = _xraw;
      _xraw = t;
      break;
    case 2:
      _yraw = 4095 - _yraw;
      break;
    default:
      t = _yraw;
      _yraw = 4095 - _xraw;
      _xraw = 4095 - t;
      break;
  }
}
#endif

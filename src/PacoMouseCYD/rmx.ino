/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/

      This software and associated files are a DIY project that is not intended for commercial use.
      This software uses libraries with different licenses, follow all their different terms included.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.

      Sources are only provided for building and uploading to the device.
      You are not allowed to modify the source code or fork/publish this project.
      Commercial use is forbidden.
*/

/*
  #define RMX_PKT       0x7C                                  // Packet start
  #define RMX_OK        0x00
  #define RMX_ERR       0x01
  #define RMX_MODE      0x03
  #define RMX_STATUS    0x04
  #define RMX_WRITESX   0x05
  #define RMX_READSX    0x06
  #define RMX_LOCO_INFO 0x08
  #define RMX_CHANNEL   0x20
  #define RMX_LOCO_SPD  0x24
  #define RMX_LOCO_FNC  0x28

  enum rmxData {RMX_HEADER, RMX_LNG, RMX_CMD, RMX_DATA0, RMX_DATA1, RMX_DATA2, RMX_DATA3, RMX_DATA4, RMX_DATA5, RMX_DATA6, RMX_DATA7};

*/

////////////////////////////////////////////////////////////
// ***** RMXnet SOPORTE *****
////////////////////////////////////////////////////////////

void showTrkRMX() {
  if (csStatus > 0) {
    stopTimer (TMR_POWER);
    iconData[ICON_POWER].color = COLOR_GREEN;
    setColorRGB(LED_RGB_GREEN);
    if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
      newEvent(OBJ_ICON, ICON_POWER, EVNT_DRAW);
    if (isWindow(WIN_STA_PLAY)) {
      fncData[FNC_STA_RAYO].state = false;
      newEvent(OBJ_FNC, FNC_STA_RAYO, EVNT_DRAW);
    }
    if (isWindow(WIN_ALERT)) {
      switch (errType) {
        case ERR_SERV:
        case ERR_STOP:
        case ERR_CV:
          closeWindow(WIN_ALERT);
          break;
      }
    }
  }
  else {
    iconData[ICON_POWER].color = COLOR_RED;                       // Power off
    setTimer (TMR_POWER, 5, TMR_PERIODIC);                        // Flash power icon
    setColorRGB(LED_RGB_RED);
    if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
      newEvent(OBJ_ICON, ICON_POWER, EVNT_DRAW);
    if (isWindow(WIN_STA_PLAY)) {
      fncData[FNC_STA_RAYO].state = true;
      newEvent(OBJ_FNC, FNC_STA_RAYO, EVNT_DRAW);
    }
  }
}

void getStatusRMX() {                                             // ask for track status
  //sendRMX(RMX_STATUS, 3);
  DEBUG_MSG("Get Status")
}

void resumeOperationsRMX() {
  rmxOutBuf[RMX_DATA0] = 0x80;                                    // GO
  sendRMX(RMX_MODE, 4);
}

void emergencyOffRMX() {
  rmxOutBuf[RMX_DATA0] = 0x40;                                    // STOP
  sendRMX(RMX_MODE, 4);
}

void emergencyStopRMX() {
  rmxOutBuf[RMX_DATA0] = 0x08;                                    // Stop all locos
  sendRMX(RMX_MODE, 4);
}

byte getMaxStepRMX() {
  byte stp;
  switch (locoData[myLocoData].mySteps) {
    case 2:                                                       // SX1. 31 steps
      stp = 31;
      locoData[myLocoData].myProtocol = LOK_SX1;
      break;
    case 17:                                                      // SX2. 31 steps
      locoData[myLocoData].myProtocol = LOK_SX2;
      stp = 31;
      break;
    case 7:                                                       // SX2. 127 steps
      stp = 127;
      locoData[myLocoData].myProtocol = LOK_SX2;
      break;
    case 9:                                                       // DCC Short. 14 steps
    case 10:                                                      // DCC Long.  14 steps
      stp = 14;
      locoData[myLocoData].myProtocol = LOK_DCC;
      break;
    case 12:                                                      // DCC Short. 28 steps
    case 13:                                                      // DCC Long.  28 steps
      stp = 28;
      locoData[myLocoData].myProtocol = LOK_DCC;
      break;
    case 15:                                                      // DCC Short. 126 steps
    case 16:                                                      // DCC Long.  126 steps
      stp = 126;
      locoData[myLocoData].myProtocol = LOK_DCC;
      break;
    default:
      stp = 31;
      break;
  }
  return (stp);
}

byte getCurrentStepRMX() {
  return locoData[myLocoData].mySpeed;
}

void infoLocomotoraRMX (uint16_t loco) {
  uint16_t id;
  findLocoData(loco);
  if (sdDetected)
    readLocoData(SD, loco, myLocoData);
  else
    readLocoData(LittleFS, loco, myLocoData);
  updateSpeedHID();
  updateFuncState(isWindow(WIN_THROTTLE));
  lpicData[LPIC_MAIN].id = locoData[myLocoData].myLocoID;
}

void locoOperationSpeedRMX() {
  rmxOutBuf[RMX_DATA0] = locoData[myLocoData].myAddr.adr[1];      // loco number
  rmxOutBuf[RMX_DATA1] = locoData[myLocoData].myAddr.adr[0];
  rmxOutBuf[RMX_DATA2] = locoData[myLocoData].mySpeed;
  rmxOutBuf[RMX_DATA3] = (locoData[myLocoData].myDir & 0x80) ? 0 : 1;
  sendRMX(RMX_LOCO_SPD, 7);
}

void funcOperationsRMX(byte fnc) {
  rmxOutBuf[RMX_DATA0] = locoData[myLocoData].myAddr.adr[1];      // loco number
  rmxOutBuf[RMX_DATA1] = locoData[myLocoData].myAddr.adr[0];
  rmxOutBuf[RMX_DATA2] = locoData[myLocoData].myFunc.xFunc[0];    // Functions F0..F7
  rmxOutBuf[RMX_DATA3] = locoData[myLocoData].myFunc.xFunc[1];    // Functions F8..F15
  rmxOutBuf[RMX_DATA4] = locoData[myLocoData].myFunc.xFunc[2];    // Functions F16 (..F23)
  sendRMX(RMX_LOCO_FNC, 8);
}

void setAccessoryRMX(unsigned int FAdr, int pair, bool activate)  {
  uint8_t value, n;
  uint16_t adr;
  if (activate) {
    FAdr--;
    value = 0;
    adr = (FAdr & 0x07F8) + 1;                                    // initial address group
    for (n = 0; n < 8; n++) {
      if (bitRead(accPosition[(adr + n) >> 3], (adr + n) & 0x07)) // current status
        bitSet(value, n);
    }
    if (pair > 0)
      value |= (1 << (FAdr & 0x07));
    rmxOutBuf[RMX_DATA0] = 1;                                     // Bus 1
    rmxOutBuf[RMX_DATA1] = FAdr >> 3;                             // Channel
    rmxOutBuf[RMX_DATA2] = value;                                 // Port
    sendRMX(RMX_WRITESX, 6);
  }
}


void setTimeRMX(byte hh, byte mm, byte rate) {                    // Set Fast Clock. RMX unsupported

}

bool canProgRMX(uint32_t prot) {
  if (locoData[myLocoData].mySteps < 18) {
    if (prot & (1 << locoData[myLocoData].mySteps))               // Protocol for programming validation
      return true;
  }
  return false;
}

void readCVRMX (unsigned int adr, byte stepPrg) {                 // Read CV. DCC only?
  if (!modeProg) {
    if (canProgRMX(0x0003B680)) {                                 // Read DCC & SX2
      rmxOutBuf[RMX_DATA0] = 0;
      rmxOutBuf[RMX_DATA1] = adr >> 8;
      rmxOutBuf[RMX_DATA2] = adr & 0xFF;
      rmxOutBuf[RMX_DATA3] = canProgRMX(0x0001B600) ? 0x08 : 0x04;      // read
      rmxOutBuf[RMX_DATA4] = 0x00;
      sendRMX(RMX_PROG, 8);
      DEBUG_MSG("Read CV%d", adr)
    }
  }
}

void writeCVRMX (unsigned int adr, unsigned int data, byte stepPrg) {   // Write CV. DCC only?
  if (modeProg) {                                                 // main track
    if (canProgRMX(0x0003B680)) {                                 // Write POM DCC & SX2
      rmxOutBuf[RMX_DATA0] = locoData[myLocoData].myDir & 0x7F;   // Channel
      rmxOutBuf[RMX_DATA1] = adr >> 8;
      rmxOutBuf[RMX_DATA2] = adr & 0xFF;
      rmxOutBuf[RMX_DATA3] = canProgRMX(0x0001B600) ? 0x0B : 0x07;      // write POM
      rmxOutBuf[RMX_DATA4] = data;
      sendRMX(RMX_PROG, 8);
      DEBUG_MSG("Write POM CV%d", adr)
    }
  }
  else {
    if (canProgRMX(0x0003B680)) {                                 // Write DCC & SX2
      rmxOutBuf[RMX_DATA0] = 0;
      rmxOutBuf[RMX_DATA1] = adr >> 8;
      rmxOutBuf[RMX_DATA2] = adr & 0xFF;
      rmxOutBuf[RMX_DATA3] = canProgRMX(0x0001B600) ? 0x0A : 0x06;      // write
      rmxOutBuf[RMX_DATA4] = data;
      sendRMX(RMX_PROG, 8);
      DEBUG_MSG("Write CV%d", adr)
    }
  }
}

void queryFeedbackRMX(uint16_t fbk) {                             // Query feedback status. Read RMX1 channel?
  fbk--;
  rmxOutBuf[RMX_DATA0] = 1;                                       // Bus 1
  rmxOutBuf[RMX_DATA1] = fbk >> 3;                                // Channel
  sendRMX(RMX_READSX, 5);
}

////////////////////////////////////////////////////////////
// ***** RMXnet PROTOCOL *****
////////////////////////////////////////////////////////////

void sendRMX (uint8_t cmd, uint8_t lng) {
  rmxOutBuf[RMX_HEADER] = RMX_PKT;
  rmxOutBuf[RMX_LNG] = lng;
  rmxOutBuf[RMX_CMD] = cmd;
  Client.write((byte *)&rmxOutBuf[RMX_HEADER], lng);
}


void startRMX() {
  recordsRMX = 0;
  csStatus = 2;
  rmxOutBuf[RMX_DATA0] = RMX_OK;                                  // establish communication
  sendRMX(RMX_OK, 4);
  waitWifiData(100);
  rmxOutBuf[RMX_DATA0] = 1;                                       // loco database
  sendRMX(RMX_LOCO_INFO, 4);
  waitWifiData(100);
  //DEBUG_MSG("Loco database")
  rmxOutBuf[RMX_DATA0] = 2;                                       // initialize
  rmxOutBuf[RMX_DATA1] = 1;
  sendRMX(RMX_MODE, 5);
  //DEBUG_MSG("Initialize RMX")

}


void processRMX() {
  uint8_t rxByte;
  while (Client.available()) {
    rxByte = Client.read();
    rmxBuf[rmxPos] = rxByte;
    switch (rmxPos) {
      case RMX_HEADER:
        if (rxByte == RMX_PKT)
          rmxPos++;
        break;
      case RMX_LNG:
        rmxPos++;
        if (rxByte > 32)
          rmxPos = RMX_HEADER;                                    // too long, discard
        break;
      default:
        rmxPos++;
        if (rmxPos == rmxBuf[RMX_LNG]) {
          rmxPos = RMX_HEADER;
          parseRMX();
        }
        break;
    }
  }
}


void parseRMX() {
  uint16_t adr, pos, FAdr;
  uint8_t value, value1, value2, n;
#ifdef DEBUG
  for (uint8_t n = 0; n < rmxBuf[RMX_LNG]; n++) {
    Serial.print(rmxBuf[n], HEX);
    Serial.print(' ');
  }
  Serial.println();
#endif

  adr = (rmxBuf[RMX_DATA0] << 8) + rmxBuf[RMX_DATA1];
  switch (rmxBuf[RMX_CMD]) {
    case RMX_STATUS:
      value = (rmxBuf[RMX_DATA0] & 0x80) ? 1 : 0;                 // Power on/off
      if (value != csStatus) {
        csStatus = value;
        showTrkRMX();
      }
      break;
    case RMX_LOCO_SPD:
      if (adr == locoData[myLocoData].myAddr.address) {           // speed & direction
        locoData[myLocoData].mySpeed = rmxBuf[RMX_DATA2];
        if (rmxBuf[RMX_DATA3] > 0)
          bitClear(locoData[myLocoData].myDir, 7);
        else
          bitSet(locoData[myLocoData].myDir, 7);
        if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
          updateSpeedHID();
      }
      else {
        pos = 0;
        while (pos < LOCOS_IN_STACK) {                            // find address loco data
          if (locoData[pos].myAddr.address == adr) {
            locoData[pos].mySpeed = rmxBuf[RMX_DATA2];
            if (rmxBuf[RMX_DATA3] > 0)
              bitClear(locoData[myLocoData].myDir, 7);
            else
              bitSet(locoData[myLocoData].myDir, 7);
            pos = LOCOS_IN_STACK;
          }
          pos++;
        }
      }
      DEBUG_MSG("Loco: %d Speed: %d %s", adr, rmxBuf[RMX_DATA2], rmxBuf[RMX_DATA3] ? "REV" : "FWD");
      break;
    case RMX_LOCO_FNC:
      if (adr == locoData[myLocoData].myAddr.address) {           // Functions
        locoData[myLocoData].myFunc.xFunc[0] = rmxBuf[RMX_DATA2];         // Functions F0..F7
        locoData[myLocoData].myFunc.xFunc[1] = rmxBuf[RMX_DATA3];         // Functions F8..F15
        locoData[myLocoData].myFunc.xFunc[2] = rmxBuf[RMX_DATA4];         // Functions F16 (..F23)
        updateFuncState(isWindow(WIN_THROTTLE));
      }
      else {
        pos = 0;
        while (pos < LOCOS_IN_STACK) {                            // find address loco data
          if (locoData[pos].myAddr.address == adr) {
            locoData[pos].myFunc.xFunc[0] = rmxBuf[RMX_DATA2];            // Functions F0..F7
            locoData[pos].myFunc.xFunc[1] = rmxBuf[RMX_DATA3];            // Functions F8..F15
            locoData[pos].myFunc.xFunc[2] = rmxBuf[RMX_DATA4];            // Functions F16 (..F23)
            pos = LOCOS_IN_STACK;
          }
          pos++;
        }
      }
      break;
    case RMX_READSX:
      if (rmxBuf[RMX_DATA0] == 1) {                               // Bus 1. Switches and feedback
        value1 = rmxBuf[RMX_DATA1];
        value2 = rmxBuf[RMX_DATA2];
        for (n = 0; n < 8; n++) {
          FAdr = (value1 << 3) + n + 1;
          if (bitRead(value2, n) != bitRead(accPosition[FAdr >> 3], (FAdr & 0x07))) {
            accessoryChange(FAdr, bitRead(value2, n));
            DEBUG_MSG("RMX1: %d Data: %d FAdr: %d State: %d", value1, value2, FAdr, bitRead(value2, n));
          }
        }
        for (n = 0; n < MAX_AUTO_SEQ; n++) {
          if ((automation[n].opcode == 'Z') || (automation[n].opcode == 'z')) {
            if (value1 == ((automation[n].param - 1) >> 3))
              automation[n].value = bitRead(value2, (automation[n].param - 1) & 0x07);
          }
        }
      }
      break;
    case RMX_CHANNEL:
      pos = 0;                                                  // channel, protocol & steps
      while (pos < LOCOS_IN_STACK) {                            // find address loco data
        if (locoData[pos].myAddr.address == adr) {
          locoData[pos].myDir = locoData[pos].myDir & 0x80;
          if (!((rmxBuf[RMX_DATA3] == 0) && (rmxBuf[RMX_DATA4] == 0))) {
            locoData[pos].myDir += (rmxBuf[RMX_DATA2] & 0x7F);  // channel
            locoData[pos].mySteps = rmxBuf[RMX_DATA3];          // protocol
          }
          pos = LOCOS_IN_STACK;
        }
        pos++;
      }
      DEBUG_MSG("Channel: %d Adr: %d: Prot: %d Steps: %d", rmxBuf[RMX_DATA2], adr, rmxBuf[RMX_DATA3], rmxBuf[RMX_DATA4]);
      break;
    case RMX_LOCO_INFO:
      if (recordsRMX > 0) {                                       // loco data from database list
        if (numLoks < LOCOS_IN_STACK) {
          locoData[numLoks].myAddr.address = (rmxBuf[RMX_DATA0] << 8) + rmxBuf[RMX_DATA1];
          locoData[numLoks].mySteps = rmxBuf[RMX_DATA3];
          if (rmxBuf[RMX_LNG] > 7) {                              // name
            for (n = 7; n < rmxBuf[RMX_LNG]; n++)
              locoData[numLoks].myName[n - 7] = rmxBuf[n];
            locoData[numLoks].myName[n] = '\0';
            DEBUG_MSG("Loco: %d Short: %d Prot: %d Name: %s", locoData[numLoks].myAddr.address, rmxBuf[RMX_DATA2], locoData[numLoks].mySteps, locoData[numLoks].myName);
          }
          pushLoco(locoData[numLoks].myAddr.address);
          numLoks++;
        }
      }
      else {                                                      // new loco created
        DEBUG_MSG("New loco received")
      }
      break;
    case RMX_OK:
      switch (rmxBuf[RMX_DATA0]) {
        case 0:
          if (recordsRMX > 0) {                                   // end of loco database
            recordsRMX = 0;
          }
          break;
        case 1:
          switch (rmxBuf[RMX_LNG]) {
            case 4:                                               // positive confirmation
              break;
            case 6:                                               // start of loco database
              recordsRMX = (rmxBuf[RMX_DATA1] << 8) + rmxBuf[RMX_DATA2];
              numLoks = 0;
              initLocos();
              DEBUG_MSG("New database: %d records", recordsRMX);
              break;
          }
          break;
      }
      break;
    case RMX_ERR:                                                 // Errors
      /*
        0x01: unbekannter OPCODE
        0x03: Loknummer nicht in Datenbank
        0x04: Eingabefehler
        0x05: Mode ungleich 0x01
        0x07: Eingabe Lokomotiven Datenbank voll
        0x08: Steuerkanäle belegt
      */
      DEBUG_MSG("ERROR: %d", rmxBuf[RMX_DATA0])
      break;
    case RMX_PROG:
      CVdata = (rmxBuf[RMX_DATA3] & 0x80) ? 0x0600 : rmxBuf[RMX_DATA4];
      endProg();
      break;

  }
}

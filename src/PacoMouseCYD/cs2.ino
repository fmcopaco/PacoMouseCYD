/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/

      This software and associated files are a DIY project that is not intended for commercial use.
      This software uses libraries with different licenses, follow all their different terms included.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.

      Sources are only provided for building and uploading to the device.
      You are not allowed to modify the source code or fork/publish this project.
      Commercial use is forbidden.
*/

////////////////////////////////////////////////////////////
// ***** CS2 SOPORTE *****
////////////////////////////////////////////////////////////

void showErrorCS2 (uint8_t errorCS) {                                     // muestra pantalla de error
  switch (errorCS) {
    case CS2_SYS_STOP:
      iconData[ICON_POWER].color = COLOR_RED;
      setTimer (TMR_POWER, 5, TMR_PERIODIC);                              // Flash power icon
      setColorRGB(LED_RGB_RED);
      if ((isWindow(WIN_ALERT)) && (errType == ERR_STOP))
        closeWindow(WIN_ALERT);
      if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
        newEvent(OBJ_ICON, ICON_POWER, EVNT_DRAW);
      if (isWindow(WIN_STA_PLAY)) {
        fncData[FNC_STA_RAYO].state = true;
        newEvent(OBJ_FNC, FNC_STA_RAYO, EVNT_DRAW);
        break;
      case CS2_SYS_HALT:
        setColorRGB(LED_RGB_BLUE);
        if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
          alertWindow(ERR_STOP);
        break;
      }
  }
}


void showNormalOpsCS2() {
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

////////////////////////////////////////////////////////////
// ***** CS2 PROTOCOL *****
////////////////////////////////////////////////////////////

/// UDP --> CS3-Settings: CAN-Gateway must be set to "broadcast" and IP-adress must be given
/// TCP --> CS3-Settings: CAN-Gateway must be set to "auto"

void processCS2() {
  int len, packetSize;
  if (wifiSetting.serverType) {                                           // TCP
    while (Client.available()) {
      packetBuffer[posPktCS2++] = Client.read();
      if (posPktCS2 == CS2_PKT_LNG) {                                     // CS2 packets are always 13 bytes
        DecodeCS2();                                                      // decode received packet
        posPktCS2 = CS2_PRIO;
      }
    }
  }
  else {                                                                  // UDP
    packetSize = Udp.parsePacket();
    if (packetSize) {
      len = Udp.read(packetBuffer, packetSize);                           // read the packet into packetBufffer
      if (len == CS2_PKT_LNG)                                             // CS2 packets are always 13 bytes
        DecodeCS2();                                                      // decode received packet
    }
  }
  if (millis() - pingTimer > CS2_PING_INTERVAL) {                         // Refresh to maintain connection
    pingTimer = millis();
    pingCS2();
  }
}


void clearDataCS2() {
  for (int n = 0; n < CS2_PKT_LNG; n++)                                   // CS2 packets are always 13 bytes. unused bytes should to be set to 0
    OutData[n] = 0;
  OutData[CS2_HASH_H] = CS2_HASH_MAGIC_H;                                 // Default data for hash 0x0300 / 0x4711 / 0x0769
  OutData[CS2_HASH_L] = CS2_HASH_MAGIC_L;
}

void sendDataCS2() {
  DEBUG_MSG("TX C:%0X R:%d  H:%0X%0X DLC:%d", ((OutData[CS2_PRIO] << 7) | (OutData[CS2_CMD] >> 1)), (OutData[CS2_CMD] & 0x01), OutData[CS2_HASH_H], OutData[CS2_HASH_L], OutData[CS2_DLC])
  DEBUG_MSG("PAYLOAD: %0X %0X %0X %0X %0X %0X %0X %0X", OutData[CS2_DATA0], OutData[CS2_DATA1], OutData[CS2_DATA2], OutData[CS2_DATA3], OutData[CS2_DATA4], OutData[CS2_DATA5], OutData[CS2_DATA6], OutData[CS2_DATA7])

  if (wifiSetting.serverType) {                                           // TCP
    Client.write((byte *)&OutData[CS2_PRIO], CS2_PKT_LNG);                // envia paquete CS2
  }
  else  {                                                                 // UDP
    Udp.beginPacket(wifiSetting.CS_IP, CS2Port);
    Udp.write(OutData, CS2_PKT_LNG);
    Udp.endPacket();
  }
  delay(0);
}


void pingCS2() {                                                          // Ping CAN participants
  clearDataCS2();
  OutData[CS2_CMD] = CS2_PING;
  sendDataCS2();
}

void emergencyOffCS2() {
  clearDataCS2();
  OutData[CS2_CMD] = CS2_SYS;
  OutData[CS2_DLC] = 5;
  OutData[CS2_DATA4] = CS2_SYS_STOP;
  sendDataCS2();
  csStatus = 0;
}

void emergencyStopCS2() {
  clearDataCS2();
  OutData[CS2_CMD] = CS2_SYS;
  OutData[CS2_DLC] = 5;
  OutData[CS2_DATA4] = CS2_SYS_HALT;
  sendDataCS2();
  csStatus = 0;
}

void resumeOperationsCS2() {
  clearDataCS2();
  OutData[CS2_CMD] = CS2_SYS;
  OutData[CS2_DLC] = 5;
  OutData[CS2_DATA4] = CS2_SYS_GO;
  sendDataCS2();
  csStatus = 1;
}

void getStatusCS2() {                                                     // undocumented
  clearDataCS2();
  OutData[CS2_CMD] = CS2_SYS;
  OutData[CS2_DLC] = 4;
  sendDataCS2();
}
/*
  void bootCS2() {                                                          // undocumented. To activate communications as Rocrail do
  clearDataCS2();
  OutData[CS2_CMD] = CS2_BOOT;
  sendDataCS2();
  }
*/
void bootMessageCS2() {                                                   // undocumented. To activate Gleisbox without MS2 connected
  clearDataCS2();
  OutData[CS2_CMD] = CS2_BOOT;
  OutData[CS2_DLC] = 5;
  OutData[CS2_DATA4] = CS2_BOOT_MS2;
  sendDataCS2();
}

void newMsgCS2 (uint8_t cmd, uint8_t dlc, uint16_t uid) {
  clearDataCS2();
  OutData[CS2_CMD] = cmd;
  OutData[CS2_DLC] = dlc;
  OutData[CS2_DATA2] = highByte(uid);
  OutData[CS2_DATA3] = lowByte(uid);
}

uint16_t getAddrCS2(uint16_t loco, uint8_t prot) {
  switch (prot) {
    case LOK_MFX:
      loco = (locoData[myLocoData].myLocoID & 0x3FFF) | CS2_MFX;
      break;
    case LOK_MM:
      loco = loco | CS2_MM;
      break;
    default:
      loco = loco | CS2_DCC;
      break;
  }
  return loco;
}

uint16_t getMyAddrCS2() {
  return getAddrCS2(locoData[myLocoData].myAddr.address, locoData[myLocoData].myProtocol);
}

void infoLocomotoraCS2 (uint16_t loco) {
  byte n;
  loco = getAddrCS2(loco, locoData[myLocoData].myProtocol);
  newMsgCS2 (CS2_LOK_SPEED, 4, loco);
  sendDataCS2();

  OutData[CS2_CMD] = CS2_LOK_DIR;
  sendDataCS2();

  OutData[CS2_CMD] = CS2_LOK_FUNC;
  OutData[CS2_DLC] = 5;
  for (n = 0; n < 29; n++) {
    OutData[CS2_DATA4] = n;
    sendDataCS2();
  }
}

void changeDirectionCS2() {
  newMsgCS2 (CS2_LOK_DIR, 5, getMyAddrCS2());
  OutData[CS2_DATA4] = (locoData[myLocoData].myDir & 0x80) ? 1 : 2;      // direction (1=forward, 2=rückwärts).
  sendDataCS2();
}

byte getCurrentStepCS2() {
  return (locoData[myLocoData].mySpeed);
}

void locoOperationSpeedCS2() {
  uint16_t spd;
  if (locoData[myLocoData].mySpeed == 1) {                                // Emergency stop
    newMsgCS2 (CS2_SYS, 5, getMyAddrCS2());
    OutData[CS2_DATA4] = CS2_LOK_NOTHALT;
  }
  else {                                                                  // Normal speed
    newMsgCS2 (CS2_LOK_SPEED, 6, getMyAddrCS2());
    if (locoData[myLocoData].mySpeed > 1) {
      spd = (uint16_t)locoData[myLocoData].mySpeed * 10;                  // 0..100 -> 0..1000
      OutData[CS2_DATA4] = highByte(spd);
      OutData[CS2_DATA5] = lowByte(spd);
    }
  }
  sendDataCS2();
}

void funcOperationsCS2(byte fnc) {
  newMsgCS2 (CS2_LOK_FUNC, 6, getMyAddrCS2());
  OutData[CS2_DATA4] = fnc;
  OutData[CS2_DATA5] = (bitRead(locoData[myLocoData].myFunc.Bits, fnc)) ? 1 : 0;
  sendDataCS2();
}


void setAccessoryCS2(unsigned int FAdr, int pair, bool activate)  {
  uint16_t prot;
  if (activate) {                                                         // Switching with default timeout / system timeout of the track format processor
    prot =  CS2_DCC_ACC;
    if (bitRead(optionsCS2, CS2_OPT_ACC) && (FAdr < 321))                 // MM + DCC: MM: 1-320, DCC: 321-2048
      prot =  CS2_MM_ACC;
    newMsgCS2 (CS2_ACC, 6, (FAdr - 1) | prot);
    OutData[CS2_DATA4] = (pair > 0) ? 1 : 0;                              // DCC: Only position 0 (off) or 1 (on) is possible.
    OutData[CS2_DATA5] = 1;                                               // Current 0 - 31 (dimming function, depending on protocol support) 0: Off. 1: On.
    sendDataCS2();
  }
}


bool isMyLocoCS2() {
  uint16_t loco;
  if ((packetBuffer[CS2_DATA0] > 0) || (packetBuffer[CS2_DATA1] > 0))
    return false;
  loco = getMyAddrCS2();
  return ((packetBuffer[CS2_DATA2] == highByte(loco)) && (packetBuffer[CS2_DATA3] == lowByte(loco)));
}

void setTimeCS2(byte hh, byte mm, byte rate) {                            // undocumented
  clockHour = hh;
  clockMin = mm;
  clockRate = rate;
  newMsgCS2 (CS2_SYS, 8, 0);
  OutData[CS2_DATA4] = CS2_SYS_ZEIT;
  OutData[CS2_DATA5] = clockHour;
  OutData[CS2_DATA6] = clockMin;
  OutData[CS2_DATA7] = clockRate;
  sendDataCS2();
}

void readCVCS2 (unsigned int adr, byte stepPrg) {
  if ((!modeProg) && (locoData[myLocoData].myProtocol == LOK_DCC)) {      // Read only in Direct mode DCC
    newMsgCS2 (CS2_READ_CFG, 7, getMyAddrCS2());
    OutData[CS2_DATA4] = (highByte(adr) & 0x03);
    OutData[CS2_DATA5] = lowByte(adr);
    OutData[CS2_DATA6] = 0x01;                                            // bytes to be read
    sendDataCS2();
  }
  progStepCV = stepPrg;
}


void writeCVCS2 (unsigned int adr, unsigned int data, byte stepPrg) {
  uint16_t loco;
  loco = getMyAddrCS2();
  if ((!modeProg) && (locoData[myLocoData].myProtocol == LOK_MM))         // MM Lok Programmiergleis: Lok 80
    loco = (80 | CS2_MM);
  newMsgCS2 (CS2_WRITE_CFG, 8, loco);
  OutData[CS2_DATA4] = (highByte(adr) & 0x03);
  OutData[CS2_DATA5] = lowByte(adr);
  OutData[CS2_DATA6] = data;
  OutData[CS2_DATA7] = modeProg ? 0x80 : 0x00;                            // Bit 7: 1:Hauptgleis 0: Programmiergleis, Bit 6: Multibyte, Bit6.5: 00: Direct, 01: Reg 10: Bit, 11: Reserved
  sendDataCS2();
  progStepCV = stepPrg;
}

void queryFeedbackCS2(uint16_t fbk) {
  newMsgCS2 (CS2_S88_EVENT, 4, fbk);
  sendDataCS2();
}

/*
  void getLokNameCS2 (uint16_t ctr, uint16_t cnt) {                         // get 'loknamen' stream, cnt: MS2 search 2 locomotives at time, ctr: 0... numloks .wert
  char folgeInfo[9];
  int lng, n;
  newMsgCS2 (CS2_CFG_DATA, 8, 0);
  OutData[CS2_DATA0] = 'l';
  OutData[CS2_DATA1] = 'o';
  OutData[CS2_DATA2] = 'k';
  if (ms2New) {
    OutData[CS2_DATA3] = 'l';                                             // MS2 v3.x
    OutData[CS2_DATA4] = 'i';
    OutData[CS2_DATA5] = 's';
    OutData[CS2_DATA6] = 't';
    OutData[CS2_DATA7] = 'e';
  }
  else {
    OutData[CS2_DATA3] = 'n';                                             // MS2 v2.x
    OutData[CS2_DATA4] = 'a';
    OutData[CS2_DATA5] = 'm';
    OutData[CS2_DATA6] = 'e';
    OutData[CS2_DATA7] = 'n';
  }
  sendDataCS2();
  newMsgCS2 (CS2_CFG_DATA, 8, 0);
  snprintf(folgeInfo, 9, "%d %d", ctr, cnt);
  lng = strlen(folgeInfo);
  for (n = 0; n < lng; n++)
    OutData[CS2_DATA0 + n] = folgeInfo[n];
  sendDataCS2();
  }
*/

void getLokinfoCS2() {                                                    // get 'lokinfo' stream for current edit loco name
  int lng, i, n;
  lng = strlen (locoEditName);
  if (lng > 0) {
    newMsgCS2 (CS2_CFG_DATA, 8, 0);
    OutData[CS2_DATA0] = 'l';
    OutData[CS2_DATA1] = 'o';
    OutData[CS2_DATA2] = 'k';
    OutData[CS2_DATA3] = 'i';
    OutData[CS2_DATA4] = 'n';
    OutData[CS2_DATA5] = 'f';
    OutData[CS2_DATA6] = 'o';
    sendDataCS2();
    newMsgCS2 (CS2_CFG_DATA, 8, 0);
    n = (lng > 8) ? 8 : lng;
    for (i = 0; i < n; i++)
      OutData[CS2_DATA0 + i] = locoEditName[i];
    sendDataCS2();
    newMsgCS2 (CS2_CFG_DATA, 8, 0);
    if (lng > 8) {
      for (i = 0; i < (lng - 8); i++)
        OutData[CS2_DATA0 + i] = locoEditName[i + 8];
    }
    sendDataCS2();
  }
}

void endLokinfoCS2() {
  lokinfoState = LOKINFO_IDLE;                                            // all data stream received or aborted
  buttonData[BUT_CS2_UID].backgnd = (scrProt == LOK_MFX) ? COLOR_CREAM : COLOR_DARKGREY;
  newEvent(OBJ_BUTTON, BUT_CS2_UID, EVNT_DRAW);
  snprintf(locoEditProt, NAME_LNG + 1, "%s", locoNameProt[scrProt]);
  newEvent(OBJ_TXT, TXT_EDIT_PROT, EVNT_DRAW);
  if (!uidFound)
    alertWindow(ERR_NAME);
}

void getDataStream(byte *buf) {                                           // get a complete line of data stream
  char chr;
  uint8_t n;
  for (n = 0; n < 8; n++) {
    chr = (char)buf[CS2_DATA0 + n];
    if ((chr == 0x0A) || (chr == '\0')) {
      lineStreamCS2[streamPosCS2] = '\0';
      parseLineStream();
      streamPosCS2 = 0;
      n = 8;
    }
    else
      lineStreamCS2[streamPosCS2++] = chr;
  }
}


void parseLineStream() {                                                  // find UID in data stream
  uint16_t uid;
  //DEBUG_MSG("Stream line: %s", lineStreamCS2)
  uid = 0;
  if (!strncmp(lineStreamCS2, " .uid=0x", 8)) {
    uidHEX(lineStreamCS2 + 8, &uid);
    uidFound = true;
    if ((uid & 0xC000) == 0x4000) {                                       // check if UID is of a MFX locomotive
      snprintf (locoEditID, UID_LNG + 1, "%d", uid);
      newEvent(OBJ_TXT, TXT_EDIT_IMAGE, EVNT_DRAW);
      lpicData[LPIC_LOK_EDIT].id = uid;
      tft.fillRect(lpicData[LPIC_LOK_EDIT].x, lpicData[LPIC_LOK_EDIT].y, 190, 40, COLOR_BACKGROUND);
      newEvent(OBJ_LPIC, LPIC_LOK_EDIT, EVNT_DRAW);
    }
  }
  if (!strncmp(lineStreamCS2, " .typ=mfx", 9)) {                          // show the current protocol for this loco name
    scrProt = LOK_MFX;
  }
  if (!strncmp(lineStreamCS2, " .typ=dcc", 9)) {
    scrProt = LOK_DCC;
    endLokinfoCS2();                                                      // exit if not an MFX loco
  }
  if (!strncmp(lineStreamCS2, " .typ=mm", 8)) {
    scrProt = LOK_MM;
    endLokinfoCS2();                                                      // exit if not an MFX loco
  }
}


void uidHEX (char *buf, uint16_t *value) {                                // convert hex value
  uint16_t val, n;
  char c;
  val = 0;
  n = 0;
  while (buf[n]) {
    c = buf[n++];
    val *= 16;
    if (isDigit(c)) {
      val += (c - '0');
    }
    else {
      c &= 0xDF;
      val += ((c - 'A') + 10);
    }
  }
  *value = val;
}




////////////////////////////////////////////////////////////
// ***** CS2 DECODE *****
////////////////////////////////////////////////////////////


void DecodeCS2() {
  uint8_t cmd;
  uint16_t value;

  // P = priority, C = command, H = hash, R = response, L = length)
  // Byte 1: P|P|P|P|_|_|_|C    Byte 2: C|C|C|C|C|C|C|R     Byte 3: H|H|H|H|H|H|H|H   Byte 4: H|H|H|H|H|H|H|H    Byte 5: _|_|_|_|L|L|L|L     Byte 6-13: payload

  DEBUG_MSG("RX C:%0X R:%d  H:%0X%0X DLC:%d", ((packetBuffer[CS2_PRIO] << 7) | (packetBuffer[CS2_CMD] >> 1)), (packetBuffer[CS2_CMD] & 0x01), packetBuffer[CS2_HASH_H], packetBuffer[CS2_HASH_L], packetBuffer[CS2_DLC])
  DEBUG_MSG("PAYLOAD: %0X %0X %0X %0X %0X %0X %0X %0X", packetBuffer[CS2_DATA0], packetBuffer[CS2_DATA1], packetBuffer[CS2_DATA2], packetBuffer[CS2_DATA3], packetBuffer[CS2_DATA4], packetBuffer[CS2_DATA5], packetBuffer[CS2_DATA6], packetBuffer[CS2_DATA7])

  if (!bitRead(packetBuffer[CS2_PRIO], 0)) {
    if (bitRead(packetBuffer[CS2_CMD], 0)) {                              // parse only answers
      cmd = packetBuffer[CS2_CMD] & 0xFE;
      switch (cmd) {
        case CS2_SYS:
          switch (packetBuffer[CS2_DLC]) {
            case 5:
              switch (packetBuffer[CS2_DATA4]) {
                case CS2_SYS_HALT:
                  locoData[myLocoData].mySpeed = 0;
                  if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
                    updateSpeedHID();
                case CS2_SYS_STOP:
                  csStatus = 0;
                  showErrorCS2(packetBuffer[CS2_DATA4]);
                  break;
                case CS2_SYS_GO:
                  csStatus = 1;
                  showNormalOpsCS2();
                  break;
                case CS2_LOK_NOTHALT:
                  if (isMyLocoCS2())  {
                    locoData[myLocoData].mySpeed = 0;
                    if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
                      updateSpeedHID();
                  }
                  break;
              }
              break;
            case 8:
              switch (packetBuffer[CS2_DATA4]) {
                case CS2_SYS_ZEIT:                                        // undocumented. SRSEII
                  clockHour = packetBuffer[CS2_DATA5];
                  clockMin =  packetBuffer[CS2_DATA6];
                  clockRate = packetBuffer[CS2_DATA7];
                  clockTimer = millis();
                  DEBUG_MSG("CLOCK %d:%d R:%d", clockHour, clockMin, clockRate);
                  updateFastClock();
                  break;
              }
              break;
          }
          break;
        case CS2_LOK_SPEED:
          if ((packetBuffer[CS2_DLC] == 6) && isMyLocoCS2())  {
            value = (packetBuffer[CS2_DATA4] << 8) | packetBuffer[CS2_DATA5];
            locoData[myLocoData].mySpeed = value / 10;
            if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
              updateSpeedHID();
          }
          break;
        case CS2_LOK_DIR:
          if ((packetBuffer[CS2_DLC] == 5) && isMyLocoCS2())  {
            switch (packetBuffer[CS2_DATA4]) {
              case 1:
                locoData[myLocoData].myDir = 0x80;                        // forward
                break;
              case 2:
                locoData[myLocoData].myDir = 0x00;                        // backwards
                break;
            }
            if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
              updateSpeedDir();
          }
          break;
        case CS2_LOK_FUNC:
          if ((packetBuffer[CS2_DATA4] < 29) && isMyLocoCS2())  {
            if ((packetBuffer[CS2_DLC] == 6) || (packetBuffer[CS2_DLC] == 8)) {
              if (packetBuffer[CS2_DATA5] > 0)
                bitSet(locoData[myLocoData].myFunc.Bits, packetBuffer[CS2_DATA4]);
              else
                bitClear(locoData[myLocoData].myFunc.Bits, packetBuffer[CS2_DATA4]);
              updateFuncState(isWindow(WIN_THROTTLE));
            }
          }
          break;
        case CS2_PING:
          // CAN Device Type:
          // 0000 Gleis Format Prozessor 60213,60214 / Booster 60173, 60174 (UID: 0x42xxxxxx)
          // 0010 Gleisbox 60112 und 60113, 60116
          // 0011 Gleisbox
          // 0020 Connect 6021 Art-Nr.60128
          // 0030 MS2 60653, Txxxxx
          // 0031 MS2
          // 0032 MS2
          // 0033 MS2 60657
          // 0034 MS2
          // 0040 LinkS88 (UID: 0x533xxxxx) / SRSEII S88 gateway
          // 0050 CS2/3-GFP
          // EEEE Software App
          // FFE0 Wireless Devices
          // FFF0 CS2 (Slave)
          // FFFF CS2/3-GUI (Master)

          if ((packetBuffer[CS2_DATA7] & 0xF0) == 0x30)
            ms2New = (packetBuffer[CS2_DATA4] > 2) ? true : false;
          DEBUG_MSG("UID: %0X%0X%0X%0X v%d.%d Type: %0X%0X", packetBuffer[CS2_DATA0], packetBuffer[CS2_DATA1], packetBuffer[CS2_DATA2], packetBuffer[CS2_DATA3], packetBuffer[CS2_DATA4], packetBuffer[CS2_DATA5], packetBuffer[CS2_DATA6], packetBuffer[CS2_DATA7])
          break;
        case CS2_READ_CFG:
          if (progStepCV != PRG_IDLE) {
            switch (packetBuffer[CS2_DLC]) {
              case 7:                                                     // CV read ok
                CVdata = packetBuffer[CS2_DATA6];
                endProg();
                break;
              case 6:                                                     // CV read error
                CVdata = 0x0600;
                endProg();
                break;
            }
          }
          break;
        case CS2_WRITE_CFG:
          if (packetBuffer[CS2_DLC] == 8) {
            CVdata = (packetBuffer[CS2_DATA7] & 0xC0) ? packetBuffer[CS2_DATA6] : 0x0600;
            endProg();
          }
          break;
        case CS2_ACC:
          value = (packetBuffer[CS2_DATA2] << 8 | packetBuffer[CS2_DATA3]) & 0x07FF;
          accessoryChange(value + 1, packetBuffer[CS2_DATA4] & 0x01);
          break;
        case CS2_S88_POLLING:
          DEBUG_MSG("Module: %d Contact: 0x%02X 0x%02X", packetBuffer[CS2_DATA4], packetBuffer[CS2_DATA5], packetBuffer[CS2_DATA6])
          value = packetBuffer[CS2_DATA4] << 4;                                             // S88 module
          for (byte n = 0; n < MAX_AUTO_SEQ; n++) {
            if ((automation[n].opcode == 'Z') || (automation[n].opcode == 'z')) {
              if (((automation[n].param - 1) & 0xFFF0) == value) {
                value = (packetBuffer[CS2_DATA5] << 8) | packetBuffer[CS2_DATA6];           // S88 data
                automation[n].value = bitRead(value, automation[n].param & 0x000F);
              }
            }
          }
          break;
        case CS2_S88_EVENT:
          value = (packetBuffer[CS2_DATA2] << 8 | packetBuffer[CS2_DATA3]);                 // S88 addr
          DEBUG_MSG("Contact: %d State: %d", value, packetBuffer[CS2_DATA5])
          for (byte n = 0; n < MAX_AUTO_SEQ; n++) {
            if ((automation[n].opcode == 'Z') || (automation[n].opcode == 'z')) {
              if (automation[n].param == value)
                automation[n].value = packetBuffer[CS2_DATA5];
            }
          }
          break;
        default:
          DEBUG_MSG("%c%c%c%c%c%c%c%c", (char)packetBuffer[CS2_DATA0], (char)packetBuffer[CS2_DATA1], (char)packetBuffer[CS2_DATA2], (char)packetBuffer[CS2_DATA3], (char)packetBuffer[CS2_DATA4], (char)packetBuffer[CS2_DATA5], (char)packetBuffer[CS2_DATA6], (char)packetBuffer[CS2_DATA7])
          break;
      }
    }
    else {                                                                // command from CS2
      switch (packetBuffer[CS2_CMD]) {
        case CS2_CFG_STREAM:                                              // Config Data Stream
          switch (lokinfoState) {
            case LOKINFO_START:
              if (packetBuffer[CS2_DLC] == 6) {                           // First packet with data length and CRC
                lokinfoTotal = (packetBuffer[CS2_DATA0] << 24) | (packetBuffer[CS2_DATA1] << 16) | (packetBuffer[CS2_DATA2] << 8) | packetBuffer[CS2_DATA3];
                lokinfoLng = 0;
                streamPosCS2 = 0;
                lokinfoState = LOKINFO_DATA;
                uidFound = false;
                isMFX = false;
                //DEBUG_MSG("Stream length: %d", lokinfoTotal)
              }
              break;
            case LOKINFO_DATA:
              if (packetBuffer[CS2_DLC] == 8) {                           // data packet always has 8 bytes
                getDataStream(packetBuffer);
                lokinfoLng += 8;
                if (lokinfoLng >= lokinfoTotal)
                  endLokinfoCS2();
                //DEBUG_MSG("Stream bytes: %d", lokinfoLng)
              }
              else
                endLokinfoCS2();                                          // all other data lengths aborts reception
              break;
          }
          DEBUG_MSG("%c%c%c%c%c%c%c%c", (char)packetBuffer[CS2_DATA0], (char)packetBuffer[CS2_DATA1], (char)packetBuffer[CS2_DATA2], (char)packetBuffer[CS2_DATA3], (char)packetBuffer[CS2_DATA4], (char)packetBuffer[CS2_DATA5], (char)packetBuffer[CS2_DATA6], (char)packetBuffer[CS2_DATA7])
          break;
        case CS2_PING:                                                    // PING
          /*
            newMsgCS2 (CS2_PING | 0x01, 8, 0x464D);                         // not sure if I have to answer
            OutData[CS2_DATA4] = 0x00;
            OutData[CS2_DATA5] = 0x0C;
            OutData[CS2_DATA6] = 0xFF;
            OutData[CS2_DATA7] = 0xE0;
            sendDataCS2();
          */
          break;
      }
    }
  }
}

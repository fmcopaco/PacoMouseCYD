/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/

      This software and associated files are a DIY project that is not intended for commercial use.
      This software uses libraries with different licenses, follow all their different terms included.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.

      Sources are only provided for building and uploading to the device.
      You are not allowed to modify the source code or fork/publish this project.
      Commercial use is forbidden.
*/


////////////////////////////////////////////////////////////
// ***** Z21 SOPORTE *****
////////////////////////////////////////////////////////////


void readCVZ21 (unsigned int adr, byte stepPrg) {
  if (!modeProg) {
    askZ21begin (LAN_X_Header);
    askZ21data (0x23);
    askZ21data (0x11);
    adr--;
    askZ21data ((adr >> 8) & 0xFF);
    askZ21data (adr & 0xFF);
    askZ21xor ();
    sendUDP (0x09);
    waitResultCV = true;
    lastCV = lowByte(adr);
    progStepCV = stepPrg;
    DEBUG_MSG("Read CV %d", adr + 1);
  }
}


void writeCVZ21 (unsigned int adr, unsigned int data, byte stepPrg) {
  byte Adr_MSB;
  if (modeProg) {
    askZ21begin (LAN_X_Header);
    askZ21data (0xE6);
    askZ21data (0x30);
    Adr_MSB = locoData[myLocoData].myAddr.adr[1] & 0x3F;
    if (locoData[myLocoData].myAddr.address & 0x3F80)
      Adr_MSB |= 0xC0;
    askZ21data (Adr_MSB);
    askZ21data (locoData[myLocoData].myAddr.adr[0]);
    adr--;
    askZ21data (0xEC | ((adr >> 8) & 0x03));
    askZ21data (adr & 0xFF);
    askZ21data (data);
    askZ21xor ();
    sendUDP (0x0C);
  }
  else {
    askZ21begin (LAN_X_Header);
    askZ21data (0x24);
    askZ21data (0x12);
    adr--;
    askZ21data ((adr >> 8) & 0xFF);
    askZ21data (adr & 0xFF);
    askZ21data (data);
    askZ21xor ();
    sendUDP (0x0A);
    waitResultCV = true;
    lastCV = lowByte(adr);

  }
  progStepCV = stepPrg;
  DEBUG_MSG("Write CV%d = %d", adr + 1, data);
}


void showErrorZ21() {                                        // muestra pantalla de error
  if (csStatus & csTrackVoltageOff) {
    iconData[ICON_POWER].color = COLOR_RED;
    setTimer (TMR_POWER, 5, TMR_PERIODIC);                   // Flash power icon
    setColorRGB(LED_RGB_RED);
    if ((isWindow(WIN_ALERT)) && (errType == ERR_STOP))
      closeWindow(WIN_ALERT);
    if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
      newEvent(OBJ_ICON, ICON_POWER, EVNT_DRAW);
    if (isWindow(WIN_STA_PLAY)) {
      fncData[FNC_STA_RAYO].state = true;
      newEvent(OBJ_FNC, FNC_STA_RAYO, EVNT_DRAW);
    }
  }
  else {
    if (csStatus & csEmergencyStop) {
      setColorRGB(LED_RGB_BLUE);
      if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
        alertWindow(ERR_STOP);
    }
  }
  if (csStatus & csProgrammingModeActive) {
    setColorRGB(LED_RGB_BLUE);
    if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
      alertWindow(ERR_SERV);
  }
}


void showNormalOpsZ21() {
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


void setTimeZ21(byte hh, byte mm, byte rate) {
  clockHour = hh;
  clockMin = mm;
  clockRate = rate;
  askZ21begin (LAN_FAST_CLOCK_CONTROL);                       // set clock
  if (rate > 0) {
    askZ21data (0x24);
    askZ21data (0x2B);
    askZ21data (hh);
    askZ21data (mm);
    askZ21data (rate);
    askZ21xor ();
    sendUDP (0x0A);
    askZ21begin (LAN_FAST_CLOCK_CONTROL);
    askZ21data (0x21);                                        // start clock
    askZ21data (0x2C);
    askZ21xor ();
    sendUDP (0x07);
  }
  else {
    askZ21data (0x21);                                        // recommended for rate=0. stop clock
    askZ21data (0x2D);
    askZ21xor ();
    sendUDP (0x07);
  }
}


////////////////////////////////////////////////////////////
// ***** Z21 DECODE *****
////////////////////////////////////////////////////////////


void processZ21() {
  int len, packetSize;

  packetSize = Udp.parsePacket();                             // z21 UDP packet
  if (packetSize) {
    len = Udp.read(packetBuffer, packetSize);                 // read the packet into packetBufffer
    ReceiveZ21 (len, packetBuffer);                           // decode received packet
  }
  delay(0);
  if (millis() - infoTimer > 1000UL) {                        // Cada segundo
    infoTimer = millis();
    pingTimer++;
    if (pingTimer >= Z21_PING_INTERVAL) {
      pingTimer = 0;
      if (!(csStatus & csProgrammingModeActive))
        getStatusZ21();
    }
  }
  if (progFinished) {                                         // fin de lectura/programacion CV
    progFinished = false;
    endProg();
  }
  delay(0);
}

// -------------------------------------------------------------------------------------

void setBroadcastFlags (unsigned long bFlags) {
  askZ21begin (LAN_SET_BROADCASTFLAGS);
  askZ21data (bFlags & 0xFF);
  askZ21data ((bFlags >> 8) & 0xFF);
  askZ21data ((bFlags >> 16) & 0xFF);
  askZ21data ((bFlags >> 24) & 0xFF);
  sendUDP (0x08);
}

void resumeOperationsZ21 () {                                 // LAN_X_SET_TRACK_POWER_ON
  askZ21begin (LAN_X_Header);
  askZ21data (0x21);
  askZ21data (0x81);
  askZ21xor ();
  sendUDP (0x07);
}


void emergencyOffZ21() {                                      // LAN_X_SET_TRACK_POWER_OFF
  askZ21begin (LAN_X_Header);
  askZ21data (0x21);
  askZ21data (0x80);
  askZ21xor ();
  sendUDP (0x07);
}


void getStatusZ21 () {
  askZ21begin (LAN_X_Header);
  askZ21data (0x21);
  askZ21data (0x24);
  askZ21xor ();
  sendUDP (0x07);
}

void getSerialNumber () {
  askZ21begin (LAN_GET_SERIAL_NUMBER);
  sendUDP (0x04);
}

void infoLocomotoraZ21 (unsigned int Adr) {
  byte Adr_MSB;
  askZ21begin (LAN_X_Header);
  askZ21data (0xE3);
  askZ21data (0xF0);
  Adr_MSB = (Adr >> 8) & 0x3F;
  if (Adr & 0x3F80)
    Adr_MSB |= 0xC0;
  askZ21data (Adr_MSB);
  askZ21data (Adr & 0xFF);
  askZ21xor ();
  sendUDP (0x09);
}

void locoOperationSpeedZ21() {
  byte Adr_MSB;
  DEBUG_MSG("Loco Operations")
  askZ21begin (LAN_X_Header);
  askZ21data (0xE4);
  if (bitRead(locoData[myLocoData].mySteps, 2)) {             // 128 steps
    askZ21data (0x13);
  }
  else {
    if (bitRead(locoData[myLocoData].mySteps, 1)) {           // 28 steps
      askZ21data (0x12);
    }
    else {
      askZ21data (0x10);                                      // 14 steps
    }
  }
  Adr_MSB = locoData[myLocoData].myAddr.adr[1] & 0x3F;
  if (locoData[myLocoData].myAddr.address & 0x3F80)
    Adr_MSB |= 0xC0;
  askZ21data (Adr_MSB);
  askZ21data (locoData[myLocoData].myAddr.adr[0]);
  askZ21data (locoData[myLocoData].mySpeed | locoData[myLocoData].myDir);
  askZ21xor ();
  sendUDP (0x0A);
  bitClear(locoData[myLocoData].mySteps, 3);                  // currently operated by me
  updateSpeedDir();
}


byte getCurrentStepZ21() {
  byte currStep;
  DEBUG_MSG("Get Steps: %02X - Speed: %02X",  locoData[myLocoData].mySteps, locoData[myLocoData].mySpeed);
  if (bitRead(locoData[myLocoData].mySteps, 2)) {             // 128 steps -> 0..126
    if (locoData[myLocoData].mySpeed > 1)
      return (locoData[myLocoData].mySpeed - 1);
  }
  else {
    if (bitRead(locoData[myLocoData].mySteps, 1)) {           // 28 steps -> 0..28    '---04321' -> '---43210'
      currStep = (locoData[myLocoData].mySpeed << 1) & 0x1F;
      bitWrite(currStep, 0, bitRead(locoData[myLocoData].mySpeed, 4));
      if (currStep > 3)
        return (currStep - 3);
    }
    else {                                                    // 14 steps -> 0..14
      if (locoData[myLocoData].mySpeed > 1)
        return (locoData[myLocoData].mySpeed - 1);
    }
  }
  return (0);
}


void funcOperationsZ21 (byte fnc) {
  byte Adr_MSB;
  askZ21begin (LAN_X_Header);
  askZ21data (0xE4);
  askZ21data (0xF8);
  Adr_MSB = locoData[myLocoData].myAddr.adr[1] & 0x3F;
  if (locoData[myLocoData].myAddr.address & 0x3F80)
    Adr_MSB |= 0xC0;
  askZ21data (Adr_MSB);
  askZ21data (locoData[myLocoData].myAddr.adr[0]);
  if (bitRead(locoData[myLocoData].myFunc.Bits, fnc))
    askZ21data (fnc | 0x40);
  else
    askZ21data (fnc);
  askZ21xor ();
  sendUDP (0x0A);
  bitClear(locoData[myLocoData].mySteps, 3);                  // currently operated by me
}


void infoDesvio (unsigned int FAdr) {
  FAdr--;
  askZ21begin (LAN_X_Header);
  askZ21data (0x43);
  askZ21data ((FAdr >> 8) & 0xFF);
  askZ21data (FAdr & 0xFF);
  askZ21xor ();
  sendUDP (0x08);
}


void setAccessoryZ21 (unsigned int FAdr, int pair, bool active) {
  byte db2;
  FAdr--;
  askZ21begin (LAN_X_Header);
  askZ21data (0x53);
  askZ21data ((FAdr >> 8) & 0xFF);
  askZ21data (FAdr & 0xFF);
  db2 = active ? 0x88 : 0x80;
  if (pair > 0)
    db2 |= 0x01;
  askZ21data (db2);                                           // '10Q0A00P'
  askZ21xor ();
  sendUDP (0x09);
}


void getFeedbackInfo (byte group) {
  askZ21begin (LAN_RMBUS_GETDATA);
  askZ21data (group);
  sendUDP (0x05);
}


void ReceiveZ21 (int len, byte * packet) {                    // get UDP packet, maybe more than one!!
  int  DataLen, isPacket;
#ifdef DEBUG
  Serial.print("\nRX Length: ");
  Serial.println (len);
  for (int i = 0; i < len; i++) {
    Serial.print(packet[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
#endif
  isPacket = 1;
  while (isPacket) {
    DataLen = (packet[DATA_LENH] << 8) + packet[DATA_LENL];
    DecodeZ21 (DataLen, packet);
    if (DataLen >= len) {
      isPacket = 0;
    }
    else {
      packet = packet + DataLen;
      len = len - DataLen;
    }
    delay(0);
  }
}


void DecodeZ21 (int len, byte * packet) {                     // decode z21 UDP packets
  int Header, DataLen;
  unsigned int FAdr;
  byte group;
  bool state;

  Header = (packet[DATA_HEADERH] << 8) + packet[DATA_HEADERL];
  switch (Header) {
    case  LAN_GET_SERIAL_NUMBER:
      break;
    case LAN_GET_CODE:                                        // FW 1.28
      break;
    case LAN_GET_HWINFO:
      break;
    case LAN_GET_BROADCASTFLAGS:
      break;
    case LAN_GET_LOCOMODE:
      break;
    case LAN_GET_TURNOUTMODE:
      break;
    case LAN_RMBUS_DATACHANGED:
      /*
        #ifdef USE_AUTOMATION
        for (byte n = 0; n < MAX_AUTO_SEQ; n++) {
          if ((automation[n].opcode & OPC_AUTO_MASK) == OPC_AUTO_FBK) {
            if ((packet[4] == 0x01) && (automation[n].param > 9))
              automation[n].value = packet[automation[n].param - 5];
            if ((packet[4] == 0x00) && (automation[n].param < 10))
              automation[n].value = packet[automation[n].param + 5];
            DEBUG_MSG("RBUS %d", automation[n].value)
          }
        }
        #endif
      */
      break;
    case LAN_SYSTEMSTATE_DATACHANGED:
      csStatus = packet[16] & (csEmergencyStop | csTrackVoltageOff | csProgrammingModeActive);    // CentralState
      if (packet[16] & csShortCircuit)
        csStatus |= csTrackVoltageOff;
      break;
    case LAN_RAILCOM_DATACHANGED:
      break;

    case LAN_LOCONET_Z21_TX:                                  // a message has been written to the LocoNet bus by the Z21.
    case LAN_LOCONET_Z21_RX:                                  // a message has been received by the Z21 from the LocoNet bus.
    case LAN_LOCONET_FROM_LAN:                                // another LAN client has written a message to the LocoNet bus via the Z21.
      switch (packet[4]) {
        case 0x83:
          csStatus = csNormalOps;                             // OPC_GPON
          showNormalOpsZ21();
          break;
        case 0x82:
          csStatus |= csTrackVoltageOff;                      // OPC_GPOFF
          showErrorZ21();
          break;
      }
      break;
    case LAN_LOCONET_DETECTOR:
      break;
    case LAN_FAST_CLOCK_DATA:                                 // fast clock data FW 1.43
      if (packet[8] & 0x80) {                                 // Stop flag
        clockRate = 0;
      }
      else {
        clockHour = packet[6] & 0x1F;
        clockMin = packet[7] & 0x3F;
        clockRate = packet[9] & 0x3F;
        updateFastClock();
      }
      DEBUG_MSG("Clock: %d:%d %d", clockHour, clockMin, clockRate);
      break;

    case LAN_X_Header:
      switch (packet[XHEADER]) {
        case 0x43:                                            // LAN_X_TURNOUT_INFO
          FAdr = (packet[DB0] << 8) + packet[DB1] + 1;
          state = ((packet[DB2] & 0x03) == 0x02) ? true : false;
          accessoryChange(FAdr, state);
          break;
        case 0x61:
          switch (packet[DB0]) {
            case 0x01:                                        // LAN_X_BC_TRACK_POWER_ON
              csStatus = csNormalOps;
              showNormalOpsZ21();
              break;
            case 0x08:                                        // LAN_X_BC_TRACK_SHORT_CIRCUIT
              csStatus |= csShortCircuit;
            case 0x00:                                        // LAN_X_BC_TRACK_POWER_OFF
              csStatus |= csTrackVoltageOff;
              showErrorZ21();
              break;
            case 0x02:                                        // LAN_X_BC_PROGRAMMING_MODE
              csStatus |= csProgrammingModeActive;
              if (!waitResultCV)
                showErrorZ21();
              break;
            case 0x12:                                        // LAN_X_CV_NACK_SC
            case 0x13:                                        // LAN_X_CV_NACK
              CVdata = 0x0600;
              waitResultCV = false;
              progFinished = true;
              break;
            case 0x82:                                        // LAN_X_UNKNOWN_COMMAND
              break;
          }
          break;
        case 0x62:
          switch (packet[DB0]) {
            case 0x22:                                        // LAN_X_STATUS_CHANGED
              csStatus = packet[DB1] & (csEmergencyStop | csTrackVoltageOff | csProgrammingModeActive);
              if (packet[DB1] & csShortCircuit)
                csStatus |= csTrackVoltageOff;
              if (csStatus == csNormalOps)
                showNormalOpsZ21();
              else
                showErrorZ21();
              break;
          }
          break;
        case 0x64:
          if (packet[DB0] == 0x14) {                            // LAN_X_CV_RESULT
            if (packet[DB2] == lastCV) {
              lastCV ^= 0x55;
              CVdata = packet[DB3];
              waitResultCV = false;
              progFinished = true;
            }
          }
          break;
        case 0x81:
          if (packet[DB0] == 0) {                               // LAN_X_BC_STOPPED
            csStatus |= csEmergencyStop;
            showErrorZ21();
          }
          break;
        case 0xEF:                                              // LAN_X_LOCO_INFO
          DEBUG_MSG("RX: Loco data")
          FAdr = ((packet[DB0] << 8) + packet[DB1]) & 0x3FFF;
          if (FAdr == locoData[myLocoData].myAddr.address) {
            locoData[myLocoData].mySteps = packet[DB2];                              // '0000BFFF'
            locoData[myLocoData].myDir = packet[DB3] & 0x80;                         // 'RVVVVVVV'
            locoData[myLocoData].mySpeed = packet[DB3] & 0x7F;
            locoData[myLocoData].myFunc.Bits &= 0xE0000000;                          // '000FFFFF','FFFFFFFF'
            locoData[myLocoData].myFunc.xFunc[0] |= ((packet[DB4] & 0x0F) << 1);
            bitWrite(locoData[myLocoData].myFunc.xFunc[0], 0, bitRead(packet[DB4], 4));
            locoData[myLocoData].myFunc.Bits |= (unsigned long)(packet[DB5] << 5);
            locoData[myLocoData].myFunc.Bits |= (unsigned long)(packet[DB6] << 13);
            locoData[myLocoData].myFunc.Bits |= (unsigned long)(packet[DB7] << 21);
            updateFuncState(isWindow(WIN_THROTTLE));
            if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
              updateSpeedHID();                               // set encoder
          }
          break;
      }
      break;

    default:                                                  // Header other
      break;

  }
}


void askZ21begin (unsigned int header) {
  OutData[DATA_HEADERL] = header & 0xFF;
  OutData[DATA_HEADERH] = header >> 8;
  OutPos = XHEADER;
  OutXOR = 0;
}


void askZ21data (byte data) {
  OutData[OutPos++] = data;
  OutXOR ^= data;
}

void askZ21xor () {
  OutData[OutPos] = OutXOR;
}


void sendUDP (int len) {
  OutData[DATA_LENL] = len & 0xFF;
  OutData[DATA_LENH] = len >> 8;
  Udp.beginPacket(wifiSetting.CS_IP, z21Port);
  Udp.write(OutData, len);
  Udp.endPacket();
  delay(0);
#ifdef DEBUG
  Serial.print("TX: ");
  for (int i = 0; i < len; i++) {
    Serial.print(OutData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
#endif
}

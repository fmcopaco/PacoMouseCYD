/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/

      This software and associated files are a DIY project that is not intended for commercial use.
      This software uses libraries with different licenses, follow all their different terms included.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.

      Sources are only provided for building and uploading to the device.
      You are not allowed to modify the source code or fork/publish this project.
      Commercial use is forbidden.
*/


////////////////////////////////////////////////////////////
// ***** XPRESSNET LAN SOPORTE *****
////////////////////////////////////////////////////////////

void showErrorXnet() {                                      // muestra pantalla de error
  if (csStatus & csEmergencyOff) {
    iconData[ICON_POWER].color = COLOR_RED;
    setTimer (TMR_POWER, 5, TMR_PERIODIC);                  // Flash power icon
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
  if (csStatus & csServiceMode) {
    setColorRGB(LED_RGB_BLUE);
    if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
      alertWindow(ERR_SERV);
  }
}

void showNormalOpsXnet() {
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

uint16_t addrXnet(uint16_t adr) {
  if (adr > 99)                                             // Comprueba si es direccion larga
    adr |= 0xC000;
  return adr;
}


bool isRecentMM () {                                        // Comprueba central Multimaus reciente
  if ((xnetCS == 0x10) && (highVerMM > 0) && (lowVerMM > 0x02))
    return true;
  else
    return false;
}


////////////////////////////////////////////////////////////
// ***** XPRESSNET LAN MESSAGES *****
////////////////////////////////////////////////////////////

void getStatusXnet () {
  headerXN (0x21);                                          // Command station status request         (0x21,0x24,0x05)
  dataXN (0x24);
  sendXN();
}


void getVersionXnet () {
  headerXN (0x21);                                          // Command station software version       (0x21,0x21,0x00)
  dataXN (0x21);
  sendXN();
}

void versionMultimaus() {
  headerXN (0xF1);                                          // Multimaus software version             (0xF1,0x0A,XOR)
  dataXN (0x0A);
  sendXN();
}


void getResultsXnet() {
  headerXN (0x21);                                          // Request for Service Mode results       (0x21,0x10,0x31)
  dataXN (0x10);
  sendXN();
  setColorRGB(LED_RGB_BLUE);
}


void resumeOperationsXnet () {
  headerXN (0x21);                                          // Resume operations request              (0x21,0x81,0xA0)
  dataXN (0x81);
  sendXN();
}


void emergencyOffXnet() {
  headerXN (0x21);                                          // Stop operations request (emergency off)(0x21,0x80,0xA1)
  dataXN (0x80);
  sendXN();
}


void infoLocomotoraXnet (unsigned int loco) {               // Locomotive information request         (0xE3,0x00,ADRH,ADRL,XOR)
  uint16_t adr;
  adr = addrXnet(loco);
  headerXN (0xE3);
  dataXN (0x00);
  dataXN (highByte(adr));
  dataXN (lowByte (adr));
  sendXN();
  if ((xnetVersion > 0x35) || (xnetCS == 0x10)) {
    headerXN (0xE3);
    if (xnetCS == 0x10)
      dataXN (0xF0);                                       // Locomotive function F13..F20 info MM    (0xE3,0xF0,ADRH,ADRL,XOR)
    else
      dataXN (0x09);                                       // Locomotive function F13..F28 info v3.6  (0xE3,0x09,ADRH,ADRL,XOR)
    dataXN (highByte(adr));
    dataXN (lowByte (adr));
    sendXN();
  }
  getInfoLoco = false;
}


void locoOperationSpeedXnet() {                             // Locomotive speed and direction operations (0xE4,ID,ADRH,ADRL,SPD,XOR)
  uint16_t adr;
  adr = addrXnet(locoData[myLocoData].myAddr.address);
  headerXN (0xE4);
  if (bitRead(locoData[myLocoData].mySteps, 2)) {           // 128 steps
    dataXN (0x13);
  }
  else {
    if (bitRead(locoData[myLocoData].mySteps, 1)) {         // 28 steps
      dataXN (0x12);
    }
    else {
      dataXN (0x10);                                        // 14 steps
    }
  }
  dataXN (highByte(adr));
  dataXN (lowByte(adr));
  dataXN (locoData[myLocoData].mySpeed | locoData[myLocoData].myDir);
  sendXN();
  bitClear(locoData[myLocoData].mySteps, 3);               // currently operated by me
  updateSpeedDir();
}


void funcOperationsXnet (byte fnc) {                       // Function operation instructions        (0xE4,ID,ADRH,ADRL,GRP,XOR)
  byte grp, grpID;
  uint16_t adr;
  adr = addrXnet(locoData[myLocoData].myAddr.address);
  if (fnc > 20) {
    grpID = 0x28;                                           // F21..F28
    grp =  ((locoData[myLocoData].myFunc.xFunc[2] >> 5) & 0x07);
    grp |= (locoData[myLocoData].myFunc.xFunc[3] << 3);
  }
  else {
    if (fnc > 12) {
      if (xnetCS == 0x10)
        grpID = 0xF3;                                       // F13..F20 MM                            (0xE4,0xF3,ADH,ADL,F13F20,XOR)
      else
        grpID = 0x23;                                       // F13..F20
      grp =  ((locoData[myLocoData].myFunc.xFunc[1] >> 5) & 0x07);
      grp |= (locoData[myLocoData].myFunc.xFunc[2] << 3);
    }
    else {
      if (fnc > 8) {
        grpID = 0x22;                                       // F9..F12
        grp =  ((locoData[myLocoData].myFunc.xFunc[1] >> 1) & 0x0F);
      }
      else {
        if (fnc > 4) {
          grpID = 0x21;                                     // F5..F8
          grp =  ((locoData[myLocoData].myFunc.xFunc[0] >> 5) & 0x07);
          if (bitRead(locoData[myLocoData].myFunc.xFunc[1], 0))
            grp |= 0x08;
        }
        else {
          grpID = 0x20;                                     // F0..F4
          grp =  ((locoData[myLocoData].myFunc.xFunc[0] >> 1) & 0x0F);
          if (bitRead(locoData[myLocoData].myFunc.xFunc[0], 0))
            grp |= 0x10;
        }
      }
    }
  }
  headerXN (0xE4);
  dataXN (grpID);
  dataXN (highByte(adr));
  dataXN (lowByte(adr));
  dataXN (grp);
  sendXN();
  bitClear(locoData[myLocoData].mySteps, 3);                                     // currently operated by me
}


byte getCurrentStepXnet() {
  byte currStep;
  if (bitRead(locoData[myLocoData].mySteps, 2)) {                                // 128 steps -> 0..126
    if (locoData[myLocoData].mySpeed > 1)
      return (locoData[myLocoData].mySpeed - 1);
  }
  else {
    if (bitRead(locoData[myLocoData].mySteps, 1)) {                              // 28 steps -> 0..28    '---04321' -> '---43210'
      currStep = (locoData[myLocoData].mySpeed << 1) & 0x1F;
      bitWrite(currStep, 0, bitRead(locoData[myLocoData].mySpeed, 4));
      if (currStep > 3)
        return (currStep - 3);
    }
    else {                                                  // 14 steps -> 0..14
      if (locoData[myLocoData].mySpeed > 1)
        return (locoData[myLocoData].mySpeed - 1);
    }
  }
  return (0);
}


void setAccessoryXnet (unsigned int direccion, bool activa, byte posicion) {    // 1..1024
  byte  adr, dato;
  direccion--;                                              // 000000AAAAAAAABB
  adr = (direccion >> 2) & 0x00FF;                          // AAAAAAAA
  dato = ((direccion & 0x0003) << 1) | 0x80;                // 1000xBBx
  if (posicion > 0)
    dato |= 0x01;
  if (activa) {                                             // 1000dBBD
    dato |= 0x08;
  }
  headerXN (0x52);                                          // Accessory Decoder operation request    (0x52,AAAAAAAA,1000dBBD,XOR)
  dataXN (adr);
  dataXN (dato);
  sendXN();
}


void setTimeXnet(byte hh, byte mm, byte rate) {
  clockHour = hh;
  clockMin = mm;
  clockRate = rate;
  if (rate > 0) {
    headerXN (0x24);                                        // set clock
    dataXN (0x2B);
    dataXN (hh);
    dataXN (mm);
    dataXN (rate);
    sendXN ();
    /*
      headerXN (0x21);                                        // start clock
      dataXN (0x2C);
      sendXN (0x07);
    */
  }
  else {
    headerXN (0x21);                                        // recommended for rate=0. stop clock
    dataXN (0x2D);
    sendXN ();
  }
}

void readCVXnet (unsigned int adr, byte stepPrg) {
  if (!modeProg) {                                          // Read only in Direct mode
    if (isRecentMM()) {
      headerXN (0x23);                                      // Multimaus v1.03
      dataXN (0x15);
      adr--;
      dataXN (highByte(adr) & 0x03);
      dataXN (lowByte(adr));
      sendXN();
      lastCV = lowByte(adr) + 1;
    }
    else {
      headerXN (0x22);
      if (xnetVersion > 0x35)
        dataXN (0x18 | (highByte(adr) & 0x03));               // v3.6 & up  CV1..CV1024
      else
        dataXN (0x15);                                        // v3.0       CV1..CV256
      dataXN (lowByte(adr));
      sendXN();
      lastCV = lowByte(adr);
    }
    getResultsSM = true;
    infoTimer = millis();
    progStepCV = stepPrg;
    //DEBUG_MSG("Read CV %d", adr);
  }
}


void writeCVXnet (unsigned int adr, unsigned int data, byte stepPrg) {
  uint16_t adrLoco;
  if (modeProg) {
    headerXN (0xE6);                                        // Operations Mode Programming byte mode write request (0xE6,0x30,ADRH,ADRL,0xEC+C,CV,DATA,XOR)
    dataXN (0x30);
    adrLoco = addrXnet(locoData[myLocoData].myAddr.address);
    dataXN (highByte(adrLoco));
    dataXN (lowByte(adrLoco));
    adr--;
    dataXN (0xEC | (highByte(adr) & 0x03));
    dataXN (lowByte(adr));
    dataXN(data);
    sendXN();
  }
  else {
    if (isRecentMM()) {
      headerXN (0x24);                                      // Multimaus v1.03
      dataXN (0x16);
      adr--;
      dataXN (highByte(adr) & 0x03);
      dataXN (lowByte(adr));
      dataXN(data);
      sendXN();
      lastCV = lowByte(adr) + 1;
    }
    else {
      headerXN (0x23);
      if (xnetVersion > 0x35)
        dataXN (0x1C | (highByte(adr) & 0x03));               // v3.6 & up  CV1..CV1024
      else
        dataXN (0x16);                                        // v3.0       CV1..CV256
      dataXN (lowByte(adr));
      dataXN(data);
      sendXN();
      lastCV = lowByte(adr);
    }
    getResultsSM = true;
    infoTimer = millis();
  }
  progStepCV = stepPrg;
  //DEBUG_MSG("Write CV%d = %d", adr, data);
}


////////////////////////////////////////////////////////////
// ***** XPRESSNET LAN DECODE *****
////////////////////////////////////////////////////////////

void headerXN (byte header) {
  txBytes = HEADER;                                         // coloca header en el buffer
  txXOR = header;
  txBuffer[txBytes++] = header;
  txBuffer[FRAME1] = 0xFF;
  txBuffer[FRAME2] = 0xFE;
}


void dataXN (byte dato) {
  txBuffer[txBytes++] = dato;                               // coloca dato en el buffer
  txXOR ^= dato;
}


void sendXN () {
  bool recvAnswer;
  txBuffer[txBytes++] = txXOR;                              // coloca XOR byte en el buffer
#ifdef DEBUG
  Serial.print(F("TX: "));
  for (uint8_t x = 0; x < txBytes; x++) {
    uint8_t val = txBuffer[x];
    if (val < 16)
      Serial.print('0');
    Serial.print(val, HEX);
    Serial.print(' ');
  }
  Serial.println();
#endif
  Client.write((byte *)&txBuffer[FRAME1], txBytes);         // envia paquete xpressnet
  timeoutXnet = millis();
  recvAnswer = false;
  while ((millis() - timeoutXnet < 500) && (!recvAnswer))   // wait answer for 500ms
    recvAnswer = xnetReceive();
}


bool xnetReceive() {
  bool getAnswer;
  getAnswer = false;
  while (Client.available()) {
    rxData = Client.read();
    //DEBUG_MSG("%d-%02X", rxIndice, rxData);
    switch (rxIndice) {
      case FRAME1:
        rxBufferXN[FRAME1] = rxData;
        if (rxData == 0xFF)                                 // 0xFF... Posible inicio de paquete
          rxIndice = FRAME2;
        break;
      case FRAME2:
        rxBufferXN[FRAME2] = rxData;
        switch (rxData) {
          case 0xFF:                                        // 0xFF 0xFF... FRAME2 puede ser FRAME1 (inicio de otro paquete)
            break;
          case 0xFE:                                        // 0xFF 0xFE... Inicio paquete correcto
          case 0xFD:                                        // 0xFF 0xFD... Inicio paquete de broadcast correcto
            rxIndice = HEADER;
            rxXOR = 0;
            break;
          default:                                          // 0xFF 0xXX... No es inicio de paquete
            rxIndice = FRAME1;
            break;
        }
        break;
      default:
        rxBufferXN[rxIndice++] = rxData;
        rxXOR ^= rxData;
        if (((rxBufferXN[HEADER] & 0x0F) + 4) == rxIndice) {  // si se han recibido todos los datos indicados en el paquete
          if (rxXOR == 0) {                                   // si el paquete es correcto
            rxBytes = rxIndice;
#ifdef DEBUG
            Serial.print(F("RX: "));
            for (uint8_t x = 0; x < rxBytes; x++) {
              uint8_t val = rxBufferXN[x];
              if (val < 16)
                Serial.print('0');
              Serial.print(val, HEX);
              Serial.print(' ');
            }
            Serial.println();
#endif
            procesaXN();                                      // nuevo paquete recibido, procesarlo
            getAnswer = true;
          }
          rxIndice = FRAME1;                                  // proximo paquete
        }
        break;
    }
  }
  return getAnswer;
}

void processXnet () {                                       // procesa Xpressnet
  xnetReceive();
  if (getInfoLoco && (csStatus == csNormalOps))
    infoLocomotoraXnet(addrXnet(locoData[myLocoData].myAddr.address));
  if (millis() - infoTimer > 1000UL) {                      // Cada segundo
    infoTimer = millis();
    if (getResultsSM)                                       // Resultados de CV pendientes
      getResultsXnet();                                     // pide resultados
    else {
      if (bitRead(locoData[myLocoData].mySteps, 3))         // Loco controlada por otro mando
        getInfoLoco = true;                                 // pide info locomotora
      if (askMultimaus) {                                   // pide info Multimaus
        askMultimaus = false;
        versionMultimaus();
      }
    }
  }
  if (progFinished) {                                       // fin de lectura/programacion CV
    progFinished = false;
    endProg();
  }
  if (millis() - pingTimer > XNET_PING_INTERVAL) {          // Refresca para mantener la conexion
    pingTimer = millis();
    getStatusXnet();                                        // pide estado de la central
  }
}


void procesaXN () {
  byte n, longitud, modulo, dato;
  uint16_t adr;

  switch (rxBufferXN[HEADER]) {                             // segun el header byte
    case 0x61:
      switch (rxBufferXN[DATA1]) {
        case 0x01:                                          // Normal operation resumed                   (0x61,0x01,0x60)
          csStatus = csNormalOps;
          showNormalOpsXnet();
          break;
        case 0x08:                                          // Z21 LAN_X_BC_TRACK_SHORT_CIRCUIT           (0x61,0x08,XOR)
        case 0x00:                                          // Track power off                            (0x61,0x00,0x61)
          csStatus |= csEmergencyOff;
          showErrorXnet();
          break;
        case 0x02:                                          // Service mode entry                         (0x61,0x02,0x63)
          csStatus |= csServiceMode;
          if (!getResultsSM)                                // show 'Service Mode' if we aren't programming CV
            showErrorXnet();
          break;
        case 0x12:                                          // Programming info. "shortcircuit"           (0x61,0x12,XOR)
        case 0x13:                                          // Programming info. "Data byte not found"    (0x61,0x13,XOR)
          CVdata = 0x0600;
          getResultsSM = false;
          progFinished = true;
          break;
        case 0x81:                                          // Command station busy response              (0x61,0x81,XOR)
          break;
        case 0x1F:                                          // Programming info. "Command station busy"   (0x61,0x1F,XOR)
          getResultsSM = true;
          infoTimer = millis();
          break;
        case 0x82:                                          // Instruction not supported by command station (0x61,0x82,XOR)
          getResultsSM = false;
          if (csStatus & csServiceMode) {
            CVdata = 0x0600;
            progFinished = true;
          }
          break;
      }
      break;
    case 0x81:
      if (rxBufferXN[DATA1] == 0) {                         // Emergency Stop                             (0x81,0x00,0x81)
        csStatus |= csEmergencyStop;
        showErrorXnet();
      }
      break;
    case 0x62:
      if (rxBufferXN[DATA1] == 0x22) {                      // Command station status indication response (0x62,0x22,DATA,XOR)
        csStatus = rxBufferXN[DATA2] & (csEmergencyStop | csEmergencyOff | csServiceMode) ;
        if ((xnetCS >= 0x10) && (rxBufferXN[DATA2] & csProgrammingModeActive))   // Multimaus/Z21 Service Mode
          csStatus |= csServiceMode;
        if (csStatus == csNormalOps)
          showNormalOpsXnet();
        else
          showErrorXnet();
      }
      break;
    case 0x63:
      switch (rxBufferXN[DATA1]) {
        case 0x03:                                          // Broadcast "Modellzeit"                     (0x63,0x03,dddhhhhh,s0mmmmmm,XOR) (v4.0)
          clockHour = rxBufferXN[DATA2] & 0x1F;
          clockMin = rxBufferXN[DATA3] & 0x3F;
          clockRate = !bitRead(rxBufferXN[DATA3], 7);
          updateFastClock();
          break;
        case 0x14:                                          // Service Mode response for Direct CV mode   (0x63,0x1x,CV,DATA,XOR)
        case 0x15:
        case 0x16:
        case 0x17:
          if (rxBufferXN[DATA2] == lastCV) {                // comprobar CV (DR5000)
            lastCV ^= 0x55;
            getResultsSM = false;
            CVdata = rxBufferXN[DATA3];
            progFinished = true;
          }
          break;
        case 0x21:                                          // Command station software version           (0x63,0x21,VER,ID,XOR)
          xnetVersion = rxBufferXN[DATA2];
          xnetCS = rxBufferXN[DATA3];
          if (xnetCS == 0x10)
            askMultimaus = true;
          break;
      }
      break;
    case 0xE3:
      if (rxBufferXN[DATA1] == 0x40) {                      // Locomotive is being operated by another device response  (0xE3,0x40,ADRH,ADRL,XOR)
        adr = addrXnet(locoData[myLocoData].myAddr.address);
        if ((rxBufferXN[DATA3] == lowByte(adr)) && (rxBufferXN[DATA2] == highByte(adr))) {           // DR5000 workaround
          bitSet(locoData[myLocoData].mySteps, 3);
        }
      }
      if (rxBufferXN[DATA1] == 0x52) {                      // Locomotive function info F13..F28          (0xE3,0x52,FNC,FNC,XOR)
        locoData[myLocoData].myFunc.Bits &= 0xE0001FFF;
        locoData[myLocoData].myFunc.Bits |= ((unsigned long)rxBufferXN[DATA2] << 13);
        locoData[myLocoData].myFunc.Bits |= ((unsigned long)rxBufferXN[DATA3] << 21);
        updateFuncState(isWindow(WIN_THROTTLE));
      }
      break;
    case 0xE4:
      if ((rxBufferXN[DATA1] & 0xF0) == 0x00) {             // Locomotive information normal locomotive   (0xE4,ID,SPD,FKTA,FKTB,XOR)
        locoData[myLocoData].mySteps = rxBufferXN[DATA1];                        // '0000BFFF'
        locoData[myLocoData].myDir   = rxBufferXN[DATA2] & 0x80;                 // 'RVVVVVVV'
        locoData[myLocoData].mySpeed = rxBufferXN[DATA2] & 0x7F;
        locoData[myLocoData].myFunc.Bits &= 0xFFFFE000;                          // '000FFFFF','FFFFFFFF'
        locoData[myLocoData].myFunc.Bits |= ((unsigned long)rxBufferXN[DATA4] << 5);
        locoData[myLocoData].myFunc.xFunc[0] |= ((rxBufferXN[DATA3] & 0x0F) << 1);
        bitWrite(locoData[myLocoData].myFunc.xFunc[0], 0, bitRead(rxBufferXN[DATA3], 4));
        updateFuncState(isWindow(WIN_THROTTLE));
        if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
          updateSpeedHID();
      }
      break;
    case 0xE7:
      if ((rxBufferXN[DATA1] & 0xF0) == 0x00) {             // Locomotive function info F13..F20 MM       (0xE7,STP,SPD,FNC,FNC,FNC,0x00,0x00,XOR)
        locoData[myLocoData].mySteps = rxBufferXN[DATA1];                        // '0000BFFF'
        locoData[myLocoData].myDir   = rxBufferXN[DATA2] & 0x80;                 // 'RVVVVVVV'
        locoData[myLocoData].mySpeed = rxBufferXN[DATA2] & 0x7F;
        locoData[myLocoData].myFunc.Bits &= 0xFE00000;
        locoData[myLocoData].myFunc.Bits |= ((unsigned long)rxBufferXN[DATA5] << 13);
        locoData[myLocoData].myFunc.Bits |= ((unsigned long)rxBufferXN[DATA4] << 5);
        locoData[myLocoData].myFunc.xFunc[0] |= ((rxBufferXN[DATA3] & 0x0F) << 1);
        bitWrite(locoData[myLocoData].myFunc.xFunc[0], 0, bitRead(rxBufferXN[DATA3], 4));
        updateFuncState(isWindow(WIN_THROTTLE));
        if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
          updateSpeedHID();
      }
      break;
    case 0xF3:
      if (rxBufferXN[DATA1] == 0x0A) {                      // Multimaus firmware version                 (0xF3,0x0A,VERH,VERL,XOR)
        highVerMM = rxBufferXN[DATA2];
        lowVerMM = rxBufferXN[DATA3];
      }
      break;
    default:
      if ((rxBufferXN[HEADER] & 0xF0) == 0x40) {            // Feedback broadcast / Accessory decoder information response (0x4X,MOD,DATA,...,XOR)
        for (n = HEADER; n < (rxBytes - 2); n += 2) {
          modulo = rxBufferXN[n + 1];
          dato = rxBufferXN[n + 2];
          if ((dato & 0x60) != 0x40) {                      // Accessory feedback
            adr = (modulo << 2) + 1;
            if (bitRead(dato, 4))
              adr = adr + 2;
            if ((dato & 0x03) == 0x01)
              accessoryChange(adr, false);
            if ((dato & 0x03) == 0x02)
              accessoryChange(adr, true);
            if ((dato & 0x0C) == 0x04)
              accessoryChange(adr + 1, false);
            if ((dato & 0x0C) == 0x08)
              accessoryChange(adr + 1, true);
          }
          else {                                            // RS Feedback

          }
        }
      }
      break;
  }
}

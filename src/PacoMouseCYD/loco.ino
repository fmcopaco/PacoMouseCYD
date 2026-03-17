/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

////////////////////////////////////////////////////////////
// ***** SOPORTE LOCOMOTORAS *****
////////////////////////////////////////////////////////////

uint16_t checkLocoAddress(uint16_t loco) {
  if (useID) {
  }
  else {
    loco &= 0x3FFF;
    if ((loco > 9999) || (loco == 0))                       // Comprueba que este entre 1 y 9999
      loco = 3;
  }
  //DEBUG_MSG("Loco: %d", loco);
  return loco;
}


void pushLoco(unsigned int loco) {                          // mete locomotora en el stack
  byte pos;
  unsigned int adr;
  for (pos = 0; pos < LOCOS_IN_STACK; pos++) {              // busca loco en stack
    if (locoStack[pos] == loco)
      locoStack[pos] = 0;                                   // evita que se repita
  }
  pos = 0;
  do {
    adr = locoStack[pos];                                   // push the loco in stack
    locoStack[pos] = loco;
    loco = adr;
    pos++;
  } while ((adr > 0) && (pos < LOCOS_IN_STACK));
#ifdef DEBUG
  Serial.print(F("STACK: "));
  for (pos = 0; pos < LOCOS_IN_STACK; pos++) {
    Serial.print(locoStack[pos]);
    Serial.print(' ');
  }
  Serial.println();
#endif
}

void popLoco(unsigned int loco) {                           // elimina locomotora del stack
  byte pos, n;
  unsigned int adr;
  pos = LOCOS_IN_STACK;
  for (n = 0; n < LOCOS_IN_STACK; n++) {                    // busca loco en stack
    if (locoStack[n] == loco)
      pos = n;                                              // save position
  }
  if (pos != LOCOS_IN_STACK) {
    for (n = pos; n < (LOCOS_IN_STACK - 1); n++) {
      locoStack[n] = locoStack[n + 1];
    }
    locoStack[LOCOS_IN_STACK - 1] = 0;
  }
}

void initLocos() {
  uint16_t pos;
  myLocoData = 0;
  for (pos = 0; pos < LOCOS_IN_STACK; pos++) {
    locoStack[pos] = 0;                                     // clear loco stack
    clearLocoData(pos);
  }
}


void clearLocoData (uint16_t pos) {
  uint16_t cnt;
  locoData[pos].myAddr.address = 0;                         // clear loco data
  locoData[pos].myName[0] = '\0';
  locoData[pos].myFunc.Bits = 0;
  locoData[pos].myDir = 0x80;
  locoData[pos].mySpeed = 0;
  locoData[pos].mySteps = DEFAULT_STEPS;
  locoData[pos].myVmax = 100;
  locoData[pos].myLocoID = SYS_NO_LOK;
  locoData[pos].myFuncIcon[0] = FNC_LIGHT_OFF;
  for (cnt = 1; cnt < 29; cnt++)
    locoData[pos].myFuncIcon[cnt] = FNC_FUNC_OFF;
  locoData[pos].myFuncIcon[cnt] = FNC_BLANK_OFF;
}


void getLastLoco() {
  uint16_t loco;
  loco = locoStack[0];                                      // get most recent loco in stack (filesystem)
  loco = checkLocoAddress(loco);                            // avoid empty stack
  findLocoData(loco);                                       // get pos in stack also check stack full
  loadThrottleData();                                       // load data to throttle
  updateSpeedHID();                                         // set encoder
  setTimer(TMR_INFO, 10, TMR_ONESHOT);
}

void getNewLoco(uint16_t loco) {
  if (useID) {
    if (loco != locoData[myLocoData].myLocoID) {
      releaseLoco();
    }
  }
  else {
    if (loco != locoData[myLocoData].myAddr.address) {
      releaseLoco();
    }
  }
  loco = checkLocoAddress(loco);
  findLocoData(loco);                                       // get pos in stack also check stack full
  loadThrottleData();                                       // load data to throttle
  updateSpeedHID();                                         // set encoder
  infoLocomotora(loco);                                     // ask current values
  setTimer(TMR_INFO, 5, TMR_ONESHOT);
}

void findLocoData(uint16_t loco) {
  uint16_t pos, cnt;
  pos = 0;
  if (useID) {
    while (pos < LOCOS_IN_STACK) {                          // find ID loco data
      if (locoData[pos].myLocoID == loco) {
        myLocoData = pos;
        pushLoco(loco);
        DEBUG_MSG("Find ID%d data in %d", loco, pos);
        return;
      }
      pos++;
    }
  }
  else {
    while (pos < LOCOS_IN_STACK) {                          // find address loco data
      if (locoData[pos].myAddr.address == loco) {
        myLocoData = pos;
        pushLoco(loco);
        DEBUG_MSG("Find %d data in %d", loco, pos);
        return;
      }
      pos++;
    }
    DEBUG_MSG("New Loco")
    pos = 0;                                                // new loco
    while (pos < LOCOS_IN_STACK) {
      if (locoData[pos].myAddr.address == 0) {
        myLocoData = pos;
        locoData[pos].myAddr.address = loco;
        locoData[pos].myLocoID = SYS_ELOK;
        //locoData[pos].mySteps = DEFAULT_STEPS;
        pushLoco(loco);
        return;
      }
      pos++;
    }
  }
  myLocoData = 0;                                           // stack full
  alertWindow(ERR_FULL);
}


void loadThrottleData() {
  uint16_t n;
  snprintf (locoName, NAME_LNG + 1, "%s", locoData[myLocoData].myName );
  txtData[TXT_LOCO_NAME].font = (strlen(locoName) > 13) ? FSS7 : FSS9;
  sprintf (locoAddr, "%d", locoData[myLocoData].myAddr.address);
  lpicData[LPIC_MAIN].id = locoData[myLocoData].myLocoID;
  iconData[ICON_FWD].color = (locoData[myLocoData].myDir & 0x80) ? COLOR_NAVY : COLOR_DARKGREY;
  iconData[ICON_REV].color = (locoData[myLocoData].myDir & 0x80) ? COLOR_DARKGREY : COLOR_NAVY;
  for (n = 0; n < 10; n++) {
    fncData[FNC_FX0 + n].num = n;
    fncData[FNC_FX0 + n].idIcon = locoData[myLocoData].myFuncIcon[n];
  }
  updateFuncState(false);
}


void updateFuncState(bool show) {
  byte n;
  bool state;
  for (n = 0; n < 10; n++) {
    state = bitRead(locoData[myLocoData].myFunc.Bits, fncData[FNC_FX0 + n].num);
    if (fncData[FNC_FX0 + n].state != state) {
      fncData[FNC_FX0 + n].state = state;
      if (show)
        newEvent(OBJ_FNC, FNC_FX0 + n, EVNT_DRAW);
      DEBUG_MSG("Change in F%d", fncData[FNC_FX0 + n].num);
    }
  }
}


void toggleFunction(uint8_t fnc, uint8_t id) {
  locoData[myLocoData].myFunc.Bits ^= bit(fnc);
  fncData[id].state = bitRead(locoData[myLocoData].myFunc.Bits, fnc);
  funcOperations(fnc);
  newEvent(OBJ_FNC, id, EVNT_DRAW);
}

void showFuncBlock(uint8_t fncOffset) {
  uint16_t ini, cnt;
  for (ini = 0; ini < 10; ini++) {
    cnt = ini + FNC_FX0;
    fncData[cnt].idIcon = locoData[myLocoData].myFuncIcon[ini + fncOffset];
    fncData[cnt].num = ini + fncOffset;
    fncData[cnt].state = bitRead(locoData[myLocoData].myFunc.Bits, fncData[cnt].num);
    drawObject(OBJ_FNC, cnt);
  }
}

void showNextFuncBlock()  {
  uint16_t ini, fncOffset;
  ini = fncData[FNC_FX0].num;
  fncOffset = (ini == 20) ? 0 : ini + 10;
  showFuncBlock(fncOffset);
}

void showPrevFuncBlock()  {
  uint16_t ini, fncOffset;
  ini = fncData[FNC_FX0].num;
  fncOffset = (ini == 0) ? 20 : ini - 10;
  showFuncBlock(fncOffset);
}

void updateSpeedHID() {
  byte spd, steps;
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
    case CLIENT_XNET:
      if (bitRead(locoData[myLocoData].mySteps, 2)) {       // 0..127 -> 0..63
        encoderMax = 63;
        encoderValue = (locoData[myLocoData].mySpeed > 1) ? (locoData[myLocoData].mySpeed >> 1) : 0;
      }
      else {
        if (bitRead(locoData[myLocoData].mySteps, 1)) {     // 0..31
          encoderMax = 31;
          spd = (locoData[myLocoData].mySpeed & 0x0F) << 1;
          if (bitRead(locoData[myLocoData].mySpeed, 4))
            bitSet(spd, 0);;
          encoderValue = (spd > 3) ? spd : 0;
        }
        else {                                              // 0..15
          encoderMax = 15;
          spd = locoData[myLocoData].mySpeed & 0x0F;
          encoderValue = (spd > 1) ? spd : 0;
        }
      }
      break;
    case CLIENT_LNET:
      steps =   getMaxStepLnet();
      encoderMax = (steps == 128) ? 63 : ((steps == 28) ? 31 : 15);
      if (locoData[myLocoData].mySpeed > 1) {
        if (steps == 128) {                                 // Max 100% speed (64 pasos, compatible con 14, 28 y 128 pasos)
          encoderValue = locoData[myLocoData].mySpeed >> 1; // 0..127 -> 0..63
        }
        else {
          if (steps == 28) {                                // 0..31
            spd = (((locoData[myLocoData].mySpeed - 2) << 1) / 9);
            encoderValue = spd + 4;
          }
          else {
            spd = (locoData[myLocoData].mySpeed - 2) / 9;
            encoderValue = spd + 2;
          }
        }
      }
      else
        encoderValue = 0;
      //DEBUG_MSG("HID Enc:%d Spd:%d", encoderValue, mySpeed);
      break;
    case CLIENT_ECOS:
      encoderMax = 63;                                      // 0..127 -> 0..63
      encoderValue = (locoData[myLocoData].mySpeed > 1) ? (locoData[myLocoData].mySpeed >> 1) : 0;
      break;
  }
  updateSpeedDir();
}


void updateMySpeed() {
  byte spd, steps;
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
    case CLIENT_XNET:
      if (bitRead(locoData[myLocoData].mySteps, 2)) {       // 0..63 -> 0..127
        if ((encoderValue == 0) && shuntingMode)            // comprueba Modo maniobras
          encoderValue = 1;
        locoData[myLocoData].mySpeed = encoderValue << 1;
      }
      else {
        if (bitRead(locoData[myLocoData].mySteps, 1)) {     // 0..31 -> 0..31     '---43210' -> '---04321'
          encoderValue = shuntingSpeed (encoderValue, 4);
          if (encoderValue > 3) {
            locoData[myLocoData].mySpeed =  (encoderValue >> 1) & 0x0F;
            bitWrite(locoData[myLocoData].mySpeed, 4, bitRead(encoderValue, 0));
          }
          else {
            switch (encoderValue) {
              case 0:
              case 3:
                locoData[myLocoData].mySpeed = 0;
                encoderValue = 0;
                break;
              case 1:
              case 2:
                locoData[myLocoData].mySpeed = 2;
                encoderValue = 4;
                break;
            }
          }
        }
        else {
          encoderValue = shuntingSpeed (encoderValue, 2);
          locoData[myLocoData].mySpeed = (encoderValue > 1) ? encoderValue : 0;       // 0..15 -> 0..15
        }
      }
      break;
    case CLIENT_LNET:
      steps =   getMaxStepLnet();
      if (steps == 128) {
        if ((encoderValue == 0) && shuntingMode)            // Modo maniobras
          encoderValue = 1;
        locoData[myLocoData].mySpeed = (encoderValue << 1); // 0..63 -> 0..127
        if (encoderValue > 61)
          locoData[myLocoData].mySpeed++;
      }
      else {
        if (steps == 28) {
          encoderValue = shuntingSpeed (encoderValue, 4);
          if (encoderValue > 3) {
            spd = ((encoderValue - 3) * 9) + 1;             // 0..31 -> 0..127
            locoData[myLocoData].mySpeed =  (spd >> 1) + 1;
          }
          else {
            switch (encoderValue) {
              case 0:
              case 3:
                locoData[myLocoData].mySpeed = 0;
                encoderValue = 0;
                break;
              case 1:
              case 2:
                locoData[myLocoData].mySpeed = 5;
                encoderValue = 4;
                break;
            }
          }
        }
        else {
          encoderValue = shuntingSpeed (encoderValue, 2);
          if (encoderValue == 1)
            return;
          locoData[myLocoData].mySpeed = (encoderValue > 1) ? ((encoderValue - 2) * 9) + 2 : 0;    // 0..15 -> 0..127
        }
      }
      //DEBUG_MSG("LN  Enc:%d Spd:%d", encoderValue, mySpeed);
      break;
    case CLIENT_ECOS:
      if ((encoderValue == 0) && shuntingMode)              // Modo maniobras
        encoderValue = 1;
      locoData[myLocoData].mySpeed = (encoderValue << 1);   // 0..63 -> 0..127
      if (encoderValue > 61)
        locoData[myLocoData].mySpeed++;
      break;
  }
  locoOperationSpeed();
}


byte shuntingSpeed (byte encoder, byte stepMin) {           // comprueba Modo maniobras
  if (encoder < stepMin) {
    if (shuntingMode)
      return stepMin;
  }
  return encoder;
}


void updateSpeedDir() {
  uint16_t angle, spd, stp;
  iconData[ICON_FWD].color = (locoData[myLocoData].myDir & 0x80) ? COLOR_NAVY : COLOR_DARKGREY;
  iconData[ICON_REV].color = (locoData[myLocoData].myDir & 0x80) ? COLOR_DARKGREY : COLOR_NAVY;
  if (isWindow(WIN_THROTTLE)) {
    angle = map(encoderValue, 0, encoderMax, 0, 255);
    // if ((encoderMax == 15) && (encoderValue < 2))
    //   angle = 0;
    drawObject(OBJ_ICON, ICON_FWD);
    drawObject(OBJ_ICON, ICON_REV);
    spd = map(angle, 0, 255, 0, locoData[myLocoData].myVmax);
    stp = getCurrentStep();
    drawSpeed(angle, spd, stp);
    DEBUG_MSG("Enc: %d-%d Spd: %d Stp: %d", encoderValue, encoderMax, spd, stp)
  }
  if (isWindow(WIN_SPEEDO)) {
    gaugeData[GAUGE_SPEEDO].value = map(encoderValue, 0, encoderMax, 0, 255);
    fncData[FNC_SPEEDO_DIR].idIcon = (locoData[myLocoData].myDir & 0x80) ? FNC_NEXT_OFF : FNC_PREV_OFF;
    drawObject(OBJ_GAUGE, GAUGE_SPEEDO);
    drawObject(OBJ_FNC, FNC_SPEEDO_DIR);
    drawSpeedoStep();
  }
  if (isWindow(WIN_STA_PLAY)) {
    gaugeData[GAUGE_STATION].value = map(encoderValue, 0, encoderMax, 0, 255);
    fncData[FNC_STA_DIR].idIcon = (locoData[myLocoData].myDir & 0x80) ? FNC_NEXT_OFF : FNC_PREV_OFF;
    drawObject(OBJ_GAUGE, GAUGE_STATION);
    drawObject(OBJ_FNC, FNC_STA_DIR);
  }
}


void drawSpeed(uint16_t angle, uint16_t spd, uint16_t stp) {
  gaugeData[GAUGE_SPEED].value = angle;                     // draw needle
  drawGaugeNeedle(GAUGE_SPEED);
  sprite.createSprite(36, 16);                              // 8bpp requires 36 * 16 = 576 bytes
  sprite.fillSprite(gaugeData[GAUGE_SPEED].color);          // Fill the Sprite with black //gaugeData[GAUGE_SPEED].color
  sprite.setFreeFont(FSSB9);
  sprite.setTextColor(shuntingMode ? COLOR_YELLOW : COLOR_WHITE);
  sprite.setTextDatum(MC_DATUM);
  sprite.drawNumber(spd, 18, 8);                            // Draw current speed in km/h
  sprite.pushSprite(gaugeData[GAUGE_SPEED].x - 18, gaugeData[GAUGE_SPEED].y - 8, COLOR_TRANSPARENT);
  sprite.fillSprite(COLOR_BACKGROUND);                      // Draw current step
  sprite.setFreeFont(FSS7);
  sprite.drawNumber(stp, 18, 8);
  sprite.pushSprite(gaugeData[GAUGE_SPEED].x - 18, gaugeData[GAUGE_SPEED].y + 44, COLOR_TRANSPARENT);
  sprite.deleteSprite();
}


void drawSpeedoStep() {
  uint16_t stp;
  stp = getCurrentStep();
  sprite.setColorDepth(16);                                 // Create an 16bpp Sprite
  sprite.createSprite(36, 16);                              // 8bpp requires 36 * 16 = 576 bytes * 2
  sprite.fillSprite(COLOR_BACKGROUND);                      // Draw current step
  sprite.setFreeFont(FSS7);
  sprite.setTextColor(COLOR_WHITE);
  sprite.setTextDatum(MC_DATUM);
  sprite.drawNumber(stp, 18, 8);
  sprite.pushSprite(gaugeData[GAUGE_SPEEDO].x - 18, gaugeData[GAUGE_SPEEDO].y + 20, COLOR_TRANSPARENT);
  sprite.deleteSprite();
}


uint16_t countLocoInStack() {
  uint16_t pos, total;
  total = 0;
  pos = 0;
  while ((locoStack[pos++] > 0) && (pos < LOCOS_IN_STACK))
    total++;
  return total;
}


void prepareLocoList() {
  uint16_t pos;
  for (pos = 0; pos < 6; pos++) {                           // delete list
    txtData[TXT_SEL_ADDR1 + pos].buf[0] = '\0';
    txtData[TXT_SEL_NAME1 + pos].buf[0] = '\0';
  }
  encoderValue = 0;                                         // count locos in stack
  encoderMax = countLocoInStack();
  DEBUG_MSG("Locos in list: %d", encoderMax);
  if (encoderMax > 0)
    encoderMax--;
  sortLocoList(SORT_LAST);
  populateLocoList();
}


void populateLocoList() {
  uint16_t posName;
  uint16_t line, adr, n;
  for (n = 0; n < 6; n++) {
    if (n < encoderMax + 1) {
      line = (encoderValue > 5) ? encoderValue - 5 : 0;
      adr = sortedLocoStack[line + n];
      posName = findLocoPos(adr);
      //DEBUG_MSG("Enc: %d Line: %d Adr: %d", encoderValue, line, adr);
      if (useID)
        snprintf(txtData[TXT_SEL_ADDR1 + n].buf, ADDR_LNG + 1, "%d", locoData[posName].myAddr.address);
      else
        snprintf(txtData[TXT_SEL_ADDR1 + n].buf, ADDR_LNG + 1, "%d", adr);
      if (posName != LOCOS_IN_STACK)
        snprintf(txtData[TXT_SEL_NAME1 + n].buf, NAME_LNG + 1, locoData[posName].myName);
    }
  }
  line = (encoderValue > 5) ? 5 : encoderValue;
  for (n = 0; n < 6; n++) {
    txtData[TXT_SEL_ADDR1 + n].backgnd = (n == line) ? COLOR_YELLOW : COLOR_WHITE;
    txtData[TXT_SEL_NAME1 + n].backgnd = (n == line) ? COLOR_YELLOW : COLOR_WHITE;
  }
}


uint16_t findLocoPos(uint16_t loco) {
  uint16_t pos, n;
  pos = LOCOS_IN_STACK;
  for (n = 0; n < LOCOS_IN_STACK; n++) {
    if (useID) {
      if (locoData[n].myLocoID  == loco)                    // search ID in loco stack
        pos = n;
    }
    else {
      if (locoData[n].myAddr.address  == loco)              // search address in loco stack
        pos = n;
    }
  }
  return pos;
}


void sortLocoList (uint16_t order) {
  uint16_t tmp, n, i, j, total, pos;
  bool reverse;
#ifdef DEBUG
  Serial.print(F("INI.STACK: "));
  for (pos = 0; pos < LOCOS_IN_STACK; pos++) {
    Serial.print(sortedLocoStack[pos]);
    Serial.print(' ');
  }
  Serial.println();
#endif
  objStack[posObjStack1].objID = ICON_LAST_UP + order;
  currOrder = order;
  total = countLocoInStack();
  reverse = false;
  //DEBUG_MSG("Order %d Total %d", currOrder, total)
  switch (order) {
    case SORT_LAST:
      for (n = 0; n < LOCOS_IN_STACK; n++)
        sortedLocoStack[n] = locoStack[n];
      break;
    case SORT_NUM_DWN:
      reverse = true;
    case SORT_NUM_UP:
      for (i = 1; i < total; i++) {
        if (useID) {
          for (j = i; j > 0 && (idOrder(sortedLocoStack[j - 1], sortedLocoStack[j]) != reverse); j--) {
            tmp = sortedLocoStack[j - 1];
            sortedLocoStack[j - 1] = sortedLocoStack[j];
            sortedLocoStack[j] = tmp;
          }
        }
        else {
          for (j = i; j > 0 && ((sortedLocoStack[j - 1] > sortedLocoStack[j]) != reverse); j--) {
            tmp = sortedLocoStack[j - 1];
            sortedLocoStack[j - 1] = sortedLocoStack[j];
            sortedLocoStack[j] = tmp;
          }
        }
      }
      break;
    case SORT_NAME_DWN:
      reverse = true;
    case SORT_NAME_UP:
      for (i = 1; i < total; i++) {
        for (j = i; j > 0 && (nameOrder(sortedLocoStack[j - 1], sortedLocoStack[j]) != reverse); j--) {
          tmp = sortedLocoStack[j - 1];
          sortedLocoStack[j - 1] = sortedLocoStack[j];
          sortedLocoStack[j] = tmp;
        }
      }
      break;
  }
#ifdef DEBUG
  Serial.print(F("END.STACK: "));
  for (pos = 0; pos < LOCOS_IN_STACK; pos++) {
    Serial.print(sortedLocoStack[pos]);
    Serial.print(' ');
  }
  Serial.println();
#endif
}


bool nameOrder(uint16_t first, uint16_t second) {
  uint16_t posFirst, posSecond;
  posFirst = findLocoPos(first);
  posSecond = findLocoPos(second);
  return strcmp(locoData[posFirst].myName, locoData[posSecond].myName) > 0;
}


bool idOrder(uint16_t first, uint16_t second) {
  uint16_t posFirst, posSecond;
  posFirst = findLocoPos(first);
  posSecond = findLocoPos(second);
  return (locoData[posFirst].myAddr.address > locoData[posSecond].myAddr.address);
}

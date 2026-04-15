/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/


////////////////////////////////////////////////////////////
// ***** ACCESSORY FIFO *****
////////////////////////////////////////////////////////////

void initFIFO() {
  lastInFIFO = 0;
  firstOutFIFO = 0;
  cntFIFO = 0;
  stateFIFO = FIFO_EMPTY;
}


unsigned int  readFIFO () {
  firstOutFIFO = (firstOutFIFO + 1 ) & 0x1F;                      // next one (hardcoded)
  cntFIFO--;
  return (accessoryFIFO[firstOutFIFO]);
}


void writeFIFO (uint16_t FAdr, uint8_t pos) {
  lastInFIFO = (lastInFIFO + 1 ) & 0x1F;                          // next one (hardcoded)
  cntFIFO++;
  if (pos > 0)
    FAdr |= 0x8000;
  accessoryFIFO[lastInFIFO] = FAdr;                               // save in FIFO
}


void sendAccessoryFIFO () {
  switch (stateFIFO) {
    case FIFO_ACC_CDU:                                            // wait for CDU recharge
    case FIFO_EMPTY:
      if (cntFIFO > 0) {                                          // activate accessory from FIFO
        lastAccessory = readFIFO();
        sendAccessory(lastAccessory & 0x7FFF, bitRead(lastAccessory, 15), true);
        setTimer(TMR_ACCESSORY, TIME_ACC_ON, TMR_ONESHOT);
        stateFIFO = FIFO_ACC_ON;
        DEBUG_MSG("Moving acc. %d-%d", lastAccessory & 0x7FFF, lastAccessory >> 15);
      }
      else
        stateFIFO = FIFO_EMPTY;
      break;
    case FIFO_ACC_ON:                                             // deactivate accessory
      sendAccessory(lastAccessory & 0x7FFF, bitRead(lastAccessory, 15), false);
      setTimer(TMR_ACCESSORY, TIME_ACC_CDU, TMR_ONESHOT);
      stateFIFO = FIFO_ACC_CDU;
      break;
  }
}

////////////////////////////////////////////////////////////
// ***** ACCESSORY  *****
////////////////////////////////////////////////////////////

void moveAccessory(uint16_t FAdr, uint8_t pos) {
  writeFIFO(FAdr, pos);                                           // put accessory in FIFO
  if (stateFIFO == FIFO_EMPTY)                                    // if not pending accessories, force sending now
    sendAccessoryFIFO();
}

void setAccAspect(uint16_t FAdr, uint16_t FAdr2, uint8_t aspect, uint16_t outs) {
  uint8_t pos;
  pos = aspect * 4;
  if (FAdr > 0) {
    if (bitRead(outs, pos)) {
      moveAccessory(FAdr, 0);
    }
    else {
      if (bitRead(outs, pos + 1))
        moveAccessory(FAdr, 1);
    }
  }
  if (FAdr2 > 0) {
    if (bitRead(outs, pos + 2)) {
      moveAccessory(FAdr2, 0);
    }
    else {
      if (bitRead(outs, pos + 3))
        moveAccessory(FAdr2, 1);
    }
  }
}


void changeAccPosition(uint16_t FAdr, bool state) {
  uint8_t type, pos, aspect;
  for (pos = 0; pos < 16; pos++) {
    type = accPanel[pos].type;
    if (accDef[type].aspects == 2) {                              // only for two aspects accessories
      if (accPanel[pos].addr == FAdr) {
        aspect = getAccAspect(accPanel[pos].activeOutput, state);
        accPanel[pos].currAspect = aspect;
        fncData[FNC_ACC0 + pos].idIcon = accDef[type].icon[aspect].fncIcon;
        fncData[FNC_ACC0 + pos].color = accDef[type].icon[aspect].color;
        fncData[FNC_ACC0 + pos].colorOn = accDef[type].icon[aspect].colorOn;
        newEvent(OBJ_FNC, FNC_ACC0 + pos, EVNT_DRAW);
      }
    }
  }
}


uint8_t getAccAspect(uint16_t activeOutput, bool state) {
  uint16_t outAcc;
  outAcc = state ? 0x0002 : 0x0001;
  if ((activeOutput & 0x000F) == outAcc)
    return 0;
  outAcc = state ? 0x0020 : 0x0010;
  if ((activeOutput & 0x00F0) == outAcc)
    return 1;
  return 0;
}


////////////////////////////////////////////////////////////
// ***** PANEL  *****
////////////////////////////////////////////////////////////

void deleteAccPanelElement(uint8_t pos) {
  accPanel[pos].type = ACC_UNDEF;
  accPanel[pos].currAspect = 0;
  accPanel[pos].addr = 0;
  accPanel[pos].addr2 = 0;
  accPanel[pos].activeOutput = 0;
  accPanel[pos].accName[0] = '\0';
}

void loadDefaultAccPanel() {
  uint8_t n;
  for (n = 0; n < 16; n++) {                                      // Default panel is all blank with only a keypad button
    deleteAccPanelElement(n);
  }
  accPanel[15].type = ACC_KEYPAD;
}


void updateAccPanel() {
  uint8_t n, type, aspect;

  snprintf(panelNameBuf, PANEL_LNG + 1, panelNamesBuf[currPanel]);
  for (n = 0; n < 16; n++) {
    type = accPanel[n].type;
    aspect = accPanel[n].currAspect;
    snprintf(accNamesBuf[n], ACC_LNG + 1, accPanel[n].accName);
    fncData[FNC_ACC0 + n].num = accDef[type].num;
    fncData[FNC_ACC0 + n].idIcon = accDef[type].icon[aspect].fncIcon;
    fncData[FNC_ACC0 + n].color = accDef[type].icon[aspect].color;
    fncData[FNC_ACC0 + n].colorOn = accDef[type].icon[aspect].colorOn;
    fncData[FNC_ACC0 + n].backgnd = (type == ACC_UNDEF) ? COLOR_WHITE : COLOR_LIGHTGREY;
    buttonData[BUT_ACC_0 + n].backgnd = (type == ACC_UNDEF) ? COLOR_WHITE : COLOR_LIGHTGREY;
  }
  n = 30;
  getLabelOption(n + 024, panelLabelBuf, 0);
  if (isLabelOption(panelLabelBuf, "tu:nofacis.phem/r") != n)
    iconData[n - 021].x = n * 4;
}

void saveCurrentAspects() {
  uint8_t n;
  for (n = 0; n < 16; n++)
    savedAspect[currPanel][n] = accPanel[n].currAspect;
}

void getLastAspects() {
  uint8_t n, type;
  uint16_t FAdr;
  bool state;
  for (n = 0; n < 16; n++) {
    type = accPanel[n].type;
    if (accDef[type].aspects == 2) {                              // only for two aspects accessories
      FAdr = accPanel[n].addr;
      state = bitRead(accPosition[FAdr >> 3], FAdr & 0x0007);
      accPanel[n].currAspect =  getAccAspect(accPanel[n].activeOutput, state);
    }
    else
      accPanel[n].currAspect = savedAspect[currPanel][n];
  }
}

void deleteLastAspects() {
  uint8_t n;
  for (n = 0; n < 16; n++)
    accPanel[n].currAspect = 0;
}

void initLastAspects() {
  uint16_t i, j;
  for (i = 0; i < 16; i++)
    for (j = 0; j < 16; j++)
      savedAspect[i][j] = 0;;
  for (i = 0; i < 256; i++)
    accPosition[i] = 0;
}

void populateAccPanel() {
  loadDefaultAccPanel();                                          // Load panel data
  if (sdDetected) {
    if (loadAccPanel(SD))
      getLastAspects();
    else
      deleteLastAspects();
  }
  else {
    if (loadAccPanel(LittleFS))
      getLastAspects();
    else
      deleteLastAspects();
  }
  updateAccPanel();
}

void accPanelClick(uint8_t pos) {
  uint16_t addr, outs;
  uint8_t type, aspect;
  currPanelAcc = pos;
  if (editAccessory) {
    paramChild = pos;
    type = accPanel[pos].type;
    fncData[FNC_ACC_TYPE].num = accDef[type].num;
    fncData[FNC_ACC_TYPE].idIcon = accDef[type].icon[0].fncIcon;
    fncData[FNC_ACC_TYPE].color = accDef[type].icon[0].color;
    fncData[FNC_ACC_TYPE].colorOn = accDef[type].icon[0].colorOn;
    encoderValue = type;
    encoderMax = ACC_MAX - 1;
    openWindow(WIN_ACC_TYPE);
  }
  else {
    type = accPanel[pos].type;
    switch (type) {
      case ACC_UNDEF:
        break;
      case ACC_KEYPAD:
        openWindow(WIN_ACC_CTRL);
        break;
      default:
        addr = accPanel[pos].addr;
        outs = accPanel[pos].activeOutput;
        switch (accDef[type].aspects) {
          case 2:
            aspect = (accPanel[pos].currAspect > 0) ? 0 : 1;
            accPanel[pos].currAspect = aspect;
            fncData[FNC_ACC0 + pos].idIcon = accDef[type].icon[aspect].fncIcon;
            fncData[FNC_ACC0 + pos].color = accDef[type].icon[aspect].color;
            fncData[FNC_ACC0 + pos].colorOn = accDef[type].icon[aspect].colorOn;
            setAccAspect(addr, 0, aspect, outs);
            newEvent(OBJ_BUTTON, BUT_ACC_0 + pos, EVNT_DRAW);
            break;
          case 3:
            currAccAspects = 3;
            winData[WIN_ACC_ASPECT].x = 30;
            winData[WIN_ACC_ASPECT].w = 180;
            buttonData[BUT_ACC_ASPECT0].x = 50;
            buttonData[BUT_ACC_ASPECT1].x = 100;
            buttonData[BUT_ACC_ASPECT2].x = 150;
            fncData[FNC_ASPECT0].x = 54;
            fncData[FNC_ASPECT0].idIcon  = accDef[type].icon[0].fncIcon;
            fncData[FNC_ASPECT0].color   = accDef[type].icon[0].color;
            fncData[FNC_ASPECT0].colorOn = accDef[type].icon[0].colorOn;
            fncData[FNC_ASPECT1].x = 104;
            fncData[FNC_ASPECT1].idIcon  = accDef[type].icon[1].fncIcon;
            fncData[FNC_ASPECT1].color   = accDef[type].icon[1].color;
            fncData[FNC_ASPECT1].colorOn = accDef[type].icon[1].colorOn;
            fncData[FNC_ASPECT2].x = 154;
            fncData[FNC_ASPECT2].idIcon  = accDef[type].icon[2].fncIcon;
            fncData[FNC_ASPECT2].color   = accDef[type].icon[2].color;
            fncData[FNC_ASPECT2].colorOn = accDef[type].icon[2].colorOn;
            openWindow(WIN_ACC_ASPECT);
            break;
          case 4:
            currAccAspects = 4;
            winData[WIN_ACC_ASPECT].x = 5;
            winData[WIN_ACC_ASPECT].w = 230;
            buttonData[BUT_ACC_ASPECT0].x = 25;
            buttonData[BUT_ACC_ASPECT1].x = 75;
            buttonData[BUT_ACC_ASPECT2].x = 125;
            buttonData[BUT_ACC_ASPECT3].x = 175;
            fncData[FNC_ASPECT0].x = 29;
            fncData[FNC_ASPECT0].idIcon  = accDef[type].icon[0].fncIcon;
            fncData[FNC_ASPECT0].color   = accDef[type].icon[0].color;
            fncData[FNC_ASPECT0].colorOn = accDef[type].icon[0].colorOn;
            fncData[FNC_ASPECT1].x = 79;
            fncData[FNC_ASPECT1].idIcon  = accDef[type].icon[1].fncIcon;
            fncData[FNC_ASPECT1].color   = accDef[type].icon[1].color;
            fncData[FNC_ASPECT1].colorOn = accDef[type].icon[1].colorOn;
            fncData[FNC_ASPECT2].x = 129;
            fncData[FNC_ASPECT2].idIcon  = accDef[type].icon[2].fncIcon;
            fncData[FNC_ASPECT2].color   = accDef[type].icon[2].color;
            fncData[FNC_ASPECT2].colorOn = accDef[type].icon[2].colorOn;
            fncData[FNC_ASPECT3].x = 179;
            fncData[FNC_ASPECT3].idIcon  = accDef[type].icon[3].fncIcon;
            fncData[FNC_ASPECT3].color   = accDef[type].icon[3].color;
            fncData[FNC_ASPECT3].colorOn = accDef[type].icon[3].colorOn;
            openWindow(WIN_ACC_ASPECT);
            break;
          default:
            buttonData[BUT_ACC_0 + pos].backgnd = COLOR_BLACK;
            drawObject(OBJ_BUTTON, BUT_ACC_0 + pos);
            buttonData[BUT_ACC_0 + pos].backgnd = COLOR_LIGHTGREY;
            setAccAspect(addr, 0, 0, outs);
            newEvent(OBJ_BUTTON, BUT_ACC_0 + pos, EVNT_DRAW);
            delay(80);
            break;
        }
        break;
    }
  }
}


void accAspectClick(uint8_t aspect) {
  uint16_t addr, addr2, outs;
  uint8_t type;
  type = accPanel[currPanelAcc].type;
  accPanel[currPanelAcc].currAspect = aspect;
  fncData[FNC_ACC0 + currPanelAcc].idIcon = accDef[type].icon[aspect].fncIcon;
  fncData[FNC_ACC0 + currPanelAcc].color = accDef[type].icon[aspect].color;
  fncData[FNC_ACC0 + currPanelAcc].colorOn = accDef[type].icon[aspect].colorOn;
  addr = accPanel[currPanelAcc].addr;
  addr2 = accPanel[currPanelAcc].addr2;
  outs = accPanel[currPanelAcc].activeOutput;
  setAccAspect(addr, addr2, aspect, outs);
}


void accTypeClick() {
  uint8_t index, n;
  index = encoderValue;
  switch (index) {
    case ACC_UNDEF:
      alertWindow(ERR_ASK_SURE);
      break;
    case ACC_KEYPAD:
      //editAccessory = false;
      accPanelChanged = true;
      //winData[WIN_ACCESSORY].backgnd = COLOR_WHITE;
      deleteAccPanelElement(paramChild);
      accPanel[paramChild].type = ACC_KEYPAD;
      updateAccPanel();
      updateSpeedHID();                                           // set encoder
      closeWindow(WIN_ACC_TYPE);
      break;
    default:
      if (index != accPanel[paramChild].type) {
        currAccEdit.type = (accType)index;
        currAccEdit.addr = 0;
        currAccEdit.addr2 = 0;
        currAccEdit.currAspect = 0;
        currAccEdit.activeOutput = accOutDefault[index];
        currAccEdit.accName[0] = '\0';
      }
      else {
        currAccEdit = accPanel[paramChild];
      }
      snprintf(accKeybName, ACC_LNG + 1, currAccEdit.accName);
      snprintf(accKeybAddr1, ADDR_LNG + 1, "%d", currAccEdit.addr);
      snprintf(accKeybAddr2, ADDR_LNG + 1, "%d", currAccEdit.addr2);
      for (n = 0; n < 4; n++) {
        fncData[FNC_EDIT_ASPECT0 + n].idIcon = accDef[index].icon[n].fncIcon;
        fncData[FNC_EDIT_ASPECT0 + n].color = accDef[index].icon[n].color;
        fncData[FNC_EDIT_ASPECT0 + n].colorOn = accDef[index].icon[n].colorOn;
      }
      accOutUpdate();
      closeWindow(WIN_ACC_TYPE);
      openWindow(WIN_ACC_EDIT);
      break;
  }
}


void accOutUpdate() {
  uint8_t n;
  for (n = 0; n < 16; n += 2) {
    buttonData[BUT_ACC_OUT0 + n].backgnd = bitRead(currAccEdit.activeOutput, n) ? COLOR_RED : COLOR_LIGHTBLACK;
    buttonData[BUT_ACC_OUT0 + n + 1].backgnd = bitRead(currAccEdit.activeOutput, n + 1) ? COLOR_GREEN : COLOR_LIGHTBLACK;
  }
}


void accOutClick(uint8_t out) {
  uint8_t outR, outG;
  outR = out & 0xFE;
  outG = out | 0x01;
  currAccEdit.activeOutput ^= bit(out);
  if (bitRead(out, 0)) {                                          // green
    if (bitRead(currAccEdit.activeOutput, outG))
      bitClear(currAccEdit.activeOutput, outR);
  }
  else {                                                          // red
    if (bitRead(currAccEdit.activeOutput, outR))
      bitClear(currAccEdit.activeOutput, outG);
  }
  accOutUpdate();
  newEvent(OBJ_BUTTON, BUT_ACC_OUT0 + outR, EVNT_DRAW);
  newEvent(OBJ_BUTTON, BUT_ACC_OUT0 + outG, EVNT_DRAW);
}


void updateAccChange() {
  accPanel[paramChild] = currAccEdit;
  updateAccPanel();
  accPanelChanged = true;
}


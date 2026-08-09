/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

/*
  Automation Opcodes
  0..9  Data
  L     Select loco
  l     Loco dir 0:REV, 1:FWD, 2:CHG, 3:E.STOP
  S     Speed 0..100
  F f   Function
  D     Delay (x100ms)
  T t   Turnout (green, red)
  Z z   Feedback
  R     New route start
  r     Loop (this route)
  C     Call other route
  X     End
*/

////////////////////////////////////////////////////////////
// ***** AUTOMATION EDITION *****
////////////////////////////////////////////////////////////

void autoShowItems () {                                                           // Show items in automation menu
  uint8_t num, n;
  bool fileOK;
  for (n = 0; n < 5; n++) {
    num = autoIndexStart + n + 1;
    txtData[TXT_AUTO_NUM0 + n].color = editAutomation ? COLOR_RED : COLOR_BLACK;
    txtData[TXT_AUTO_NAME0 + n].color = editAutomation ? COLOR_RED : COLOR_BLUE;
    sprintf(autoNumBuf[n], "%d", num);
    if (sdDetected)
      fileOK = loadAutomation(SD, num);
    else
      fileOK = loadAutomation(LittleFS, num);
    autoFileFound[n] = fileOK;
    if (fileOK) {
      snprintf(autoNameBuf[n], NAME_LNG + 1, "%s", autoName);
      if (editAutomation)
        fncData[FNC_AUTO0 + n].idIcon = FNC_ROUTE_DEL_OFF;
      else
        fncData[FNC_AUTO0 + n].idIcon = (findAutomation(num) != MAX_AUTO_SEQ) ? FNC_ROUTE_RUN_OFF : FNC_ROUTE_OFF;
    }
    else {
      fncData[FNC_AUTO0 + n].idIcon = editAutomation ? FNC_ROUTE_ADD_OFF : FNC_ROUTE_EMPTY_OFF;
      autoNameBuf[n][0] = '\0';
    }
  }
}

uint16_t getOpcodeParameter(char *buf, uint16_t *pos) {                           // get numeric parameter
  uint16_t value, cnt;
  char chr;
  value = 0;
  cnt = *pos;
  chr = buf[cnt];
  while (isDigit(chr)) {
    value = (value * 10) + ((uint16_t)(chr - '0'));
    cnt++;
    chr = buf[cnt];
  }
  *pos = cnt;
  return value;
}

char getOpcode(char *buf, uint16_t *pos, uint16_t *value) {                       // get opcode and parameter
  char chr;
  uint16_t cnt;
  *value = 0;
  cnt = *pos;
  chr = buf[cnt];
  switch (chr) {
    case 'L':
    case 'l':
    case 'S':
    case 'F':
    case 'f':
    case 'D':
    case 'T':
    case 't':
    case 'Z':
    case 'z':
    case 'R':
    case 'C':
      cnt++;
      *value = getOpcodeParameter(buf, &cnt);
      break;
    case 'r':
      cnt++;
      break;
    default:
      chr = 'X';
      break;
  }
  *pos = cnt;
  return chr;
}


uint16_t getOpcodeCount() {                                                       // get opcode count in automation
  uint16_t pos, cnt, value;
  pos = 0;
  cnt = 0;
  while (getOpcode(autoEditOpcodes, &pos, &value) != 'X') {
    cnt++;
  }
  return (cnt + 1);
}


void showOpcodeButton(uint16_t pos, uint8_t type, uint16_t value) {
  iconData[ICON_AUTO_OPC0 + pos].bitmap = autoDef[type].bitmap;
  iconData[ICON_AUTO_OPC0 + pos].color = autoDef[type].color;
  txtData[TXT_AUTO_OPC0 + pos].font = (value > 9999) ? FSSB6 : FSSB9;
  if (value == OBJ_NOT_FOUND)
    autoOpcodeBuf[pos][0] = '\0';
  else
    sprintf(autoOpcodeBuf[pos], "%d", value);
  if (pos == 0) {
    editType = type;
    editValue = value;
  }
}


void showOpcodes(uint16_t pos) {                                                  // show opcodes next five opcodes
  uint16_t n, value;
  char opc;
  for (n = 0; n < 5; n++) {
    opc = getOpcode(autoEditOpcodes, &pos, &value);
    switch (opc) {
      case 'L':
        showOpcodeButton(n, AUTO_L, value);
        break;
      case 'l':
        switch (value) {
          case 0:
            showOpcodeButton(n, AUTO_l0, OBJ_NOT_FOUND);
            break;
          case 1:
            showOpcodeButton(n, AUTO_l1, OBJ_NOT_FOUND);
            break;
          case 2:
            showOpcodeButton(n, AUTO_l2, OBJ_NOT_FOUND);
            break;
          case 3:
            showOpcodeButton(n, AUTO_l3, OBJ_NOT_FOUND);
            break;
        }
        break;
      case 'S':
        showOpcodeButton(n, AUTO_S, value);
        break;
      case 'F':
        showOpcodeButton(n, AUTO_F, value);
        break;
      case 'f':
        showOpcodeButton(n, AUTO_f, value);
        break;
      case 'D':
        showOpcodeButton(n, AUTO_D, value);
        txtData[TXT_AUTO_OPC0 + n].font = (value > 999) ? FSSB6 : FSSB9;
        sprintf(autoOpcodeBuf[n], "%d.%d", value / 10, value % 10);
        break;
      case 'T':
        showOpcodeButton(n, AUTO_T, value);
        break;
      case 't':
        showOpcodeButton(n, AUTO_t, value);
        break;
      case 'Z':
        showOpcodeButton(n, AUTO_Z, value);
        break;
      case 'z':
        showOpcodeButton(n, AUTO_z, value);
        break;
      case 'R':
        showOpcodeButton(n, AUTO_R, value);
        break;
      case 'C':
        showOpcodeButton(n, AUTO_C, value);
        break;
      case 'r':
        showOpcodeButton(n, AUTO_r, OBJ_NOT_FOUND);
        break;
      case 'X':
        showOpcodeButton(n, AUTO_X, OBJ_NOT_FOUND);
        while (n < 4) {
          n++;
          iconData[ICON_AUTO_OPC0 + n].bitmap = blank32;
          autoOpcodeBuf[n][0] = '\0';
        }
        break;
    }
  }
}


void populateOpcodeList() {
  uint8_t ini;
  uint16_t pos, value;
  ini = encoderValue;
  autoCurrEditPos = ini;
  pos = 0;
  while (ini > 0) {
    getOpcode(autoEditOpcodes, &pos, &value);
    ini--;
  }
  showOpcodes(pos);
  sprintf(autoOpcCntBuf, "%d/%d", (autoCurrEditPos + 1), getOpcodeCount());
}


void updateOpcodeList() {
  newEvent(OBJ_BUTTON, BUT_AUTO_OPC0, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_AUTO_OPC0, EVNT_DRAW);
  newEvent(OBJ_BUTTON, BUT_AUTO_OPC1, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_AUTO_OPC1, EVNT_DRAW);
  newEvent(OBJ_BUTTON, BUT_AUTO_OPC2, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_AUTO_OPC2, EVNT_DRAW);
  newEvent(OBJ_BUTTON, BUT_AUTO_OPC3, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_AUTO_OPC3, EVNT_DRAW);
  newEvent(OBJ_BUTTON, BUT_AUTO_OPC4, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_AUTO_OPC4, EVNT_DRAW);
  newEvent(OBJ_TXT, TXT_AUTO_CNT, EVNT_DRAW);
}


void autoSetEncoder(uint8_t pos) {                                                // setup encoder for edit automation
  encoderValue = pos;
  encoderMax = getOpcodeCount() - 1;
  autoCurrEditPos = pos;
}


void autoNewItem() {                                                              // default automation only has an end opcode
  sprintf(autoNameEditBuf, "FS%d", autoCurrEditing);
  sprintf(autoEditOpcodes, "X");
}

void deleteOpcode() {
  uint8_t ini;
  uint16_t opcPos, iniPos, pos, value, n;
  char opc;
  DEBUG_MSG("OPC: %s", autoEditOpcodes)
  pos = 0;
  ini = autoCurrEditPos;
  while (ini > 0) {
    getOpcode(autoEditOpcodes, &pos, &value);
    ini--;
  }
  opcPos = pos;
  opc = getOpcode(autoEditOpcodes, &pos, &value);
  iniPos = pos;
  while (opc != 'X')
    opc = getOpcode(autoEditOpcodes, &pos, &value);
  for (n = 0; n < (pos - iniPos + 1); n++)
    autoEditOpcodes[opcPos + n] = autoEditOpcodes[iniPos + n];
  autoEditOpcodes[opcPos + n] = '\0';
  DEBUG_MSG("DEL: %s", autoEditOpcodes)
}

void autoSaveFile() {
  bool fileOK;
  snprintf(autoName, NAME_LNG + 1, "%s", autoNameEditBuf);
  snprintf(autoOpcodes, MAX_LABEL_LNG + 1, "%s", autoEditOpcodes);
  if (sdDetected)
    fileOK = saveAutomation(SD, autoCurrEditing);
  else
    fileOK = saveAutomation(LittleFS, autoCurrEditing);
}


void getAutomation() {
  bool fileOK;
  if (sdDetected)
    fileOK = loadAutomation(SD, autoCurrEditing);
  else
    fileOK = loadAutomation(LittleFS, autoCurrEditing);
  if (fileOK) {
    snprintf(autoNameEditBuf, NAME_LNG + 1, "%s", autoName);
    snprintf(autoEditOpcodes, MAX_LABEL_LNG + 1, "%s", autoOpcodes);
  }
  else {
    autoNewItem();                                                                // file error, set default value
  }
  autoSetEncoder(0);
  populateOpcodeList();
}

void autoDeleteFile (uint8_t num) {
  bool fileOK;
  if (sdDetected)
    fileOK = deleteAutomation(SD, num);
  else
    fileOK = deleteAutomation(LittleFS, num);
  autoShowItems();
}


void setEditOpcode(bool addOpc) {
  addOpcode = addOpc;
  if (addOpc) {
    editType = AUTO_t;
    editValue = 1;
  }
  showOpcodeEdit(editType, editValue);
  encoderValue = editType;
  encoderMax = AUTO_OBJ_MAX - 2;
}

void showOpcodeEdit(uint8_t type, uint16_t value) {
  iconData[ICON_AUTO_ADD].bitmap = autoDef[type].bitmap;
  iconData[ICON_AUTO_ADD].color = autoDef[type].color;
  if (value == OBJ_NOT_FOUND)
    autoAddBuf[0] = '\0';
  else
    sprintf(autoAddBuf, "%d", value);
  showConvertValue(autoDef[type].opc, value);
}

void showConvertValue (char opc, uint16_t value) {
  uint16_t bus, mod, cnt;
  switch (opc) {
    case 'D':                                                                     // delay
      snprintf(autoConvBuf, ACC_LNG + 1, "%d.%ds", value / 10, value % 10);
      break;
    case 'Z':                                                                     // feedback
    case 'z':
      switch (wifiSetting.protocol) {
        case CLIENT_Z21:
          snprintf(autoConvBuf, ACC_LNG + 1, "%d.%d", (value >> 3) + 1, (value & 0x07) + 1);
          break;
        case CLIENT_LNET:
          snprintf(autoConvBuf, ACC_LNG + 1, "%d", value);
          break;
        case CLIENT_XNET:
          snprintf(autoConvBuf, ACC_LNG + 1, "%d.%d", (value >> 3) + 1, (value & 0x07) + 1);
          break;
        case CLIENT_ECOS:
          snprintf(autoConvBuf, ACC_LNG + 1, "%d.%d", (value >> 4) + 1, (value & 0x0F) + 1);
          break;
        case CLIENT_CS2:
          bus = value / 1000;
          value = value % 1000;
          mod = ((value - 1) / 16) + 1;
          cnt = (value & 0x000F) ? value % 16 : 16;
          snprintf(autoConvBuf, ACC_LNG + 1, "%d.%d.%d", bus, mod, cnt);
          break;
      }
      break;
    default:
      autoConvBuf[0] = '\0';
      break;
  }
}


void addEditOpcode () {
  //autoCurrEditPos: order in definition
  //addOpcode: add or edit
  //encoderValue: opcode selection autoDef[encoderValue].opc
  //autoAddBuf: value

  uint8_t ini, opcIndex;;
  uint16_t opcPos, iniPos, nxtPos, pos, value, lng, diff, blk, n;
  char opc;
  char newOpc[10];

  DEBUG_MSG("OPC:  %s", autoEditOpcodes)
  opcIndex = encoderValue;
  opc = autoDef[opcIndex].opc;
  value = atoi(autoAddBuf);
  switch (opcIndex) {
    case AUTO_r:
      sprintf(newOpc, "r");
      break;
    case AUTO_l0:
    case AUTO_l1:
    case AUTO_l2:
    case AUTO_l3:
      sprintf(newOpc, "l%d", opcIndex - AUTO_l0);
      break;
    default:
      sprintf(newOpc, "%c%d", opc, value);
      break;
  }
  pos = 0;
  ini = autoCurrEditPos;
  while (ini > 0) {
    getOpcode(autoEditOpcodes, &pos, &value);
    ini--;
  }
  opcPos = pos;                                                                   // position to add or edit
  lng = strlen(newOpc);
  if (addOpcode) {                                                                // add opcode: make room
    autoMakeRoom (opcPos, lng);
  }
  else {
    getOpcode(autoEditOpcodes, &pos, &value);
    diff = pos - opcPos;
    if (lng > diff) {                                                             // edit opcode, new is larger: make room
      autoMakeRoom (opcPos, lng - diff);
    }
    else {
      blk = MAX_LABEL_LNG - pos;
      for (n = 0; n < blk; n++) {                                                 // edit opcode, new is shorter: move data
        autoEditOpcodes[opcPos + n + lng] = autoEditOpcodes[pos + n];
      }
    }
  }
  for (n = 0; n < lng; n++)                                                       // put new opcode
    autoEditOpcodes[opcPos + n] = newOpc[n];
  autoEditOpcodes[MAX_LABEL_LNG] = '\0';
  DEBUG_MSG("EDIT: %s", autoEditOpcodes)
}


void autoMakeRoom (uint16_t opcPos, uint16_t lng) {
  uint16_t blk, n;
  blk = MAX_LABEL_LNG - opcPos - lng + 1;
  for (n = 0; n < blk; n++) {
    autoEditOpcodes[MAX_LABEL_LNG - n] = autoEditOpcodes[MAX_LABEL_LNG - n - lng];
  }
}

////////////////////////////////////////////////////////////
// ***** AUTOMATION *****
////////////////////////////////////////////////////////////

void initAutomation() {
  uint8_t n;
  for (n = 0; n < MAX_AUTO_SEQ; n++) {
    automation[n].num = 0;
    automation[n].currStep = 0;
    automation[n].opcode = 'X';
    automation[n].value = 0;
  }
  currAutomation = MAX_AUTO_SEQ;
  timerAutomation = millis();
}


uint8_t findAutomation (uint8_t num) {                                            // find file in running sequences
  uint8_t n;
  for (n = 0; n < MAX_AUTO_SEQ; n++)
    if (automation[n].num == num)
      return n;
  return MAX_AUTO_SEQ;
}


bool startAutomation (uint8_t seq) {
  bool fileOK;
  uint8_t num;
  DEBUG_MSG("Start %d: ", seq)
  if (findAutomation(seq) == MAX_AUTO_SEQ) {                                      // avoid same sequence running twice
    for (num = 0; num < MAX_AUTO_SEQ; num++) {
      if (automation[num].num == 0) {
        if (sdDetected)
          fileOK = loadAutomation(SD, seq);
        else
          fileOK = loadAutomation(LittleFS, seq);
        if (fileOK) {
          snprintf(automation[num].opcodes, MAX_LABEL_LNG + 1, "%s", autoOpcodes);
          automation[num].num = seq;
          automation[num].currStep = 0;
          automation[num].opcode = 'X';
          autoShowStatus(seq, true);
          return true;
        }
      }
    }
  }
  return false;                                                                   // already started or full
}


void stopAutomation (uint8_t seq) {
  uint8_t num;
  DEBUG_MSG("Stop %d: ", seq)
  num = findAutomation(seq);                                                      // stop only if running
  if (num != MAX_AUTO_SEQ) {
    automation[num].num = 0;
    automation[num].opcode = 'X';
    autoShowStatus(seq, false);
  }
}

void autoShowStatus (uint8_t seq, bool stat) {
  uint8_t n, aspect;
  if (isWindow(WIN_AUTOMATION)) {
    if ((seq > autoIndexStart) && (seq < (autoIndexStart + 6))) {
      n = seq - autoIndexStart - 1;
      fncData[FNC_AUTO0 + n].idIcon = stat ? FNC_ROUTE_RUN_OFF : FNC_ROUTE_OFF;
      newEvent(OBJ_FNC, FNC_AUTO0 + n, EVNT_DRAW);
    }
  }
  for (n = 0; n < 16; n++) {
    if (accPanel[n].type == ACC_ROUTE) {
      if (accPanel[n].addr == seq) {
        aspect = stat ? 1 : 0;
        accPanel[n].currAspect = aspect;
        fncData[FNC_ACC0 + n].idIcon = accDef[ACC_ROUTE].icon[aspect].fncIcon;
        fncData[FNC_ACC0 + n].color = accDef[ACC_ROUTE].icon[aspect].color;
        fncData[FNC_ACC0 + n].colorOn = accDef[ACC_ROUTE].icon[aspect].colorOn;
        if (isWindow(WIN_ACCESSORY))
          newEvent(OBJ_FNC, FNC_ACC0 + n, EVNT_DRAW);
      }
    }
  }
}


void automationProcess() {
  uint8_t pos, n;
  uint16_t param;

  if (millis() - timerAutomation > AUTO_INTERVAL) {                               // update timer for automation
    timerAutomation = millis();
    for (n = 0; n < MAX_AUTO_SEQ; n++) {                                          // check timed opcodes
      switch (automation[n].opcode) {
        case 'D':
        case 'L':
          if (automation[n].value > 0)
            automation[n].value--;
          break;
      }
    }
  }
  currAutomation++;
  if (currAutomation >= MAX_AUTO_SEQ)
    currAutomation = 0;
  if (automation[currAutomation].num > 0) {                                       // only runing sequences
    switch (automation[currAutomation].opcode) {                                  // check waiting opcodes (delay, feedback, loco change)
      case 'D':
      case 'L':
        if ((automation[currAutomation].value) > 0)                               // wait to complete delay
          return;
        break;
      case 'Z':
        if (automation[currAutomation].value == 0)                                // waiting for occupied
          return;
        break;
      case 'z':
        if (automation[currAutomation].value != 0)                                // waiting for free
          return;
        break;
      case 'C':                                                                   // waiting for route to end
        pos = automation[currAutomation].value;
        if ( pos != MAX_AUTO_SEQ) {
          if (automation[pos].num == automation[currAutomation].param)
            return;
        }
        break;
    }
    automation[currAutomation].opcode =  getOpcode(automation[currAutomation].opcodes, &automation[currAutomation].currStep, &automation[currAutomation].param);
    param = automation[currAutomation].param;
    switch (automation[currAutomation].opcode) {
      case 'L':                                                                   // loco address
        if (param > 0) {
          getNewLoco(param);                                                      // Z21,XNET,LNET: Loco addr, ECOS: Loco ID, CS2: Loco addr (if MFX be sure UID is set correctly)
          automation[currAutomation].value = AUTO_LOCO_CHG;
          if (isWindow(WIN_THROTTLE))
            newEvent(OBJ_WIN, WIN_THROTTLE, EVNT_DRAW);
        }
        break;
      case 'l':
        switch (param) {
          case 0:                                                                 // reverse
            locoData[myLocoData].myDir = 0x00;
            changeDirection();
            break;
          case 1:                                                                 // forward
            locoData[myLocoData].myDir = 0x80;
            changeDirection();
            break;
          case 2:                                                                 // change direction
            locoData[myLocoData].myDir ^= 0x80;
            changeDirection();
            break;
          case 3:                                                                 // E.Stop
            locoData[myLocoData].mySpeed = 1;
            locoOperationSpeed();
            break;
        }
        updateSpeedDir();
        break;
      case 'F':                                                                   // functions F0..F28
      case 'f':
        if (param < 29) {
          if (automation[currAutomation].opcode == 'F')
            bitSet(locoData[myLocoData].myFunc.Bits, param);
          else
            bitClear(locoData[myLocoData].myFunc.Bits, param);
          funcOperations(param);
          updateFuncState(isWindow(WIN_THROTTLE));
        }
        break;
      case 'S':                                                                   // speed 0..100%
        if (param <= 100)
          autoSetSpeed(param);                                                    // 0..100
        break;
      case 'D':                                                                   // delay
        automation[currAutomation].value = param;
        break;
      case 'T':
      case 't':
        if (param < 2048)
          moveAccessory (param, (uint8_t)(automation[currAutomation].opcode == 'T'));
        break;
      case 'r':                                                                 // loop
        automation[currAutomation].currStep = 0;
        break;
      case 'R':
        if ((param > 0) && (param <= AUTOMATION_MAX))
          startAutomation(param);                                               // route
        break;
      case 'C':                                                                 // call
        if ((param > 0) && (param <= AUTOMATION_MAX))
          automation[currAutomation].value = startAutomation(param) ? findAutomation(param) : MAX_AUTO_SEQ;
        break;
      case 'X':
        stopAutomation(automation[currAutomation].num);                         // end
        break;
      case 'Z':                                                                 // feedback
      case 'z':
        automation[currAutomation].value = (automation[currAutomation].opcode == 'Z') ? 0x0000 : 0xFFFF;
        queryFeedback(param);
        break;
    }
  }
}


void autoSetSpeed (uint16_t percent) {
  uint16_t spd;
  spd = (percent * 127) / 100;                                                  // 0..100 -> 0..127
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
    case CLIENT_XNET:
      if (bitRead(locoData[myLocoData].mySteps, 2))                             // 128 steps
        locoData[myLocoData].mySpeed = spd;
      else {
        if (bitRead(locoData[myLocoData].mySteps, 1)) {                         // 28 steps
          if (spd < 16)
            locoData[myLocoData].mySpeed = 0;
          else {
            locoData[myLocoData].mySpeed =  (spd >> 3);
            bitWrite(locoData[myLocoData].mySpeed, 4, bitRead(spd, 2));
          }
        }
        else {                                                                  // 14 steps
          locoData[myLocoData].mySpeed = spd >> 3;
        }
      }
      break;
    case CLIENT_LNET:
    case CLIENT_ECOS:
      locoData[myLocoData].mySpeed = spd;                                       // 0..127
      break;
    case CLIENT_CS2:
      locoData[myLocoData].mySpeed = percent;                                   // 0..100
      break;
  }
  locoOperationSpeed();
}

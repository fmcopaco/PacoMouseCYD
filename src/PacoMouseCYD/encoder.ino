/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

////////////////////////////////////////////////////////////
// ***** ENCODER *****
////////////////////////////////////////////////////////////

#ifdef ALTERNATE_ENCODER

IRAM_ATTR void encoderISR () {
  // Encoder interrupt routine for both pins. Updates counter
  // if they are valid and have rotated a full indent. Based on:
  //https://garrysblog.com/2021/03/20/reliably-debouncing-rotary-encoders-with-arduino-and-esp32/

  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // Encoder value
  static const int8_t enc_states[]  = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // Lookup table

  old_AB <<= 2;                                                       // Remember previous state

  if (digitalRead(ENCODER_A)) old_AB |= 0x02;                       // Add current state of pin A
  if (digitalRead(ENCODER_B)) old_AB |= 0x01;                       // Add current state of pin B

  encval += enc_states[( old_AB & 0x0F )];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if ( encval > 3 ) {                                               // Four steps
    encoderValue = (encoderValue > 0) ? --encoderValue : 0;                     // CCW, hasta 0
    encval -= 4;
    encoderChange = true;
  }
  else if ( encval < -3 ) {                                         // Four steps
    encoderValue = (encoderValue < encoderMax) ? ++encoderValue : encoderMax ;  // CW, hasta maximo
    encval += 4;
    encoderChange = true;
  }
}

#else

IRAM_ATTR void encoderISR () {                                    // Encoder interrupt
  encoderNeedService = true;
}

#endif

void encoderService () {                                          // Encoder interrupt service
  encoderNeedService = false;
  lastTimeEncoder = millis();
  outA = digitalRead (ENCODER_A);
  outB = digitalRead (ENCODER_B);
  if (outA != copyOutA) {                                         // evitamos rebotes
    copyOutA = outA;
    if (copyOutB == 0x80) {
      copyOutB = outB;
    }
    else {
      if ( outB != copyOutB) {
        copyOutB = 0x80;
        if (outA == outB)                                         // comprueba sentido de giro
          encoderValue = (encoderValue < encoderMax) ? ++encoderValue : encoderMax ;  // CW, hasta maximo
        else
          encoderValue = (encoderValue > 0) ? --encoderValue : 0;                     // CCW, hasta 0
        encoderChange = true;
      }
    }
  }
}

void readButtons () {
  byte inputButton;

  timeButtons = millis();                                         // lee cada cierto tiempo
  inputButton = digitalRead (ENCODER_SW);                         // comprueba cambio en boton del encoder
  if (statusSwitch != inputButton) {
    statusSwitch = inputButton;
    if (statusSwitch == LOW)
      switchOn = true;
  }
  inputButton = digitalRead (SW_BOOT_PIN);                            // comprueba cambio en boton BOOT
  if (statusBOOT != inputButton) {
    statusBOOT = inputButton;
    if (statusBOOT == LOW) {
      newEvent(OBJ_DATA, DATA_SW_BOOT, EVNT_PRESS);
      DEBUG_MSG("BOOT switch pressed...")
    }
  }
#if (USE_RGB_LED == FUNC_BUTTONS)
  inputButton = digitalRead (RGB_LED_R);                            // comprueba cambio en boton BOOT
  if (statusSW_R != inputButton) {
    statusSW_R = inputButton;
    if (statusSW_R == LOW) {
      newEvent(OBJ_DATA, DATA_SW_R, EVNT_PRESS);
      DEBUG_MSG("R switch pressed...")
    }
  }
  inputButton = digitalRead (RGB_LED_G);                            // comprueba cambio en boton BOOT
  if (statusSW_G != inputButton) {
    statusSW_G = inputButton;
    if (statusSW_G == LOW) {
      newEvent(OBJ_DATA, DATA_SW_G, EVNT_PRESS);
      DEBUG_MSG("G switch pressed...")
    }
  }
  inputButton = digitalRead (RGB_LED_B);                            // comprueba cambio en boton BOOT
  if (statusSW_B != inputButton) {
    statusSW_B = inputButton;
    if (statusSW_B == LOW) {
      newEvent(OBJ_DATA, DATA_SW_B, EVNT_PRESS);
      DEBUG_MSG("B switch pressed...")
    }
  }
#endif
}


void controlEncoder() {                                           // encoder movement
  encoderChange = false;
  aliveAndKicking();
  DEBUG_MSG("Encoder: %d", encoderValue);
  switch (objStack[lastWinStack].objID) {
    case WIN_SSID:
      scrSSID = encoderValue;
      scanWiFiFill();
      drawObject(OBJ_TXT, TXT_SSID1);
      drawObject(OBJ_TXT, TXT_SSID2);
      drawObject(OBJ_TXT, TXT_SSID3);
      drawObject(OBJ_TXT, TXT_SSID4);
      drawObject(OBJ_TXT, TXT_SSID5);
      drawObject(OBJ_TXT, TXT_SSID6);
      break;
    case WIN_THROTTLE:
    case WIN_SPEEDO:
    case WIN_ACCESSORY:
    case WIN_PANELS:
      updateMySpeed();
      break;
    case WIN_STA_PLAY:
      updateMySpeed();
      gaugeData[GAUGE_STATION].value = map(encoderValue, 0, encoderMax, 0, 255);
      drawObject(OBJ_GAUGE, GAUGE_STATION);
      break;
    case WIN_CHG_FUNC:
      fncData[FNC_CHG].idIcon = encoderValue * 2;
      drawObject(OBJ_FNC, FNC_CHG);
      break;
    case WIN_SEL_LOCO:
      populateLocoList();
      drawObject(OBJ_TXT, TXT_SEL_ADDR1);
      drawObject(OBJ_TXT, TXT_SEL_NAME1);
      drawObject(OBJ_TXT, TXT_SEL_ADDR2);
      drawObject(OBJ_TXT, TXT_SEL_NAME2);
      drawObject(OBJ_TXT, TXT_SEL_ADDR3);
      drawObject(OBJ_TXT, TXT_SEL_NAME3);
      drawObject(OBJ_TXT, TXT_SEL_ADDR4);
      drawObject(OBJ_TXT, TXT_SEL_NAME4);
      drawObject(OBJ_TXT, TXT_SEL_ADDR5);
      drawObject(OBJ_TXT, TXT_SEL_NAME5);
      drawObject(OBJ_TXT, TXT_SEL_ADDR6);
      drawObject(OBJ_TXT, TXT_SEL_NAME6);
      break;
    case WIN_STEAM:
      showSpeedSteam((encoderValue << 1) + 240);
      break;
    case WIN_ACC_TYPE:
      fncData[FNC_ACC_TYPE].num = accDef[encoderValue].num;
      fncData[FNC_ACC_TYPE].idIcon = accDef[encoderValue].icon[0].fncIcon;
      fncData[FNC_ACC_TYPE].color = accDef[encoderValue].icon[0].color;
      fncData[FNC_ACC_TYPE].colorOn = accDef[encoderValue].icon[0].colorOn;
      drawObject(OBJ_FNC, FNC_ACC_TYPE);
      break;
  }
}


void controlSwitch() {                                            // encoder switch
  uint16_t value, value2, txtID;
  uint32_t delta, dist;
  char msg[NAME_LNG + 1];
  switchOn = false;
  aliveAndKicking();
  DEBUG_MSG("Encoder Switch");
  switch (objStack[lastWinStack].objID) {
    case WIN_SSID:
      snprintf (wifiSetting.ssid, 33, WiFi.SSID(scrSSID).c_str());     //saveSSID(scrSSID);
      DEBUG_MSG("New SSID: %s", wifiSetting.ssid);
      eepromChanged = true;
      closeWindow(WIN_SSID);
      openWindow(WIN_WIFI);
      break;
    case WIN_THROTTLE:
    case WIN_STA_PLAY:
    case WIN_ACCESSORY:
    case WIN_PANELS:
      if (encoderValue > 0) {
        encoderValue = 0;
        if (stopMode > 0)
          locoData[myLocoData].mySpeed = 1;
        else
          locoData[myLocoData].mySpeed = 0;
        locoOperationSpeed();
      }
      else {
        locoData[myLocoData].myDir ^= 0x80;
        changeDirection();
      }
      updateSpeedDir();
      break;
    case WIN_CHG_FUNC:
      fncData[FNC_F0 + paramChild].idIcon = fncData[FNC_CHG].idIcon;
      closeWindow(WIN_CHG_FUNC);
      break;
    case WIN_SEL_LOCO:
      releaseLoco();
      txtID = (encoderValue > 5) ? 5 : encoderValue;
      if (useID) {
        value2 = (encoderValue > 5) ? encoderValue - 5 : 0;
        value = sortedLocoStack[value2 + txtID];
      }
      else {
        value = atoi(txtData[TXT_SEL_ADDR1 + txtID].buf);
      }
      //value = atoi(txtData[TXT_SEL_ADDR1 + txtID].buf);
      DEBUG_MSG("Selected Loco %d", value);
      closeWindow(WIN_SEL_LOCO);
      getNewLoco(value);
      break;
    case WIN_SPEEDO:
      switch (speedoPhase) {                                      //enum speedo {SPD_WAIT, SPD_BEGIN, SPD_COUNT, SPD_ARRIVE, SPD_END};
        case SPD_WAIT:
          if (getCurrentStep() > 0) {
            speedoStartTime = millis();
            setSpeedoPhase(SPD_BEGIN);
            getLabelTxt(LBL_MEASURE, msg);
            snprintf(spdSpeedBuf, NAME_LNG + 1, "%s", msg);
            drawObject(OBJ_TXT, TXT_SPEEDO_SPD);
            setTimer(TMR_SPEEDO, 5, TMR_ONESHOT);
          }
          else {
            locoData[myLocoData].myDir ^= 0x80;
            changeDirection();
            updateSpeedDir();
          }
          break;
        case SPD_BEGIN:
        case SPD_COUNT:
          speedoEndTime = millis();
          setSpeedoPhase(SPD_ARRIVE);
          setTimer(TMR_SPEEDO, 5, TMR_ONESHOT);
          dist = speedoLength * 36 * speedoScale;
          delta = (speedoEndTime - speedoStartTime) * 10;
          speedoSpeed = dist / delta;
          snprintf(spdSpeedBuf, NAME_LNG + 1, "%d km/h", speedoSpeed);
          drawObject(OBJ_TXT, TXT_SPEEDO_SPD);
          break;
        case SPD_ARRIVE:
          break;
        case SPD_END:
          break;
      }
      break;
    case WIN_STEAM:
      steamThrottleStop();
      currentSteamSpeed = 0;
      locoData[myLocoData].mySpeed = 0;
      locoOperationSpeed();
      break;
    case WIN_ACC_TYPE:
      accTypeClick();
      break;
  }
}


void setActionOption(uint8_t actionSW, uint8_t pos) {
  if (actionSW < ACT_F0)
    getLabelOption(LBL_ACTIONS, keybActionBuf[pos], actionSW);
  else
    snprintf(keybActionBuf[pos], NAME_LNG + 1, "F%d", actionSW - ACT_F0);
}

uint8_t getActionOption(uint16_t adr) {
  uint8_t actionSW;
  actionSW = EEPROM.read(adr);                      // get button action
  if (actionSW > ACT_F28)
    actionSW = (adr == EE_ACT_BOOT) ? ACT_CALIBRATE : ACT_UNDEF;
  return actionSW;
}

void doAction(uint8_t actionSW) {
  uint8_t n;
  uint16_t value;
  DEBUG_MSG("ACTION: %d", actionSW)
  switch (actionSW) {
    case ACT_UNDEF:
      n = 0;
      break;
    case ACT_CALIBRATE:
      startCalibration();
      break;
    case ACT_STOP:
      if (notLocked())
        togglePower();
      break;
    case ACT_SHUNTING:
      shuntingMode = !shuntingMode;
      if (isWindow(WIN_THROTTLE))
        updateSpeedDir();
      break;
    case ACT_NEXT:
      if (isWindow(WIN_THROTTLE))
        showNextFuncBlock();
      if (isWindow(WIN_STEAM)) {
        if (barData[BAR_JOHNSON].value < 6) {
          barData[BAR_JOHNSON].value++;
          newEvent(OBJ_BAR, BAR_JOHNSON, EVNT_DRAW);
        }
      }
      if (isWindow(WIN_ACCESSORY)) {
        if ((!editAccessory) && (currPanel < 15)) {
          if (accPanelChanged) {
            accPanelChanged = false;
            if (sdDetected)
              saveCurrAccPanel(SD);
            else
              saveCurrAccPanel(LittleFS);
          }
          saveCurrentAspects();
          currPanel++;
          populateAccPanel();
          newEvent(OBJ_WIN, WIN_ACCESSORY, EVNT_DRAW);
        }
      }
      if (isWindow(WIN_SCREEN)) {
        value = (barData[BAR_BLIGHT].value < 0xF0) ? barData[BAR_BLIGHT].value + 0x10 : 0xFF;
        barData[BAR_BLIGHT].value = value;
        setBacklight (value);
        backlight = value;
        newEvent(OBJ_BAR, BAR_BLIGHT, EVNT_DRAW);
      }
      break;
    case ACT_PREV:
      if (isWindow(WIN_THROTTLE))
        showPrevFuncBlock();
      if (isWindow(WIN_STEAM)) {
        if (barData[BAR_JOHNSON].value > 0) {
          barData[BAR_JOHNSON].value--;
          newEvent(OBJ_BAR, BAR_JOHNSON, EVNT_DRAW);
        }
      }
      if (isWindow(WIN_ACCESSORY)) {
        if ((!editAccessory) && (currPanel > 0)) {
          if (accPanelChanged) {
            accPanelChanged = false;
            if (sdDetected)
              saveCurrAccPanel(SD);
            else
              saveCurrAccPanel(LittleFS);
          }
          saveCurrentAspects();
          currPanel--;
          populateAccPanel();
          newEvent(OBJ_WIN, WIN_ACCESSORY, EVNT_DRAW);
        }
      }
      if (isWindow(WIN_SCREEN)) {
        value = (barData[BAR_BLIGHT].value > (USER_MIN_BL + 0x0F)) ? barData[BAR_BLIGHT].value - 0x10 : USER_MIN_BL;
        barData[BAR_BLIGHT].value = value;
        setBacklight (value);
        backlight = value;
        newEvent(OBJ_BAR, BAR_BLIGHT, EVNT_DRAW);
      }
      break;
    default:                                        // Function key
      n = actionSW - ACT_F0;
      locoData[myLocoData].myFunc.Bits ^= bit(n);
      funcOperations(n);
      updateFuncState(isWindow(WIN_THROTTLE));
      break;
  }
}

uint8_t nextAction (uint8_t actionSW, uint8_t pos, uint16_t txt) {
  if (actionSW < ACT_F28) {
    actionSW++;
    setActionOption(actionSW, pos);
    newEvent(OBJ_TXT, txt, EVNT_DRAW);
  }
  return actionSW;
}

uint8_t prevAction (uint8_t actionSW, uint8_t pos, uint16_t txt) {
  if (actionSW > 0) {
    actionSW--;
    setActionOption(actionSW, pos);
    newEvent(OBJ_TXT, txt, EVNT_DRAW);
  }
  return actionSW;
}

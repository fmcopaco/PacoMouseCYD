/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

////////////////////////////////////////////////////////////
// ***** ENCODER *****
////////////////////////////////////////////////////////////

IRAM_ATTR void encoderISR () {                                    // Encoder interrupt
  encoderNeedService = true;
}


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
    case WIN_STA_PLAY:
      updateMySpeed();
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
      snprintf (wifiSetting.ssid, 32, WiFi.SSID(scrSSID).c_str());     //saveSSID(scrSSID);
      DEBUG_MSG("New SSID: %s", wifiSetting.ssid);
      eepromChanged = true;
      closeWindow(WIN_SSID);
      openWindow(WIN_WIFI);
      break;
    case WIN_THROTTLE:
    case WIN_STA_PLAY:
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

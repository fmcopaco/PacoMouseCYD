/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

////////////////////////////////////////////////////////////
// ***** EVENTS *****
////////////////////////////////////////////////////////////

void eventProcess() {
  char chr;
  uint16_t txtID, lng, n;
  int value, value2, value3, value4;
  wEvent event;
  event = getEvent();
  switch (event.eventID) {
    case EVNT_TIMER:
      DEBUG_MSG("Timer event");
      switch (event.objID) {
        case TMR_BLIGHT:
          setBacklight(SYS_MIN_BL);
          break;
        case TMR_END_LOGO:
          closeWindow(WIN_LOGO);
          openWindow(WIN_THROTTLE);
          switch (initStatus) {
            case INIT_NO_WIFI:
              openWindow(WIN_SSID);
              break;
            case INIT_NO_CONNECT:
              openWindow(WIN_WIFI);
              break;
            case INIT_NO_SD:
            case INIT_OK:
              infoLocomotora(locoData[myLocoData].myAddr.address);
              break;
          }
          break;
        case TMR_POWER:
          if (iconData[ICON_POWER].color == COLOR_RED)
            iconData[ICON_POWER].color = COLOR_PINK;
          else
            iconData[ICON_POWER].color = COLOR_RED;
          if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
            newEvent(OBJ_ICON, ICON_POWER, EVNT_DRAW);
          break;
        case TMR_ACCESSORY:
          sendAccessoryFIFO();
          break;
        case TMR_SPEEDO:
          switch (speedoPhase) {
            case SPD_BEGIN:
              setSpeedoPhase(SPD_COUNT);
              break;
            case SPD_ARRIVE:
              setSpeedoPhase(SPD_END);
              setTimer(TMR_SPEEDO, 10, TMR_ONESHOT);
              break;
            case SPD_END:
              setSpeedoPhase(SPD_WAIT);
              break;
          }
          break;
        case TMR_INFO:
          if (useID)
            infoLocomotora(locoData[myLocoData].myLocoID);
          else
            infoLocomotora(locoData[myLocoData].myAddr.address);
          break;
        case TMR_WAIT:
          barData[BAR_WAIT].value += 20;
          if (barData[BAR_WAIT].value >= 100) {
            n = barData[BAR_WAIT].colorOn;
            barData[BAR_WAIT].colorOn = barData[BAR_WAIT].colorOff;
            barData[BAR_WAIT].colorOff = n;
            barData[BAR_WAIT].value = 0;
          }
          if (isWindow(WIN_ALERT)) {
            newEvent(OBJ_BAR, BAR_WAIT, EVNT_DRAW);
            setTimer(TMR_WAIT, 5, TMR_ONESHOT);
          }
          break;
        case TMR_STEAM:
          showSpeedSteam((encoderValue << 1) + 240);
          showPressure(steamPressure * 270 / 100);
          break;
        case TMR_SCAN:
          wifiAnalyzer();
          break;
        case TMR_FNC_ECOS:
          if (isWindow(WIN_THROTTLE))
            showFuncBlock(0);
          break;
        case TMR_STA_RUN:
          if (isWindow(WIN_STA_PLAY)) {
            if (staCurrTime > 0) {
              staCurrTime--;
              updateStationTime(staCurrTime);
              newEvent(OBJ_TXT, TXT_STA_TIME, EVNT_DRAW);
            }
            else {
              stopTimer(TMR_STA_RUN);
              closeWindow(WIN_STA_PLAY);/*
              updateStationTime(staTime);
              updateTargetStations();
*/
              openWindow(WIN_STA_STARS);
            }
          }
          else
            stopTimer(TMR_STA_RUN);
          break;
      }
      break;
    case EVNT_DRAW:
      if (event.objType == OBJ_WIN)
        drawWindow(event.objID);
      else
        drawObject(event.objType, event.objID);
      break;
    case EVNT_WOPEN:
      if (event.objType == OBJ_WIN) {
        openWindow(event.objID);
      }
      break;
    case EVNT_WCLOSE:
      if (event.objType == OBJ_WIN) {
        closeWindow(event.objID);
      }
      break;
    case EVNT_BOOT:
      calibrationPending = true;
      newEvent(OBJ_WIN, WIN_CALIBRATE, EVNT_WOPEN);
      break;
    case EVNT_CLICK:
      aliveAndKicking();
      switch (event.objType) {
        case OBJ_WIN:
          switch (event.objID) {
            case WIN_CALIBRATE:
              if (calibrationPending) {                     // only valid click if calibration pending
                calibrateTouchscreen(COLOR_CYAN, COLOR_NAVY, COLOR_BLACK);
                objStack[posObjStack1].objType = OBJ_BUTTON;                       // change text with OK button
                objStack[posObjStack1].objID = BUT_CAL_OK;
                objStack[posObjStack2].objID = LBL_CAL_DONE;
                newEvent(OBJ_WIN, WIN_CALIBRATE, EVNT_DRAW);
                calibrationPending = false;
                DEBUG_MSG("Calibrated!")
              }
              break;
            case WIN_ALERT:
              switch (errType) {
                case ERR_FULL:
                case ERR_CHG_WIFI:
                  closeWindow(WIN_ALERT);
                  break;
                case ERR_STOP:
                case ERR_CV:
                  closeWindow(WIN_ALERT);
                  resumeOperations();
                  break;
              }
              break;
            case WIN_ABOUT:
              closeWindow(WIN_ABOUT);
              break;
            case WIN_STA_STARS:
              if (staCurrTime == 0)
                newStationCounters(true);
              updateStationTime(staTime);
              updateTargetStations();
              updateStationLevel();
              updateStationStars();
              closeWindow(WIN_STA_STARS);
              break;
          }
          break;
        case OBJ_BUTTON:
          txtID = 0;
          switch (event.objID) {
            case BUT_CAL_OK:
              closeWindow(WIN_CALIBRATE);
              bootPressed = false;
              saveCalibrationValues();                      // save calibration values
              DEBUG_MSG("Calibration window closed")
              break;
            case BUT_SSID_CLOSE:
              closeWindow(WIN_SSID);
              break;
            case BUT_WIFI_OK:
              value  = atoi(keybIP1Buf);
              value2 = atoi(keybIP2Buf);
              value3 = atoi(keybIP3Buf);
              value4 = atoi(keybIP4Buf);
              if ((value > 255) || (value2 > 255) || (value3 > 255) || (value4 > 255)) {
                for (n = 0; n < 4; n++) {
                  txtData[TXT_IP1 + n].backgnd = COLOR_PINK;
                  drawObject(OBJ_TXT, TXT_IP1 + n);
                }
              }
              else {
                n = atoi(keybPortBuf);
                if ((wifiSetting.port != n) || (wifiSetting.CS_IP[0] != value) || (wifiSetting.CS_IP[1] != value2) || (wifiSetting.CS_IP[2] != value3) || (wifiSetting.CS_IP[3] != value4)) {
                  eepromChanged = true;
                  wifiSetting.port = n;
                  wifiSetting.CS_IP[0] = value;
                  wifiSetting.CS_IP[1] = value2;
                  wifiSetting.CS_IP[2] = value3;
                  wifiSetting.CS_IP[3] = value4;
                  DEBUG_MSG("NewIP: %d.%d.%d.%d - Port: %d", wifiSetting.CS_IP[0], wifiSetting.CS_IP[1], wifiSetting.CS_IP[2], wifiSetting.CS_IP[3], wifiSetting.port)
                  EEPROM.put(EE_WIFI, wifiSetting);
                }
                closeWindow(WIN_WIFI);
                if (eepromChanged) {
                  EEPROM.commit();
                  eepromChanged = false;
                  alertWindow(ERR_CHG_WIFI);
                  DEBUG_MSG("Saving WiFi changes in EEPROM")
                }
              }
              break;
            case BUT_PWD_CNCL:
              closeWindow(WIN_WIFI_PWD);
              break;
            case BUT_PWD_OK:
              snprintf (wifiSetting.password, 64, "%s", keybPwdBuf);    //savePassword();
              EEPROM.put(EE_WIFI, wifiSetting);
              eepromChanged = true;
              DEBUG_MSG("New password: %s", wifiSetting.password);
              closeWindow(WIN_WIFI_PWD);
              break;
            case BUT_PROT_OK:
              closeWindow(WIN_PROTOCOL);
              break;
            case BUT_OPT_OK:
              if ((shortAddress != EEPROM.read(EE_SHORT)) || (typeCmdStation != EEPROM.read(EE_CMD_STA)) || (autoIdentifyCS != EEPROM.read(EE_CMD_AUTO))) {
                EEPROM.write(EE_SHORT, shortAddress);
                EEPROM.write(EE_CMD_STA, typeCmdStation);
                EEPROM.write(EE_CMD_AUTO, autoIdentifyCS);
                eepromChanged = true;
              }
              closeWindow(WIN_OPTIONS);
              break;
            case BUT_SCR_OK:
              locationUSB = (switchData[SW_ROTATE].state) ? USB_UP : USB_DOWN;
              setRotationDisplay(locationUSB);
              EEPROM.write(EE_USB_LOCATION, locationUSB);
              EEPROM.write(EE_BACKLIGHT, backlight);
              eepromChanged = true;
              closeWindow(WIN_SCREEN);
              break;
            case BUT_SCR_CNCL:
              backlight = EEPROM.read(EE_BACKLIGHT);
              if (backlight < USER_MIN_BL)
                backlight = USER_MIN_BL;
              setBacklight (backlight);
              closeWindow(WIN_SCREEN);
              break;
            case BUT_SPD_OK:
              value = 0;
              if ((radioData[RAD_STOP_MODE].value > 0) && (stopMode == 0))
                value++;
              if ((radioData[RAD_STOP_MODE].value == 0) && (stopMode > 0))
                value++;
              if ((shuntingMode != switchData[SW_SHUNTING].state) || (value > 0)) {
                shuntingMode = switchData[SW_SHUNTING].state;
                stopMode = radioData[RAD_STOP_MODE].value;
                EEPROM.write(EE_SHUNTING, shuntingMode);
                EEPROM.write(EE_STOP_MODE, stopMode);
                eepromChanged = true;
                DEBUG_MSG("Changes in Speed Options");
              }
              closeWindow(WIN_SPEED);
              break;
            case BUT_LOCK:
              value = 0;
              if (switchData[SW_LOCK_LOK].state)
                bitSet(value, LOCK_SEL_LOCO);
              if (switchData[SW_LOCK_ACC].state)
                bitSet(value, LOCK_TURNOUT);
              if (switchData[SW_LOCK_PRG].state)
                bitSet(value, LOCK_PROG);
              if ((byte)value != lockOptions) {
                lockOptions = (byte)value;
                EEPROM.write(EE_LOCK, lockOptions);
                eepromChanged = true;
                DEBUG_MSG("Changed LOCK options: %d", lockOptions)
              }
              closeWindow(WIN_LOCK);
              break;
            case BUT_CFG_TOUCH:
              bootPressed = true;
              newEvent(OBJ_WIN, WIN_CALIBRATE, EVNT_BOOT);
              break;
            case BUT_CLOCK_CNCL:
              closeWindow(WIN_SET_CLOCK);
              break;
            case BUT_CLOCK_OK:
              value  = atoi(keybHourBuf);
              value2 = atoi(keybMinBuf);
              value3 = atoi(keybRateBuf);
              value4 = (wifiSetting.protocol == CLIENT_LNET) ? 127 : 63;
              if ((value > 23) || (value2 > 59) || (value3 > value4)) {
                for (n = 0; n < 3; n++) {
                  txtData[TXT_HOUR + n].backgnd = COLOR_PINK;
                  drawObject(OBJ_TXT, TXT_HOUR + n);
                }
              }
              else {
                setTime(value, value2, value3);
                closeWindow(WIN_SET_CLOCK);
              }
              break;
            case BUT_EDIT_CNCL:
              updateSpeedHID();
              closeWindow(WIN_LOK_EDIT);
              break;
            case BUT_EDIT_OK:
              snprintf (locoData[myLocoData].myName, NAME_LNG + 1, locoEditName);
              for (n = 0; n < 29; n++)
                locoData[myLocoData].myFuncIcon[n] = fncData[FNC_F0 + n].idIcon;
              locoData[myLocoData].myVmax = atoi(locoEditVmax);
              if (wifiSetting.protocol != CLIENT_ECOS)
                locoData[myLocoData].myLocoID = atoi(locoEditID);
              loadThrottleData();
              updateSpeedHID();
              //locoData[myLocoData].myVmax = atoi(locoEditVmax);
              if (sdDetected)
                saveLocoData(SD, myLocoData);
              else
                saveLocoData(LittleFS, myLocoData);
              closeWindow(WIN_LOK_EDIT);
              break;
            case BUT_NAME_CNCL:
              if (isWindow(WIN_EDIT_NAME))
                closeWindow(WIN_EDIT_NAME);
              if (isWindow(WIN_ACC_NAME))
                closeWindow(WIN_ACC_NAME);
              if (isWindow(WIN_PANEL_NAME))
                closeWindow(WIN_PANEL_NAME);
              break;
            case BUT_NAME_OK:
              if (isWindow(WIN_EDIT_NAME)) {
                snprintf(locoEditName, NAME_LNG + 1, keybNameBuf);
                closeWindow(WIN_EDIT_NAME);
              }
              if (isWindow(WIN_ACC_NAME)) {
                snprintf(accKeybName, ACC_LNG + 1, keybNameBuf);
                snprintf(currAccEdit.accName, ACC_LNG + 1, keybNameBuf);
                closeWindow(WIN_ACC_NAME);
              }
              if (isWindow(WIN_PANEL_NAME)) {
                snprintf(panelNameBuf, PANEL_LNG + 1, keybNameBuf);
                snprintf(panelNamesBuf[currPanel], PANEL_LNG + 1, keybNameBuf);
                if (sdDetected)
                  saveAccPanelNames(SD);
                else
                  saveAccPanelNames(LittleFS);
                closeWindow(WIN_PANEL_NAME);
              }
              break;
            case BUT_EDIT_FUNC:
              openWindow(WIN_FUNC);
              break;
            case BUT_FNC_CNCL:
              for (n = 0; n < 29; n++)                                    // reload original icons
                fncData[FNC_F0 + n].idIcon = locoData[myLocoData].myFuncIcon[n];
              closeWindow(WIN_FUNC);
              break;
            case BUT_FNC_OK:
              closeWindow(WIN_FUNC);
              break;
            case BUT_IMAGE_CNCL:
              closeWindow(WIN_SEL_IMAGE);
              break;
            case BUT_MENU_I_DRIVE:
            case BUT_MENU_T_DRIVE:
              updateSpeedHID();
              closeWindow(WIN_MENU);
              getStatusCS();
              break;
            case BUT_MENU_I_ACC:
            case BUT_MENU_T_ACC:
              if (notLockedOption(LOCK_TURNOUT)) {
                closeWindow(WIN_MENU);
                openWindow(WIN_ACCESSORY);
              }
              break;
            case BUT_MENU_I_CV:
            case BUT_MENU_T_CV:
              if (notLockedOption(LOCK_PROG)) {
                if ((CVaddress > 1024) || (CVaddress == 0))
                  CVaddress = 8;
                enterCVdata = false;
                openWindow(WIN_PROG_CV);
              }
              break;
            case BUT_MENU_I_CFG:
            case BUT_MENU_T_CFG:
              closeWindow(WIN_MENU);
              openWindow(WIN_CONFIG);
              break;
            case BUT_MENU_I_UTILS:
            case BUT_MENU_T_UTILS:
              closeWindow(WIN_MENU);
              openWindow(WIN_UTIL);
              break;
            case BUT_CFG_I_LANG:
            case BUT_CFG_T_LANG:
              currLanguage++;
              if (currLanguage >= MAX_LANG)
                currLanguage = LANG_ENGLISH;
              buttonData[BUT_CFG_I_LANG].objID = DSTR_ENGLISH + currLanguage;
              newEvent(OBJ_WIN, WIN_CONFIG, EVNT_DRAW);
              break;
            case BUT_CFG_I_SCR:
            case BUT_CFG_T_SCR:
              openWindow(WIN_SCREEN);
              break;
            case BUT_CFG_I_SPD:
            case BUT_CFG_T_SPD:
              openWindow(WIN_SPEED);
              break;
            case BUT_CFG_I_WIFI:
            case BUT_CFG_T_WIFI:
              openWindow(WIN_WIFI);
              break;
            case BUT_CFG_I_FCLK:
            case BUT_CFG_T_FCLK:
              openWindow(WIN_SET_CLOCK);
              break;
            case BUT_CFG_I_LOCK:
            case BUT_CFG_T_LOCK:
              openWindow(WIN_LOCK);
              break;
            case BUT_CFG_I_ABOUT:
            case BUT_CFG_T_ABOUT:
              openWindow(WIN_ABOUT);
              break;
            case BUT_OPTIONS:
              if (wifiSetting.protocol != CLIENT_ECOS)
                openWindow(WIN_OPTIONS);
              break;
            case BUT_SPEEDO_CNCL:
              stopTimer(TMR_SPEEDO);
              closeWindow(WIN_SPEEDO);
              break;
            case BUT_SPEEDO_H0:
              setScaleSpeedo(87);
              break;
            case BUT_SPEEDO_N:
              setScaleSpeedo(160);
              break;
            case BUT_SPEEDO_TT:
              setScaleSpeedo(120);
              break;
            case BUT_SPEEDO_Z:
              setScaleSpeedo(220);
              break;
            case BUT_SPEEDO_0:
              setScaleSpeedo(45);
              break;
            case BUT_SPEEDO_CV:
              if (notLockedOption(LOCK_PROG)) {
                modeProg = true;                      // program CV5 in PoM
                enterCVdata = true;
                CVaddress = 5;
                openWindow(WIN_PROG_CV);
              }
              break;
            case BUT_CV_READ:
              if (!modeProg)
                openWindow(WIN_READ_CV);
              break;
            case BUT_CV_CNCL:
              closeWindow(WIN_PROG_CV);
              exitProgramming();
              setTimer(TMR_INFO, 5, TMR_ONESHOT);
              break;
            case BUT_CV_ADDR:
              readBasicCV(1);
              break;
            case BUT_CV_SPD_L:
              readBasicCV(2);
              break;
            case BUT_CV_SPD_M:
              readBasicCV(6);
              break;
            case BUT_CV_SPD_H:
              readBasicCV(5);
              break;
            case BUT_CV_ACC:
              readBasicCV(3);
              break;
            case BUT_CV_DEC:
              readBasicCV(4);
              break;
            case BUT_CV_CFG:
              readBasicCV(29);
              break;
            case BUT_CV_MAN:
              readBasicCV(8);
              break;
            case BUT_CV_0:
            case BUT_CV_1:
            case BUT_CV_2:
            case BUT_CV_3:
            case BUT_CV_4:
            case BUT_CV_5:
            case BUT_CV_6:
            case BUT_CV_7:
              CVdata = (CVdata ^ bit(event.objID - BUT_CV_0)) & 0x00FF;
              enterCVdata = true;
              setFieldsCV();
              setBitsCV();
              showFieldsCV();
              break;
            case BUT_ADDR_CNCL:
              closeWindow(WIN_PROG_ADDR);
              setStatusCV();

              break;
            case BUT_UTL_I_SPEEDO:
            case BUT_UTL_T_SPEEDO:
              closeWindow(WIN_UTIL);
              openWindow(WIN_SPEEDO);
              updateSpeedHID();
              setTimer(TMR_INFO, 5, TMR_ONESHOT);
              break;
            case BUT_UTL_I_STEAM:
            case BUT_UTL_T_STEAM:
              initSteamThrottle();
              closeWindow(WIN_UTIL);
              openWindow(WIN_STEAM);
              break;
            case BUT_UTL_I_SCAN:
            case BUT_UTL_T_SCAN:
              openWindow(WIN_WIFI_SCAN);
              break;
            case BUT_UTL_I_STA:
            case BUT_UTL_T_STA:
              newStationCounters(true);
              openWindow(WIN_STA_RUN);
              break;
            case BUT_STEAM_CNCL:
              updateSpeedHID();                 // set encoder
              closeWindow(WIN_STEAM);
              break;
            case BUT_EDIT_DEL:
              alertWindow(ERR_ASK_SURE);
              break;
            case BUT_SURE_CNCL:
              closeWindow(WIN_ALERT);
              break;
            case BUT_SURE_OK:                   // remove loco from system
              closeWindow(WIN_ALERT);
              if (isWindow(WIN_LOK_EDIT)) {
                closeWindow(WIN_LOK_EDIT);
                n = locoData[myLocoData].myAddr.address;
                clearLocoData(myLocoData);
                popLoco(n);
                if (sdDetected)
                  deleteLocoData(SD, n);
                else
                  deleteLocoData(LittleFS, n);
                getNewLoco(locoStack[0]);
              }
              if (isWindow(WIN_ACC_TYPE)) {
                editAccessory = false;
                accPanelChanged = true;
                winData[WIN_ACCESSORY].backgnd = COLOR_WHITE;
                deleteAccPanelElement(paramChild);
                updateAccPanel();
                updateSpeedHID();                 // set encoder
                closeWindow(WIN_ACC_TYPE);
              }
              break;
            case BUT_CV_LNCV:
              artNum = 0;
              modNum = 1;
              numLNCV = 0;
              valLNCV = 1;
              optLNCV = LNCV_ART;
              openWindow(WIN_PROG_LNCV);
              break;
            case BUT_LNCV_CNCL:
              //if (optLNCV != LNCV_ART)
              numLNCV = 0;
              valLNCV = modNum;
              sendLNCV(LNCV_REQID_CFGREQUEST, LNCV_FLAG_PROFF);
              closeWindow(WIN_PROG_LNCV);
              break;
            case BUT_LNCV_FIND:
              artNum = 65535;
              numLNCV = 0;
              valLNCV = 65535;
              optLNCV = LNCV_MOD;
              setFieldsLNCV();
              showFieldsLNCV();
              sendLNCV(LNCV_REQID_CFGREQUEST, 0);
              break;
            case BUT_ACC_CNCL:
              if (accPanelChanged) {
                accPanelChanged = false;
                if (sdDetected)
                  saveCurrAccPanel(SD);
                else
                  saveCurrAccPanel(LittleFS);
              }
              closeWindow(WIN_ACCESSORY);
              break;
            case BUT_ACC_EDIT:
              if (notLockedOption(LOCK_PROG)) {
                editAccessory = !editAccessory;
                winData[WIN_ACCESSORY].backgnd = editAccessory ? COLOR_PINK : COLOR_WHITE;
                newEvent(OBJ_WIN, WIN_ACCESSORY, EVNT_DRAW);
              }
              break;
            case BUT_ACC_0:
            case BUT_ACC_1:
            case BUT_ACC_2:
            case BUT_ACC_3:
            case BUT_ACC_4:
            case BUT_ACC_5:
            case BUT_ACC_6:
            case BUT_ACC_7:
            case BUT_ACC_8:
            case BUT_ACC_9:
            case BUT_ACC_10:
            case BUT_ACC_11:
            case BUT_ACC_12:
            case BUT_ACC_13:
            case BUT_ACC_14:
            case BUT_ACC_15:
              accPanelClick(event.objID - BUT_ACC_0);
              break;
            case BUT_ACC_ASPECT0:
            case BUT_ACC_ASPECT1:
            case BUT_ACC_ASPECT2:
            case BUT_ACC_ASPECT3:
              accAspectClick(event.objID - BUT_ACC_ASPECT0);
              closeWindow(WIN_ACC_ASPECT);
              break;
            case BUT_TYPE_OK:
              updateAccChange();
              updateSpeedHID();
              closeWindow(WIN_ACC_EDIT);
              break;
            case BUT_TYPE_CNCL:
              updateSpeedHID();
              closeWindow(WIN_ACC_EDIT);
              break;
            case BUT_ACC_OUT0:
            case BUT_ACC_OUT1:
            case BUT_ACC_OUT2:
            case BUT_ACC_OUT3:
            case BUT_ACC_OUT4:
            case BUT_ACC_OUT5:
            case BUT_ACC_OUT6:
            case BUT_ACC_OUT7:
            case BUT_ACC_OUT8:
            case BUT_ACC_OUT9:
            case BUT_ACC_OUT10:
            case BUT_ACC_OUT11:
            case BUT_ACC_OUT12:
            case BUT_ACC_OUT13:
            case BUT_ACC_OUT14:
            case BUT_ACC_OUT15:
              accOutClick(event.objID - BUT_ACC_OUT0);
              break;
            case BUT_ACC_RED:
            case BUT_ACC_GREEN:
              value = atoi(txtData[TXT_ACC_ADDR].buf);
              if ((value > 0) && (value < 2049)) {
                buttonData[event.objID].backgnd = (event.objID == BUT_ACC_RED) ? COLOR_DARKRED : COLOR_DARKGREEN;;
                drawObject(OBJ_BUTTON, event.objID);
                myTurnout = value;
                buttonData[event.objID].backgnd = (event.objID == BUT_ACC_RED) ? COLOR_RED : COLOR_GREEN;
                moveAccessory(myTurnout, event.objID - BUT_ACC_RED);
                newEvent(OBJ_BUTTON, event.objID, EVNT_DRAW);
                delay(80);
              }
              break;
            case BUT_STA_START:
              //staCurrStation = 0;
              updateCountStations();
              if (staLevel == 1)
                staLastStation = MAX_STATIONS;
              staCurrTime = staTime;
              updateStationTime(staCurrTime);
              setTimer(TMR_STA_RUN, 10, TMR_PERIODIC);
              setNewTarget();
              openWindow(WIN_STA_PLAY);
              updateSpeedHID();
              break;
            case BUT_STA_CNCL:
              closeWindow(WIN_STA_RUN);
              break;
            case BUT_STA_STOP:
              closeWindow(WIN_STA_PLAY);
              closeWindow(WIN_STA_RUN);
              break;
            case BUT_STA_EDIT:
              closeWindow(WIN_STA_EDIT);
              if (eepromChanged) {
                EEPROM.commit();
                eepromChanged = false;
              }
              break;
            case BUT_STA_STAM:
              if (staMaxStations > 3) {
                staMaxStations--;
                snprintf(staStatNumBuf, IP_LNG + 1, "%d", staMaxStations);
                newEvent(OBJ_TXT, TXT_STA_STATNUM, EVNT_DRAW);
                EEPROM.write(EE_STA_NUM, staMaxStations);
                eepromChanged = true;
              }
              break;
            case BUT_STA_STAP:
              if (staMaxStations < 5) {
                staMaxStations++;
                snprintf(staStatNumBuf, IP_LNG + 1, "%d", staMaxStations);
                newEvent(OBJ_TXT, TXT_STA_STATNUM, EVNT_DRAW);
                EEPROM.write(EE_STA_NUM, staMaxStations);
                eepromChanged = true;
              }
              break;
            case BUT_STA_TURNM:
              if (staMaxTurnout > 1) {
                staMaxTurnout--;
                snprintf(staTurnNumBuf, IP_LNG + 1, "%d", staMaxTurnout);
                newEvent(OBJ_TXT, TXT_STA_TURNNUM, EVNT_DRAW);
                EEPROM.write(EE_STA_TRNNUM, staMaxTurnout);
                eepromChanged = true;
              }
              break;
            case BUT_STA_TURNP:
              if (staMaxTurnout < 4) {
                staMaxTurnout++;
                snprintf(staTurnNumBuf, IP_LNG + 1, "%d", staMaxTurnout);
                newEvent(OBJ_TXT, TXT_STA_TURNNUM, EVNT_DRAW);
                EEPROM.write(EE_STA_TRNNUM, staMaxTurnout);
                eepromChanged = true;
              }
              break;
            case BUT_STA_ACC0:
              staTurnoutPos[0] = !staTurnoutPos[0];
              moveAccessory(staTurnoutAdr1, bitRead(staTurnoutDef, 4) ? staTurnoutPos[0] : !staTurnoutPos[0]);
              updateTurnoutButtons();
              newEvent(OBJ_BUTTON, BUT_STA_ACC0, EVNT_DRAW);
              break;
            case BUT_STA_ACC1:
              staTurnoutPos[1] = !staTurnoutPos[1];
              moveAccessory(staTurnoutAdr2, bitRead(staTurnoutDef, 5) ? staTurnoutPos[1] : !staTurnoutPos[1]);
              updateTurnoutButtons();
              newEvent(OBJ_BUTTON, BUT_STA_ACC1, EVNT_DRAW);
              break;
            case BUT_STA_ACC2:
              staTurnoutPos[2] = !staTurnoutPos[2];
              moveAccessory(staTurnoutAdr3, bitRead(staTurnoutDef, 6) ? staTurnoutPos[2] : !staTurnoutPos[2]);
              updateTurnoutButtons();
              newEvent(OBJ_BUTTON, BUT_STA_ACC2, EVNT_DRAW);
              break;
            case BUT_STA_ACC3:
              staTurnoutPos[3] = !staTurnoutPos[3];
              moveAccessory(staTurnoutAdr4, bitRead(staTurnoutDef, 7) ? staTurnoutPos[3] : !staTurnoutPos[3]);
              updateTurnoutButtons();
              newEvent(OBJ_BUTTON, BUT_STA_ACC3, EVNT_DRAW);
              break;
          }
          break;
        case OBJ_ICON:
          switch (event.objID) {
            case ICON_FNEXT:
              showNextFuncBlock();
              break;
            case ICON_POWER:
              togglePower();
              break;
            case ICON_MENU:
              openWindow(WIN_MENU);
              break;
            case ICON_LOK_EDIT:
              if (notLockedOption(LOCK_SEL_LOCO))
                openWindow(WIN_LOK_EDIT);
              break;
            /*
              case ICON_KEYB:
              openWindow(WIN_ENTER_ADDR);
              break;
            */
            case ICON_SEL_LOK:
              currOrder = (currOrder == SORT_NAME_DWN) ? SORT_LAST : currOrder + 1;
              sortLocoList(currOrder);
              encoderValue = 0;
              populateLocoList();
              newEvent(OBJ_WIN, WIN_SEL_LOCO, EVNT_DRAW);
              break;
            case ICON_NEXT_IMAGE:
              if (locoImages[locoImageIndex + 6] != 0) {
                locoImageIndex += 6;
                populateImageList();
                newEvent(OBJ_WIN, WIN_SEL_IMAGE, EVNT_DRAW);
              }
              break;
            case ICON_PREV_IMAGE:
              if (locoImageIndex > 5) {
                locoImageIndex -= 6;
                populateImageList();
                newEvent(OBJ_WIN, WIN_SEL_IMAGE, EVNT_DRAW);
              }
              break;
            case ICON_CFG_EXIT:
              if (lastLanguage != currLanguage) {
                EEPROM.write(EE_LANGUAGE, currLanguage);
                eepromChanged = true;
                DEBUG_MSG("Language changed")
              }
              if (eepromChanged) {                                        // save in EEPROM all changes in configuration options not saved before
                EEPROM.commit();
                eepromChanged = false;
                DEBUG_MSG("Saving EEPROM")
              }
              closeWindow(WIN_CONFIG);
              break;
            case ICON_UTL_EXIT:
              closeWindow(WIN_UTIL);
              break;
            case ICON_ABOUT_PACO:
              closeWindow(WIN_ABOUT);
              break;
            case ICON_KEYB_ACC:
              closeWindow(WIN_ACC_CTRL);
              break;
            case ICON_PLUS_ONE:
              if (currAccEdit.addr < 2048) {
                currAccEdit.addr2 = currAccEdit.addr + 1;
                snprintf(accKeybAddr2, ADDR_LNG + 1, "%d",  currAccEdit.addr2);
                newEvent(OBJ_TXT, TXT_ACC_ADDR2, EVNT_DRAW);
              }
              break;
            case ICON_STA_TARGET:
            case ICON_STA_PIN:
              clickTargetStation();
              break;
            case ICON_STA_EDIT:
              if (notLockedOption(LOCK_PROG)) {
                eepromChanged = false;
                openWindow(WIN_STA_EDIT);
              }
              break;
          }
          break;
        case OBJ_FNC:
          switch (event.objID) {
            case FNC_FX0:
            case FNC_FX1:
            case FNC_FX2:
            case FNC_FX3:
            case FNC_FX4:
            case FNC_FX5:
            case FNC_FX6:
            case FNC_FX7:
            case FNC_FX8:
            case FNC_FX9:
              value = fncData[event.objID].num;
              if (value < 29) {
                toggleFunction(value, event.objID);                       // Function Fx
              }
              break;
            case FNC_F0:
            case FNC_F1:
            case FNC_F2:
            case FNC_F3:
            case FNC_F4:
            case FNC_F5:
            case FNC_F6:
            case FNC_F7:
            case FNC_F8:
            case FNC_F9:
            case FNC_F10:
            case FNC_F11:
            case FNC_F12:
            case FNC_F13:
            case FNC_F14:
            case FNC_F15:
            case FNC_F16:
            case FNC_F17:
            case FNC_F18:
            case FNC_F19:
            case FNC_F20:
            case FNC_F21:
            case FNC_F22:
            case FNC_F23:
            case FNC_F24:
            case FNC_F25:
            case FNC_F26:
            case FNC_F27:
            case FNC_F28:
              if (wifiSetting.protocol != CLIENT_ECOS) {
                paramChild = event.objID - FNC_F0;
                snprintf(locoEditFunc, ADDR_LNG, "F%d", paramChild);
                fncData[FNC_CHG].idIcon = fncData[FNC_F0 + paramChild].idIcon;
                encoderValue = fncData[FNC_CHG].idIcon >> 1;
                encoderMax = FNC_ICON_MAX;
                openWindow(WIN_CHG_FUNC);
              }
              break;
            case FNC_CHG:
              fncData[FNC_F0 + paramChild].idIcon = fncData[FNC_CHG].idIcon;
              closeWindow(WIN_CHG_FUNC);
              break;
            case FNC_ST_FIRE:
              shovelCoal = !shovelCoal;
              setFirebox();
              newEvent(OBJ_FNC, FNC_ST_FIRE, EVNT_DRAW);
              break;
            case FNC_ST_WATER:
              if (waterInjection)
                endWaterInjection();
              else
                startWaterInjection();
              break;
            case FNC_ST_TENDER:
              if (fillTender)
                endTenderFill();
              else
                startTenderFill();
              break;
            case FNC_ST_WHISTLE:
              toggleFunction(2, FNC_ST_WHISTLE);                           // Function F2
              break;
            case FNC_ACC_TYPE:
              accTypeClick();
              break;
            case FNC_ACC_PANEL:
              if (notLockedOption(LOCK_TURNOUT))
                openWindow(WIN_ACCESSORY);
              break;
            case FNC_SEL_KEYPAD:
              openWindow(WIN_ENTER_ADDR);
              break;
            case FNC_SCAN_RESET:
              ESP.restart();
              break;
            case FNC_STA_RAYO:
              if (isTrackOff())
                resumeOperations();
          }
          break;
        case OBJ_TXT:
          DEBUG_MSG("Textbox click")
          txtID = 0;
          switch (event.objID) {
            case TXT_SSID6:
              txtID++;
            case TXT_SSID5:
              txtID++;
            case TXT_SSID4:
              txtID++;
            case TXT_SSID3:
              txtID++;
            case TXT_SSID2:
              txtID++;
            case TXT_SSID1:
              txtData[TXT_SSID1 + txtID].backgnd = COLOR_DARKCYAN;
              drawObject(OBJ_TXT, TXT_SSID1 + txtID);
              delay(200);
              if (scrSSID > 5)
                txtID += (scrSSID - 5);
              snprintf (wifiSetting.ssid, 32, WiFi.SSID(txtID).c_str());     //saveSSID(txtID);
              EEPROM.put(EE_WIFI, wifiSetting);
              DEBUG_MSG("New SSID: %s", wifiSetting.ssid);
              eepromChanged = true;
              closeWindow(WIN_SSID);
              openWindow(WIN_WIFI);
              break;
            case TXT_SSID:
              closeWindow(WIN_WIFI);
              openWindow(WIN_SSID);
              break;
            case TXT_PORT:
              if (wifiSetting.protocol != CLIENT_LNET)
                break;
              txtID++;
            case TXT_IP4:
              txtID++;
            case TXT_IP3:
              txtID++;
            case TXT_IP2:
              txtID++;
            case TXT_IP1:
              for (n = 0; n < 5; n++)
                txtData[TXT_IP1 + n].backgnd = COLOR_BACKGROUND;
              txtData[TXT_IP1 + txtID].backgnd = COLOR_YELLOW;              // select focus on textbox
              keybData[KEYB_IP].idTextbox = TXT_IP1 + txtID;
              for (n = 0; n < 5; n++)
                drawObject(OBJ_TXT, TXT_IP1 + n);
              break;
            case TXT_PWD_HIDE:
              openWindow(WIN_WIFI_PWD);
              break;
            case TXT_PROTOCOL:
              openWindow(WIN_PROTOCOL);
              break;
            case TXT_RATE:
              txtID++;
            case TXT_MIN:
              txtID++;
            case TXT_HOUR:
              showClockData(TXT_HOUR + txtID);
              for (n = 0; n < 3; n++)
                drawObject(OBJ_TXT, TXT_HOUR + n);
              break;
            case TXT_EDIT_NAME:
              openWindow(WIN_EDIT_NAME);
              break;
            case TXT_EDIT_VMAX:
              openWindow(WIN_VMAX);
              break;
            case TXT_SEL_ADDR6:
            case TXT_SEL_NAME6:
              txtID++;
            case TXT_SEL_ADDR5:
            case TXT_SEL_NAME5:
              txtID++;
            case TXT_SEL_ADDR4:
            case TXT_SEL_NAME4:
              txtID++;
            case TXT_SEL_ADDR3:
            case TXT_SEL_NAME3:
              txtID++;
            case TXT_SEL_ADDR2:
            case TXT_SEL_NAME2:
              txtID++;
            case TXT_SEL_ADDR1:
            case TXT_SEL_NAME1:
              value2 = countLocoInStack();
              if (txtID < value2) {
                txtData[TXT_SEL_ADDR1 + txtID].backgnd = COLOR_DARKCYAN;
                txtData[TXT_SEL_NAME1 + txtID].backgnd = COLOR_DARKCYAN;
                drawObject(OBJ_TXT, TXT_SEL_ADDR1 + txtID);
                drawObject(OBJ_TXT, TXT_SEL_NAME1 + txtID);
                delay(200);
                if (useID) {
                  value2 = (encoderValue > 5) ? encoderValue - 5 : 0;
                  value = sortedLocoStack[value2 + txtID];
                }
                else {
                  value = atoi(txtData[TXT_SEL_ADDR1 + txtID].buf);
                }
                DEBUG_MSG("Selected Loco %d", value);
                closeWindow(WIN_SEL_LOCO);
                getNewLoco(value);
              }
              break;
            case TXT_SPEEDO_LNG:
              openWindow(WIN_SPEEDO_LNG);
              break;
            case TXT_SPEEDO_SCALE:
              openWindow(WIN_SPEEDO_SCALE);
              break;
            case TXT_ABOUT:
            case TXT_ABOUT_IP:
            case TXT_ABOUT_MAC:
              closeWindow(WIN_ABOUT);
              break;
            case TXT_CV:
              enterCVdata = false;
              setFieldsCV();
              showFieldsCV();
              break;
            case TXT_CV_VAL:
              enterCVdata = true;
              setFieldsCV();
              showFieldsCV();
              break;
            case TXT_LNCV_ART:
            case TXT_LNCV_MOD:
              if (optLNCV > LNCV_MOD) {
                numLNCV = 0;
                valLNCV = modNum;
                sendLNCV (LNCV_REQID_CFGREQUEST, LNCV_FLAG_PROFF);
              }
              optLNCV = event.objID - TXT_LNCV_ART;
              setFieldsLNCV();
              showFieldsLNCV();
              break;
            case TXT_LNCV_ADR:
            case TXT_LNCV_VAL:
              if (optLNCV < LNCV_ADR) {
                numLNCV = 0;
                valLNCV = modNum;
                sendLNCV (LNCV_REQID_CFGREQUEST, LNCV_FLAG_PRON);
              }
              optLNCV = event.objID - TXT_LNCV_ART;
              setFieldsLNCV();
              showFieldsLNCV();
              break;
            case TXT_PANEL:
              if (editAccessory)
                openWindow(WIN_PANEL_NAME);
              else {
                if (accPanelChanged) {
                  accPanelChanged = false;
                  if (sdDetected)
                    saveCurrAccPanel(SD);
                  else
                    saveCurrAccPanel(LittleFS);
                }
                saveCurrentAspects();
                openWindow(WIN_PANELS);
              }
              break;
            case TXT_PANEL0:
            case TXT_PANEL1:
            case TXT_PANEL2:
            case TXT_PANEL3:
            case TXT_PANEL4:
            case TXT_PANEL5:
            case TXT_PANEL6:
            case TXT_PANEL7:
            case TXT_PANEL8:
            case TXT_PANEL9:
            case TXT_PANEL10:
            case TXT_PANEL11:
            case TXT_PANEL12:
            case TXT_PANEL13:
            case TXT_PANEL14:
            case TXT_PANEL15:
              currPanel = event.objID - TXT_PANEL0;
              populateAccPanel();
              closeWindow(WIN_PANELS);
              break;
            case TXT_ACC_NAME:
              openWindow(WIN_ACC_NAME);
              break;
            case TXT_ACC_ADDR1:
              openWindow(WIN_ACC_ADDR1);
              break;
            case TXT_ACC_ADDR2:
              openWindow(WIN_ACC_ADDR2);
              break;
            case TXT_STA_STARTTIME:
            case TXT_STA_TURNOUT1:
            case TXT_STA_TURNOUT2:
            case TXT_STA_TURNOUT3:
            case TXT_STA_TURNOUT4:
              keybData[KEYB_STA].idTextbox = event.objID;
              openWindow(WIN_STA_KEYB);
              break;
          }
          break;
        case OBJ_SWITCH:
          DEBUG_MSG("Switch click")
          txtID = 0;
          switch (event.objID) {
            case SW_SHUNTING:
              switchData[SW_SHUNTING].state = !switchData[SW_SHUNTING].state;
              //shuntingMode = switchData[SW_SHUNTING].state;
              newEvent(OBJ_SWITCH, SW_SHUNTING, EVNT_DRAW);
              break;
            case SW_ROTATE:
              switchData[SW_ROTATE].state = !switchData[SW_ROTATE].state;
              newEvent(OBJ_SWITCH, SW_ROTATE, EVNT_DRAW);
              break;
            case SW_LOCK_LOK:
              switchData[SW_LOCK_LOK].state = !switchData[SW_LOCK_LOK].state;
              newEvent(OBJ_SWITCH, SW_LOCK_LOK, EVNT_DRAW);
              break;
            case SW_LOCK_ACC:
              switchData[SW_LOCK_ACC].state = !switchData[SW_LOCK_ACC].state;
              newEvent(OBJ_SWITCH, SW_LOCK_ACC, EVNT_DRAW);
              break;
            case SW_LOCK_PRG:
              switchData[SW_LOCK_PRG].state = !switchData[SW_LOCK_PRG].state;
              newEvent(OBJ_SWITCH, SW_LOCK_PRG, EVNT_DRAW);
              break;
            case SW_OPT_ADR:
              if (wifiSetting.protocol == CLIENT_Z21) {
                switchData[SW_OPT_ADR].state = !switchData[SW_OPT_ADR].state;
                newEvent(OBJ_SWITCH, SW_OPT_ADR, EVNT_DRAW);
                shortAddress = switchData[SW_OPT_ADR].state ? 99 : 127;
              }
              break;
            case SW_OPT_DISCOVER:
              if (wifiSetting.protocol == CLIENT_LNET) {
                switchData[SW_OPT_DISCOVER].state = !switchData[SW_OPT_DISCOVER].state;
                newEvent(OBJ_SWITCH, SW_OPT_DISCOVER, EVNT_DRAW);
                autoIdentifyCS = switchData[SW_OPT_DISCOVER].state ? 1 : 0;
              }
              break;
            case SW_POM:
              modeProg = !switchData[SW_POM].state;
              switchData[SW_POM].state = modeProg;
              newEvent(OBJ_SWITCH, SW_POM, EVNT_DRAW);
              break;
            case SW_STA_OR1:
            case SW_STA_OR2:
            case SW_STA_OR3:
            case SW_STA_OR4:
            case SW_STA_INV1:
            case SW_STA_INV2:
            case SW_STA_INV3:
            case SW_STA_INV4:
              switchData[event.objID].state = !switchData[event.objID].state;
              bitWrite(staTurnoutDef, event.objID - SW_STA_OR1, switchData[event.objID].state);
              newEvent(OBJ_SWITCH, event.objID, EVNT_DRAW);
              EEPROM.write(EE_STA_TRNDEF, staTurnoutDef);
              eepromChanged = true;
              DEBUG_MSG("STA DEF: %02X", staTurnoutDef);
              break;
          }
          break;
        case OBJ_RADIO:
          switch (event.objID) {
            case RAD_STOP_MODE:
              n = map(lastClickY, radioData[RAD_STOP_MODE].y, radioData[RAD_STOP_MODE].y + (radioData[RAD_STOP_MODE].h * radioData[RAD_STOP_MODE].num), 0, radioData[RAD_STOP_MODE].num);
              radioData[RAD_STOP_MODE].value = n;
              newEvent(OBJ_RADIO, RAD_STOP_MODE, EVNT_DRAW);
              DEBUG_MSG("STOP MODE %d", n);
              break;
            case RAD_PROTOCOL:
              n = map(lastClickY, radioData[RAD_PROTOCOL].y, radioData[RAD_PROTOCOL].y + (radioData[RAD_PROTOCOL].h * radioData[RAD_PROTOCOL].num), 0, radioData[RAD_PROTOCOL].num);
              radioData[RAD_PROTOCOL].value = n;
              newEvent(OBJ_RADIO, RAD_PROTOCOL, EVNT_DRAW);
              wifiSetting.protocol = CLIENT_Z21 + n;
              if (wifiSetting.protocol == CLIENT_LNET)
                radioData[RAD_PROTOCOL_LN].value = (wifiSetting.serverType) ? 0 : 1;
              else
                radioData[RAD_PROTOCOL_LN].value = radioData[RAD_PROTOCOL_LN].num;
              newEvent(OBJ_RADIO, RAD_PROTOCOL_LN, EVNT_DRAW);
              EEPROM.put(EE_WIFI, wifiSetting);
              eepromChanged = true;
              setProtocolData();
              DEBUG_MSG("PROTOCOL %d", n);
              break;
            case RAD_PROTOCOL_LN:
              if (wifiSetting.protocol == CLIENT_LNET) {
                n = map(lastClickY, radioData[RAD_PROTOCOL_LN].y, radioData[RAD_PROTOCOL_LN].y + (radioData[RAD_PROTOCOL_LN].h * radioData[RAD_PROTOCOL_LN].num), 0, radioData[RAD_PROTOCOL_LN].num);
                radioData[RAD_PROTOCOL_LN].value = n;
                newEvent(OBJ_RADIO, RAD_PROTOCOL_LN, EVNT_DRAW);
                wifiSetting.serverType = (n == 0) ? true : false;
                EEPROM.put(EE_WIFI, wifiSetting);
                eepromChanged = true;
                setProtocolData();
                DEBUG_MSG("PROT. LN %d", n);
              }
              break;
            case RAD_CSTATION:
              if (wifiSetting.protocol == CLIENT_LNET) {
                n = map(lastClickY, radioData[RAD_CSTATION].y, radioData[RAD_CSTATION].y + (radioData[RAD_CSTATION].h * radioData[RAD_CSTATION].num), 0, radioData[RAD_CSTATION].num);
                radioData[RAD_CSTATION].value = n;
                newEvent(OBJ_RADIO, RAD_CSTATION, EVNT_DRAW);
                typeCmdStation = n;
              }
              break;
          }
          break;
        case OBJ_KEYBOARD:
          chr = getKeyTyped(event.objID, lastClickX, lastClickY);       // typed char
          txtID = keybData[event.objID].idTextbox;                      // text box
          lng = strlen(txtData[txtID].buf);                             // current text length
          switch (chr) {
            case CHR_REDRAW:
              newEvent(OBJ_KEYBOARD, event.objID, EVNT_DRAW);
              break;
            case CHR_BKSPC:
              DEBUG_MSG("Key Delete")
              if (lng > 0) {
                txtData[txtID].buf[lng - 1] = '\0';
                newEvent(OBJ_TXT, txtID, EVNT_DRAW);
                switch (event.objID) {
                  case KEYB_CV:
                    if  (enterCVdata) {
                      CVdata = atoi(keybCvValBuf);
                      setBitsCV();
                      showFieldsCV();
                    }
                    else {
                      CVaddress = atoi(keybCvBuf);
                      setStatusCV();
                      newEvent(OBJ_TXT, TXT_CV_STATUS, EVNT_DRAW);
                    }
                    break;
                  case KEYB_LNCV:
                    switch (optLNCV) {
                      case LNCV_ART:
                        artNum = atoi(keybLncvArtBuf);
                        break;
                      case LNCV_MOD:
                        modNum = atoi(keybLncvModBuf);
                        break;
                      case LNCV_ADR:
                        numLNCV = atoi(keybLncvAdrBuf);
                        break;
                      case LNCV_VAL:
                        valLNCV = atoi(keybLncvValBuf);
                        break;
                    }
                    showFieldsLNCV();
                    break;
                  case KEYB_ACC:
                    txtData[txtID].buf[0] = '\0';
                    break;
                }
              }
              break;
            case CHR_ENTER:
              switch (event.objID) {
                case KEYB_VMAX:
                  value = atoi(locoEditVmax);
                  if (value > 0) {
                    closeWindow(WIN_VMAX);
                  }
                  break;
                case KEYB_ADDR:
                  closeWindow(WIN_ENTER_ADDR);
                  value = atoi(locoKeybAddr);
                  DEBUG_MSG("Typed loco: %d", value);
                  if (value > 0) {
                    encoderMax = 63;
                    closeWindow(WIN_SEL_LOCO);
                    getNewLoco(value);
                  }
                  break;
                case KEYB_LNG:
                  value = atoi(speedoKeybLng);
                  if (value > 0) {
                    closeWindow(WIN_SPEEDO_LNG);
                    speedoLength = value;
                    snprintf(spdLengthBuf, NAME_LNG + 1, "%d", speedoLength);
                    setTimer(TMR_INFO, 5, TMR_ONESHOT);
                  }
                  break;
                case KEYB_SCALE:
                  value = atoi(spdSelScaleNumBuf);
                  if (value > 0) {
                    closeWindow(WIN_SPEEDO_SCALE);
                    speedoScale = value;
                    setTextSpeedo();
                    setTimer(TMR_INFO, 5, TMR_ONESHOT);
                  }
                  break;
                case KEYB_CV:
                  if (enterCVdata) {
                    if (CVdata > 255) {                                           // CV read data error
                      txtData[TXT_CV_VAL].backgnd = COLOR_PINK;
                      newEvent(OBJ_TXT, TXT_CV_VAL, EVNT_DRAW);
                    }
                    else {
                      if (keybCvValBuf[0] != '\0') {                              // CV data not blank
                        if ((CVaddress > 1024) || (CVaddress == 0)) {             // Range of CV address
                          txtData[TXT_CV].backgnd = COLOR_PINK;
                          newEvent(OBJ_TXT, TXT_CV, EVNT_DRAW);
                        }
                        else {
                          if (modeProg) {
                            enterCVdata = false;
                            setFieldsCV();
                            showFieldsCV();
                          }
                          else {
                            alertWindow(ERR_CV);
                          }
                          writeCV(CVaddress, CVdata, PRG_CV);
                        }
                      }
                    }
                  }
                  else {
                    if ((CVaddress > 1024) || (CVaddress == 0)) {                 // Range of CV address
                      txtData[TXT_CV].backgnd = COLOR_PINK;
                      newEvent(OBJ_TXT, TXT_CV, EVNT_DRAW);
                    }
                    else {
                      if (!modeProg) {
                        alertWindow(ERR_CV);
                        readCV(CVaddress, PRG_CV);
                      }
                    }
                  }
                  break;
                case KEYB_CV_ADDR:
                  closeWindow(WIN_PROG_ADDR);
                  n = atoi(locoEditAddr);
                  if (n > 0) {
                    decoAddress = n;
                    switch (wifiSetting.protocol) {
                      case CLIENT_Z21:
                        n = shortAddress;
                        break;
                      case CLIENT_XNET:
                        n = 99;
                        break;
                      case CLIENT_ECOS:
                      case CLIENT_LNET:
                        n = 127;
                        break;
                    }
                    alertWindow(ERR_CV);
                    if (decoAddress > n) {
                      CVaddress = 17;
                      writeCV(CVaddress, highByte(decoAddress) | 0xC0, PRG_WR_CV17);
                    }
                    else  {
                      CVaddress = 1;
                      writeCV(CVaddress, decoAddress & 0x7F, PRG_WR_CV1);
                    }
                  }
                  break;
                case KEYB_LNCV:
                  switch (optLNCV) {
                    case LNCV_ART:
                      optLNCV = LNCV_MOD;
                      setFieldsLNCV();
                      showFieldsLNCV();
                      break;
                    case LNCV_MOD:
                      numLNCV = 0;
                      valLNCV = modNum;
                      sendLNCV (LNCV_REQID_CFGREQUEST, LNCV_FLAG_PRON);
                      optLNCV = LNCV_ADR;
                      setFieldsLNCV();
                      showFieldsLNCV();
                      break;
                    case LNCV_ADR:
                      optLNCV = LNCV_VAL;
                      setFieldsLNCV();
                      showFieldsLNCV();
                      sendLNCV (LNCV_REQID_CFGREQUEST, 0);
                      break;
                    case LNCV_VAL:
                      optLNCV = LNCV_ADR;
                      setFieldsLNCV();
                      showFieldsLNCV();
                      sendLNCV (LNCV_REQID_CFGWRITE, 0);
                      break;
                  }
                  break;
                case KEYB_ACC:
                  closeWindow(WIN_ACC_CTRL);
                  break;
                case KEYB_ACC_ADDR:
                  if (isWindow(WIN_ACC_ADDR1)) {
                    value = atoi(accKeybAdrEdit);
                    if ((value > 0) && (value < 2049)) {
                      snprintf(accKeybAddr1, ADDR_LNG + 1, "%d",  value);
                      currAccEdit.addr = value;
                    }
                    closeWindow(WIN_ACC_ADDR1);
                  }
                  if (isWindow(WIN_ACC_ADDR2)) {
                    value = atoi(accKeybAdrEdit);
                    if ((value > 0) && (value < 2049)) {
                      snprintf(accKeybAddr2, ADDR_LNG + 1, "%d",  value);
                      currAccEdit.addr2 = value;
                    }
                    closeWindow(WIN_ACC_ADDR2);
                  }
                  break;
                case KEYB_STA:
                  switch (keybData[KEYB_STA].idTextbox) {
                    case TXT_STA_STARTTIME:
                      value = atoi(staStartTimeBuf);
                      staStartTime = constrain(value, 10, 255);
                      snprintf(staStartTimeBuf, IP_LNG + 1, "%d", staStartTime);
                      EEPROM.write(EE_STA_TIME, staStartTime);
                      updateStationTarget();
                      updateStationTime(staTime);
                      break;
                    case TXT_STA_TURNOUT1:
                      value = atoi(staTurnout1Buf);
                      staTurnoutAdr1 = constrain(value, 1, 2048);
                      snprintf(staTurnout1Buf, ADDR_LNG + 1, "%d", staTurnoutAdr1);
                      EEPROM.write(EE_STA_ADRH1, staTurnoutAdr1 >> 8);
                      EEPROM.write(EE_STA_ADRL1, staTurnoutAdr1 & 0xFF);
                      break;
                    case TXT_STA_TURNOUT2:
                      value = atoi(staTurnout2Buf);
                      staTurnoutAdr2 = constrain(value, 1, 2048);
                      snprintf(staTurnout2Buf, ADDR_LNG + 1, "%d", staTurnoutAdr2);
                      EEPROM.write(EE_STA_ADRH2, staTurnoutAdr2 >> 8);
                      EEPROM.write(EE_STA_ADRL2, staTurnoutAdr2 & 0xFF);
                      break;
                    case TXT_STA_TURNOUT3:
                      value = atoi(staTurnout3Buf);
                      staTurnoutAdr3 = constrain(value, 1, 2048);
                      snprintf(staTurnout3Buf, ADDR_LNG + 1, "%d", staTurnoutAdr3);
                      EEPROM.write(EE_STA_ADRH3, staTurnoutAdr3 >> 8);
                      EEPROM.write(EE_STA_ADRL3, staTurnoutAdr3 & 0xFF);
                      break;
                    case TXT_STA_TURNOUT4:
                      value = atoi(staTurnout4Buf);
                      staTurnoutAdr4 = constrain(value, 1, 2048);
                      snprintf(staTurnout4Buf, ADDR_LNG + 1, "%d", staTurnoutAdr4);
                      EEPROM.write(EE_STA_ADRH4, staTurnoutAdr4 >> 8);
                      EEPROM.write(EE_STA_ADRL4, staTurnoutAdr4 & 0xFF);
                      break;
                  }
                  eepromChanged = true;
                  closeWindow(WIN_STA_KEYB);
                  break;
              }
              break;
            case ',':
            case ';':
              if (event.objID == KEYB_NAME) {                                     // not valid characters for loco name
                break;
              }
            default:
              if (chr >= ' ') {
                DEBUG_MSG("Key %d - %c", chr, chr)
                if (txtData[txtID].maxLength > lng) {
                  txtData[txtID].buf[lng++] = chr;
                  txtData[txtID].buf[lng] = '\0';
                  newEvent(OBJ_TXT, txtID, EVNT_DRAW);
                  switch (event.objID) {
                    case KEYB_CV:
                      if (enterCVdata) {                                          // calc new CV data/address
                        CVdata = atoi(keybCvValBuf);
                        setBitsCV();
                        showFieldsCV();
                      }
                      else {
                        CVaddress = atoi(keybCvBuf);
                        setStatusCV();
                        newEvent(OBJ_TXT, TXT_CV_STATUS, EVNT_DRAW);
                      }
                      break;
                    case KEYB_LNCV:
                      switch (optLNCV) {
                        case LNCV_ART:
                          artNum = atoi(keybLncvArtBuf);
                          break;
                        case LNCV_MOD:
                          modNum = atoi(keybLncvModBuf);
                          break;
                        case LNCV_ADR:
                          numLNCV = atoi(keybLncvAdrBuf);
                          break;
                        case LNCV_VAL:
                          valLNCV = atoi(keybLncvValBuf);
                          break;
                      }
                      break;
                  }
                }
              }
              break;
          }
          break;
        case OBJ_LPIC:
          switch (event.objID) {
            case LPIC_MAIN:
              if (notLockedOption(LOCK_SEL_LOCO))
                openWindow(WIN_SEL_LOCO);
              break;
            case LPIC_LOK_EDIT:
              if (wifiSetting.protocol != CLIENT_ECOS)
                openWindow(WIN_SEL_IMAGE);
              break;
            case LPIC_SEL_IMG1:
            case LPIC_SEL_IMG2:
            case LPIC_SEL_IMG3:
            case LPIC_SEL_IMG4:
            case LPIC_SEL_IMG5:
            case LPIC_SEL_IMG6:
              txtID = event.objID - LPIC_SEL_IMG1;
              lpicData[LPIC_LOK_EDIT].id = lpicData[LPIC_SEL_IMG1 + txtID].id;
              sprintf (locoEditID, "%d", lpicData[LPIC_LOK_EDIT].id);
              closeWindow(WIN_SEL_IMAGE);
              break;
          }
          break;
        case OBJ_BAR:
          switch (event.objID) {
            case BAR_BLIGHT:
              n = map(lastClickX, barData[BAR_BLIGHT].x, barData[BAR_BLIGHT].x + barData[BAR_BLIGHT].w, USER_MIN_BL, 255);
              if ((n < 256) && (n > USER_MIN_BL)) {
                DEBUG_MSG("New Bright: %d", n);
                barData[BAR_BLIGHT].value = n;
                setBacklight (n);
                backlight = n;
                newEvent(OBJ_BAR, BAR_BLIGHT, EVNT_DRAW);
              }
              break;
            case BAR_JOHNSON:
              n = map(lastClickY, barData[BAR_JOHNSON].y + barData[BAR_JOHNSON].h, barData[BAR_JOHNSON].y, 0, 6);
              switch (barData[BAR_JOHNSON].value) {
                case 0:                                 // Current forward direction
                case 1:
                case 2:
                  if (n > STEAM_JOHNSON_NEUTRAL)        // Set neutral position of Johnson Bar
                    n = STEAM_JOHNSON_NEUTRAL;
                  break;
                case 4:                                 // Current reverse direction
                case 5:
                case 6:
                  if (n < STEAM_JOHNSON_NEUTRAL)        // Set neutral position of Johnson Bar
                    n = STEAM_JOHNSON_NEUTRAL;
                  break;
              }
              barData[BAR_JOHNSON].value = n;
              newEvent(OBJ_BAR, BAR_JOHNSON, EVNT_DRAW);
              break;
            case BAR_BRAKE:
              n = map(lastClickX, barData[BAR_BRAKE].x, barData[BAR_BRAKE].x + barData[BAR_BRAKE].w, 0, 4);
              barData[BAR_BRAKE].value = n;
              newEvent(OBJ_BAR, BAR_BRAKE, EVNT_DRAW);
              break;
          }
          break;
      }
      break;
  }
}

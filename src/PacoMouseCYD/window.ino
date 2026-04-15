/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

////////////////////////////////////////////////////////////
// ***** WINDOW OBJECTS *****
////////////////////////////////////////////////////////////

void openWindow(uint16_t id) {
  uint16_t n;
  char buf[MAX_LABEL_LNG];
  switch (id) {
    case WIN_LOGO:
      createObject(OBJ_WIN, WIN_LOGO);
      createObject(OBJ_DRAWSTR, DSTR_INIT);
      createObject(OBJ_LABEL, LBL_PACO_TXT);
      posObjStack1 = createObject(OBJ_LABEL, LBL_INIT);
      createObject(OBJ_ICON, ICON_PACO);
      createObject(OBJ_BAR, BAR_INIT);
      startWindow(WIN_LOGO);
      break;
    case WIN_CALIBRATE:
      createObject(OBJ_WIN, WIN_CALIBRATE);
      posObjStack2 = createObject(OBJ_LABEL, LBL_CAL);
      posObjStack1 = createObject(OBJ_LABEL, LBL_PRESS);
      newEvent(OBJ_WIN, WIN_CALIBRATE, EVNT_DRAW);
      break;
    case WIN_SSID:
      createObject(OBJ_WIN, WIN_SSID);
      createObject(OBJ_ICON, ICON_WIFI_SSID);
      createObject(OBJ_BUTTON, BUT_SSID_CLOSE);
      posObjStack1 = createObject(OBJ_LABEL, LBL_SCAN);
      drawWindow(WIN_SSID);
      scanWiFi();
      objStack[posObjStack1].objID = LBL_SSID_SCAN;
      createObject(OBJ_TXT, TXT_SSID1);
      createObject(OBJ_TXT, TXT_SSID2);
      createObject(OBJ_TXT, TXT_SSID3);
      createObject(OBJ_TXT, TXT_SSID4);
      createObject(OBJ_TXT, TXT_SSID5);
      createObject(OBJ_TXT, TXT_SSID6);
      scanWiFiFill();
      newEvent(OBJ_WIN, WIN_SSID, EVNT_DRAW);
      break;
    case WIN_WIFI:
      snprintf(networkNameBuf, NAME_LNG + 1, "%s", wifiSetting.network);
      snprintf(ssidName, SSID_LNG + 1, "%s", wifiSetting.ssid);
      snprintf(keybPwdHideBuf, 9, "********");
      snprintf(keybIP1Buf, 4, "%d", wifiSetting.CS_IP[0]);
      snprintf(keybIP2Buf, 4, "%d", wifiSetting.CS_IP[1]);
      snprintf(keybIP3Buf, 4, "%d", wifiSetting.CS_IP[2]);
      snprintf(keybIP4Buf, 4, "%d", wifiSetting.CS_IP[3]);
      setProtocolData();
      createObject(OBJ_WIN, WIN_WIFI);
      createObject(OBJ_ICON, ICON_WIFI_CFG);
      createObject(OBJ_LABEL, LBL_SSID);
      createObject(OBJ_TXT, TXT_NETWORK);
      createObject(OBJ_TXT, TXT_SSID);
      createObject(OBJ_LABEL, LBL_PWD_HIDE);
      createObject(OBJ_TXT, TXT_PWD_HIDE);
      createObject(OBJ_LABEL, LBL_IP);
      createObject(OBJ_TXT, TXT_IP1);
      createObject(OBJ_TXT, TXT_IP2);
      createObject(OBJ_TXT, TXT_IP3);
      createObject(OBJ_TXT, TXT_IP4);
      createObject(OBJ_LABEL, LBL_PORT);
      createObject(OBJ_TXT, TXT_PORT);
      createObject(OBJ_KEYBOARD, KEYB_IP);
      createObject(OBJ_LABEL, LBL_PROTOCOL);
      createObject(OBJ_TXT, TXT_PROTOCOL);
      createObject(OBJ_BUTTON, BUT_WIFI_OK);
      newEvent(OBJ_WIN, WIN_WIFI, EVNT_DRAW);
      break;
    case WIN_WIFI_PWD:
      txtData[TXT_PWD].maxLength = PWD_LNG;
      snprintf(keybPwdBuf, PWD_LNG + 1, wifiSetting.password);
      createObject(OBJ_WIN, WIN_WIFI_PWD);
      createObject(OBJ_TXT, TXT_PWD);
      createObject(OBJ_KEYBOARD, KEYB_PWD);
      createObject(OBJ_BUTTON, BUT_PWD_OK);
      createObject(OBJ_BUTTON, BUT_PWD_CNCL);
      newEvent(OBJ_WIN, WIN_WIFI_PWD, EVNT_DRAW);
      break;
    case WIN_WIFI_NET_NAME:
      txtData[TXT_PWD].maxLength = NAME_LNG;
      snprintf(keybPwdBuf, NAME_LNG + 1, wifiSetting.network);
      createObject(OBJ_WIN, WIN_WIFI_NET_NAME);
      createObject(OBJ_TXT, TXT_PWD);
      createObject(OBJ_KEYBOARD, KEYB_PWD);
      createObject(OBJ_BUTTON, BUT_PWD_OK);
      createObject(OBJ_BUTTON, BUT_PWD_CNCL);
      newEvent(OBJ_WIN, WIN_WIFI_NET_NAME, EVNT_DRAW);
      break;
    case WIN_WIFI_NET:
      createObject(OBJ_WIN, WIN_WIFI_NET);
      createObject(OBJ_LABEL, LBL_WIFI_NET);
      createObject(OBJ_RADIO, RAD_NETWORKS);
      createObject(OBJ_TXT, TXT_NETWORK_0);
      createObject(OBJ_TXT, TXT_NETWORK_1);
      createObject(OBJ_TXT, TXT_NETWORK_2);
      createObject(OBJ_TXT, TXT_NETWORK_3);
      createObject(OBJ_BUTTON, BUT_WIFI_SEL);
      createObject(OBJ_BUTTON, BUT_WIFI_CFG);
      createObject(OBJ_ICON, ICON_NET_WIFI);
      createObject(OBJ_ICON, ICON_NET_SCR);
      newEvent(OBJ_WIN, WIN_WIFI_NET, EVNT_DRAW);
      break;
    case WIN_PROTOCOL:
      radioData[RAD_PROTOCOL].value = wifiSetting.protocol - CLIENT_Z21;
      if (wifiSetting.protocol == CLIENT_LNET)
        radioData[RAD_PROTOCOL_LN].value = (wifiSetting.serverType) ? 0 : 1;
      else
        radioData[RAD_PROTOCOL_LN].value = radioData[RAD_PROTOCOL_LN].num;
      createObject(OBJ_WIN, WIN_PROTOCOL);
      createObject(OBJ_RADIO, RAD_PROTOCOL);
      createObject(OBJ_RADIO, RAD_PROTOCOL_LN);
      createObject(OBJ_LABEL, LBL_SEL_PROT);
      createObject(OBJ_LABEL, LBL_SEL_Z21);
      createObject(OBJ_LABEL, LBL_SEL_XNET);
      createObject(OBJ_LABEL, LBL_SEL_ECOS);
      createObject(OBJ_LABEL, LBL_SEL_LNET);
      createObject(OBJ_LABEL, LBL_SEL_LBSERVER);
      createObject(OBJ_LABEL, LBL_SEL_BINARY);
      createObject(OBJ_BUTTON, BUT_PROT_OK);
      createObject(OBJ_BUTTON, BUT_OPTIONS);
      newEvent(OBJ_WIN, WIN_PROTOCOL, EVNT_DRAW);
      break;
    case WIN_THROTTLE:
      iconData[ICON_LOK_EDIT].bitmap = (wifiSetting.protocol == CLIENT_ECOS) ? info24 : wrench;
      createObject(OBJ_WIN, WIN_THROTTLE);
      checkLowBatt();
      createObject(OBJ_ICON, ICON_MENU);
      createObject(OBJ_ICON, ICON_POWER);
      createObject(OBJ_LPIC, LPIC_MAIN);
      createObject(OBJ_ICON, ICON_LOW_BATT);
      createObject(OBJ_ICON, ICON_FNEXT);
      createObject(OBJ_ICON, ICON_LOK_EDIT);
      createObject(OBJ_FNC, FNC_ACC_PANEL);
      //createObject(OBJ_ICON, ICON_FWD);
      //createObject(OBJ_ICON, ICON_REV);
      createObject(OBJ_TXT, TXT_CLOCK);
      createObject(OBJ_TXT, TXT_LOCO_NAME);
      createObject(OBJ_TXT, TXT_LOCO_ADDR);
      createObject(OBJ_LABEL, LBL_KMH);
      createObject(OBJ_GAUGE, GAUGE_SPEED);
      createObject(OBJ_FNC, FNC_FX0);
      createObject(OBJ_FNC, FNC_FX1);
      createObject(OBJ_FNC, FNC_FX2);
      createObject(OBJ_FNC, FNC_FX3);
      createObject(OBJ_FNC, FNC_FX4);
      createObject(OBJ_FNC, FNC_FX5);
      createObject(OBJ_FNC, FNC_FX6);
      createObject(OBJ_FNC, FNC_FX7);
      createObject(OBJ_FNC, FNC_FX8);
      createObject(OBJ_FNC, FNC_FX9);
      newEvent(OBJ_WIN, WIN_THROTTLE, EVNT_DRAW);
      break;
    case WIN_CONFIG:
      buttonData[BUT_CFG_I_LANG].objID = DSTR_ENGLISH + currLanguage;
      lastLanguage = currLanguage;
      createObject(OBJ_WIN, WIN_CONFIG);
      createObject(OBJ_DRAWSTR, DSTR_CFG_MENU);
      createObject(OBJ_BUTTON, BUT_CFG_I_LANG);
      createObject(OBJ_BUTTON, BUT_CFG_T_LANG);
      createObject(OBJ_BUTTON, BUT_CFG_I_SCR);
      createObject(OBJ_BUTTON, BUT_CFG_T_SCR);
      createObject(OBJ_BUTTON, BUT_CFG_I_SPD);
      createObject(OBJ_BUTTON, BUT_CFG_T_SPD);
      createObject(OBJ_BUTTON, BUT_CFG_I_WIFI);
      createObject(OBJ_BUTTON, BUT_CFG_T_WIFI);
      createObject(OBJ_BUTTON, BUT_CFG_I_FCLK);
      createObject(OBJ_BUTTON, BUT_CFG_T_FCLK);
      createObject(OBJ_BUTTON, BUT_CFG_I_LOCK);
      createObject(OBJ_BUTTON, BUT_CFG_T_LOCK);
      createObject(OBJ_BUTTON, BUT_CFG_I_ABOUT);
      createObject(OBJ_BUTTON, BUT_CFG_T_ABOUT);
      createObject(OBJ_ICON, ICON_CFG_EXIT);
      newEvent(OBJ_WIN, WIN_CONFIG, EVNT_DRAW);
      break;
    case WIN_SCREEN:
      barData[BAR_BLIGHT].value = backlight;
      switchData[SW_ROTATE].state = (locationUSB == USB_UP);
      switchData[SW_RGB_LED].state = (activeRGB > 0);
      createObject(OBJ_WIN, WIN_SCREEN);
      createObject(OBJ_ICON, ICON_BLIGHT);
      createObject(OBJ_BAR, BAR_BLIGHT);
      createObject(OBJ_SWITCH, SW_ROTATE);
#if (USE_RGB_LED == PRESENT)
      createObject(OBJ_SWITCH, SW_RGB_LED);
      createObject(OBJ_DRAWSTR, DSTR_RGB);
#endif
      createObject(OBJ_LABEL, LBL_SCR_ROTATE);
      createObject(OBJ_BUTTON, BUT_CFG_TOUCH);
      createObject(OBJ_BUTTON, BUT_CFG_SW);
#if (BATT_MODE != READ_UNUSED)
      createObject(OBJ_BUTTON, BUT_CFG_BATT);
#endif
      createObject(OBJ_BUTTON, BUT_SCR_OK);
      createObject(OBJ_BUTTON, BUT_SCR_CNCL);
      newEvent(OBJ_WIN, WIN_SCREEN, EVNT_DRAW);
      break;
    case WIN_SPEED:
      switchData[SW_SHUNTING].state = shuntingMode;
      radioData[RAD_STOP_MODE].value = (stopMode > 0) ? 1 : 0;
      createObject(OBJ_WIN, WIN_SPEED);
      createObject(OBJ_LABEL, LBL_SHUNTING);
      createObject(OBJ_SWITCH, SW_SHUNTING);
      createObject(OBJ_RADIO, RAD_STOP_MODE);
      createObject(OBJ_ICON, ICON_STOP);
      createObject(OBJ_LABEL, LBL_STOP_0);
      createObject(OBJ_LABEL, LBL_STOP_E);
      createObject(OBJ_BUTTON, BUT_SPD_OK);
      newEvent(OBJ_WIN, WIN_SPEED, EVNT_DRAW);
      break;
    case WIN_SET_CLOCK:
      snprintf(keybHourBuf, 3, "%d", clockHour);
      snprintf(keybMinBuf, 3, "%d", clockMin);
      snprintf(keybRateBuf, 4, "%d", clockRate);
      showClockData(TXT_HOUR);
      createObject(OBJ_WIN, WIN_SET_CLOCK);
      createObject(OBJ_DRAWSTR, DSTR_CLOCK);
      createObject(OBJ_CHAR, CHAR_CLK_COLON);
      createObject(OBJ_ICON, ICON_SET_CLOCK);
      createObject(OBJ_TXT, TXT_HOUR);
      createObject(OBJ_TXT, TXT_MIN);
      createObject(OBJ_TXT, TXT_RATE);
      createObject(OBJ_LABEL, LBL_RATE);
      createObject(OBJ_KEYBOARD, KEYB_CLOCK);
      createObject(OBJ_BUTTON, BUT_CLOCK_OK);
      createObject(OBJ_BUTTON, BUT_CLOCK_CNCL);
      newEvent(OBJ_WIN, WIN_SET_CLOCK, EVNT_DRAW);
      break;
    case WIN_LOCK:
      switchData[SW_LOCK_LOK].state = (bitRead(lockOptions, LOCK_SEL_LOCO)) ? true : false;
      switchData[SW_LOCK_ACC].state = (bitRead(lockOptions, LOCK_TURNOUT)) ? true : false;
      switchData[SW_LOCK_PRG].state = (bitRead(lockOptions, LOCK_PROG)) ? true : false;
      createObject(OBJ_WIN, WIN_LOCK);
      createObject(OBJ_SWITCH, SW_LOCK_LOK);
      createObject(OBJ_SWITCH, SW_LOCK_ACC);
      createObject(OBJ_SWITCH, SW_LOCK_PRG);
      createObject(OBJ_LABEL, LBL_LOCK_LOK);
      createObject(OBJ_LABEL, LBL_LOCK_ACC);
      createObject(OBJ_LABEL, LBL_LOCK_PRG);
      createObject(OBJ_BUTTON, BUT_LOCK);
      newEvent(OBJ_WIN, WIN_LOCK, EVNT_DRAW);
      break;
    case WIN_ABOUT:
      snprintf (aboutPacoMouseCYD, PWD_LNG + 1, "v%s.%s%s", VER_H, VER_L, VER_R);
      snprintf (aboutIP,  PWD_LNG + 1, "IP:  %u.%u.%u.%u", WiFi.localIP().operator[](0), WiFi.localIP().operator[](1), WiFi.localIP().operator[](2), WiFi.localIP().operator[](3));
      snprintf (aboutMAC, PWD_LNG + 1, "MAC: %s", WiFi.macAddress().c_str());
      winData[WIN_ABOUT].h = 220;
      createObject(OBJ_WIN, WIN_ABOUT);
      createObject(OBJ_DRAWSTR, DSTR_ABOUT);
      createObject(OBJ_LABEL, LBL_PACO_TXT);
      createObject(OBJ_ICON, ICON_ABOUT_PACO);
      createObject(OBJ_TXT, TXT_ABOUT);
      createObject(OBJ_TXT, TXT_ABOUT_IP);
      createObject(OBJ_TXT, TXT_ABOUT_MAC);
      createObject(OBJ_LABEL, LBL_PACO_WEB);
      if (sdDetected) {
        winData[WIN_ABOUT].h = 290;
        createObject(OBJ_BUTTON, BUT_UPDATE);
      }
      newEvent(OBJ_WIN, WIN_ABOUT, EVNT_DRAW);
      break;
    case WIN_LOK_EDIT:
      snprintf (locoEditName, NAME_LNG + 1, "%s", locoData[myLocoData].myName );
      sprintf (locoEditAddr, "%d", locoData[myLocoData].myAddr.address);
      sprintf (locoEditID, "%d", locoData[myLocoData].myLocoID);
      sprintf (locoEditVmax, "%d", locoData[myLocoData].myVmax);
      lpicData[LPIC_LOK_EDIT].id = locoData[myLocoData].myLocoID;
      for (n = 0; n < 29; n++)
        fncData[FNC_F0 + n].idIcon = locoData[myLocoData].myFuncIcon[n];
      createObject(OBJ_WIN, WIN_LOK_EDIT);
      createObject(OBJ_LABEL, LBL_ADDR);
      createObject(OBJ_LABEL, LBL_IMAGE);
      createObject(OBJ_LPIC, LPIC_LOK_EDIT);
      createObject(OBJ_LABEL, LBL_NAME);
      createObject(OBJ_LABEL, LBL_VMAX);
      createObject(OBJ_TXT, TXT_EDIT_ADDR);
      createObject(OBJ_TXT, TXT_EDIT_NAME);
      createObject(OBJ_TXT, TXT_EDIT_IMAGE);
      createObject(OBJ_TXT, TXT_EDIT_VMAX);
      createObject(OBJ_BUTTON, BUT_EDIT_FUNC);
      createObject(OBJ_BUTTON, BUT_EDIT_CNCL);
      if (wifiSetting.protocol != CLIENT_ECOS) {
        createObject(OBJ_BUTTON, BUT_EDIT_OK);
        if ((locoData[myLocoData].mySpeed < 2) && (countLocoInStack() > 1))     // stopped and remaining locos in stack
          createObject(OBJ_BUTTON, BUT_EDIT_DEL);
      }
      newEvent(OBJ_WIN, WIN_LOK_EDIT, EVNT_DRAW);
      break;
    case WIN_EDIT_NAME:
      snprintf(keybNameBuf, NAME_LNG + 1, locoData[myLocoData].myName);
      txtData[TXT_NAME].maxLength = NAME_LNG;
      createObject(OBJ_WIN, WIN_EDIT_NAME);
      createObject(OBJ_TXT, TXT_NAME);
      createObject(OBJ_KEYBOARD, KEYB_NAME);
      createObject(OBJ_BUTTON, BUT_NAME_OK);
      createObject(OBJ_BUTTON, BUT_NAME_CNCL);
      newEvent(OBJ_WIN, WIN_EDIT_NAME, EVNT_DRAW);
      break;
    case WIN_FUNC:
      createObject(OBJ_WIN, WIN_FUNC);
      for (n = 0; n < 29; n++)
        createObject(OBJ_FNC, FNC_F0 + n);
      createObject(OBJ_LABEL, LBL_ADDR);
      createObject(OBJ_TXT, TXT_EDIT_ADDR);
      createObject(OBJ_LABEL, LBL_EDIT_FUNC);
      if (wifiSetting.protocol != CLIENT_ECOS)
        createObject(OBJ_BUTTON, BUT_FNC_OK);
      createObject(OBJ_BUTTON, BUT_FNC_CNCL);
      newEvent(OBJ_WIN, WIN_FUNC, EVNT_DRAW);
      break;
    case WIN_CHG_FUNC:
      createObject(OBJ_WIN, WIN_CHG_FUNC);
      createObject(OBJ_FNC, FNC_CHG);
      createObject(OBJ_TXT, TXT_EDIT_FNC);
      newEvent(OBJ_WIN, WIN_CHG_FUNC, EVNT_DRAW);
      break;
    case WIN_VMAX:
      createObject(OBJ_WIN, WIN_VMAX);
      createObject(OBJ_TXT, TXT_KEYB_VMAX);
      createObject(OBJ_KEYBOARD, KEYB_VMAX);
      newEvent(OBJ_WIN, WIN_VMAX, EVNT_DRAW);
      break;
    case WIN_SEL_LOCO:
      createObject(OBJ_WIN, WIN_SEL_LOCO);
      createObject(OBJ_DRAWSTR, DSTR_SELLOK);
      posObjStack1 = createObject(OBJ_ICON, ICON_LAST_UP);
      prepareLocoList();
      createObject(OBJ_ICON, ICON_SEL_LOK);
      if (wifiSetting.protocol != CLIENT_ECOS)
        createObject(OBJ_FNC, FNC_SEL_KEYPAD);
      createObject(OBJ_TXT, TXT_SEL_ADDR1);
      createObject(OBJ_TXT, TXT_SEL_NAME1);
      createObject(OBJ_TXT, TXT_SEL_ADDR2);
      createObject(OBJ_TXT, TXT_SEL_NAME2);
      createObject(OBJ_TXT, TXT_SEL_ADDR3);
      createObject(OBJ_TXT, TXT_SEL_NAME3);
      createObject(OBJ_TXT, TXT_SEL_ADDR4);
      createObject(OBJ_TXT, TXT_SEL_NAME4);
      createObject(OBJ_TXT, TXT_SEL_ADDR5);
      createObject(OBJ_TXT, TXT_SEL_NAME5);
      createObject(OBJ_TXT, TXT_SEL_ADDR6);
      createObject(OBJ_TXT, TXT_SEL_NAME6);
      newEvent(OBJ_WIN, WIN_SEL_LOCO, EVNT_DRAW);
      break;
    case WIN_ENTER_ADDR:
      locoKeybAddr[0] = '\0';
      createObject(OBJ_WIN, WIN_ENTER_ADDR);
      createObject(OBJ_TXT, TXT_KEYB_ADDR);
      createObject(OBJ_KEYBOARD, KEYB_ADDR);
      newEvent(OBJ_WIN, WIN_ENTER_ADDR, EVNT_DRAW);
      break;
    case WIN_SEL_IMAGE:
      createObject(OBJ_WIN, WIN_SEL_IMAGE);
      createObject(OBJ_LABEL, LBL_SEL_IMAGE);
      createObject(OBJ_BUTTON, BUT_IMAGE_CNCL);
      drawWindow(WIN_SEL_IMAGE);
      populateImageList();
      createObject(OBJ_LPIC, LPIC_SEL_IMG1);
      createObject(OBJ_LPIC, LPIC_SEL_IMG2);
      createObject(OBJ_LPIC, LPIC_SEL_IMG3);
      createObject(OBJ_LPIC, LPIC_SEL_IMG4);
      createObject(OBJ_LPIC, LPIC_SEL_IMG5);
      createObject(OBJ_LPIC, LPIC_SEL_IMG6);
      createObject(OBJ_ICON, ICON_PREV_IMAGE);
      createObject(OBJ_ICON, ICON_NEXT_IMAGE);
      newEvent(OBJ_WIN, WIN_SEL_IMAGE, EVNT_DRAW);
      break;
    case WIN_MENU:
      createObject(OBJ_WIN, WIN_MENU);
      createObject(OBJ_BUTTON, BUT_MENU_I_DRIVE);
      createObject(OBJ_BUTTON, BUT_MENU_T_DRIVE);
      createObject(OBJ_BUTTON, BUT_MENU_I_ACC);
      createObject(OBJ_BUTTON, BUT_MENU_T_ACC);
      createObject(OBJ_BUTTON, BUT_MENU_I_CV);
      createObject(OBJ_BUTTON, BUT_MENU_T_CV);
      createObject(OBJ_BUTTON, BUT_MENU_I_CFG);
      createObject(OBJ_BUTTON, BUT_MENU_T_CFG);
      createObject(OBJ_BUTTON, BUT_MENU_I_UTILS);
      createObject(OBJ_BUTTON, BUT_MENU_T_UTILS);
      createObject(OBJ_DRAWSTR, DSTR_MENU);
      newEvent(OBJ_WIN, WIN_MENU, EVNT_DRAW);
      break;
    case WIN_OPTIONS:
      setOptionsData();
      createObject(OBJ_WIN, WIN_OPTIONS);
      switch (wifiSetting.protocol) {
        case CLIENT_Z21:
          createObject(OBJ_SWITCH, SW_OPT_ADR);
          createObject(OBJ_LABEL, LBL_OPT_ADR);
          break;
        case CLIENT_XNET:
          break;
        case CLIENT_LNET:
          switchData[SW_OPT_DISCOVER].state = (autoIdentifyCS > 0) ? true : false;
          createObject(OBJ_SWITCH, SW_OPT_DISCOVER);
          createObject(OBJ_LABEL, LBL_OPT_DISCOVER);
          createObject(OBJ_RADIO, RAD_CSTATION);
          createObject(OBJ_LABEL, LBL_OPT_IB2);
          createObject(OBJ_LABEL, LBL_OPT_UHLI);
          createObject(OBJ_LABEL, LBL_OPT_DIG);
          break;
        case CLIENT_ECOS:
          break;
      }
      createObject(OBJ_BUTTON, BUT_OPT_OK);
      newEvent(OBJ_WIN, WIN_OPTIONS, EVNT_DRAW);
      break;
    case WIN_SPEEDO:
      speedoSpeed = 0;
      speedoPhase = SPD_WAIT;
      setSpeedoPhase(SPD_WAIT);
      setTextSpeedo();
      snprintf(spdLengthBuf, NAME_LNG + 1, "%d", speedoLength);
      snprintf(spdSpeedBuf, NAME_LNG + 1, "%d km/h", speedoSpeed);
      iconData[ICON_SPEEDO_LOK].x = 40 + (speedoPhase * 32);
      drawStrData[DSTR_SPEEDO_BLANK].x = 40 + (speedoPhase * 32);
      lpicData[LPIC_SPEEDO].id = locoData[myLocoData].myLocoID;
      createObject(OBJ_WIN, WIN_SPEEDO);
      createObject(OBJ_LPIC, LPIC_SPEEDO);
      createObject(OBJ_LABEL, LBL_SCALE);
      createObject(OBJ_LABEL, LBL_MM);
      createObject(OBJ_GAUGE, GAUGE_SPEEDO);
      createObject(OBJ_FNC, FNC_SPEEDO_DIR);
      createObject(OBJ_DRAWSTR, DSTR_SPEEDO_BLANK);
      createObject(OBJ_DRAWSTR, DSTR_SPEEDO_TRK);
      createObject(OBJ_ICON, ICON_SPEEDO_LOK);
      createObject(OBJ_ICON, ICON_SPEEDO_RADAR);
      createObject(OBJ_BUTTON, BUT_SPEEDO_CNCL);
      createObject(OBJ_BUTTON, BUT_SPEEDO_CV);
      createObject(OBJ_TXT, TXT_SPEEDO_SCALE);
      createObject(OBJ_TXT, TXT_SPEEDO_LNG);
      createObject(OBJ_TXT, TXT_SPEEDO_SPD);
      newEvent(OBJ_WIN, WIN_SPEEDO, EVNT_DRAW);
      break;
    case WIN_SPEEDO_LNG:
      snprintf(speedoKeybLng, PORT_LNG + 1, "%d", speedoLength);
      createObject(OBJ_WIN, WIN_SPEEDO_LNG);
      createObject(OBJ_TXT, TXT_EDIT_LNG);
      createObject(OBJ_KEYBOARD, KEYB_LNG);
      newEvent(OBJ_WIN, WIN_SPEEDO_LNG, EVNT_DRAW);
      break;
    case WIN_SPEEDO_SCALE:
      setTextSpeedo();
      createObject(OBJ_WIN, WIN_SPEEDO_SCALE);
      createObject(OBJ_TXT, TXT_EDIT_SCALE);
      createObject(OBJ_TXT, TXT_NUM_SCALE);
      createObject(OBJ_KEYBOARD, KEYB_SCALE);
      createObject(OBJ_BUTTON, BUT_SPEEDO_H0);
      createObject(OBJ_BUTTON, BUT_SPEEDO_N);
      createObject(OBJ_BUTTON, BUT_SPEEDO_TT);
      createObject(OBJ_BUTTON, BUT_SPEEDO_Z);
      createObject(OBJ_BUTTON, BUT_SPEEDO_0);
      newEvent(OBJ_WIN, WIN_SPEEDO_SCALE, EVNT_DRAW);
      break;
    case WIN_READ_CV:
      createObject(OBJ_WIN, WIN_READ_CV);
      createObject(OBJ_DRAWSTR, DSTR_CFG_MENU);
      createObject(OBJ_BUTTON, BUT_CV_ADDR);
      createObject(OBJ_BUTTON, BUT_CV_SPD_L);
      createObject(OBJ_BUTTON, BUT_CV_SPD_M);
      createObject(OBJ_BUTTON, BUT_CV_SPD_H);
      createObject(OBJ_BUTTON, BUT_CV_ACC);
      createObject(OBJ_BUTTON, BUT_CV_DEC);
      createObject(OBJ_BUTTON, BUT_CV_CFG);
      createObject(OBJ_BUTTON, BUT_CV_MAN);
      newEvent(OBJ_WIN, WIN_READ_CV, EVNT_DRAW);
      break;
    case WIN_PROG_CV:
      //buttonData[BUT_CV_LNCV].backgnd = (wifiSetting.protocol == CLIENT_LNET) ? COLOR_CREAM : COLOR_LIGHTBLACK;
      setFieldsCV();
      setBitsCV();
      setStatusCV();
      switchData[SW_POM].state = modeProg;
      createObject(OBJ_WIN, WIN_PROG_CV);
      createObject(OBJ_LABEL, LBL_CV);
      createObject(OBJ_LABEL, LBL_POM);
      createObject(OBJ_LABEL, LBL_BITS);
      createObject(OBJ_SWITCH, SW_POM);
      createObject(OBJ_BUTTON, BUT_CV_READ);
      createObject(OBJ_BUTTON, BUT_CV_CNCL);
      if (wifiSetting.protocol == CLIENT_LNET)
        createObject(OBJ_BUTTON, BUT_CV_LNCV);
      createObject(OBJ_KEYBOARD, KEYB_CV);
      createObject(OBJ_CHAR, CHAR_CV_EQUAL);
      createObject(OBJ_BUTTON, BUT_CV_0);
      createObject(OBJ_BUTTON, BUT_CV_1);
      createObject(OBJ_BUTTON, BUT_CV_2);
      createObject(OBJ_BUTTON, BUT_CV_3);
      createObject(OBJ_BUTTON, BUT_CV_4);
      createObject(OBJ_BUTTON, BUT_CV_5);
      createObject(OBJ_BUTTON, BUT_CV_6);
      createObject(OBJ_BUTTON, BUT_CV_7);
      createObject(OBJ_TXT, TXT_CV);
      createObject(OBJ_TXT, TXT_CV_VAL);
      createObject(OBJ_TXT, TXT_CV_STATUS);
      newEvent(OBJ_WIN, WIN_PROG_CV, EVNT_DRAW);
      break;
    case WIN_PROG_ADDR:
      getLabelTxt(LBL_CV_ADDR, buf);
      snprintf(cvStatusBuf, PWD_LNG + 1, "%s", buf);
      createObject(OBJ_WIN, WIN_PROG_ADDR);
      createObject(OBJ_TXT, TXT_CV_STATUS);
      createObject(OBJ_ICON, ICON_ADDR);
      createObject(OBJ_TXT, TXT_CV_ADDR);
      createObject(OBJ_KEYBOARD, KEYB_CV_ADDR);
      createObject(OBJ_BUTTON, BUT_ADDR_CNCL);
      newEvent(OBJ_WIN, WIN_PROG_ADDR, EVNT_DRAW);
      break;
    case WIN_PROG_LNCV:
      setFieldsLNCV();
      createObject(OBJ_WIN, WIN_PROG_LNCV);
      createObject(OBJ_KEYBOARD, KEYB_LNCV);
      createObject(OBJ_LABEL, LBL_LNCV_ART);
      createObject(OBJ_LABEL, LBL_LNCV_MOD);
      createObject(OBJ_LABEL, LBL_LNCV_NUM);
      createObject(OBJ_BUTTON, BUT_LNCV_FIND);
      createObject(OBJ_BUTTON, BUT_LNCV_CNCL);
      createObject(OBJ_TXT, TXT_LNCV_ART);
      createObject(OBJ_TXT, TXT_LNCV_MOD);
      createObject(OBJ_TXT, TXT_LNCV_ADR);
      createObject(OBJ_TXT, TXT_LNCV_VAL);
      createObject(OBJ_CHAR, CHAR_LNCV_EQUAL);
      newEvent(OBJ_WIN, WIN_PROG_LNCV, EVNT_DRAW);
      break;
    case WIN_STEAM:
      fncData[FNC_ST_SMOKE].state = false;
      createObject(OBJ_WIN, WIN_STEAM);
      checkLowBatt();
      createObject(OBJ_DRAWSTR, DSTR_STEAM);
      createObject(OBJ_ICON, ICON_POWER);
      createObject(OBJ_ICON, ICON_MANOMETER);
      createObject(OBJ_ICON, ICON_STEAM_EDIT);
      createObject(OBJ_ICON, ICON_LBATT_STEAM);
      createObject(OBJ_BUTTON, BUT_STEAM_CNCL);
      createObject(OBJ_FNC, FNC_ST_WATER);
      createObject(OBJ_FNC, FNC_ST_TENDER);
      createObject(OBJ_FNC, FNC_ST_WHISTLE);
      createObject(OBJ_FNC, FNC_ST_FIRE);
      createObject(OBJ_FNC, FNC_ST_SMOKE);
      createObject(OBJ_BAR, BAR_JOHNSON);
      createObject(OBJ_BAR, BAR_WATER);
      createObject(OBJ_BAR, BAR_TENDER);
      createObject(OBJ_BAR, BAR_BRAKE);
      newEvent(OBJ_WIN, WIN_STEAM, EVNT_DRAW);
      break;
    case WIN_UTIL:
      createObject(OBJ_WIN, WIN_UTIL);
      createObject(OBJ_BUTTON, BUT_UTL_I_SPEEDO);
      createObject(OBJ_BUTTON, BUT_UTL_T_SPEEDO);
      createObject(OBJ_BUTTON, BUT_UTL_I_STEAM);
      createObject(OBJ_BUTTON, BUT_UTL_T_STEAM);
      createObject(OBJ_BUTTON, BUT_UTL_I_SCAN);
      createObject(OBJ_BUTTON, BUT_UTL_T_SCAN);
      createObject(OBJ_BUTTON, BUT_UTL_I_STA);
      createObject(OBJ_BUTTON, BUT_UTL_T_STA);
      createObject(OBJ_ICON, ICON_UTL_EXIT);
      createObject(OBJ_DRAWSTR, DSTR_UTL_MENU);
      newEvent(OBJ_WIN, WIN_UTIL, EVNT_DRAW);
      break;
    case WIN_ACCESSORY:
      editAccessory = false;
      winData[WIN_ACCESSORY].backgnd = COLOR_WHITE;
      updateAccPanel();
      createObject(OBJ_WIN, WIN_ACCESSORY);
      createObject(OBJ_BUTTON, BUT_ACC_0);
      createObject(OBJ_BUTTON, BUT_ACC_1);
      createObject(OBJ_BUTTON, BUT_ACC_2);
      createObject(OBJ_BUTTON, BUT_ACC_3);
      createObject(OBJ_BUTTON, BUT_ACC_4);
      createObject(OBJ_BUTTON, BUT_ACC_5);
      createObject(OBJ_BUTTON, BUT_ACC_6);
      createObject(OBJ_BUTTON, BUT_ACC_7);
      createObject(OBJ_BUTTON, BUT_ACC_8);
      createObject(OBJ_BUTTON, BUT_ACC_9);
      createObject(OBJ_BUTTON, BUT_ACC_10);
      createObject(OBJ_BUTTON, BUT_ACC_11);
      createObject(OBJ_BUTTON, BUT_ACC_12);
      createObject(OBJ_BUTTON, BUT_ACC_13);
      createObject(OBJ_BUTTON, BUT_ACC_14);
      createObject(OBJ_BUTTON, BUT_ACC_15);
      createObject(OBJ_BUTTON, BUT_ACC_CNCL);
      createObject(OBJ_BUTTON, BUT_ACC_EDIT);
      createObject(OBJ_TXT, TXT_ACC_0);
      createObject(OBJ_TXT, TXT_ACC_1);
      createObject(OBJ_TXT, TXT_ACC_2);
      createObject(OBJ_TXT, TXT_ACC_3);
      createObject(OBJ_TXT, TXT_ACC_4);
      createObject(OBJ_TXT, TXT_ACC_5);
      createObject(OBJ_TXT, TXT_ACC_6);
      createObject(OBJ_TXT, TXT_ACC_7);
      createObject(OBJ_TXT, TXT_ACC_8);
      createObject(OBJ_TXT, TXT_ACC_9);
      createObject(OBJ_TXT, TXT_ACC_10);
      createObject(OBJ_TXT, TXT_ACC_11);
      createObject(OBJ_TXT, TXT_ACC_12);
      createObject(OBJ_TXT, TXT_ACC_13);
      createObject(OBJ_TXT, TXT_ACC_14);
      createObject(OBJ_TXT, TXT_ACC_15);
      createObject(OBJ_TXT, TXT_PANEL);
      newEvent(OBJ_WIN, WIN_ACCESSORY, EVNT_DRAW);
      break;
    case WIN_PANELS:
      createObject(OBJ_WIN, WIN_PANELS);
      createObject(OBJ_TXT, TXT_PANEL0);
      createObject(OBJ_TXT, TXT_PANEL1);
      createObject(OBJ_TXT, TXT_PANEL2);
      createObject(OBJ_TXT, TXT_PANEL3);
      createObject(OBJ_TXT, TXT_PANEL4);
      createObject(OBJ_TXT, TXT_PANEL5);
      createObject(OBJ_TXT, TXT_PANEL6);
      createObject(OBJ_TXT, TXT_PANEL7);
      createObject(OBJ_TXT, TXT_PANEL8);
      createObject(OBJ_TXT, TXT_PANEL9);
      createObject(OBJ_TXT, TXT_PANEL10);
      createObject(OBJ_TXT, TXT_PANEL11);
      createObject(OBJ_TXT, TXT_PANEL12);
      createObject(OBJ_TXT, TXT_PANEL13);
      createObject(OBJ_TXT, TXT_PANEL14);
      createObject(OBJ_TXT, TXT_PANEL15);
      newEvent(OBJ_WIN, WIN_PANELS, EVNT_DRAW);
      break;
    case WIN_PANEL_NAME:
      snprintf(keybNameBuf, PANEL_LNG + 1, panelNameBuf);
      txtData[TXT_NAME].maxLength = PANEL_LNG;
      createObject(OBJ_WIN, WIN_PANEL_NAME);
      createObject(OBJ_TXT, TXT_NAME);
      createObject(OBJ_KEYBOARD, KEYB_NAME);
      createObject(OBJ_BUTTON, BUT_NAME_OK);
      createObject(OBJ_BUTTON, BUT_NAME_CNCL);
      newEvent(OBJ_WIN, WIN_PANEL_NAME, EVNT_DRAW);
      break;
    case WIN_ACC_CTRL:
      snprintf(accKeybAddr, ADDR_LNG + 1, "%d", myTurnout);
      createObject(OBJ_WIN, WIN_ACC_CTRL);
      createObject(OBJ_TXT, TXT_ACC_ADDR);
      createObject(OBJ_KEYBOARD, KEYB_ACC);
      createObject(OBJ_ICON, ICON_KEYB_ACC);
      createObject(OBJ_BUTTON, BUT_ACC_RED);
      createObject(OBJ_BUTTON, BUT_ACC_GREEN);
      newEvent(OBJ_WIN, WIN_ACC_CTRL, EVNT_DRAW);
      break;
    case WIN_ACC_ASPECT:
      createObject(OBJ_WIN, WIN_ACC_ASPECT);
      createObject(OBJ_BUTTON, BUT_ACC_ASPECT0);
      createObject(OBJ_BUTTON, BUT_ACC_ASPECT1);
      createObject(OBJ_BUTTON, BUT_ACC_ASPECT2);
      if (currAccAspects == 4)
        createObject(OBJ_BUTTON, BUT_ACC_ASPECT3);
      newEvent(OBJ_WIN, WIN_ACC_ASPECT, EVNT_DRAW);
      break;
    case WIN_ACC_TYPE:
      createObject(OBJ_WIN, WIN_ACC_TYPE);
      createObject(OBJ_LABEL, LBL_ACC_TYPE);
      createObject(OBJ_FNC, FNC_ACC_TYPE);
      newEvent(OBJ_WIN, WIN_ACC_TYPE, EVNT_DRAW);
      break;
    case WIN_ACC_EDIT:
      n = accDef[currAccEdit.type].aspects;
      winData[WIN_ACC_EDIT].h = 130 + (n * 40);
      buttonData[BUT_TYPE_OK].y = 93 + (n * 40);
      buttonData[BUT_TYPE_CNCL].y = 93 + (n * 40);
      iconData[ICON_TYPE_OK].y = 97 + (n * 40);
      iconData[ICON_TYPE_CNCL].y = 97 + (n * 40);
      createObject(OBJ_WIN, WIN_ACC_EDIT);
      createObject(OBJ_LABEL, LBL_ACC_NAME);
      createObject(OBJ_LABEL, LBL_ACC_ADDR);
      createObject(OBJ_TXT, TXT_ACC_NAME);
      createObject(OBJ_TXT, TXT_ACC_ADDR1);
      createObject(OBJ_FNC, FNC_EDIT_ASPECT0);
      createObject(OBJ_BUTTON, BUT_ACC_OUT0);
      createObject(OBJ_BUTTON, BUT_ACC_OUT1);
      if (n > 1) {
        createObject(OBJ_FNC, FNC_EDIT_ASPECT1);
        createObject(OBJ_BUTTON, BUT_ACC_OUT4);
        createObject(OBJ_BUTTON, BUT_ACC_OUT5);
      }
      if (n > 2) {
        createObject(OBJ_TXT, TXT_ACC_ADDR2);
        createObject(OBJ_ICON, ICON_PLUS_ONE);
        createObject(OBJ_FNC, FNC_EDIT_ASPECT2);
        createObject(OBJ_BUTTON, BUT_ACC_OUT2);
        createObject(OBJ_BUTTON, BUT_ACC_OUT3);
        createObject(OBJ_BUTTON, BUT_ACC_OUT6);
        createObject(OBJ_BUTTON, BUT_ACC_OUT7);
        createObject(OBJ_BUTTON, BUT_ACC_OUT8);
        createObject(OBJ_BUTTON, BUT_ACC_OUT9);
        createObject(OBJ_BUTTON, BUT_ACC_OUT10);
        createObject(OBJ_BUTTON, BUT_ACC_OUT11);
      }
      if (n > 3) {
        createObject(OBJ_FNC, FNC_EDIT_ASPECT3);
        createObject(OBJ_BUTTON, BUT_ACC_OUT12);
        createObject(OBJ_BUTTON, BUT_ACC_OUT13);
        createObject(OBJ_BUTTON, BUT_ACC_OUT14);
        createObject(OBJ_BUTTON, BUT_ACC_OUT15);
      }
      createObject(OBJ_BUTTON, BUT_TYPE_OK);
      createObject(OBJ_BUTTON, BUT_TYPE_CNCL);
      newEvent(OBJ_WIN, WIN_ACC_EDIT, EVNT_DRAW);
      break;
    case WIN_ACC_NAME:
      snprintf(keybNameBuf, ACC_LNG + 1, accKeybName);
      txtData[TXT_NAME].maxLength = ACC_LNG;
      createObject(OBJ_WIN, WIN_ACC_NAME);
      createObject(OBJ_TXT, TXT_NAME);
      createObject(OBJ_KEYBOARD, KEYB_NAME);
      createObject(OBJ_BUTTON, BUT_NAME_OK);
      createObject(OBJ_BUTTON, BUT_NAME_CNCL);
      newEvent(OBJ_WIN, WIN_ACC_NAME, EVNT_DRAW);
      break;
    case WIN_ACC_ADDR1:
      snprintf(accKeybAdrEdit, ADDR_LNG + 1, "%d",  currAccEdit.addr);
      createObject(OBJ_WIN, WIN_ACC_ADDR1);
      createObject(OBJ_TXT, TXT_ACC_EDIT);
      createObject(OBJ_KEYBOARD, KEYB_ACC_ADDR);
      newEvent(OBJ_WIN, WIN_ACC_ADDR1, EVNT_DRAW);
      break;
    case WIN_ACC_ADDR2:
      snprintf(accKeybAdrEdit, ADDR_LNG + 1, "%d",  currAccEdit.addr2);
      createObject(OBJ_WIN, WIN_ACC_ADDR2);
      createObject(OBJ_TXT, TXT_ACC_EDIT);
      createObject(OBJ_KEYBOARD, KEYB_ACC_ADDR);
      newEvent(OBJ_WIN, WIN_ACC_ADDR2, EVNT_DRAW);
      break;
    case WIN_WIFI_SCAN:
      setTimer(TMR_SCAN, 5, TMR_ONESHOT);
      createObject(OBJ_WIN, WIN_WIFI_SCAN);
      createObject(OBJ_DRAWSTR, DSTR_WIFI_SCAN);
      createObject(OBJ_LABEL, LBL_SSID_SCAN);
      createObject(OBJ_FNC, FNC_SCAN_RESET);
      newEvent(OBJ_WIN, WIN_WIFI_SCAN, EVNT_DRAW);
      break;
    case WIN_STA_RUN:
      updateStationTime(staTime);
      updateStationLevel();
      updateStationStars();
      updateTargetStations();
      createObject(OBJ_WIN, WIN_STA_RUN);
      createObject(OBJ_LABEL, LBL_STA_RUN);
      createObject(OBJ_LABEL, LBL_STA_LEVEL);
      createObject(OBJ_LABEL, LBL_STA_INSTR);
      createObject(OBJ_FNC, FNC_STA_STARS);
      createObject(OBJ_ICON, ICON_STA_CLOCK);
      createObject(OBJ_ICON, ICON_STA_STATION);
      createObject(OBJ_ICON, ICON_STA_EDIT);
      createObject(OBJ_BUTTON, BUT_STA_START);
      createObject(OBJ_BUTTON, BUT_STA_CNCL);
      createObject(OBJ_TXT, TXT_STA_LEVEL);
      createObject(OBJ_TXT, TXT_STA_STARS);
      createObject(OBJ_TXT, TXT_STA_STATION);
      createObject(OBJ_TXT, TXT_STA_CLOCK);
      newEvent(OBJ_WIN, WIN_STA_RUN, EVNT_DRAW);
      break;
    case WIN_STA_PLAY:
      updateTurnoutButtons();
      fncData[FNC_STA_RAYO].state = isTrackOff();
      createObject(OBJ_WIN, WIN_STA_PLAY);
      createObject(OBJ_DRAWSTR, DSTR_STATION_PLAY);
      createObject(OBJ_ICON, ICON_STA_TARGET);
      createObject(OBJ_ICON, ICON_STA_TRAIN);
      createObject(OBJ_ICON, ICON_STA_PIN);
      createObject(OBJ_ICON, ICON_STA_TIME);
      createObject(OBJ_ICON, ICON_STA_COUNT);
      createObject(OBJ_TXT, TXT_STA_TIME);
      createObject(OBJ_TXT, TXT_STA_COUNT);
      createObject(OBJ_TXT, TXT_STA_STARC);
      createObject(OBJ_GAUGE, GAUGE_STATION);
      createObject(OBJ_FNC, FNC_STA_DIR);
      createObject(OBJ_FNC, FNC_STA_STARC);
      createObject(OBJ_FNC, FNC_STA_RAYO);
      createObject(OBJ_BUTTON, BUT_STA_STOP);
      switch (staMaxTurnout) {
        case 1:
          fncData[FNC_STA_ACC0].x = 104;
          buttonData[BUT_STA_ACC0].x = 100;
          createObject(OBJ_BUTTON, BUT_STA_ACC0);
          break;
        case 2:
          fncData[FNC_STA_ACC0].x = 54;
          fncData[FNC_STA_ACC1].x = 154;
          buttonData[BUT_STA_ACC0].x = 50;
          buttonData[BUT_STA_ACC1].x = 150;
          createObject(OBJ_BUTTON, BUT_STA_ACC0);
          createObject(OBJ_BUTTON, BUT_STA_ACC1);
          break;
        case 3:
          fncData[FNC_STA_ACC0].x = 40;
          fncData[FNC_STA_ACC1].x = 104;
          fncData[FNC_STA_ACC2].x = 168;
          buttonData[BUT_STA_ACC0].x = 36;
          buttonData[BUT_STA_ACC1].x = 100;
          buttonData[BUT_STA_ACC2].x = 164;
          createObject(OBJ_BUTTON, BUT_STA_ACC0);
          createObject(OBJ_BUTTON, BUT_STA_ACC1);
          createObject(OBJ_BUTTON, BUT_STA_ACC2);
          break;
        default:
          fncData[FNC_STA_ACC0].x = 20;
          fncData[FNC_STA_ACC1].x = 76;
          fncData[FNC_STA_ACC2].x = 132;
          fncData[FNC_STA_ACC3].x = 188;
          buttonData[BUT_STA_ACC0].x = 16;
          buttonData[BUT_STA_ACC1].x = 72;
          buttonData[BUT_STA_ACC2].x = 128;
          buttonData[BUT_STA_ACC3].x = 184;
          createObject(OBJ_BUTTON, BUT_STA_ACC0);
          createObject(OBJ_BUTTON, BUT_STA_ACC1);
          createObject(OBJ_BUTTON, BUT_STA_ACC2);
          createObject(OBJ_BUTTON, BUT_STA_ACC3);
          break;
      }
      newEvent(OBJ_WIN, WIN_STA_PLAY, EVNT_DRAW);
      break;
    case WIN_STA_STARS:
      createObject(OBJ_WIN, WIN_STA_STARS);
      if (staCurrTime > 0) {
        staStars++;
        createObject(OBJ_FNC, FNC_STA_STAR1);
        if (staCurrTime > 10) {          // time remaining
          staStars++;
          createObject(OBJ_FNC, FNC_STA_STAR2);
          createObject(OBJ_LABEL, LBL_STA_EXCEL);
        }
        else {
          createObject(OBJ_LABEL, LBL_STA_GREAT);
        }
        updateStationStars();
      }
      else {
        createObject(OBJ_ICON, ICON_STA_TIMEOUT);
        createObject(OBJ_LABEL, LBL_STA_TIMEOUT);
      }
      newEvent(OBJ_WIN, WIN_STA_STARS, EVNT_DRAW);
      break;
    case WIN_STA_EDIT:
      snprintf(staStartTimeBuf, IP_LNG + 1, "%d", staStartTime);
      snprintf(staStatNumBuf, IP_LNG + 1, "%d", staMaxStations);
      snprintf(staTurnNumBuf, IP_LNG + 1, "%d", staMaxTurnout);
      snprintf(staTurnout1Buf, ADDR_LNG + 1, "%d", staTurnoutAdr1);
      snprintf(staTurnout2Buf, ADDR_LNG + 1, "%d", staTurnoutAdr2);
      snprintf(staTurnout3Buf, ADDR_LNG + 1, "%d", staTurnoutAdr3);
      snprintf(staTurnout4Buf, ADDR_LNG + 1, "%d", staTurnoutAdr4);
      for (n = 0; n < 8; n++)
        switchData[SW_STA_OR1 + n].state = bitRead(staTurnoutDef, n);
      createObject(OBJ_WIN, WIN_STA_EDIT);
      createObject(OBJ_LABEL, LBL_STA_STATIONS);
      createObject(OBJ_LABEL, LBL_STA_TURNOUTS);
      createObject(OBJ_LABEL, LBL_STA_TIME);
      createObject(OBJ_LABEL, LBL_STA_DESC);
      createObject(OBJ_TXT, TXT_STA_STARTTIME);
      createObject(OBJ_TXT, TXT_STA_STATNUM);
      createObject(OBJ_TXT, TXT_STA_TURNNUM);
      createObject(OBJ_TXT, TXT_STA_TURNOUT1);
      createObject(OBJ_TXT, TXT_STA_TURNOUT2);
      createObject(OBJ_TXT, TXT_STA_TURNOUT3);
      createObject(OBJ_TXT, TXT_STA_TURNOUT4);
      createObject(OBJ_BUTTON, BUT_STA_EDIT);
      createObject(OBJ_SWITCH, SW_STA_OR1);
      createObject(OBJ_SWITCH, SW_STA_OR2);
      createObject(OBJ_SWITCH, SW_STA_OR3);
      createObject(OBJ_SWITCH, SW_STA_OR4);
      createObject(OBJ_SWITCH, SW_STA_INV1);
      createObject(OBJ_SWITCH, SW_STA_INV2);
      createObject(OBJ_SWITCH, SW_STA_INV3);
      createObject(OBJ_SWITCH, SW_STA_INV4);
      createObject(OBJ_BUTTON, BUT_STA_STAM);
      createObject(OBJ_BUTTON, BUT_STA_STAP);
      createObject(OBJ_BUTTON, BUT_STA_TURNM);
      createObject(OBJ_BUTTON, BUT_STA_TURNP);
      newEvent(OBJ_WIN, WIN_STA_EDIT, EVNT_DRAW);
      break;
    case WIN_STA_KEYB:
      createObject(OBJ_WIN, WIN_STA_KEYB);
      createObject(OBJ_KEYBOARD, KEYB_STA);
      newEvent(OBJ_WIN, WIN_STA_KEYB, EVNT_DRAW);
      break;
    case WIN_DEF_ACTION:
      createObject(OBJ_WIN, WIN_DEF_ACTION);
      createObject(OBJ_LABEL, LBL_DEF_ACTION);
      createObject(OBJ_TXT, TXT_ACTION_BOOT);
      createObject(OBJ_ICON, ICON_PREV_ACT);
      createObject(OBJ_ICON, ICON_NEXT_ACT);
      createObject(OBJ_BUTTON, BUT_ACT_OK);
#if (USE_RGB_LED == FUNC_BUTTONS)
      createObject(OBJ_TXT, TXT_ACTION_R);
      createObject(OBJ_TXT, TXT_ACTION_G);
      createObject(OBJ_TXT, TXT_ACTION_B);
      createObject(OBJ_ICON, ICON_PREV_ACT_R);
      createObject(OBJ_ICON, ICON_NEXT_ACT_R);
      createObject(OBJ_ICON, ICON_PREV_ACT_G);
      createObject(OBJ_ICON, ICON_NEXT_ACT_G);
      createObject(OBJ_ICON, ICON_PREV_ACT_B);
      createObject(OBJ_ICON, ICON_NEXT_ACT_B);
#endif
      newEvent(OBJ_WIN, WIN_DEF_ACTION, EVNT_DRAW);
      break;
    case WIN_BATT:
      buttonData[BUT_LOW_BATT].border = COLOR_WHITE;
      buttonData[BUT_FULL_BATT].border = COLOR_WHITE;
      createObject(OBJ_WIN, WIN_BATT);
      createObject(OBJ_LABEL, LBL_BATT);
      createObject(OBJ_DRAWSTR, DSTR_BATT);
      createObject(OBJ_BAR, BAR_BATT);
      createObject(OBJ_ICON, ICON_BATT_CHARGE);
      createObject(OBJ_BUTTON, BUT_LOW_BATT);
      createObject(OBJ_BUTTON, BUT_FULL_BATT);
      createObject(OBJ_BUTTON, BUT_BATT_OK);
      //createObject(OBJ_BUTTON, BUT_BATT_CNCL);
      createObject(OBJ_BUTTON, BUT_BATT_CFG);
      newEvent(OBJ_WIN, WIN_BATT, EVNT_DRAW);
      break;
  }
}


void alertWindow(byte err) {
  errType = err;
  createObject(OBJ_WIN, WIN_ALERT);
  switch (err) {
    case ERR_SERV:
      createObject(OBJ_ICON, ICON_WARNING);
      createObject(OBJ_ICON, ICON_WARNING_ON);
      createObject(OBJ_LABEL, LBL_SERVICE);
      break;
    case ERR_CHG_WIFI:
      createObject(OBJ_ICON, ICON_INFO);
      createObject(OBJ_LABEL, LBL_CHG_WIFI);
      break;
    case ERR_FULL:
      createObject(OBJ_ICON, ICON_WARNING);
      createObject(OBJ_ICON, ICON_WARNING_ON);
      createObject(OBJ_LABEL, LBL_STACK_FULL);
      break;
    case ERR_STOP:
      createObject(OBJ_ICON, ICON_ESTOP);
      createObject(OBJ_LABEL, LBL_ESTOP);
      break;
    case ERR_WAIT:
    case ERR_CV:
      barData[BAR_WAIT].value = 0;
      setTimer(TMR_WAIT, 5, TMR_ONESHOT);
      if (err == ERR_WAIT)
        createObject(OBJ_ICON, ICON_WAIT);
      else
        createObject(OBJ_ICON, ICON_WAIT_CV);
      createObject(OBJ_BAR, BAR_WAIT);
      break;
    case ERR_ASK_SURE:
      createObject(OBJ_ICON, ICON_WARNING);
      createObject(OBJ_ICON, ICON_WARNING_ON);
      createObject(OBJ_LABEL, LBL_ASK_SURE);
      createObject(OBJ_BUTTON, BUT_SURE_OK);
      createObject(OBJ_BUTTON, BUT_SURE_CNCL);
      break;
    case ERR_FILE:
      createObject(OBJ_ICON, ICON_WARNING);
      createObject(OBJ_ICON, ICON_WARNING_ON);
      createObject(OBJ_LABEL, LBL_NOT_FOUND);
      break;
  }
  newEvent(OBJ_WIN, WIN_ALERT, EVNT_DRAW);
}

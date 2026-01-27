/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

////////////////////////////////////////////////////////////
// ***** SYSTEM SUPPORT *****
////////////////////////////////////////////////////////////

void initPins() {
  // Set all chip selects high to avoid bus contention during initialisation of each peripheral
  digitalWrite(TFT_CS, HIGH);                                     // TFT screen chip select
  digitalWrite(SD_CS,  HIGH);                                     // SD card chips select
  digitalWrite(XPT2046_CS, HIGH);                                 // Touch screen chips select
  pinMode (SW_BOOT,    INPUT);                                    // Button BOOT
  pinMode (ENCODER_A,  INPUT);                                    // Encoder
  pinMode (ENCODER_B,  INPUT);
  pinMode (ENCODER_SW, INPUT);
#if (USE_RGB_LED == PRESENT)
  pinMode(RGB_LED_R, OUTPUT);                                     // RGB LED
  pinMode(RGB_LED_G, OUTPUT);
  pinMode(RGB_LED_B, OUTPUT);
  setColorRGB(0);                                                 // turn off RGB LED
#endif
}


void setBacklight (uint8_t value) {                               // set PWM backlight
#if (ESP_ARDUINO_VERSION_MAJOR > 2)
  // Code for version 3.x
  ledcWrite(TFT_BL, value);
#else
  // Code for version 2.x
  ledcWrite(LEDC_CHANNEL_0, value);
#endif
  currBacklight = value;
}

void setRotationDisplay(uint8_t pos) {                            // Rotate display and touchscreen
  tft.setRotation(pos);
  touchscreen.setRotation((pos + XPT_ROTATION) & 0x03);
}

void aliveAndKicking() {
  setTimer (TMR_BLIGHT, INACT_TIME, TMR_ONESHOT);                 // reset timeout and restore backlight
  if (currBacklight != backlight)
    setBacklight(backlight);
}

#if (USE_RGB_LED == PRESENT)
void setColorRGB (uint16_t color) {                               // set color of RGB LED
  int state;
  state = (color & 0x04) ? LOW : HIGH;
  digitalWrite(RGB_LED_G, state);
  state = (color & 0x02) ? LOW : HIGH;
  digitalWrite(RGB_LED_R, state);
  state = (color & 0x01) ? LOW : HIGH;
  digitalWrite(RGB_LED_B, state);
  DEBUG_MSG("Color: %d", color & 0x07)
}
#endif

initResult initSequence() {                                       // Performs init sequence
  char label[MAX_LABEL_LNG];
  char chr;
  int n;
  initResult result;
  result = INIT_OK;
  delay(500);
  drawObject(OBJ_ICON, ICON_SDCARD);                              // detecting SD card
  if (sdDetected) {
    sprintf (FileName, "/image/logo.bmp");
    if (tft.width() == 240)
      drawBmp (FileName, 0, 180);
    else
      drawBmp (FileName, 40, 260);
    loadLocoFiles(SD, "/loco");                                   // load loco data & panel names from SD file
    loadAccPanelNames(SD);
  }
  else {
    if (LittleFS.begin(false)) {
      loadLocoFiles(LittleFS, "/loco");                           // load loco data & panel names from FS file
      loadAccPanelNames(LittleFS);
    }
    else {
      DEBUG_MSG("LittleFS Mount Failed. Formating....");
      LittleFS.format();
    }
    drawObject(OBJ_ICON, ICON_NO_SD);
    result = INIT_NO_SD;
    DEBUG_MSG("Total: %ul Used: %ul", LittleFS.totalBytes(), LittleFS.usedBytes())
  }
  populateAccPanel();                                             // load first accessory panel
  barData[BAR_INIT].value = 10;
  drawObject(OBJ_BAR, BAR_INIT);
  drawObject(OBJ_ICON, ICON_WIFI);                                // connecting to WiFi network
  drawObject(OBJ_DRAWSTR, DSTR_INIT_STAT);
  drawObject(OBJ_LABEL, LBL_CONNECT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSetting.ssid, wifiSetting.password);
  n = 0;
  while ((WiFi.status() != WL_CONNECTED) && n < 80) {             // tries to connect to router in 20 seconds
    n += 2;
    barData[BAR_INIT].value = 10 + n;
    drawObject(OBJ_BAR, BAR_INIT);
    delay(500);
    DEBUG_MSG(".");
  }
  barData[BAR_INIT].value = 90;
  drawObject(OBJ_BAR, BAR_INIT);
  if (WiFi.status() == WL_CONNECTED) {                            // Connect to server with current protocol
    drawObject(OBJ_DRAWSTR, DSTR_INIT_STAT);                      // show Protocol
    getLabelTxt(LBL_SEL_Z21 + wifiSetting.protocol, label);
    tft.drawString(label, 20, 120, GFXFF);
    DEBUG_MSG("Channel: %d", WiFi.channel());
    DEBUG_MSG("IP address: %u.%u.%u.%u", WiFi.localIP().operator[](0), WiFi.localIP().operator[](1), WiFi.localIP().operator[](2), WiFi.localIP().operator[](3));
    DEBUG_MSG("%s", WiFi.macAddress().c_str())
    useID = false;
    switch (wifiSetting.protocol) {
      case CLIENT_Z21:
        WiFi.setSleep(false);
        Udp.begin(z21Port);
        //wifiSetting.port = z21Port;
        DEBUG_MSG("Now listening UDP port %d", z21Port);
        getStatusZ21();                                           // every x seconds
        getSerialNumber();
        delay(500);
        setBroadcastFlags (0x00000013);                           // Broadcasts and info messages concerning driving and switching, report changes on feedback bus & fast clock
        getStatusZ21();
        //askZ21begin (LAN_GET_BROADCASTFLAGS);
        //sendUDP (0x04);
        break;
      case CLIENT_LNET:
        if (!Client.connect(wifiSetting.CS_IP, wifiSetting.port)) {
          DEBUG_MSG("Connection to Loconet over TCP/IP failed");
          result = INIT_NO_CONNECT;
        }
        else {
          Client.setNoDelay(true);
          rcvStrPhase = WAIT_TOKEN;
          getTypeCS();
        }
        break;
      case CLIENT_XNET:
        if (!Client.connect(wifiSetting.CS_IP, XnetPort)) {
          DEBUG_MSG("Connection to Xpressnet failed");
          result = INIT_NO_CONNECT;
        }
        else {
          wifiSetting.port = XnetPort;
          Client.setNoDelay(true);
          rxIndice = FRAME1;
          getVersionXnet();                                       // pide la version del Xpressnet
          getStatusXnet();                                        // pide estado de la central
        }
        break;
      case CLIENT_ECOS:
        useID = true;
        if (!Client.connect(wifiSetting.CS_IP, ECoSPort)) {
          DEBUG_MSG("Connection to ECoS failed");
          result = INIT_NO_CONNECT;
        }
        else {
          wifiSetting.port = ECoSPort;
          Client.setNoDelay(true);
          requestViews();
          requestLocoList();
          waitWifiData(500);
        }
        break;
    }
  }
  else {
    drawObject(OBJ_ICON, ICON_NO_WIFI);
    result = INIT_NO_WIFI;
  }
  barData[BAR_INIT].value = 95;
  drawObject(OBJ_BAR, BAR_INIT);
  drawObject(OBJ_ICON, ICON_INIT_LOCO);                           // fill image list
  initImageList();
  barData[BAR_INIT].value = 100;
  drawObject(OBJ_BAR, BAR_INIT);
  setTimer (TMR_END_LOGO, 7, TMR_ONESHOT);                        // Wait for answer
  return result;
}


bool notLocked () {                                               // check if not locked
  if (lockOptions & ((1 << LOCK_SEL_LOCO) | (1 << LOCK_TURNOUT) | (1 << LOCK_PROG)))
    return false;
  else
    return true;
}


bool notLockedOption (byte opt) {                                 // check if option not locked
  if (lockOptions & (1 << opt))
    return false;
  else
    return true;
}


////////////////////////////////////////////////////////////
// ***** TOUCHSCREEN *****
////////////////////////////////////////////////////////////

void calibrateTouchscreen(uint16_t colorIn, uint16_t colorOut, uint16_t bg) {
  uint16_t TS_TOP, TS_BOT, TS_LEFT, TS_RT;
  uint16_t x, y, z;
  TSPoint p;
  TS_TOP = 4095;
  TS_BOT = 0;
  TS_LEFT = 4095;
  TS_RT = 0;
  tft.fillScreen(bg);
  for (int i = 0; i < 4; i++) {
    tft.fillCircle(0, 0, 15, bg);                                 // delete touch corners points
    tft.fillCircle(tft.width(), 0, 15, bg);
    tft.fillCircle(0, tft.height(), 15, bg);
    tft.fillCircle(tft.width(), tft.height(), 15, bg);
    DEBUG_MSG("Calibrate step: %d", i)
    switch (i) {                                                  // show current touch corner point
      case 0:
        tft.fillCircle(0, 0, 15, colorOut);
        tft.fillCircle(0, 0, 7, colorIn);
        break;
      case 1:
        tft.fillCircle(tft.width(), 0, 15, colorOut);
        tft.fillCircle(tft.width(), 0, 7, colorIn);
        break;
      case 2:
        tft.fillCircle(0, tft.height(), 15, colorOut);
        tft.fillCircle(0, tft.height(), 7, colorIn);
        break;
      case 3:
        tft.fillCircle(tft.width(), tft.height(), 15, colorOut);
        tft.fillCircle(tft.width(), tft.height(), 7, colorIn);
        break;
    }
    while (touchscreen.touched())                                 // wait to release
      delay(0);
    DEBUG_MSG("Pen released")
    while (!touchscreen.touched())                                // wait to touch
      delay(0);
    DEBUG_MSG("Pen touched")
    touchscreen.readData(&x, &y, &z);
    if (x < TS_LEFT) {
      TS_LEFT = x;
    }
    if (y < TS_TOP) {
      TS_TOP = y;
    }
    if (x > TS_RT) {
      TS_RT = x;
    }
    if (y > TS_BOT) {
      TS_BOT = y;
    }
  }
  tft.fillCircle(tft.width(), tft.height(), 15, bg);              // delete last touch corner point
  touchscreen.setCalibration(TS_LEFT, TS_RT, TS_TOP, TS_BOT);
  DEBUG_MSG("xMin: %d, xMax: %d, yMin: %d, yMax: %d", TS_LEFT, TS_RT, TS_TOP, TS_BOT);
}


void showClockData(uint16_t txtFocus) {
  uint16_t n;
  for (n = 0; n < 3; n++)
    txtData[TXT_HOUR + n].backgnd = COLOR_BACKGROUND;
  txtData[txtFocus].backgnd = COLOR_YELLOW;                       // select focus on selected field
  keybData[KEYB_CLOCK].idTextbox = txtFocus;
}

////////////////////////////////////////////////////////////
// ***** WIFI *****
////////////////////////////////////////////////////////////

void scanWiFi() {
  networks = 0;
  while (networks == 0) {
    WiFi.disconnect(true); //DISCONNECT WITH TRUE (SHUOLD TURN OFF THE RADIO)
    delay(1000);
    WiFi.mode(WIFI_STA); //CALLING THE WIFI MODE AS STATION
    WiFi.scanDelete();
    networks = WiFi.scanNetworks();
    DEBUG_MSG("Networks: %d", networks);
    if ((networks > 0) && (networks < 32768)) {
      encoderMax = networks - 1;
      encoderValue = 0;
      scrSSID = 0;
    }
    else
      networks = 0;
  }
}


void scanWiFiFill() {
  uint16_t n, line;
  n = (scrSSID > 5) ? scrSSID - 5 : 0;
  snprintf (ssidName1, SSID_LNG, WiFi.SSID(n).c_str());
  snprintf (ssidName2, SSID_LNG, WiFi.SSID(n + 1).c_str());
  snprintf (ssidName3, SSID_LNG, WiFi.SSID(n + 2).c_str());
  snprintf (ssidName4, SSID_LNG, WiFi.SSID(n + 3).c_str());
  snprintf (ssidName5, SSID_LNG, WiFi.SSID(n + 4).c_str());
  snprintf (ssidName6, SSID_LNG, WiFi.SSID(n + 5).c_str());
  line = (scrSSID > 5) ? 5 : scrSSID;
  for (n = 0; n < 6; n++) {
    txtData[TXT_SSID1 + n].backgnd = (n == line) ? COLOR_BLUE : COLOR_BLACK;
  }
}


void wifiAnalyzer() {
  int16_t n, i;
  char txt[10];
  for (n = 0; n < 14; n++) {
    ap_count[n] = 0;
    max_rssi[n] = RSSI_FLOOR;
  }
  n = WiFi.scanNetworks();
  drawObject(OBJ_DRAWSTR, DSTR_WIFI_SCAN);
  drawObject(OBJ_LABEL, LBL_SSID_SCAN);
  drawObject(OBJ_FNC, FNC_SCAN_RESET);
  tft.setFreeFont(FSSB6);
  if ((n > 0) && (n < 32768)) {
    for (i = 0; i < n; i++) {
      int32_t channel = WiFi.channel(i);
      int32_t rssi = WiFi.RSSI(i);
      uint16_t color = channel_color[channel - 1];
      int height = constrain(map(rssi, RSSI_FLOOR, RSSI_CEILING, 1, GRAPH_HEIGHT), 1, GRAPH_HEIGHT);
      // channel stat
      ap_count[channel - 1]++;
      if (rssi > max_rssi[channel - 1]) {
        max_rssi[channel - 1] = rssi;
      }
      tft.drawLine((channel * CHANNEL_WIDTH) + GRAPH_OFFSET, GRAPH_BASELINE - height, ((channel - 1) * CHANNEL_WIDTH) + GRAPH_OFFSET, GRAPH_BASELINE + 1, color);
      tft.drawLine((channel * CHANNEL_WIDTH) + GRAPH_OFFSET, GRAPH_BASELINE - height, ((channel + 1) * CHANNEL_WIDTH) + GRAPH_OFFSET, GRAPH_BASELINE + 1, color);
      // Print SSID, signal strengh and if not encrypted
      tft.setTextColor(color);
      tft.setTextDatum(MC_DATUM);
      tft.drawString(WiFi.SSID(i), (channel * CHANNEL_WIDTH) + GRAPH_OFFSET, GRAPH_BASELINE - 10 - height, GFXFF);
      // rest for WiFi routine?
      delay(10);
    }
  }
  else {
    tft.setFreeFont(FSSB9);
    tft.setTextColor(COLOR_WHITE);
    tft.drawString("SSID = 0", 120 + GRAPH_OFFSET, 120, GFXFF);
  }
  tft.setFreeFont(FSSB6);
  tft.setTextDatum(TC_DATUM);
  for (i = 1; i < 15; i++) {
    tft.setTextColor(channel_color[i - 1]);
    snprintf(txt, 10, "%d", i);
    tft.drawString(txt, (i * CHANNEL_WIDTH) + GRAPH_OFFSET, GRAPH_BASELINE + 12, GFXFF);
    if (ap_count[i - 1] > 0) {
      snprintf(txt, 10, "(%d)", ap_count[i - 1]);
      tft.drawString(txt, (i * CHANNEL_WIDTH) + GRAPH_OFFSET, GRAPH_BASELINE + 24, GFXFF);
    }
  }
  setTimer(TMR_SCAN, 50, TMR_ONESHOT);
}


void wifiProcess() {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      processZ21();
      break;
    case CLIENT_XNET:
      processXnet();
      break;
    case CLIENT_LNET:
      processLnet();
      break;
    case CLIENT_ECOS:
      ECoSProcess();
      break;
  }
}


void waitWifiData(uint32_t ms) {
  uint32_t now;
  now = millis();
  while ((millis() - now) < ms)
    wifiProcess();
}


void setProtocolData() {
  uint16_t n;
  useID = false;
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      snprintf(keybProtoBuf, PWD_LNG, "Z21");
      snprintf(keybPortBuf, 6, "%d", z21Port);
      break;
    case CLIENT_LNET:
      if (wifiSetting.serverType)
        snprintf(keybProtoBuf, PWD_LNG, "LBServer");
      else
        snprintf(keybProtoBuf, PWD_LNG, "Loconet Binary");
      snprintf(keybPortBuf, 6, "%d", wifiSetting.port);
      break;
    case CLIENT_XNET:
      snprintf(keybProtoBuf, PWD_LNG, "Xpressnet LAN");
      snprintf(keybPortBuf, 6, "%d", XnetPort);
      break;
    case CLIENT_ECOS:
      useID = true;
      snprintf(keybProtoBuf, PWD_LNG, "ECoS");
      snprintf(keybPortBuf, 6, "%d", ECoSPort);
      break;
  }
  for (n = 0; n < 5; n++)
    txtData[TXT_IP1 + n].backgnd = COLOR_BACKGROUND;
  txtData[TXT_IP1].backgnd = COLOR_YELLOW;                        // select focus on first IP byte
  keybData[KEYB_IP].idTextbox = TXT_IP1;
}

void setOptionsData() {
  switchData[SW_OPT_ADR].state = false;                           // show disable all as default
  switchData[SW_OPT_ADR].colorKnob = COLOR_BACKGROUND;
  radioData[RAD_CSTATION].value = radioData[RAD_CSTATION].num;
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      switchData[SW_OPT_ADR].colorKnob = COLOR_WHITE;
      switchData[SW_OPT_ADR].state = (shortAddress == 99) ? true : false;
      break;
    case CLIENT_XNET:
      break;
    case CLIENT_LNET:
      radioData[RAD_CSTATION].value = typeCmdStation;
      break;
    case CLIENT_ECOS:
      break;
  }
}


////////////////////////////////////////////////////////////
// ***** PROTOCOL COMMON *****
////////////////////////////////////////////////////////////

void infoLocomotora (unsigned int loco) {
  DEBUG_MSG("Info Loco % d", loco)
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      infoLocomotoraZ21 (loco);
      break;
    case CLIENT_XNET:
      infoLocomotoraXnet (loco);
      break;
    case CLIENT_LNET:
      infoLocomotoraLnet (loco);
      break;
    case CLIENT_ECOS:
      infoLocomotoraECoS (loco);       // ID
      break;
  }
}


void locoOperationSpeed() {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      locoOperationSpeedZ21();
      break;
    case CLIENT_XNET:
      locoOperationSpeedXnet();
      break;
    case CLIENT_LNET:
      locoOperationSpeedLnet();
      break;
    case CLIENT_ECOS:
      locoOperationSpeedECoS();
      break;
  }
}

void changeDirection() {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      locoOperationSpeedZ21();
      break;
    case CLIENT_XNET:
      locoOperationSpeedXnet();
      break;
    case CLIENT_LNET:
      changeDirectionF0F4Lnet();
      break;
    case CLIENT_ECOS:
      changeDirectionECoS();
      break;
  }
}

void funcOperations (uint8_t fnc) {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      funcOperationsZ21 (fnc);
      break;
    case CLIENT_XNET:
      funcOperationsXnet (fnc);
      break;
    case CLIENT_LNET:
      funcOperationsLnet (fnc);
      break;
    case CLIENT_ECOS:
      funcOperationsECoS(fnc);
      break;
  }
}

byte getCurrentStep() {
  byte value;
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      value = getCurrentStepZ21();
      break;
    case CLIENT_XNET:
      value = getCurrentStepXnet();
      break;
    case CLIENT_LNET:
      value = getCurrentStepLnet();
      break;
    case CLIENT_ECOS:
      value = getCurrentStepECoS();
      break;
  }
  return value;
}

void releaseLoco() {
  switch (wifiSetting.protocol) {
    case CLIENT_LNET:
      doDispatchGet = false;
      doDispatchPut = false;
      liberaSlot();                                               // pasa slot actual a COMMON
      break;
    case CLIENT_ECOS:
      releaseLocoECoS();
      break;
  }
}


void sendAccessory(unsigned int FAdr, int pair, bool activate) {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      setAccessoryZ21(FAdr, pair, activate);
      break;
    case CLIENT_XNET:
      setAccessoryXnet(FAdr, activate, pair);
      break;
    case CLIENT_LNET:
      lnetRequestSwitch (FAdr, activate, pair);
      break;
    case CLIENT_ECOS:
      setAccessoryECoS(FAdr, pair, activate);
      break;
  }
}


void resumeOperations() {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      resumeOperationsZ21();
      break;
    case CLIENT_XNET:
      resumeOperationsXnet();
      break;
    case CLIENT_LNET:
      resumeOperationsLnet();
      break;
    case CLIENT_ECOS:
      resumeOperationsECoS();
      break;
  }
}


void emergencyOff() {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      emergencyOffZ21();
      break;
    case CLIENT_XNET:
      emergencyOffXnet();
      break;
    case CLIENT_LNET:
      emergencyOffLnet();
      break;
    case CLIENT_ECOS:
      emergencyOffECoS();
      break;
  }
}


void togglePower() {
  if (isTrackOff())
    resumeOperations();                                           //  Track Power On
  else
    emergencyOff();                                               //  Track Power Off
}


bool isTrackOff() {
  bool state;
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      state = (csStatus & csTrackVoltageOff) ? true : false;
      break;
    case CLIENT_XNET:
      state =  (csStatus & csEmergencyOff) ? true : false;
      break;
    case CLIENT_LNET:
      state = (bitRead(mySlot.trk, 0)) ? false : true;
      break;
    case CLIENT_ECOS:
      state = (csStatus > 0) ? false : true;
      break;
  }
  return state;
}

void getStatusCS() {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      getStatusZ21();
      break;
    case CLIENT_XNET:
      getStatusXnet();
      break;
    case CLIENT_LNET:
      getTypeCS();                                                // workaround, not defined for Lnet
      break;
    case CLIENT_ECOS:
      getStatusECoS();
      break;
  }
}

void setTime(byte hh, byte mm, byte rate) {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      setTimeZ21(hh, mm, rate);
      break;
    case CLIENT_XNET:
      setTimeXnet(hh, mm, rate);
      break;
    case CLIENT_LNET:
      setTimeLnet(hh, mm, rate);
      break;
      // ECoS not supported
  }
}


void readCV (unsigned int adr, byte stepPrg) {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      readCVZ21(adr, stepPrg);
      break;
    case CLIENT_XNET:
      readCVXnet(adr, stepPrg);
      break;
    case CLIENT_LNET:
      readCVLnet(adr, stepPrg);
      break;
    case CLIENT_ECOS:
      readCVECoS(adr, stepPrg);
      break;
  }
}


void writeCV (unsigned int adr, unsigned int data, byte stepPrg) {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      writeCVZ21(adr, data, stepPrg);
      break;
    case CLIENT_XNET:
      writeCVXnet(adr, data, stepPrg);
      break;
    case CLIENT_LNET:
      writeCVLnet(adr, data, stepPrg);
      break;
    case CLIENT_ECOS:
      writeCVECoS(adr, data, stepPrg);
      break;
  }
}


void exitProgramming() {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
      if (csStatus & csProgrammingModeActive)
        resumeOperationsZ21();
      break;
    case CLIENT_XNET:
      if (csStatus & csServiceMode)
        resumeOperationsXnet();
      break;
    case CLIENT_ECOS:
      exitProgrammingECoS();
      break;
  }
}

////////////////////////////////////////////////////////////
// ***** CV PROGRAMMING *****
////////////////////////////////////////////////////////////


void endProg() {                                                  // Fin de programcion/lectura CV
  DEBUG_MSG("END PROG: CVData - % d Step: % d", CVdata, progStepCV);
  if (CVdata > 255) {
    if (progStepCV == PRG_RD_CV29)                                // Si buscaba direccion, muestra CV1 en lugar de CV29
      CVaddress = 1;
    showDataCV();
  }
  else {
    switch (progStepCV) {
      case PRG_CV:
        showDataCV();
        break;
      case PRG_RD_CV29:
        cv29 = (byte) CVdata;
        if (bitRead(cv29, 5)) {
          CVaddress = 17;                                         // Long address
          readCV(CVaddress, PRG_RD_CV17);
        }
        else {
          CVaddress = 1;                                          // Short address
          readCV(CVaddress, PRG_RD_CV1);
        }
        break;
      case PRG_RD_CV1:
        decoAddress = CVdata;
        showDirCV();
        break;
      case PRG_RD_CV17:
        cv17 = (byte) CVdata;
        CVaddress = 18;
        readCV(CVaddress, PRG_RD_CV18);
        break;
      case PRG_RD_CV18:
        cv18 = (byte) CVdata;
        decoAddress = ((cv17 & 0x3F) << 8) | cv18;
        showDirCV();
        break;
      case PRG_WR_CV17:                                           // Long address
        CVaddress = 18;
        writeCV(CVaddress, lowByte(decoAddress), PRG_WR_CV18);
        break;
      case PRG_WR_CV18:
        bitSet(cv29, 5);
        CVaddress = 29;
        writeCV(CVaddress, cv29, PRG_WR_CV29);
        break;
      case PRG_WR_CV1:                                            // short address
        bitClear(cv29, 5);
        CVaddress = 29;
        writeCV(CVaddress, cv29, PRG_WR_CV29);
        break;
      case PRG_WR_CV29:
        showDirCV();
        break;
    }
  }
}



void showDataCV() {                                               // muestra valor de la CV
  progStepCV = PRG_IDLE;
  enterCVdata =  (CVdata > 255) ? false : true;
  setStatusCV();                                                  // show error / manufacturer / CV / pom
  setFieldsCV();
  setBitsCV();
  if (isWindow(WIN_ALERT))
    closeWindow(WIN_ALERT);
  if (isWindow(WIN_PROG_CV)) {
    showFieldsCV();
    newEvent(OBJ_TXT, TXT_CV_STATUS, EVNT_DRAW);
  }
  if (wifiSetting.protocol == CLIENT_LNET)
    progUhli(UHLI_PRG_END);
}


void showDirCV() {                                                // muestra direccion de la locomotora segun sus CV
  progStepCV = PRG_IDLE;
  setStatusCV();                                                  // show error / manufacturer / CV / pom
  setFieldsCV();
  setBitsCV();
  if (isWindow(WIN_ALERT))
    closeWindow(WIN_ALERT);
  sprintf(locoEditAddr, " % d", decoAddress);
  openWindow(WIN_PROG_ADDR);
  if (wifiSetting.protocol == CLIENT_LNET)
    progUhli(UHLI_PRG_END);
  if (wifiSetting.protocol != CLIENT_ECOS)
    pushLoco(decoAddress);                                        // mete esta loco en el stack
}


void readBasicCV (uint16_t num) {
  closeWindow(WIN_READ_CV);
  if (num == 1) {
    readCV(29, PRG_RD_CV29);
  }
  else {
    CVaddress = num;
    readCV(num, PRG_CV);
  }
  alertWindow(ERR_CV);
}

////////////////////////////////////////////////////////////
// ***** EEPROM *****
////////////////////////////////////////////////////////////

void saveCalibrationValues() {
  TouchCalibration cal;
  cal = touchscreen.getCalibration();
  EEPROM.write (EE_XMIN_H, highByte(cal.xMin));
  EEPROM.write (EE_XMIN_L, lowByte(cal.xMin));
  EEPROM.write (EE_XMAX_H, highByte(cal.xMax));
  EEPROM.write (EE_XMAX_L, lowByte(cal.xMax));
  EEPROM.write (EE_YMIN_H, highByte(cal.yMin));
  EEPROM.write (EE_YMIN_L, lowByte(cal.yMin));
  EEPROM.write (EE_YMAX_H, highByte(cal.yMax));
  EEPROM.write (EE_YMAX_L, lowByte(cal.yMax));
  EEPROM.commit();
}

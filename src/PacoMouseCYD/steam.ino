/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

////////////////////////////////////////////////////////////
// ***** STEAM THROTTLE *****
////////////////////////////////////////////////////////////

void initSteamThrottle () {
  uint16_t n;
  if (oldSteamLoco != locoData[myLocoData].myAddr.address) {
    oldSteamLoco = locoData[myLocoData].myAddr.address;
    steamPressure = 80;
    waterLevelBoiler = 80;
    waterLevelTender = 350;
    oldPressure = 0;
    oldLevelBoiler = 0;
    oldLevelTender = 0;
    barData[BAR_JOHNSON].value = STEAM_JOHNSON_NEUTRAL;                           // neutral position
    barData[BAR_BRAKE].value = 0;
    steamDir = locoData[myLocoData].myDir;
    for (n = 0; n < MAX_LIMIT; n++)
      steamSpeedLimit[n] = LIMIT_NONE;
  }
  currentSteamTime = millis();
  steamTimeSpeed = currentSteamTime;
  steamTimeSteam = currentSteamTime;
  steamTimeWater = currentSteamTime;
  steamTimeLoad  = currentSteamTime;
  steamTimeSmoke = currentSteamTime;
  shovelCoal = false;
  setFirebox();
  endWaterInjection();
  endTenderFill();
  setTimer(TMR_STEAM, 5, TMR_ONESHOT);
  changeSmoke = STEAM_SMOKE_FAST;
  oldPressure = 0;
  oldSpeedSteam = 305;
  encoderMax = 31;                                                                // set throttle to current speed
  updateSteamThrottle();
  if (steamDir != locoData[myLocoData].myDir) {                                   // check Johnson bar position
    steamDir = locoData[myLocoData].myDir;
    barData[BAR_JOHNSON].value = 6 - barData[BAR_JOHNSON].value;
  }
}

void updateSteamThrottle() {
  switch (wifiSetting.protocol) {
    case CLIENT_Z21:
    case CLIENT_XNET:
      if (bitRead(locoData[myLocoData].mySteps, 2)) {                             // 0..127
        currentSteamSpeed = locoData[myLocoData].mySpeed;
        encoderValue = currentSteamSpeed >> 2;
      }
      else {
        if (bitRead(locoData[myLocoData].mySteps, 1)) {                           // 0..31
          encoderValue = (locoData[myLocoData].mySpeed & 0x0F) << 1;
          if (bitRead(locoData[myLocoData].mySpeed, 4))
            bitSet(encoderValue, 0);
          currentSteamSpeed = (encoderValue > 3) ? (encoderValue << 2) + (encoderValue >> 3) : 0;
        }
        else {                                                                    // 0..15
          encoderValue = locoData[myLocoData].mySpeed & 0x0F;
          encoderValue = (encoderValue > 1) ? encoderValue << 1 : 0;
          currentSteamSpeed = (encoderValue << 2) + (encoderValue >> 3);
        }
      }
      break;
    case CLIENT_LNET:
    case CLIENT_ECOS:
      currentSteamSpeed = locoData[myLocoData].mySpeed;
      encoderValue = currentSteamSpeed >> 2;
      break;
  }
}


void steamProcess() {
  uint16_t value, newSpeed, mappedThrottle, jFactor;

  steamSpeedLimit[LIMIT_THROTTLE] = (encoderValue << 2) + (encoderValue >> 3);    // Read Throtle Speed (0..31 -> 0..127)

  steamSpeedLimit[LIMIT_JOHNSON] = LIMIT_NONE;
  switch (barData[BAR_JOHNSON].value) {                                           // Read Johnson Bar
    case 0:                                                                       // full reverse
      jFactor = 3;
      changeSpeed = 80;
      steamDir = 0x00;
      break;
    case 1:
      jFactor = 2;
      changeSpeed = 200;
      steamDir = 0x00;
      break;
    case 2:
      jFactor = 1;
      changeSpeed = 500;
      steamDir = 0x00;
      break;
    case STEAM_JOHNSON_NEUTRAL:                                                   // Neutral position of Johnson Bar
      jFactor = 1;
      changeSpeed = 1000;
      steamSpeedLimit[LIMIT_JOHNSON] = 0;
      break;
    case 4:
      jFactor = 1;
      changeSpeed = 500;
      steamDir = 0x80;
      break;
    case 5:
      jFactor = 2;
      changeSpeed = 200;
      steamDir = 0x80;
      break;
    case 6:                                                                         // full forward
      jFactor = 3;
      changeSpeed = 80;
      steamDir = 0x80;
      break;
  }

  if (steamDir != locoData[myLocoData].myDir) {                                    // Check direction
    locoData[myLocoData].myDir = steamDir;
    changeDirection();
    DEBUG_MSG("STEAM: Change direction")
  }

  value = steamSpeedLimit[LIMIT_THROTTLE];
  changeSteam = 10000 - ((value * 9) * jFactor);                                  // Steam timeout: 6571 to 10000
  changeWater = 14000 - ((value * 27) * jFactor);                                 // Water timeout: 3713 to 14000
  if (barData[BAR_BRAKE].value > 0) {                                             // Brake bar: 300, 150, 100
    changeSpeed = 300 / barData[BAR_BRAKE].value;
  }
  currentSteamTime = millis();

  if (currentSteamTime > (steamTimeWater + changeWater)) {                         // Water consumption
    steamTimeWater = currentSteamTime;
    if (waterLevelBoiler > 0) {
      waterLevelBoiler--;
      DEBUG_MSG("Boiler Level: %d", waterLevelBoiler)
    }
  }
  if (waterLevelBoiler < 10) {                                                     // Stop loco if not enough water
    steamSpeedLimit[LIMIT_WATER] = 0;
    steamThrottleStop();
  }
  else {
    steamSpeedLimit[LIMIT_WATER] = LIMIT_NONE;
  }

  if (currentSteamTime > (steamTimeSteam + changeSteam)) {                         // Steam consumption
    steamTimeSteam = currentSteamTime;
    if (steamPressure > 0)
      steamPressure--;
  }

  if (steamPressure < 50) {                                                       // Limit speed based on steam level
    value = (steamPressure < 20) ? 0 : map(steamPressure, 20, 50, 20, 120);
    steamSpeedLimit[LIMIT_PRESSURE] = value;
  }
  else {
    steamSpeedLimit[LIMIT_PRESSURE] = LIMIT_NONE;
  }

  if (currentSteamTime > (steamTimeLoad + STEAM_LOAD_TIME)) {                   // Load coal and water
    steamTimeLoad = currentSteamTime;
    if (shovelCoal) {                                                           // Fire open for shoveling coal
      if (steamPressure > 96) {
        shovelCoal = false;
        setFirebox();
        newEvent(OBJ_FNC, FNC_ST_FIRE, EVNT_DRAW);
      }
      else {
        if (steamPressure < 20)                                                 // slowly pressure up at beginning
          steamPressure += 1;
        else
          steamPressure += 2;
      }
    }
    if (waterInjection) {                                                       // Water injector open
      if (waterLevelTender > 0) {                                               // Inject water with water from tender
        if (waterLevelBoiler > 95) {
          endWaterInjection();
        }
        else {
          waterLevelBoiler += 2;
          waterLevelTender--;
        }
        steamSpeedLimit[LIMIT_TENDER] = LIMIT_NONE;
      }
      else {
        endWaterInjection();                                                    // Stop locomotive if tender empty
        steamSpeedLimit[LIMIT_TENDER] = 0;
        steamThrottleStop();
      }
    }
    if (fillTender) {
      if ((waterLevelTender > 495) || (currentSteamSpeed != 0)) {               // Only fill tender when stopped
        endTenderFill();
      }
      else {
        waterLevelTender++;
        if (waterLevelTender > 6)                                               // Minimum level to run again
          steamSpeedLimit[LIMIT_TENDER] = LIMIT_NONE;
      }
    }
  }

  if (currentSteamTime > (steamTimeSmoke + changeSmoke)) {                      // Chimney smoke
    steamTimeSmoke = currentSteamTime;
    if (currentSteamSpeed > 0) {
      fncData[FNC_ST_SMOKE].state = !fncData[FNC_ST_SMOKE].state;
      changeSmoke = map(currentSteamSpeed, 0, 127, STEAM_SMOKE_SLOW, STEAM_SMOKE_FAST);
    }
    else {
      fncData[FNC_ST_SMOKE].state = false;
      changeSmoke = STEAM_SMOKE_SLOW + STEAM_SMOKE_SLOW;
    }
    newEvent(OBJ_FNC, FNC_ST_SMOKE, EVNT_DRAW);
  }

  if (barData[BAR_BRAKE].value > 0) {                                           // Braking
    value = barData[BAR_BRAKE].value * 8;
    value = (currentSteamSpeed > value) ? (currentSteamSpeed - value) : 0;
    steamSpeedLimit[LIMIT_BRAKE] = value;
  }
  else {
    steamSpeedLimit[LIMIT_BRAKE] = LIMIT_NONE;
  }

  targetSpeedSteam = LIMIT_NONE;                                                // Find lower limit
  for (value = 0; value < MAX_LIMIT; value++) {
    if (steamSpeedLimit[value] < targetSpeedSteam)
      targetSpeedSteam = steamSpeedLimit[value];
  }

  newSpeed = currentSteamSpeed;
  if (currentSteamTime > (steamTimeSpeed + changeSpeed)) {                      // Speed acceleration
    steamTimeSpeed = currentSteamTime;
    //DEBUG_MSG("Target: %d Current: %d New: %d", targetSpeedSteam, currentSteamSpeed, newSpeed)
    if (targetSpeedSteam > currentSteamSpeed) {
      newSpeed = currentSteamSpeed + 1;
      DEBUG_MSG("Inc New: %d", newSpeed)
    }
    if (targetSpeedSteam < currentSteamSpeed) {
      newSpeed = currentSteamSpeed - 1;
      DEBUG_MSG("Dec New: %d", newSpeed)
    }
    //DEBUG_MSG("New acc: %d", newSpeed)
  }


  if (currentSteamSpeed != newSpeed) {                                          // changes in speed
    DEBUG_MSG("Step: %d - New: %d  LIMITS ", currentSteamSpeed, newSpeed)
#ifdef DEBUG
    for (value = 0; value < MAX_LIMIT; value++) {
      Serial.print(steamSpeedLimit[value]);
      Serial.print(" ");
    }
    Serial.println();
#endif
    currentSteamSpeed = newSpeed;
    switch (wifiSetting.protocol) {
      case CLIENT_Z21:
      case CLIENT_XNET:
        if (bitRead(locoData[myLocoData].mySteps, 2)) {                         // 128 steps
          mappedThrottle = (currentSteamSpeed > 1) ? currentSteamSpeed : 0;
        }
        else {
          if (bitRead(locoData[myLocoData].mySteps, 1)) {                       // 28 steps
            if (currentSteamSpeed > 15) {
              mappedThrottle = currentSteamSpeed >> 3;
              bitWrite(mappedThrottle, 4, bitRead(currentSteamSpeed, 2));
            }
            else {
              mappedThrottle = 0;
            }
          }
          else {                                                                // 14 steps
            mappedThrottle = (currentSteamSpeed > 15) ? currentSteamSpeed >> 3 : 0;
          }
        }
        break;
      case CLIENT_LNET:
      case CLIENT_ECOS:
        mappedThrottle = (currentSteamSpeed > 1) ? currentSteamSpeed : 0;
        break;
    }
    locoData[myLocoData].mySpeed = mappedThrottle;
    locoOperationSpeed();
    DEBUG_MSG("Steam step: %d", currentSteamSpeed)
    if ((currentSteamSpeed > 0) && (changeSmoke > STEAM_SMOKE_SLOW)) {          // initial chuff
      changeSmoke = 0;
      fncData[FNC_ST_SMOKE].state = false;
    }
  }

  if ((oldLevelTender / 10) != (waterLevelTender / 10)) {
    oldLevelTender = waterLevelTender;
    barData[BAR_TENDER].value = waterLevelTender;
    newEvent(OBJ_BAR, BAR_TENDER, EVNT_DRAW);
  }

  value = waterLevelBoiler / 2;
  if (oldLevelBoiler != value ) {
    oldLevelBoiler = value;
    barData[BAR_WATER].value = value;
    newEvent(OBJ_BAR, BAR_WATER, EVNT_DRAW);
  }

  value = steamPressure * 270 / 100;
  if (oldPressure != value) {
    showPressure(value);
    DEBUG_MSG("Pressure: %d", steamPressure)
  }
}


void startWaterInjection () {
  waterInjection = true;
  fncData[FNC_ST_WATER].colorOn = COLOR_DARKGREEN;
  drawObject(OBJ_FNC, FNC_ST_WATER);
}


void endWaterInjection () {
  waterInjection = false;
  fncData[FNC_ST_WATER].colorOn = COLOR_RED;
  drawObject(OBJ_FNC, FNC_ST_WATER);
}


void startTenderFill() {
  fillTender = true;
  //fncData[FNC_ST_TENDER].color   = COLOR_RED;
  fncData[FNC_ST_TENDER].colorOn = COLOR_DARKGREEN;
  drawObject(OBJ_FNC, FNC_ST_TENDER);
}


void endTenderFill() {
  fillTender = false;
  //fncData[FNC_ST_TENDER].color   = COLOR_WHITE;
  fncData[FNC_ST_TENDER].colorOn = COLOR_RED;
  drawObject(OBJ_FNC, FNC_ST_TENDER);
}


void setFirebox() {
  if (shovelCoal) {
    fncData[FNC_ST_FIRE].idIcon = FNC_FIRE_OP_OFF;
    fncData[FNC_ST_FIRE].color = COLOR_ORANGE;
    fncData[FNC_ST_FIRE].colorOn = COLOR_YELLOW;
  }
  else {
    fncData[FNC_ST_FIRE].idIcon = FNC_FIRE_CL_OFF;
    fncData[FNC_ST_FIRE].color = COLOR_SILVER;
    fncData[FNC_ST_FIRE].colorOn = COLOR_RED;
  }
}


void steamThrottleStop() {                                                      // set controls for stop
  if (encoderValue > 0) {
    encoderValue = 0;
    showSpeedSteam(240);
  }
  if (barData[BAR_JOHNSON].value != STEAM_JOHNSON_NEUTRAL) {
    barData[BAR_JOHNSON].value = STEAM_JOHNSON_NEUTRAL;                         // Neutral
    drawObject(OBJ_BAR, BAR_JOHNSON);
  }
}


void showPressure(uint16_t angle) {
  tft.setPivot(140, 105);                                                       // Set pivot to center of manometer in TFT screen
  sprite.setColorDepth(8);                                                      // Create an 8bpp Sprite
  sprite.createSprite(19, 19);                                                  // 8bpp requires 19 * 19 = 361 bytes
  sprite.setPivot(9, 9);                                                        // Set pivot relative to top left corner of Sprite
  sprite.fillSprite(COLOR_WHITE);                                               // Fill the Sprite with background
  sprite.pushRotated(oldPressure);
  oldPressure = angle;
  sprite.drawBitmap(0, 0, needle_bar, 19, 19, COLOR_BLUE);
  sprite.pushRotated(angle);
  sprite.deleteSprite();
}

void showSpeedSteam(uint16_t angle) {
  tft.setPivot(120, 170);                                                       // Set pivot to center of bar in TFT screen
  sprite.setColorDepth(8);                                                      // Create an 8bpp Sprite
  sprite.createSprite(83, 15);                                                  // 8bpp requires 83 * 15 = 1245 bytes
  sprite.setPivot(76, 7);                                                       // Set pivot relative to top left corner of Sprite
  sprite.fillSprite(COLOR_BLACK);                                               // Fill the Sprite with background
  sprite.pushRotated(oldSpeedSteam);
  oldSpeedSteam = angle;
  sprite.drawBitmap(0, 0, speed_steam, 83, 15, COLOR_RED);
  sprite.pushRotated(angle);
  tft.drawArc(120, 170, 27, 22, 315, 45, COLOR_RED, COLOR_BLACK, false);
  sprite.deleteSprite();
}

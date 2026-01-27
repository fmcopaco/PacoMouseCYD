/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/


////////////////////////////////////////////////////////////
// ***** STATION RUN - MODEL TRAIN GAME FOR KIDS *****
////////////////////////////////////////////////////////////

void updateStationTime (uint16_t seconds) {
  snprintf(staTimeBuf, ACC_LNG + 1, "%d:%02d", seconds / 60, seconds % 60);
}

void updateStationTarget() {
  staStations = staLevel + 4;
  if (staStartTime < 10)
    staStartTime = 10;
  staTime = staStartTime + ((staLevel - 1) * 10);
}

void newStationCounters (bool ini) {
  if (ini) {
    staStars = 0;
    staLevel = 1;
    randomSeed(millis());
  }
  staCurrStation = 0;
  updateStationTarget();
}

uint8_t newStation(byte last) {
  uint8_t station;                                // genera numero estacion sin repetir la ultima
  do {
    station = random (0, staMaxStations);
  } while (station == last);
  return (station);
}

void updateTargetStations() {
  snprintf(staStationsBuf, ACC_LNG + 1, "%d", staStations);
}

void updateCountStations() {
  snprintf(staStationsBuf, ACC_LNG + 1, "%d/%d", staCurrStation, staStations);
}

void updateStationLevel() {
  snprintf(staLevelBuf, ADDR_LNG + 1, "%d", staLevel);
}

void updateStationStars() {
  snprintf(staStarsBuf, ADDR_LNG + 1, "%d", staStars);
}

void setNewTarget() {
  uint16_t pos;
  staLastStation = newStation(staLastStation);
  iconData[ICON_STA_TARGET].color = staColors[staLastStation];
  pos = iconData[ICON_STA_TRAIN].x;
  iconData[ICON_STA_TRAIN].x = iconData[ICON_STA_TARGET].x;
  iconData[ICON_STA_TARGET].x = pos;
  iconData[ICON_STA_PIN].x = pos + 8;
}

void clickTargetStation() {
  encoderValue = 0;
  locoData[myLocoData].mySpeed = 0;
  locoOperationSpeed();
  updateSpeedDir();
  staCurrStation++;
  if (staCurrStation == staStations) {
    stopTimer(TMR_STA_RUN);
    staLevel++;
    updateStationLevel();
    newStationCounters(false);
    updateTargetStations();
    updateStationTime(staTime);
    closeWindow(WIN_STA_PLAY);
    openWindow(WIN_STA_STARS);                // well done!
  }
  else {
    updateCountStations();
    setNewTarget();
    newEvent(OBJ_WIN, WIN_STA_PLAY, EVNT_DRAW);
  }
}

uint16_t  staGetTurnoutAdr(uint16_t eeAdr, uint16_t defAdr) {
  uint16_t adr;
  adr = (EEPROM.read(eeAdr) << 8) + EEPROM.read(eeAdr + 1);
  if (adr > 2048)
    adr = defAdr;
  return adr;
}

void updateTurnoutButtons() {
  uint16_t n, icon;
  for (n = 0; n < 4; n++) {
    if (staTurnoutPos[n])
      fncData[FNC_STA_ACC0 + n].idIcon = bitRead(staTurnoutDef, n) ? FNC_TURNRD_OFF : FNC_TURNLD_OFF;
    else
      fncData[FNC_STA_ACC0 + n].idIcon = bitRead(staTurnoutDef, n) ? FNC_TURNRS_OFF : FNC_TURNLS_OFF;
    fncData[FNC_STA_ACC0 + n].colorOn = staTurnoutPos[n] ? COLOR_RED : COLOR_GREEN;
  }
}

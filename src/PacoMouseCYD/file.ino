/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/


bool checkName(char *fileName) {
  bool result;
  uint16_t lng;
  result = false;
  lng = strlen(fileName);
  if (lng > 4) {
    if ((fileName[lng - 4] == '.') && (fileName[lng - 3] == 'c') && (fileName[lng - 2] == 's') && (fileName[lng - 1] == 'v'))
      return true;
  }
  return result;
}

bool saveLocoData(fs::FS &fs, uint16_t pos) {                           // save loco data in .csv file
  char field[30];
  uint16_t cnt;
  bool dataOK, isDir;
  File myFile;
  dataOK = false;
  myFile = fs.open("/loco");
  if (myFile) {
    isDir = myFile.isDirectory();
    myFile.close();
    if (!isDir)
      return dataOK;
    DEBUG_MSG("/loco is a directory");
  }
  else {
    DEBUG_MSG("Directory /loco not found. Creating...")
    fs.mkdir("/loco");
  }
  sprintf (field, "/loco/%d.csv", locoData[pos].myAddr.address);
  myFile = fs.open(field, FILE_WRITE);
  if (myFile) {
    DEBUG_MSG("File %s opened for writting", field);
    getLabelTxt(LBL_NAME, field);                         // Header
    myFile.print(field);
    myFile.print(CSV_FILE_DELIMITER);
    getLabelTxt(LBL_IMAGE, field);
    myFile.print(field);
    myFile.print(CSV_FILE_DELIMITER);
    getLabelTxt(LBL_VMAX, field);
    myFile.print(field);
    for (cnt = 0; cnt < 29; cnt++) {
      myFile.print(CSV_FILE_DELIMITER);
      myFile.print('F');
      myFile.print(cnt);
    }
    myFile.print("\r\n");
    myFile.print(locoData[pos].myName);                   // Loco data
    myFile.print(CSV_FILE_DELIMITER);
    myFile.print(locoData[pos].myLocoID);
    myFile.print(CSV_FILE_DELIMITER);
    myFile.print(locoData[pos].myVmax);
    for (cnt = 0; cnt < 29; cnt++) {
      myFile.print(CSV_FILE_DELIMITER);
      myFile.print(locoData[pos].myFuncIcon[cnt] >> 1);
    }
    myFile.print("\r\n");
    myFile.close();
    dataOK = true;
  }
  return dataOK;
}


void deleteLocoData (fs::FS &fs, uint16_t num) {
  char line[200];
  sprintf (line, "/loco/%d.csv", num);
  if (fs.remove(line)) {
    DEBUG_MSG("File %s deleted", line);
  }
  else {
    DEBUG_MSG("File %s delete failed", line);
  }
}


bool readLocoData(fs::FS &fs, uint16_t num, uint16_t pos) {            // read loco data from .csv file
  char line[200];
  bool dataOK;
  uint16_t n;
  File myFile;
  dataOK = false;
  sprintf (line, "/loco/%d.csv", num);
  myFile = fs.open(line);
  if (myFile) {
    if (readCSV(myFile, line, sizeof(line), false)) {                                     // skip header line
      if (readCSV(myFile, locoData[pos].myName, sizeof(locoData[pos].myName), true)) {    // read data field: Name
        if (readCSV(myFile, line, sizeof(line), true)) {                                  // read data field: Picture
          locoData[pos].myLocoID = atoi(line);
          if (readCSV(myFile, line, sizeof(line), true)) {
            locoData[pos].myVmax = atoi(line);
            DEBUG_MSG("%d %s %d %d", num, locoData[pos].myName, locoData[pos].myLocoID, locoData[pos].myVmax)
            for (n = 0; n < 29; n++) {
              if (!readCSV(myFile, line, sizeof(line), true)) {                             // read data field: Functions
                myFile.close();
                return false;
              }
              locoData[pos].myFuncIcon[n] = (uint8_t)(atoi(line) << 1);
              //DEBUG_MSG("Func: %d - %d", n, locoData[pos].myFuncIcon[n])
            }
            locoData[pos].myAddr.address = num;
            dataOK = true;
          }
        }
      }
    }
    myFile.close();
  }
  return dataOK;
}


bool readCSV(File & f, char* line, uint16_t maxLen, bool getField) {
  uint16_t n;                                               // read field or line from CSV file
  char chr;
  n = 0;
  yield();
  while (n < maxLen) {
    chr = f.read();
    switch (chr) {
      case 0:
        line[n] = '\0';
        return false;                                       // EOF
        break;
      case '\r':
        line[n] = '\0';
        break;
      case '\n':
        line[n] = '\0';
        return true;
        break;
      case ',':
      case ';':
        if (getField) {
          line[n] = '\0';
          return true;
        }
        break;
      default:
        line[n++] = chr;
        break;
    }
  }
  return false;                                             // too long
}


void loadLocoFiles(fs::FS &fs, const char * dirname) {
  uint16_t pos, adr;
  char nameFile[50];
  File myFile;
  if (wifiSetting.protocol == CLIENT_ECOS)
    return;
  File root = fs.open(dirname);
  if (!root) {
    //DEBUG_MSG("Failed to open directory %s", dirname);
    return;
  }
  if (!root.isDirectory()) {
    //DEBUG_MSG("%s Not a directory", dirname);
    return;
  }
  pos = 0;
  File file = root.openNextFile();
  while (file) {
    if (! file.isDirectory()) {
      if (pos < LOCOS_IN_STACK) {
        sprintf(nameFile, "%s", file.name());
        adr = atoi(nameFile);
        //DEBUG_MSG("%d %s", adr, nameFile);
        if (readLocoData(fs, adr, pos++))
          pushLoco(adr);
      }
    }
    file = root.openNextFile();
  }
}

void initImageList() {
  uint16_t n, maxImg;
  uint16_t pos, id;
  char nameFile[50];
  File myFile;

  for (n = 0; n < MAX_LOCO_IMAGE; n++)                          // add to list system images
    locoImages[n] = (n < (MAX_SYS_LPIC - 1)) ? n + 1 : 0;
  if (wifiSetting.protocol != CLIENT_ECOS) {
    if (sdDetected) {                                           // add  to list user images from SD
      File root = SD.open("/image");
      if (root) {
        if (root.isDirectory()) {
          pos = MAX_SYS_LPIC - 1;
          File file = root.openNextFile();
          while (file) {
            if (! file.isDirectory()) {
              if (pos < MAX_LOCO_IMAGE) {
                sprintf(nameFile, "%s", file.name());
                id = atoi(nameFile);
                if (id >= 1000) {
                  locoImages[pos++] = id;
                }
              }
            }
            file = root.openNextFile();
          }
        }
      }
    }
  }
  locoImageIndex = 0;
}


void populateImageList() {
  uint16_t n;
  for (n = 0; n < 6; n++)
    lpicData[LPIC_SEL_IMG1 + n].id = locoImages[locoImageIndex + n];
}


bool saveCurrAccPanel(fs::FS &fs) {
  char field[30];
  uint16_t cnt;
  bool dataOK, isDir;
  File myFile;
  dataOK = false;
  myFile = fs.open("/acc");
  if (myFile) {
    isDir = myFile.isDirectory();
    myFile.close();
    if (!isDir)
      return dataOK;
    DEBUG_MSG("/acc is a directory");
  }
  else {
    DEBUG_MSG("Directory /acc not found. Creating...")
    fs.mkdir("/acc");
  }
  sprintf (field, "/acc/%d.csv", currPanel);
  myFile = fs.open(field, FILE_WRITE);
  if (myFile) {
    DEBUG_MSG("File %s opened for writting", field);
    getLabelTxt(LBL_ACC_TYPE, field);                         // Header
    myFile.print(field);
    myFile.print(CSV_FILE_DELIMITER);
    getLabelTxt(LBL_ACC_ADDR, field);
    myFile.print(field);
    myFile.print(CSV_FILE_DELIMITER);
    getLabelTxt(LBL_ACC_ADDR, field);
    myFile.print(field);
    myFile.print(CSV_FILE_DELIMITER);
    getLabelTxt(LBL_ACC_NAME, field);
    myFile.print(field);
    myFile.print(CSV_FILE_DELIMITER);
    getLabelTxt(LBL_BITS, field);
    myFile.print(field);
    myFile.print(CSV_FILE_DELIMITER);
    myFile.print("\r\n");
    for (cnt = 0; cnt < 16; cnt++) {
      myFile.print(accPanel[cnt].type);                  // Acc data
      myFile.print(CSV_FILE_DELIMITER);
      myFile.print(accPanel[cnt].addr);
      myFile.print(CSV_FILE_DELIMITER);
      myFile.print(accPanel[cnt].addr2);
      myFile.print(CSV_FILE_DELIMITER);
      myFile.print(accPanel[cnt].accName);
      myFile.print(CSV_FILE_DELIMITER);
      myFile.print(accPanel[cnt].activeOutput);
      myFile.print(CSV_FILE_DELIMITER);
      myFile.print("\r\n");
    }
    myFile.close();
    dataOK = true;
  }
  return dataOK;
}


bool loadAccPanel(fs::FS & fs) {
  char line[200];
  bool dataOK;
  uint16_t cnt = 0;
  File myFile;
  dataOK = false;
  sprintf (line, "/acc/%d.csv", currPanel);
  myFile = fs.open(line);
  if (myFile) {
    if (readCSV(myFile, line, sizeof(line), false)) {                                     // skip header line
      for (cnt = 0; cnt < 16; cnt++) {
        readCSV(myFile, line, sizeof(line), true);
        accPanel[cnt].type = (accType)atoi(line);
        readCSV(myFile, line, sizeof(line), true);
        accPanel[cnt].addr = atoi(line);
        readCSV(myFile, line, sizeof(line), true);
        accPanel[cnt].addr2 = atoi(line);
        readCSV(myFile, line, sizeof(line), true);
        snprintf(accPanel[cnt].accName, ACC_LNG + 1, line);
        readCSV(myFile, line, sizeof(line), false);
        accPanel[cnt].activeOutput = atoi(line);
        DEBUG_MSG("Line %d: %d, %d, %d, \"%s\", %d", cnt, accPanel[cnt].type, accPanel[cnt].addr, accPanel[cnt].addr2, accPanel[cnt].accName, accPanel[cnt].activeOutput)
      }
    }
    myFile.close();
    dataOK = true;
  }
  return dataOK;
}


bool saveAccPanelNames(fs::FS & fs) {
  char field[30];
  uint16_t cnt;
  bool dataOK, isDir;
  File myFile;
  dataOK = false;
  myFile = fs.open("/acc");
  if (myFile) {
    isDir = myFile.isDirectory();
    myFile.close();
    if (!isDir)
      return dataOK;
    DEBUG_MSG("/acc is a directory");
  }
  else {
    DEBUG_MSG("Directory /acc not found. Creating...")
    fs.mkdir("/acc");
  }
  sprintf (field, "/acc/panel.csv");
  myFile = fs.open(field, FILE_WRITE);
  if (myFile) {
    DEBUG_MSG("File %s opened for writting", field);
    getLabelTxt(LBL_NAME, field);                         // Header
    myFile.print(field);
    myFile.print("\r\n");
    for (cnt = 0; cnt < 16; cnt++) {
      myFile.print(panelNamesBuf[cnt]);                   // Panel names
      myFile.print("\r\n");
    }
    myFile.close();
    dataOK = true;
  }
  return dataOK;
}


bool loadAccPanelNames(fs::FS & fs) {
  char line[200];
  bool dataOK;
  uint16_t n;
  File myFile;
  dataOK = false;
  sprintf (line, "/acc/panel.csv");
  myFile = fs.open(line);
  if (myFile) {
    if (readCSV(myFile, line, sizeof(line), false)) {                                     // skip header line
      for (n = 0; n < 16; n++) {
        if (readCSV(myFile, line, sizeof(line), true)) {                                  // read data field
          snprintf(panelNamesBuf[n], PANEL_LNG + 1, line);
          DEBUG_MSG("%s", panelNamesBuf[n])
        }
      }
    }
    myFile.close();
  }
  else {
    for (n = 0; n < 16; n++)                                                            // default names
      snprintf(panelNamesBuf[n], PANEL_LNG + 1, "Panel %d", n);
  }
  return dataOK;
}


void updateFirmware() {
  File firmware =  SD.open("/PacoMouseCYD.ino.esp32.bin");
  if (firmware) {
    drawWindow(WIN_ABOUT);
    barData[BAR_UPDATE].value = 0;
    drawObject(OBJ_BAR, BAR_UPDATE);
    Update.onProgress(progressCallBack);
    Update.begin(firmware.size(), U_FLASH);
    Update.writeStream(firmware);
    if (Update.end()) {
      DEBUG_MSG("Update done!")
      barData[BAR_UPDATE].colorOn = COLOR_GREEN;
    }
    else {
      DEBUG_MSG("Update Error")
      barData[BAR_UPDATE].colorOn = COLOR_RED;
    }
    drawObject(OBJ_BAR, BAR_UPDATE);
    tft.drawBitmap(barData[BAR_UPDATE].x + 84, barData[BAR_UPDATE].y + 4, screen, 32, 32, COLOR_YELLOW); // monochrome
    firmware.close();
    delay(2000);
    ESP.restart();
  }
  else {
    alertWindow(ERR_FILE);
  }
}


void progressCallBack(size_t currSize, size_t totalSize) {                              // Update progress status
  uint16_t done;
  done = (uint16_t)((currSize * 100) / totalSize);
  barData[BAR_UPDATE].value = done;
  drawObject(OBJ_BAR, BAR_UPDATE);
  DEBUG_MSG("CALLBACK:  Update process at %d of %d bytes...n", currSize, totalSize);
  DEBUG_MSG("%d updated", done)
}



/*
  void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  }
*/

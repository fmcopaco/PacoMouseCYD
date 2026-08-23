/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
      Basic FTP server based on https://github.com/techworldthink/ESP32_FTPSERVER_SD

   Connect to the same wifi router as PacoMouseCYD is connected.
   Only one simultaneous data connection is allowed.

   You can save, load, delete and rename files and directories on the SD.
   
   Configuration:
   
   FTP client:  (FileZilla, WinSCP, etc.)
    User:       paco
    Password:   mouse
    IP:         use the IP address shown by PacoMOuseCYD
    Port:       21              (Data port:  50009)
    
    
   File explorer:  ftp://paco:mouse@192.168.1.43/   (use the IP address shown by PacoMouseCYD)

   
   Supported commads

   CDUP - Change to Parent Directory
   CWD  - Change Working Directory
   PWD  - Print Directory
   QUIT - disconnect client
   MODE - Transfer Mode
   PASV - Passive Connection management
   STRU - File Structure
   TYPE - Data Type
   ABOR - Abort transfer
   DELE - Delete a File
   LIST - List
   NOOP
   RETR - Retrieve
   STOR - Store
   SIZE - Size of the file
   
*/


WiFiServer ftpServer(FTP_CTRL_PORT);
WiFiServer dataServer(FTP_DATA_PORT_PASV);

FtpServer::FtpServer() {
}

void FtpServer::begin() {
  ftpServer.begin();
  delay(10);
  dataServer.begin();
  delay(10);
  millisTimeOut = (uint32_t)FTP_TIME_OUT * 60 * 1000;
  millisDelay = 0;
  cmdStatus = FTP_NO_CLIENT;
  ftpIniVariables();
}


void FtpServer::ftpIniVariables() {
  dataPort = FTP_DATA_PORT_PASV;                          // Default for data port
  dataPassiveConn = true;                                 // Default Data connection is Active
  strcpy(cwdName, "/");                                   // Set the root directory
  transferStatus = FTP_DATA_END;
  rnfrCmd = false;                                        // previous command was RNFR
}


int FtpServer::handleFTP() {
  if ((int32_t)( millisDelay - millis() ) > 0 )
    return 0;
  if (ftpServer.hasClient()) {
    ftpClient.stop();
    ftpClient = ftpServer.available();
  }
  if (cmdStatus == FTP_NO_CLIENT) {
    if (ftpClient.connected())
      disconnectClient();
    cmdStatus = FTP_WAIT_CONN;
  } else if (cmdStatus == FTP_WAIT_CONN ) {
    abortTransfer();                                      // Ftp server waiting for connection
    ftpIniVariables();
    DEBUG_MSG("FTP waiting on port %d", FTP_CTRL_PORT);
    cmdStatus = FTP_IDLE;
  } else if (cmdStatus == FTP_IDLE )  {
    if (ftpClient.connected()) {                          // Ftp server idle
      clientConnected();
      millisEndConnection = millis() + (10 * 1000);       // wait client id during 10 s.
      cmdStatus = FTP_USER_IDENT;
    }
  } else if (readChar() > 0 ) {                           // got response
    if (cmdStatus == FTP_USER_IDENT)                      // Ftp server waiting for user identity
      if (userIdentity())
        cmdStatus = FTP_USER_REG;
      else
        cmdStatus = FTP_NO_CLIENT;
    else if (cmdStatus == FTP_USER_REG)                   // Ftp server waiting for user registration
      if (userPassword()) {
        cmdStatus = FTP_USER_CMD;
        millisEndConnection = millis() + millisTimeOut;
      }
      else
        cmdStatus = FTP_NO_CLIENT;
    else if (cmdStatus == FTP_USER_CMD) {                 // Ftp server waiting for user command
      if ( ! processCommand())
        cmdStatus = FTP_NO_CLIENT;
      else
        millisEndConnection = millis() + millisTimeOut;
    }
  } else if (!ftpClient.connected() || !ftpClient) {
    cmdStatus = FTP_WAIT_CONN;
    showUserFTP(false);
    DEBUG_MSG("Client disconnected");
  }

  if (transferStatus == FTP_DATA_RETRIEVE) {
    if ( ! doRetrieve())                                  // Retrieve data
      transferStatus = FTP_DATA_END;
  } else if (transferStatus == FTP_DATA_STORE) {
    if ( ! doStore())                                     // Store data
      transferStatus = FTP_DATA_END;
  } else if ((cmdStatus > FTP_IDLE) && ! ((int32_t) (millisEndConnection - millis()) > 0 )) {
    sendResponseCode(530);                                // 530 Timeout
    millisDelay = millis() + 200;                         // delay of 200 ms
    cmdStatus = FTP_NO_CLIENT;
  }
  return    ((transferStatus != FTP_DATA_END) || (cmdStatus != FTP_NO_CLIENT));
}

void FtpServer::sendResponseCode(uint16_t code) {
  char resp[8];
  sprintf(resp, "%d ", code);
  ftpClient.println(resp);
}

void FtpServer::clientConnected() {
  DEBUG_MSG("Client connected!");
  ftpClient.println( "220 PacoMouseCYD FTP server");
  iCL = 0;
  showUserFTP(true);
}


void FtpServer::disconnectClient() {
  DEBUG_MSG(" Disconnecting client");
  abortTransfer();
  sendResponseCode(221);                                  // 221 Goodbye
  ftpClient.stop();
}

bool FtpServer::userIdentity() {
  if (strcmp(command, "USER"))
    sendResponseCode(500);                                // 500 Syntax error
  if (strcmp(parameters, "paco"))
    sendResponseCode(530);                                // 530 user not found
  else {
    sendResponseCode(331);                                // 331 OK. Password required
    strcpy( cwdName, "/" );
    return true;
  }
  millisDelay = millis() + 100;                           // delay of 100 ms
  return false;
}


bool FtpServer::userPassword() {
  if (strcmp( command, "PASS"))
    sendResponseCode(500);                                // 500 Syntax error
  else if (strcmp( parameters, "mouse"))
    sendResponseCode(530);
  else {
    DEBUG_MSG( "OK. Waiting for commands.");
    sendResponseCode(230);                                // 230 OK.
    return true;
  }
  millisDelay = millis() + 100;                           // delay of 100 ms
  return false;
}


/*  Read a char from client connected to ftp server
    Update cmdLine and command buffers, iCL and parameters pointers

    return:
      -2 if buffer cmdLine is full
      -1 if line not completed
       0 if empty line received
      length of cmdLine (positive) if no empty line received
*/

int8_t FtpServer::readChar() {
  int8_t rc = -1;
  if ( ftpClient.available()) {
    char c = ftpClient.read();
#ifdef DEBUG
    Serial.print(c);
#endif
    if (c == '\\') {
      c = '/';
    }
    if (c != '\r') {
      if (c != '\n') {
        if (iCL < FTP_CMD_SIZE )
          cmdLine[iCL++] = c;
        else
          rc = -2; //  Line too long
      } else {
        cmdLine[iCL] = 0;
        command[0] = 0;
        parameters = NULL;
        if (iCL == 0)                                     // empty line?
          rc = 0;
        else {
          rc = iCL;
          parameters = strchr(cmdLine, ' ');              // search for space between command and parameters
          if ( parameters != NULL ) {
            if (parameters - cmdLine > 4) {
              rc = -2;                                    // Syntax error
            } else {
              strncpy(command, cmdLine, parameters - cmdLine);
              command[parameters - cmdLine] = 0;
              while ( *(++parameters) == ' ');
            }
          }
          else if (strlen(cmdLine) > 4)
            rc = -2;                                      // Syntax error.
          else
            strcpy(command, cmdLine);
          iCL = 0;
        }
      }
    }
    if (rc > 0) {
      for (uint8_t i = 0; i < strlen(command); i++) {
        command[i] = toupper(command[i]);
      }
    }
    if (rc == -2) {
      iCL = 0;
      sendResponseCode(500);                              // 500 Syntax error
    }
  }
  return rc;
}


bool FtpServer::dataConnect() {
  unsigned long startTime = millis();
  if (!ftpData.connected()) {
    while (!dataServer.hasClient() && millis() - startTime < 10000) {
      yield();
    }
    if (dataServer.hasClient()) {
      ftpData.stop();
      ftpData = dataServer.available();
      rnfrCmd = false;
      DEBUG_MSG("FTP data server client...")
    }
  }
  ftpData.setNoDelay(true);
  return ftpData.connected();
}

void FtpServer::closeTransfer() {
  sendResponseCode(226);                                  // 226 File successfully transferred
  ftpFile.close();
  ftpData.stop();
  showTransferFTP("", COLOR_GHOST_WHITE, arrowL);
  DEBUG_MSG("Transfer closed")
}

void FtpServer::abortTransfer() {
  if (transferStatus > FTP_DATA_END) {
    ftpFile.close();
    ftpData.stop();
    sendResponseCode(426);                                // 426 Transfer aborted
    showTransferFTP("", COLOR_GHOST_WHITE, arrowL);
    DEBUG_MSG( "Transfer aborted!")
  }
  transferStatus = FTP_DATA_END;
}

bool FtpServer::doRetrieve() {
  int16_t nb = ftpFile.readBytes(ftpBuf, FTP_BUF_SIZE);
  if (nb > 0) {
    ftpData.write((uint8_t*) ftpBuf, nb);
    bytesTransfered += nb;
    DEBUG_MSG("%d bytes read", bytesTransfered)
    return true;
  }
  closeTransfer();
  return false;
}

bool FtpServer::doStore() {
  if (ftpData.connected()) {
    int16_t nb = ftpData.readBytes((uint8_t*) ftpBuf, FTP_BUF_SIZE);
    if (nb > 0) {
      size_t written = ftpFile.write((uint8_t*) ftpBuf, nb );
      bytesTransfered += nb;
      DEBUG_MSG("%d bytes saved", bytesTransfered)
    }
    return true;
  }
  closeTransfer();
  DEBUG_MSG("Total: %d bytes saved", bytesTransfered)
  return false;
}

bool FtpServer::makePath(char *fullName) {
  return makePath(fullName, parameters);
}

bool FtpServer::makePath( char *fullName, char *param ) {
  if (param == NULL)
    param = parameters;
  if ((strcmp(param, "/") == 0) || (strlen(param) == 0)) {  // Root or empty?
    strcpy(fullName, "/");
    return true;
  }
  if (param[0] != '/') {                                  // If relative path, concatenate with current dir
    strcpy( fullName, cwdName);
    if (fullName[strlen(fullName) - 1 ] != '/')
      strncat(fullName, "/", FTP_CWD_SIZE);
    strncat(fullName, param, FTP_CWD_SIZE);
  }
  else
    strcpy(fullName, param);
  uint16_t strl = strlen(fullName) - 1;                   // If ends with '/', remove it
  if ( fullName[strl] == '/' && (strl > 1))
    fullName[strl] = 0;
  if (strlen(fullName) < FTP_CWD_SIZE)
    return true;
  sendResponseCode(500);                                  // 500 Command line too long
  return false;
}


bool FtpServer::processCommand() {
  char answer[FTP_CMD_SIZE];
  char path[FTP_CWD_SIZE];
  File Fdir;
  File Ffile;
  //String dir;

  //------------------------ ACCESS CONTROL COMMANDS ------------------------//

  if ( ! strcmp(command, "CDUP")) {                       // Change to Parent Directory.
    DEBUG_MSG("Current dir: %s", cwdName)
    char *lastSlash = strrchr(cwdName, '/');
    if (lastSlash && (lastSlash != cwdName))
      *lastSlash = '\0';
    else {
      if (lastSlash == cwdName)
        *(lastSlash + 1) = '\0';
    }
    //sendResponseCode(250);                              // 250 "PATH" cwdName
    sprintf(answer, "250 \"%s\"", cwdName);
    ftpClient.println(answer);
    DEBUG_MSG("Parent dir: %s", cwdName)
  }

  else if ( ! strcmp(command, "CWD")) {                   //  CWD - Change Working Directory
    if (strcmp( parameters, ".") == 0) {                  // 'CWD .' is the same as PWD command
      sprintf(answer, "257 \"%s\"", cwdName);
      ftpClient.println(answer);
    }
    else {
      if (parameters[0] == '/') {
        int length_ = strlen(parameters);
        if (length_ == 1) {                               // root directory
          sprintf(answer, "/");
        } else {
          if (parameters[length_ - 1] == '/') {
            parameters[length_ - 1] = '\0';               // remove "/" from the right side of uri
            strcpy(answer, parameters);
          } else {
            strcpy(answer, parameters);
          }
        }
      } else if (!strcmp(cwdName, "/")) {
        sprintf(answer, "/%s", parameters);
      } else {
        sprintf (answer, "%s/%s", cwdName, parameters);
      }
    }
    if (SD.exists(answer)) {
      strcpy(cwdName, answer);
      //sendResponseCode(250);                              // 250 "PATH" cwdName
      sprintf(answer, "250 \"%s\"", cwdName);
      ftpClient.println(answer);
    } else {
      sprintf(answer, "550 \"%s\"", parameters);            // 550 directory or file does not exist
      ftpClient.println(answer);
    }
  }

  else if ( ! strcmp(command, "PWD" )) {
    sprintf(answer, "257 \"%s\"", cwdName);               //  PWD - Print Directory
    ftpClient.println(answer);
    DEBUG_MSG("Dir: %s", cwdName)
  }

  else if ( ! strcmp( command, "QUIT")) {
    disconnectClient();                                   //  QUIT
    return false;
  }

  //------------------------ TRANSFER PARAMETER COMMANDS ------------------------//

  else if ( ! strcmp( command, "MODE")) {                 //  MODE - Transfer Mode
    if ( ! strcmp( parameters, "S"))
      sendResponseCode(200);                              // 200 S Ok
    else
      sendResponseCode(504);                              // 504 Only S(tream) is suported
  }

  else if ( ! strcmp( command, "PASV")) {                 //  PASV - Passive Connection management
    if (ftpData.connected())
      ftpData.stop();
    ftpDataIp = WiFi.localIP();
    dataPort = FTP_DATA_PORT_PASV;
    sprintf (answer, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).", ftpDataIp[0], ftpDataIp[1], ftpDataIp[2], ftpDataIp[3], dataPort >> 8, dataPort & 0xFF);
    ftpClient.println(answer);
    dataPassiveConn = true;
  }
  /*
    else if ( ! strcmp( command, "PORT")) {                 //  PORT - Data Port      // Removed. Security issue: FTP bounce.
      if (ftpData)
        ftpData.stop();
      ftpDataIp[0] = atoi( parameters);                     // get IP of data client
      char *p = strchr( parameters, ',');
      for ( uint8_t i = 1; i < 4; i++) {
        ftpDataIp[i] = atoi(++p);
        p = strchr( p, ',');
      }
      dataPort = 256 * atoi(++p);                           // get port of data client
      p = strchr( p, ',' );
      dataPort += atoi(++p);
      if (p == NULL)
        sendResponseCode(501);                              // 501 Can't interpret parameters
      else {
        sendResponseCode(200);                              // 200 PORT command successful
        dataPassiveConn = false;
      }
    }
  */
  else if ( ! strcmp( command, "STRU")) {                 //  STRU - File Structure
    if ( ! strcmp( parameters, "F"))
      sendResponseCode(200);                              // 200 F Ok
    else
      sendResponseCode(504);                              // 504 Only F(ile) is suported
  }

  else if ( ! strcmp( command, "TYPE")) {                 //  TYPE - Data Type
    if ( ! strcmp( parameters, "A"))
      sendResponseCode(200);                              // 200 TYPE is now ASCII
    else if ( ! strcmp( parameters, "I"))
      sendResponseCode(200);                              // 200 TYPE is now 8-bit binary
    else
      sendResponseCode(504);                              // 504 Unknow TYPE
  }

  //------------------------ FTP SERVICE COMMANDS ------------------------//

  else if ( ! strcmp(command, "ABOR")) {                  //  ABOR - Abort
    abortTransfer();
    sendResponseCode(226);                                // 226 Data connection closed
  }


  else if ( ! strcmp(command, "DELE")) {                  //  DELE - Delete a File
    if ( strlen(parameters) == 0 )
      sendResponseCode(501);                              // 501 No file name
    else if (makePath( path)) {
      showTransferFTP(path, COLOR_BLACK, trash);
      if ( ! SD.exists(path)) {
        sendResponseCode(550);                            // 550 File "FILE" not found. parameters
      }
      else {
        if (SD.remove(path)) {
          sendResponseCode(250);                          // 250 Deleted "FILE". parameters
        }
        else {
          sendResponseCode(450);                          // 450 Can't delete "FILE" not found. parameters
        }
      }
      showTransferFTP("", COLOR_GHOST_WHITE, trash);
    }
  }

  else if ( ! strcmp(command, "LIST")) {                  //  LIST - List
    if ( ! dataConnect())
      sendResponseCode(425);                              // 425 No data connection
    else {
      sendResponseCode(150);                              // 150 Accepted data connection
      uint16_t nm = 0;
      Fdir = SD.open(cwdName);
      if ((!Fdir) || (!Fdir.isDirectory())) {
        sendResponseCode(550);                            // 550 Can't open directory "FILE". cwdName
      }
      else {
        Ffile = Fdir.openNextFile();
        while (Ffile) {
          if (Ffile.isDirectory()) {
            sprintf(answer, "01-01-2000  00:00AM <DIR> %s", Ffile.name());
            ftpData.println(answer);
          }
          else {
            sprintf(answer, "01-01-2000  00:00AM %d %s", Ffile.size(), Ffile.name());
            ftpData.println(answer);
          }
          nm++;
          Ffile = Fdir.openNextFile();
        }
        sendResponseCode(226);                            // 226 %d matches total. nm
      }
      ftpData.stop();
    }
  }

  else if ( ! strcmp(command, "NOOP")) {                  //  NOOP
    sendResponseCode(200);                                // 200 Zzz...
  }

  else if ( ! strcmp(command, "RETR")) {                  //  RETR - Retrieve
    if (strlen(parameters) == 0)
      sendResponseCode(501);                              // 501 No file name
    else if (makePath(path)) {
      ftpFile = SD.open(path, "rb");
      if (!ftpFile) {
        sendResponseCode(550);                            // 550 File FILE not found. parameters
      }
      else if (!ftpFile ) {
        sendResponseCode(450);                            // 450 Can't open FILE. parameters
      }
      else if ( ! dataConnect())
        sendResponseCode(425);                            // 425 No data connection
      else {
        showTransferFTP(path, COLOR_ORANGE, arrowL);
        sprintf(answer, "150 %d bytes to download", ftpFile.size());
        ftpClient.println(answer);
        bytesTransfered = 0;
        transferStatus = FTP_DATA_RETRIEVE;
      }
    }
  }

  else if ( ! strcmp(command, "STOR")) {                  //  STOR - Store
    if ( strlen(parameters) == 0 )
      sendResponseCode(501);                              // 501 No file name
    else if (makePath(path)) {
      ftpFile = SD.open(path, "w");
      if ( !ftpFile) {
        sendResponseCode(451);                            // 451 Can't open/create FILE. parameters
      }
      else if ( ! dataConnect()) {
        sendResponseCode(425);                            // 425 No data connection
        ftpFile.close();
      } else {
        showTransferFTP(path, COLOR_ORANGE, arrowR);
        sprintf(answer, "150 Connected to port %d", dataPort);
        ftpClient.println(answer);
        bytesTransfered = 0;
        transferStatus = FTP_DATA_STORE;
        DEBUG_MSG("Storing file %s", path)
      }
    }
  }

  else if ( ! strcmp(command, "MKD")) {                   //  MKD - Make Directory
    if (!strcmp(cwdName, "/")) {
      sprintf(answer, "/%s", parameters);
    } else {
      sprintf(answer, "%s/%s", cwdName, parameters);
    }
    if (SD.mkdir(answer)) {
      sprintf(answer, "257 \"%s\"", parameters);          // 257 "DIR" Directory successfully created
      ftpClient.println(answer);
    }
    else {
      sendResponseCode(502);                              // 502 Can't create
    }
  }

  else if ( ! strcmp( command, "RMD" )) {                 //  RMD - Remove a Directory
    if (!strcmp(cwdName, "/")) {
      sprintf(answer, "/%s", parameters);
    } else {
      sprintf(answer, "%s/%s", cwdName, parameters);
    }
    showTransferFTP(answer, COLOR_BLACK, trash);
    if (SD.rmdir(answer)) {
      sendResponseCode(250);                              // 250 RMD command successful
    } else {
      sendResponseCode(502);                              // 502 Can't delete
    }
    showTransferFTP("", COLOR_GHOST_WHITE, trash);
  }


  else if ( ! strcmp(command, "RNFR")) {                  //  RNFR - Rename From
    ftpBuf[0] = '\0';
    if ( strlen(parameters) == 0 )
      sendResponseCode(501);                              // 501 No file name
    else if (makePath(ftpBuf)) {
      if ( ! SD.exists(ftpBuf))
        sendResponseCode(550);                            // 550 File FILE not found. parameters
      else {
        sendResponseCode(350);                            // 350 RNFR accepted - file exists, ready for destination
        rnfrCmd = true;
      }
    }
  }


  else if ( ! strcmp( command, "RNTO" )) {                //  RNTO - Rename To
    if (strlen(ftpBuf) == 0 || ! rnfrCmd)
      sendResponseCode(503);                              // 503 Need RNFR before RNTO
    else if (strlen(parameters) == 0)
      sendResponseCode(501);                              // 501 No file name
    else if (makePath(path)) {
      if (SD.exists(path))
        sendResponseCode(553);                            // 553 "FILE" already exists
      else {
        DEBUG_MSG("Renaming %s to %s", ftpBuf, path);
        if (SD.rename(ftpBuf, path))
          sendResponseCode(250);                          // 250 File successfully renamed or moved
        else
          sendResponseCode(451);                          // 451 Rename/move failure
      }
    }
    rnfrCmd = false;
  }


  //------------------------ EXTENSIONS COMMANDS (RFC 3659) ------------------------//

  else if ( ! strcmp(command, "SIZE")) {                  //  SIZE - Size of the file
    if (strlen( parameters ) == 0)
      sendResponseCode(501);                              // 501 No file name
    else if (makePath(path)) {
      ftpFile = SD.open(path, "r");
      if (!ftpFile) {
        sprintf(answer, "450 Can't open %s", parameters);
        ftpClient.println(answer);
      }
      else {
        sprintf(answer, "213 %d", ftpFile.size());
        ftpClient.println(answer);
        ftpFile.close();
      }
    }
  }

  //------------------------ UNRECOGNIZED COMMANDS ------------------------//

  else                                                    // Unrecognized commands
    sendResponseCode(500);                                // 500 Unknow command
  return true;
}


void showUserFTP(bool user) {
  fncData[FNC_FTP_USER].idIcon = user ? FNC_CONDUCTOR_OFF : FNC_BLANK_OFF;
  newEvent(OBJ_FNC, FNC_FTP_USER, EVNT_DRAW);
}

void showTransferFTP(char *path, uint16_t color, const uint8_t *bitmap) {
  snprintf(ftpCurrFileBuf, PWD_LNG + 1, "%s", path);
  iconData[ICON_FTP_DIR].color = color;
  if (color != COLOR_GHOST_WHITE)
    iconData[ICON_FTP_DIR].bitmap = bitmap;
  drawObject(OBJ_TXT, TXT_FTP_FILE);
  drawObject(OBJ_ICON, ICON_FTP_DIR);
}

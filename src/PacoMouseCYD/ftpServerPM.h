/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
      Basic FTP server based on https://github.com/techworldthink/ESP32_FTPSERVER_SD
*/

#ifndef FTP_SERVER_PM_H
#define FTP_SERVER_PM_H

#define FTP_CTRL_PORT       21          // Command port on wich server is listening  
#define FTP_DATA_PORT_PASV  50009       // Data port in passive mode

#define FTP_TIME_OUT        5           // Disconnect client after 5 minutes of inactivity
#define FTP_CMD_SIZE        255 + 8     // max size of a command
#define FTP_CWD_SIZE        255 + 8     // max size of a directory name
#define FTP_FIL_SIZE        255         // max size of a file name
#define FTP_BUF_SIZE        (8192*1)-1  // size of file buffer for read/write

class FtpServer {
  public:

    FtpServer();
    void    begin();
    int     handleFTP();

  private:
    void    ftpIniVariables();
    void    clientConnected();
    void    disconnectClient();
    bool    userIdentity();
    bool    userPassword();
    bool    processCommand();
    bool    dataConnect();
    bool    doRetrieve();
    bool    doStore();
    void    closeTransfer();
    void    abortTransfer();
    void    sendResponseCode(uint16_t code);
    bool    makePath(char *fullname);
    bool    makePath(char *fullName, char *param);
    int8_t  readChar();

    IPAddress  ftpDataIp;               // IP address of client for data
    WiFiClient ftpClient;
    WiFiClient ftpData;

    File ftpFile;

    boolean  dataPassiveConn;
    uint16_t dataPort;
    char     ftpBuf[FTP_BUF_SIZE];      // data buffer for transfers
    char     cmdLine[FTP_CMD_SIZE];     // where to store incoming char from client
    char     cwdName[FTP_CWD_SIZE];     // name of current directory
    char     command[5];                // command sent by client
    bool     rnfrCmd;                   // previous command was RNFR
    char     *parameters;               // point to begin of parameters sent by client
    uint16_t iCL;                       // pointer to cmdLine next incoming char
    int8_t   cmdStatus,                 // status of ftp command connexion
             transferStatus;            // status of ftp data transfer
    uint32_t millisTimeOut,             // disconnect after 5 min of inactivity
             millisDelay,
             millisEndConnection,       //
             bytesTransfered;           // Current bytes transfered of file

    enum    cmdstat       {FTP_NO_CLIENT, FTP_WAIT_CONN, FTP_IDLE, FTP_USER_IDENT, FTP_USER_REG, FTP_USER_CMD};
    enum    transferstat  {FTP_DATA_END, FTP_DATA_RETRIEVE, FTP_DATA_STORE};
};


#endif

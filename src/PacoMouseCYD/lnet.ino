/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/

      This software and associated files are a DIY project that is not intended for commercial use.
      This software uses libraries with different licenses, follow all their different terms included.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.

      Sources are only provided for building and uploading to the device.
      You are not allowed to modify the source code or fork/publish this project.
      Commercial use is forbidden.
*/

////////////////////////////////////////////////////////////
// ***** LOCONET OVER TCP SOPORTE *****
////////////////////////////////////////////////////////////

void lnetSend (lnMsg * Msg) {
  byte n, pos, chk, nibble;
  byte msgLng;
  char msgStr[120];

  chk = 0xFF;
  msgLng = getLnMsgSize(Msg);
  //msgLng = ((Msg->sz.command & 0x60) == 0x60) ? Msg->sz.mesg_size : ((Msg->sz.command & 0x60) >> 4) + 2;
  if (wifiSetting.serverType) {
    strcpy(msgStr, "SEND ");                                        // Loconet over TCP/IP LBServer
    pos = 5;
    for (n = 0; n < msgLng - 1; n++) {
      chk ^= Msg->data[n];
      nibble = Msg->data[n] >> 4;
      msgStr[pos++] = (nibble > 9) ? nibble + 0x37 : nibble + 0x30;
      nibble = Msg->data[n] & 0x0F;
      msgStr[pos++] = (nibble > 9) ? nibble + 0x37 : nibble + 0x30;
      msgStr[pos++] = ' ';
    }
    nibble = chk >> 4;
    msgStr[pos++] = (nibble > 9) ? nibble + 0x37 : nibble + 0x30;
    nibble = chk & 0x0F;
    msgStr[pos++] = (nibble > 9) ? nibble + 0x37 : nibble + 0x30;
    //msgStr[pos++] = '\r';
    msgStr[pos++] = '\n';
    msgStr[pos++] = '\0';
    Client.write(msgStr);
    DEBUG_MSG(msgStr);
    sentOK = false;
    timeoutSend = millis();
    while ((millis() - timeoutSend < 200) && (!sentOK))             // wait confirmation
      lnetReceive();
  }
  else {
    for (n = 0; n < msgLng - 1; n++)                                // Loconet over TCP/IP Binary
      chk ^= Msg->data[n];
    Msg->data[n] = chk;
    Client.write((byte *)&Msg->data[0], msgLng);
  }
}


void lnetReceive() {
  char rxByte;
  byte lng;
  while (Client.available()) {
    rxByte = Client.read();
    if (wifiSetting.serverType) {                                   // Loconet over TCP/IP LBServer
#ifdef DEBUG
      Serial.print(rxByte);
#endif
      switch (rcvStrPhase) {
        case WAIT_TOKEN:                                            // wait for RECEIVE token
          switch (rxByte) {
            case 'R':                                               // wait for RECEIVE token
              rcvStrPos = 0;                                        // Possible match: RECEIVE. veRsion, bREak, eRRoR Checksum, eRRoR line, eRRoR message / No match: send, sent,timestamp
              rcvStr[rcvStrPos++] = rxByte;
              rcvStrPhase = RECV_TOKEN;
              break;
            case 'S':                                               // wait for SENT token
              rcvStrPos = 0;                                        // Possible match: Send, Sent, timeStamp, verSion, error checkSum, error meSSage / No match: receive, break, error line
              rcvStr[rcvStrPos++] = rxByte;
              rcvStrPhase = SENT_TOKEN;
              break;
          }
          break;
        case SENT_TOKEN:
          switch (rxByte) {
            case 'E':                                               // SENT valid characters
            case 'N':
            case 'T':
            case ' ':
              rcvStr[rcvStrPos++] = rxByte;
              if (rcvStrPos == 5) {
                if (! strncmp(rcvStr, "SENT ", 5)) {
                  rcvStrPhase = SENT_PARAM;
                  rcvStrPos = 0;
                  RecvPacket.data[rcvStrPos] = 0;
                }
                else
                  rcvStrPhase = WAIT_TOKEN;
              }
              break;
            default:                                                // SENT invalid characters
              rcvStrPhase = WAIT_TOKEN;
              break;
          }
          break;
        case SENT_PARAM:
          if ((rxByte == '\n') || (rxByte == '\r')) {
            //DEBUG_MSG("SENT token detected!")
            sentOK = true;
            rcvStrPhase = WAIT_TOKEN;
          }
          break;
        case RECV_TOKEN:
          switch (rxByte) {
            case 'E':                                               // RECEIVE valid characters
            case 'C':
            case 'I':
            case 'V':
            case ' ':
              rcvStr[rcvStrPos++] = rxByte;
              if (rcvStrPos == 8) {
                if (! strncmp(rcvStr, "RECEIVE ", 8)) {
                  rcvStrPhase = RECV_PARAM;
                  rcvStrPos = 0;
                  RecvPacket.data[rcvStrPos] = 0;
                  //DEBUG_MSG(" - RECEIVE token detected!")
                }
                else
                  rcvStrPhase = WAIT_TOKEN;
              }
              break;
            default:                                                // RECEIVE invalid characters
              rcvStrPhase = WAIT_TOKEN;
              break;
          }
          break;
        case RECV_PARAM:
          switch (rxByte) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              RecvPacket.data[rcvStrPos] = (RecvPacket.data[rcvStrPos] << 4) + (rxByte & 0x0F);
              break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'a':                                               // workaround for LocoNetEtherBuffer example of Loconet.h library.
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
              RecvPacket.data[rcvStrPos] = (RecvPacket.data[rcvStrPos] << 4) + (rxByte & 0x0F) + 9;
              break;
            case '\n':
              rcvStrPhase = WAIT_TOKEN;
              //DEBUG_MSG("Message received! CHK: %02X", chk)
              lnetDecode(&RecvPacket);
              /*
                #ifdef DEBUG
                rcvStrPos = ((RecvPacket.sz.command & 0x60) == 0x60) ? RecvPacket.sz.mesg_size : ((RecvPacket.sz.command & 0x60) >> 4) + 2;             // imprime paquete
                DEBUG_MSG("LN Lng: % d", rcvStrPos)
                Serial.print(F("RX: "));
                for (uint8_t x = 0; x < rcvStrPos; x++) {
                uint8_t val = RecvPacket.data[x];
                if (val < 16)
                  Serial.print('0');
                Serial.print(val, HEX);
                Serial.print(' ');
                }
                Serial.println();
                #endif
              */
              break;
            case ' ':
              rcvStrPos++;
              if (rcvStrPos > 31)
                rcvStrPhase = WAIT_TOKEN;                           // message too long, discard
              else
                RecvPacket.data[rcvStrPos] = 0;
              break;
          }
          break;
      }
    }
    else {                                                          // Loconet over TCP/IP Binary
      if (rxByte & 0x80) {
        rcvStrPos = 1;
        RecvPacket.sz.command = rxByte;
      }
      else {
        RecvPacket.data[rcvStrPos++] = rxByte;
        lng = ((RecvPacket.sz.command & 0x60) == 0x60) ? RecvPacket.sz.mesg_size : ((RecvPacket.sz.command & 0x60) >> 4) + 2;
        if (lng > sizeof(RecvPacket))                               // discard message too long
          rcvStrPos = 2;
        if (rcvStrPos == lng) {
          //DEBUG_MSG("Message received!")
          lnetDecode(&RecvPacket);
#ifdef DEBUG
          //DEBUG_MSG("LN Lng: % d Pkt: % d", rcvStrPos, lng)
          Serial.print(F("RX: "));
          for (uint8_t x = 0; x < lng; x++) {
            uint8_t val = RecvPacket.data[x];
            if (val < 16)
              Serial.print('0');
            Serial.print(val, HEX);
            Serial.print(' ');
          }
          Serial.println();
#endif
        }
      }
    }
  }
}


////////////////////////////////////////////////////////////
// ***** LOCONET SOPORTE *****
////////////////////////////////////////////////////////////

uint8_t getLnMsgSize(volatile lnMsg* Msg) {
  return ((Msg->sz.command & 0x60) == 0x60) ? Msg->sz.mesg_size : ((Msg->sz.command & 0x60) >> 4) + 2;
}


void send4byteMsg (byte opcode, byte slot, byte val) {              // Envia un mensaje de 4 bytes
  SendPacket.data[0] = opcode;
  SendPacket.data[1] = slot;
  SendPacket.data[2] = val;
  lnetSend(&SendPacket);
}


void nullMoveSlot (byte slot) {
  send4byteMsg(OPC_MOVE_SLOTS, slot, slot);                         // NULL MOVE, expect <E7>SLOT READ or LACK
  pingTimer = millis();
}


void resumeOperationsLnet() {
  SendPacket.data[0] = OPC_GPON;                                    //  Track Power On
  lnetSend(&SendPacket);
}


void emergencyOffLnet() {
  SendPacket.data[0] = OPC_GPOFF;                                   //  Track Power Off
  lnetSend(&SendPacket);
}


void infoLocomotoraLnet (unsigned int address) {
  byte adrH, adrL;
  adrH = (address >> 7) & 0x7F;
  adrL = address & 0x7F;
  send4byteMsg(OPC_LOCO_ADR, adrH, adrL);                           // REQ loco ADR, expect <E7>SLOT READ
  if (typeCmdStation == CMD_DR)
    send4byteMsg(OPC_LOCO_ADR_UHLI, adrH, adrL);                    // REQ loco ADR, expect <E6>SLOT READ UHLI
}


byte getMaxStepLnet() {
  byte stp;
  stp = stepsLN[mySlot.state & 0x07];
  return stp;
}


byte getCurrentStepLnet() {
  byte maxStep, calcStep;
  maxStep = getMaxStepLnet();
  if (locoData[myLocoData].mySpeed > 1) {
    if (maxStep == 128) {                                           // 128 steps -> 0..126
      return (locoData[myLocoData].mySpeed - 1);
    }
    else {
      if (maxStep == 28) {                                          // 28 steps -> 0..28
        calcStep = ((locoData[myLocoData].mySpeed - 2) << 1) / 9;
        return (calcStep + 1);
      }
      else {                                                        // 14 steps -> 0..14
        calcStep = (locoData[myLocoData].mySpeed - 2) / 9;
        return (calcStep + 1);
      }
    }
  }
  return (0);
}

void liberaSlot() {
  if (mySlot.num > 0) {
    send4byteMsg(OPC_SLOT_STAT1, mySlot.num, mySlot.state & ~(STAT1_SL_BUSY));    //
    DEBUG_MSG("Liberando slot %d", mySlot.num);
  }
  mySlot.num = 0;
  locoData[myLocoData].myFunc.Bits = 0;
}

void locoOperationSpeedLnet() {                                     // Envia velocidad
  if (mySlot.num > 0) {
    send4byteMsg(OPC_LOCO_SPD, mySlot.num, locoData[myLocoData].mySpeed);
    //DEBUG_MSG("Operation Speed: %d", mySpeed);
  }
}


void changeDirectionF0F4Lnet() {                                    // Envia sentido y F0..F4
  byte fnc;
  if (mySlot.num > 0) {
    fnc =  ((locoData[myLocoData].myFunc.xFunc[0] >> 1) & 0x0F);
    if (bitRead(locoData[myLocoData].myFunc.xFunc[0], 0))
      fnc |= 0x10;
    if (!(locoData[myLocoData].myDir & 0x80))
      fnc |= 0x20;
    send4byteMsg(OPC_LOCO_DIRF, mySlot.num, fnc);
    //DEBUG_MSG("Dir: %d FncF0F4 %d", myDir >> 7, fnc & 0x1F);
  }
}


void funcOperationsLnet (uint8_t fnc) {
  uint8_t data;
  if (mySlot.num > 0) {
    switch (fnc) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
        changeDirectionF0F4Lnet();                                  // LNPE
        break;
      case 5:
      case 6:
      case 7:
      case 8:
        data =  ((locoData[myLocoData].myFunc.xFunc[0] >> 5) & 0x07);   // LNPE
        if (bitRead(locoData[myLocoData].myFunc.xFunc[1], 0))
          data |= 0x08;
        send4byteMsg(OPC_LOCO_SND, mySlot.num, data);
        //DEBUG_MSG("FncF5F8 %d", data);
        break;
      default:
        switch (typeCmdStation) {
          case CMD_DR:                                              // DR5000 / IB II
            if (fnc > 12) {
              changeFuncULHI(fnc);
            }
            else {
              data =  ((locoData[myLocoData].myFunc.xFunc[1] >> 1) & 0x0F);
              send4byteMsg(OPC_LOCO_F9F12, mySlot.num, data);
              //DEBUG_MSG("DR5000-FncF9F12 %d",  data);
            }
            break;
          case CMD_ULI:                                             // Uhlenbrock
            changeFuncULHI(fnc);
            break;
          case CMD_DIG:                                             // Digitrax
            changeFuncIMM(fnc);
            break;
        }
        break;
    }
  }
}


void changeFuncULHI(uint8_t fnc) {
  uint8_t arg4;
  SendPacket.data[0] = OPC_UHLI_FUN;                                // Uhlenbrock
  SendPacket.data[1] = 0x20;
  SendPacket.data[2] = mySlot.num;
  switch (fnc) {                                                    // ---87654 32109876 54321098 76543210
    case 12:
    case 20:
    case 28:
      SendPacket.data[3] = 0x05;
      arg4 = bitRead(locoData[myLocoData].myFunc.Bits, 12) << 4;
      arg4 |= bitRead(locoData[myLocoData].myFunc.Bits, 20) << 5;
      arg4 |= bitRead(locoData[myLocoData].myFunc.Bits, 28) << 6;
      SendPacket.data[4] = arg4 & 0x70;                             // F12,F20,F28
      break;
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
      SendPacket.data[3] = 0x08;
      arg4 =  ((locoData[myLocoData].myFunc.xFunc[1] >> 5) & 0x07);
      arg4 |= (locoData[myLocoData].myFunc.xFunc[2] << 3);
      SendPacket.data[4] = arg4 & 0x7F;                             // F13..F19
      break;
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
      SendPacket.data[3] = 0x09;
      arg4 =  ((locoData[myLocoData].myFunc.xFunc[2] >> 5) & 0x07);
      arg4 |= (locoData[myLocoData].myFunc.xFunc[3] << 3);
      SendPacket.data[4] = arg4 & 0x7F;                             // F21..F27
      break;
    default:
      SendPacket.data[3] = 0x07;
      arg4 = (uint8_t)(locoData[myLocoData].myFunc.Bits >> 5);
      SendPacket.data[4] = arg4 & 0x7F;                             // F5..F11
      break;
  }
  lnetSend(&SendPacket);
}


void changeFuncIMM(uint8_t fnc) {
  uint8_t data, adrH, adrL;
  adrH = (locoData[myLocoData].myAddr.address >> 7) & 0x7F;         // Digitrax
  adrL = locoData[myLocoData].myAddr.adr[0] & 0x7F;
  SendPacket.data[0] = OPC_IMM_PACKET;
  SendPacket.data[1] = 0x0B;
  SendPacket.data[2] = 0x7F;
  if (fnc > 12) {
    if (fnc > 20)
      data = (uint8_t)(locoData[myLocoData].myFunc.Bits >> 21) & 0xFF;
    else
      data = (uint8_t)(locoData[myLocoData].myFunc.Bits >> 13) & 0xFF;
    if (adrH) {
      SendPacket.data[3] = 0x44;                                    // REPS: D4,5,6=#IM bytes,D3=0(reserved); D2,1,0=repeat CNT
      SendPacket.data[4] = (bitRead(locoData[myLocoData].myAddr.adr[0], 7)) ? 0x07 : 0x05; // DHI
      if (bitRead(data, 7))
        SendPacket.data[4] |= 0x08;
      SendPacket.data[5] = locoData[myLocoData].myAddr.adr[1] | 0x40;                // IM1
      SendPacket.data[6] = adrL;                                    // IM2
      if (fnc > 20)
        SendPacket.data[7] = 0x5F;                                  // IM3
      else
        SendPacket.data[7] = 0x5E;                                  // IM3
      SendPacket.data[8] = data & 0x7F;                             // IM4
    }
    else {
      SendPacket.data[3] = 0x34;                                    // REPS: D4,5,6=#IM bytes,D3=0(reserved); D2,1,0=repeat CNT
      SendPacket.data[4] = (bitRead(data, 7)) ? 0x06 : 0x02;        // DHI
      SendPacket.data[5] = adrL;                                    // IM1
      if (fnc > 20)
        SendPacket.data[6] = 0x5F;                                  // IM2   [110-11111]
      else
        SendPacket.data[6] = 0x5E;                                  // IM2   [110-11110]
      SendPacket.data[7] = data & 0x7F;                             // IM3
      SendPacket.data[8] = 0x00;                                    // IM4
    }
    SendPacket.data[9] = 0x00;                                      // IM5
    lnetSend(&SendPacket);
  }
  else {
    data = (uint8_t)(locoData[myLocoData].myFunc.Bits >> 9) & 0x0F;
    if (adrH) {
      SendPacket.data[3] = 0x34;                                    // REPS: D4,5,6=#IM bytes,D3=0(reserved); D2,1,0=repeat CNT
      SendPacket.data[4] = (bitRead(locoData[myLocoData].myAddr.adr[0], 7)) ? 0x07 : 0x05;  // DHI
      SendPacket.data[5] = locoData[myLocoData].myAddr.adr[1] | 0x40;                       // IM1
      SendPacket.data[6] = adrL;                                    // IM2
      SendPacket.data[7] = 0x20 | data;                             // IM3    [101SDDDD]
    }
    else  {
      SendPacket.data[3] = 0x24;
      SendPacket.data[4] = 0x02;
      SendPacket.data[5] = adrL;
      SendPacket.data[6] = 0x20 | data;
      SendPacket.data[7] = 0x00;
    }
    SendPacket.data[8] = 0x00;                                      // IM4
    SendPacket.data[9] = 0x00;                                      // IM5
    lnetSend(&SendPacket);
    //DEBUG_MSG("Digitrax-FncF9F12 %d", fnc);
  }
}


void lnetRequestSwitch (unsigned int addr, byte output, byte dir) {
  byte adrH, adrL;
  adrH = ((--addr >> 7) & 0x0F);
  adrL = addr & 0x7F;
  if (output)
    adrH |= OPC_SW_REQ_OUT;                                         // output on
  if (dir)
    adrH |= OPC_SW_REQ_DIR;                                         // direction closed/thrown
  send4byteMsg(OPC_SW_REQ, adrL, adrH);
}

void getTypeCS() {
  send4byteMsg(OPC_RQ_SL_DATA, SLOT_0, 0);                          // Read slot 0 to get TRK and identify command station
}


void setTimeLnet(byte hh, byte mm, byte rate) {
  clockHour = hh;
  clockMin = mm;
  clockRate = rate;
  SendPacket.data[0]  = OPC_WR_SL_DATA;                             // Fast clock
  SendPacket.data[1]  = 0x0E;
  SendPacket.data[2]  = SLOT_FC;
  SendPacket.data[3]  = rate;                                       // clock rate
  SendPacket.data[4]  = 0x7F;
  SendPacket.data[5]  = 0x79;
  SendPacket.data[6]  = mm + 0x43;
  SendPacket.data[7]  = mySlot.trk;                                 // trk
  SendPacket.data[8]  = hh + 0x68;
  SendPacket.data[9]  = 0x00;                                       // day
  SendPacket.data[10] = 0x40;                                       // control
  SendPacket.data[11] = 0x00;                                       // ID
  SendPacket.data[12] = 0x00;
  lnetSend(&SendPacket);
}


void showTrkLnet(uint8_t trk) {
  if (lastTrk != trk) {
    lastTrk = trk;
    if (trk & GTRK_POWER) {                                         // Normal power on
      stopTimer (TMR_POWER);
      iconData[ICON_POWER].color = COLOR_GREEN;
      setColorRGB(LED_RGB_GREEN);
      if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
        newEvent(OBJ_ICON, ICON_POWER, EVNT_DRAW);
      if (isWindow(WIN_STA_PLAY)) {
        fncData[FNC_STA_RAYO].state = false;
        newEvent(OBJ_FNC, FNC_STA_RAYO, EVNT_DRAW);
      }
      if (isWindow(WIN_ALERT)) {
        switch (errType) {
          case ERR_SERV:
          case ERR_STOP:
          case ERR_CV:
            closeWindow(WIN_ALERT);
            break;
        }
      }
    }
    else {
      iconData[ICON_POWER].color = COLOR_RED;                       // Power off
      setTimer (TMR_POWER, 5, TMR_PERIODIC);                        // Flash power icon
      setColorRGB(LED_RGB_RED);
      if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
        newEvent(OBJ_ICON, ICON_POWER, EVNT_DRAW);
      if (isWindow(WIN_STA_PLAY)) {
        fncData[FNC_STA_RAYO].state = true;
        newEvent(OBJ_FNC, FNC_STA_RAYO, EVNT_DRAW);
      }
    }
    /*
      if (trk & GTRK_PROG_BUSY) {                                     // Programming track busy
      if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
        alertWindow(ERR_SERV);
      }
      if (!(trk & GTRK_IDLE)) {                                       // broadcasting an emergency stop?
      if ((isWindow(WIN_THROTTLE)) || (isWindow(WIN_STEAM)))
        alertWindow(ERR_STOP);
      }
    */
  }
}




////////////////////////////////////////////////////////////
// ***** LOCONET DECODE *****
////////////////////////////////////////////////////////////

void lnetDecode (lnMsg * LnPacket) {
  unsigned int adr;
  uint8_t slotStatus, i, n, msgLen;
  bool state;

#ifdef DEBUG
  msgLen = getLnMsgSize(LnPacket);                                  // imprime paquete
  Serial.print(F("Decoding: "));
  for (uint8_t x = 0; x < msgLen; x++) {
    uint8_t val = LnPacket->data[x];
    if (val < 16)
      Serial.print('0');
    Serial.print(val, HEX);
    Serial.print(' ');
  }
  Serial.println();
#endif

  msgLen = getLnMsgSize(LnPacket);                                  // calcula checksum
  i = 0;
  for (n = 0; n < msgLen; n++)
    i ^= LnPacket->data[n];
  DEBUG_MSG("CHK: %02X", i)
  if (i != 0xFF)                                                    // comprueba checksum
    return;

  switch (LnPacket->sz.command) {
    case OPC_LOCO_SPD:
      if (LnPacket->lsp.slot == mySlot.num) {                       // cambio en la velocidad
        locoData[myLocoData].mySpeed = LnPacket->lsp.spd;
        pingTimer = millis();
        if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
          updateSpeedHID();
      }
      break;
    case OPC_LOCO_DIRF:
      if (LnPacket->ldf.slot == mySlot.num) {                       // cambio en el sentido o F0..F4
        locoData[myLocoData].myDir = (((LnPacket->ldf.dirf << 2) ^ 0x80) & 0x80);
        locoData[myLocoData].myFunc.xFunc[0] &= 0xE0;
        locoData[myLocoData].myFunc.xFunc[0] |= ((LnPacket->ldf.dirf & 0x0F) << 1);
        if (LnPacket->ldf.dirf & 0x10)
          locoData[myLocoData].myFunc.xFunc[0] |= 0x01;
        updateFuncState(isWindow(WIN_THROTTLE));
        if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
          updateSpeedDir();
      }
      break;
    case OPC_LOCO_SND:
      if (LnPacket->ls.slot == mySlot.num) {                        // cambio en F5..F8
        locoData[myLocoData].myFunc.xFunc[0] &= 0x1F;
        locoData[myLocoData].myFunc.xFunc[1] &= 0xFE;
        locoData[myLocoData].myFunc.xFunc[0] |= (LnPacket->ls.snd << 5);
        if (LnPacket->ls.snd & 0x08)
          locoData[myLocoData].myFunc.xFunc[1] |= 0x01;
        updateFuncState(isWindow(WIN_THROTTLE));
      }
      break;
    case OPC_LOCO_F9F12:                                            // DR5000 - Intellibox-II
      if (LnPacket->ls.slot == mySlot.num) {                        // cambio en F9..F12
        locoData[myLocoData].myFunc.xFunc[1] &= 0xE1;
        locoData[myLocoData].myFunc.xFunc[1] |= (LnPacket->ls.snd << 1);
        updateFuncState(isWindow(WIN_THROTTLE));
      }
      break;
    case OPC_UHLI_FUN:
      if ((LnPacket->data[1] == 0x20) && (LnPacket->data[2] == mySlot.num)) {     // Used by Intellibox-I for F0-F28 and Intellibox-II for F13-F28
        switch (LnPacket->data[3]) {
          case 0x05:
            updateUhliF12F20F28(LnPacket->data[4]);
            break;
          case 0x06:
            updateUhliF0F4(LnPacket->data[4]);
            break;
          case 0x07:
            updateUhliF5F11(LnPacket->data[4]);
            break;
          case 0x08:
            updateUhliF13F19(LnPacket->data[4]);
            break;
          case 0x09:
            updateUhliF21F27(LnPacket->data[4]);
            break;
        }
        updateFuncState(isWindow(WIN_THROTTLE));
      }
      break;
    case OPC_SW_REQ:                                                // Alguien mueve un desvio (OPC_SW_REQ)
      adr = (LnPacket->srq.sw1 | ((LnPacket->srq.sw2 & 0x0F) << 7));
      adr++;
      state = (LnPacket->srq.sw2 & OPC_SW_REQ_DIR) ? true : false;
      accessoryChange(adr, state);
      break;
    case OPC_SW_REP:                                                // Turnout SENSOR state REPORT (OPC_SW_REP)
      adr = (LnPacket->srp.sn1 | ((LnPacket->srp.sn2 & 0x0F) << 7));
      adr++;
      if (LnPacket->srp.sn2 & OPC_SW_REP_INPUTS)
        state = (LnPacket->srp.sn2 & OPC_SW_REP_SW) ? true : false;       // input levels for turnout feedback
      else
        state = (LnPacket->srp.sn2 & OPC_SW_REP_CLOSED) ? true : false;   // output levels for turnout feedback
      accessoryChange(adr, state);
      break;
    case OPC_IMM_PACKET:
      if ((LnPacket->sp.mesg_size == 0x0B) && (LnPacket->sp.val7f == 0x7F)) {
        if (bitRead(LnPacket->sp.dhi, 0)) {
          if (bitRead(LnPacket->sp.im1, 6)) {                       // im1:1 1LLLLLL im2: L LLLLLLL
            adr = (bitRead(LnPacket->sp.dhi, 1)) ? LnPacket->sp.im2 | 0x80 : LnPacket->sp.im2;
            adr |= ((LnPacket->sp.im1 & 0x3F) << 8);
            if (adr == locoData[myLocoData].myAddr.address) {
              if (bitRead(LnPacket->sp.dhi, 2)) {                   // im3: 1 XXXFFFF
                i = (bitRead(LnPacket->sp.dhi, 3)) ? LnPacket->sp.im4 | 0x80 : LnPacket->sp.im4;
                decodeFuncIMM (LnPacket->sp.im3, i);
              }
            }
          }
        }
        else {
          if (LnPacket->sp.im1 == locoData[myLocoData].myAddr.address) {             // im1: 0 LLLLLLL
            if (bitRead(LnPacket->sp.dhi, 1)) {                     // im2: 1 XXXFFFF
              i = (bitRead(LnPacket->sp.dhi, 2)) ? LnPacket->sp.im3 | 0x80 : LnPacket->sp.im3;
              decodeFuncIMM (LnPacket->sp.im2, i);
            }
          }
        }
      }
      break;
    case OPC_GPON:
      if (!lnetProg) {                                              // workaround for Daisy II WLAN
        bitSet(mySlot.trk, 0);
        showTrkLnet(mySlot.trk);
      }
      break;
    case OPC_GPOFF:
      if (!lnetProg) {                                              // workaround for Daisy II WLAN
        bitClear(mySlot.trk, 0);
        showTrkLnet(mySlot.trk);
      }
      break;
    case OPC_SL_RD_DATA:                                            // informacion de un slot
      adr = (LnPacket->sd.adr2 << 7) + LnPacket->sd.adr;
      /*
        if (doDispatchGet && (LnPacket->sd.slot < 0x79)) {      // valid slot 1..120
        locoData[myLocoData].myAddr.address = adr;
        }
      */
      if ((adr == locoData[myLocoData].myAddr.address) && (LnPacket->sd.slot < 0x79) && (!doDispatchPut)) {    // valid slot 1..120
        DEBUG_MSG("Slot read for ADDR:%d", adr);
        mySlot.num = LnPacket->sd.slot;                             // es  mi locomotora, guarda slot
        mySlot.state = LnPacket->sd.stat;
        mySlot.trk = LnPacket->sd.trk;
        slotStatus = (LnPacket->sd.stat >> 4) & 0x03;
        //DEBUG_MSG("Slot % d STATUS: % d", mySlot.num, slotStatus);
        locoData[myLocoData].mySpeed = LnPacket->sd.spd;                            // actualiza velocidad
        locoData[myLocoData].myDir = (((LnPacket->sd.dirf << 2) ^ 0x80) & 0x80);    // actualiza sentido   (LnPacket->sd.dirf << 2) & 0x80;
        updateUhliF0F4(LnPacket->sd.dirf);
        locoData[myLocoData].myFunc.Bits &= 0xFFFFFE1F;             // F5..F8
        locoData[myLocoData].myFunc.xFunc[0] |= ((LnPacket->sd.snd & 0x07) << 5);
        bitWrite(locoData[myLocoData].myFunc.Bits, 8, bitRead(LnPacket->sd.snd, 3));
        if (slotStatus != STAT_IN_USE) {
          nullMoveSlot (mySlot.num);                                // si el slot no se usa, tomo el control para que refresque
        }
        if (doDispatchGet) {
          /*
            doDispatchGet = false;
            checkLocoAddress();
            pushLoco(locoData[myLocoData].myAddr.address);                               // guarda en stack
            updateEEPROM (EE_ADRH, locoData[myLocoData].myAddr.adr[1]);                  // guarda nueva direccion en EEPROM
            updateEEPROM (EE_ADRL, locoData[myLocoData].myAddr.adr[0]);
            optOLED = OPT_SPEED;
            enterMenuOption();
          */
        }
        else {
          updateFuncState(isWindow(WIN_THROTTLE));
          if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
            updateSpeedHID();                                       // set encoder
        }
      }

      if (LnPacket->sd.slot == SLOT_FC) {                           // FAST Clock
        if (LnPacket->fc.clk_cntrl & 0x40) {                        // bit 6 = 1; data is valid clock info
          setFastClock(LnPacket);
        }
      }

      if (LnPacket->sd.slot == SLOT_PRG) {                          // Programmer Task Final reply
        if (progStepCV != PRG_IDLE) {
          mySlot.trk = LnPacket->pt.trk;
          CVdata = LnPacket->pt.data7 | (bitRead(LnPacket->pt.cvh, 1) << 7);
          CVdata |= ((LnPacket->pt.pstat & 0x0F) << 8);
          endProg();
        }
      }

      if (LnPacket->sd.slot == SLOT_0) {                            // Slot 0
        mySlot.trk = LnPacket->sd.trk;                              // get command station status
        showTrkLnet(mySlot.trk);
        identifyCS(LnPacket);                                       // identify command station
      }
      break;
    case OPC_WR_SL_DATA:
      if ((mySlot.num > 0) && (LnPacket->sd.slot == mySlot.num)) {  // Cambios en mi slot
        infoLocomotoraLnet(locoData[myLocoData].myAddr.address);    // do it with read slot
      }
      if (LnPacket->sd.slot == SLOT_PRG) {                          // Programmer Task Start
        //CVdata = LnPacket->pt.cvl | bitRead((LnPacket->pt.cvh, 1) << 6);
      }
      if (LnPacket->sd.slot == SLOT_FC) {                           // FAST Clock
        //if (LnPacket->fc.clk_cntrl & 0x40)                        // bit 6 = 1; data is valid clock info. JMRI sends only EF 0E 7B ... (OPC_WR_SL_DATA) with clk_cntrl == 0
        setFastClock(LnPacket);
      }
      break;
    case OPC_RQ_SL_DATA:
      /*
        if (LnPacket->sr.slot == SLOT_FC) {               // FAST Clock SYNC
          clockTimer = millis();                          // reset local sub-minute phase counter
        }
      */
      break;

    case OPC_SL_RD_UHLI:                                            // Uhlenbrock loco data
    case OPC_WR_SL_UHLI:
      if (LnPacket->usd.mesg_size == 0x15) {
        adr = (LnPacket->usd.adr2 << 7) + LnPacket->usd.adr;
        if (adr == locoData[myLocoData].myAddr.address) {
          mySlot.state = LnPacket->usd.stat;
          mySlot.trk = LnPacket->usd.trk;
          slotStatus = (LnPacket->usd.stat >> 4) & 0x03;
          locoData[myLocoData].mySpeed = LnPacket->usd.spd;                               // actualiza velocidad
          locoData[myLocoData].myDir = (((LnPacket->usd.dirf << 2) ^ 0x80) & 0x80);       // actualiza sentido
          updateUhliF0F4(LnPacket->usd.dirf);
          updateUhliF5F11(LnPacket->usd.snd2);
          updateUhliF13F19(LnPacket->usd.snd3);
          updateUhliF21F27(LnPacket->usd.snd4);
          updateUhliF12F20F28(LnPacket->usd.fhi);
          updateFuncState(isWindow(WIN_THROTTLE));
          if (isWindow(WIN_THROTTLE) || isWindow(WIN_SPEEDO))
            updateSpeedHID();                                       // set encoder
        }
      }
      break;

    case OPC_LONG_ACK:
      //DEBUG_MSG("LACK Opcode: %x resp: %x", LnPacket->lack.opcode | 0x80, LnPacket->lack.ack1);
      if (progStepCV != PRG_IDLE) {
        if (LnPacket->lack.opcode == (OPC_WR_SL_DATA & 0x7F)) {
          switch (LnPacket->lack.ack1) {
            case 0x7F:                                              // Function NOT implemented, no reply.
            case 0x00:                                              // Programmer BUSY , task aborted, no reply.
              CVdata = 0x0600;                                      // show ERR
            case 0x40:                                              // Task accepted blind NO <E7> reply at completion
              endProg();
              break;
            case 0x01:                                              // Task accepted , <E7> reply at completion.
              break;
          }
        }
      }
      if (isWindow(WIN_PROG_LNCV)) {
        if (LnPacket->lack.opcode == (OPC_IMM_PACKET & 0x7F) && (LnPacket->lack.ack1 != 0x7F)) {  // error writing LNCV
          txtData[TXT_LNCV_VAL].backgnd = COLOR_PINK;
          newEvent(OBJ_TXT, TXT_LNCV_VAL, EVNT_DRAW);
        }
      }

      break;
    case OPC_MOVE_SLOTS:
      if ((LnPacket->sm.src == mySlot.num) && (LnPacket->sm.dest == mySlot.num)) {
        // me quieren robar el slot. He sido yo?
      }
      break;
    case OPC_SLOT_STAT1:
      if ((mySlot.num > 0) && (LnPacket->ss.slot == mySlot.num)) {
        // Cambios en mi slot
      }
      break;

    case  OPC_PEER_XFER:
      //  [E5 0F 05 49 4B 1F 01 2F 13 00 00 01 00 00 31]  (LNCV) READ_CV_REPLY from module (Article #5039):
      if ((LnPacket->ub.DSTL == 'I') && (LnPacket->ub.DSTH == 'K')) {
        if ((LnPacket->ub.SRC == 0x05) && (LnPacket->ub.ReqId == LNCV_REQID_CFGREAD)) {
          //DEBUG_MSG("PXCT: %0X", LnPacket->ub.PXCT1)
          if (bitRead(LnPacket->ub.PXCT1, 0))                       // expand bits in PXCT1
            bitSet(LnPacket->ub.D0, 7);
          if (bitRead(LnPacket->ub.PXCT1, 1))
            bitSet(LnPacket->ub.D1, 7);
          if (bitRead(LnPacket->ub.PXCT1, 2))
            bitSet(LnPacket->ub.D2, 7);
          if (bitRead(LnPacket->ub.PXCT1, 3))
            bitSet(LnPacket->ub.D3, 7);
          if (bitRead(LnPacket->ub.PXCT1, 4))
            bitSet(LnPacket->ub.D4, 7);
          if (bitRead(LnPacket->ub.PXCT1, 5))
            bitSet(LnPacket->ub.D5, 7);
          if (bitRead(LnPacket->ub.PXCT1, 6))
            bitSet(LnPacket->ub.D6, 7);
          artNum  = LnPacket->ub.D0 | (LnPacket->ub.D1 << 8);
          numLNCV = LnPacket->ub.D2 | (LnPacket->ub.D3 << 8);
          valLNCV = LnPacket->ub.D4 | (LnPacket->ub.D5 << 8);
          DEBUG_MSG("Art: %d LNCV: %d-%d", artNum, numLNCV, valLNCV);
          if (numLNCV == 0)
            modNum = valLNCV;
          setFieldsLNCV();
          if (isWindow(WIN_PROG_LNCV))
            showFieldsLNCV();
        }
      }
      break;


  }
}


void lnetTimers() {
  if (millis() - pingTimer > LNET_PING_INTERVAL) {                  // Refresca velocidad para mantener slot en IN_USE
    if (mySlot.num > 0) {
      pingTimer = millis();
      send4byteMsg(OPC_LOCO_SPD, mySlot.num, locoData[myLocoData].mySpeed);
      //DEBUG_MSG("Refresing speed %d", mySpeed);
    }
  }
  if (clockRate > 0) {                                              // Actualiza fast clock interno
    if (millis() - clockTimer > clockInterval) {
      clockTimer = millis();
      clockMin++;
      if (clockMin > 59) {
        clockMin = 0;
        clockHour++;
      }
      if (clockHour > 23)
        clockHour = 0;
    }
  }


}


void processLnet() {
  lnetReceive();
  lnetTimers();
}

////////////////////////////////////////////////////////////
// ***** LOCONET DECODE SUPPORT *****
////////////////////////////////////////////////////////////

void setFastClock(lnMsg * LnPacket) {
  if (LnPacket->fc.clk_rate != clockRate) {                         // 0 = Freeze clock, 1 = normal, 10 = 10:1 etc. Max is 0x7f
    clockRate = LnPacket->fc.clk_rate;                              // calcula nuevo intervalo interno
    if (clockRate > 0)
      clockInterval = 60000UL / (unsigned long)clockRate;           // calcula intervalo para un minuto
  }                                                                 // [EF 0E 7B 01 7F 79 43 07 68 1B 40 00 00 15] JMRI: 00:00 DAY 27
  clockMin =  LnPacket->fc.mins_60 - 0x43;                          // 256 - minutes   ???
  clockHour = LnPacket->fc.hours_24 - 0x68;                         // 256 - hours     ???
  clockTimer = millis();
  DEBUG_MSG("CLOCK %d:%d R:%d", clockHour, clockMin, clockRate);
  updateFastClock();
}


void decodeFuncIMM (uint8_t cmd, byte data) {
  /*
    if ((cmd & 0x60) == 0x00) {                                     // 100D-DDDD  F0-F4
      locoData[myLocoData].myFunc.xFunc[0] &= 0xE0;
      locoData[myLocoData].myFunc.xFunc[0] |= ((cmd & 0x0F) << 1);
      if (cmd & 0x10)
        locoData[myLocoData].myFunc.xFunc[0] |= 0x01;
      cmd = 0x80;
    }
    if ((cmd & 0x70) == 0x30) {                                     // 1011-FFFF  F5-F8
      locoData[myLocoData].myFunc.xFunc[0] &= 0x1F;
      locoData[myLocoData].myFunc.xFunc[0] |= (cmd << 5);
      if (cmd & 0x10)
        locoData[myLocoData].myFunc.xFunc[1] |= 0x01;
      else
        locoData[myLocoData].myFunc.xFunc[1] &= 0xFE;
      cmd = 0x80;
    }
  */
  if ((cmd & 0x70) == 0x20) {                                       // 1010-FFFF  F9-F12
    locoData[myLocoData].myFunc.xFunc[1] &= 0xE1;
    locoData[myLocoData].myFunc.xFunc[1] |= ((cmd & 0x0F) << 1);
    cmd = 0x80;
  }
  if (cmd == 0x5E) {                                                // 1101-1110  DDDDDDDD F13-F20
    locoData[myLocoData].myFunc.Bits &= 0xFFE01FFF;
    locoData[myLocoData].myFunc.Bits |= ((uint32_t)(data) << 13);
    cmd = 0x80;
  }
  if (cmd == 0x5F) {                                                // 1101-1111  DDDDDDDD F21-F28
    locoData[myLocoData].myFunc.Bits &= 0xE01FFFFF;
    locoData[myLocoData].myFunc.Bits |= ((uint32_t)(data) << 21);
    cmd = 0x80;
  }
  if (cmd == 0x80)
    updateFuncState(isWindow(WIN_THROTTLE));
}

void updateUhliF5F11(uint8_t fnc) {
  locoData[myLocoData].myFunc.Bits &= 0xFFFFF01F;
  locoData[myLocoData].myFunc.Bits |= ((unsigned long)(fnc & 0x7F) << 5);
}

void updateUhliF12F20F28(uint8_t fnc) {
  bitWrite(locoData[myLocoData].myFunc.Bits, 12, bitRead(fnc, 4));
  bitWrite(locoData[myLocoData].myFunc.Bits, 20, bitRead(fnc, 5));
  bitWrite(locoData[myLocoData].myFunc.Bits, 28, bitRead(fnc, 6));
}

void updateUhliF13F19(uint8_t fnc) {
  locoData[myLocoData].myFunc.Bits &= 0xFFF01FFF;
  locoData[myLocoData].myFunc.Bits |= ((unsigned long)(fnc & 0x7F) << 13);
}

void updateUhliF21F27(uint8_t fnc) {
  locoData[myLocoData].myFunc.Bits &= 0xF01FFFFF;
  locoData[myLocoData].myFunc.Bits |= ((unsigned long)(fnc & 0x7F) << 21);
}

void updateUhliF0F4(uint8_t fnc) {
  locoData[myLocoData].myFunc.Bits &= 0xFFFFFFE0;
  locoData[myLocoData].myFunc.Bits |= ((unsigned long)(fnc & 0x0F) << 1);
  bitWrite(locoData[myLocoData].myFunc.Bits, 0, bitRead(fnc, 4));
}

void clearPacketUlhi() {                                            // Borra paquete largo para Intellibox II
  for (byte i = 0; i < 31; i++)
    SendPacket.data[i] = 0x00;
}

void progUhli (byte mode) {
  ulhiProg = mode;
  if (typeCmdStation == CMD_DR) {                                   // Intellibox II program task start or end
    SendPacket.data[0] = OPC_PEER_XFER;
    SendPacket.data[1] = 0x07;
    SendPacket.data[2] = 0x01;
    SendPacket.data[3] = 'I';
    SendPacket.data[4] = 'B';
    SendPacket.data[5] = mode;
    lnetSend(&SendPacket);
  }
}


void readCVLnet (unsigned int adr, byte stepPrg) {
  byte cvh;
  if (!modeProg) {                                                  // Read only in Direct mode
    clearPacketUlhi();
    if (typeCmdStation == CMD_DR) {
      //if (!modeProg)
      if (ulhiProg == UHLI_PRG_END)
        progUhli(UHLI_PRG_START);                                   // Intellibox II format
      SendPacket.data[0] = OPC_IMM_PACKET;
      SendPacket.data[1] = 0x1F;
      SendPacket.data[2] = 0x01;
      SendPacket.data[3] = 'I';
      SendPacket.data[4] = 'B';
      SendPacket.data[5] = 0x71 | (bitRead(adr, 7) << 1);
      SendPacket.data[6] = 0x72;
      SendPacket.data[7] = adr & 0x7F;
      SendPacket.data[8] = (adr >> 8) & 0x03;
      SendPacket.data[10] = 0x70;
      SendPacket.data[15] = 0x10;
    }
    else {
      SendPacket.data[0] = OPC_WR_SL_DATA;
      SendPacket.data[1] = 0x0E;
      SendPacket.data[2] = SLOT_PRG;                                // Slot 0x7C
      SendPacket.data[3] = 0x28;                                    // PCMD Direct Read
      //    SendPacket.data[4] = 0x00;
      //    SendPacket.data[5] = 0x00;                              // HOPSA Loco address
      //    SendPacket.data[6] = 0x00;                              // LOPSA
      SendPacket.data[7] = mySlot.trk;                              // TRK
      adr--;
      cvh = bitRead(adr, 7) | (bitRead(adr, 8) << 4) | (bitRead(adr, 9) << 5);
      SendPacket.data[8] = cvh;                                     // CVH <0,0,CV9,CV8 - 0,0, D7,CV7>
      SendPacket.data[9] = adr & 0x7F;                              // CVL
      //    SendPacket.data[10] = 0x00;                             // DATA7
      //    SendPacket.data[11] = 0x00;
      //    SendPacket.data[12] = 0x00;
    }
    lnetSend(&SendPacket);
    //DEBUG_MSG("Read CV %d", adr);
    progStepCV = stepPrg;
  }
}


void writeCVLnet (unsigned int adr, unsigned int data, byte stepPrg) {
  byte cvh;
  clearPacketUlhi();
  if (typeCmdStation == CMD_DR) {
    if (!modeProg)
      if (ulhiProg == UHLI_PRG_END)
        progUhli(UHLI_PRG_START);                                   // Intellibox II format
    SendPacket.data[0] = OPC_IMM_PACKET;
    SendPacket.data[1] = 0x1F;
    SendPacket.data[2] = 0x01;
    SendPacket.data[3] = 'I';
    SendPacket.data[4] = 'B';
    if (modeProg) {
      SendPacket.data[5] = 0x71 | (bitRead(locoData[myLocoData].myAddr.address, 7) << 1) | (bitRead(adr, 7) << 3);
      SendPacket.data[6] = 0x5E;
      SendPacket.data[7] = locoData[myLocoData].myAddr.address & 0x7F;
      SendPacket.data[8] = (locoData[myLocoData].myAddr.address >> 8) & 0x3F;
      SendPacket.data[9] = adr & 0x7F;
      SendPacket.data[10] = 0x70 | (bitRead(data, 7) << 1);
      SendPacket.data[11] = (adr >> 8) & 0x03;
      SendPacket.data[12] = data & 0x7F;
    }
    else {
      SendPacket.data[5] = 0x71 | (bitRead(adr, 7) << 1) | (bitRead(data, 7) << 3);
      SendPacket.data[6] = 0x71;
      SendPacket.data[7] = adr & 0x7F;
      SendPacket.data[8] = (adr >> 8) & 0x03;
      SendPacket.data[9] = data & 0x7F;
      SendPacket.data[10] = 0x70;
    }
    SendPacket.data[15] = 0x10;
  }
  else {
    SendPacket.data[0] = OPC_WR_SL_DATA;                            // Write in Direct mode or POM
    SendPacket.data[1] = 0x0E;
    SendPacket.data[2] = SLOT_PRG;                                  // Slot 0x7C
    //  SendPacket.data[4] = 0x00;
    if (modeProg) {
      SendPacket.data[3] = 0x64;                                    // PCMD PoM Write
      SendPacket.data[5] = (locoData[myLocoData].myAddr.address >> 7) & 0x7F;    // HOPSA Loco address
      SendPacket.data[6] = locoData[myLocoData].myAddr.address & 0x7F;           // LOPSA
    }
    else {
      SendPacket.data[3] = 0x68;                                    // PCMD Direct Write
      //    SendPacket.data[5] = 0x00;                              // HOPSA Loco address
      //    SendPacket.data[6] = 0x00;                              // LOPSA
    }
    SendPacket.data[7] = mySlot.trk;                                // TRK
    adr--;
    cvh = bitRead(adr, 7) | (bitRead(adr, 8) << 4) | (bitRead(adr, 9) << 5) | (bitRead(data, 7) << 1);
    SendPacket.data[8] = cvh;                                       // CVH <0,0,CV9,CV8 - 0,0, D7,CV7>
    SendPacket.data[9] = adr & 0x7F;                                // CVL
    SendPacket.data[10] = data & 0x7F;                              // DATA7
    //  SendPacket.data[11] = 0x00;
    //  SendPacket.data[12] = 0x00;
  }
  lnetSend(&SendPacket);
  progStepCV = stepPrg;
  //DEBUG_MSG("Write CV%d = %d", adr, data);
}


void sendLNCV (byte id, byte flags) {
  byte i;
  SendPacket.data[0]  = (flags == LNCV_FLAG_PROFF) ? OPC_PEER_XFER : OPC_IMM_PACKET;
  SendPacket.data[1]  = 0x0F;
  SendPacket.data[2]  = 0x01;
  SendPacket.data[3]  = 0x05;
  SendPacket.data[4]  = 0x00;
  SendPacket.data[5]  = id;
  SendPacket.data[6]  = 0x00;                                       // PXCT1
  SendPacket.data[7]  = lowByte(artNum);
  SendPacket.data[8]  = highByte(artNum);
  SendPacket.data[9]  = lowByte(numLNCV);
  SendPacket.data[10] = highByte(numLNCV);
  SendPacket.data[11] = lowByte(valLNCV);
  SendPacket.data[12] = highByte(valLNCV);
  SendPacket.data[13] = flags;
  for (i = 0; i < 7; i++) {                                         // set bits in PXCT1
    if (SendPacket.data[7 + i] & 0x80) {
      bitSet(SendPacket.data[6], i);
      bitClear(SendPacket.data[7 + i], 7);
    }
  }
  lnetSend(&SendPacket);
}


/*
  Detect Hardware type (Read Slot 0)
  0xE7, 0x0E, 0x00, 0x00, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x00, 0x49, 0x42, 0x18  "Intellibox / TwinCenter"               ADR: 0    ID: 'IB'  SPD: 2
  0xE7, 0x0E, 0x00, 0x03, 0x00, 0x03, 0x00, 0x06, 0x08, 0x00, 0x00, 0x49, 0x42, 0x13  "DR5000"                                ADR: 0    ID: 'IB'  SPD: 3
  0xE7, 0x0E, 0x00, 0x03, 0x00, 0x03, 0x00, 0x07, 0x08, 0x00, 0x00, 0x49, 0x42, 0x12  "YD7001"                                ADR: 0    ID: 'IB'  SPD: 3
  0xE7, 0x0E, 0x00, 0x02, 0x42, 0x03, 0x00, 0x07, 0x00, 0x00, 0x15, 0x49, 0x42, 0x4C  "Intellibox II / IB-Basic / IB-Com"     ADR: 'B'  ID: 'IB'  SPD: 3
  0xE7, 0x0E, 0x00, 0x02, 0x42, 0x03, 0x00, 0x07, 0x00, 0x00, 0x15, 0x49, 0x42, 0x4C  "Intellibox2neo"                        ADR: 'B'  ID: 'IB'  SPD: 3
  0xE7, 0x0E, 0x00, 0x02, 0x42, 0x03, 0x00, 0x06, 0x00, 0x00, 0x15, 0x49, 0x42, 0x4D  "System Control 7"                      ADR: 'B'  ID: 'IB'  SPD: 3
  0xE7, 0x0E, 0x00, 0x02, 0x42, 0x03, 0x00, 0x07, 0x00, 0x00, 0x15, 0x49, 0x42, 0x4C  "Daisy II Tillig"                       ADR: 'B'  ID: 'IB'  SPD: 3
  0xE7, 0x0E, 0x00, 0x02, 0x42, 0x03, 0x00, 0x07, 0x00, 0x00, 0x15, 0x49, 0x42, 0x4C  "Daisy II WLAN"                         ADR: 'B'  ID: 'IB'  SPD: 3
  0xE7, 0x0E, 0x00, 0x00, 0x44, 0x02, 0x00, 0x07, 0x00, 0x59, 0x01, 0x49, 0x42, 0x04  "Daisy"                                 ADR: 'DY' ID: 'IB'  SPD: 2
  0xE7, 0x0E, 0x00, 0x00, 0x4C, 0x01, 0x00, 0x07, 0x00, 0x49, 0x02, 0x49, 0x42, 0x1C  "Adapter 63820"                         ADR: 'LI' ID: 'IB'  SPD: 1
  0xE7, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x5A, 0x21, 0x6A  "Z21 Black"                             ADR: 0    ID: 'Z'21 SPD: 0
  0xE7, 0x0E, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11  "Digitrax Chief"                        ADR: 0    ID: 0     SPD: 0
  0xe7, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11  DCS100
  0xe7, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12  DCS200
  0xe7, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12  DCS50
  0xe7, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52  DCS52
  0xe7, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x25, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30  DT200
  0xe7, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51  DCS210
  0xe7, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x11, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40  DCS240
  0xe7, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x20, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x71  DCS240+
  0xe7, 0x0e, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16  DB150                                   ADR: 0    ID: 0     SPD: 4
  0xe7, 0x0e, 0x00, 0x33, 0x0e, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x2d  DCS51                                   ADR: 0    ID: 1     SPD: 0
  0xE7, 0x0E, 0x00, 0x00, 0x4E, 0x10, 0x01, 0x07, 0x00, 0x4C, 0x00, 0x4A, 0x46, 0x09  NanoL                                   ADR: 'NL' ID: 'JF'  SPD: 16
  0xB4, 0x3B, 0x00, 0x70                                                              RB1110                                  not supported

              SLOT  STAT1 ADRL  SPD   DIRF  TRK   STAT2 ADH   SND   ID1   ID2   CHK
*/

void identifyCS(lnMsg * LnPacket) {
  uint8_t typeCS;
  typeCS = CMD_UNKNOW;
  if (autoIdentifyCS) {
    switch (LnPacket->sd.id1) {
      case 'I':
        if (LnPacket->sd.id2 == 'B') {
          switch (LnPacket->sd.adr) {
            case 0:
              switch (LnPacket->sd.spd) {
                case 2:
                  DEBUG_MSG("Intellibox / TwinCenter");
                  typeCS = CMD_ULI;
                  break;
                case 3:
                  DEBUG_MSG("DR5000");
                  typeCS = CMD_DR;
                  break;
              }
              break;
            case 'B':
              DEBUG_MSG("Intellibox II / IB-Basic / IB-Com");
              typeCS = CMD_DR;
              break;
            case 'D':
              if (LnPacket->sd.adr2 == 'Y') {
                DEBUG_MSG("Daisy");
                typeCS = CMD_ULI;
              }
              break;
            case 'L':
              if (LnPacket->sd.adr2 == 'I') {
                DEBUG_MSG("Adapter 63820");                         // Only interface, commad station unknow
                typeCS = CMD_DIG;
              }
              break;
          }
        }
        break;
      case 'Z':
        if (LnPacket->sd.id2 == 0x21) {
          DEBUG_MSG("Z21 Black");
          typeCS = CMD_DR;
        }
        break;
      case 0x00:
        if (LnPacket->sd.id2 == 0x00) {
          DEBUG_MSG("Digitrax Chief");
          typeCS = CMD_DIG;
        }
        break;
    }
    if (typeCS == CMD_UNKNOW) {
      DEBUG_MSG("CS Unknow");
      autoIdentifyCS = 0x00;
    }
    else {
      typeCmdStation = typeCS;
      radioData[RAD_CSTATION].value = typeCS;
    }
  }
}

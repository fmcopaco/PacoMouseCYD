/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
      Very basic Graphical User Interface (GUI) for PacoMouseCYD
      All data in absolute coordinates
*/

////////////////////////////////////////////////////////////
// ***** GUI (Graphical User Interface) *****
////////////////////////////////////////////////////////////

void initGUI() {
  clearTimers();
  clearEventStack();
  clearObjectStack();
  createObject(OBJ_WIN, WIN_DEFAULT);
}


////////////////////////////////////////////////////////////
// ***** TIMER *****
////////////////////////////////////////////////////////////

void clearTimers() {
  uint16_t  n;
  for (n = 0; n < MAX_SYS_TIMER; n++)
    setTimer(n, 0, TMR_STOP);
  timerSys = millis();
}


void setTimer(uint16_t  id, uint16_t  count, uint16_t  type) {
  wTimerStack[id].tmrDelay = count;
  wTimerStack[id].tmrCount = count;
  wTimerStack[id].type = type;
}

void stopTimer(uint16_t  id) {
  setTimer(id, 0, TMR_STOP);
}


void timerProcess() {
  uint16_t  n;
  if (millis() - timerSys > TMR_RESOLUTION) {              // Every 100ms
    timerSys = millis();
    for (n = 0; n < MAX_SYS_TIMER; n++) {
      if (wTimerStack[n].type != TMR_STOP) {               // only running timers
        if (wTimerStack[n].tmrCount > 0) {
          wTimerStack[n].tmrCount--;
        }
        else {
          newEvent(OBJ_TIMER, n, EVNT_TIMER);              // timer expired
          if (wTimerStack[n].type == TMR_ONESHOT)
            wTimerStack[n].type = TMR_STOP;
          if (wTimerStack[n].type == TMR_PERIODIC)
            wTimerStack[n].tmrCount = wTimerStack[n].tmrDelay;
        }
      }
    }
  }
}


////////////////////////////////////////////////////////////
// ***** EVENT STACK *****
////////////////////////////////////////////////////////////

void clearEventStack() {                                // clear pending events
  eventIn = 0;
  eventOut = 0;
  eventsPending = 0;
}

wEvent getEvent() {                                     // get event from stack
  eventOut = (eventOut + 1) & 0x001F;
  eventsPending--;
  return eventStack[eventOut];
}

void putEvent(wEvent event) {                           // put event on stack
  eventIn = (eventIn + 1) & 0x001F;
  eventsPending++;
  eventStack[eventIn] = event;
}


void newEvent (uint16_t objType, uint16_t objID, uint16_t eventID) {    // create new event
  wEvent event;
  event.objID = objID;
  event.objType = objType;
  event.eventID = eventID;
  putEvent(event);
}


void sendClickEvent(uint16_t x, uint16_t y) {
  uint16_t pos, type, id, w, h;
  uint16_t xm, xM, ym, yM;
  pos = endObjStack;
  while (pos > 0) {
    id = objStack[pos].objID;
    switch (objStack[pos].objType) {
      case OBJ_WIN:
        xm = winData[id].x;
        ym = winData[id].y;
        xM = xm + winData[id].w;
        yM = ym + winData[id].h;
        break;
      case OBJ_ICON:
        xm = iconData[id].x;
        ym = iconData[id].y;
        xM = xm + iconData[id].w;
        yM = ym + iconData[id].h;
        //DEBUG_MSG("Click Icon ID: %d Pos: %d",objStack[pos].objID,pos)
        break;
      case OBJ_BUTTON:
        xm = buttonData[id].x;
        ym = buttonData[id].y;
        xM = xm + buttonData[id].w;
        yM = ym + buttonData[id].h;
        break;
      case OBJ_BAR:
        if (barData[id].w > barData[id].h) {              // Horizontal bar
          if (barData[id].r > (barData[id].h / 2)) {      // big cursor
            w = barData[id].r * 2;
            h = barData[id].h / 2;
            xm = barData[id].x - barData[id].r;
            ym = barData[id].y + h - barData[id].r;
            xM = xm + barData[id].w + w + 1;
            yM = ym + w + 1;
          }
          else {
            xm = barData[id].x;
            ym = barData[id].y;
            xM = xm + barData[id].w;
            yM = ym + barData[id].h;
          }
        }
        else {                                            // Vertical bar
          if (barData[id].r > (barData[id].w / 2)) {      // big cursor
            h = barData[id].r * 2;
            w = barData[id].w / 2;
            xm = barData[id].x + w - barData[id].r;
            ym = barData[id].y - barData[id].r;
            xM = xm + h + 1;
            yM = ym + barData[id].h + h + 1;
          }
          else {
            xm = barData[id].x;
            ym = barData[id].y;
            xM = xm + barData[id].w;
            yM = ym + barData[id].h;
          }
        }
        break;
      case OBJ_TXT:
        xm = txtData[id].x;
        ym = txtData[id].y;
        xM = xm + txtData[id].w;
        yM = ym + txtData[id].h;
        break;
      case OBJ_SWITCH:
        xm = switchData[id].x;
        ym = switchData[id].y;
        xM = xm + ((switchData[id].h) * 2);
        yM = ym + switchData[id].h;
        break;
      case OBJ_RADIO:
        xm = radioData[id].x;
        ym = radioData[id].y;
        xM = xm + radioData[id].h;
        yM = ym + (radioData[id].h * radioData[id].num);
        break;
      case OBJ_LPIC:
        xm = lpicData[id].x;
        ym = lpicData[id].y;
        xM = xm + LPIC_WIDTH;
        yM = ym + LPIC_HEIGHT;
        break;
      case OBJ_FNC:
        xm = fncData[id].x;
        ym = fncData[id].y;
        xM = xm + FNC_WIDTH;
        yM = ym + FNC_HEIGHT;
        break;
      case OBJ_KEYBOARD:
        xm = keybData[id].x;
        ym = keybData[id].y;
        switch (keybData[id].type) {
          case KEYB_KEYPAD:
            xM = xm + KEYPAD_WIDTH;
            yM = ym + KEYPAD_HEIGHT;
            break;
          case KEYB_KEYPAD_BIG:
          case KEYB_KEYPAD_OPT:
            xM = xm + KEYPAD_BIG_WIDTH;
            yM = ym + KEYPAD_BIG_HEIGHT;
            break;
          default:
            xM = xm + KEYB_WIDTH;
            yM = ym + KEYB_HEIGHT;
            break;
        }
        break;
      default:
        xm = tft.width();                               // other objects don't generate click event
        ym = tft.height();
        xM = xm;
        yM = ym;
    }
    //if ((x >= xm) && (x < xM) && (y >= ym) && (y < yM)) {   // check if click is inside object
    if (isInside(x, y, xm, xM, ym, yM)) {
      newEvent(objStack[pos].objType, id, EVNT_CLICK);
      lastClickX = x;
      lastClickY = y;
      return;
    }
    if ((pos == lastWinStack) && isModalWindow(objStack[lastWinStack].objID))   // With Modal window only cliks inside last window are valid
      return;
    pos--;
  }
}



////////////////////////////////////////////////////////////
// ***** GRAPHIC OBJECT STACK *****
////////////////////////////////////////////////////////////

void clearObjectStack() {                               // Clear list of objects
  endObjStack = 0;
  objStack[endObjStack].objType = OBJ_UNDEF;
  lastWinStack = endObjStack;
}


uint16_t createObject (uint16_t obj, uint16_t id) {     // Add a new object to the list of objects
  objStack[endObjStack].objType = obj;
  objStack[endObjStack].objID = id;
  if (obj == OBJ_WIN)
    lastWinStack = endObjStack;
  endObjStack++;
  objStack[endObjStack].objType = OBJ_UNDEF;
  //DEBUG_MSG("Create Object. Pos: %d", endObjStack - 1)
  return (endObjStack - 1);
}


uint16_t findWindow(uint16_t id) {
  uint16_t pos, cnt;
  pos = OBJ_NOT_FOUND;
  cnt = endObjStack;
  while (cnt > 0) {
    if ((objStack[cnt].objType == OBJ_WIN) && (objStack[cnt].objID == id)) {
      pos = cnt;
      cnt = 1;                                          // finded, don't search anymore
    }
    cnt--;
  }
  DEBUG_MSG("Window finded at pos: %d", pos)
  return pos;
}

uint16_t findPreviousWindow(uint16_t cnt) {             // find previous window from position
  uint16_t pos;
  pos = 0;
  while (cnt > 0) {
    if (objStack[cnt].objType == OBJ_WIN) {
      pos = cnt;
      cnt = 1;                                          // finded, don't search anymore
    }
    cnt--;
  }
  return pos;
}

uint16_t findNextWindow (uint16_t cnt) {                // find next window from position
  //uint16_t pos;
  DEBUG_MSG("Curr. window: %d, End stack: %d", cnt, endObjStack)
  cnt++;
  while (cnt < endObjStack) {
    if (objStack[cnt].objType == OBJ_WIN) {
      //pos = cnt;
      return cnt;
    }
    cnt++;
  }
  return endObjStack;
}


void closeWindow(uint16_t id) {                         // Close window
  uint16_t pos, nxt, cnt, posRedraw;
  if (objStack[lastWinStack].objID == id) {             // check if it is last window
    endObjStack = lastWinStack;
    objStack[endObjStack].objType = OBJ_UNDEF;
    DEBUG_MSG("Closing last window... %d", id)
  }
  else {
    DEBUG_MSG("Closing other window ID: %d", id)
    pos = findWindow(id);                               // find window and next window
    if (pos == OBJ_NOT_FOUND)
      return;
    nxt = findNextWindow(pos);
    DEBUG_MSG("Pos: %d, Next window: %d", pos, nxt)
    cnt = 0;
    while ((cnt + nxt) < endObjStack + 1) {             // move rest of objects
      objStack[pos + cnt] = objStack[nxt + cnt];
      cnt++;
    }
    endObjStack = pos + cnt - 1;
    DEBUG_MSG("Closing window pos: %d, nxt: %d", pos, nxt)
  }
  printObjStack();
  //lastWinStack = findPreviousWindow(endObjStack);       // update last window pointer
  for (pos = findContainerWindow(id); pos < endObjStack; pos++) {
    if (objStack[pos].objType == OBJ_WIN) {
      newEvent (OBJ_WIN, objStack[pos].objID, EVNT_DRAW);   // re-draw other windows
    }
  }
}


uint16_t findContainerWindow (uint16_t id) {                         // Find window of stack that contains the closed window
  uint16_t pos, x1, y1, xm, xM, ym, yM, winID;
  lastWinStack = findPreviousWindow(endObjStack);       // update last window pointer
  pos = lastWinStack;
  while (pos) {
    winID = objStack[pos].objID;
    xm = winData[winID].x;
    xM = xm + winData[winID].w + 1;
    ym = winData[winID].y;
    yM = xm + winData[winID].h + 1;
    x1 = winData[id].x;
    y1 = winData[id].y;
    if (isInside(x1, y1, xm, xM, ym, yM)) {                                          // Left top corner
      if (isInside(x1 + winData[id].w, y1, xm, xM, ym, yM)) {                        // Rigth top corner
        if (isInside(x1, y1 + winData[id].h, xm, xM, ym, yM)) {                      // Left bottom corner
          if (isInside(x1 + winData[id].w, y1 + winData[id].h, xm, xM, ym, yM)) {    // Right bottom corner
            DEBUG_MSG("Container window at %d", pos);
            return pos;
          }
        }
      }
    }
    pos = findPreviousWindow(pos - 1);
  }
  return pos;
}


void setModalWindow(uint16_t id, bool state) {
  winData[id].modal = state;
}


bool isModalWindow (uint16_t id) {
  return winData[id].modal;
}


bool isWindow(uint16_t win) {
  return (objStack[lastWinStack].objID == win);
}


////////////////////////////////////////////////////////////
// ***** DRAW OBJECTS *****
////////////////////////////////////////////////////////////

void printLabelTxt(uint16_t id) {
  char label[MAX_LABEL_LNG];
  uint16_t cnt, yPos;
  char chr;
  const char *language;
  language = translations[id][currLanguage];
  if (language == NULL)
    language = translations[id][LANG_ENGLISH];
  tft.setFreeFont(labelData[id].font);
  tft.setTextColor(labelData[id].color);
  tft.setTextDatum(labelData[id].align);
  yPos = labelData[id].y;
  cnt = 0;
  chr = *language++;
  while (chr) {
    if (chr == '\n') {
      label[cnt] = 0;
      tft.drawString(label, labelData[id].x, yPos, GFXFF);
      yPos += tft.fontHeight();
      cnt = 0;
      chr = *language++;
    }
    else {
      label[cnt++] = chr;
      chr = *language++;
    }
  }
  label[cnt] = 0;
  tft.drawString(label, labelData[id].x, yPos, GFXFF);
}


const char *getParmNum(const char *str, uint16_t *parm) {
  char chr;
  uint16_t value;
  value = 0;
  chr = *str;
  while (isDigit(chr)) {
    value = (value * 10) + (chr - '0');
    str++;
    chr = *str;
  }
  *parm = value;
  return str;
}


void drawDrawStr(uint16_t id) {
  char chr, cmd;
  uint16_t x, y, w, c, x2, y2, color, value, depth, band, colorRGB;
  const char *str;
  bool isSprite;
  x = drawStrData[id].x;
  y = drawStrData[id].y;
  color = COLOR_TRANSPARENT;
  depth = 8;
  band = 15;
  str = drawStrData[id].str;
  isSprite = false;
  chr = *str++;
  while (chr != 0) {
    if (chr != ',')
      cmd = chr;
    switch (chr) {
      case 'K':                                         // Kn:    Color n
        str = getParmNum(str, &color);
        //DEBUG_MSG("Color: %d", color);
        break;
      case 'k':                                         // kn:    Sprite color depth
        str = getParmNum(str, &depth);
        sprite.setColorDepth(depth);
        break;
      case ',':
        str = getParmNum(str, &value);
        //DEBUG_MSG("Value: %d", value);
        switch (cmd) {
          case 'R':
            if (isSprite)
              sprite.fillRect(x, y, w, value, colorDraw[color]);
            else
              tft.fillRect(x, y, w, value, colorDraw[color]);
            break;
          case 'r':
            if (isSprite)
              sprite.drawRect(x, y, w, value, colorDraw[color]);
            else
              tft.drawRect(x, y, w, value, colorDraw[color]);
            break;
          case 'L':
            if (isSprite)
              sprite.drawLine(x, y, w, value, colorDraw[color]);
            else
              tft.drawLine(x, y, w, value, colorDraw[color]);
            break;
          case 'l':
            if (isSprite)
              sprite.drawLine(x, y, w, value, colorDraw[color]);
            else
              tft.drawLine(x, y, w, value, colorDraw[color]);
            x = w;
            y = value;
            break;
          case 'p':
            y2 = value;
            break;
          case 'T':
            if (isSprite)
              sprite.fillTriangle(x, y, x2, y2, w, value, colorDraw[color]);
            else
              tft.fillTriangle(x, y, x2, y2, w, value, colorDraw[color]);
            break;
          case 't':
            if (isSprite)
              sprite.drawTriangle(x, y, x2, y2, w, value, colorDraw[color]);
            else
              tft.drawTriangle(x, y, x2, y2, w, value, colorDraw[color]);
            break;
          case 'S':
            sprite.createSprite(w, value);              // then create the sprite
            sprite.fillSprite(colorDraw[color]);
            isSprite = true;
            x = 0;
            y = 0;
            break;
          case 'D':
            colorRGB = colorDraw[color];
            for (c = 0; c < value; c += band) {
              if (isSprite)
                sprite.fillRect(x, y + c, w, band, colorRGB);
              else
                tft.fillRect(x, y + c, w, band, colorRGB);
              if (colorRGB & 0x001F)
                colorRGB -= 0x0001;
              if (colorRGB & 0x07E0)
                colorRGB -= 0x0040;
              if (colorRGB & 0xF800)
                colorRGB -= 0x0800;
            }
            break;
        }
        break;
      case 'R':                                         // Rw,h:  fill Rectangle
      case 'r':                                         // rw,h:  draw Rectangle
      case 'D':                                         // Dw,h:  Degrade rectangle
        str = getParmNum(str, &w);
        //DEBUG_MSG("Rect W: %d", w);
        break;
      case 'C':                                         // Cr:    fill Circle radius r
        str = getParmNum(str, &c);
        if (isSprite)
          sprite.fillCircle(x, y, c, colorDraw[color]);
        else
          tft.fillCircle(x, y, c, colorDraw[color]);
        break;
      case 'c':                                         // cr :   draw Circle radius r
        str = getParmNum(str, &c);
        if (isSprite)
          sprite.drawCircle(x, y, c, colorDraw[color]);
        else
          tft.drawCircle(x, y, c, colorDraw[color]);
        break;
      case 'l':                                         // lx,y:  draw polyline to x,y (X,Y set to end position: x,y)
      case 'L':                                         // Lx,y:  draw Line to x,y
        str = getParmNum(str, &w);
        break;
      case 't':                                         // tx,y:  draw triangle (X,Y),(px,y),(tx,y)
      case 'T':                                         // Tx,y:  fill triangle (X,Y),(px,y),(Tx,y)
        str = getParmNum(str, &w);
        break;
      case 'S':                                         // Sw,h:  create sprite and draw string into it. x & y set to 0, background color defined with Kn -> KnkdSw,h
        str = getParmNum(str, &w);
        break;
      case 's':                                         // sn:    draw sprite with transparent color n
        str = getParmNum(str, &value);
        //sprite.pushSprite(drawStrData[id].x, drawStrData[id].y, value);
        sprite.pushSprite(drawStrData[id].x, drawStrData[id].y, colorDraw[value]);
        sprite.deleteSprite();
        isSprite = false;
        break;
      case 'X':                                         // X:     x position
        str = getParmNum(str, &x);
        //DEBUG_MSG("X: %d", x);
        break;
      case 'x':                                         // x:     increment x position
        str = getParmNum(str, &value);
        x += value;
        break;
      case 'Y':                                         // Y:     y position
        str = getParmNum(str, &y);
        //DEBUG_MSG("Y: %d", y);
        break;
      case 'y':                                         // y:     increment y position
        str = getParmNum(str, &value);
        y += value;
        break;
      case 'p':
        str = getParmNum(str, &x2);                     // p:     second point (for triangles)  -> XxYypx,yTx,y
        break;
      case 'd':                                         // dn:    Degrade band height
        str = getParmNum(str, &band);
        break;
    }
    chr = *str++;
  }
}


void drawObject (uint16_t type, uint16_t id) {
  uint16_t value, state, w, h;
  char buf[20];
  delay(0);
  switch (type) {
    case OBJ_WIN:
      tft.fillRect(winData[id].x, winData[id].y, winData[id].w, winData[id].h, winData[id].backgnd);  // window box
      tft.drawRect(winData[id].x, winData[id].y, winData[id].w, winData[id].h, winData[id].color);
      break;
    case OBJ_LABEL:
      printLabelTxt(id);
      break;
    case OBJ_CHAR:
      tft.setFreeFont(charData[id].font);
      tft.setTextColor(charData[id].color);
      tft.setTextDatum(MC_DATUM);
      tft.drawChar(charData[id].chr, charData[id].x, charData[id].y);
      break;
    case OBJ_TXT:
      tft.fillRoundRect(txtData[id].x + 1, txtData[id].y + 1, txtData[id].w - 2, txtData[id].h - 2, 3, txtData[id].backgnd);
      tft.drawRoundRect(txtData[id].x, txtData[id].y, txtData[id].w, txtData[id].h, 3, txtData[id].border);
      tft.setFreeFont(txtData[id].font);
      tft.setTextColor(txtData[id].color);
      if (txtData[id].alignCenter) {
        tft.setTextDatum(MC_DATUM);
        tft.drawString(txtData[id].buf, txtData[id].x + (txtData[id].w / 2), txtData[id].y + (txtData[id].h / 2), GFXFF);
      }
      else {
        tft.setTextDatum(ML_DATUM);
        tft.drawString(txtData[id].buf, txtData[id].x + 3, txtData[id].y + (txtData[id].h / 2), GFXFF);
      }
      break;
    case OBJ_DRAWSTR:
      drawDrawStr(id);
      break;
    case OBJ_ICON:
      //tft.pushImage(260, 120, 24, 32, (uint16_t*)cara_paco24x32, false);  // 16 bits RGB565
      tft.drawBitmap(iconData[id].x, iconData[id].y, iconData[id].bitmap, iconData[id].w, iconData[id].h, iconData[id].color);  // monochrome
      //DEBUG_MSG("Print Icon ID: %d, X:%d, Y:%d, PTR:%d, W:%d, H:%d, Color:%d",id, iconData[id].x, iconData[id].y, iconData[id].bitmap, iconData[id].w, iconData[id].h, iconData[id].color);
      break;
    case OBJ_BUTTON:
      tft.fillRoundRect(buttonData[id].x + 1, buttonData[id].y + 1, buttonData[id].w - 2, buttonData[id].h - 2, 3, buttonData[id].backgnd);
      tft.drawRoundRect(buttonData[id].x, buttonData[id].y, buttonData[id].w, buttonData[id].h, 4, buttonData[id].border);
      if (buttonData[id].objType != OBJ_UNDEF)
        drawObject(buttonData[id].objType, buttonData[id].objID);
      break;
    case OBJ_BAR:
      if (barData[id].w > barData[id].h) {              // Horizontal bar
        if (barData[id].r > (barData[id].h / 2)) {
          w = barData[id].r * 2;
          h = barData[id].h / 2;
          tft.fillRect(barData[id].x - barData[id].r, barData[id].y + h - barData[id].r, barData[id].w + w + 1, w + 1, barData[id].backgnd);
        }
        value = map (barData[id].value, barData[id].min, barData[id].max, 0, barData[id].w);
        tft.fillRect(barData[id].x + 1, barData[id].y + 1, value, barData[id].h - 2, barData[id].colorOn);
        tft.fillRect(barData[id].x + value + 1, barData[id].y + 1, barData[id].w - value - 2, barData[id].h - 2, barData[id].colorOff);
        tft.drawRect(barData[id].x, barData[id].y, barData[id].w, barData[id].h, barData[id].border);
        if (barData[id].r > 0) {
          tft.fillCircle(barData[id].x + value, barData[id].y + (barData[id].h / 2), barData[id].r, barData[id].border);
        }
      }
      else {                                            // Vertical bar
        if (barData[id].r > (barData[id].w / 2)) {
          h = barData[id].r * 2;
          w = barData[id].w / 2;
          tft.fillRect(barData[id].x + w - barData[id].r, barData[id].y - barData[id].r, h + 1, barData[id].h + h + 1, barData[id].backgnd);
        }
        value = map (barData[id].value, barData[id].min, barData[id].max, 0, barData[id].h);
        tft.fillRect(barData[id].x + 1, barData[id].y + barData[id].h - value - 1,  barData[id].w - 2, value, barData[id].colorOn);
        tft.fillRect(barData[id].x + 1, barData[id].y + 1, barData[id].w - 2, barData[id].h - value, barData[id].colorOff);
        tft.drawRect(barData[id].x, barData[id].y, barData[id].w, barData[id].h, barData[id].border);
        if (barData[id].r > 0) {
          tft.fillCircle(barData[id].x + w, barData[id].y + barData[id].h - value, barData[id].r, barData[id].border);
        }
      }
      break;
    case OBJ_SWITCH:
      value = switchData[id].state ? switchData[id].colorOn : switchData[id].colorOff;
      tft.fillRoundRect(switchData[id].x, switchData[id].y, switchData[id].h * 2, switchData[id].h, switchData[id].h / 2, value);
      value = switchData[id].h / 2;
      state = switchData[id].state ? value + switchData[id].h : value;
      tft.fillCircle(switchData[id].x + state, switchData[id].y + value, value - 2, switchData[id].colorKnob);
      break;
    case OBJ_RADIO:
      w = radioData[id].h / 2;
      h = 0;
      for (value = 0; value < radioData[id].num; value++) {
        tft.fillCircle(radioData[id].x + w, radioData[id].y + w + h, radioData[id].r, radioData[id].backgnd);
        tft.drawCircle(radioData[id].x + w, radioData[id].y + w + h, radioData[id].r, radioData[id].border);
        if (value == radioData[id].value)
          tft.fillCircle(radioData[id].x + w, radioData[id].y + w + h, radioData[id].r - 2, radioData[id].border);
        h += radioData[id].h;
      }
      break;
    case OBJ_FNC:
      value = fncData[id].num;
      tft.drawBitmap(fncData[id].x, fncData[id].y, full32, 32, 32, fncData[id].backgnd);                              // delete
      tft.drawBitmap(fncData[id].x, fncData[id].y, funcIcon[fncData[id].idIcon], 32, 32, fncData[id].color);          // black
      if (fncData[id].state)
        tft.drawBitmap(fncData[id].x, fncData[id].y, funcIcon[fncData[id].idIcon + 1], 32, 32, fncData[id].colorOn);  // yellow
      if (value <= FNC_MAX) {
        snprintf(buf, 4, "%d", value);
        tft.setFreeFont(FSSB6);
        tft.setTextColor(COLOR_WHITE);                                                                                // white
        tft.setTextDatum(BR_DATUM);
        tft.drawString(buf, fncData[id].x + 31, fncData[id].y + 31, GFXFF);
      }
      break;

    case OBJ_LPIC:
      if (lpicData[id].id > 999) {
        sprintf (FileName, "/image/%d.bmp", lpicData[id].id );
        if (! drawBmp (FileName, lpicData[id].x, lpicData[id].y)) {
          value = sdDetected ? SYS_NO_LOK : SYS_ELOK;
          tft.drawBitmap(lpicData[id].x, lpicData[id].y, sysLocoPic[value], LPIC_WIDTH, LPIC_HEIGHT, COLOR_BLACK);          // monochrome
        }
      }
      else {
        tft.drawBitmap(lpicData[id].x, lpicData[id].y, sysLocoPic[lpicData[id].id], LPIC_WIDTH, LPIC_HEIGHT, COLOR_BLACK);  // monochrome
      }
      break;
    case OBJ_GAUGE:
      if (gaugeData[id].r > 0) {                        // Normal gauge
        tft.drawArc(gaugeData[id].x, gaugeData[id].y, gaugeData[id].r, gaugeData[id].r - 15, 52, 52 + gaugeData[id].value, gaugeData[id].color, gaugeData[id].backgnd, false);
        tft.drawArc(gaugeData[id].x, gaugeData[id].y, gaugeData[id].r, gaugeData[id].r - 15, 52 + gaugeData[id].value, 308, gaugeData[id].backgnd, gaugeData[id].color, false);
      }
      else {                                            // Speed gauge
        tft.setPivot(gaugeData[id].x, gaugeData[id].y); // Set pivot to middle of TFT screen
        sprite.setColorDepth(8);                        // Create an 8bpp Sprite
        sprite.createSprite(11, 13);                    // 8bpp requires 11 * 13 = 143 bytes
        sprite.setPivot(70, 7);                         // Set pivot relative to top left corner of Sprite
        sprite.fillSprite(gaugeData[id].backgnd);       // Fill the Sprite with background
        sprite.drawBitmap(0, 0, arc10, 11, 13, gaugeData[id].color);
        for (value = 330; value < 360; value += 10) {   // draw dial
          sprite.pushRotated(value);
        }
        for (value = 0; value < 230; value += 10) {
          sprite.pushRotated(value);
        }
        sprite.deleteSprite();
        delay(0);
        tft.drawArc(gaugeData[id].x, gaugeData[id].y, 71, 69, 270, 315, COLOR_RED, gaugeData[id].backgnd, false);
        tft.drawCircle(gaugeData[id].x, gaugeData[id].y, 75, COLOR_GOLD);
        tft.fillCircle(gaugeData[id].x, gaugeData[id].y, 20, gaugeData[id].color);
        delay(0);
        updateSpeedDir();                               // draw needle & dir
        delay(0);
      }
      break;
    case OBJ_KEYBOARD:
      switch (keybData[id].type) {
        case KEYB_ALPHA:
          drawKeyboard(keybData[id].x, keybData[id].y, KeybAlpha);
          break;
        case KEYB_NUM:
          drawKeyboard(keybData[id].x, keybData[id].y, KeybNum);
          break;
        case KEYB_SYM:
          drawKeyboard(keybData[id].x, keybData[id].y, KeybSym);
          break;
        case KEYB_KEYPAD_BIG:
          drawKeypadBig(keybData[id].x, keybData[id].y, KeybKeypad, true);
          break;
        case KEYB_KEYPAD_OPT:
          drawKeypadBig(keybData[id].x, keybData[id].y, KeybKeypad, false);
          break;
        case KEYB_KEYPAD:
          drawKeypad(keybData[id].x, keybData[id].y, KeybKeypad);
          break;
        default:
          drawKeyboard(keybData[id].x, keybData[id].y, KeybAlphaCaps);
          break;
      }
      break;


  }
  delay(0);
}


void drawWindow(uint16_t id) {
  uint16_t pos;
  //wEvent redraw;
  DEBUG_MSG("Finding window for printing... %d", id)
  pos = findWindow(id);
  if (pos != OBJ_NOT_FOUND) {
    DEBUG_MSG("Printing window box ID: %d", id)
    drawObject(OBJ_WIN, id);                              // draw window box
    pos++;
    printObjStack();
    while ((objStack[pos].objType != OBJ_UNDEF) && (objStack[pos].objType != OBJ_WIN)) {    // draw objects in window
      drawObject(objStack[pos].objType, objStack[pos].objID);
      //DEBUG_MSG("Drawing object pos: %d", pos)
      pos++;
    }
  }
}



bool drawBmp(const char *filename, int16_t x, int16_t y) {   // Draw BMP file (24 bits)
  File bmpFS;
  if ((x >= tft.width()) || (y >= tft.height()))
    return false;
  bmpFS = SD.open(filename);                            // Open requested file on SD card
  if (!bmpFS) {
    DEBUG_MSG("File %s not found", filename );
    return false;
  }
  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();
  if (read16(bmpFS) == 0x4D42) {                        // BMP signature
    read32(bmpFS);                                      // File size
    read32(bmpFS);                                      // Read & ignore creator bytes
    seekOffset = read32(bmpFS);                         // Start of image data
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) { // # planes -- must be '1', 24 bits per pixel, 0 = uncompressed
      y += h - 1;

      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3];

      for (row = 0; row < h; row++) {
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++) {        // Convert pixel from BMP to TFT format (RGB565)
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }
        if (padding)                                    // Read any line padding
          bmpFS.read((uint8_t*)tptr, padding);
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer); // Push the pixel row to screen, pushImage will crop the line if needed
        delay(0);
      }
      DEBUG_MSG("%s Loaded in %lu ms", filename, (millis() - startTime))
    }
    else {
      DEBUG_MSG("BMP format not recognized.")
    }
  }
  bmpFS.close();
  tft.setSwapBytes(false);
  return true;
}


uint16_t read16(fs::File & f) {                         // helper functions for BMP load
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}


uint32_t read32(fs::File & f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


void drawKey (uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  tft.fillRoundRect(x + 1, y + 1, w - 2, h - 2, 3, COLOR_NAVY); //inner button color
  tft.drawRoundRect(x, y, w, h, 3, COLOR_WHITE);        // outter button color
}


void drawKeyboard (uint16_t posX, uint16_t posY, const char type[][12]) {
  int x, y, shiftRight;
  tft.fillRect(posX + 1, posY, KEYB_WIDTH - 2, KEYB_HEIGHT - 2, COLOR_BACKGROUND);
  tft.setFreeFont(FSSB9);
  tft.setTextColor(COLOR_WHITE);
  //tft.setTextDatum(MC_DATUM);
  for (y = 0; y < 3; y++) {                             // Keys
    shiftRight = type[y][0];
    for (x = 2; x < type[y][1]; x++) {
      drawKey(posX + 8 + shiftRight + (23 * (x - 2)) , posY + (30 * y), 20, 25);
      tft.setCursor(posX + 11 + shiftRight + (23 * (x - 2)) , posY + 5 + 11 + (30 * y));
      tft.print(type[y][x]);
    }
  }
  drawKey(posX + 5, posY + 60, 30, 25);                 // ShiftKey
  tft.drawBitmap(posX + 11, posY + 65, shift, 16, 16, COLOR_WHITE);  // delete
  if (type == KeybAlphaCaps)
    tft.drawFastHLine(posX + 11 + 6, posY + 65 + 12, 5, COLOR_WHITE);
  drawKey(posX + 5, posY + 90, 40, 25);                 // Num
  tft.setCursor(posX + 8, posY + 11 + 95);
  tft.print("123");
  drawKey(posX + 200, posY + 60, 30, 25);               // BackSpace
  tft.drawBitmap(posX + 206, posY + 64, del, 16, 16, COLOR_WHITE);  // delete
  drawKey(posX + 189, posY + 90, 45, 25);               // Special Characters
  tft.setCursor(posX + 195, posY + 11 + 95);
  tft.print("#?+");
  drawKey(posX + 52, posY + 90, 130, 25);               // Spacebar
}


void drawKeypad (uint16_t posX, uint16_t posY, const char type[][12]) {
  int x, y, shiftRight;
  tft.fillRect(posX + 1, posY + 1, KEYPAD_WIDTH - 2, KEYPAD_HEIGHT - 2, COLOR_BACKGROUND);
  tft.setFreeFont(FSSB9);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextDatum(MC_DATUM);
  for (y = 0; y < 3; y++) {
    shiftRight = type[y][0];
    for (x = 2; x < type[y][1]; x++) {
      drawKey(posX + 8 + shiftRight + (23 * (x - 2)) , posY + (30 * y), 20, 25);
      tft.setCursor(posX + 12 + shiftRight + (23 * (x - 2)) , posY + 5 + 11 + (30 * y));
      tft.print(type[y][x]);
    }
  }
  drawKey(posX + 8, posY + 90, 32, 25);                 // Num 0
  tft.setCursor(posX + 18, posY + 11 + 95);
  tft.print("0");
  drawKey(posX + 43, posY + 90, 30, 25);                // BackSpace
  tft.drawBitmap(posX + 43 + 7, posY + 94, del, 16, 16, COLOR_WHITE); // delete
}

void drawKeypadBig (uint16_t posX, uint16_t posY, const char type[][12], bool enter) {
  int x, y, shiftRight;
  tft.fillRect(posX + 1, posY + 1, KEYPAD_BIG_WIDTH - 2, KEYPAD_BIG_HEIGHT - 2, COLOR_BACKGROUND);
  tft.setFreeFont(FSSB12);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextDatum(MC_DATUM);
  for (y = 0; y < 3; y++) {
    shiftRight = type[y][0];
    for (x = 2; x < type[y][1]; x++) {
      drawKey(posX + 8 + shiftRight + (46 * (x - 2)) , posY + (50 * y), 40, 40);
      tft.setCursor(posX + 20 + shiftRight + (46 * (x - 2)) , posY + 5 + 25 + (50 * y));
      tft.print(type[y][x]);
    }
  }
  drawKey(posX + 8, posY + 150, 40, 40);                    // BackSpace
  tft.drawBitmap(posX + 20, posY + 160, del, 16, 16, COLOR_WHITE); // delete
  drawKey(posX + 8 + 46, posY + 150, 40, 40);               // Num 0
  tft.setCursor(posX + 20 + 46, posY + 25 + 150);
  tft.print("0");
  drawKey(posX + 16 + 84, posY + 150, 40, 40);              // Enter
  if (enter)
    tft.drawBitmap(posX + 84 + 26, posY + 160, arrowL, 16, 16, COLOR_GREENYELLOW); // enter
}

////////////////////////////////////////////////////////////
// ***** SUPPORT *****
////////////////////////////////////////////////////////////

void getLabelTxt(uint16_t id, char *buf) {
  uint16_t cnt;
  char chr;
  const char *language;
  language = translations[id][currLanguage];
  if (language == NULL)
    language = translations[id][LANG_ENGLISH];
  cnt = 0;
  chr = *language++;
  //while (chr != END_OF_TEXT) {
  while (chr) {
    buf[cnt++] = chr;
    chr = *language++;
  }
  buf[cnt] = '\0';;
}


char getKeyTyped(uint16_t id, uint16_t x, uint16_t y) { // get key typed on keyboard object
  uint16_t x0, y0, xm, ym, xM, yM;
  uint16_t xKey, yKey, xShift, xNum;

  x0 = keybData[id].x;                                  // window position
  y0 = keybData[id].y;
  switch (keybData[id].type) {
    case KEYB_KEYPAD:                                   // keypad
      ym = y0 + 90;                                     // last row
      yM = ym + 25;
      xm = x0 + 8;                                        // Num 0
      xM = xm + 32;
      if (isInside(x, y, xm, xM, ym, yM))
        return '0';
      xm = x0 + 43;                                       // Backspace
      xM = xm + 32;
      if (isInside(x, y, xm, xM, ym, yM))
        return CHR_BKSPC;
      for (yKey = 0; yKey < 3; yKey++) {                  // check first three rows
        xShift = KeybKeypad[yKey][0];
        xNum = KeybKeypad[yKey][1];
        for (xKey = 2; xKey < xNum; xKey++) {
          xm = x0 + xShift + 8 + (23 * (xKey - 2));
          xM = xm + 20;
          ym = y0 + (30 * yKey);
          yM = ym + 25;
          if (isInside(x, y, xm, xM, ym, yM)) {
            return KeybKeypad[yKey][xKey];
          }
        }
      }
      break;
    case KEYB_KEYPAD_BIG:                               // keypad big
    case KEYB_KEYPAD_OPT:
      ym = y0 + 150;                                    // last row
      yM = ym + 40;
      xm = x0 + 8;                                      // Backspace
      xM = xm + 40;
      if (isInside(x, y, xm, xM, ym, yM))
        return CHR_BKSPC;
      xm = x0 + 8 + 46;                                 // Num 0
      xM = xm + 40;
      if (isInside(x, y, xm, xM, ym, yM))
        return '0';
      xm = x0 + 16 + 84;                                // Enter
      xM = xm + 40;
      if (isInside(x, y, xm, xM, ym, yM))
        return '\n';
      for (yKey = 0; yKey < 3; yKey++) {                    // check first three rows
        xShift = KeybKeypad[yKey][0];
        xNum = KeybKeypad[yKey][1];
        for (xKey = 2; xKey < xNum; xKey++) {
          xm = x0 + xShift + 8 + (46 * (xKey - 2));
          xM = xm + 40;
          ym = y0 + (50 * yKey);
          yM = ym + 40;
          if (isInside(x, y, xm, xM, ym, yM)) {
            return KeybKeypad[yKey][xKey];
          }
        }
      }
      break;
    default:                                            // keyboard
      ym = y0 + 90;                                     // last row
      yM = ym + 25;
      xm = x0 + 52;                                       // Spacebar
      xM = xm + 130;
      if (isInside(x, y, xm, xM, ym, yM))
        return ' ';
      xm = x0 + 5;                                        // Num
      xM = xm + 40;
      if (isInside(x, y, xm, xM, ym, yM)) {
        keybData[id].type = KEYB_NUM;
        return CHR_REDRAW;
      }
      xm = x0 + 189;                                      // Sym
      xM = xm + 45;
      if (isInside(x, y, xm, xM, ym, yM)) {
        keybData[id].type = KEYB_SYM;
        return CHR_REDRAW;
      }
      ym = y0 + 60;                                       // third row
      yM = ym + 25;
      xm = x0 + 200;                                      // Backspace
      xM = xm + 30;
      if (isInside(x, y, xm, xM, ym, yM))
        return CHR_BKSPC;
      xm = x0 + 5;                                        // ShiftKey
      xM = xm + 30;
      if (isInside(x, y, xm, xM, ym, yM)) {
        keybData[id].type = (keybData[id].type == KEYB_CAPS) ? KEYB_ALPHA : KEYB_CAPS;
        return CHR_REDRAW;
      }
      for (yKey = 0; yKey < 3; yKey++) {                    // check first three rows
        switch (keybData[id].type) {
          case KEYB_ALPHA:
            xShift = KeybAlpha[yKey][0];
            xNum = KeybAlpha[yKey][1];
            break;
          case KEYB_NUM:
            xShift = KeybNum[yKey][0];
            xNum = KeybNum[yKey][1];
            break;
          case KEYB_SYM:
            xShift = KeybSym[yKey][0];
            xNum = KeybSym[yKey][1];
            break;
          default:
            xShift = KeybAlphaCaps[yKey][0];
            xNum = KeybAlphaCaps[yKey][1];
            break;
        }
        for (xKey = 2; xKey < xNum; xKey++) {
          xm = x0 + xShift + 8 + (23 * (xKey - 2));
          xM = xm + 20;
          ym = y0 + (30 * yKey);
          yM = ym + 25;
          if (isInside(x, y, xm, xM, ym, yM)) {
            switch (keybData[id].type) {
              case KEYB_ALPHA:
                return KeybAlpha[yKey][xKey];
                break;
              case KEYB_NUM:
                return KeybNum[yKey][xKey];
                break;
              case KEYB_SYM:
                return KeybSym[yKey][xKey];
                break;
              default:
                return KeybAlphaCaps[yKey][xKey];
                break;
            }
          }
        }
      }
      break;
  }
  return 0;                                             // no key
}


bool isInside (uint16_t x, uint16_t y, uint16_t xm, uint16_t xM, uint16_t ym, uint16_t yM) {
  return ((x >= xm) && (x < xM) && (y >= ym) && (y < yM));
}



void printObjStack() {
#ifdef DEBUG
  return;
  for (uint16_t n = 0; n < (endObjStack + 1); n++) {
    Serial.print(objStack[n].objType);
    Serial.print("(");
    Serial.print(objStack[n].objID);
    Serial.print("), ");
  }
  Serial.println("<- Stack");
#endif
}

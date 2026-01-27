/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/
*/

enum language {LANG_ENGLISH, LANG_SPANISH, LANG_CATALAN, LANG_GERMAN, MAX_LANG};

static const char *translations[][MAX_LANG] = {             // DON'T CHANGE ORDER: Same order defined in labelObj. If a complete translation is missing, must be English definition.
  { // LBL_PACO_TXT
    [LANG_ENGLISH] = "PacoMouseCYD",
  },
  { // LBL_INIT
    [LANG_ENGLISH] = "Welcome!",
    [LANG_SPANISH] = "Bienvenido!",
    [LANG_CATALAN] = "Benvingut!",
    [LANG_GERMAN]  = "Willkommen!",
  },
  { // LBL_CONNECT
    [LANG_ENGLISH] = "Connecting...",
    [LANG_SPANISH] = "Conectando...",
    [LANG_CATALAN] = "Connectant...",
    [LANG_GERMAN]  = "Verbindung...",
  },
  { // LBL_PRESS
    [LANG_ENGLISH] = "Touch to start...",
    [LANG_SPANISH] = "Toca para empezar...",
    [LANG_CATALAN] = "Toca per iniciar...",
    [LANG_GERMAN]  = "Beruhren Sie den\nBildschirm, um \nzu beginnen...",
  },
  { // LBL_CAL
    [LANG_ENGLISH] = "To calibrate the\ntouchscreen, touch the\ncorners that appear\nwith the pen.",
    [LANG_SPANISH] = "Para calibrar la pantalla,\ntoque las esquinas que\naparecen con el lapiz",
    [LANG_CATALAN] = "Per calibrar la pantalla,\ntoqueu les cantonades que\napareixen amb el llapis",
    [LANG_GERMAN]  = "Um den Touchscreen\nzu kalibrieren, beruhren\nSie die angezeigten Ecken\nmit dem Stift.",
  },
  { // LBL_CAL_DONE
    [LANG_ENGLISH] = "Calibration done.",
    [LANG_SPANISH] = "Calibracion realizada.",
    [LANG_CATALAN] = "Calibratge realitzat.",
    [LANG_GERMAN]  = "Kalibrierung\nabgeschlossen.",
  },
  { // LBL_SCAN
    [LANG_ENGLISH] = "SSID WiFi\nScanning...",
    [LANG_SPANISH] = "SSID WiFi\nBuscando......",
    [LANG_CATALAN] = "SSID WiFi\nEscanejant...",
    [LANG_GERMAN]  = "SSID WiFi\nScannen...",
  },
  { // LBL_SSID_SCAN
    [LANG_ENGLISH] = "SSID WiFi",
  },
  { // LBL_SSID
    [LANG_ENGLISH] = "SSID",
  },
  { // LBL_IP
    [LANG_ENGLISH] = "IP",
  },
  { // LBL_PWD_HIDE
    [LANG_ENGLISH] = "Password",
    [LANG_SPANISH] = NULL,
    [LANG_CATALAN] = NULL,
    [LANG_GERMAN]  = "Passwort",
  },
  { // LBL_PORT
    [LANG_ENGLISH] = "Port",
    [LANG_SPANISH] = "Puerto",
  },
  { // LBL_PROTOCOL
    [LANG_ENGLISH] = "Protocol",
    [LANG_SPANISH] = "Protocolo",
    [LANG_CATALAN] = NULL,
    [LANG_GERMAN]  = "Protokoll",
  },
  { // LBL_SEL_PROT
    [LANG_ENGLISH] = "Protocol",
    [LANG_SPANISH] = "Protocolo",
    [LANG_CATALAN] = NULL,
    [LANG_GERMAN]  = "Protokoll",
  },
  { // LBL_SEL_Z21
    [LANG_ENGLISH] = "Z21",
  },
  { // LBL_SEL_XNET
    [LANG_ENGLISH] = "Xpressnet LAN",
  },
  { // LBL_SEL_ECOS
    [LANG_ENGLISH] = "ECoS",
  },
  { // LBL_SEL_LNET
    [LANG_ENGLISH] = "Loconet over TCP/IP",
  },
  { // LBL_SEL_LBSERVER
    [LANG_ENGLISH] = "LBServer",
  },
  { // LBL_SEL_BINARY
    [LANG_ENGLISH] = "Binary",
  },
  { // LBL_OPTIONS
    [LANG_ENGLISH] = "Options",
    [LANG_SPANISH] = "Opciones",
    [LANG_CATALAN] = "Opcions",
    [LANG_GERMAN]  = "Optionen",
  },
  { // LBL_NAME
    [LANG_ENGLISH] = "Name",
    [LANG_SPANISH] = "Nombre",
    [LANG_CATALAN] = "Nom",
    [LANG_GERMAN]  = NULL,
  },
  { // LBL_ADDR
    [LANG_ENGLISH] = "Address",
    [LANG_SPANISH] = "Direccion",
    [LANG_CATALAN] = "Direccio",
    [LANG_GERMAN]  = "Adresse",
  },
  { // LBL_IMAGE
    [LANG_ENGLISH] = "Image",
    [LANG_SPANISH] = "Imagen",
    [LANG_CATALAN] = "Imatge",
    [LANG_GERMAN]  = "Bild",
  },
  { // LBL_VMAX
    [LANG_ENGLISH] = "Speed max.",
    [LANG_SPANISH] = "Vel. max.",
    [LANG_CATALAN] = "Vel. max.",
    [LANG_GERMAN]  = "Geschw.max.",
  },
  { // LBL_FUNC
    [LANG_ENGLISH] = "Functions",
    [LANG_SPANISH] = "Funciones",
    [LANG_CATALAN] = "Funcions",
    [LANG_GERMAN]  = "Funktionen",
  },
  { // LBL_SERVICE
    [LANG_ENGLISH] = "Service Mode",
    [LANG_SPANISH] = "Modo Servicio",
    [LANG_CATALAN] = "Mode Servei",
    [LANG_GERMAN]  = "Servicemodus",
  },
  { // LBL_KMH
    [LANG_ENGLISH] = "km/h",
  },
  { // LBL_SHUNTING
    [LANG_ENGLISH] = "Shunting",
    [LANG_SPANISH] = "Maniobras",
    [LANG_CATALAN] = "Maniobres",
    [LANG_GERMAN]  = "Rangieren",
  },
  { // LBL_RATE
    [LANG_ENGLISH] = "Rate  1:",
    [LANG_SPANISH] = "Ratio  1:",
    [LANG_CATALAN] = "Ratio  1:",
    [LANG_GERMAN]  = NULL,
  },
  { // LBL_CHG_WIFI
    [LANG_ENGLISH] = "Reset to apply\nchanges",
    [LANG_SPANISH] = "Reinicie para\naplicar los\ncambios",
    [LANG_CATALAN] = "Reiniciar per \naplicar els\ncanvis",
    [LANG_GERMAN]  = "Neustart, um die\nAnderungen zu\nübernehmen",
  },
  { // LBL_EDIT_FUNC
    [LANG_ENGLISH] = "Functions",
    [LANG_SPANISH] = "Funciones",
    [LANG_CATALAN] = "Funcions",
    [LANG_GERMAN]  = "Funktionen",
  },
  { // LBL_STACK_FULL
    [LANG_ENGLISH] = "Locomotive stack\nfull!",
    [LANG_SPANISH] = "Almacen de\nlocomotoras\nlleno!",
    [LANG_CATALAN] = "Magatzem de\nlocomotores\nple!",
    [LANG_GERMAN]  = "Lokomotivstapel\nvoll!",
  },
  { // LBL_STOP_0
    [LANG_ENGLISH] = "Speed 0",
    [LANG_SPANISH] = "Velocidad 0",
    [LANG_CATALAN] = "Velocitat 0",
    [LANG_GERMAN]  = "Geschw. 0",
  },
  { // LBL_STOP_E
    [LANG_ENGLISH] = "Emerg. Stop",
    [LANG_SPANISH] = "Stop Emerg.",
    [LANG_CATALAN] = "Stop Emerg.",
    [LANG_GERMAN]  = "Not-Halt",
  },
  { // LBL_SEL_IMAGE
    [LANG_ENGLISH] = "Image",
    [LANG_SPANISH] = "Imagen",
    [LANG_CATALAN] = "Imatge",
    [LANG_GERMAN]  = "Bild",
  },
  { // LBL_MENU_DRIVE
    [LANG_ENGLISH] = "Drive",
    [LANG_SPANISH] = "Conducir",
    [LANG_CATALAN] = "Conduir",
    [LANG_GERMAN]  = "Fahren",
  },
  { // LBL_MENU_ACC
    [LANG_ENGLISH] = "Accesory",
    [LANG_SPANISH] = "Accesorios",
    [LANG_CATALAN] = "Accesoris",
    [LANG_GERMAN]  = "Zubehorartikel",
  },
  { // LBL_MENU_CV
    [LANG_ENGLISH] = "CV Programming",
    [LANG_SPANISH] = "Programar CV",
    [LANG_CATALAN] = "Programar CV",
    [LANG_GERMAN]  = "CV-Programmierung",
  },
  { // LBL_MENU_CFG
    [LANG_ENGLISH] = "Configure",
    [LANG_SPANISH] = "Configurar",
    [LANG_CATALAN] = "Configurar",
    [LANG_GERMAN]  = "Einstellungen",
  },
  { // LBL_MENU_UTILS
    [LANG_ENGLISH] = "Utilities",
    [LANG_SPANISH] = "Utilidades",
    [LANG_CATALAN] = "Utilitats",
    [LANG_GERMAN]  = "Dienstprogramme",
  },
  { // LBL_CFG_LANG
    [LANG_ENGLISH] = "Language",
    [LANG_SPANISH] = "Idioma",
    [LANG_CATALAN] = "Idioma",
    [LANG_GERMAN]  = "Sprache",
  },
  { // LBL_CFG_SCR
    [LANG_ENGLISH] = "Screen",
    [LANG_SPANISH] = "Pantalla",
    [LANG_CATALAN] = "Pantalla",
    [LANG_GERMAN]  = "Bildschirm",
  },
  { // LBL_CFG_SPD
    [LANG_ENGLISH] = "Speed",
    [LANG_SPANISH] = "Velocidad",
    [LANG_CATALAN] = "Velocitat",
    [LANG_GERMAN]  = "Geschwindigkeit",
  },
  { // LBL_CFG_WIFI
    [LANG_ENGLISH] = "WiFi",
  },
  { // LBL_CFG_FCLK
    [LANG_ENGLISH] = "Fast Clock",
    [LANG_SPANISH] = "Reloj",
    [LANG_CATALAN] = "Rellotge",
    [LANG_GERMAN]  = "Uhr",
  },
  { // LBL_CFG_LOCK
    [LANG_ENGLISH] = "Lock",
    [LANG_SPANISH] = "Bloquear",
    [LANG_CATALAN] = "Bloquejar",
    [LANG_GERMAN]  = "Sperre",
  },
  { // LBL_CFG_ABOUT
    [LANG_ENGLISH] = "About...",
    [LANG_SPANISH] = "Acerca...",
    [LANG_CATALAN] = "Sobre...",
    [LANG_GERMAN]  = "Info...",
  },
  { // LBL_SCR_ROTATE
    [LANG_ENGLISH] = "Rotate",
    [LANG_SPANISH] = "Girar",
    [LANG_CATALAN] = "Girar",
    [LANG_GERMAN]  = "Drehen",
  },
  { // LBL_PACO_WEB
    [LANG_ENGLISH] = "https://usuaris.tinet.cat/fmco",
  },
  { // LBL_LOCK_LOK
    [LANG_ENGLISH] = "Locomotives",
    [LANG_SPANISH] = "Locomotoras",
    [LANG_CATALAN] = "Locomotores",
    [LANG_GERMAN]  = "Lokomotiven",
  },
  { // LBL_LOCK_ACC
    [LANG_ENGLISH] = "Accesory",
    [LANG_SPANISH] = "Accesorios",
    [LANG_CATALAN] = "Accesoris",
    [LANG_GERMAN]  = "Zubehorartikel",
  },
  { // LBL_LOCK_PRG
    [LANG_ENGLISH] = "Programming",
    [LANG_SPANISH] = "Programar",
    [LANG_CATALAN] = "Programar",
    [LANG_GERMAN]  = "Programmierung",
  },
  /*
    { // LBL_OPT_ROCO
    [LANG_ENGLISH] = "Turntable offset",
    [LANG_SPANISH] = "Offset Plataforma",
    [LANG_CATALAN] = "Offset Plataforma",
    [LANG_GERMAN]  = "Drehscheibe Versatz",
    },
  */
  { // LBL_OPT_ADR
    [LANG_ENGLISH] = "Short Addr. (1 to 99)",
    [LANG_SPANISH] = "Dir. corta (1 a 99)",
    [LANG_CATALAN] = "Dir. curta (1 a 99)",
    [LANG_GERMAN]  = "Kurze Adr. (1 bis 99)",
  },
  { // LBL_OPT_IB2
    [LANG_ENGLISH] = "IBII / DR5000",
  },
  { // LBL_OPT_UHLI
    [LANG_ENGLISH] = "Uhlenbrock",
  },
  { // LBL_OPT_DIG
    [LANG_ENGLISH] = "Digitrax",
  },
  { // LBL_ESTOP
    [LANG_ENGLISH] = "Emergency Stop",
    [LANG_SPANISH] = "Stop Emergencia",
    [LANG_CATALAN] = "Stop Emergencia",
    [LANG_GERMAN]  = "Nothalt",
  },
  { // LBL_SCALE
    [LANG_ENGLISH] = "Scale",
    [LANG_SPANISH] = "Escala",
    [LANG_CATALAN] = "Escala",
    [LANG_GERMAN]  = "Skala",
  },
  { // LBL_MM
    [LANG_ENGLISH] = "mm",
  },
  { // LBL_SCALE_H0
    [LANG_ENGLISH] = "H0",
  },
  { // LBL_SCALE_N
    [LANG_ENGLISH] = "N",
  },
  { // LBL_SCALE_TT
    [LANG_ENGLISH] = "TT",
  },
  { // LBL_SCALE_Z
    [LANG_ENGLISH] = "Z",
  },
  { // LBL_SCALE_0
    [LANG_ENGLISH] = "0",
  },
  { // LBL_MEASURE
    [LANG_ENGLISH] = "Measuring",
    [LANG_SPANISH] = "Midiendo",
    [LANG_CATALAN] = "Mesurant",
    [LANG_GERMAN]  = "Messung",
  },
  { // LBL_CV_ADDR
    [LANG_ENGLISH] = "Loco Address",
    [LANG_SPANISH] = "Direcc. Loco",
    [LANG_CATALAN] = "Direcc. Loco",
    [LANG_GERMAN]  = "Lokadresse",
  },
  { // LBL_CV_SPD_L
    [LANG_ENGLISH] = "Speed min.",
    [LANG_SPANISH] = "Velocidad min.",
    [LANG_CATALAN] = "Velocitat min.",
    [LANG_GERMAN]  = "Minimale Geschw.",
  },
  { // LBL_CV_SPD_M
    [LANG_ENGLISH] = "Speed mid.",
    [LANG_SPANISH] = "Velocidad media",
    [LANG_CATALAN] = "Velocitat mitja",
    [LANG_GERMAN]  = "Mittlere Geschw.",
  },
  { // LBL_CV_SPD_H
    [LANG_ENGLISH] = "Speed max.",
    [LANG_SPANISH] = "Velocidad max.",
    [LANG_CATALAN] = "Velocitat max.",
    [LANG_GERMAN]  = "Maximale Geschw.",
  },
  { // LBL_CV_ACC
    [LANG_ENGLISH] = "Acceleration",
    [LANG_SPANISH] = "Aceleracion",
    [LANG_CATALAN] = "Acceleracio",
    [LANG_GERMAN]  = "Beschleunig",
  },
  { // LBL_CV_DEC
    [LANG_ENGLISH] = "Braking",
    [LANG_SPANISH] = "Frenado",
    [LANG_CATALAN] = "Frenada",
    [LANG_GERMAN]  = "Bremsen",
  },
  { // LBL_CV_CFG
    [LANG_ENGLISH] = "Configuration",
    [LANG_SPANISH] = "Configuracion",
    [LANG_CATALAN] = "Configuracio",
    [LANG_GERMAN]  = "Konfiguration",
  },
  { // LBL_CV_MAN
    [LANG_ENGLISH] = "Manufacturer",
    [LANG_SPANISH] = "Fabricante",
    [LANG_CATALAN] = "Fabricant",
    [LANG_GERMAN]  = "Hersteller",
  },
  { // LBL_CV
    [LANG_ENGLISH] = "CV",
  },
  { // LBL_LNCV
    [LANG_ENGLISH] = "LNCV",
  },
  { // LBL_POM
    [LANG_ENGLISH] = "PoM",
  },
  { // LBL_BITS
    [LANG_ENGLISH] = "Bits",
  },
  { // LBL_CV_ERROR
    [LANG_ENGLISH] = "CV ERROR",
    [LANG_SPANISH] = "ERROR CV",
    [LANG_CATALAN] = "ERROR CV",
    [LANG_GERMAN]  = "CV-FEHLER",
  },
  { // LBL_UTIL_SPEED
    [LANG_ENGLISH] = "Measure speed",
    [LANG_SPANISH] = "Medir velocidad",
    [LANG_CATALAN] = "Mesurar velocitat",
    [LANG_GERMAN]  = "Geschw. messen",
  },
  { // LBL_UTIL_STEAM
    [LANG_ENGLISH] = "Steam locomotive",
    [LANG_SPANISH] = "Locomotora de vapor",
    [LANG_CATALAN] = "Locomotora de vapor",
    [LANG_GERMAN]  = "Dampflokomotive",
  },
  { // LBL_UTIL_SCAN
    [LANG_ENGLISH] = "WiFi Analyzer",
    [LANG_SPANISH] = "Analizador de WiFi",
    [LANG_CATALAN] = "Analitzador de WiFi",
    [LANG_GERMAN]  = "WLAN-Analysator",
  },
  { // LBL_UTIL_STA
    [LANG_ENGLISH] = "Station Run",
    [LANG_SPANISH] = "Carrera de estaciones",
    [LANG_CATALAN] = "Carrera d'estacions",
    [LANG_GERMAN]  = "Bahnhofsrennen",
  },
  { // LBL_ASK_SURE
    [LANG_ENGLISH] = "Are you sure?",
    [LANG_SPANISH] = "Estas seguro?",
    [LANG_CATALAN] = "Segur?",
    [LANG_GERMAN]  = "Bist du sicher?",
  },
  { // LBL_OPT_DISCOVER
    [LANG_ENGLISH] = "Discover",
    [LANG_SPANISH] = "Descubrir",
    [LANG_CATALAN] = "Descobrir",
    [LANG_GERMAN]  = "Entdecken",
  },
  { // LBL_LNCV_ART
    [LANG_ENGLISH] = "Article",
    [LANG_SPANISH] = "Articulo",
    [LANG_CATALAN] = NULL,
    [LANG_GERMAN]  = "Artikel",
  },
  { // LBL_LNCV_MOD
    [LANG_ENGLISH] = "Module",
    [LANG_SPANISH] = "Modulo",
    [LANG_CATALAN] = "Modul",
    [LANG_GERMAN]  = "Modul",
  },
  { // LBL_LNCV_NUM
    [LANG_ENGLISH] = "LNCV",
  },
  { // LBL_ACC_TYPE
    [LANG_ENGLISH] = "Accessory type",
    [LANG_SPANISH] = "Tipo accesorio",
    [LANG_CATALAN] = "Tipus accessori",
    [LANG_GERMAN]  = "Zubehortyp",
  },
  { // LBL_ACC_NAME
    [LANG_ENGLISH] = "Name",
    [LANG_SPANISH] = "Nombre",
    [LANG_CATALAN] = "Nom",
    [LANG_GERMAN]  = NULL,
  },
  { // LBL_ACC_ADDR
    [LANG_ENGLISH] = "Addr.",
    [LANG_SPANISH] = "Dir.",
    [LANG_CATALAN] = "Dir.",
    [LANG_GERMAN]  = "Adr.",
  },
  { // LBL_STA_RUN
    [LANG_ENGLISH] = "Station Run",
    [LANG_SPANISH] = "Carrera de estaciones",
    [LANG_CATALAN] = "Carrera d'estacions",
    [LANG_GERMAN]  = "Bahnhofsrennen",
  },
  { // LBL_STA_LEVEL
    [LANG_ENGLISH] = "Level:",
    [LANG_SPANISH] = "Nivel:",
    [LANG_CATALAN] = "Nivell:",
    [LANG_GERMAN]  = NULL,
  },
  { // LBL_STA_START
    [LANG_ENGLISH] = "Start",
    [LANG_SPANISH] = "Iniciar",
    [LANG_CATALAN] = "Iniciar",
    [LANG_GERMAN]  = "Starten",
  },
  { // LBL_STA_INSTR
    [LANG_ENGLISH] = "Go to the station of your\ndestination color.\nPress it when you arrive",
    [LANG_SPANISH] = "Ve a la estacion del color\nde tu destino.\nPulsala  cuando llegues",
    [LANG_CATALAN] = "Ves a l'estacio del color\nde la teva destinacio.\nPrem-la quan arribis",
    [LANG_GERMAN]  = "Gehen Sie sich zum Bahnhof\nIhrer Zielfarbe. Klicken Sie\ndas Symbol, wenn Sie ankommen",
  },
  { // LBL_STA_EXCEL
    [LANG_ENGLISH] = "Excellent!",
    [LANG_SPANISH] = "Excelente!",
    [LANG_CATALAN] = "Excel.lent!",
    [LANG_GERMAN]  = "Exzellent!",
  },
  { // LBL_STA_GREAT
    [LANG_ENGLISH] = "Great!",
    [LANG_SPANISH] = "Muy bien!",
    [LANG_CATALAN] = "Molt be!",
    [LANG_GERMAN]  = "Sehr gut!",
  },
  { // LBL_STA_TIMEOUT
    [LANG_ENGLISH] = "TIME'S UP\nThanks for playing",
    [LANG_SPANISH] = "FIN DEL TIEMPO\nGracias por jugar",
    [LANG_CATALAN] = "FINAL DEL TEMPS\nGracies per jugar",
    [LANG_GERMAN]  = "DIE ZEIT IST UM\nDanke furs Spielen",
  },
  { // LBL_STA_STATIONS
    [LANG_ENGLISH] = "Stations",
    [LANG_SPANISH] = "Estaciones",
    [LANG_CATALAN] = "Estacions",
    [LANG_GERMAN]  = "Bahnhofe",
  },
  { // LBL_STA_TURNOUTS
    [LANG_ENGLISH] = "Turnouts",
    [LANG_SPANISH] = "Desvios",
    [LANG_CATALAN] = "Agulles",
    [LANG_GERMAN]  = "Weichen",
  },
  { // LBL_STA_TIME
    [LANG_ENGLISH] = "Time",
    [LANG_SPANISH] = "Tiempo",
    [LANG_CATALAN] = "Temps",
    [LANG_GERMAN]  = "Zeit",
  },
  { // LBL_STA_DESC
    [LANG_ENGLISH] = "Address  Orientat.   Inverted",
    [LANG_SPANISH] = "Direcc.  Orientac.   Invertido",
    [LANG_CATALAN] = "Direcc.  Orientacio   Invertit",
    [LANG_GERMAN]  = "Adresse  Ausricht.  Invertiert",
  },
};

/*    PacoMouseCYD throttle -- F. Cañada 2025-2026 --  https://usuaris.tinet.cat/fmco/

      This software and associated files are a DIY project that is not intended for commercial use.
      This software uses libraries with different licenses, follow all their different terms included.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.

      Sources are only provided for building and uploading to the device.
      You are not allowed to modify the source code or fork/publish this project.
      Commercial use is forbidden.
*/

// OPCODES
#define OPC_GPOFF               0x82              // GLOBAL power OFF request 
#define OPC_GPON                0x83              // GLOBAL power ON request

#define OPC_LOCO_SPD            0xA0              // SET SLOT speed
#define OPC_LOCO_DIRF           0xA1              // SET SLOT dir,F0-4 state
#define OPC_LOCO_SND            0xA2              // SET SLOT sound functions 
#define OPC_LOCO_F9F12          0xA3              // Uhlenbrock 

#define OPC_SW_REQ              0xB0              // REQ SWITCH function
#define OPC_SW_REP              0xB1              // Turnout  SENSOR state REPORT
#define OPC_INPUT_REP           0xB2              // General SENSOR Input codes

#define OPC_LONG_ACK            0xB4              // Long acknowledge 
#define OPC_SLOT_STAT1          0xB5              // WRITE slot stat1

#define OPC_MOVE_SLOTS          0xBA              // MOVE slot SRC to DEST
#define OPC_RQ_SL_DATA          0xBB              // Request SLOT DATA/status block
#define OPC_SW_STATE            0xBC              // REQ state of SWITCH
#define OPC_LOCO_ADR_UHLI       0xBE              // REQ  loco ADR Uhlenbrock
#define OPC_LOCO_ADR            0xBF              // REQ  loco ADR

#define OPC_UHLI_FUN            0xD4              // Uhlenbrock

#define OPC_PEER_XFER           0xE5              // move 8 bytes PEER to PEER, SRC->DST
#define OPC_SL_RD_UHLI          0xE6              // SLOT DATA return, 21 bytes Uhlenbrock
#define OPC_SL_RD_DATA          0xE7              // SLOT DATA return, 10 bytes
#define OPC_IMM_PACKET          0xED              // SEND n-byte packet immediate
#define OPC_WR_SL_UHLI          0xEE              // WRITE SLOT DATA, 21 bytes Uhlenbrock
#define OPC_WR_SL_DATA          0xEF              // WRITE SLOT DATA, 10 bytes


// BIT MASK
#define OPC_SW_REP_INPUTS       0x40              // sensor inputs, outputs otherwise  
#define OPC_SW_REP_SW           0x20              // switch input, aux input otherwise 
#define OPC_SW_REP_HI           0x10              // input is HI, LO otherwise
#define OPC_SW_REP_CLOSED       0x20              // 'Closed' line is ON, OFF otherwise  
#define OPC_SW_REP_THROWN       0x10              // 'Thrown' line is ON, OFF otherwise  

#define OPC_SW_REQ_DIR          0x20              // switch direction - closed/thrown
#define OPC_SW_REQ_OUT          0x10              // output On/Off                        

#define OPC_INPUT_REP_SW        0x20              // input is switch input, aux otherwise 
#define OPC_INPUT_REP_HI        0x10              // input is HI, LO otherwise   

#define STAT1_SL_BUSY           0x20              // BUSY/ACTIVE: bit encoding for SLOT activity
#define STAT1_SL_ACTIVE         0x10

#define GTRK_PROG_BUSY          0x08              // programming track is Busy
#define GTRK_IDLE               0x02              // 0 = Track paused, B'cast EMERG STOP, 1 = Power On
#define GTRK_POWER              0x01              // DCC packets are on and global power is up

// VALUES
#define SLOT_0                  0x00              // Slot 0. Identifies command station type if implemented
#define SLOT_FC                 0x7B              // Fast clock slot
#define SLOT_PRG                0x7C              // This slot communicates with the programming track

#define UHLI_PRG_START          0x41              // Intellibox II program task
#define UHLI_PRG_END            0x40

#define LNCV_REQID_CFGREAD      0x1F              // LNCV task
#define LNCV_REQID_CFGWRITE     0x20
#define LNCV_REQID_CFGREQUEST   0x21
#define LNCV_FLAG_PRON          0x80
#define LNCV_FLAG_PROFF         0x40


// Message structure to determine the size of a message
typedef struct {
  uint8_t command;   /* LocoNet Op Code */
  uint8_t mesg_size; /* size of the message in bytes */
} szMsg;

/* Turnout sensor state report */
typedef struct swrep_t {
  uint8_t command;
  uint8_t sn1;           /* first  byte of report                                */
  uint8_t sn2;           /* second byte of report                                */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} swRepMsg;

/* Request Switch function */
typedef struct swreq_t {
  uint8_t command;
  uint8_t sw1;           /* first  byte of request                               */
  uint8_t sw2;           /* second byte of request                               */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} swReqMsg;

/* Sensor input report */
typedef struct inputrep_t {
  uint8_t command;
  uint8_t in1;           /* first  byte of report                                */
  uint8_t in2;           /* second byte of report                                */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} inputRepMsg;

/* Slot data request */
typedef struct slotreq_t {
  uint8_t command;
  uint8_t slot;          /* slot number for this request                         */
  uint8_t pad;           /* should be zero                                       */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} slotReqMsg;

/* Read/Write Slot data messages */
typedef struct rwslotdata_t {
  uint8_t command;
  uint8_t mesg_size;     /* ummmmm, size of the message in bytes?                */
  uint8_t slot;          /* slot number for this request                         */
  uint8_t stat;          /* slot status                                          */
  uint8_t adr;           /* loco address                                         */
  uint8_t spd;           /* command speed                                        */
  uint8_t dirf;          /* direction and F0-F4 bits                             */
  uint8_t trk;           /* track status                                         */
  uint8_t ss2;           /* slot status 2 (tells how to use ID1/ID2 & ADV Consist*/
  uint8_t adr2;          /* loco address high                                    */
  uint8_t snd;           /* Sound 1-4 / F5-F8                                    */
  uint8_t id1;           /* ls 7 bits of ID code                                 */
  uint8_t id2;           /* ms 7 bits of ID code                                 */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} rwSlotDataMsg;

/* Fast Clock Message */
typedef struct fastclock_t {
  uint8_t command;
  uint8_t mesg_size;     /* ummmmm, size of the message in bytes?                    */
  uint8_t slot;          /* slot number for this request                             */
  uint8_t clk_rate;      /* 0 = Freeze clock, 1 = normal, 10 = 10:1 etc. Max is 0x7f */
  uint8_t frac_minsl;    /* fractional minutes. not for external use.                */
  uint8_t frac_minsh;
  uint8_t mins_60;       /* 256 - minutes   */
  uint8_t track_stat;    /* track status    */
  uint8_t hours_24;      /* 256 - hours     */
  uint8_t days;          /* clock rollovers */
  uint8_t clk_cntrl;     /* bit 6 = 1; data is valid clock info */
  /*  "  "   0; ignore this reply        */
  uint8_t id1;           /* id1/id2 is device id of last device to set the clock */
  uint8_t id2;           /*  "   "  = zero shows not set has happened            */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} fastClockMsg;

/* Programmer Task Message (used in Start and Final Reply, both )*/
typedef struct progtask_t {
  uint8_t command;
  uint8_t mesg_size;     /* ummmmm, size of the message in bytes?                    */
  uint8_t slot;          /* slot number for this request - slot 124 is programmer    */
  uint8_t pcmd;          /* programmer command                                       */
  uint8_t pstat;         /* programmer status error flags in reply message           */
  uint8_t hopsa;         /* Ops mode - 7 high address bits of loco to program        */
  uint8_t lopsa;         /* Ops mode - 7 low  address bits of loco to program        */
  uint8_t trk;           /* track status. Note: bit 3 shows if prog track is busy    */
  uint8_t cvh;           /* hi 3 bits of CV# and msb of data7                        */
  uint8_t cvl;           /* lo 7 bits of CV#                                         */
  uint8_t data7;         /* 7 bits of data to program, msb is in cvh above           */
  uint8_t pad2;
  uint8_t pad3;
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} progTaskMsg;

/* Set slot sound functions */
typedef struct locosnd_t {
  uint8_t command;
  uint8_t slot;          /* slot number for this request                         */
  uint8_t snd;           /* sound/function request                               */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} locoSndMsg;

/* Set slot direction and F0-F4 functions */
typedef struct locodirf_t {
  uint8_t command;
  uint8_t slot;          /* slot number for this request                         */
  uint8_t dirf;          /* direction & function request                         */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} locoDirfMsg;

/* Set slot speed functions */
typedef struct locospd_t {
  uint8_t command;
  uint8_t slot;          /* slot number for this request                         */
  uint8_t spd;           /* speed request                                        */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} locoSpdMsg;

/* send packet immediate message */
typedef struct sendpkt_t {
  uint8_t command;
  uint8_t mesg_size;     /* ummmmm, size of the message in bytes?                */
  uint8_t val7f;         /* fixed value of 0x7f                                  */
  uint8_t reps;          /* repeat count                                         */
  uint8_t dhi;           /* high bits of data bytes                              */
  uint8_t im1;
  uint8_t im2;
  uint8_t im3;
  uint8_t im4;
  uint8_t im5;
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} sendPktMsg;

/* Long ACK message */
typedef struct longack_t {
  uint8_t command;
  uint8_t opcode;        /* op-code of message getting the response (msb=0)      */
  uint8_t ack1;          /* response code                                        */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} longAckMsg;

/* Write slot status message */
typedef struct slotstat_t {
  uint8_t command;
  uint8_t slot;          /* slot number for this request                         */
  uint8_t stat;          /* status to be written                                 */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} slotStatusMsg;

/* Move/Link Slot Message */
typedef struct slotmove_t {
  uint8_t command;
  uint8_t src;           /* source slot number for the move/link                 */
  uint8_t dest;          /* destination slot for the move/link                   */
  uint8_t chksum;        /* exclusive-or checksum for the message                */
} slotMoveMsg;

typedef struct
{
  uint8_t command;        // OPC_PEER_XFER for replies, OPC_IMM_PACKET for commands
  uint8_t mesg_size;      // 15 bytes
  uint8_t SRC;            // source
  uint8_t DSTL;           // destination, low byte
  uint8_t DSTH;           // destination, high byte
  uint8_t ReqId;          // Request ID, distinguishes commands
  uint8_t PXCT1;          // MSBs of following data
  uint8_t D0;             // Data Bytes
  uint8_t D1;
  uint8_t D2;
  uint8_t D3;
  uint8_t D4;
  uint8_t D5;
  uint8_t D6;
} UhlenbrockMsg;

typedef struct
{
  uint8_t command;        // OPC_SL_RD_UHLI for replies, OPC_WR_SL_UHLI for commands
  uint8_t mesg_size;      // 21 bytes
  uint8_t unk0;
  uint8_t slot;           // slot number
  uint8_t stat;           // slot status
  uint8_t adr;            // loco address
  uint8_t adr2;           // loco address high
  uint8_t trk;            // track status
  uint8_t spd;            // command speed
  uint8_t fhi;            // function high bits: F12,F20,F28
  uint8_t dirf;           // direction and F0-F4 bits
  uint8_t snd2;           // F5..F11
  uint8_t snd3;           // F13..F19
  uint8_t snd4;           // F21..F27
  uint8_t unk1;           // steps???
  uint8_t unk2;
  uint8_t unk3;
  uint8_t unk4;
  uint8_t unk5;
  uint8_t unk6;
} UhliSlotMsg;


typedef union {
  szMsg           sz ;
  swRepMsg        srp ;
  swReqMsg        srq ;
  inputRepMsg     ir ;
  slotReqMsg      sr ;
  rwSlotDataMsg   sd ;
  fastClockMsg    fc ;
  progTaskMsg     pt ;
  locoSndMsg      ls ;
  locoDirfMsg     ldf ;
  locoSpdMsg      lsp ;
  sendPktMsg      sp ;
  longAckMsg      lack ;
  slotStatusMsg   ss ;
  slotMoveMsg     sm ;
  UhlenbrockMsg   ub;
  UhliSlotMsg     usd;
  uint8_t         data[32] ;
} lnMsg ;

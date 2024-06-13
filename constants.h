#ifndef CONSTANTS_H
#define CONSTANTS_H

#define REMOTE_KEY_NUM                (  27 )

typedef struct
{
  uint8_t  yAddress;
  uint8_t  yCode;
  uint8_t  yArrayIdx;
  bool     bCanBeHeld;
  bool     bCanBeLongPressed;
//  uint16_t wHoldTime;          //zB 500ms
  uint8_t  yHeldEventRaster;   //zB 100ms
} REMOTE_KEY_TY;

typedef enum
{
  ST_MENU_OFF,
  ST_CONTRAST,
  ST_BRIGHTNESS,
  ST_HSIZE,
  ST_HPHASE,
  ST_VSIZE,
  ST_VSHIFT,
  ST_VLIN,
  ST_VFREQ,
  ST_PINAMP,
  ST_KEYSTONE
}STATE_TY;

typedef enum
{
  MSG_OFF,
  MSG_MENU,
  MSG_ON
}MSG_TYPE;


// Angepasst an Sony Remote RM-887
const REMOTE_KEY_TY remotePwr     = { 0x01, 0x15,  0, false, true,   0   }; // Power
const REMOTE_KEY_TY remoteOkay    = { 0x01, 0x65,  1, false, true,   0   }; // OK
const REMOTE_KEY_TY remoteMenu    = { 0x01, 0x60,  2, false, false,  0   }; // Menu
const REMOTE_KEY_TY remoteUp      = { 0x01, 0x74,  3, true,  false,  100 }; // Hoch
const REMOTE_KEY_TY remoteDown    = { 0x01, 0x75,  4, true,  false,  100 }; // Runter
const REMOTE_KEY_TY remoteLeft    = { 0x01, 0x34,  5, true,  false,  100 }; // Links
const REMOTE_KEY_TY remoteRight   = { 0x01, 0x33,  6, true,  false,  100 }; // Rechts
const REMOTE_KEY_TY remoteOne     = { 0x01, 0x00,  7, false, false,  0   }; // 1
const REMOTE_KEY_TY remoteTwo     = { 0x01, 0x01,  8, false, false,  0   }; // 2
const REMOTE_KEY_TY remoteThree   = { 0x01, 0x02,  9, false, false,  0   }; // 3
const REMOTE_KEY_TY remoteFour    = { 0x01, 0x03, 10, false, false,  0   }; // 4
const REMOTE_KEY_TY remoteFive    = { 0x01, 0x04, 11, false, false,  0   }; // 5
const REMOTE_KEY_TY remoteSix     = { 0x01, 0x05, 12, false, false,  0   }; // 6
const REMOTE_KEY_TY remoteSeven   = { 0x01, 0x06, 13, false, false,  0   }; // 7
const REMOTE_KEY_TY remoteEight   = { 0x01, 0x07, 14, false, false,  0   }; // 8
const REMOTE_KEY_TY remoteNine    = { 0x01, 0x08, 15, false, false,  0   }; // 9
const REMOTE_KEY_TY remoteZero    = { 0x01, 0x09, 16, false, false,  0   }; // 0
const REMOTE_KEY_TY remoteMute    = { 0x01, 0x14, 17, false, true,   0   }; // Mute
const REMOTE_KEY_TY remoteSource  = { 0x01, 0x25, 18, false, false,  0   }; // Input Switch

const REMOTE_KEY_TY remoteRed     = { 0x03, 0x4C, 19, false, false,  0   }; // Rot
const REMOTE_KEY_TY remoteGreen   = { 0x03, 0x4D, 20, false, false,  0   }; // Grün
const REMOTE_KEY_TY remoteYellow  = { 0x03, 0x4E, 21, false, false,  0   }; // Gelb
const REMOTE_KEY_TY remoteBlue    = { 0x03, 0x4F, 22, false, false,  0   }; // Blau
const REMOTE_KEY_TY remoteProgUp  = { 0x01, 0x10, 23, true,  false,  0   }; // Program Up
const REMOTE_KEY_TY remoteProgDn  = { 0x01, 0x11, 24, true,  false,  0   }; // Program Dn
const REMOTE_KEY_TY remoteVolUp   = { 0x01, 0x12, 25, true,  false,  100 }; // Volume Up
const REMOTE_KEY_TY remoteVolDn   = { 0x01, 0x13, 26, true,  false,  100 }; // Volume Dn

const REMOTE_KEY_TY constants_atKeyArray[ REMOTE_KEY_NUM ] =
{
  remotePwr  ,
  remoteOkay ,
  remoteMenu ,
  remoteUp   ,
  remoteDown ,
  remoteLeft ,
  remoteRight,
  remoteOne  ,
  remoteTwo  ,
  remoteThree,
  remoteFour ,
  remoteFive ,
  remoteSix  ,
  remoteSeven,
  remoteEight,
  remoteNine ,
  remoteZero ,
  remoteMute ,
  remoteSource,
  remoteRed  ,
  remoteGreen,
  remoteYellow,
  remoteBlue ,
  remoteProgUp,
  remoteProgDn,
  remoteVolUp,
  remoteVolDn
};

/*

Ausgelesene Remote Codes der RM-887

P=19 A=0x1 C=0x14     Mute
P=19 A=0x1 C=0x15     Power
P=19 A=0x1 C=0x3A     i+
P=19 A=0x1 C=0x25     Input Switch
P=19 A=0x1 C=0x00     1
P=19 A=0x1 C=0x01     2
P=19 A=0x1 C=0x02     3
P=19 A=0x1 C=0x03     4
P=19 A=0x1 C=0x04     5
P=19 A=0x1 C=0x05     6
P=19 A=0x1 C=0x06     7
P=19 A=0x1 C=0x07     8
P=19 A=0x1 C=0x08     9
P=19 A=0x1 C=0x1D     -/--
P=19 A=0x1 C=0x09     0
P=19 A=0x1 C=0x3B     Drehende Pfeile
P=19 A=0x1 C=0x38     Display
P=19 A=0x1 C=0x3F     Teletext
P=19 A=0x1 C=0x60     MENU
P=19 A=0x1 C=0x34     Links
P=19 A=0x1 C=0x74     Hoch
P=19 A=0x1 C=0x33     Rechts
P=19 A=0x1 C=0x75     Runter
P=19 A=0x1 C=0x65     OK
P=19 A=0x3 C=0x4C     Rot
P=19 A=0x3 C=0x4D     Grün
P=19 A=0x3 C=0x4E     Gelb
P=19 A=0x3 C=0x4F     Blau
P=19 A=0x1 C=0x12     Volume +
P=19 A=0x1 C=0x10     Prog +
P=19 A=0x1 C=0x13     Volume -
P=19 A=0x1 C=0x11     Prog -
*/

#endif

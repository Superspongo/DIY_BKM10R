// ircomm-Modul
// Benötigt die Arduino Library IRRemote (getestet in v3.9.0)

#ifndef IRCOMM_HPP
#define IRCOMM_HPP

#define IRCOMM_PRESS     ( 0x01 )
#define IRCOMM_LONG      ( 0x02 )
#define IRCOMM_HELD      ( 0x04 )
#define IRCOMM_HOLD      ( 0x08 )
#define IRCOMM_LONG_TRIG ( 0x10 )

typedef enum
{
  Power,
  OK,
  Menu,
  Up,
  Down,
  Left,
  Right,
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Zero,
  Mute,
  Source,
  Red,
  Green,
  Yellow,
  Blue,
  ProgramUp,
  ProgramDn,
  VolumeUp,
  VolumeDn
}IRCOMM_BUTTON;

typedef enum
{
  PressEvent,
  LongPressEvent,
  HoldEvent
}IRCOMM_FUNC;

void ircomm_init( void );
void ircomm_exec ( unsigned long lActualTime );
bool ircomm_get_event ( IRCOMM_BUTTON eButton, IRCOMM_FUNC eFunction );
bool ircomm_get_press ( IRCOMM_BUTTON eButton );
bool ircomm_event_flag( void );

#endif

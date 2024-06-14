// Using Libraries:
//
// U8G2
// Rotary Encoder 
// Arduino-Pico
// NeoPixel
// IRRemote       https://github.com/Arduino-IRremote/Arduino-IRremote
// 

#include "defines.h"
#include "display.h"
#include "ircomm.hpp"
#include "bkmcomm.hpp"

#define DEBOUNCE_TIME (   300 )    // in Milliseconds

//------------------
// Global variables
//------------------

bool m_bEncoderButtonEdge  = false;
bool m_bFunctionButtonEdge = false;

unsigned long m_lActualTime;
unsigned long m_lLastEncoderEdge = 0;    // the last time the output pin was toggled

static bool m_bToggleShift = false;

void setup() 
{
  // Serial1 soll auf den Pins GPIO 0 und 1 ausgegeben werden
  // Der Pi Pico kann so konfiguriert werden und der Core
  // unterstützt das.
  
  // Encoder Buttons
  pinMode( PIN_ENCODER_BUTTON,  INPUT_PULLUP );  // Drehencoder Eindrücken
  pinMode( PIN_FUNCTION_BUTTON, INPUT_PULLUP );  // "Reset" Taster

  // Debug USB Seriell Kommunikation  
  Serial.begin( 115200 );

  // Display Konfiguration
  display_init();

  // Remote control
  ircomm_init();

  // monitor communication
  bkmcomm_init();
}

// Invertierendes Lesen, alle Eingänge schalten auf Low
static bool ReadSe( byte yPinNumber )
{
  bool bRetVal = !digitalRead( yPinNumber );

  return bRetVal;
}

static void ReadInputs()
{
  static bool bFunctionButtonBuff;
  static bool bEncoderButtonBuff;

  static int iPositionBuff = 0;
  bool bFunctionButton = ReadSe( PIN_FUNCTION_BUTTON );
  bool bEncoderButton  = ReadSe( PIN_ENCODER_BUTTON  );
  
  if ( ( m_lActualTime - m_lLastEncoderEdge ) > DEBOUNCE_TIME )
  {
    if ( !bEncoderButtonBuff && bEncoderButton )
    {
      m_bEncoderButtonEdge = true;
      m_lLastEncoderEdge   = m_lActualTime;
    }
  } 
  else
  {
    m_bEncoderButtonEdge = false;
  }

  // Function Button
  if ( !bFunctionButtonBuff && bFunctionButton )
  {
    m_bFunctionButtonEdge = true;
  }
  else
  {
    m_bFunctionButtonEdge = false;
  } 

  // Buffer zur Flankenerkennung
  bFunctionButtonBuff = bFunctionButton;
  bEncoderButtonBuff  = bEncoderButton;
}

void loop() 
{
  // Zeitstempel, Senderaster
  m_lActualTime = millis();
  
  ReadInputs();
  
  ircomm_exec ( m_lActualTime );
  
  bkmcomm_exec();
  
  display_exec ( ircomm_get_event( Left,   PressEvent ), 
                 ircomm_get_event( Right,  PressEvent ), 
                 m_bEncoderButtonEdge // Switch active Pot command
               );
  
  if ( ircomm_get_event( Source, PressEvent ) || ircomm_get_event( Green, PressEvent ) )display_set_brightness( true, false );

  // Testen der Shift Toggle Funktion
  if ( ircomm_get_event( Mute, PressEvent ) )
  {
    m_bToggleShift = !m_bToggleShift;
    display_set_function_button( IDX_SHIFT, m_bToggleShift );
  }

  if ( ircomm_get_event( Menu, PressEvent     ) )Serial.println( "Menu Press received" );
  if ( ircomm_get_event( Menu, LongPressEvent ) )Serial.println( "Menu Hold  received" );
}

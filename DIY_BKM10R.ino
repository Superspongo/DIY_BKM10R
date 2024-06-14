// Using Libraries:
//
// U8G2
// Rotary Encoder 
// Arduino-Pico
// NeoPixel
// IRRemote       https://github.com/Arduino-IRremote/Arduino-IRremote
// 

#include "defines.h"
#include "RotaryEncoder.h"
#include "display.h"
#include "ircomm.hpp"

#define DEBOUNCE_TIME (   300 )    // in Milliseconds

//------------------
// Global variables
//------------------

bool m_bEncoderButtonEdge  = false;
bool m_bFunctionButtonEdge = false;

unsigned long m_lActualTime;
unsigned long m_lLastEncoderEdge = 0;    // the last time the output pin was toggled

static bool m_bToggleShift = false;

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder( PIN_ENCODER_2, PIN_ENCODER_1, RotaryEncoder::LatchMode::TWO03 );

void setup() 
{
  // Serial1 soll auf den Pins GPIO 0 und 1 ausgegeben werden
  // Der Pi Pico kann so konfiguriert werden und der Core
  // unterstützt das.
  
  // Encoder Buttons
  pinMode( PIN_ENCODER_1, INPUT_PULLUP );        // Encoder Eingang 1
  pinMode( PIN_ENCODER_2, INPUT_PULLUP );        // Encoder Eingang 2
  pinMode( PIN_ENCODER_BUTTON,  INPUT_PULLUP );  // Drehencoder Eindrücken
  pinMode( PIN_FUNCTION_BUTTON, INPUT_PULLUP );  // "Reset" Taster

  // RS422 Kommunikation zum Monitor
  // Alternative Pins benutzen
  Serial1.setTX( PIN_RS485_TX );
  Serial1.setRX( PIN_RS485_RX );
  Serial1.begin( BAUDRATE_RS422_38K4 );   

  // Debug USB Seriell Kommunikation  
  Serial.begin( 115200 );

  // Display Konfiguration
  display_init();

  // Remote control
  ircomm_init();
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

  encoder.tick();
  int iNewPosition = encoder.getPosition();
  
  if ( iPositionBuff != iNewPosition ) 
  {
    int iDirection = (int)encoder.getDirection();
    
    Serial.print("pos:");
    Serial.print(iNewPosition);
    Serial.print(" dir:");
    Serial.println( iDirection );
    iPositionBuff = iNewPosition;

    if ( 1 == iDirection ) display_set_contrast( true,  false );
    else                   display_set_contrast( false, true  );
  } // if

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

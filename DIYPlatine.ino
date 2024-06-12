// Using Libraries:
//
// U8G2
// Rotary Encoder
// 

#include "defines.h"
#include "RotaryEncoder.h"
#include "display.h"


//------------------
// Global variables
//------------------

bool m_bEncoderButtonEdge  = false;
bool m_bFunctionButtonEdge = false;

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder( PIN_ENCODER_2, PIN_ENCODER_1, RotaryEncoder::LatchMode::TWO03 );

void setup() 
{
  // Serial1 soll auf den Pins GPIO 0 und 1 ausgegeben werden
  // Der Pi Pico kann so konfiguriert werden und der Core
  // unterstützt das.
  
  // RS422 Kommunikation zum Monitor
  // Alternative Pins benutzen
  Serial1.setTX( PIN_RS485_TX );
  Serial1.setRX( PIN_RS485_RX );
  Serial1.begin( BAUDRATE_RS422_38K4 );   

  // Debug USB Seriell Kommunikation  
  Serial.begin( 115200 );

  // Display Konfiguration
  display_init();
  
  // Encoder Buttons
  pinMode( PIN_ENCODER_1, INPUT_PULLUP );        // Encoder Eingang 1
  pinMode( PIN_ENCODER_2, INPUT_PULLUP );        // Encoder Eingang 2
  pinMode( PIN_ENCODER_BUTTON,  INPUT_PULLUP );  // Drehencoder Eindrücken
  pinMode( PIN_FUNCTION_BUTTON, INPUT_PULLUP );  // "Reset" Taster
}

// Invertierendes Lesen, alle Eingänge schalten auf Low
static bool ReadSe( byte yPinNumber )
{
  bool bRetVal = !digitalRead( yPinNumber );

  return bRetVal;
}

static void ReadInputs( void )
{
  static bool bEncoderButtonBuff;
  static bool bFunctionButtonBuff;

  static int pos = 0;
  bool bEncoderButton  = ReadSe( PIN_ENCODER_BUTTON  );
  bool bFunctionButton = ReadSe( PIN_FUNCTION_BUTTON );
  
  if ( !bEncoderButtonBuff  && bEncoderButton  ) m_bEncoderButtonEdge  = true;
  else                                           m_bEncoderButtonEdge  = false;
  if ( !bFunctionButtonBuff && bFunctionButton ) m_bFunctionButtonEdge = true;
  else                                           m_bFunctionButtonEdge = false;

  encoder.tick();
  int newPos = encoder.getPosition();
  
  if (pos != newPos) 
  {
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println((int)(encoder.getDirection()));
    pos = newPos;
  } // if

  // Buffer zur Flankenerkennung
  bEncoderButtonBuff  = bEncoderButton;
  bFunctionButtonBuff = bFunctionButton;
}

void loop() 
{
  ReadInputs();
  
  display_loop( m_bEncoderButtonEdge, m_bFunctionButtonEdge, false, false );

}

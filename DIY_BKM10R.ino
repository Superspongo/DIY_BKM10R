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

bool g_bEncoderButtonEdge  = false;
bool g_bFunctionButtonEdge = false;

bool g_bToggleEncoderLight = false;

unsigned long g_lActualTime;
unsigned long g_lLastEncoderEdge = 0;    // the last time the output pin was toggled

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
  
  if ( ( g_lActualTime - g_lLastEncoderEdge ) > DEBOUNCE_TIME )
  {
    if ( !bEncoderButtonBuff && bEncoderButton )
    {
      g_bEncoderButtonEdge = true;
      g_lLastEncoderEdge   = g_lActualTime;
    }
  } 
  else
  {
    g_bEncoderButtonEdge = false;
  }

  // Function Button
  if ( !bFunctionButtonBuff && bFunctionButton )
  {
    g_bFunctionButtonEdge = true;
    g_bToggleEncoderLight = !g_bToggleEncoderLight;

    Serial.println( "Toggling backlight" );
    Serial.println( "Toggling backlight" );
    display_encoder_backlight( g_bToggleEncoderLight );
  }
  else
  {
    g_bFunctionButtonEdge = false;
  } 

  // Buffer zur Flankenerkennung
  bFunctionButtonBuff = bFunctionButton;
  bEncoderButtonBuff  = bEncoderButton;
}

void loop() 
{
  // Zeitstempel, Senderaster
  g_lActualTime = millis();
  
  ReadInputs();
  
  ircomm_exec ( g_lActualTime );
  
  bkmcomm_exec();
  
  display_exec ( ircomm_get_event( Left,   PressEvent ), 
                 ircomm_get_event( Right,  PressEvent ), 
                 g_bEncoderButtonEdge // Switch active Pot command
               );
  
  if ( ircomm_get_event( Source, PressEvent ) || ircomm_get_event( Green, PressEvent ) )display_set_brightness( true, false );

  if ( ircomm_get_event( Menu, PressEvent     ) )Serial.println( "Menu Press received" );
  if ( ircomm_get_event( Menu, LongPressEvent ) )Serial.println( "Menu Hold  received" );
}

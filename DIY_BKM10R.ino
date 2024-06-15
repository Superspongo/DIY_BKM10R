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

bool g_bToggleEncoderLight = true;

unsigned long g_lActualTime;
unsigned long g_lLastEncoderEdge  = 0;    // the last time the output pin was toggled
unsigned long g_lLastFunctionEdge = 0;    // the last time the output pin was toggled

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

static void debouncedEdge( unsigned long *plLastEdge, bool *pbBuffer, bool *pbIn, bool *bEdge )
{
  if ( ( g_lActualTime - *plLastEdge ) > DEBOUNCE_TIME )
  {
    if ( !*pbBuffer && *pbIn )
    {
      *bEdge = true;
      *plLastEdge   = g_lActualTime;
    }
  } 
  else
  {
    *bEdge = false;
  }
}

static void ReadInputs()
{
  static bool bFunctionButtonBuff;
  static bool bEncoderButtonBuff;

  static int iPositionBuff = 0;
  bool bFunctionButton = ReadSe( PIN_FUNCTION_BUTTON );
  bool bEncoderButton  = ReadSe( PIN_ENCODER_BUTTON  );
  
  debouncedEdge( &g_lLastEncoderEdge,  &bEncoderButtonBuff,  &bEncoderButton,  &g_bEncoderButtonEdge  );
  debouncedEdge( &g_lLastFunctionEdge, &bFunctionButtonBuff, &bFunctionButton, &g_bFunctionButtonEdge );

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
  
  // Function Button toggles encoder backlight
  if ( g_bFunctionButtonEdge )
  {
    g_bToggleEncoderLight = !g_bToggleEncoderLight;

    Serial.println( "Toggling backlight" );
    display_encoder_backlight( g_bToggleEncoderLight );
  }
  
  if ( ircomm_get_event( Source, PressEvent ) || ircomm_get_event( Green, PressEvent ) )display_set_brightness( true, false );
}

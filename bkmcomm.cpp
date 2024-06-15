#include <Arduino.h>
#include "defines.h"
#include "ButtonDefs.h"
#include "ircomm.hpp"
#include "display.h"
#include "RotaryEncoder.h"

enum CurrentBank {
  CB_INIT,
  CB_SWITCHES,
  CB_LEDS,
  CB_ENCODERS,
  CB_CC,
  CB_MT,
};

#define ENCODER_UPDATE_DELTA_MS (100)

#define LED_ADDR_SAFE_AREA  (0x0001)
#define LED_F2_F4           (0x0002)
#define LED_CONTRAST_MANUAL (0x0004)
#define LED_BRIGHT_MANUAL   (0x0008)
#define LED_CHROMA_MANUAL   (0x0010)
#define LED_PHASE_MANUAL    (0x0020)
#define GPIO_U10_UNUSED_1   (0x0040)
#define GPIO_U10_UNUSED_2   (0x0080)
#define LED_SHIFT           (0x0100)
#define LED_UNDERSCAN_16_9  (0x0200)
#define LED_HORIZ_SYNC      (0x0400)
#define LED_VERT_BLUE_ONLY  (0x0800)
#define LED_MONO_R          (0x1000)
#define LED_APT_G           (0x2000)
#define LED_COMB_B          (0x4000)
#define LED_F1_F3           (0x8000)

CurrentBank currentBank = CB_INIT;

const uint8_t KEY             = 0x44;
const uint8_t BANK            = 0x49;

const uint8_t BANK_ENCODER[]  = { 0x45, 0x4E }; // IEN - Encoders
const uint8_t BANK_SWITCH[]   = { 0x53, 0x57 }; // ISW - Switches
const uint8_t BANK_LED[]      = { 0x4C, 0x45 }; // ILE - LEDS
const uint8_t BANK_CC[]       = { 0x43, 0x43 }; // ICC - Unknown
const uint8_t BANK_MT[]       = { 0x4D, 0x54 }; // IMT - Unknown

const uint8_t ENCODER_PHASE     = 0x00;
const uint8_t ENCODER_CHROMA    = 0x01;
const uint8_t ENCODER_BRIGHT    = 0x02;
const uint8_t ENCODER_CONTRAST  = 0x03;

const uint8_t POWER_ON[] = {KEY,0x33,0x31};

int ledState = 0;
int prev_ledState = ledState;

// LED States
// See protocol description to learn about key groups
uint8_t group2leds = 0x0;
uint8_t group3leds = 0x0;
uint8_t group4leds = 0x0;

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder( PIN_ENCODER_2, PIN_ENCODER_1, RotaryEncoder::LatchMode::TWO03 );

void serialEvent() 
{
  uint8_t buf[3];
  
  if(Serial1.available()) 
  {
    int received = Serial1.readBytes(buf,3);
    if(received != 3) return;    // Invalid command length
    
    switch( buf[0] ) 
    {
      case KEY: 
      {
        // == LEDs ==
        // 
        // LEDs use the same groups and masks as keypresses. Every time the CRT needs to
        // change the lights, it changes to the LED bank:
        // 
        // 0x49 0x4c 0x45
        // 
        // then sends changed switch groups:
        // 
        // 0x44 <group> <mask>
        // 
        // where <mask> is all of the active LED masks OR'd together.
        uint8_t* leds = NULL;
        switch(buf[1]) 
        {
          case 0x02:
            leds = &group2leds;
          break;
          case 0x03:
            leds = &group3leds;
          break;
          case 0x04:
            leds = &group4leds;
          break;
          default:
          break;
        }
        // copy LED states reported by the monitor
        // to the appropriate global variable
        if(leds != NULL) *leds = buf[2];
      }
      break;
      case BANK: 
      {
        // Monitor forces a bank switch
        if(buf[1] == BANK_LED[0] &&
           buf[2] == BANK_LED[1])
        {
          currentBank = CB_LEDS;
        } else
        if(buf[1] == BANK_CC[0] &&
           buf[2] == BANK_CC[1])
        {
          currentBank = CB_CC;
          Serial1.write(BANK);
          Serial1.write(BANK_CC,2);
        } else
        if(buf[1] == BANK_MT[0] &&
           buf[2] == BANK_MT[1])
        {
          currentBank = CB_MT;
        }
      }
      break;
      default:
      break;
    }
  }
}

void wakeup_monitor() {
  Serial1.write(BANK);
  Serial1.write(BANK_CC,2);
  Serial1.write(BANK);
  Serial1.write(BANK_MT,2);
  Serial1.write(POWER_ON,3);  
}

struct Encoder {
  int8_t m_value;
  uint8_t m_id;
};

#define ENCODER_COUNT (4)

Encoder encoders[ENCODER_COUNT] = {
  { .m_value = 0, .m_id = ENCODER_PHASE    },
  { .m_value = 0, .m_id = ENCODER_CHROMA   },
  { .m_value = 0, .m_id = ENCODER_BRIGHT   },
  { .m_value = 0, .m_id = ENCODER_CONTRAST }
};

unsigned long nextEncoderCheck_ms = 0;

void reportEncoder(uint8_t encoder, int8_t value) 
{
  // Martin tried out different values here
  // If the value of "encoder tick deltas" between updates is greater than |3|
  // so if the encoder has been turned fast), a value of |127| is transmitted
  // If it was smaller, |1| is transmitted.
  int8_t ticks = 0;
    
  if( value < 0 ) 
  {
    if( value >= -3 )              ticks = -1;     // small step
    else if (    ( value <  -3 ) 
              && ( value >= -6 ) ) ticks = -50;    // medium step
    else                           ticks = -127;   // big step
  } 
  else 
  {
    if( value <= 3 )               ticks = 1;      // small step
    else if (    ( value >   3 ) 
              && ( value <=  6 ) ) ticks = 50;     // medium step
    else                           ticks = 127;    // big step
  }

  // ## DEBUG
  // Serial.print( "Reported value to monitor: ");
  // Serial.println( ticks );
  
  if(currentBank != CB_ENCODERS) 
  {
    Serial1.write(BANK);
    Serial1.write(BANK_ENCODER,2);
    currentBank = CB_ENCODERS;
  }

  Serial1.write(KEY);
  Serial1.write(encoder);
  Serial1.write(ticks);   
}

void checkEncoders() 
{
  for(int j = 0; j < ENCODER_COUNT; ++j) 
  {
    Encoder& e = encoders[j];
    if(e.m_value != 0) 
    {
      reportEncoder(e.m_id,e.m_value);
      e.m_value = 0;
    }
  }

  // reset Encoder position
  encoder.setPosition(0);
}

void bkmcomm_init() {
  // RS422 Kommunikation zum Monitor
  // Alternative Pins benutzen
  Serial1.setTX( PIN_RS485_TX );
  Serial1.setRX( PIN_RS485_RX );
  Serial1.begin( BAUDRATE_RS422_38K4 );   

  delay(100);
 
  pinMode( PIN_ENCODER_1, INPUT_PULLUP );        // Encoder Eingang 1
  pinMode( PIN_ENCODER_2, INPUT_PULLUP );        // Encoder Eingang 2

  // "Flush" any garbage
  while (Serial1.available()) Serial1.read();
  
  wakeup_monitor();
}

void pushButton(Button_t& b) {
  if(currentBank != CB_SWITCHES) {
    Serial1.write(BANK);
    Serial1.write(BANK_SWITCH,2);
    currentBank = CB_SWITCHES;
  }
  Serial1.write(KEY);
  Serial1.write(b.group);
  Serial1.write(b.id);          
};

void updateLEDStates() 
{
  if(group2leds & B_PHASE_MANUAL.id) {
    ledState |= LED_PHASE_MANUAL;
    display_set_function_button( IDX_PHASE, true );
  } else {
    ledState &= ~LED_PHASE_MANUAL;
    display_set_function_button( IDX_PHASE, false );
  }

  if(group2leds & B_CHROMA_MANUAL.id) {
    ledState |= LED_CHROMA_MANUAL;
    display_set_function_button( IDX_CHROMA, true );
  } else {
    ledState &= ~LED_CHROMA_MANUAL;
    display_set_function_button( IDX_CHROMA, false );
  }

  if(group2leds & B_BRIGHT_MANUAL.id) {
    ledState |= LED_BRIGHT_MANUAL;
    display_set_function_button( IDX_BRIGHTNESS, true );
  } else {
    ledState &= ~LED_BRIGHT_MANUAL;
    display_set_function_button( IDX_BRIGHTNESS, false );
  }

  if(group2leds & B_CONTRAST_MANUAL.id) {
    ledState |= LED_CONTRAST_MANUAL;
    display_set_function_button( IDX_CONTRAST, true );
  } else {
    ledState &= ~LED_CONTRAST_MANUAL;
    display_set_function_button( IDX_CONTRAST, false );
  }

  if(group3leds & B_SHIFT.id) {
    ledState |= LED_SHIFT;
    display_set_function_button( IDX_SHIFT, true );
  } else {
    ledState &= ~LED_SHIFT;
    display_set_function_button( IDX_SHIFT, false );
  }

  if(group3leds & B_UNDERSCAN_16_9.id) {
    ledState |= LED_UNDERSCAN_16_9;
    display_set_function_button( IDX_UNDERSCAN, true );
    display_set_function_button( IDX_16BY9,     true );
  } else {
    ledState &= ~LED_UNDERSCAN_16_9;
    display_set_function_button( IDX_UNDERSCAN, false );
    display_set_function_button( IDX_16BY9,     false );
  }

  if(group3leds & B_HORIZ_SYNC.id) {
    ledState |= LED_HORIZ_SYNC;
    display_set_function_button( IDX_HDELAY, true );
    display_set_function_button( IDX_SYNC,   true );
  } else {
    ledState &= ~LED_HORIZ_SYNC;
    display_set_function_button( IDX_HDELAY, false );
    display_set_function_button( IDX_SYNC,   false );    
  }

  if(group3leds & B_VERT_BLUE_ONLY.id) {
    ledState |= LED_VERT_BLUE_ONLY;
    display_set_function_button( IDX_VDELAY,   true );
    display_set_function_button( IDX_BLUEONLY, true );
  } else {
    ledState &= ~LED_VERT_BLUE_ONLY;
    display_set_function_button( IDX_VDELAY,   false );
    display_set_function_button( IDX_BLUEONLY, false );    
  }

  if(group3leds & B_MONO_R.id) {
    ledState |= LED_MONO_R;
    display_set_function_button( IDX_MONO, true );
    display_set_function_button( IDX_RED,  true );
  } else {
    ledState &= ~LED_MONO_R;
    display_set_function_button( IDX_MONO, false );
    display_set_function_button( IDX_RED,  false );
  }

  if(group4leds & B_APT_G.id) {
    ledState |= LED_APT_G;
    display_set_function_button( IDX_APT,   true );
    display_set_function_button( IDX_GREEN, true );
  } else {
    ledState &= ~LED_APT_G;
    display_set_function_button( IDX_APT,   false );
    display_set_function_button( IDX_GREEN, false );    
  }

  if(group4leds & B_COMB_B.id) {
    ledState |= LED_COMB_B;
    display_set_function_button( IDX_COMB, true );
    display_set_function_button( IDX_BLUE, true );    
  } else {
    ledState &= ~LED_COMB_B;
    display_set_function_button( IDX_COMB, false );
    display_set_function_button( IDX_BLUE, false );        
  }

  if(group4leds & B_F1_F3.id) {
    ledState |= LED_F1_F3;
    display_set_function_button( IDX_F1, true );
    display_set_function_button( IDX_F3, true );        
  } else {
    ledState &= ~LED_F1_F3;
    display_set_function_button( IDX_F1, false );
    display_set_function_button( IDX_F3, false );    
  }

  if(group4leds & B_F2_F4.id) {
    ledState |= LED_F2_F4;
    display_set_function_button( IDX_F2, true );
    display_set_function_button( IDX_F4, true );            
  } else {
    ledState &= ~LED_F2_F4;
    display_set_function_button( IDX_F2, false );
    display_set_function_button( IDX_F4, false );        
  }

  if(group4leds & B_SAFE_AREA_ADDR.id) {
    ledState |= LED_ADDR_SAFE_AREA;
    display_set_function_button( IDX_ADDRESS,  true );
    display_set_function_button( IDX_SAFEAREA, true );        
  } else {
    ledState &= ~LED_ADDR_SAFE_AREA;
    display_set_function_button( IDX_ADDRESS,  false );
    display_set_function_button( IDX_SAFEAREA, false );            
  }
}

void bkmcomm_exec( void ) {
  // Check inputs
  byte yActiveEncoder = display_read_active_encoder();
  byte yCursor  = display_read_highlighted_function();
  bool bClicked = ircomm_get_press( OK );

  bool bDown    = ircomm_get_press( Down   );
  bool bEnter   = ircomm_get_press( Yellow );
  bool bUp      = ircomm_get_press( Up     );
  bool bMenu    = ircomm_get_press( Menu   );
  bool bPower   = ircomm_get_press( Power  );
  
  // Display-highlightable buttons
  if ( bClicked )
  {
    // We have a button
  // if( pressedU11(DEGAUSS))         pushButton(B_DEGAUSS);          else
	  if( IDX_CONTRAST   == yCursor ) pushButton(B_CONTRAST_MANUAL);  else
    if( IDX_BRIGHTNESS == yCursor ) pushButton(B_BRIGHT_MANUAL);    else
    if( IDX_CHROMA     == yCursor ) pushButton(B_CHROMA_MANUAL);    else
    if( IDX_PHASE      == yCursor ) pushButton(B_PHASE_MANUAL);     else
    if( IDX_ADDRESS    == yCursor ) pushButton(B_SAFE_AREA_ADDR);   else
    if( IDX_SAFEAREA   == yCursor ) pushButton(B_SAFE_AREA_ADDR);   else
    if( IDX_F1         == yCursor ) pushButton(B_F1_F3);            else
    if( IDX_F3         == yCursor ) pushButton(B_F1_F3);            else
    if( IDX_F2         == yCursor ) pushButton(B_F2_F4);            else
    if( IDX_F4         == yCursor ) pushButton(B_F2_F4);            else
    if( IDX_COMB       == yCursor ) pushButton(B_COMB_B);           else
    if( IDX_BLUE       == yCursor ) pushButton(B_COMB_B);           else
    if( IDX_APT        == yCursor ) pushButton(B_APT_G);            else
    if( IDX_GREEN      == yCursor ) pushButton(B_APT_G);            else
    if( IDX_MONO       == yCursor ) pushButton(B_MONO_R);           else
    if( IDX_RED        == yCursor ) pushButton(B_MONO_R);           else
    if( IDX_SHIFT      == yCursor ) pushButton(B_SHIFT);            else 
    if( IDX_UNDERSCAN  == yCursor ) pushButton(B_UNDERSCAN_16_9);   else
	  if( IDX_16BY9      == yCursor ) pushButton(B_UNDERSCAN_16_9);   else
    if( IDX_HDELAY     == yCursor ) pushButton(B_HORIZ_SYNC);       else
	  if( IDX_SYNC       == yCursor ) pushButton(B_HORIZ_SYNC);       else
    if( IDX_VDELAY     == yCursor ) pushButton(B_VERT_BLUE_ONLY);   else
	  if( IDX_BLUEONLY   == yCursor ) pushButton(B_VERT_BLUE_ONLY);  

   // if(pressedU12(KEY_DEL))         pushButton(B_KEY_DEL);          else
   // if(pressedU12(KEY_ENTER))       pushButton(B_KEY_ENT);
  }
  else
  {
    // Remote control buttons
    if( bDown  )                    pushButton(B_DOWN);             else
    if( bEnter )                    pushButton(B_ENTER);            else
    if( bUp    )                    pushButton(B_UP);               else
    if( bMenu  )                    pushButton(B_MENU);             else
    if( bPower )                    pushButton(B_POWER);            else 
    if(ircomm_get_press( Zero ) )   pushButton(B_KEY_0);            else
    if(ircomm_get_press( One  ) )   pushButton(B_KEY_1);            else
    if(ircomm_get_press( Two  ) )   pushButton(B_KEY_2);            else
    if(ircomm_get_press( Three) )   pushButton(B_KEY_3);            else
    if(ircomm_get_press( Four ) )   pushButton(B_KEY_4);            else
    if(ircomm_get_press( Five ) )   pushButton(B_KEY_5);            else
    if(ircomm_get_press( Six  ) )   pushButton(B_KEY_6);            else
    if(ircomm_get_press( Seven) )   pushButton(B_KEY_7);            else
    if(ircomm_get_press( Eight) )   pushButton(B_KEY_8);            else
    if(ircomm_get_press( Nine ) )   pushButton(B_KEY_9);            
  }

  updateLEDStates();

  // Encoder updates
  encoder.tick();

  // Get encoder index to "bkmcomm index"
  yActiveEncoder -= IDX_PHASE;
  encoders[ yActiveEncoder ].m_value = encoder.getPosition();

  if(millis() >= nextEncoderCheck_ms) {
    checkEncoders();
    nextEncoderCheck_ms += ENCODER_UPDATE_DELTA_MS;
  }

  
  // Implementing a Menu Mode where OK Button is disabled for pressing a button
  if ( ircomm_get_event( Menu, PressEvent     ) )Serial.println( "Menu Press received" );
  if ( ircomm_get_event( Menu, LongPressEvent ) )Serial.println( "Menu Hold  received" );
}

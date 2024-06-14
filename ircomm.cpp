#define DECODE_SONY
#define EXCLUDE_EXOTIC_PROTOCOLS
#define EXCLUDE_UNIVERSAL_PROTOCOLS

#include <Arduino.h>
#include <IRremote.hpp>
#include "ircomm.hpp"
#include "pindefsandmore.h"
#include "constants.h"

#define IR_BUTTON_TIMEOUT   (  100 )
#define IR_HOLD_TIME        (  500 )

#define IR_BUTTON_NUM REMOTE_KEY_NUM

unsigned long lTimeLastReceived;

uint8_t  ayButtonEvents[ IR_BUTTON_NUM ];
uint16_t wFirstReceived;
uint16_t wLastHoldEvent;

void ircomm_init( void )
{
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN );
}

//----------------------  

static void ResetLocalRuntimeArray ( void )
{
  uint8_t yIndex;

  for ( yIndex = 0; yIndex < IR_BUTTON_NUM; yIndex++ )
  {
    ayButtonEvents[ yIndex ] = 0;
    wFirstReceived = 0;
    wLastHoldEvent = 0;
  }
}

static boolean GetKeyStruct ( uint8_t yAddress, uint8_t yCommand, REMOTE_KEY_TY *ptReturnStruct )
{
  boolean bSuccess = false;
  uint8_t yIndex;

  for ( yIndex = 0; ( ( yIndex < IR_BUTTON_NUM ) && !bSuccess ); yIndex++ )
  {
    if (     ( constants_atKeyArray[ yIndex ].yAddress == yAddress ) // @suppress("Field cannot be resolved")
          && ( constants_atKeyArray[ yIndex ].yCode    == yCommand ) ) // @suppress("Field cannot be resolved")
    {
      bSuccess = true;
      *ptReturnStruct = constants_atKeyArray[ yIndex ];
    }
  }

  return bSuccess;
}

static void handleReceivedIRData( uint8_t yAddress, uint8_t yCommand, unsigned long lActualTime ) 
{
  REMOTE_KEY_TY tKeyStruct;
  uint16_t wActualTime = (uint16_t)( lActualTime & 0x0000FFFF );
   
  if ( GetKeyStruct( yAddress, yCommand, &tKeyStruct  ) )
  {
    uint8_t yArrayIndex = tKeyStruct.yArrayIdx;
    
    // Match gefunden, gültiger ArrayIndex raus gesucht
    if ( yArrayIndex < IR_BUTTON_NUM )
    {
      // Das Press-Event auslösen
      if ( ( ayButtonEvents[ yArrayIndex ] & IRCOMM_HELD ) != IRCOMM_HELD )
      {
        // Knopf wird noch nicht gehalten --> Beide Flags setzen
        ayButtonEvents[ yArrayIndex ] |= ( IRCOMM_HELD | IRCOMM_PRESS );
      }
      
      if ( 0 == wFirstReceived )
      {
        wFirstReceived = (uint16_t)( lActualTime & 0x0000FFFF);
      }
    
      if ( ( ayButtonEvents[ yArrayIndex ] & IRCOMM_HELD ) == IRCOMM_HELD )
      {
        // Long-Press Event Handling
        if ( tKeyStruct.bCanBeLongPressed )
        {
          // Darf ein Long Press Event auslösen 
          if ( ( wActualTime - wFirstReceived ) >= IR_HOLD_TIME )
          {
            if ( ( ayButtonEvents[ yArrayIndex ] & IRCOMM_LONG_TRIG ) != IRCOMM_LONG_TRIG )
            {
              // Noch nicht getriggert --> Flags setztn
              ayButtonEvents[ yArrayIndex ] |= ( IRCOMM_LONG | IRCOMM_LONG_TRIG );
            }
          }
        } // end if ( tKeyStruct.bCanBeLongPressed )

        // Halte-Event
        if ( tKeyStruct.bCanBeHeld )
        {
          if ( ( wActualTime - wFirstReceived ) >= IR_HOLD_TIME )
          {
            // Haltezeit ist vorbei, groß genug. Jetzt muss aufs Raster geschaut werden
            if ( 0 == wLastHoldEvent )
            {
              wLastHoldEvent = wActualTime;
              ayButtonEvents[ yArrayIndex ] |= IRCOMM_HOLD;

            }
            else
            {
              if ( ( wActualTime - wLastHoldEvent ) >= tKeyStruct.yHeldEventRaster )
              {
                wLastHoldEvent = wActualTime;
                ayButtonEvents[ yArrayIndex ] |= IRCOMM_HOLD;              
              }
              else
              {
                ayButtonEvents[ yArrayIndex ] &= ~IRCOMM_HOLD;
              }
            }
          }
        } // end if ( tKeyStruct.bCanBeHeld )
      } // end if ( ayButtonEvents[ yArrayIndex ] & IRCOMM_HELD ) == IRCOMM_HELD )
    } // end if ( yArrayIndex < IR_BUTTON_NUM )
  } // end if ( GetKeyStruct( yAddress, yCommand, &tKeyStruct  ) )

}


void ircomm_exec( unsigned long lActualTime )
{
  uint8_t yAddress;
  uint8_t yCommand;
  bool    bHandleData = false;
    
  if ( IrReceiver.decode() ) 
  {
    if ( IrReceiver.decodedIRData.protocol != UNKNOWN ) 
    { 
      bHandleData = true;
      //Serial.println( "Isch hab da was!" );  schneller debug

      lTimeLastReceived = lActualTime;
      // Wenn der Knopf gedrückt gehalten wird, dann bekommt der Receiver ca. alle 50ms einen neuen Wert
          
      yAddress = IrReceiver.decodedIRData.address;
      yCommand = IrReceiver.decodedIRData.command;
    }
        
    /*
     * !!!Important!!! Enable receiving of the next value,
     * since receiving has stopped after the end of the current received data packet.
     */
    IrReceiver.resume(); // Enable receiving of the next value 

    if ( bHandleData )
    {
      handleReceivedIRData( yAddress, yCommand, lActualTime );
    }
  }
  else
  {
    if ( lActualTime - lTimeLastReceived >= IR_BUTTON_TIMEOUT )
    {
      // Reset all keys
      ResetLocalRuntimeArray();
    }
  }    
}


bool ircomm_get_event ( IRCOMM_BUTTON eButton, IRCOMM_FUNC eFunction )
{
  bool bRetval = false;
  
  switch ( eFunction )
  {
    case PressEvent:
      bRetval = ( ( ayButtonEvents[ (uint8_t)eButton ] & IRCOMM_PRESS ) == IRCOMM_PRESS );
      ayButtonEvents[ (uint8_t)eButton ] &= ~IRCOMM_PRESS;
    break;
  
    case LongPressEvent:
      bRetval = ( ( ayButtonEvents[ (uint8_t)eButton ] & IRCOMM_LONG ) == IRCOMM_LONG );
      ayButtonEvents[ (uint8_t)eButton ] &= ~IRCOMM_LONG;
    break;
  
    case HoldEvent:
      bRetval = ( ( ayButtonEvents[ (uint8_t)eButton ] & IRCOMM_HOLD ) == IRCOMM_HOLD );
      ayButtonEvents[ (uint8_t)eButton ] &= ~IRCOMM_HOLD;
    break;
  
    default:
    break;
  }

  return bRetval;
}

bool ircomm_get_press ( IRCOMM_BUTTON eButton )
{
  return ircomm_get_event( eButton, PressEvent );
}

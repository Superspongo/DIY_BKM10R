#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>
#include "graphics.h"
#include "defines.h"
#include "display.h"

#define LED_BACKGROUND  (  0 )
#define LED_ENCODER_1   (  1 )
#define LED_ENCODER_2   (  2 )

// Hardware Solution. MUST SET SPI PINS!! See setup
 U8G2_ST7567_OS12864_F_4W_HW_SPI display(U8G2_R2, /* cs=*/PIN_LCD_EN, 
                                                  /* dc=*/PIN_LCD_RS, 
                                               /* reset=*/PIN_LCD_RESET ); 

// SOFTWARE SOLUTION WORKS!
//U8G2_ST7567_OS12864_F_4W_SW_SPI display (U8G2_R2 , PIN_LCD_SPI_SCK, PIN_LCD_SPI_MOSI, PIN_LCD_EN, PIN_LCD_RS, PIN_LCD_RESET);

// Hintergrundbeleuchtung
Adafruit_NeoPixel pixels(3, PIN_LCD_RGB, NEO_GRB + NEO_KHZ800);

// Global Variables
unsigned long g_lStartTime;
byte g_yContrast   = CONTRAST_LCD_DEF;
byte g_yBrightness = BRIGHTNESS_LCD_DEF;

// Show the pot background colors
bool g_bShowPotColors = true;

                               //  Gr,  Rd,  Bl
int iRed         = pixels.Color(    0, 100,   0);
int iGreen       = pixels.Color(  100,   0,   0);
int iBlue        = pixels.Color(    0,   0, 100);
int iWhite       = pixels.Color(  100, 100, 100);

int iGreenBg     = pixels.Color(  200,   0,   0);
int iYellowBg    = pixels.Color(  193, 204,   0);
int iWhiteBg     = pixels.Color(  200, 200, 200);

byte ayCursorPositionP12[] = { PAGE1COL1, PAGE1ROW1CURSOR };
byte ayCursorPositionP3[]  = { PAGE3COL1, PAGE3ROW1CURSOR };
byte yActiveIndicatorPosition = PAGE3COL1; 
bool abButtonStates[ BUTTON_NUM ];

bool bPage1Initialized  = false;
bool bPage2Initialized  = false;
bool bPage3Initialized  = false;
byte yCurrentPage       = PAGE_NO_1;

bool bDrawPage1Buttons    = false;
bool bDrawPage3Buttons    = false;
bool bDrawPage1Cursor     = false;
bool bDrawPage3Cursor     = false;
bool bDrawActiveIndicator = false;
word wButtonStatesP1Buff  = 0x0000U;
word wButtonStatesP2Buff  = 0x0000U;
word wButtonStatesP3Buff  = 0x0000U;
word wButtonStatesP1      = 0x0000U;
word wButtonStatesP2      = 0x0000U;
word wButtonStatesP3      = 0x0000U;

// Image pointers of the buttons, page 1 to 3
const uint8_t *pButton1  = image_ButtonNotPressed_bits; 
const uint8_t *pButton2  = image_ButtonNotPressed_bits; 
const uint8_t *pButton3  = image_ButtonNotPressed_bits; 
const uint8_t *pButton4  = image_ButtonNotPressed_bits; 
const uint8_t *pButton5  = image_ButtonNotPressed_bits; 

const uint8_t *pButton6  = image_ButtonNotPressed_bits; 
const uint8_t *pButton7  = image_ButtonNotPressed_bits; 
const uint8_t *pButton8  = image_ButtonNotPressed_bits; 
const uint8_t *pButton9  = image_ButtonNotPressed_bits; 
const uint8_t *pButton10 = image_ButtonNotPressed_bits; 

const uint8_t *pButton11 = image_ButtonNotPressed_bits; 
const uint8_t *pButton12 = image_ButtonNotPressed_bits; 
const uint8_t *pButton13 = image_ButtonNotPressed_bits; 
const uint8_t *pButton14 = image_ButtonNotPressed_bits; 
//-----------------------------------------------------

//-----------------
// Local functions 
//-----------------

// Initial Boot Logo
static void displayBootLogo( void )
{
  g_lStartTime = millis();
  
  display.clearBuffer();					// clear the internal memory

  // code from https://lopaka.app/
  display.setFontMode(1);
  display.setBitmapMode(1);
  display.drawXBMP(0, 16, 128, 23, image_SONY_LOGO_bits);
  display.setFont(u8g2_font_5x8_tr);
  display.drawStr(16, 52, "BKM-10R DIY Edition");

  display.sendBuffer();					// transfer internal memory to the display
}

static void setIndividualColor( bool bEncoderOrBackground, int iColor )
{
  if ( bEncoderOrBackground )
  {
    // Set Background LED
    pixels.setPixelColor( LED_BACKGROUND, iColor  );
  }
  else
  {
    // Set Encoder LEDs
    pixels.setPixelColor( LED_ENCODER_1, iColor  );
    pixels.setPixelColor( LED_ENCODER_2, iColor  );
  }

  pixels.show();
}

static void updateButtonBitfields( void )
{
  // This is the part in the code where the actual
	// connection between the bool-array and the 
	// position of the button on the display is set!
  if ( abButtonStates[ IDX_SHIFT      ] ) wButtonStatesP1 |=  BUTTON_MASK_R1C1;    
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R1C1;                                         
  if ( abButtonStates[ IDX_SHIFT      ] ) wButtonStatesP2 |=  BUTTON_MASK_R1C1;    
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R1C1;                                         
  if ( abButtonStates[ IDX_F1         ] ) wButtonStatesP1 |=  BUTTON_MASK_R1C2;
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R1C2;
  if ( abButtonStates[ IDX_F2         ] ) wButtonStatesP1 |=  BUTTON_MASK_R2C2;
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R2C2;
  if ( abButtonStates[ IDX_F3         ] ) wButtonStatesP2 |=  BUTTON_MASK_R1C2;
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R1C2;
  if ( abButtonStates[ IDX_F4         ] ) wButtonStatesP2 |=  BUTTON_MASK_R2C2;
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R2C2;
  if ( abButtonStates[ IDX_HDELAY     ] ) wButtonStatesP1 |=  BUTTON_MASK_R1C3;
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R1C3;
  if ( abButtonStates[ IDX_VDELAY     ] ) wButtonStatesP1 |=  BUTTON_MASK_R2C3;
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R2C3;
  if ( abButtonStates[ IDX_APT        ] ) wButtonStatesP1 |=  BUTTON_MASK_R1C4;
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R1C4;
  if ( abButtonStates[ IDX_MONO       ] ) wButtonStatesP1 |=  BUTTON_MASK_R1C5;
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R1C5;
  if ( abButtonStates[ IDX_UNDERSCAN  ] ) wButtonStatesP1 |=  BUTTON_MASK_R2C1;
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R2C1;                  
  if ( abButtonStates[ IDX_COMB       ] ) wButtonStatesP1 |=  BUTTON_MASK_R2C4;
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R2C4;
  if ( abButtonStates[ IDX_ADDRESS    ] ) wButtonStatesP1 |=  BUTTON_MASK_R2C5;
  else                                    wButtonStatesP1 &= ~BUTTON_MASK_R2C5;
  if ( abButtonStates[ IDX_RED        ] ) wButtonStatesP2 |=  BUTTON_MASK_R1C3;
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R1C3;
  if ( abButtonStates[ IDX_GREEN      ] ) wButtonStatesP2 |=  BUTTON_MASK_R1C4;
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R1C4;
  if ( abButtonStates[ IDX_BLUE       ] ) wButtonStatesP2 |=  BUTTON_MASK_R1C5;
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R1C5;
  if ( abButtonStates[ IDX_SYNC       ] ) wButtonStatesP2 |=  BUTTON_MASK_R2C1;
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R2C1;
  if ( abButtonStates[ IDX_16BY9      ] ) wButtonStatesP2 |=  BUTTON_MASK_R2C3;
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R2C3;
  if ( abButtonStates[ IDX_BLUEONLY   ] ) wButtonStatesP2 |=  BUTTON_MASK_R2C4;
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R2C4;
  if ( abButtonStates[ IDX_SAFEAREA   ] ) wButtonStatesP2 |=  BUTTON_MASK_R2C5;
  else                                    wButtonStatesP2 &= ~BUTTON_MASK_R2C5;
  if ( abButtonStates[ IDX_PHASE      ] ) wButtonStatesP3 |=  BUTTON_MASK_KNOB1;
  else                                    wButtonStatesP3 &= ~BUTTON_MASK_KNOB1;
  if ( abButtonStates[ IDX_CHROMA     ] ) wButtonStatesP3 |=  BUTTON_MASK_KNOB2;
  else                                    wButtonStatesP3 &= ~BUTTON_MASK_KNOB2;                 
  if ( abButtonStates[ IDX_BRIGHTNESS ] ) wButtonStatesP3 |=  BUTTON_MASK_KNOB3;
  else                                    wButtonStatesP3 &= ~BUTTON_MASK_KNOB3;                 
  if ( abButtonStates[ IDX_CONTRAST   ] ) wButtonStatesP3 |=  BUTTON_MASK_KNOB4;
  else                                    wButtonStatesP3 &= ~BUTTON_MASK_KNOB4;
}

static void setButtonStates( word wStateBitfield )
{
  if ( ( BUTTON_MASK_R1C1 & wStateBitfield ) == BUTTON_MASK_R1C1   ) pButton1  = image_ButtonPressed_bits;
  else                                                               pButton1  = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_R1C2 & wStateBitfield ) == BUTTON_MASK_R1C2   ) pButton2  = image_ButtonPressed_bits;
  else                                                               pButton2  = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_R1C3  & wStateBitfield ) == BUTTON_MASK_R1C3  ) pButton3  = image_ButtonPressed_bits; 
  else                                                               pButton3  = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_R1C4  & wStateBitfield ) == BUTTON_MASK_R1C4  ) pButton4  = image_ButtonPressed_bits;
  else                                                               pButton4  = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_R1C5  & wStateBitfield ) == BUTTON_MASK_R1C5  ) pButton5  = image_ButtonPressed_bits;
  else                                                               pButton5  = image_ButtonNotPressed_bits;

  if ( ( BUTTON_MASK_R2C1 & wStateBitfield ) == BUTTON_MASK_R2C1 ) pButton6  = image_ButtonPressed_bits;
  else                                                               pButton6  = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_R2C2 & wStateBitfield ) == BUTTON_MASK_R2C2    ) pButton7  = image_ButtonPressed_bits;
  else                                                               pButton7  = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_R2C3  & wStateBitfield ) == BUTTON_MASK_R2C3  ) pButton8  = image_ButtonPressed_bits;
  else                                                               pButton8  = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_R2C4  & wStateBitfield ) == BUTTON_MASK_R2C4  ) pButton9  = image_ButtonPressed_bits;
  else                                                               pButton9  = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_R2C5  & wStateBitfield ) == BUTTON_MASK_R2C5  ) pButton10 = image_ButtonPressed_bits;
  else                                                               pButton10 = image_ButtonNotPressed_bits;

  if ( ( BUTTON_MASK_KNOB1  & wStateBitfield ) == BUTTON_MASK_KNOB1  ) pButton11 = image_ButtonPressed_bits;
  else                                                                 pButton11 = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_KNOB2  & wStateBitfield ) == BUTTON_MASK_KNOB2  ) pButton12 = image_ButtonPressed_bits;
  else                                                                 pButton12 = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_KNOB3  & wStateBitfield ) == BUTTON_MASK_KNOB3  ) pButton13 = image_ButtonPressed_bits;
  else                                                                 pButton13 = image_ButtonNotPressed_bits;
  if ( ( BUTTON_MASK_KNOB4  & wStateBitfield ) == BUTTON_MASK_KNOB4  ) pButton14 = image_ButtonPressed_bits;
  else                                                                 pButton14 = image_ButtonNotPressed_bits;
}

static void drawPage3ActiveIndicator( byte yCol )
{
	byte xPosition = ( yCol - ACTIVE_INDICATOR_OFFSET );
	
	// Setting all possible Cursor positions to blank
  display.setDrawColor( 0 );
  display.drawBox( 0, 11, 128, 20 ); // Erasing the whole line of cursors
  
	display.setDrawColor( 1 );
	display.setFont(u8g2_font_4x6_tr);
  display.drawStr( xPosition, 22, "active");
	display.drawXBMP( xPosition, 24, 24, 5,  image_ActiveArrow_bits );
}

static void drawPage1Cursor( byte yRow, byte yCol )
{
  // Setting all possible Cursor positions to blank
  display.setDrawColor( 0 );
  display.drawBox( PAGE1COL1, PAGE1ROW1CURSOR, 107, 4 ); // Erasing the whole line of cursors
  display.drawBox( PAGE1COL1, PAGE1ROW2CURSOR, 107, 4 ); // Erasing the whole line of cursors
  display.setDrawColor( 1 );
  
  display.drawXBMP(  yRow, yCol, 7, 4,  image_SmallArrowUp_bits );
}

static void drawPage3Cursor( byte yRow, byte yCol )
{
  // Setting all possible Cursor positions to blank
  display.setDrawColor( 0 );
  display.drawBox( PAGE3COL1, PAGE3ROW1CURSOR, 114, 4 ); // Erasing the whole line of cursors
  display.setDrawColor( 1 );
  
  display.drawXBMP(  yRow, yCol, 7, 4,  image_SmallArrowUp_bits );
}

static void displayInitialSetupPage( byte yPageNumber, bool bDrawButtons, bool bDrawCursor )
{
	// Initialize static elements of the page once
	  display.clearBuffer();					// clear the internal memory
    display.setFontMode(1);
    display.setBitmapMode(1);

    // Display Header
	  display.drawLine(3, 10, 123, 10);
    display.setFont(u8g2_font_6x10_tr);
    
	  if ( yPageNumber == PAGE_NO_1 ) display.drawStr(3, 8, "Buttons");
    if ( yPageNumber == PAGE_NO_2 ) display.drawStr(3, 8, "Buttons");
	  if ( yPageNumber == PAGE_NO_3 ) display.drawStr(3, 8, "Knobs"  );

    if ( yPageNumber == PAGE_NO_1 ) display.drawStr(107, 8, "1/2");  // Page 1 or 2 is determined by Shift
    if ( yPageNumber == PAGE_NO_2 ) display.drawStr(107, 8, "1/2");  // Page 1 or 2 is determined by Shift
	  if ( yPageNumber == PAGE_NO_3 ) display.drawStr(107, 8, "2/2");  

    display.setFont(u8g2_font_5x8_tr);
      
	  if ( yPageNumber == PAGE_NO_1 ) 
    {
      // First Row of Labels
      display.drawStr(  1, 30, "SHIFT");
      display.drawStr( 34, 30, "F1"   );
	    display.drawStr( 54, 30, "HDLY" );
      display.drawStr( 82, 30, "APT"  );
	    display.drawStr(103, 30, "MONO" );
	  
	    // Second Row of Labels
	    display.drawStr(   1, 58, "USCAN");
	    display.drawStr(  34, 58, "F2"   );
	    display.drawStr(  54, 58, "VDLY" );
      display.drawStr(  79, 58, "COMB" );
	    display.drawStr( 103, 58, "ADDR" );
    }

    if ( yPageNumber == PAGE_NO_2 ) 
    {
      // First Row of Labels
      display.drawStr(  1, 30, "SHIFT");
      display.drawStr( 34, 30, "F3"   );
      display.drawStr( 61, 30, "R"    );
      display.drawStr( 86, 30, "G"    );
      display.drawStr(111, 30, "B"    );
      
      // Second Row of Labels
      display.drawStr(  4, 58, "SYNC" );
      display.drawStr( 34, 58, "F4"   );
      display.drawStr( 54, 58, "16:9" );
      display.drawStr( 79, 58, "BLUE" );
      display.drawStr(103, 58, "SAFE" );
    }

    if ( yPageNumber == PAGE_NO_3 ) 
    {
      display.drawStr( 3, 50, "PHASE" );
      display.drawStr(31, 50, "CHROMA");
      display.drawStr(64, 50, "BRIGHT");
      display.drawStr(99, 50, "CONTR" );
    }
    
    // Set the global button pointers to images corresponding to button state 
    if ( PAGE_NO_1 == yPageNumber ) setButtonStates( wButtonStatesP1 );
	  if ( PAGE_NO_2 == yPageNumber ) setButtonStates( wButtonStatesP2 );
    if ( PAGE_NO_3 == yPageNumber ) setButtonStates( wButtonStatesP3 ); 
    
    if ( ( PAGE_NO_1 == yPageNumber ) || ( PAGE_NO_2 == yPageNumber ) )
    {
      // First Row of Button indicators
	    display.drawXBMP( PAGE1COL1, PAGE1ROW1, 7, 7,  pButton1  );
      display.drawXBMP( PAGE1COL2, PAGE1ROW1, 7, 7,  pButton2  );
      display.drawXBMP( PAGE1COL3, PAGE1ROW1, 7, 7,  pButton3  );
      display.drawXBMP( PAGE1COL4, PAGE1ROW1, 7, 7,  pButton4  );
      display.drawXBMP( PAGE1COL5, PAGE1ROW1, 7, 7,  pButton5  );
	  
      // Second Row of Button indicators
      display.drawXBMP( PAGE1COL1, PAGE1ROW2, 7, 7,  pButton6  );
      display.drawXBMP( PAGE1COL2, PAGE1ROW2, 7, 7,  pButton7  );
      display.drawXBMP( PAGE1COL3, PAGE1ROW2, 7, 7,  pButton8  );
      display.drawXBMP( PAGE1COL4, PAGE1ROW2, 7, 7,  pButton9  );
      display.drawXBMP( PAGE1COL5, PAGE1ROW2, 7, 7,  pButton10 );
	  
	    // Draw Cursor arrow
			drawPage1Cursor ( ayCursorPositionP12[0], ayCursorPositionP12[1] );  
    }

    if ( PAGE_NO_3 == yPageNumber )
    {
      // First Row of Button indicators
	    display.drawXBMP( PAGE3COL1, PAGE3ROW1, 7, 7,  pButton11  );
      display.drawXBMP( PAGE3COL2, PAGE3ROW1, 7, 7,  pButton12  );
      display.drawXBMP( PAGE3COL3, PAGE3ROW1, 7, 7,  pButton13  );
      display.drawXBMP( PAGE3COL4, PAGE3ROW1, 7, 7,  pButton14  );

      // Draw Cursor Arrow
			drawPage3Cursor ( ayCursorPositionP3[0], ayCursorPositionP3[1] );

			// Draw "Active" indicator Arrow
			drawPage3ActiveIndicator( yActiveIndicatorPosition );
    } 

    // Update Display
	  display.sendBuffer();
}

// Page to display
static void displayPage( byte yPageNumber )
{
	if ( ( PAGE_NO_1 == yPageNumber ) && !bPage1Initialized )
	{
	  displayInitialSetupPage( yPageNumber, true, true );
	
	  bPage1Initialized = true;
    bPage2Initialized = false;
    bPage3Initialized = false;
	}
  else if( ( PAGE_NO_2 == yPageNumber ) && !bPage2Initialized )
	{
	  displayInitialSetupPage( yPageNumber, true, true );
	
	  bPage1Initialized = false;
    bPage2Initialized = true;
    bPage3Initialized = false;
	}
  else if( ( PAGE_NO_3 == yPageNumber ) && !bPage3Initialized )
	{
	  displayInitialSetupPage( yPageNumber, true, true );
	
	  bPage1Initialized = false;
    bPage2Initialized = false;
    bPage3Initialized = true;
	} 
	else
	{
		if ( bDrawPage1Buttons )
		{
			if ( PAGE_NO_1 == yCurrentPage ) setButtonStates( wButtonStatesP1 ); 
		  if ( PAGE_NO_2 == yCurrentPage ) setButtonStates( wButtonStatesP2 );

          // Clear Old Values
					display.setDrawColor(0);					// Setting DrawColor to Clear individual Pixels
					
					display.drawXBMP( PAGE1COL1, PAGE1ROW1, 7, 7,  image_ButtonPressed_bits );
          display.drawXBMP( PAGE1COL2, PAGE1ROW1, 7, 7,  image_ButtonPressed_bits );
          display.drawXBMP( PAGE1COL3, PAGE1ROW1, 7, 7,  image_ButtonPressed_bits );
          display.drawXBMP( PAGE1COL4, PAGE1ROW1, 7, 7,  image_ButtonPressed_bits );
          display.drawXBMP( PAGE1COL5, PAGE1ROW1, 7, 7,  image_ButtonPressed_bits );
      
          display.drawXBMP( PAGE1COL1, PAGE1ROW2, 7, 7,  image_ButtonPressed_bits );
          display.drawXBMP( PAGE1COL2, PAGE1ROW2, 7, 7,  image_ButtonPressed_bits );
          display.drawXBMP( PAGE1COL3, PAGE1ROW2, 7, 7,  image_ButtonPressed_bits );
          display.drawXBMP( PAGE1COL4, PAGE1ROW2, 7, 7,  image_ButtonPressed_bits );
          display.drawXBMP( PAGE1COL5, PAGE1ROW2, 7, 7,  image_ButtonPressed_bits );  


					// Set the new values
					display.setDrawColor(1);					// Setting DrawColor back
					display.drawXBMP( PAGE1COL1, PAGE1ROW1, 7, 7,  pButton1 );
          display.drawXBMP( PAGE1COL2, PAGE1ROW1, 7, 7,  pButton2 );
          display.drawXBMP( PAGE1COL3, PAGE1ROW1, 7, 7,  pButton3 );
          display.drawXBMP( PAGE1COL4, PAGE1ROW1, 7, 7,  pButton4 );
          display.drawXBMP( PAGE1COL5, PAGE1ROW1, 7, 7,  pButton5 );
      
          display.drawXBMP( PAGE1COL1, PAGE1ROW2, 7, 7,  pButton6  );
          display.drawXBMP( PAGE1COL2, PAGE1ROW2, 7, 7,  pButton7  );
          display.drawXBMP( PAGE1COL3, PAGE1ROW2, 7, 7,  pButton8  );
          display.drawXBMP( PAGE1COL4, PAGE1ROW2, 7, 7,  pButton9  );
          display.drawXBMP( PAGE1COL5, PAGE1ROW2, 7, 7,  pButton10 );  

		  // Update Display
		  display.sendBuffer();

		  // Done Updating
		  bDrawPage1Buttons = false;
	  }

    if ( bDrawPage1Cursor )
		{
      drawPage1Cursor ( ayCursorPositionP12[0], ayCursorPositionP12[1] );

      // Update Display
		  display.sendBuffer();

      // Done Updating
      bDrawPage1Cursor = false;
    }

    if ( bDrawPage3Buttons )
		{
      setButtonStates( wButtonStatesP3 );
			
			// Clear Old Values
			display.setDrawColor(0);					// Setting DrawColor to Clear individual Pixels
			display.drawXBMP( PAGE3COL1, PAGE3ROW1, 7, 7,  image_ButtonPressed_bits  );
      display.drawXBMP( PAGE3COL2, PAGE3ROW1, 7, 7,  image_ButtonPressed_bits  );
      display.drawXBMP( PAGE3COL3, PAGE3ROW1, 7, 7,  image_ButtonPressed_bits  );
      display.drawXBMP( PAGE3COL4, PAGE3ROW1, 7, 7,  image_ButtonPressed_bits  );
		
		  // Set the new values
			display.setDrawColor(1);					// Setting DrawColor back
			display.drawXBMP( PAGE3COL1, PAGE3ROW1, 7, 7,  pButton11  );
      display.drawXBMP( PAGE3COL2, PAGE3ROW1, 7, 7,  pButton12  );
      display.drawXBMP( PAGE3COL3, PAGE3ROW1, 7, 7,  pButton13  );
      display.drawXBMP( PAGE3COL4, PAGE3ROW1, 7, 7,  pButton14  );
		
		  // Update Display
		  display.sendBuffer();

		  // Done Updating
		  bDrawPage3Buttons = false; 
		}
		
		if ( bDrawPage3Cursor )
		{
      drawPage3Cursor ( ayCursorPositionP3[0], ayCursorPositionP3[1] );

      // Update Display
		  display.sendBuffer();

      // Done Updating
      bDrawPage3Cursor = false;
    }

		if ( bDrawActiveIndicator )
		{
      drawPage3ActiveIndicator ( yActiveIndicatorPosition );

      // Update Display
		  display.sendBuffer();
			
			// Done
			bDrawActiveIndicator = false;
		}
	}
}


void display_exec( bool bMoveCursorLeft, bool bMoveCursorRight, bool bMoveActiveIndicator ) 
{
  if ( g_lStartTime != BOOT_DONE )
  {
    if ( ( millis() - g_lStartTime ) < BOOT_LOGO_TIME )
    {
      return;
    }
    else
    {
      g_lStartTime = BOOT_DONE;
      setIndividualColor( true, iGreenBg );  // Display background
    }
  }
  
  //------------------------------
  // process Inputs
  //
  // Input signals have to already 
  // be rising edge signals!
  //------------------------------

  wButtonStatesP1Buff = wButtonStatesP1;
  wButtonStatesP2Buff = wButtonStatesP2;
	wButtonStatesP3Buff = wButtonStatesP3;

	// Now the function bool-Array has to be passed on
	// to the Bitfields of the three pages
	updateButtonBitfields();

  // if a function was updated, the display has to be redrawn
	if ( ( PAGE_NO_1 == yCurrentPage ) || ( PAGE_NO_2 == yCurrentPage ) )
	{
	  if (  wButtonStatesP1Buff != wButtonStatesP1     ) bDrawPage1Buttons = true;
	  if (  wButtonStatesP2Buff != wButtonStatesP2     ) bDrawPage1Buttons = true;
	} 

  if ( PAGE_NO_3 == yCurrentPage )
	{
	  if (  wButtonStatesP3Buff != wButtonStatesP3     ) bDrawPage3Buttons = true;
	}

  //---------------
  // move cursor
  //---------------
  
  //------------------
  // Moving right
  //------------------
  if ( bMoveCursorRight )
  {
    // Rising Edge detected -> Move Cursor to the Right
    if ( ( PAGE_NO_1 == yCurrentPage ) || ( PAGE_NO_2 == yCurrentPage ) )
    {
      if ( PAGE1COL5 == ayCursorPositionP12[0] )
      {
        // Special 1: Cursor is at the end of its Row:
        if ( PAGE1ROW1CURSOR == ayCursorPositionP12[1] )
        {
          // End of first Row -> jump to 2nd Row, 1st Col
          ayCursorPositionP12[0] = PAGE1COL1;
          ayCursorPositionP12[1] = PAGE1ROW2CURSOR;

					bDrawPage1Cursor = true;
        }
        else if ( ayCursorPositionP12[1] == PAGE1ROW2CURSOR )
        {
          // End of second Row -> jump to next page
          ayCursorPositionP12[0] = PAGE1COL1;
          ayCursorPositionP12[1] = PAGE1ROW1CURSOR;

          // Always jump to Page 3
          // Page 1 or 2 is dependent on Shift Button
          yCurrentPage = PAGE_NO_3;
          ayCursorPositionP3[0] = PAGE3COL1;
        }
        else
        {
        }
      }
      else
      {
        // Jump X-position by one Column
        ayCursorPositionP12[0] += 25;
        bDrawPage1Cursor = true;
			}
    }
    else if ( PAGE_NO_3 == yCurrentPage )
    {
      // Page 3
      // Special: Cursor is at the end
      if ( PAGE3COL4 == ayCursorPositionP3[0] )
      {
        // Setting up Cursor pos for coming page 1/2
        ayCursorPositionP12[0] = PAGE1COL1;
        ayCursorPositionP12[1] = PAGE1ROW1CURSOR;
        
        if ( abButtonStates[ IDX_SHIFT ] )
        {
          // Shifted buttons
          yCurrentPage = PAGE_NO_2;
        }
        else
        {
          // Non-shifted buttons
          yCurrentPage = PAGE_NO_1;
        }
      }
      else
      {
        ayCursorPositionP3[0] += 32;
        bDrawPage3Cursor = true;
			}
    }
    else
    {}
  }

	//------------------
  // Moving left
  //------------------
  if ( bMoveCursorLeft )
  {
    // Rising Edge detected -> Move Cursor to the Left
    if ( ( PAGE_NO_1 == yCurrentPage ) || ( PAGE_NO_2 == yCurrentPage ) )
    {
      if ( PAGE1COL1 == ayCursorPositionP12[0] )
      {
        // Special 1: Cursor is at the end of its Row:
        if ( PAGE1ROW2CURSOR == ayCursorPositionP12[1] )
        {
          // End of second Row -> jump to 1st Row, 5th Col
          ayCursorPositionP12[0] = PAGE1COL5;
          ayCursorPositionP12[1] = PAGE1ROW1CURSOR;

					bDrawPage1Cursor = true;
        }
        else if ( ayCursorPositionP12[1] == PAGE1ROW1CURSOR )
        {
          // End of first Row -> jump to previous page
          ayCursorPositionP12[0] = PAGE1COL5;
          ayCursorPositionP12[1] = PAGE1ROW2CURSOR;

          // Always jumps to page 3, p1 or p2 is determined by SHIFT
          yCurrentPage = PAGE_NO_3;
          ayCursorPositionP3[0] = PAGE3COL4;
        }
        else
        {
        }
      }
      else
      {
        // Jump X-position by one Column
        ayCursorPositionP12[0] -= 25;
        bDrawPage1Cursor = true;
			}
    }
    else if ( PAGE_NO_3 == yCurrentPage )
    {
      // Page 3
      // Special: Cursor is at the end
      if ( PAGE3COL1 == ayCursorPositionP3[0] )
      {
        // Setting up Cursor pos for coming page 1/2
        ayCursorPositionP12[0] = PAGE1COL5;
        ayCursorPositionP12[1] = PAGE1ROW2CURSOR;
        
        if ( abButtonStates[ IDX_SHIFT ] )
        {
          // Shifted buttons
          yCurrentPage = PAGE_NO_2;
        }
        else
        {
          // Non-shifted buttons
          yCurrentPage = PAGE_NO_1;
        }
      }
      else
      {
        ayCursorPositionP3[0] -= 32;
        bDrawPage3Cursor = true;
			}
    }
    else
    {}
  }

  //-----------------------------
  // Move Active Indicator Arrow
  //-----------------------------
	if ( bMoveActiveIndicator )
	{
		if ( yActiveIndicatorPosition < PAGE3COL4 )
		{
			yActiveIndicatorPosition += PAGE3COLSTEPS;
		}
		else                            
		{
      yActiveIndicatorPosition = PAGE3COL1;
		}             
		
		if ( PAGE_NO_3 == yCurrentPage )
		{
		  bDrawActiveIndicator = true;
		}

    if ( g_bShowPotColors )
    {
      if ( PAGE3COL1 == yActiveIndicatorPosition ) setIndividualColor( false, iRed   );
      if ( PAGE3COL2 == yActiveIndicatorPosition ) setIndividualColor( false, iGreen );
      if ( PAGE3COL3 == yActiveIndicatorPosition ) setIndividualColor( false, iBlue  );
      if ( PAGE3COL4 == yActiveIndicatorPosition ) setIndividualColor( false, iWhite );
    }
	}

  //---------------------

  displayPage( yCurrentPage );
}

//-------------------
// Exported functions
//-------------------

void display_set_contrast( bool bUp, bool bDown )
{
  if ( bUp || bDown )
  {
    if ( bUp )
    {
      if ( g_yContrast <= ( CONTRAST_LCD_MAX - CONTRAST_LCD_STEP ) )
      {
        g_yContrast += CONTRAST_LCD_STEP;
      }
      else 
      {
        g_yContrast = CONTRAST_LCD_MAX;
      }
    } 

    if ( bDown )
    {
      if ( g_yContrast >= ( CONTRAST_LCD_MIN + CONTRAST_LCD_STEP ) )
      {
        g_yContrast -= CONTRAST_LCD_STEP;
      }
      else 
      {
        g_yContrast = CONTRAST_LCD_MIN;
      }
    } 

    Serial.print( "Contrast: ");
    Serial.println( g_yContrast );
    display.setContrast( g_yContrast );
  }
}

void display_set_brightness( bool bUp, bool bDown )
{
  if ( bUp )
  {
    if ( g_yBrightness <= ( BRIGHTNESS_LCD_MAX - BRIGHTNESS_LCD_STEP ) )
    {
      g_yBrightness += BRIGHTNESS_LCD_STEP;
    }
    else if ( g_yBrightness == BRIGHTNESS_LCD_MAX )
    {
      g_yBrightness = BRIGHTNESS_LCD_MIN;
    }
    else
    {
      g_yBrightness = BRIGHTNESS_LCD_MAX;
    }
  } 

  if ( bDown )
  {
    if ( g_yBrightness >= ( BRIGHTNESS_LCD_MIN + BRIGHTNESS_LCD_STEP ) )
    {
      g_yBrightness -= BRIGHTNESS_LCD_STEP;
    }
    else if ( g_yBrightness == BRIGHTNESS_LCD_MIN )
    {
      g_yBrightness = BRIGHTNESS_LCD_MAX;
    }
    else
    {
      g_yBrightness = BRIGHTNESS_LCD_MIN;
    }
  } 

  if ( bUp || bDown )
  {
    Serial.print( "Brightness: ");
    Serial.println( g_yBrightness );
    pixels.setBrightness( g_yBrightness );
    pixels.show();
  }
}

byte display_read_highlighted_function( void )
{
  // give the current highlighted function to
  // send out the OpCode to the monitor
	byte yReturnVal = 0;
	
	if ( PAGE_NO_1 == yCurrentPage )
	{
    if ( PAGE1ROW1CURSOR == ayCursorPositionP12[1] )
		{
      // First Row
		  if ( PAGE1COL1 == ayCursorPositionP12[0] ) yReturnVal = IDX_SHIFT;
		  if ( PAGE1COL2 == ayCursorPositionP12[0] ) yReturnVal = IDX_F1;
      if ( PAGE1COL3 == ayCursorPositionP12[0] ) yReturnVal = IDX_HDELAY;
      if ( PAGE1COL4 == ayCursorPositionP12[0] ) yReturnVal = IDX_APT;
      if ( PAGE1COL5 == ayCursorPositionP12[0] ) yReturnVal = IDX_MONO;
    }
		
		if ( PAGE1ROW2CURSOR == ayCursorPositionP12[1] )
		{
      // Second Row
			if ( PAGE1COL1 == ayCursorPositionP12[0] ) yReturnVal = IDX_UNDERSCAN;
		  if ( PAGE1COL2 == ayCursorPositionP12[0] ) yReturnVal = IDX_F2;
      if ( PAGE1COL3 == ayCursorPositionP12[0] ) yReturnVal = IDX_VDELAY;
      if ( PAGE1COL4 == ayCursorPositionP12[0] ) yReturnVal = IDX_COMB;
      if ( PAGE1COL5 == ayCursorPositionP12[0] ) yReturnVal = IDX_ADDRESS;
		}
	}

	if ( PAGE_NO_2 == yCurrentPage )
	{
    if ( PAGE1ROW1CURSOR == ayCursorPositionP12[1] )
		{
      // First Row
		  if ( PAGE1COL1 == ayCursorPositionP12[0] ) yReturnVal = IDX_SHIFT;
		  if ( PAGE1COL2 == ayCursorPositionP12[0] ) yReturnVal = IDX_F3;
      if ( PAGE1COL3 == ayCursorPositionP12[0] ) yReturnVal = IDX_RED;
      if ( PAGE1COL4 == ayCursorPositionP12[0] ) yReturnVal = IDX_GREEN;
      if ( PAGE1COL5 == ayCursorPositionP12[0] ) yReturnVal = IDX_BLUE;
    }
		
		if ( PAGE1ROW2CURSOR == ayCursorPositionP12[1] )
		{
      // Second Row
			if ( PAGE1COL1 == ayCursorPositionP12[0] ) yReturnVal = IDX_SYNC;
		  if ( PAGE1COL2 == ayCursorPositionP12[0] ) yReturnVal = IDX_F4;
      if ( PAGE1COL3 == ayCursorPositionP12[0] ) yReturnVal = IDX_16BY9;
      if ( PAGE1COL4 == ayCursorPositionP12[0] ) yReturnVal = IDX_BLUEONLY;
      if ( PAGE1COL5 == ayCursorPositionP12[0] ) yReturnVal = IDX_SAFEAREA;
		}
	}
	
	if ( PAGE_NO_3 == yCurrentPage )
	{
	  if ( PAGE3COL1 == ayCursorPositionP3[0] ) yReturnVal = IDX_PHASE; 
		if ( PAGE3COL2 == ayCursorPositionP3[0] ) yReturnVal = IDX_CHROMA;
		if ( PAGE3COL3 == ayCursorPositionP3[0] ) yReturnVal = IDX_BRIGHTNESS;
		if ( PAGE3COL4 == ayCursorPositionP3[0] ) yReturnVal = IDX_CONTRAST;
	}

	return yReturnVal;
}

bool display_set_function_button( byte yFunctionIdx, bool bValue )
{
  bool bRetVal = false;

  // Set Button state according to the monitor's response
  if ( yFunctionIdx < BUTTON_NUM )
  {
    // valid Index
    abButtonStates[ yFunctionIdx ] = bValue;
    bRetVal = true;

    // Special: If Shift is toggled, the page has to switch
    if ( IDX_SHIFT == yFunctionIdx )
    {
      if ( abButtonStates[ yFunctionIdx ] )
      {
        // Shift Mode - Yellow Backgound
        setIndividualColor( true, iYellowBg );
        
        if ( ( yCurrentPage == PAGE_NO_1 ) || ( yCurrentPage == PAGE_NO_2 ) )
        {
          // Shift was false and is now true --> change to Page 2
          yCurrentPage = PAGE_NO_2;
          Serial.println( "Should now be Page 2" );
        }
      }
      else
      {
        // normal Mode - Green Backgound
        setIndividualColor( true, iGreenBg );
        
        if ( ( yCurrentPage == PAGE_NO_1 ) || ( yCurrentPage == PAGE_NO_2 ) )
        {
          // Shift was false and is now true --> change to Page 1
          yCurrentPage = PAGE_NO_1;
          Serial.println( "Should now be Page 1" );
        }
      }
    }
  }

  return bRetVal;
}

void display_init( void )
{
  SPI.setTX( PIN_LCD_SPI_MOSI );
  SPI.setSCK( PIN_LCD_SPI_SCK );
  SPI.setRX( 4 );  // unused
  SPI.setCS( 5 );  // unused
  
  display.begin();
  display.setContrast( CONTRAST_LCD_DEF );

  // Display Sony Logo
  displayBootLogo();

  // Hintergrundbeleuchung
  pixels.begin();
  pixels.setPixelColor( LED_BACKGROUND, iWhiteBg );  // Display background
  if ( g_bShowPotColors )
  {
    pixels.setPixelColor( LED_ENCODER_1,  iRed     );  // Encoder Pot LED 1
    pixels.setPixelColor( LED_ENCODER_2,  iRed     );  // Encoder Pot LED 2
  }
  pixels.setBrightness( g_yBrightness );
  pixels.show();
}

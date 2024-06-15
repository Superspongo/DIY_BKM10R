//---------------------------
// Display Defines
//---------------------------
#define CONTRAST_LCD_MIN    (   1 ) 
#define CONTRAST_LCD_MAX    ( 100 ) // 125 is basically white already
#define CONTRAST_LCD_DEF    (  10 ) 
#define CONTRAST_LCD_STEP   (   1 )

#define BRIGHTNESS_LCD_MIN  (  50 ) 
#define BRIGHTNESS_LCD_MAX  ( 250 ) 
#define BRIGHTNESS_LCD_DEF  ( 200 ) 
#define BRIGHTNESS_LCD_STEP (  50 )

#define BOOT_LOGO_TIME      ( 3000 )  // Boot logo in ms
#define BOOT_DONE           ( 0xFFFFFFFFU )

#define PAGE_NO_1           ( 0x01 )
#define PAGE_NO_2           ( 0x02 )
#define PAGE_NO_3           ( 0x03 )

#define ROW_NUM_BUTTONS     ( 2 )  // Number of Rows on the Button Page 
#define COL_NUM_BUTTONS     ( 5 )  // Number of Colums on the Button Page

#define BUTTON_ACTIVE_PIN        ( 12 )
#define BUTTON_ENTER_PIN         (  8 )
#define BUTTON_CURSOR_LEFT_PIN   (  PIN_ENCODER_BUTTON  )
#define BUTTON_CURSOR_RIGHT_PIN  (  PIN_FUNCTION_BUTTON )

// Buttons Left Right on the MSBs
#define BUTTON_MASK_R1C1   ( 0x0001 )
#define BUTTON_MASK_R1C2   ( 0x0002 )
#define BUTTON_MASK_R1C3   ( 0x0004 )
#define BUTTON_MASK_R1C4   ( 0x0008 )
#define BUTTON_MASK_R1C5   ( 0x0010 )

#define BUTTON_MASK_R2C1   ( 0x0020 )
#define BUTTON_MASK_R2C2   ( 0x0040 )
#define BUTTON_MASK_R2C3   ( 0x0080 )
#define BUTTON_MASK_R2C4   ( 0x0100 )
#define BUTTON_MASK_R2C5   ( 0x0200 )

#define BUTTON_MASK_KNOB1  ( 0x0400 )
#define BUTTON_MASK_KNOB2  ( 0x0800 )
#define BUTTON_MASK_KNOB3  ( 0x1000 )
#define BUTTON_MASK_KNOB4  ( 0x2000 )

#define BUTTON_NUM         (     23 )
#define IDX_SHIFT          (      0 )  
#define IDX_F1             (      1 )
#define IDX_F2             (      2 )
#define IDX_F3             (      3 )
#define IDX_F4             (      4 )
#define IDX_HDELAY         (      5 )
#define IDX_VDELAY         (      6 )
#define IDX_APT            (      7 )
#define IDX_MONO           (      8 )
#define IDX_UNDERSCAN      (      9 )
#define IDX_COMB           (     10 )
#define IDX_ADDRESS        (     11 )
#define IDX_RED            (     12 )
#define IDX_GREEN          (     13 )
#define IDX_BLUE           (     14 )
#define IDX_SYNC           (     15 )
#define IDX_16BY9          (     16 )
#define IDX_BLUEONLY       (     17 )
#define IDX_SAFEAREA       (     18 )
#define IDX_PHASE          (     19 )
#define IDX_CHROMA         (     20 )
#define IDX_BRIGHTNESS     (     21 )
#define IDX_CONTRAST       (     22 )

// X-Positions of Buttons and Cursor
// 25 Pixels inbetween
enum eColsPage1
{
  PAGE1COL1 =  10,
  PAGE1COL2 =  35,
  PAGE1COL3 =  60,
	PAGE1COL4 =  85,
	PAGE1COL5 = 110
};

// 32 Pixels inbetween
enum eColsPage3
{
  PAGE3COL1 =  11,
  PAGE3COL2 =  43,
  PAGE3COL3 =  75,
	PAGE3COL4 =  107
};

#define PAGE3COLSTEPS  ( 32 )

// The "active" cursor on page 3 is wider than the normal cursor
#define ACTIVE_INDICATOR_OFFSET  (  8 )
#define ACTIVE_INDICATOR_ARROW_Y ( 29 )

// Y-Positions of Buttons
enum eRowsPage1
{
  PAGE1ROW1 = 14,
  PAGE1ROW2 = 42,
};

#define PAGE3ROW1   ( 33 )

// Y-Positions of Cursor
enum eRowsPage1Cursor
{
  PAGE1ROW1CURSOR = 32,
  PAGE1ROW2CURSOR = 61,
};

#define PAGE3ROW1CURSOR  ( 53 )

//--------------------
// Exported Functions
//--------------------

void display_init( void );
void display_exec( bool bMoveCursorLeft, bool bMoveCursorRight, bool bMoveActiveIndicator );
bool display_set_function_button( byte yFunctionIdx, bool bValue );
byte display_read_highlighted_function( void );
byte display_read_active_encoder( void );
void display_set_contrast( bool bUp, bool bDown );
void display_set_brightness( bool bUp, bool bDown );
void display_encoder_backlight( bool bOnOff );

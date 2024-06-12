#include <U8g2lib.h>

static const unsigned char image_SONY_LOGO_bits[] U8X8_PROGMEM = {0x80,0x7f,0x00,0x00,0x00,0xfe,0x01,0x80,0xff,0x01,0xf8,0x3f,0xfc,0x7f,0xe0,0xff,0xf0,0xff,0xc7,0x00,0xe0,0xff,0x1f,
                                                                  0x80,0xff,0x03,0xf8,0x3f,0xfc,0x7f,0xe0,0xff,0xfc,0xff,0xff,0x00,0xf8,0xff,0x7f,0x80,0xff,0x07,0xf8,0x3f,0xfc,0x7f,
                                                                  0xe0,0xff,0x3e,0x00,0xfe,0x00,0xfc,0x01,0xfe,0x00,0xf8,0x0f,0x80,0x07,0xf0,0x07,0x00,0x1e,0x1f,0x00,0xf8,0x00,0x7e,
                                                                  0x00,0xf8,0x01,0xf8,0x1f,0x80,0x07,0xe0,0x0f,0x00,0x0f,0x0f,0x00,0xf0,0x00,0x3f,0x00,0xf0,0x03,0xf8,0x3f,0x80,0x07,
                                                                  0xc0,0x1f,0x80,0x07,0x0f,0x00,0xe0,0x80,0x1f,0x00,0xe0,0x07,0xb8,0x7f,0x80,0x07,0x80,0x3f,0xc0,0x03,0x1f,0x00,0xc0,
                                                                  0x80,0x1f,0x00,0xe0,0x07,0x38,0xff,0x80,0x07,0x00,0x7f,0xe0,0x01,0xff,0x0f,0x00,0xc0,0x0f,0x00,0xc0,0x0f,0x38,0xfe,
                                                                  0x81,0x07,0x00,0xfe,0xf0,0x00,0xff,0xff,0x07,0xc0,0x0f,0x00,0xc0,0x0f,0x38,0xfc,0x83,0x07,0x00,0xfc,0x79,0x00,0xfe,
                                                                  0xff,0x3f,0xc0,0x0f,0x00,0xc0,0x0f,0x38,0xf8,0x87,0x07,0x00,0xf8,0x3f,0x00,0xfc,0xff,0x7f,0xc0,0x0f,0x00,0xc0,0x0f,
                                                                  0x38,0xf0,0x8f,0x07,0x00,0xf0,0x1f,0x00,0xf0,0xff,0xff,0xc0,0x0f,0x00,0xc0,0x0f,0x38,0xe0,0x9f,0x07,0x00,0xe0,0x0f,
                                                                  0x00,0x00,0xfc,0xff,0xc1,0x0f,0x00,0xc0,0x0f,0x38,0xc0,0xbf,0x07,0x00,0xc0,0x0f,0x00,0x03,0x00,0xfc,0xc1,0x0f,0x00,
                                                                  0xc0,0x0f,0x38,0x80,0xff,0x07,0x00,0xc0,0x0f,0x00,0x03,0x00,0xf0,0xc1,0x1f,0x00,0xe0,0x07,0x38,0x00,0xff,0x07,0x00,
                                                                  0xc0,0x0f,0x00,0x07,0x00,0xf0,0x81,0x1f,0x00,0xe0,0x07,0x38,0x00,0xfe,0x07,0x00,0xc0,0x0f,0x00,0x0f,0x00,0xf0,0x01,
                                                                  0x3f,0x00,0xf0,0x03,0x38,0x00,0xfc,0x07,0x00,0xc0,0x0f,0x00,0x1f,0x00,0xf0,0x01,0x7e,0x00,0xf8,0x01,0x38,0x00,0xf8,
                                                                  0x07,0x00,0xc0,0x0f,0x00,0xff,0x00,0xfc,0x00,0xfc,0x01,0xfe,0x00,0x38,0x00,0xf0,0x07,0x00,0xc0,0x0f,0x00,0xff,0xff,
                                                                  0x7f,0x00,0xf8,0xff,0x7f,0x00,0x7c,0x00,0xe0,0x07,0x00,0xe0,0x1f,0x00,0xc7,0xff,0x1f,0x00,0xe0,0xff,0x1f,0x80,0xff,
                                                                  0x03,0xc0,0x07,0x00,0xfc,0xff,0x00,0x00,0xfc,0x01,0x00,0x00,0xff,0x03,0x80,0xff,0x03,0x80,0x07,0x00,0xfc,0xff,0x00};
static const unsigned char image_ButtonPressed_bits[] U8X8_PROGMEM    = {0x7f,0x41,0x5d,0x5d,0x5d,0x41,0x7f};
static const unsigned char image_ButtonNotPressed_bits[] U8X8_PROGMEM = {0x7f,0x41,0x41,0x49,0x41,0x41,0x7f};
static const unsigned char image_SmallArrowUp_bits[] U8X8_PROGMEM = {0x08,0x1c,0x3e,0x7f};
static const unsigned char image_ActiveArrow_bits[] U8X8_PROGMEM = {0xff,0x80,0xff,0x00,0x41,0x00,0x00,0x22,0x00,0x00,0x14,0x00,0x00,0x08,0x00};
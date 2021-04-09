#ifndef SSD1306OLED_h
#define SSD1306OLED_h

// SSD1306 OLED I2C driver for Raspberry Pi Pico board, derived from aleopardstailDisplayDriver 
// generic base graphics class
// defines a memory buffer for a 128 pixel wide by 64 pixel high display
// pixels arranged in the buffer as per the SSD1306 memory pattern, each byte
// is 8 vertical pixels, next byte is the next column of 8 horizontally

#include "hardware/i2c.h"
#include "DisplayDriver.h"
#include "pico/stdlib.h"
#include <stdlib.h>

// control constants for the SSD1306
#define SET_CONTRAST		0x81
#define SET_ENTIRE_ON		0xA4
#define SET_NORM_INV		0xA6
#define SET_DISP			0xAE
#define SET_MEM_ADDR		0x20
#define SET_COL_ADDR		0x21
#define SET_PAGE_ADDR		0x22
#define SET_DISP_START_LINE	0x40
#define SET_SEG_REMAP		0xA0
#define SET_MUX_RATIO		0xA8
#define SET_COM_OUT_DIR		0xC0
#define SET_DISP_OFFSET		0xD3
#define SET_COM_PIN_CFG		0xDA
#define SET_DISP_CLK_DIV	0xD5
#define SET_PRECHARGE		0xD9
#define SET_VCOM_DESEL		0xDB
#define SET_CHARGE_PUMP		0x8D

class SSD1306OLED : public aleopardstailDisplayDriver
{
	public:
		// class constructor, i2C address typically 0x3C
		SSD1306OLED(i2c_inst_t *i2c, uint SDA, uint SCL, uint8_t i2c_address);
		
		// call .begin() prior to any other calls, this sets up I2C and the associated pin functions,
		// allocates the screen buffer etc
		void begin(void);
		
		// this is the _only_ function that actually writes to the screen, call after drawing operations
		void display(void);
		
		// set the entire screen buffer to black (unilluminated pixels)
		void clearAll(void);
		
		// return the value of a given pixel
		uint8_t getPixel(int16_t X, int16_t Y);
		
		// set the value of a given pixel, returns its previous value
		uint8_t setPixel(int16_t X, int16_t Y, uint8_t V);
		
	private:
		i2c_inst_t *_i2c;
		uint _SDA;
		uint _SCL;
		uint8_t _i2c_address;
		uint8_t _height = 64;
		uint8_t _width = 128;
		
		uint16_t _displayBufferSize;			// size of display buffer, in bytes
		uint8_t *_displayBuffer;
		
		void write_cmd(uint8_t cmd);
};

#endif

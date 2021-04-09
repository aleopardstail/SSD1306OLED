#include "SSD1306OLED.h"

SSD1306OLED::SSD1306OLED(i2c_inst_t *i2c, uint SDA, uint SCL, uint8_t i2c_address)
{
	_i2c = i2c;
	_SDA = SDA;
	_SCL = SCL;
	_i2c_address = i2c_address;
	
	return;
}

void SSD1306OLED::begin(void)
{
	// initialise I2C
	i2c_init(_i2c, 100 * 1000);
	gpio_set_function(_SDA, GPIO_FUNC_I2C);
	gpio_set_function(_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(_SDA);
	gpio_pull_up(_SCL);
	
	// initialise the display
	uint8_t init_sequence[] = {
		SET_DISP | 0x00,
		SET_MEM_ADDR,
		0x00,
		SET_DISP_START_LINE | 0x00,
		SET_SEG_REMAP | 0x01,
		SET_MUX_RATIO,
		_height - 1,
		SET_COM_OUT_DIR | 0x08,
		SET_DISP_OFFSET,
		0x00,
		SET_DISP_CLK_DIV,
		0x80,
		SET_PRECHARGE,
		0xF1,
		SET_VCOM_DESEL,
		0x40,
		SET_CONTRAST,
		0xFF,
		SET_ENTIRE_ON,
		SET_NORM_INV,
		SET_CHARGE_PUMP,
		0x14,
		SET_DISP | 0x01
	};
	
	for (uint8_t i = 0; i <sizeof(init_sequence); i++)
	{
		write_cmd(init_sequence[i]);
	}
	
	// set up the display buffer
	_displayBufferSize = 1024 + 1;		// 128x64 mono display + byte for the initial instruction
	_displayBuffer = (uint8_t *)malloc(_displayBufferSize);
	
	_displayBuffer[0] = 0x40;		// initial instruction to write out
	for (uint16_t i = 1; i < _displayBufferSize; i++)
	{
		_displayBuffer[i] = 0b00000000;		// initial bit pattern on start up
	}
	
	display();	// actually write the display
	
	return;
}

// output the display buffer
void SSD1306OLED::display(void)
{
	write_cmd(SET_MEM_ADDR);
	write_cmd(0b01);
	
	write_cmd(SET_COL_ADDR);
	write_cmd(0);				// first column
	write_cmd(127);				// last column
	
	write_cmd(SET_PAGE_ADDR);
	write_cmd(0);				// first page
	write_cmd(7);				// last page
	
	i2c_write_blocking(_i2c, _i2c_address, _displayBuffer, _displayBufferSize, false);
	
	return;
}

void SSD1306OLED::clearAll(void)
{
		for (uint16_t i = 1; i < _displayBufferSize; i++)
	{
		_displayBuffer[i] = 0x00;		// all pixels off
	}
	return;
}

// returns value of a pixel
uint8_t SSD1306OLED::getPixel(int16_t X, int16_t Y)
{
	if (X < 0 || X >= _width || Y < 0 || Y >= _height)
	{
		// outside screen area
		return 0;
	}
	
	uint8_t page = Y/8;
	uint8_t bit = 1 << (Y % 8);
	uint8_t *ptr = _displayBuffer + X * 8 + page + 1;		// +1 to skip the initial command byte
	
	uint8_t current_byte = *ptr;
	uint8_t current_bit = (current_byte >> (Y % 8)) & 0x01;
	
	return current_bit;
}
			
// sets a pixel, returns previous value
uint8_t SSD1306OLED::setPixel(int16_t X, int16_t Y, uint8_t V)
{
	if (X < 0 || X > _width || Y < 0 || Y >= _height || X < _clipRect.x || X > (_clipRect.x + _clipRect.w) || Y < _clipRect.y || Y > (_clipRect.y + _clipRect.h))
	{
		// outside screen area
		return 0;
	}
	
	uint8_t page = Y/8;
	uint8_t bit = 1 << (Y % 8);
	uint8_t *ptr = _displayBuffer + X * 8 + page + 1;		// +1 to skip the initial command byte
	
	uint8_t current_byte = *ptr;
	uint8_t current_bit = (current_byte >> (Y % 8)) & 0x01;
	
    switch(_mode)
    {
      case MODE_SET:
      {
        // do nothing
        break;
      }
      case MODE_OR:
      {
        // logical OR with exisiting value
        V |= getPixel(X, Y);
        break;
      }
      case MODE_AND:
      {
        // logical AND with exisiting value
        V &= getPixel(X, Y);
        break;
      }
      case MODE_XOR:
      {
        V ^= getPixel(X, Y);
        break;
      }
      case MODE_NOT:
      {
        V = !V;
      }
    }
	
	if (V == 0)
	{
		*ptr &= ~bit;
	}
	else if (V == 1)
	{
		*ptr |= bit;
	}	
	
	
	return current_bit;
}



void SSD1306OLED::write_cmd(uint8_t cmd)
{
	uint8_t B[2];
	
	B[0] = 0x80;	// constant for sending a command
	B[1] = cmd;
	
	i2c_write_blocking(_i2c, _i2c_address, B, 2, false);
	return;
}

#ifndef DisplayDisplay_h
#define DisplayDisplay_h

#define MODE_SET  0   // set pixel directly to specified value
#define MODE_OR   1   // logical OR with pixel already there
#define MODE_AND  2   // logical AND with pixel already there
#define MODE_XOR  3   // logical XOR with pixel already there
#define MODE_NOT  4   // draw the logical inverse of whats specified

#include "pico/stdlib.h"
#include <stdlib.h>

#define PROGMEM			// defined as nothing, allows using Adafruit font files without editing them

// Adafruit font structures
/// Font data stored PER GLYPH
typedef struct {
  uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
  uint8_t width;         ///< Bitmap dimensions in pixels
  uint8_t height;        ///< Bitmap dimensions in pixels
  uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
  int8_t xOffset;        ///< X dist from cursor pos to UL corner
  int8_t yOffset;        ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct {
  uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
  GFXglyph *glyph;  ///< Glyph array
  uint16_t first;   ///< ASCII extents (first char)
  uint16_t last;    ///< ASCII extents (last char)
  uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;


// generic rectangle structure
typedef struct 
{
  int16_t x;
  int16_t y;
  uint8_t w;
  uint8_t h;
} RECT;

class aleopardstailDisplayDriver
{
	public:
		uint8_t getMode(void);
		uint8_t setMode(uint8_t Mode);
		
		// virtual functions derived classes must provide
		virtual uint8_t getPixel(int16_t X, int16_t Y) = 0;				// returns value of a pixel
		virtual uint8_t setPixel(int16_t X, int16_t Y, uint8_t V) = 0;	// sets a pixel, returns previous value
		virtual void begin(void) = 0;
		virtual void display(void) = 0;
		virtual void clearAll(void) = 0;
		
		// clear the display to pixel values of 0, respecting the clipping rectangle set
		void clear(void);
		
		// clipping functions, display maintains a clipping rectangle that can be adjusted and will void drawing operations
		// outside this rectangle
		void clearClip(void);											// reset clip to the default screen size
		void setClip(int16_t x, int16_t y, uint8_t w, uint8_t h);
		void setClip(RECT *R);
		void getClip(RECT *R);
		
		// generic drawing functions, all assume a single pixel wide pen
		void drawHLine(int16_t x, int16_t y, int16_t w, uint8_t Value);
		void drawVLine(int16_t x, int16_t y, int16_t h, uint8_t Value);
		void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t Value);
		void drawFilledRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Value);
		void drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Value);
		void drawCircle(int16_t x, int16_t y, int16_t r, uint8_t Value);
		void drawFilledCircle(int16_t x, int16_t y, int16_t r, uint8_t Value);
		
		// text functions
		GFXfont * getFont(void);
		GFXfont * setFont(const GFXfont *font);
		uint8_t getFontHeight(const GFXfont *font);
		int16_t drawGlyph(int16_t x, int16_t y, char c, uint8_t Value);
		int16_t drawString(int16_t x, int16_t y, const char c[], uint8_t Value);
		
	protected:
		uint8_t _mode = MODE_SET;
		
		RECT _clipRect = {0, 0, 127, 63};
		
		GFXfont *_currentFont = NULL;
	
};


#endif

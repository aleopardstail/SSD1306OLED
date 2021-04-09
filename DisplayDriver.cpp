
#include "DisplayDriver.h"

uint8_t aleopardstailDisplayDriver::getMode(void)
{
	return _mode;
}

uint8_t aleopardstailDisplayDriver::setMode(uint8_t Mode)
{
	_mode = Mode;
	
	return _mode;
}

void aleopardstailDisplayDriver::drawHLine(int16_t x, int16_t y, int16_t w, uint8_t Value)
{
  for (uint16_t p = x; p <= x+w; p++)
  {
    setPixel(p,y,Value);
  }
}

void aleopardstailDisplayDriver::drawVLine(int16_t x, int16_t y, int16_t h, uint8_t Value)
{
  for (uint16_t p = y; p <= y+h; p++)
  {
    setPixel(x,p,Value);
  }  
}

void aleopardstailDisplayDriver::drawFilledRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Value)
{
  for (uint16_t _y = y; _y <= y+h; _y++)
  {
    drawHLine(x,_y, w, Value);
  }
}

void aleopardstailDisplayDriver::drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Value)
{
  drawHLine(x,y,w-1, Value);
  drawHLine(x, y+h-1, w, Value);
  drawVLine(x,y,h-1,Value);
  drawVLine(x+w-1,y,h,Value);
}

void aleopardstailDisplayDriver::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t Value)
{
  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
    setPixel(x0,y0, Value);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }  
}

// draw circle, centred on x,y with radius r
void aleopardstailDisplayDriver::drawCircle(int16_t cx, int16_t cy, int16_t r, uint8_t Value)
{
	int16_t x=0,y,p;
	
	y=	r;
	p = 3 - (2 * r);
	
	while (x < y)
	{
		setPixel(cx+x, cy+y, Value);
		setPixel(cx-x, cy+y, Value);
		setPixel(cx+x, cy-y, Value);
		setPixel(cx-x, cy-y, Value);
		setPixel(cx+y, cy+x, Value);
		setPixel(cx-y, cy+x, Value);
		setPixel(cx+y, cy-x, Value);
		setPixel(cx-y, cy-x, Value);
		
		x++;
		if (p < 0)
		{
			p = p + 4 * x + 6;
		}
		else
		{
			y--;
			p = p + 4 * (x - y) + 10;
		}
		setPixel(cx+x, cy+y, Value);
		setPixel(cx-x, cy+y, Value);
		setPixel(cx+x, cy-y, Value);
		setPixel(cx-x, cy-y, Value);
		setPixel(cx+y, cy+x, Value);
		setPixel(cx-y, cy+x, Value);
		setPixel(cx+y, cy-x, Value);
		setPixel(cx-y, cy-x, Value);
		
	}
}

// draw circle, centred on x,y with radius r
void aleopardstailDisplayDriver::drawFilledCircle(int16_t cx, int16_t cy, int16_t r, uint8_t Value)
{
	int16_t x=0,y,p;
	
	y=	r;
	p = 3 - (2 * r);
	
	while (x < y)
	{
		drawLine(cx+x, cy+y, cx-x, cy+y, Value);
		drawLine(cx+x, cy-y, cx-x, cy-y, Value);
		drawLine(cx+y, cy+x, cx-y, cy+x, Value);
		drawLine(cx+y, cy-x, cx-y, cy-x, Value);
		
		x++;
		if (p < 0)
		{
			p = p + 4 * x + 6;
		}
		else
		{
			y--;
			p = p + 4 * (x - y) + 10;
		}
		drawLine(cx+x, cy+y, cx-x, cy+y, Value);
		drawLine(cx+x, cy-y, cx-x, cy-y, Value);
		drawLine(cx+y, cy+x, cx-y, cy+x, Value);
		drawLine(cx+y, cy-x, cx-y, cy-x, Value);
		
	}
}

GFXfont * aleopardstailDisplayDriver::getFont(void)
{
	return _currentFont;
}

GFXfont * aleopardstailDisplayDriver::setFont(const GFXfont *font)
{
	GFXfont *oldFont;
	
	_currentFont = (GFXfont *)font;
	
	return oldFont; 
}

int16_t aleopardstailDisplayDriver::drawGlyph(int16_t x, int16_t y, char c, uint8_t Value)
{
	if (_currentFont == NULL)
	{
		// if we don't have a font specified then abort
		return 0;
	}
	
	// we have a font, next we need to make sure it holds the ASCII character specified
	if ((c >= _currentFont->first) && (c <= _currentFont->last))
	{
		// the required character is present in the font
		// offset is the ASCII character, less the first character in the font
		GFXglyph *G = _currentFont->glyph + c - _currentFont->first;
		uint16_t baseOffset = G->bitmapOffset;		// offset into the font bitmap array
		uint8_t w = G->width;
		uint8_t h = G->height;
		int8_t xo = G->xOffset;
		int8_t yo = G->yOffset;
		uint8_t xx, yy, bits = 0, bit = 0;
		
		for (yy = 0; yy < h; yy++)
		{
			for (xx = 0; xx < w; xx++)
			{
				if (!(bit++ & 7))
				{
					bits = _currentFont->bitmap[baseOffset++]; 
				}
				if (bits & 0x80)
				{
					setPixel(x + xo + xx, y  + _currentFont->yAdvance + yo + yy, Value);
				}
				bits <<= 1;
			}
		}
		
		return G->xAdvance;
	}
	else
	{
		return 0;
	}
	
	return 0;
}

int16_t aleopardstailDisplayDriver::drawString(int16_t x, int16_t y, const char c[], uint8_t Value)
{
	
	uint8_t idx = 0;
	char C = c[idx];
	int16_t _x = x;
	
	while (C != '\0')
	{
		_x += drawGlyph(_x,y,C,Value);
		idx++;
		C = c[idx];
	}
	
	return _x;
}

uint8_t aleopardstailDisplayDriver::getFontHeight(const GFXfont *font)
{
	if (font != NULL)
	{
		return font->yAdvance;
	}
	
	return 0;
}

void aleopardstailDisplayDriver::clearClip(void)
{
  _clipRect.x = 0;
  _clipRect.y = 0;
  _clipRect.w = 127;
  _clipRect.h = 63;
}

void aleopardstailDisplayDriver::setClip(int16_t x, int16_t y, uint8_t w, uint8_t h)
{
  _clipRect.x = x;
  _clipRect.y = y;
  _clipRect.w = w;
  _clipRect.h = h;  
}

void aleopardstailDisplayDriver::setClip(RECT *R)
{
  _clipRect.x = R->x;
  _clipRect.y = R->y;
  _clipRect.w = R->w;
  _clipRect.h = R->h;  
}

void aleopardstailDisplayDriver::getClip(RECT *R)
{
	R->x = _clipRect.x;
	R->y = _clipRect.y;
	R->w = _clipRect.w;
	R->h = _clipRect.h;
	return;
}

void aleopardstailDisplayDriver::clear(void)
{
  uint8_t OldMode = setMode(MODE_SET);
  for (int16_t x = _clipRect.x; x <= (_clipRect.x + _clipRect.w); x++)
  {
    for (int16_t y = _clipRect.y; y <= (_clipRect.y + _clipRect.h); y++)
    {
      setPixel(x,y,0);
    }
  }
  setMode(OldMode);
}

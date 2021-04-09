# SSD1306OLED
a display driver for the SSD1306 OLED displays for the Raspberry Pi Pico over the i2c interface (work in progress)

this is a frame buffer display driver for the little SSD1306 monochrome OLED that can be cheaply sourced via eBay
currently just uses the i2c version (not the SPI versions) and is written for the 128x64 displays

this is managed via two sets of files, a slightly generic DisplayDriver which is created to be able to use the 
adafruit font format (a custom created 7x5 font is included, but this will load the others from Adafruit directly)
while also providing a range of basic drawing utilities, supporting a clipping rectangle and a full frame buffer
fr a flicker free display (all operations are in ram only, then written out with the .display() command)

to use this include both header files & the font header file, and add both source files to your project then instance 
the display and use thus

example

SSD1306OLED display(i2c1, 14, 15, 0x3C);

int main(void)
{
  display.begin();
  display.setFont(&f7x5font);
  display.drawString(0,0 - display.getFontHeight(display.getFont()), "Hello",1);
  display.display();
  
 return 0;
 }
  
 other commands are outlined in the header file and will be better documented at some point, but include
 
 .drawHLine(x,y,w,V);   // V = the pixel colour, 0 = off, 1 = on
 .drawLine(x0,y0,x1,y1,V);
 .clear();                // clears the frame buffer, respecting the clipping rectangle
 .setMode(mode);          // sets the drawing mode
 
 
 Drawing Modes
 this was implimented because it seemed useful, drawing can be managed in several logical modes
 
 MODE_SET     sets all pixels to the specified value directly (the default)
 MODE_OR      performs a logical OR with the current pixel value
 MODE_AND     perfrms a logical AND with the current pixel value
 MODE_XOR     performs a logical XOR with the current pixel value
 MODE_NOT     draws the logical inverse of the speficied value
 
 note "#define PROGMEM" is included to allow working with the adafruit font files directly
 
 using with other sized displays: this will require editing of the source files, I'm hoping to work further on
 this and support other sizes and types of display with the same generic code.

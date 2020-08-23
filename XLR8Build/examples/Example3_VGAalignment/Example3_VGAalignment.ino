/*
  VGA Alignment

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: August 14th 2020

  The video memory is implememnted as two dual-port RAM blocks:
  one for the character codes and one for the color attributes.

  The full 8-bit VGA character set is supported.
  See: https://en.wikipedia.org/wiki/Code_page_437
  
  Attribute data is 8-bit: [7] = Blink, [6:4] = Background Color, [3:0] = Foreground Color
  Set bit 7 to make the character blink.
  See: https://en.wikipedia.org/wiki/Video_Graphics_Array#Color_palette
  Background RGB colors are:
    000: 000000 Black
    001: 0000AA Blue
    010: 00AA00 Green
    011: 00AAAA Cyan
    100: AA0000 Red
    101: AA00AA Magenta
    110: AA5500 Brown
    111: AAAAAA Gray
  Foreground colors are:
    0000: 000000 Black
    0001: 0000AA Blue
    0010: 00AA00 Green
    0011: 00AAAA Cyan
    0100: AA0000 Red
    0101: AA00AA Magenta
    0110: AA5500 Brown
    0111: AAAAAA Gray
    1000: 555555 Dark Gray
    1001: 5555FF Bright Blue
    1010: 55FF55 Bright Green
    1011: 55FFFF Bright Cyan
    1100: FF5555 Bright Red
    1101: FF55FF Bright Magenta
    1110: FFFF55 Yellow
    1111: FFFFFF White

  640x480 pixels with 8x16 pixels per character = 80x30 chars = 2400 bytes for character codes plus the same again for attributes
  The cx and cy counters in the Verilog are both 10 bit
  We don't care about bits cx[2:0] and cy[3:0]
  To keep things easy, we use 13 bit addressing ({cy[9:4], cx[9:3]}) = 8K of dual port RAM

  The screen area starts at: cx = 160; cy = 45
  hdmi_demo.sv removes the cx and cy offset
  So the first full character (top left) appears at RAM address 0
  Each row in memory is 2^7 bytes wide = 128
  The last character on the first visible row occupies RAM address 79
  The first character on the second row occupies RAM address 128

  print_at and attr_at use true column and row numbering:
  print_at(column, row, character)
  print_at(0, 0, n) will print character n at the first character on the first line (top left)
  print_at(1, 0, n) will print at the second character on the first line
  print_at(0, 1, n) will print at the first character on the second line

*/

#include "XLR8_HDMI.h"

#define first_char_addr 0
#define row_offset 128
#define num_rows 30
#define num_columns 80

void setup()
{
  //Set the volume attenuation for the sawtooth sound in the hdmi_demo
  //0 = no attenuation (this will be REALLY loud!)
  //9 = sensible attenuation
  //16 = maximum attenuation (mute)
  XLR8_HDMI.set_volume_attenuation(16); // Set the volume attenuation

  clear_video_memory(); // Clear the whole video memory

  // Print the double border
  // Corners
  print_at(0,0,201);
  print_at(79,0,187);
  print_at(79,29,188);
  print_at(0,29,200);
  // Top and bottom edges
  for (int col = 1; col < 79; col++)
  {
    print_at(col,0,205);
    print_at(col,29,205);
  }
  // Left and right edges
  for (int row = 1; row < 29; row++)
  {
    print_at(0,row,186);
    print_at(79,row,186);
  }
  
  // Print the single border (skip one column and one row)
  // Corners
  print_at(1,1,218);
  print_at(78,1,191);
  print_at(78,28,217);
  print_at(1,28,192);
  // Top and bottom edges
  for (int col = 2; col < 78; col++)
  {
    print_at(col,1,196);
    print_at(col,28,196);
  }
  // Left and right edges
  for (int row = 2; row < 28; row++)
  {
    print_at(1,row,179);
    print_at(78,row,179);
  }
}

void loop()
{
  // Nothing to do here
}

void clear_video_memory()
{
  // Clear the whole of the video memory: set the character codes to 0x20 (space) and the attributes to 0x0F (white text on black background)
  // (The character and attribute RAM blocks are 8K in size but in reality only 2400 bytes are needed)
  for (int addr = 0; addr < 8192; addr++)
  {
		XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
		XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
		XLR8_HDMI.set_char_data(0x20); // Set the character code to 0x20 (space)
		XLR8_HDMI.set_attr_data(0x0F); // Set the attribute to 0x0F (white text on black background)
  }
}

void clear_screen()
{
  // Clear the visible characters - leave the attributes unchanged
  for (int row = 0; row < num_rows; row++)
  {
    for (int column = 0; column < num_columns; column++)
    {
      int addr = first_char_addr + (row * row_offset) + column;
      XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
      XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
      XLR8_HDMI.set_char_data(0x20); // Set the character code to 0x20 (space)
    }
  }
}

void print_at(int column, int row, uint8_t chr) // Print chr at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  XLR8_HDMI.set_char_data(chr); // Set the character code
}

void attr_at(int column, int row, uint8_t attr) // Change the attributes at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  XLR8_HDMI.set_attr_data(attr); // Set the character attributes
}

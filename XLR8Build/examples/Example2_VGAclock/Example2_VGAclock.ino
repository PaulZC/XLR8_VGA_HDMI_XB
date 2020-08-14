/*
  VGA Clock

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
  So the first full character (top left) appears at the RAM address for column 20 row 3
  Each row in memory is 2^7 bytes wide = 128
  So the first fully-visible character occupies RAM address (20 + (3 * 128)) = 404
  The last character on the first visible row occupies RAM address (404 + 79) = 483
  The first character on the second row occupies RAM address (404 + 128) = 532

*/

#include "XLR8_HDMI.h"

#define first_char_addr 404
#define row_offset 128
#define num_rows 30
#define num_columns 80

void setup()
{
  Serial.begin(115200);
  Serial.println("HDMI VGA Clock Demo");

  //Set the volume attenuation for the sawtooth sound in the hdmi_demo
  //0 = no attenuation (this will be REALLY loud!)
  //9 = sensible attenuation
  //16 = maximum attenuation (mute)
  XLR8_HDMI.set_volume_attenuation(16); // Set the volume attenuation

  clear_video_memory(); // Clear the whole video memory

  // Print the border
  // Corners
  print_at(35,13,201);
  print_at(46,13,187);
  print_at(46,17,188);
  print_at(35,17,200);
  // Top and bottom edges
  for (int col = 36; col < 46; col++)
  {
    print_at(col,13,205);
    print_at(col,17,205);
  }
  // Left and right edges
  for (int row = 14; row < 17; row++)
  {
    print_at(35,row,186);
    print_at(46,row,186);
  }
}

void loop()
{
  unsigned long now = millis(); // Get the current time in millis
  uint8_t hour, min, sec;
  int time = (now / 1000) % (24 * 3600); // Convert time into hour,min,sec
  hour = (time / 3600) % 24;
  min = (time / 60) % 60;
  sec = time % 60;

  // Update the displayed time
  print_at(37,15,('0'+(hour/10))); // Display hours
  print_at(38,15,('0'+(hour%10)));
  print_at(39,15,':');
  print_at(40,15,('0'+(min/10))); // Display mins
  print_at(41,15,('0'+(min%10)));
  print_at(42,15,':');
  print_at(43,15,('0'+(sec/10))); // Display sec
  print_at(44,15,('0'+(sec%10)));

  // Change the color of the border
  int change_char = (now % 1000) / 33; // There are 30 characters in the border so update one every 1/30 sec = 33ms
  uint8_t attr = (sec % 10) + 1; // Change the color every second (add 1 to avoid black text)
  if (change_char == 0) attr_at(41,13,attr);
  else if (change_char == 1) attr_at(42,13,attr);
  else if (change_char == 2) attr_at(43,13,attr);
  else if (change_char == 3) attr_at(44,13,attr);
  else if (change_char == 4) attr_at(45,13,attr);
  else if (change_char == 5) attr_at(46,13,attr);
  else if (change_char == 6) attr_at(46,14,attr);
  else if (change_char == 7) attr_at(46,15,attr);
  else if (change_char == 8) attr_at(46,16,attr);
  else if (change_char == 9) attr_at(46,17,attr);
  else if (change_char == 10) attr_at(45,17,attr);
  else if (change_char == 11) attr_at(44,17,attr);
  else if (change_char == 12) attr_at(43,17,attr);
  else if (change_char == 13) attr_at(42,17,attr);
  else if (change_char == 14) attr_at(41,17,attr);
  else if (change_char == 15) attr_at(40,17,attr);
  else if (change_char == 16) attr_at(39,17,attr);
  else if (change_char == 17) attr_at(38,17,attr);
  else if (change_char == 18) attr_at(37,17,attr);
  else if (change_char == 19) attr_at(36,17,attr);
  else if (change_char == 20) attr_at(35,17,attr);
  else if (change_char == 21) attr_at(35,16,attr);
  else if (change_char == 22) attr_at(35,15,attr);
  else if (change_char == 23) attr_at(35,14,attr);
  else if (change_char == 24) attr_at(35,13,attr);
  else if (change_char == 25) attr_at(36,13,attr);
  else if (change_char == 26) attr_at(37,13,attr);
  else if (change_char == 27) attr_at(38,13,attr);
  else if (change_char == 28) attr_at(39,13,attr);
  else attr_at(40,13,attr);
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

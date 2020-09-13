/*--------------------------------------------------------------------
 Copyright (c) 2016 Alorium Technology.  All right reserved.
 The XLR8Build/XLR8BuildTemplate library provides a path
   to build FPGA images within the Arduino Framework. There
   isn't any C/C++ code associated with this library but
   this empty header file prevents the Arduino IDE from
   calling it an invalid library
 Written by Matt Weber (support@aloriumtech.com) of
 Alorium Technology (info@aloriumtech.com)


 This library is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as
 published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library.  If not, see
 <http://www.gnu.org/licenses/>.
 --------------------------------------------------------------------*/

 /*

 This Xcelerator Block will run on the Alorium Technology Snō FPGA board and will produce a configurable VGA text display over HDMI at 640x480 pixels (60Hz).
 The video memory is implemented as two dual-port RAM blocks: one for the character codes and one for the color attributes.

 The dual-port video memory was added by Paul.

 The Verilog for the HDMI video, audio and VGA text generator is a remix of the HDL Utils written by Sameer Puri:
   - https://github.com/hdl-util
   - https://github.com/hdl-util/hdmi-demo
   - https://github.com/hdl-util/vga-text-mode
   - https://github.com/hdl-util/sound
   - https://github.com/hdl-util/hdmi

 Sameer Puri's HDL Utils are dually-licensed under the MIT and Apache 2.0 licenses

 The parts written by Paul are licensed under the beerware license:
   - This code and connection guide is public domain but you buy me a beer if you use it and we meet someday

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

 set_char_at and set_attr_at use true column and row numbering:
 set_char_at(column, row, character)
 set_char_at(0, 0, n) will print character n at the first character on the first line (top left)
 set_char_at(1, 0, n) will print at the second character on the first line
 set_char_at(0, 1, n) will print at the first character on the second line

 The row offset is zero by default. The character displayed in the top left corner is read from RAM address 0.
 By setting the row offset to 1. The character displayed in the top left corner is read from RAM address 128 instead.
 This provides the illusion that the rows have all been shifted up by one instantaneously.
 Think of the row offset as a pointer to the RAM address for the top row of the screen.

 The video memory is large enough to allow a row offset of 32. So you could implement two display 'frames' or 'pages',
 the first starting at row 0 and the second starting at row 30 or 32, and switch between them instantaneously.

 */

#include "XLR8_HDMI.h"

// Clear the video memory
// Reset the row_offset
boolean XLR8_HDMI::begin(void)
{
  clear_video_memory(); // Clear the whole video memory
  current_column = 0; // Reset the current_column
  set_row_offset(32); // Set the row offset to a non-zero value
  if (get_row_offset() != 32) // If setting the row_offset failed, return false
    return (false);
  reset_row_offset(); // Set the row_offset to zero
  return (true);
}

// Set the audio volume attenuation
// 0 = no attenuation (LOUD!)
// 9 = sensible attenuation
// 16 = maximum attenuation (mute)
void XLR8_HDMI::set_volume_attenuation(uint8_t val) {
  XLR8_HDMI_VOLUME = val;
}

// Clear the whole video memory
void XLR8_HDMI::clear_video_memory(void)
{
  // Clear the whole of the video memory: set the character codes to 0x20 (space) and the attributes to 0x0F (white text on black background)
  // (The character and attribute RAM blocks are 8K in size
  for (int addr = 0; addr < 8192; addr++)
  {
    set_char_addr_hi(addr >> 8); // Set up the address
    set_char_addr_lo(addr & 0xFF);
    set_char_data(0x20); // Set the character code to 0x20 (space)
    set_attr_data(0x0F); // Set the attribute to 0x0F (white text on black background)
  }
}

// Clear the visible characters - leave the attributes unchanged
void XLR8_HDMI::clear_screen(void)
{
  // Clear the visible characters - leave the attributes unchanged
  for (int row = 0; row < XLR8_HDMI_NUM_ROWS; row++)
  {
    for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
    {
      int addr = XLR8_HDMI_FIRST_CHAR_ADDR + (row * XLR8_HDMI_ROW_LENGTH) + column;
      set_char_addr_hi(addr >> 8); // Set up the address
      set_char_addr_lo(addr & 0xFF);
      set_char_data(0x20); // Set the character code to 0x20 (space)
    }
  }
}

// Print Flash Helper String
void XLR8_HDMI::vga_print(const __FlashStringHelper *ifsh)
{
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  uint8_t offset = get_row_offset(); // Get the row offset
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    else if (c == 0x0D) // CR
    {
      current_column = 0; // Reset current_column
    }
    else if (c == 0x0A) // LF
    {
      vga_lf(); // Shift up by one line, don't reset current_column
      offset = get_row_offset(); // Update the row offset
    }
    else
    {
      set_char_at(current_column, offset + XLR8_HDMI_NUM_ROWS - 1, c); // Print the character at the current_column on the bottom row
      current_column++;
    }
  }
}

void XLR8_HDMI::vga_print(const String &s)
{
  int len = s.length();
  if (len == 0) return;
  uint8_t offset = get_row_offset(); // Get the row offset
  const char* payload = s.c_str();
  for (int i = 0; i < len; i++)
  {
    if (payload[i] == 0x0D) // CR
    {
      current_column = 0; // Reset current_column
    }
    else if (payload[i] == 0x0A) // LF
    {
      vga_lf(); // Shift up by one line, don't reset current_column
      offset = get_row_offset(); // Update the row offset
    }
    else
    {
      set_char_at(current_column, offset + XLR8_HDMI_NUM_ROWS - 1, payload[i]); // Print the character at the current_column on the bottom row
      current_column++;
    }
  }
}

void XLR8_HDMI::vga_print(char c)
{
  uint8_t offset = get_row_offset(); // Get the row offset
  if (c == 0x0D) // CR
  {
    current_column = 0; // Reset current_column
  }
  else if (c == 0x0A) // LF
  {
    vga_lf(); // Shift up by one line, don't reset current_column
    offset = get_row_offset(); // Update the row offset
  }
  else
  {
    set_char_at(current_column, offset + XLR8_HDMI_NUM_ROWS - 1, c); // Print the character at the current_column on the bottom row
    current_column++;
  }
}

void XLR8_HDMI::vga_print(unsigned char b, int base)
{
  vga_print((unsigned long) b, base);
}

void XLR8_HDMI::vga_print(int n, int base)
{
  vga_print((long) n, base);
}

void XLR8_HDMI::vga_print(unsigned int n, int base)
{
  vga_print((unsigned long) n, base);
}

// Print Unsigned Long
void XLR8_HDMI::vga_print(unsigned long n, int base)
{
  uint8_t offset = get_row_offset(); // Get the row offset

  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];
  *str = '\0';

  if (base < 2) base = 10;

  do {
    char c = n % base;
    n /= base;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while (n);

  while (*str)
  {
    char c = *str;
    set_char_at(current_column, offset + XLR8_HDMI_NUM_ROWS - 1, c);
    current_column++;
    str++;
  }
}

void XLR8_HDMI::vga_print(long n, int base)
{
  uint8_t offset = get_row_offset(); // Get the row offset

  if ((base == 10) && (n < 0))
  {
    set_char_at(current_column, offset + XLR8_HDMI_NUM_ROWS - 1, '-');
    current_column++;
  }

  if (n < 0)
  {
    n = -n;
  }

  vga_print((unsigned long)n, base);
}

// 'Move' the display up by one row (by incrementing the row_offset) and clear the next row
void XLR8_HDMI::vga_println(void)
{
  fast_vertical_shift();
  uint8_t offset = get_row_offset(); // Get the row offset
  for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
  {
    set_char_at(column, offset + XLR8_HDMI_NUM_ROWS - 1, 0x20); // Fill the next row with spaces
  }
  current_column = 0; // Reset current_column
}

// Line Feed:
// 'Move' the display up by one row (by incrementing the row_offset) and clear the next row
// Don't reset current_column
void XLR8_HDMI::vga_lf(void)
{
  fast_vertical_shift();
  uint8_t offset = get_row_offset(); // Get the row offset
  for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
  {
    set_char_at(column, offset + XLR8_HDMI_NUM_ROWS - 1, 0x20); // Fill the next row with spaces
  }
}

void XLR8_HDMI::vga_print(double number, int digits)
{
  if (isnan(number))
  {
    vga_print("nan");
    return;
  }
  if (isinf(number))
  {
    vga_print("inf");
    return;
  }
  if (number > 4294967040.0)
  {
    vga_print("ovf");  // constant determined empirically
    return;
  }
  if (number <-4294967040.0)
  {
    vga_print("ovf");  // constant determined empirically
    return;
  }

  // Handle negative numbers
  if (number < 0.0)
  {
     vga_print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  vga_print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    vga_print('.');
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    unsigned int toPrint = (unsigned int)(remainder);
    vga_print(toPrint);
    remainder -= toPrint;
  }
}

void XLR8_HDMI::vga_println(const __FlashStringHelper *ifsh)
{
  vga_print(ifsh);
  vga_println();
}

void XLR8_HDMI::vga_println(const String &s)
{
  vga_print(s);
  vga_println();
}

void XLR8_HDMI::vga_println(char c)
{
  vga_print(c);
  vga_println();
}

void XLR8_HDMI::vga_println(unsigned char b, int base)
{
  vga_print(b, base);
  vga_println();
}

void XLR8_HDMI::vga_println(int num, int base)
{
  vga_print(num, base);
  vga_println();
}

void XLR8_HDMI::vga_println(unsigned int num, int base)
{
  vga_print(num, base);
  vga_println();
}

void XLR8_HDMI::vga_println(long num, int base)
{
  vga_print(num, base);
  vga_println();
}

void XLR8_HDMI::vga_println(unsigned long num, int base)
{
  vga_print(num, base);
  vga_println();
}

void XLR8_HDMI::vga_println(double number, int digits)
{
  vga_print(number, digits);
  vga_println();
}

// Appear to shift both characters and attributes up by one row by incrementing the row offset
void XLR8_HDMI::fast_vertical_shift(void)
{
  uint8_t offset = get_row_offset(); // Get the row offset
  offset++; // Increment the row offset by one
  set_row_offset(offset); // Set the row offset
}

// Shift both characters and attributes up by one row using video memory reads and writes
void XLR8_HDMI::vertical_shift(void)
{
  for (int row = 1; row < (XLR8_HDMI_NUM_ROWS + 1); row++)
  {
    for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
    {
      int addr = XLR8_HDMI_FIRST_CHAR_ADDR + (row * XLR8_HDMI_ROW_LENGTH) + column;
      set_char_addr_hi(addr >> 8); // Set up the address
      set_char_addr_lo(addr & 0xFF);
      uint8_t chr = get_char_data(); // Read the character
      uint8_t attr = get_attr_data(); // Read the attribute
      addr = addr - XLR8_HDMI_ROW_LENGTH; // Subtract the row offset - move up by one row
      set_char_addr_hi(addr >> 8); // Change the address
      set_char_addr_lo(addr & 0xFF);
      set_char_data(chr); // Set the character code
      set_attr_data(attr); // Set the character attributes
    }
  }
}

// Shift the characters up by one row using video memory reads and writes
void XLR8_HDMI::vertical_shift_char_only(void)
{
  for (int row = 1; row < (XLR8_HDMI_NUM_ROWS + 1); row++)
  {
    for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
    {
      int addr = XLR8_HDMI_FIRST_CHAR_ADDR + (row * XLR8_HDMI_ROW_LENGTH) + column;
      set_char_addr_hi(addr >> 8); // Set up the address
      set_char_addr_lo(addr & 0xFF);
      uint8_t chr = get_char_data(); // Read the character
      addr = addr - XLR8_HDMI_ROW_LENGTH; // Subtract the row offset - move up by one row
      set_char_addr_hi(addr >> 8); // Change the address
      set_char_addr_lo(addr & 0xFF);
      set_char_data(chr); // Set the character code
    }
  }
}

// Reset the row offset so that the character displayed in the top left corner is read from RAM address 0
void XLR8_HDMI::reset_row_offset(void)
{
  set_row_offset(0); // Set the row offset to zero
}

// Change the character at (column,row) to chr
void XLR8_HDMI::set_char_at(int column, int row, uint8_t chr)
{
  int addr = XLR8_HDMI_FIRST_CHAR_ADDR + (row * XLR8_HDMI_ROW_LENGTH) + column;
  set_char_addr_hi(addr >> 8); // Set up the address
  set_char_addr_lo(addr & 0xFF);
  set_char_data(chr); // Set the character code
}

// Read the character at (column,row)
uint8_t XLR8_HDMI::get_char_at(int column, int row)
{
  int addr = XLR8_HDMI_FIRST_CHAR_ADDR + (row * XLR8_HDMI_ROW_LENGTH) + column;
  set_char_addr_hi(addr >> 8); // Set up the address
  set_char_addr_lo(addr & 0xFF);
  return(get_char_data()); // Read and return the character code
}

// Change the attributes at (column,row)
void XLR8_HDMI::set_attr_at(int column, int row, uint8_t attr)
{
  int addr = XLR8_HDMI_FIRST_CHAR_ADDR + (row * XLR8_HDMI_ROW_LENGTH) + column;
  set_char_addr_hi(addr >> 8); // Set up the address
  set_char_addr_lo(addr & 0xFF);
  set_attr_data(attr); // Set the character attributes
}

// Read the attributes at (column,row)
uint8_t XLR8_HDMI::get_attr_at(int column, int row)
{
  int addr = XLR8_HDMI_FIRST_CHAR_ADDR + (row * XLR8_HDMI_ROW_LENGTH) + column;
  set_char_addr_hi(addr >> 8); // Set up the address
  set_char_addr_lo(addr & 0xFF);
  return(get_attr_data()); // Read and return the attribute
}

// Set the Lo Byte of the RAM 2-PORT video memory address
void XLR8_HDMI::set_char_addr_lo(uint8_t val) {
 XLR8_HDMI_CHAR_ADDR_LO = val;
}

// Set the Hi Byte of the RAM 2-PORT video memory address
void XLR8_HDMI::set_char_addr_hi(uint8_t val) {
 XLR8_HDMI_CHAR_ADDR_HI = val;
}

// Load the video memory address with character val
void XLR8_HDMI::set_char_data(uint8_t val) {
 XLR8_HDMI_CHAR_DATA = val;
}

// Load the video memory address with the attribute val
void XLR8_HDMI::set_attr_data(uint8_t val) {
 XLR8_HDMI_ATTR_DATA = val;
}

// Read the character val back from video memory
uint8_t XLR8_HDMI::get_char_data() {
  return XLR8_HDMI_CHAR_DATA;
}

// Read the attribute val back from video memory
uint8_t XLR8_HDMI::get_attr_data() {
  return XLR8_HDMI_ATTR_DATA;
}

// Set the video memory row offset to val
void XLR8_HDMI::set_row_offset(uint8_t val) {
 XLR8_HDMI_ROW_OFFSET = val;
}

// Read the video memory row offset
uint8_t XLR8_HDMI::get_row_offset() {
  return XLR8_HDMI_ROW_OFFSET;
}

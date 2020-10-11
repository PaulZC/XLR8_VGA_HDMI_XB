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

 The sound generation is very primitive:
   Frequencies from 8Hz to 2040Hz can be generated in 8Hz increments.
	Sound durations can be 0.0625s to 15.9375 seconds in 0.0625s increments.

 */

#include "XLR8_HDMI.h"

// Clear the video memory
// Reset the row_offset
boolean XLR8_HDMI::begin(void)
{
  set_volume_attenuation(16); // Mute the sound
  set_sound_duration(0); // Disable any ongoing sound
  current_attr = 0x0F; // Default to white text on black background
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

uint8_t XLR8_HDMI::get_volume_attenuation() {
  return XLR8_HDMI_VOLUME;
}

// Clear the whole video memory
void XLR8_HDMI::clear_video_memory(uint8_t attr)
{
  // Clear the whole of the video memory: set the character codes to 0x20 (space) and reset the attributes
  // (The character and attribute RAM blocks are 8K in size)
  for (int addr = 0; addr < 8192; addr++)
  {
    set_char_addr_hi(addr >> 8); // Set up the address
    set_char_addr_lo(addr & 0xFF);
    set_char_data(0x20); // Set the character code to 0x20 (space)
    set_attr_data(attr); // Set the attribute
  }
}

// Clear the _visible_ characters - leave the attributes unchanged
void XLR8_HDMI::clear_screen(void)
{
  uint8_t offset = get_row_offset(); // Get the row offset
  // Clear the visible characters - leave the attributes unchanged
  for (int row = 0; row < XLR8_HDMI_NUM_ROWS; row++)
  {
    for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
    {
      int addr = XLR8_HDMI_FIRST_CHAR_ADDR + ((offset + row) * XLR8_HDMI_ROW_LENGTH) + column;
      set_char_addr_hi(addr >> 8); // Set up the address
      set_char_addr_lo(addr & 0xFF);
      set_char_data(0x20); // Set the character code to 0x20 (space)
    }
  }
}

// HDMI VGA write
size_t XLR8_HDMI::write(uint8_t c)
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
    set_char_attr_at(current_column, offset + XLR8_HDMI_NUM_ROWS - 1, c, current_attr); // Print the character at the current_column on the bottom row
    current_column++;
  }
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
    set_char_attr_at(column, offset + XLR8_HDMI_NUM_ROWS - 1, 0x20, current_attr); // Fill the next row with spaces
  }
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
  uint8_t offset = get_row_offset(); // Get the row offset
  for (int row = 1; row < (XLR8_HDMI_NUM_ROWS + 1); row++)
  {
    for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
    {
      int addr = XLR8_HDMI_FIRST_CHAR_ADDR + ((offset + row) * XLR8_HDMI_ROW_LENGTH) + column;
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
  uint8_t offset = get_row_offset(); // Get the row offset
  for (int row = 1; row < (XLR8_HDMI_NUM_ROWS + 1); row++)
  {
    for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
    {
      int addr = XLR8_HDMI_FIRST_CHAR_ADDR + ((offset + row) * XLR8_HDMI_ROW_LENGTH) + column;
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

// Change the character and attributes at (column,row) to chr and attr
// (This is quicker than doing set_char_at followed by set_attr_at)
void XLR8_HDMI::set_char_attr_at(int column, int row, uint8_t chr, uint8_t attr)
{
  int addr = XLR8_HDMI_FIRST_CHAR_ADDR + (row * XLR8_HDMI_ROW_LENGTH) + column;
  set_char_addr_hi(addr >> 8); // Set up the address
  set_char_addr_lo(addr & 0xFF);
  set_char_data(chr); // Set the character code
  set_attr_data(attr); // Set the character attributes
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

// Set the sound frequency
// freq is in increments of 8Hz
// i.e. the XB can generate frequencies from 0Hz to 2048Hz
void XLR8_HDMI::set_sound_freq(uint8_t freq) {
  XLR8_HDMI_WAVE_RATE = freq;
}

// Get the sound frequency
uint8_t XLR8_HDMI::get_sound_freq(void) {
  return XLR8_HDMI_WAVE_RATE;
}

// Set the sound duration
// duration is in increments of 62.5ms
// i.e. the XB can generate sound durations of 0.0625 to 16 seconds
void XLR8_HDMI::set_sound_duration(uint8_t duration) {
  XLR8_HDMI_WAVE_DURATION = duration;
}

// Sound In Progress
// Returns true if a sound is in progress
boolean XLR8_HDMI::sound_in_progress() {
  return (XLR8_HDMI_WAVE_DURATION == 1);
}

// Beep: a tribute to the ZX Spectrum BEEP statement:
// https://worldofspectrum.org/ZXBasicManual/zxmanchap19.html
// duration (float) is the beep duration in seconds;
// pitch (int) is given in semitones above middle C, in the range +/- 35.
// Beep is blocking - it returns when the sound is complete.
void XLR8_HDMI::beep(float duration, int pitch)
{
  // Convert pitch (in semitones) into Hz
  // https://pages.mtu.edu/~suits/notefreqs.html
  float frequency;
  switch (pitch)
  {
    case -35:
      frequency = 34.65; // C#1/Db1
      break;
    case -34:
      frequency = 36.71; // D1
      break;
    case -33:
      frequency = 38.89; // D#1/Eb1
      break;
    case -32:
      frequency = 41.20; // E1
      break;
    case -31:
      frequency = 43.65; // F1
      break;
    case -30:
      frequency = 46.25; // F#1/Gb1
      break;
    case -29:
      frequency = 49.00; // G1
      break;
    case -28:
      frequency = 51.91; // G#1/Ab1
      break;
    case -27:
      frequency = 55.00; // A1
      break;
    case -26:
      frequency = 58.27; // A#1/Bb1
      break;
    case -25:
      frequency = 61.74; // B1
      break;
    case -24:
      frequency = 65.41; // C2
      break;
    case -23:
      frequency = 69.30; // C#2/Db2
      break;
    case -22:
      frequency = 73.42; // D2
      break;
    case -21:
      frequency = 77.78; // D#2/Eb2
      break;
    case -20:
      frequency = 82.41; // E2
      break;
    case -19:
      frequency = 87.31; // F2
      break;
    case -18:
      frequency = 92.50; // F#2/Gb2
      break;
    case -17:
      frequency = 98.00; // G2
      break;
    case -16:
      frequency = 103.83; // G#2/Ab2
      break;
    case -15:
      frequency = 110.00; // A2
      break;
    case -14:
      frequency = 116.54; // A#2/Bb2
      break;
    case -13:
      frequency = 123.47; // B2
      break;
    case -12:
      frequency = 130.81; // C3
      break;
    case -11:
      frequency = 138.59; // C#3/Db3
      break;
    case -10:
      frequency = 146.83; // D3
      break;
    case -9:
      frequency = 155.56; // D#3/Eb3
      break;
    case -8:
      frequency = 164.81; // E3
      break;
    case -7:
      frequency = 174.61; // F3
      break;
    case -6:
      frequency = 185.00; // F#3/Gb3
      break;
    case -5:
      frequency = 196.00; // G3
      break;
    case -4:
      frequency = 207.65; // G#3/Ab3
      break;
    case -3:
      frequency = 220.00; // A3
      break;
    case -2:
      frequency = 233.08; // A#3/Bb3
      break;
    case -1:
      frequency = 246.94; // B3
      break;
    case 0:
      frequency = 261.63; // C4
      break;
    case 1:
      frequency = 277.18; // C#4/Db4
      break;
    case 2:
      frequency = 293.66; // D4
      break;
    case 3:
      frequency = 311.13; // D#4/Eb4
      break;
    case 4:
      frequency = 329.63; // E4
      break;
    case 5:
      frequency = 349.23; // F4
      break;
    case 6:
      frequency = 369.99; // F#4/Gb4
      break;
    case 7:
      frequency = 392.00; // G4
      break;
    case 8:
      frequency = 415.30; // G#4/Ab4
      break;
    case 9:
      frequency = 440.00; // A4
      break;
    case 10:
      frequency = 466.16; //
      break;
    case 11:
      frequency = 493.88; // B4
      break;
    case 12:
      frequency = 523.25; // C5
      break;
    case 13:
      frequency = 554.37; // C#5/Db5
      break;
    case 14:
      frequency = 587.33; // D5
      break;
    case 15:
      frequency = 622.25; // D#5/Eb5
      break;
    case 16:
      frequency = 659.25; // E5
      break;
    case 17:
      frequency = 698.46; // F5
      break;
    case 18:
      frequency = 739.99; // F#5/Gb5
      break;
    case 19:
      frequency = 783.99; // G5
      break;
    case 20:
      frequency = 830.61; // G#5/Ab5
      break;
    case 21:
      frequency = 880.00; // A5
      break;
    case 22:
      frequency = 932.33; // A#5/Bb5
      break;
    case 23:
      frequency = 987.77; // B5
      break;
    case 24:
      frequency = 1046.50; // C6
      break;
    case 25:
      frequency = 1108.73; // C#6/Db6
      break;
    case 26:
      frequency = 1174.66; // D6
      break;
    case 27:
      frequency = 1244.51; // D#6/Eb6
      break;
    case 28:
      frequency = 1318.51; // E6
      break;
    case 29:
      frequency = 1396.91; // F6
      break;
    case 30:
      frequency = 1479.98; // F#6/Gb6
      break;
    case 31:
      frequency = 1567.98; // G6
      break;
    case 32:
      frequency = 1661.22; // G#6/Ab6
      break;
    case 33:
      frequency = 1760.00; // A6
      break;
    case 34:
      frequency = 1864.66; // A#6/Bb6
      break;
    case 35:
      frequency = 1975.53; // B6
      break;
    default:
      frequency = 261.63; // C4
      break;
    }

    set_sound_freq(frequency / 8); // Convert frequency

    if (duration < 0)
      duration = 0;
    if (duration > 15.9375) // 255 * 0.0625
      duration = 15.9375;
    set_sound_duration(duration / 0.0625); // Convert duration

    while(sound_in_progress())
      delay(5); // Beep is blocking - it returns when the sound is complete
}

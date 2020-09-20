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

#ifndef _XLR8_HDMI_H_INCLUDED
#define _XLR8_HDMI_H_INCLUDED

#include <Arduino.h>

#include <Print.h> // Inherit the print functions

#define XLR8_HDMI_VOLUME    	  _SFR_MEM8(0xE0)
#define XLR8_HDMI_CHAR_ADDR_LO  _SFR_MEM8(0xE1)
#define XLR8_HDMI_CHAR_ADDR_HI  _SFR_MEM8(0xE2)
#define XLR8_HDMI_CHAR_DATA     _SFR_MEM8(0xE3)
#define XLR8_HDMI_ATTR_DATA     _SFR_MEM8(0xE4)
#define XLR8_HDMI_ROW_OFFSET    _SFR_MEM8(0xE5)
#define XLR8_HDMI_WAVE_RATE     _SFR_MEM8(0xE6)
#define XLR8_HDMI_WAVE_DURATION _SFR_MEM8(0xE7)

#define XLR8_HDMI_FIRST_CHAR_ADDR 0
#define XLR8_HDMI_ROW_LENGTH      128
#define XLR8_HDMI_NUM_ROWS        30
#define XLR8_HDMI_NUM_COLUMNS     80

class XLR8_HDMI : public Print {

public:

  uint8_t current_column; // Column used by the print functions. You can change this to implement e.g. delete or backspace.
  uint8_t current_attr; // Character attribute used by the print functions.

  // Returns true if video memory is detected
  boolean begin(void);

  // Set the audio volume attenuation
  // 0 = no attenuation (LOUD!)
  // 9 = sensible attenuation
  // 16 = maximum attenuation (mute)
  void __attribute__ ((noinline)) set_volume_attenuation(uint8_t val = 16);

  // Read the current volume attenuation
  uint8_t __attribute__ ((noinline)) get_volume_attenuation();

  // Clear the whole video memory. Default to white text on black background.
  void clear_video_memory(uint8_t attr = 0x0F);

  // Clear the visible characters - leave the attributes unchanged
  void clear_screen(void);

  using Print::print; // Inherit the print functions
  using Print::println; // Inherit the print functions

  // HDMI VGA write
  size_t write(uint8_t) override;   // Overriding base functionality

  // Appear to shift both characters and attributes up by one row by incrementing the row offset
  void fast_vertical_shift(void);

  // Shift both characters and attributes up by one row using video memory reads and writes
  void vertical_shift(void);

  // Shift the characters up by one row using video memory reads and writes
  void vertical_shift_char_only(void);

  // Reset the row offset so that the character displayed in the top left corner is read from RAM address 0
  void reset_row_offset(void);

  // Set the video memory row offset to val
  void __attribute__ ((noinline)) set_row_offset(uint8_t val);

  // Read the video memory row offset
  uint8_t __attribute__ ((noinline)) get_row_offset();

  // Change the character and attributes at (column,row) to chr and attr
  void set_char_attr_at(int column, int row, uint8_t chr, uint8_t attr);

  // Change the character at (column,row) to chr
  void set_char_at(int column, int row, uint8_t chr);

  // Read the character at (column,row)
  uint8_t get_char_at(int column, int row);

  // Change the attributes at (column,row)
  void set_attr_at(int column, int row, uint8_t attr);

  // Read the attributes at (column,row)
  uint8_t get_attr_at(int column, int row);
  
  // Set the sound frequency
  // freq is in increments of 8Hz
  // i.e. the XB can generate frequencies from 0Hz to 2048Hz
  void __attribute__ ((noinline)) set_sound_freq(uint8_t freq);

  // Get the sound frequency
  uint8_t __attribute__ ((noinline)) get_sound_freq();

  // Set the sound duration
  // duration is in increments of 62.5ms
  // i.e. the XB can generate sound durations of 0.0625 to 16 seconds
  void __attribute__ ((noinline)) set_sound_duration(uint8_t duration);

  // Sound In Progress
  // Returns true if a sound is in progress
  boolean __attribute__ ((noinline)) sound_in_progress();

private:

  // Set the Lo Byte of the RAM 2-PORT video memory address
  void __attribute__ ((noinline)) set_char_addr_lo(uint8_t val);

  // Set the Hi Byte of the RAM 2-PORT video memory address
  void __attribute__ ((noinline)) set_char_addr_hi(uint8_t val);

  // Load the video memory address with character val
  void __attribute__ ((noinline)) set_char_data(uint8_t val);

  // Load the video memory address with the attribute val
  void __attribute__ ((noinline)) set_attr_data(uint8_t val);

  // Read the character val back from video memory
  uint8_t __attribute__ ((noinline)) get_char_data();

  // Read the attribute val back from video memory
  uint8_t __attribute__ ((noinline)) get_attr_data();

  // Line Feed
  void vga_lf(void);

};

#endif

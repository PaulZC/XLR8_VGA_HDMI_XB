/*
Copyright (c) 2018 Tobias Blum . All right reserved.

Tetris Matrix Clock

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

/*

  The XLR8 HDMI VGA Text functions were added by PaulZC
  
  The video memory is implemented as two dual-port RAM blocks:
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

#ifndef TetrisMatrixDraw_h
#define TetrisMatrixDraw_h

#include <Arduino.h>

#include "XLR8_HDMI.h"

#define TETRIS_MAX_NUMBERS 11 // Was 9. The XLR8 HDMI VGA display has 80 columns so we can fit in 11 numbers

#define TETRIS_DISTANCE_BETWEEN_DIGITS 7

// Type that describes the current state of a drawn number
typedef struct
{
  int num_to_draw; // Number to draw (0-9)
  int blockindex;  // The index of the brick (as defined in the falling instructions) that is currently falling
  int fallindex;   // y-position of the brick it already has (incrementing with each step)
  int x_shift;     // x-position of the number relative to the matrix where the number should be placed.
} numstate;

class TetrisMatrixDraw
{
    public:
        TetrisMatrixDraw ();
        bool drawNumbers(int x = 0, int y = 0, bool displayColon = false, bool blinkingColon = false);
        //bool drawText(int x = 0, int y = 0);
        //void drawChar(String letter, uint8_t x, uint8_t y, uint16_t color);
        void drawShape(int blocktype, uint16_t color, int x_pos, int y_pos, int num_rot);
        void drawLargerShape(int scale, int blocktype, uint16_t color, int x_pos, int y_pos, int num_rot);
        void setTime(String time, bool forceRefresh = false);
        void setNumbers(int value, bool forceRefresh = false);
        //void setText(String txt, bool forceRefresh = false);
        void setNumState(int index, int value, int x_shift);
        void drawColon(int x, int y, uint16_t colonColour, bool blinking = false);
        int calculateWidth();
        bool _debug = false;
        int scale = 1;
        uint16_t outLineColour = 0x0000;

        XLR8_HDMI tetrisHDMI;

        uint16_t tetrisColors[9];
        uint16_t tetrisRED;
        uint16_t tetrisGREEN;
        uint16_t tetrisBLUE;
        uint16_t tetrisWHITE;
        uint16_t tetrisYELLOW;
        uint16_t tetrisCYAN;
        uint16_t tetrisMAGENTA;
        uint16_t tetrisORANGE;
        uint16_t tetrisBLACK;

        const uint8_t brick_char = 219;

        void clear_screen(void);

    private:
        void intialiseColors();
        void resetNumStates();
        void drawLargerBlock(int x_pos, int y_pos, int scale, uint16_t color);
        numstate numstates[TETRIS_MAX_NUMBERS];
        int sizeOfValue;

        // Replace the GFX display functionality
        void drawPixel(int x_pos, int y_pos, uint16_t color, bool blinking = false);
};

#endif

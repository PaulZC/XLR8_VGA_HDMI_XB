/*
  Conway's Game Of Life for the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: August 16th 2020

  The Rules:
  
  For a cell that is 'populated':
    Each cell with one or no neighbors dies, as if by solitude.
    Each cell with four or more neighbors dies, as if by overpopulation.
    Each cell with two or three neighbors survives.
  For a cell that is 'empty' or 'unpopulated':
    Each cell with three neighbors becomes populated.
  
  A live cell contains the smiley face and has attribute 0x0F (white text)
  A cell about to die contains the smiley face and has attribute 0x07 (gray text)
  A cell about to spawn contains a space and has attribute 0x08 (dark gray text)
  An empty/dead cell contains a space and has attribute 0x00 (black text)
  
  With thanks to: https://bitstorm.org/gameoflife/
  
  The version of the game coded here wraps the botttom edge to the top
  and the left edge to the right. This unfortunately means the Gosper Glider Gun
  ends up shooting itself!

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

  spawn uses true column and row numbering:
  spawn(column, row)
  spawn(0, 0) will spawn the first cell on the first line (top left)
  spawn(1, 0) will spawn the second cell on the first line
  spawn(0, 1) will spawn the first cell on the second line

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

  // Glider
  spawn(2, 1);
  spawn(3, 2);
  spawn(1, 3);
  spawn(2, 3);
  spawn(3, 3);

//  // Exploder
//  spawn(38, 14);
//  spawn(38, 15);
//  spawn(38, 16);
//  spawn(38, 17);
//  spawn(38, 18);
//  spawn(40, 14);
//  spawn(40, 18);
//  spawn(42, 14);
//  spawn(42, 15);
//  spawn(42, 16);
//  spawn(42, 17);
//  spawn(42, 18);

//  // Lightweight Spaceship
//  spawn(2,14);
//  spawn(3,14);
//  spawn(4,14);
//  spawn(5,14);
//  spawn(1,15);
//  spawn(5,15);
//  spawn(5,16);
//  spawn(1,17);
//  spawn(4,17);

//  // Gosper Glider Gun
//  spawn(1, 10);
//  spawn(1, 11);
//  spawn(2, 10);
//  spawn(2, 11);
//  spawn(35, 8);
//  spawn(35, 9);
//  spawn(36, 8);
//  spawn(36, 9);
//  spawn(9, 11);
//  spawn(9, 12);
//  spawn(10, 10);
//  spawn(10, 12);
//  spawn(11, 10);
//  spawn(11, 11);
//  spawn(23, 9);
//  spawn(23, 10);
//  spawn(24, 8);
//  spawn(24, 10);
//  spawn(25, 8);
//  spawn(25, 9);
//  spawn(17, 12);
//  spawn(17, 13);
//  spawn(17, 14);
//  spawn(18, 12);
//  spawn(19, 13);
//  spawn(36, 15);
//  spawn(36, 16);
//  spawn(36, 17);
//  spawn(37, 15);
//  spawn(38, 16);
//  spawn(25, 20);
//  spawn(25, 21);
//  spawn(26, 20);
//  spawn(26, 22);
//  spawn(27, 20);

}

void loop()
{
  // loop through each cell and count how many neighbours it has
  for (int y = 0; y < num_rows; y++) { // for each row
    for (int x = 0; x < num_columns; x++) { // for each column
      int neighbours = 0; // reset the number of neighbours
      for (int yn=y-1; yn<=y+1; yn++) { // for each of the neighbouring rows
        int ynw = yn;
        if (ynw == -1) ynw = num_rows - 1; // handle wrap-around for top edge
        if (ynw == num_rows) ynw = 0; // handle wrap-around for bottom edge
        for (int xn=x-1; xn<=x+1; xn++) { // for each of the neighbouring columns
          int xnw = xn;
          if (xnw == -1) xnw = num_columns - 1; // handle wrap-around for left edge
          if (xnw == num_columns) xnw = 0; // handle wrap-around for right edge
          if (get_char_at(xnw,ynw) != 0x20) { // count the neighbours (include dying neighbours)
            neighbours++;
          }
        }
      }
      // check if live cell is lonely or overcrowded
      // (the number of neighbours includes the cell itself)
      if ((get_attr_at(x,y) == 0x0F) && ((neighbours <= 2) || (neighbours >= 5))) {
        dying(x,y); // mark cell as about to die
      }
      // check if empty cell should spawn
      if ((get_char_at(x,y) == 0x20) && (neighbours == 3)) {
        spawning(x,y); // mark cell as about to spawn
      }
    }
  }
  // loop through the cells again and update dying and spawning cells
  for (int y = 0; y < num_rows; y++) { // for each row
    for (int x = 0; x < num_columns; x++) { // for each column
      if (get_attr_at(x,y) == 0x07) dead(x,y); // Make dying cell dead
      if (get_attr_at(x,y) == 0x08) spawn(x,y); // Make spawning cell live
    }
  }
}

void spawn(int column, int row)
{
  // spawn a cell at (column, row)
  print_at(column, row, 1); // Place a smiley face at (column, row)
  attr_at(column, row, 0x0F); // Make the cell 'healthy' (white text on black background)
}

void dying(int column, int row)
{
  // mark the cell at (column, row) as dying
  attr_at(column, row, 0x07); // Mark the cell as dying (grey text on black background)
}

void spawning(int column, int row)
{
  // mark the cell at (column, row) as about to spawn
  attr_at(column, row, 0x08); // Mark the cell as about to spawn (dark grey text on black background)
}

void dead(int column, int row)
{
  // kill off the cell at (column, row)
  print_at(column, row, 0x20); // Place a space at (column, row)
  attr_at(column, row, 0x00); // Mark the cell as dead (black text on black background)
}

void clear_video_memory()
{
  // Clear the whole of the video memory: set the character codes to 0x20 (space) and the attributes to 0x00 (black text on black background)
  // (The character and attribute RAM blocks are 8K in size but in reality only 2400 bytes are needed)
  for (int addr = 0; addr < 8192; addr++)
  {
		XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
		XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
		XLR8_HDMI.set_char_data(0x20); // Set the character code to 0x20 (space)
		XLR8_HDMI.set_attr_data(0x00); // Set the attribute to 0x00 (black text on black background)
  }
}

void print_at(int column, int row, uint8_t chr) // Print chr at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  XLR8_HDMI.set_char_data(chr); // Set the character code
}

uint8_t get_char_at(int column, int row) // Read the chr at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  return(XLR8_HDMI.get_char_data()); // Read and return the character code
}

void attr_at(int column, int row, uint8_t attr) // Change the attributes at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  XLR8_HDMI.set_attr_data(attr); // Set the character attributes
}

uint8_t get_attr_at(int column, int row) // Read the attr at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  return(XLR8_HDMI.get_attr_data()); // Read and return the attribute
}

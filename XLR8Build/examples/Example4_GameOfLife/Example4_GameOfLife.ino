/*
  Conway's Game Of Life for the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

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

  spawn uses true column and row numbering:
  spawn(column, row)
  spawn(0, 0) will spawn the first cell on the first line (top left)
  spawn(1, 0) will spawn the second cell on the first line
  spawn(0, 1) will spawn the first cell on the second line

*/

#include "XLR8_HDMI.h"

XLR8_HDMI myHDMI;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("HDMI VGA Demo: Conway's Game of Life"));

  if (myHDMI.begin() == false)
  {
    Serial.println(F("HDMI XB did not begin! Freezing..."));
    while(1);
  }

//  // Glider
//  spawn(2, 1);
//  spawn(3, 2);
//  spawn(1, 3);
//  spawn(2, 3);
//  spawn(3, 3);

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

  // Gosper Glider Gun
  spawn(1, 10);
  spawn(1, 11);
  spawn(2, 10);
  spawn(2, 11);
  spawn(35, 8);
  spawn(35, 9);
  spawn(36, 8);
  spawn(36, 9);
  spawn(9, 11);
  spawn(9, 12);
  spawn(10, 10);
  spawn(10, 12);
  spawn(11, 10);
  spawn(11, 11);
  spawn(23, 9);
  spawn(23, 10);
  spawn(24, 8);
  spawn(24, 10);
  spawn(25, 8);
  spawn(25, 9);
  spawn(17, 12);
  spawn(17, 13);
  spawn(17, 14);
  spawn(18, 12);
  spawn(19, 13);
  spawn(36, 15);
  spawn(36, 16);
  spawn(36, 17);
  spawn(37, 15);
  spawn(38, 16);
  spawn(25, 20);
  spawn(25, 21);
  spawn(26, 20);
  spawn(26, 22);
  spawn(27, 20);

}

void loop()
{
  // loop through each cell and count how many neighbours it has
  for (int y = 0; y < XLR8_HDMI_NUM_ROWS; y++) { // for each row
    for (int x = 0; x < XLR8_HDMI_NUM_COLUMNS; x++) { // for each column
      int neighbours = 0; // reset the number of neighbours
      for (int yn=y-1; yn<=y+1; yn++) { // for each of the neighbouring rows
        int ynw = yn;
        if (ynw == -1) ynw = XLR8_HDMI_NUM_ROWS - 1; // handle wrap-around for top edge
        if (ynw == XLR8_HDMI_NUM_ROWS) ynw = 0; // handle wrap-around for bottom edge
        for (int xn=x-1; xn<=x+1; xn++) { // for each of the neighbouring columns
          int xnw = xn;
          if (xnw == -1) xnw = XLR8_HDMI_NUM_COLUMNS - 1; // handle wrap-around for left edge
          if (xnw == XLR8_HDMI_NUM_COLUMNS) xnw = 0; // handle wrap-around for right edge
          if (myHDMI.get_char_at(xnw,ynw) != 0x20) { // count the neighbours (include dying neighbours)
            neighbours++;
          }
        }
      }
      // check if live cell is lonely or overcrowded
      // (the number of neighbours includes the cell itself)
      if ((myHDMI.get_attr_at(x,y) == 0x0F) && ((neighbours <= 2) || (neighbours >= 5))) {
        dying(x,y); // mark cell as about to die
      }
      // check if empty cell should spawn
      if ((myHDMI.get_char_at(x,y) == 0x20) && (neighbours == 3)) {
        spawning(x,y); // mark cell as about to spawn
      }
    }
  }
  // loop through the cells again and update dying and spawning cells
  for (int y = 0; y < XLR8_HDMI_NUM_ROWS; y++) { // for each row
    for (int x = 0; x < XLR8_HDMI_NUM_COLUMNS; x++) { // for each column
      if (myHDMI.get_attr_at(x,y) == 0x07) dead(x,y); // Make dying cell dead
      if (myHDMI.get_attr_at(x,y) == 0x08) spawn(x,y); // Make spawning cell live
    }
  }
}

void spawn(int column, int row)
{
  // spawn a cell at (column, row)
  myHDMI.set_char_at(column, row, 1); // Place a smiley face at (column, row)
  myHDMI.set_attr_at(column, row, 0x0F); // Make the cell 'healthy' (white text on black background)
}

void dying(int column, int row)
{
  // mark the cell at (column, row) as dying
  myHDMI.set_attr_at(column, row, 0x07); // Mark the cell as dying (grey text on black background)
}

void spawning(int column, int row)
{
  // mark the cell at (column, row) as about to spawn
  myHDMI.set_attr_at(column, row, 0x08); // Mark the cell as about to spawn (dark grey text on black background)
}

void dead(int column, int row)
{
  // kill off the cell at (column, row)
  myHDMI.set_char_at(column, row, 0x20); // Place a space at (column, row)
  myHDMI.set_attr_at(column, row, 0x00); // Mark the cell as dead (black text on black background)
}

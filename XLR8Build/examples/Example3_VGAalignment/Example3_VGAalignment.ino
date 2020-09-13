/*
  VGA Alignment

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

  set_char_at and set_attr_at use true column and row numbering:
  set_char_at(column, row, character)
  set_char_at(0, 0, n) will print character n at the first character on the first line (top left)
  set_char_at(1, 0, n) will print at the second character on the first line
  set_char_at(0, 1, n) will print at the first character on the second line

  The display is XLR8_HDMI_NUM_COLUMNS * XLR8_HDMI_NUM_ROWS in size (80 * 30)

*/

#include "XLR8_HDMI.h"

XLR8_HDMI myHDMI;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("HDMI VGA Alignment"));

  if (myHDMI.begin() == false)
  {
    Serial.println(F("HDMI XB did not begin! Freezing..."));
    while(1);
  }

  // Print the double border
  // Corners
  myHDMI.set_char_at(0, 0, 201);
  myHDMI.set_char_at(XLR8_HDMI_NUM_COLUMNS - 1, 0, 187);
  myHDMI.set_char_at(XLR8_HDMI_NUM_COLUMNS - 1, XLR8_HDMI_NUM_ROWS - 1, 188);
  myHDMI.set_char_at(0, XLR8_HDMI_NUM_ROWS - 1, 200);
  // Top and bottom edges
  for (int col = 1; col < XLR8_HDMI_NUM_COLUMNS - 1; col++)
  {
    myHDMI.set_char_at(col, 0, 205);
    myHDMI.set_char_at(col, XLR8_HDMI_NUM_ROWS - 1, 205);
  }
  // Left and right edges
  for (int row = 1; row < XLR8_HDMI_NUM_ROWS - 1; row++)
  {
    myHDMI.set_char_at(0, row, 186);
    myHDMI.set_char_at(XLR8_HDMI_NUM_COLUMNS - 1, row, 186);
  }
  
  // Print the single border (inset by one column and one row)
  // Corners
  myHDMI.set_char_at(1, 1, 218);
  myHDMI.set_char_at(XLR8_HDMI_NUM_COLUMNS - 2, 1, 191);
  myHDMI.set_char_at(XLR8_HDMI_NUM_COLUMNS - 2, XLR8_HDMI_NUM_ROWS - 2, 217);
  myHDMI.set_char_at(1, XLR8_HDMI_NUM_ROWS - 2, 192);
  // Top and bottom edges
  for (int col = 2; col < XLR8_HDMI_NUM_COLUMNS - 2; col++)
  {
    myHDMI.set_char_at(col, 1, 196);
    myHDMI.set_char_at(col,XLR8_HDMI_NUM_ROWS - 2,196);
  }
  // Left and right edges
  for (int row = 2; row < XLR8_HDMI_NUM_ROWS - 2; row++)
  {
    myHDMI.set_char_at(1, row, 179);
    myHDMI.set_char_at(XLR8_HDMI_NUM_COLUMNS - 2, row, 179);
  }
}

void loop()
{
  // Nothing to do here
}

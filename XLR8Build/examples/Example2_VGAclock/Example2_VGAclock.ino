/*
  VGA Clock

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

  set_char_at and set_attr_at use true column and row numbering:
  set_char_at(column, row, character)
  set_char_at(0, 0, n) will print character n at the first character on the first line (top left)
  set_char_at(1, 0, n) will print at the second character on the first line
  set_char_at(0, 1, n) will print at the first character on the second line

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

*/

#include "XLR8_HDMI.h"

XLR8_HDMI myHDMI;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("HDMI VGA Clock Demo"));

  if (myHDMI.begin() == false)
  {
    Serial.println(F("HDMI XB did not begin! Freezing..."));
    while(1);
  }

  // Print the border
  // Corners
  myHDMI.set_char_at(35,13,201);
  myHDMI.set_char_at(46,13,187);
  myHDMI.set_char_at(46,17,188);
  myHDMI.set_char_at(35,17,200);
  // Top and bottom edges
  for (int col = 36; col < 46; col++)
  {
    myHDMI.set_char_at(col,13,205);
    myHDMI.set_char_at(col,17,205);
  }
  // Left and right edges
  for (int row = 14; row < 17; row++)
  {
    myHDMI.set_char_at(35,row,186);
    myHDMI.set_char_at(46,row,186);
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
  myHDMI.set_char_at(37,15,('0'+(hour/10))); // Display hours
  myHDMI.set_char_at(38,15,('0'+(hour%10)));
  myHDMI.set_char_at(39,15,':');
  myHDMI.set_char_at(40,15,('0'+(min/10))); // Display mins
  myHDMI.set_char_at(41,15,('0'+(min%10)));
  myHDMI.set_char_at(42,15,':');
  myHDMI.set_char_at(43,15,('0'+(sec/10))); // Display sec
  myHDMI.set_char_at(44,15,('0'+(sec%10)));

  // Change the color of the border
  int change_char = (now % 1000) / 33; // There are 30 characters in the border so update one every 1/30 sec = 33ms
  uint8_t attr = (sec % 10) + 1; // Change the color every second (add 1 to avoid black text)
  if (change_char == 0) myHDMI.set_attr_at(41,13,attr);
  else if (change_char == 1) myHDMI.set_attr_at(42,13,attr);
  else if (change_char == 2) myHDMI.set_attr_at(43,13,attr);
  else if (change_char == 3) myHDMI.set_attr_at(44,13,attr);
  else if (change_char == 4) myHDMI.set_attr_at(45,13,attr);
  else if (change_char == 5) myHDMI.set_attr_at(46,13,attr);
  else if (change_char == 6) myHDMI.set_attr_at(46,14,attr);
  else if (change_char == 7) myHDMI.set_attr_at(46,15,attr);
  else if (change_char == 8) myHDMI.set_attr_at(46,16,attr);
  else if (change_char == 9) myHDMI.set_attr_at(46,17,attr);
  else if (change_char == 10) myHDMI.set_attr_at(45,17,attr);
  else if (change_char == 11) myHDMI.set_attr_at(44,17,attr);
  else if (change_char == 12) myHDMI.set_attr_at(43,17,attr);
  else if (change_char == 13) myHDMI.set_attr_at(42,17,attr);
  else if (change_char == 14) myHDMI.set_attr_at(41,17,attr);
  else if (change_char == 15) myHDMI.set_attr_at(40,17,attr);
  else if (change_char == 16) myHDMI.set_attr_at(39,17,attr);
  else if (change_char == 17) myHDMI.set_attr_at(38,17,attr);
  else if (change_char == 18) myHDMI.set_attr_at(37,17,attr);
  else if (change_char == 19) myHDMI.set_attr_at(36,17,attr);
  else if (change_char == 20) myHDMI.set_attr_at(35,17,attr);
  else if (change_char == 21) myHDMI.set_attr_at(35,16,attr);
  else if (change_char == 22) myHDMI.set_attr_at(35,15,attr);
  else if (change_char == 23) myHDMI.set_attr_at(35,14,attr);
  else if (change_char == 24) myHDMI.set_attr_at(35,13,attr);
  else if (change_char == 25) myHDMI.set_attr_at(36,13,attr);
  else if (change_char == 26) myHDMI.set_attr_at(37,13,attr);
  else if (change_char == 27) myHDMI.set_attr_at(38,13,attr);
  else if (change_char == 28) myHDMI.set_attr_at(39,13,attr);
  else myHDMI.set_attr_at(40,13,attr);
}

/*
  
  HDMI print demo

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

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
  Serial.println(F("HDMI VGA print example"));

  if (myHDMI.begin() == false)
  {
    Serial.println(F("HDMI XB did not begin! Freezing..."));
    while(1);
  }  

  for (uint8_t fg = 0; fg <= 0x0F; fg++) // Cycle through the 16 foreground colors
  {
    for (uint8_t bg = 0; bg <= 0x07; bg++) // Cycle through the 8 background colors
    {
      myHDMI.current_attr = (bg << 4) | fg; // Set the current attribute
      myHDMI.print("0123456789");
    }
    myHDMI.println();
  }
  for (uint8_t fg = 0; fg <= 0x0D; fg++) // Cycle through 14 foreground colors
  {
    for (uint8_t bg = 0; bg <= 0x07; bg++) // Cycle through the 8 background colors
    {
      myHDMI.current_attr = 0x80 | (bg << 4) | fg; // Set the current attribute and make it blink
      myHDMI.print("0123456789");
    }
    if (fg < 0x0D) myHDMI.println(); // Skip println on the last line
  }
}

void loop()
{
  // Nothing to do here
}

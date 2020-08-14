/*
  HDMI Test Pattern Generator

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: August 11th 2020

  With the 'on' values set to 0xFF and the 'off' values set to 0, the Sno
  will generate a 640x480 HDMI test pattern of vertical colored bars:
  white; cyan; magenta, blue, yellow, green, red, black

  By setting the red 'on' value to 0, the red component will disappear leaving:
  cyan, blue, green, black

*/

#include "XLR8_HDMI.h"

void setup()
{

  XLR8_HDMI.set_red_on_pixel_val(0xFF);     // Define the red component for pixels which are 'on'
  XLR8_HDMI.set_red_off_pixel_val(0x00);    // Define the red component for pixels which are 'off'
  XLR8_HDMI.set_green_on_pixel_val(0xFF);   // Define the green component for pixels which are 'on'
  XLR8_HDMI.set_green_off_pixel_val(0x00);  // Define the green component for pixels which are 'off'
  XLR8_HDMI.set_blue_on_pixel_val(0xFF);    // Define the blue component for pixels which are 'on'
  XLR8_HDMI.set_blue_off_pixel_val(0x00);   // Define the blue component for pixels which are 'off'

}

void loop()
{
  // Nothing to do here...
}

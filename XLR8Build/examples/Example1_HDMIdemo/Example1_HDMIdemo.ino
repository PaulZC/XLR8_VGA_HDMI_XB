/*
  HDMI Demo

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: August 14th 2020

  Set the volume attenuation for the sawtooth sound in the hdmi_demo
  0 = no attenuation (this will be REALLY loud!)
  9 = sensible attenuation
  16 = maximum attenuation (mute)
  
  Random VGA text and attributes are written to the whole of the video memory
  The full 8-bit VGA character set is supported
  Attribute data is 8-bit: [7] = Blink, [6:4] = Background Color, [3:0] = Foreground Color

*/

#include "XLR8_HDMI.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("HDMI VGA Text Demo");

  XLR8_HDMI.set_volume_attenuation(9); // Set the volume attenuation
  
}

void loop()
{

  // Fill the whole of the video memory with random characters and attributes
  // (The character and attribute RAM blocks are 8K in size but in reality only 2400 bytes are needed)
  for (int addr = 0; addr < 8192; addr++)
  {
		XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
		XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
		XLR8_HDMI.set_char_data(random(255)); // Write a random character
		XLR8_HDMI.set_attr_data(random(255)); // Write a random attribute
		//Serial.println(XLR8_HDMI.get_char_data()); // Read the character back
  }

}

/*
  
  HDMI print demo

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

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

  // Thanks to: https://ascii.co.uk/art/aardvark
  // The single \ chars need to be converted to \\ so they will display correctly
  
  myHDMI.println(F("                 ,"));
  myHDMI.println(F("           (`.  : \\               __..----..__"));
  myHDMI.println(F("            `.`.| |:          _,-':::''' '  `:`-._"));
  myHDMI.println(F("              `.:\\||       _,':::::'         `::::`-."));
  myHDMI.println(F("                \\\\`|    _,':::::::'     `:.     `':::`."));
  myHDMI.println(F("                 ;` `-''  `::::::.                  `::\\"));
  myHDMI.println(F("              ,-'      .::'  `:::::.         `::..    `:\\"));
  myHDMI.println(F("            ,' /_) -.            `::.           `:.     |"));
  myHDMI.println(F("          ,'.:     `    `:.        `:.     .::.          \\"));
  myHDMI.println(F("     __,-'   ___,..-''-.  `:.        `.   /::::.         |"));
  myHDMI.println(F("    |):'_,--'           `.    `::..       |::::::.      ::\\"));
  myHDMI.println(F("     `-'                 |`--.:_::::|_____\\::::::::.__  ::|"));
  myHDMI.println(F("                         |   _/|::::|      \\::::::|::/\\  :|"));
  myHDMI.println(F("                         /:./  |:::/        \\__:::):/  \\  :\\"));
  myHDMI.println(F("                       ,'::'  /:::|        ,'::::/_/    `. ``-.__"));
  myHDMI.println(F("         jrei         ''''   (//|/\\      ,';':,-'         `-.__  `'--..__"));
  myHDMI.println(F("                                                               `''---::::'"));
  myHDMI.println();
  myHDMI.println();
  myHDMI.println();
  myHDMI.println();
  myHDMI.println();
}

void loop()
{
  // Nothing to do here
}

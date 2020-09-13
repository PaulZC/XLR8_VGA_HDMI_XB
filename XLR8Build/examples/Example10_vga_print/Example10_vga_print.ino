/*
  
  HDMI vga_print demo

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

*/

#include "XLR8_HDMI.h"

XLR8_HDMI myHDMI;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("HDMI VGA vga_print example"));

  if (myHDMI.begin() == false)
  {
    Serial.println(F("HDMI XB did not begin! Freezing..."));
    while(1);
  }  
  
  myHDMI.vga_print(F("HDMI VGA vga_print example"));
  myHDMI.vga_println();

  myHDMI.vga_print(F("Flash Helper Text"));
  myHDMI.vga_println();

  myHDMI.vga_print(F("long, base 10: "));
  long long_num = 123456789;
  myHDMI.vga_print(long_num);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("long, base 10, negative: "));
  long_num = -123456789;
  myHDMI.vga_print(long_num);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("long, base 16: "));
  long_num = 65535;
  myHDMI.vga_print(long_num, 16);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("long, base 2: "));
  long_num = 65535;
  myHDMI.vga_print(long_num, 2);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("long, base 16, negative: "));
  long_num = -65535;
  myHDMI.vga_print(long_num, 16);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("long, base 2, negative: "));
  long_num = -65535;
  myHDMI.vga_print(long_num, 2);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("string: "));
  myHDMI.vga_print("Hello boys!");
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("string: "));
  String str = "Hello boys!";
  myHDMI.vga_print(str);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("long converted to string: "));
  myHDMI.vga_print(String(long_num));
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("float converted to string: "));
  float floater = 1.23456;
  myHDMI.vga_print(String(floater, 6));
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("CR\rLF\nCRLF\r\nEnd"));
  myHDMI.vga_println();
  
  myHDMI.vga_print("CR\rLF\nCRLF\r\nEnd");
  myHDMI.vga_println();
  
  myHDMI.vga_print((char)'A');
  myHDMI.vga_print((char)0x0D);
  myHDMI.vga_print((char)'B');
  myHDMI.vga_print((char)0x0A);
  myHDMI.vga_print((char)'C');
  myHDMI.vga_print((char)0x0D);
  myHDMI.vga_print((char)0x0A);
  myHDMI.vga_print(F("End"));
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("unsigned long, base 10: "));
  unsigned long ul_num = 123456789;
  myHDMI.vga_print(ul_num);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("unsigned long, base 16: "));
  ul_num = 65535;
  myHDMI.vga_print(ul_num, 16);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("float: "));
  myHDMI.vga_print(floater);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("float: "));
  myHDMI.vga_print(floater, 6);
  myHDMI.vga_println();

  myHDMI.vga_print(F("float, rounded: "));
  floater = 1.999;
  myHDMI.vga_print(floater, 2);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("float, negative: "));
  floater = -1.999;
  myHDMI.vga_print(floater, 2);
  myHDMI.vga_println();
  

  delay(5000);
  
  
  myHDMI.vga_println(F("Flash Helper Text"));

  myHDMI.vga_print(F("long, base 10: "));
  long_num = 123456789;
  myHDMI.vga_println(long_num);
  
  myHDMI.vga_print(F("long, base 10, negative: "));
  long_num = -123456789;
  myHDMI.vga_println(long_num);
  
  myHDMI.vga_print(F("long, base 16: "));
  long_num = 65535;
  myHDMI.vga_println(long_num, 16);
  
  myHDMI.vga_print(F("long, base 2: "));
  long_num = 65535;
  myHDMI.vga_println(long_num, 2);
  
  myHDMI.vga_print(F("long, base 16, negative: "));
  long_num = -65535;
  myHDMI.vga_print(long_num, 16);
  myHDMI.vga_println();
  
  myHDMI.vga_print(F("long, base 2, negative: "));
  long_num = -65535;
  myHDMI.vga_println(long_num, 2);
  
  myHDMI.vga_print(F("string: "));
  myHDMI.vga_println("Hello boys!");
  
  myHDMI.vga_print(F("string: "));
  str = "Hello boys!";
  myHDMI.vga_println(str);
  
  myHDMI.vga_print(F("long converted to string: "));
  myHDMI.vga_println(String(long_num));
  
  myHDMI.vga_print(F("float converted to string: "));
  floater = 1.23456;
  myHDMI.vga_println(String(floater, 6));
  
  myHDMI.vga_println(F("CR\rLF\nCRLF\r\nEnd"));
  
  myHDMI.vga_println("CR\rLF\nCRLF\r\nEnd");
  
  myHDMI.vga_print((char)'A');
  myHDMI.vga_print((char)0x0D);
  myHDMI.vga_print((char)'B');
  myHDMI.vga_print((char)0x0A);
  myHDMI.vga_print((char)'C');
  myHDMI.vga_print((char)0x0D);
  myHDMI.vga_print((char)0x0A);
  myHDMI.vga_println(F("End"));
  
  myHDMI.vga_print(F("unsigned long, base 10: "));
  ul_num = 123456789;
  myHDMI.vga_println(ul_num);
  
  myHDMI.vga_print(F("unsigned long, base 16: "));
  ul_num = 65535;
  myHDMI.vga_println(ul_num, 16);
  
  myHDMI.vga_print(F("float: "));
  myHDMI.vga_println(floater);
  
  myHDMI.vga_print(F("float: "));
  myHDMI.vga_println(floater, 6);
  
  myHDMI.vga_print(F("float, rounded: "));
  floater = 1.999;
  myHDMI.vga_println(floater, 2);
  
  myHDMI.vga_print(F("float, negative: "));
  floater = -1.999;
  myHDMI.vga_println(floater, 2);
  
}

void loop()
{
  // Nothing to do here
}

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
  
  myHDMI.print(F("HDMI VGA print example"));
  myHDMI.println();

  myHDMI.print(F("Flash Helper Text"));
  myHDMI.println();

  myHDMI.print(F("long, base 10: "));
  long long_num = 123456789;
  myHDMI.print(long_num);
  myHDMI.println();
  
  myHDMI.print(F("long, base 10, negative: "));
  long_num = -123456789;
  myHDMI.print(long_num);
  myHDMI.println();
  
  myHDMI.print(F("long, base 16: "));
  long_num = 65535;
  myHDMI.print(long_num, 16);
  myHDMI.println();
  
  myHDMI.print(F("long, base 2: "));
  long_num = 65535;
  myHDMI.print(long_num, 2);
  myHDMI.println();
  
  myHDMI.print(F("long, base 16, negative: "));
  long_num = -65535;
  myHDMI.print(long_num, 16);
  myHDMI.println();
  
  myHDMI.print(F("long, base 2, negative: "));
  long_num = -65535;
  myHDMI.print(long_num, 2);
  myHDMI.println();
  
  myHDMI.print(F("string: "));
  myHDMI.print("Hello world!");
  myHDMI.println();
  
  myHDMI.print(F("string: "));
  String str = "Hello world!";
  myHDMI.print(str);
  myHDMI.println();
  
  myHDMI.print(F("long converted to string: "));
  myHDMI.print(String(long_num));
  myHDMI.println();
  
  myHDMI.print(F("float converted to string: "));
  float floater = 1.23456;
  myHDMI.print(String(floater, 6));
  myHDMI.println();
  
  myHDMI.print(F("CR\rLF\nCRLF\r\nEnd"));
  myHDMI.println();
  
  myHDMI.print("CR\rLF\nCRLF\r\nEnd");
  myHDMI.println();
  
  myHDMI.print((char)'A');
  myHDMI.print((char)0x0D);
  myHDMI.print((char)'B');
  myHDMI.print((char)0x0A);
  myHDMI.print((char)'C');
  myHDMI.print((char)0x0D);
  myHDMI.print((char)0x0A);
  myHDMI.print(F("End"));
  myHDMI.println();
  
  myHDMI.print(F("unsigned long, base 10: "));
  unsigned long ul_num = 123456789;
  myHDMI.print(ul_num);
  myHDMI.println();
  
  myHDMI.print(F("unsigned long, base 16: "));
  ul_num = 65535;
  myHDMI.print(ul_num, 16);
  myHDMI.println();
  
  myHDMI.print(F("float: "));
  myHDMI.print(floater);
  myHDMI.println();
  
  myHDMI.print(F("float: "));
  myHDMI.print(floater, 6);
  myHDMI.println();

  myHDMI.print(F("float, rounded: "));
  floater = 1.999;
  myHDMI.print(floater, 2);
  myHDMI.println();
  
  myHDMI.print(F("float, negative: "));
  floater = -1.999;
  myHDMI.print(floater, 2);
  myHDMI.println();
  
  myHDMI.print(F("Backspace demo: ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
  myHDMI.current_column -= 18; // Backspace by 18 chars
  myHDMI.print(F("0123456789"));
  myHDMI.println();
  
  char buf[40];
  int a=12;
  int b=0x02FE;
  sprintf(buf, "sprintf (char[]): A: %d  B: 0x%04X", a, b);
  myHDMI.print(buf);
  myHDMI.println();


  delay(5000);
  
  
  myHDMI.println(F("HDMI VGA print example"));

  myHDMI.println(F("Flash Helper Text"));

  myHDMI.print(F("long, base 10: "));
  long_num = 123456789;
  myHDMI.println(long_num);
  
  myHDMI.print(F("long, base 10, negative: "));
  long_num = -123456789;
  myHDMI.println(long_num);
  
  myHDMI.print(F("long, base 16: "));
  long_num = 65535;
  myHDMI.println(long_num, 16);
  
  myHDMI.print(F("long, base 2: "));
  long_num = 65535;
  myHDMI.println(long_num, 2);
  
  myHDMI.print(F("long, base 16, negative: "));
  long_num = -65535;
  myHDMI.print(long_num, 16);
  myHDMI.println();
  
  myHDMI.print(F("long, base 2, negative: "));
  long_num = -65535;
  myHDMI.println(long_num, 2);
  
  myHDMI.print(F("string: "));
  myHDMI.println("Hello world!");
  
  myHDMI.print(F("string: "));
  str = "Hello world!";
  myHDMI.println(str);
  
  myHDMI.print(F("long converted to string: "));
  myHDMI.println(String(long_num));
  
  myHDMI.print(F("float converted to string: "));
  floater = 1.23456;
  myHDMI.println(String(floater, 6));
  
  myHDMI.println(F("CR\rLF\nCRLF\r\nEnd"));
  
  myHDMI.println("CR\rLF\nCRLF\r\nEnd");
  
  myHDMI.print((char)'A');
  myHDMI.print((char)0x0D);
  myHDMI.print((char)'B');
  myHDMI.print((char)0x0A);
  myHDMI.print((char)'C');
  myHDMI.print((char)0x0D);
  myHDMI.print((char)0x0A);
  myHDMI.println(F("End"));
  
  myHDMI.print(F("unsigned long, base 10: "));
  ul_num = 123456789;
  myHDMI.println(ul_num);
  
  myHDMI.print(F("unsigned long, base 16: "));
  ul_num = 65535;
  myHDMI.println(ul_num, 16);
  
  myHDMI.print(F("float: "));
  myHDMI.println(floater);
  
  myHDMI.print(F("float: "));
  myHDMI.println(floater, 6);
  
  myHDMI.print(F("float, rounded: "));
  floater = 1.999;
  myHDMI.println(floater, 2);
  
  myHDMI.print(F("float, negative: "));
  floater = -1.999;
  myHDMI.println(floater, 2);
  
  myHDMI.print(F("Backspace demo: ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
  myHDMI.current_column -= 18; // Backspace by 18 chars
  myHDMI.println(F("0123456789"));
  
  myHDMI.println(buf);
}

void loop()
{
  // Nothing to do here
}

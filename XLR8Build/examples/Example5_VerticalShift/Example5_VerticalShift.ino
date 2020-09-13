/*
  Vertical Shift

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

  This example shifts the displayed characters and attributes up by one line,
  to see how fast it can be done using the video memory read and write functions.
  This will tell us if we need to implement a fast shift in the FPGA
  (e.g. by including a row-offset register which can move the 'top of screen'
   to anywhere in memory in a single write).

  It takes 485 milliseconds to shift the entire screen (characters and atributes)
  using this method. = 16.16 milliseconds per row.
  Shiting just the characters reduces this to 350 milliseconds.
  That still seems slow. So, yes, we do need a fast shift feature... See Example6!
  
*/

#include "XLR8_HDMI.h"

XLR8_HDMI myHDMI;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("HDMI VGA Vertical Shift Test"));

  if (myHDMI.begin() == false)
  {
    Serial.println(F("HDMI XB did not begin! Freezing..."));
    while(1);
  }
}

void loop()
{
  fill_screen(); // Fill the screen with characters

  delay(2000); // Wait two seconds

  unsigned long start_time = millis();

  for (int i = 0; i < XLR8_HDMI_NUM_ROWS; i++) // Repeat the vertical shift num_rows times so the entire screen is cleared
  {
    myHDMI.vertical_shift(); // Shift the screen up by one line: characters and attributes
    //myHDMI.vertical_shift_char_only(); // Shift the screen up by one line: characters only
  }

  unsigned long end_time = millis();

  Serial.print(F("Vertical shifting took "));
  Serial.print(end_time - start_time);
  Serial.println(F(" milliseconds"));

  delay(2000); // Wait two seconds
}

void fill_screen()
{
  // Fill the visible screen with rows of characters
  char fill_char = '0'; // Fill the first row with zeros
  for (int row = 0; row < XLR8_HDMI_NUM_ROWS; row++)
  {
    for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
    {
      myHDMI.set_char_at(column, row, fill_char); // Add the character
    }
    fill_char++; // Increment the fill character for the next row
    if (fill_char == ':')
    {
      fill_char = 'A'; // After '9', switch to capital letters
    }
  }
}

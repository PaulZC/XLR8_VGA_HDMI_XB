/*
  Set Row Offset (Fast Vertical Shift)

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

  This example demonstrates how to shift the displayed characters and attributes up by one line instantaneously
  using the new set_row_offset function.
  The row offset is zero by default. The character displayed in the top left corner is read from RAM address 0.
  By setting the row offset to 1. The character displayed in the top left corner is read from RAM address 128 instead.
  This provides the illusion that the rows have all been shifted up by one instantaneously.
  Think of the row offset as a pointer to the RAM address for the top row of the screen.

  The video memory is large enough to allow a row offset of 32. So you could implement two display 'frames' or 'pages',
  the first starting at row 0 and the second starting at row 30 or 32, and switch between them instantaneously.
  
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

  fill_screen(); // Fill the screen with characters

}

void loop()
{
  // Reset the row offset so that the character displayed in the top left corner is read from RAM address 0.
  myHDMI.reset_row_offset();
  
  delay(2000); // Wait two seconds

  unsigned long start_time = millis(); // Record the start time

  for (int i = 0; i < XLR8_HDMI_NUM_ROWS; i++) // Repeat the fast vertical shift num_rows times so the entire screen is scrolled up and cleared
  {
    myHDMI.fast_vertical_shift(); // Shift the screen up by one line (characters and attributes) by incrementing the row offset
    //myHDMI.set_row_offset(i+1); // This method works too - if you want to set an absolute row offset
    //delay(100); // Uncomment this line if you want to be able to see the rows scroll up
  }

  unsigned long end_time = millis(); // Record the end time

  Serial.print(F("Vertical shifting took ")); // Print how long the shifting took to complete
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

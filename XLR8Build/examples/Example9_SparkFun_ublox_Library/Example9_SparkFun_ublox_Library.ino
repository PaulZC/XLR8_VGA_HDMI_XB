/*
  SparkFun ublox library example

  Runs on the Alorium Technologies Sno board when installed on the SparkX Sno Shoe.
  The Sno Shoe provides HDMI and Qwiic (I2C) connections.
  

  Written by: Paul Clark
  Date: September 8th 2020


  This example talks to a u-blox module over Qwiic / I2C
  and displays latitude, longitude and altitude.

  The vga_print functions are currently limited to:
  vga_print(const __FlashStringHelper *ifsh)    E.g.: vga_print(F("VGA HDMI SparkFun ublox Example"));
  vga_print(long n)                             E.g.: vga_print(latitude);
  vga_println()                                 Uses fast_vertical_shift() to scroll the display up by one row


  The video memory is implemented as two dual-port RAM blocks:
  one for the character codes and one for the color attributes.

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

  640x480 pixels with 8x16 pixels per character = 80x30 chars = 2400 bytes for character codes plus the same again for attributes
  The cx and cy counters in the Verilog are both 10 bit
  We don't care about bits cx[2:0] and cy[3:0]
  To keep things easy, we use 13 bit addressing ({cy[9:4], cx[9:3]}) = 8K of dual port RAM

  The screen area starts at: cx = 160; cy = 45
  hdmi_demo.sv removes the cx and cy offset
  So the first full character (top left) appears at RAM address 0
  Each row in memory is 2^7 bytes wide = 128
  The last character on the first visible row occupies RAM address 79
  The first character on the second row occupies RAM address 128

  print_at and attr_at use true column and row numbering:
  print_at(column, row, character)
  print_at(0, 0, n) will print character n at the first character on the first line (top left)
  print_at(1, 0, n) will print at the second character on the first line
  print_at(0, 1, n) will print at the first character on the second line


  The row offset is zero by default. The character displayed in the top left corner is read from RAM address 0.
  By setting the row offset to 1. The character displayed in the top left corner is read from RAM address 128 instead.
  This provides the illusion that the rows have all been shifted up by one instantaneously.
  Think of the row offset as a pointer to the RAM address for the top row of the screen.

  The video memory is large enough to allow a row offset of 32. So you could implement two display 'frames' or 'pages',
  the first starting at row 0 and the second starting at row 30 or 32, and switch between them instantaneously.

  In this example, the row offset is incremented after each print and simply wraps round when the end of the
  video memory is reached.
  
*/

#include "XLR8_HDMI.h"

#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to Ublox module.

#define first_char_addr 0
#define row_offset 128
#define num_rows 30
#define num_columns 80

uint8_t current_column;

void setup()
{
  //Set the volume attenuation for the sawtooth sound in the hdmi_demo
  //0 = no attenuation (this will be REALLY loud!)
  //9 = sensible attenuation
  //16 = maximum attenuation (mute)
  XLR8_HDMI.set_volume_attenuation(16); // Set the volume attenuation

  clear_video_memory(); // Clear the whole video memory

  // Reset the row offset so that the character displayed in the top left corner is read from RAM address 0.
  reset_row_offset();
  current_column = 0; // Start printing at the first column
  
  Serial.begin(115200);
  Serial.println(F("VGA HDMI SparkFun ublox Example"));
  vga_print(F("VGA HDMI SparkFun ublox Example"));
  vga_println();

  Wire.begin();

  delay(1000); // Give the u-blox module time to initialize

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    vga_print(F("Ublox GPS not detected at default I2C address."));
    vga_println();
    vga_print(F("Please check wiring. Freezing."));
    vga_println();
    while (1);
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfiguration(); //Save the current settings to flash and BBR
}

void loop()
{
  //Query module every 100ms. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available.
  if (millis() - lastTime > 100)
  {
    lastTime = millis(); //Update the timer
    
    long latitude = myGPS.getLatitude();
    vga_print(F("Lat: "));
    vga_print(latitude);

    long longitude = myGPS.getLongitude();
    vga_print(F(" Long: "));
    vga_print(longitude);
    vga_print(F(" (degrees * 10^-7)"));

    long altitude = myGPS.getAltitude();
    vga_print(F(" Alt: "));
    vga_print(altitude);
    vga_print(F(" (mm)"));

    vga_println();
  }
}

void vga_print(long n) // Print decimal long
{
  uint8_t offset = XLR8_HDMI.get_row_offset(); // Get the row offset
  
  if (n < 0)
  {
    print_at(current_column, offset + num_rows - 1, '-');
    current_column++;
    n = -n;
  }

  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];
  *str = '\0';

  do {
    char c = n % 10;
    n /= 10;
    *--str = c + '0';
  } while (n);

  while (*str)
  {
    char c = *str;
    print_at(current_column, offset + num_rows - 1, c);
    current_column++;
    str++;
  }  
}

void vga_print(const __FlashStringHelper *ifsh)
{
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  uint8_t offset = XLR8_HDMI.get_row_offset(); // Get the row offset
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    print_at(current_column, offset + num_rows - 1, c); // Print the character at the current_column on the bottom row
    current_column++;
  }
}

void vga_println() // println simply moves the display up by one row and clears the next row
{
  fast_vertical_shift();
  uint8_t offset = XLR8_HDMI.get_row_offset(); // Get the row offset
  for (int column = 0; column < num_columns; column++)
  {
    print_at(column, offset + num_rows - 1, 0x20); // Fill the next row with spaces
  }
  current_column = 0; // Reset current_column
}

void clear_video_memory()
{
  // Clear the whole of the video memory: set the character codes to 0x20 (space) and the attributes to 0x0F (white text on black background)
  // (The character and attribute RAM blocks are 8K in size but in reality only 2400 bytes are needed)
  for (int addr = 0; addr < 8192; addr++)
  {
		XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
		XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
		XLR8_HDMI.set_char_data(0x20); // Set the character code to 0x20 (space)
		XLR8_HDMI.set_attr_data(0x0F); // Set the attribute to 0x0F (white text on black background)
  }
}

void clear_screen()
{
  // Clear the visible characters - leave the attributes unchanged
  for (int row = 0; row < num_rows; row++)
  {
    for (int column = 0; column < num_columns; column++)
    {
      int addr = first_char_addr + (row * row_offset) + column;
      XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
      XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
      XLR8_HDMI.set_char_data(0x20); // Set the character code to 0x20 (space)
    }
  }
}

void reset_row_offset()
{
  XLR8_HDMI.set_row_offset(0); // Set the row offset to zero
}

void fast_vertical_shift()
{
  // Appear to shift both characters and attributes up by one row by incrementing the row offset
  uint8_t offset = XLR8_HDMI.get_row_offset(); // Get the row offset
  offset++; // Increment the row offset by one
  XLR8_HDMI.set_row_offset(offset); // Set the row offset
}

void vertical_shift()
{
  // Shift both characters and attributes up by one row
  for (int row = 1; row < (num_rows + 1); row++)
  {
    for (int column = 0; column < num_columns; column++)
    {
      int addr = first_char_addr + (row * row_offset) + column;
      XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
      XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
      uint8_t chr = XLR8_HDMI.get_char_data(); // Read the character
      uint8_t attr = XLR8_HDMI.get_attr_data(); // Read the attribute
      addr = addr - row_offset; // Subtract the row offset - move up by one row
      XLR8_HDMI.set_char_addr_hi(addr >> 8); // Change the address
      XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
      XLR8_HDMI.set_char_data(chr); // Set the character code
      XLR8_HDMI.set_attr_data(attr); // Set the character attributes
    }
  }
}

void vertical_shift_char_only()
{
  // Shift the characters up by one row
  for (int row = 1; row < (num_rows + 1); row++)
  {
    for (int column = 0; column < num_columns; column++)
    {
      int addr = first_char_addr + (row * row_offset) + column;
      XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
      XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
      uint8_t chr = XLR8_HDMI.get_char_data(); // Read the character
      addr = addr - row_offset; // Subtract the row offset - move up by one row
      XLR8_HDMI.set_char_addr_hi(addr >> 8); // Change the address
      XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
      XLR8_HDMI.set_char_data(chr); // Set the character code
    }
  }
}

void print_at(int column, int row, uint8_t chr) // Print chr at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  XLR8_HDMI.set_char_data(chr); // Set the character code
}

uint8_t get_char_at(int column, int row) // Read the chr at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  return(XLR8_HDMI.get_char_data()); // Read and return the character code
}

void attr_at(int column, int row, uint8_t attr) // Change the attributes at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  XLR8_HDMI.set_attr_data(attr); // Set the character attributes
}

uint8_t get_attr_at(int column, int row) // Read the attr at (column,row)
{
  int addr = first_char_addr + (row * row_offset) + column;
  XLR8_HDMI.set_char_addr_hi(addr >> 8); // Set up the address
  XLR8_HDMI.set_char_addr_lo(addr & 0xFF);
  return(XLR8_HDMI.get_attr_data()); // Read and return the attribute
}

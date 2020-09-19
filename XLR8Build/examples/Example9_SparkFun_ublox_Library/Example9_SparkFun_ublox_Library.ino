/*
  SparkFun ublox library example

  Runs on the Alorium Technologies Sno board when installed on the SparkX Sno Shoe.
  The Sno Shoe provides HDMI and Qwiic (I2C) connections.
  

  Written by: Paul Clark
  Date: September 19th 2020


  This example talks to a u-blox module over Qwiic / I2C and displays latitude, longitude and altitude.

*/

#include "XLR8_HDMI.h"

XLR8_HDMI myHDMI;

#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to Ublox module.

void setup()
{
  Serial.begin(115200);
  Serial.println(F("HDMI VGA SparkFun ublox Example"));

  if (myHDMI.begin() == false)
  {
    Serial.println(F("HDMI XB did not begin! Freezing..."));
    while(1);
  }  
  
  myHDMI.println(F("HDMI VGA SparkFun ublox Example"));

  Wire.begin();

  delay(1000); // Give the u-blox module time to initialize

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing..."));
    myHDMI.println(F("Ublox GPS not detected at default I2C address."));
    myHDMI.println(F("Please check wiring. Freezing..."));
    while (1);
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save the current IO port settings to flash and BBR
}

void loop()
{
  //Query module every 100ms. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available.
  if (millis() - lastTime > 100)
  {
    lastTime = millis(); //Update the timer
    
    long latitude = myGPS.getLatitude();
    myHDMI.print(F("Lat: "));
    myHDMI.print(latitude);

    long longitude = myGPS.getLongitude();
    myHDMI.print(F(" Long: "));
    myHDMI.print(longitude);
    myHDMI.print(F(" (degrees * 10^-7)"));

    long altitude = myGPS.getAltitude();
    myHDMI.print(F(" Alt: "));
    myHDMI.print(altitude);
    myHDMI.println(F(" (mm)"));
  }
}

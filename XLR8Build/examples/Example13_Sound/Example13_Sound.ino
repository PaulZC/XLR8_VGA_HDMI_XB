/*
  Sound

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 20th 2020

  A simple test of the XLR8 HDMI VGA XB's limited sound capabilities:

  set_sound_freq(freq) sets the sound frequency to (freq * 8) Hz.
  set_sound_duration(duration) sets the sound duration to (duration * 0.0625) s.
  set_volume_attenuation(attenuation) sets the sound attenuation to attenuation: 16 = mute; 5 = sensible; 0 = LOUD!
  The sound will stop automatically when duration has expired.
  You can stop the current sound by calling set_sound_duration(0).
  
*/

#include "XLR8_HDMI.h"

XLR8_HDMI myHDMI;

#define VOL 5 // sensible attenuation
#define MUTE 16 // maximum attenuation (mute)

void setup()
{
  Serial.begin(115200);
  Serial.println(F("HDMI VGA Sound Test"));

  if (myHDMI.begin() == false)
  {
    Serial.println(F("HDMI XB did not begin! Freezing..."));
    while(1);
  }

  myHDMI.println(F("HDMI VGA Sound Test"));

  myHDMI.print(F("Setting volume attenuation to "));
  myHDMI.println(VOL);
  myHDMI.set_volume_attenuation(VOL);
  
  myHDMI.print(F("Volume attenuation set to "));
  myHDMI.println(myHDMI.get_volume_attenuation());
}

void loop()
{
  myHDMI.println(F("512Hz beep for 1 second"));
  myHDMI.set_sound_freq(64); // Frequency = 64 * 8Hz = 512Hz
  myHDMI.set_sound_duration(16); // Duration = 16 * 0.0625s = 1s
  myHDMI.print(F("Freq set to "));
  myHDMI.println(myHDMI.get_sound_freq());
  delay(2000); // Wait 2s to create a 1s gap after the beep

  myHDMI.println(F("1kHz beep for 5 seconds - stopped after 2 seconds"));
  myHDMI.set_sound_freq(128); // Frequency = 128 * 8Hz = 1024Hz
  myHDMI.set_sound_duration(80); // Duration = 80 * 0.0625s = 5s
  myHDMI.print(F("Freq set to "));
  myHDMI.println(myHDMI.get_sound_freq());
  delay(2000); // Wait for 2s
  myHDMI.set_sound_duration(0); // Stop the current sound

  delay(1000); // Wait

  // https://worldofspectrum.org/ZXBasicManual/zxmanchap19.html
  myHDMI.println(F("Funeral march from Mahler's first symphony"));
  
  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(293.7 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(311.1 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(293.7 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  
  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(293.7 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(311.1 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(293.7 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);

  myHDMI.set_sound_freq(311.1 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(349.2 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(392.0 / 8); myHDMI.set_sound_duration(2 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);

  myHDMI.set_sound_freq(311.1 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(349.2 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(392.0 / 8); myHDMI.set_sound_duration(2 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  
  myHDMI.set_sound_freq(392.0 / 8); myHDMI.set_sound_duration(0.75 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(415.3 / 8); myHDMI.set_sound_duration(0.25 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(392.0 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(349.2 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(311.1 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(293.7 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);

  myHDMI.set_sound_freq(392.0 / 8); myHDMI.set_sound_duration(0.75 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(415.3 / 8); myHDMI.set_sound_duration(0.25 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(392.0 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(349.2 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(311.1 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(293.7 / 8); myHDMI.set_sound_duration(0.5 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);

  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(196.0 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(2 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  
  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(196.0 / 8); myHDMI.set_sound_duration(1 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  myHDMI.set_sound_freq(261.6 / 8); myHDMI.set_sound_duration(2 / 0.0625); while(myHDMI.sound_in_progress()) delay(5);
  
  delay(1000); // Wait and repeat
}

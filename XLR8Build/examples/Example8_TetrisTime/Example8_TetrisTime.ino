/*
  Tetris Time

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

  This example contains a remix of Tobias Blum's Tetris Animation:
  https://github.com/toblum/TetrisAnimation
  Tobias' work (shared under the GNU General Public License v3.0) is gratefully acknowledged

  Tetris-style time is dropped into position.

  The numbers and letters arrays are held in RAM and we don't have enough RAM to hold both...
  So this example is limited to numbers only.

*/

#include "TetrisMatrixDraw.h"
TetrisMatrixDraw tetris;

volatile int timer1_counter;

// Timer 1 interrupt service routine 
ISR(TIMER1_OVF_vect)
{
  TCNT1 = timer1_counter;   // preload timer
  if (tetris.scale == 1)
    tetris.drawNumbers(24, 26, true, true); // Scale 1: Display numbers with blinking colon. Start at column 24. Stop at row 26.
  else // if (tetris.scale == 2)
    tetris.drawNumbers(10, 14, true, true); // Scale 2: Display double-size numbers with blinking colon. Start at column 10. Stop at row 14.
}

int8_t old_secs = 0; // Store the previous time second

void setup()
{
  Serial.begin(115200);
  Serial.println(F("HDMI VGA Tetris Time"));


  // Initialize timer1 to provide ticks for drawNumbers
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  // Change this to speed up or slow down the drop speed
  //timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz
  //timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
  //timer1_counter = 62411;   // preload timer 65536-16MHz/256/20Hz
  timer1_counter = 59286;   // preload timer 65536-16MHz/256/10Hz
  //timer1_counter = 53036;   // preload timer 65536-16MHz/256/5Hz
  //timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
  
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts

  
  //tetris.scale = 2; // Uncomment this line to display double-size digits
  
  updateTime(0,0); // Zero the clock
}

void loop()
{
  unsigned long now = millis(); // Get the current time in millis
  uint8_t hour, min, sec;
  int time = (now / 1000) % (24 * 60 * 60); // Convert time into hour,min,sec
  hour = (time / 3600) % 24;
  min = (time / 60) % 60;
  sec = time % 60;

  if (sec != old_secs) // Have the seconds changed?
  {
    old_secs = sec; // Update old_secs
    if (sec == 0) // Have the minutes rolled over?
    {
      updateTime(hour, min); // Update the tetris time
    }
  }
}

void updateTime(int8_t hour, int8_t minute) // Update the tetris time
{
  char timestr[6];
  sprintf(timestr, "%02d:%02d", hour, minute);
  Serial.println(timestr);
  tetris.clear_screen();
  tetris.setTime(timestr, true); // Update the time, force a refresh of all digits
}

/*
  Breakout

  Runs on the Alorium Technologies Sno board

  Written by: Paul Clark
  Date: September 13th 2020

  This example is based on the 1970's Atari game Breakout.
  Colored rows of bricks are gradually broken by a bouncing ball.
  

  set_char_at and set_attr_at use true column and row numbering:
  set_char_at(column, row, character)
  set_char_at(0, 0, n) will print character n at the first character on the first line (top left)
  set_char_at(1, 0, n) will print at the second character on the first line
  set_char_at(0, 1, n) will print at the first character on the second line

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

*/

#include "XLR8_HDMI.h"

XLR8_HDMI myHDMI;

int ball_row;
int ball_column;
int ball_direction;
int new_ball_direction;
const int North_East = 0;
const int South_East = 1;
const int North_West = 2;
const int South_West = 3;
const uint8_t ball_char = 1;
const uint8_t brick_char = 219;
const uint8_t space_char = 255;
int old_bricks = 0;
int stuck = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("HDMI VGA Breakout"));

  if (myHDMI.begin() == false)
  {
    Serial.println(F("HDMI XB did not begin! Freezing..."));
    while(1);
  }

  place_bricks(); // Place the rows of bricks

  delay(2000); // Wait two seconds

  place_ball(); // Place the ball at a random position

  delay(2000); // Wait two seconds
}

void loop()
{
  bool brickLeftRight = false; // Use this flag to indicate if we have already hit a brick to the left/right
  bool brickHit = false; // Use this flag to indicate if we have hit a brick and so should ignore the diagonals

  // Check if we have reached the edge of the screen
  if (ball_direction == North_East)
  {
    if (ball_row == 0) // Have we hit the top?
    {
      new_ball_direction = South_East; // Change the direction
      if (ball_column == 79) // Have we hit the corner?
      {
        new_ball_direction = South_West; // Change the direction
      }
    }
    else if (ball_column == 79) // Have we hit the side?
    {
      new_ball_direction = North_West; // Change the direction
    }
  }
  else if (ball_direction == South_East)
  {
    if (ball_row == 29) // Have we hit the bottom?
    {
      new_ball_direction = North_East; // Change the direction
      if (ball_column == 79) // Have we hit the corner?
      {
        new_ball_direction = North_West; // Change the direction
      }
    }
    else if (ball_column == 79) // Have we hit the side?
    {
      new_ball_direction = South_West; // Change the direction
    }
  }
  else if (ball_direction == North_West)
  {
    if (ball_row == 0) // Have we hit the top?
    {
      new_ball_direction = South_West; // Change the direction
      if (ball_column == 0) // Have we hit the corner?
      {
        new_ball_direction = South_East; // Change the direction
      }
    }
    else if (ball_column == 0) // Have we hit the side?
    {
      new_ball_direction = North_East; // Change the direction
    }
  }
  else // if (ball_direction == South_West)
  {
    if (ball_row == 29) // Have we hit the bottom?
    {
      new_ball_direction = North_West; // Change the direction
      if (ball_column == 0) // Have we hit the corner?
      {
        new_ball_direction = North_East; // Change the direction
      }
    }
    else if (ball_column == 0) // Have we hit the side?
    {
      new_ball_direction = South_East; // Change the direction
    }
  }

  // Check if we have hit a brick above/below
  if (ball_direction == North_East)
  {
    if ((ball_row > 0) && (myHDMI.get_char_at(ball_column, ball_row - 1) == brick_char))
    {
      myHDMI.set_char_at(ball_column, ball_row - 1, space_char); // Erase the brick we hit
      brickHit = true;
      // Change direction
      if (ball_column == 79) // Are we at the side?
      {
        new_ball_direction = South_West;
      }
      else
      {
        new_ball_direction = South_East;
        // Check if we have also hit a brick right
        if (myHDMI.get_char_at(ball_column + 1, ball_row) == brick_char)
        {
          myHDMI.set_char_at(ball_column + 1, ball_row, space_char); // Erase the brick we hit
          brickHit = true;
          // Change direction
          new_ball_direction = South_West;
          brickLeftRight = true;
        }
      }
    }
  }
  else if (ball_direction == South_East)
  {
    if ((ball_row < 29) && (myHDMI.get_char_at(ball_column, ball_row + 1) == brick_char))
    {
      myHDMI.set_char_at(ball_column, ball_row + 1, space_char); // Erase the brick we hit
      brickHit = true;
      // Change direction
      if (ball_column == 79) // Are we at the side?
      {
        new_ball_direction = North_West;
      }
      else
      {
        new_ball_direction = North_East;
        // Check if we have also hit a brick right
        if (myHDMI.get_char_at(ball_column + 1, ball_row) == brick_char)
        {
          myHDMI.set_char_at(ball_column + 1, ball_row, space_char); // Erase the brick we hit
          brickHit = true;
          // Change direction
          new_ball_direction = North_West;
          brickLeftRight = true;
        }
      }
    }
  }
  else if (ball_direction == North_West)
  {
    if ((ball_row > 0) && (myHDMI.get_char_at(ball_column, ball_row - 1) == brick_char))
    {
      myHDMI.set_char_at(ball_column, ball_row - 1, space_char); // Erase the brick we hit
      brickHit = true;
      // Change direction
      if (ball_column == 0) // Are we at the side?
      {
        new_ball_direction = South_East;
      }
      else
      {
        new_ball_direction = South_West;
        // Check if we have also hit a brick left
        if (myHDMI.get_char_at(ball_column - 1, ball_row) == brick_char)
        {
          myHDMI.set_char_at(ball_column - 1, ball_row, space_char); // Erase the brick we hit
          brickHit = true;
          // Change direction
          new_ball_direction = South_East;
          brickLeftRight = true;
        }
      }
    }
  }
  else // if (ball_direction == South_West)
  {
    if ((ball_row < 29) && (myHDMI.get_char_at(ball_column, ball_row + 1) == brick_char))
    {
      myHDMI.set_char_at(ball_column, ball_row + 1, space_char); // Erase the brick we hit
      brickHit = true;
      // Change direction
      if (ball_column == 0) // Are we at the side?
      {
        new_ball_direction = North_East;
      }
      else
      {
        new_ball_direction = North_West;
        // Check if we have also hit a brick left
        if (myHDMI.get_char_at(ball_column - 1, ball_row) == brick_char)
        {
          myHDMI.set_char_at(ball_column - 1, ball_row, space_char); // Erase the brick we hit
          brickHit = true;
          // Change direction
          new_ball_direction = North_East;
          brickLeftRight = true;
        }
      }
    }
  }

  // Check if we have (only) hit a brick left/right
  if (brickLeftRight == false) // Ignore this if we have already hit a brick left/right
  {
    if (ball_direction == North_East)
    {
      if (ball_column < 79) // If we are not at the side
      {
        if (myHDMI.get_char_at(ball_column + 1, ball_row) == brick_char)
        {
          myHDMI.set_char_at(ball_column + 1, ball_row, space_char); // Erase the brick we hit
          brickHit = true;
          // Change direction
          new_ball_direction = North_West;
        }
      }
    }
    else if (ball_direction == South_East)
    {
      if (ball_column < 79) // If we are not at the side
      {
        if (myHDMI.get_char_at(ball_column + 1, ball_row) == brick_char)
        {
          myHDMI.set_char_at(ball_column + 1, ball_row, space_char); // Erase the brick we hit
          brickHit = true;
          // Change direction
          new_ball_direction = South_West;
        }
      }
    }
    else if (ball_direction == North_West)
    {
      if (ball_column > 0) // If we are not at the side
      {
        if (myHDMI.get_char_at(ball_column - 1, ball_row) == brick_char)
        {
          myHDMI.set_char_at(ball_column - 1, ball_row, space_char); // Erase the brick we hit
          brickHit = true;
          // Change direction
          new_ball_direction = North_East;
        }
      }
    }
    else // if (ball_direction == South_West)
    {
      if (ball_column > 0) // If we are not at the side
      {
        if (myHDMI.get_char_at(ball_column - 1, ball_row) == brick_char)
        {
          myHDMI.set_char_at(ball_column - 1, ball_row, space_char); // Erase the brick we hit
          brickHit = true;
          // Change direction
          new_ball_direction = South_East;
        }
      }
    }
  }

  // Check the diagonals - but only if we have not already hit bricks
  if (brickHit == false)
  {
    if (ball_direction == North_East)
    {
      if ((ball_column < 79) && (ball_row > 0)) // If we are not at the side
      {
        if (myHDMI.get_char_at(ball_column + 1, ball_row - 1) == brick_char)
        {
          myHDMI.set_char_at(ball_column + 1, ball_row - 1, space_char); // Erase the brick we hit
          // Change direction
          new_ball_direction = South_West;
        }
      }
    }
    else if (ball_direction == South_East)
    {
      if ((ball_column < 79) && (ball_row < 29)) // If we are not at the side
      {
        if (myHDMI.get_char_at(ball_column + 1, ball_row + 1) == brick_char)
        {
          myHDMI.set_char_at(ball_column + 1, ball_row + 1, space_char); // Erase the brick we hit
          // Change direction
          new_ball_direction = North_West;
        }
      }
    }
    else if (ball_direction == North_West)
    {
      if (ball_column > 0) // If we are not at the side
      {
        if (myHDMI.get_char_at(ball_column - 1, ball_row - 1) == brick_char)
        {
          myHDMI.set_char_at(ball_column - 1, ball_row - 1, space_char); // Erase the brick we hit
          // Change direction
          new_ball_direction = South_East;
        }
      }
    }
    else // if (ball_direction == South_West)
    {
      if (ball_column > 0) // If we are not at the side
      {
        if (myHDMI.get_char_at(ball_column - 1, ball_row + 1) == brick_char)
        {
          myHDMI.set_char_at(ball_column - 1, ball_row + 1, space_char); // Erase the brick we hit
          // Change direction
          new_ball_direction = North_East;
        }
      }
    }
  }

  // Erase the ball
  myHDMI.set_char_at(ball_column, ball_row, space_char);
  myHDMI.set_attr_at(ball_column, ball_row, 0x0F); // Set the attribute to 0x0F (white text on black background)
  
  // Move the ball
  if (new_ball_direction == North_East)
  {
    ball_row -= 1;
    ball_column += 1;
  }
  else if (new_ball_direction == South_East)
  {
    ball_row += 1;
    ball_column += 1;
  }
  else if (new_ball_direction == North_West)
  {
    ball_row -= 1;
    ball_column -= 1;
  }
  else // if (new_ball_direction == South_West)
  {
    ball_row += 1;
    ball_column -= 1;
  }

  // Print the ball at its new position
  myHDMI.set_char_at(ball_column, ball_row, ball_char);
  myHDMI.set_attr_at(ball_column, ball_row, 0x0F); // Set the attribute to 0x0F (white text on black background)

  ball_direction = new_ball_direction; // Update the direction

  // Slow it down!
  // It might be nice to set this via an analogRead?
  delay(25);

  // Count the remaining bricks. Start over if required.
  // Trap an infinite loop by making sure that the number of bricks is reducing.
  int bricks = count_bricks();
  if (bricks == old_bricks)
  {
    stuck++;
  }
  else
  {
    stuck = 0;
  }
  old_bricks = bricks;
  if ((bricks == 0) || (stuck > 24000)) // Limit stuck to a suitably large number (with a loop delay of 25ms, 24000 = 10 mins)
  {
    delay(2000);
    myHDMI.clear_video_memory(); // Clear the whole video memory
    place_bricks(); // Place the rows of bricks
    delay(2000); // Wait two seconds
    place_ball(); // Place the ball at a random position
    delay(2000); // Wait two seconds
    stuck = 0;
  }
}

void place_bricks()
{
  for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
  {
    myHDMI.set_char_at(column, 5, brick_char); // Place brick
    myHDMI.set_attr_at(column, 5, 0x04); // Red
    myHDMI.set_char_at(column, 6, brick_char); // Place brick
    myHDMI.set_attr_at(column, 6, 0x0C); // Bright Red
    myHDMI.set_char_at(column, 7, brick_char); // Place brick
    myHDMI.set_attr_at(column, 7, 0x0E); // Yellow
    myHDMI.set_char_at(column, 8, brick_char); // Place brick
    myHDMI.set_attr_at(column, 8, 0x0A); // Bright Green
    myHDMI.set_char_at(column, 9, brick_char); // Place brick
    myHDMI.set_attr_at(column, 9, 0x01); // Blue
  }
}

int count_bricks()
{
  int numBricks = 0;
  
  for (int column = 0; column < XLR8_HDMI_NUM_COLUMNS; column++)
  {
    for (int row = 5; row < 10; row++)
    {
      if (myHDMI.get_char_at(column, row) == brick_char)
      {
        numBricks++;
      }
    }
  }

  return (numBricks);
}

void place_ball()
{
  ball_row = 25;
  ball_column = random(79);
  ball_direction = random(3);
  new_ball_direction = ball_direction;
  myHDMI.set_char_at(ball_column, ball_row, ball_char); // Place ball
}

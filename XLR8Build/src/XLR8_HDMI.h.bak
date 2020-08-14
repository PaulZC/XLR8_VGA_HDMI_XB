/*--------------------------------------------------------------------
 Copyright (c) 2016 Alorium Technology.  All right reserved.
 The XLR8Build/XLR8BuildTemplate library provides a path
   to build FPGA images within the Arduino Framework. There
   isn't any C/C++ code associated with this library but
   this empty header file prevents the Arduino IDE from
   calling it an invalid library
 Written by Matt Weber (support@aloriumtech.com) of
 Alorium Technology (info@aloriumtech.com)


 This library is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as
 published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library.  If not, see
 <http://www.gnu.org/licenses/>.
 --------------------------------------------------------------------*/

#ifndef _XLR8_HDMI_H_INCLUDED
#define _XLR8_HDMI_H_INCLUDED

#include <Arduino.h>

#define XLR8_HDMI_RED_ON    _SFR_MEM8(0xE0)
#define XLR8_HDMI_RED_OFF   _SFR_MEM8(0xE1)
#define XLR8_HDMI_GREEN_ON  _SFR_MEM8(0xE2)
#define XLR8_HDMI_GREEN_OFF _SFR_MEM8(0xE3)
#define XLR8_HDMI_BLUE_ON   _SFR_MEM8(0xE4)
#define XLR8_HDMI_BLUE_OFF  _SFR_MEM8(0xE5)

class XLR8_HDMIClass {

public:

  XLR8_HDMIClass() {}

  ~XLR8_HDMIClass() {}

  void __attribute__ ((noinline)) set_red_on_pixel_val(uint8_t val) {
    XLR8_HDMI_RED_ON = val;
  }
  void __attribute__ ((noinline)) set_red_off_pixel_val(uint8_t val) {
    XLR8_HDMI_RED_OFF = val;
  }
  void __attribute__ ((noinline)) set_green_on_pixel_val(uint8_t val) {
    XLR8_HDMI_GREEN_ON = val;
  }
  void __attribute__ ((noinline)) set_green_off_pixel_val(uint8_t val) {
    XLR8_HDMI_GREEN_OFF = val;
  }
  void __attribute__ ((noinline)) set_blue_on_pixel_val(uint8_t val) {
    XLR8_HDMI_BLUE_ON = val;
  }
  void __attribute__ ((noinline)) set_blue_off_pixel_val(uint8_t val) {
    XLR8_HDMI_BLUE_OFF = val;
  }

private:

};

extern XLR8_HDMIClass XLR8_HDMI;

#endif

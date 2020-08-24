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

#define XLR8_HDMI_VOLUME    	 _SFR_MEM8(0xE0)
#define XLR8_HDMI_CHAR_ADDR_LO _SFR_MEM8(0xE1)
#define XLR8_HDMI_CHAR_ADDR_HI _SFR_MEM8(0xE2)
#define XLR8_HDMI_CHAR_DATA    _SFR_MEM8(0xE3)
#define XLR8_HDMI_ATTR_DATA    _SFR_MEM8(0xE4)
#define XLR8_HDMI_ROW_OFFSET   _SFR_MEM8(0xE5)

class XLR8_HDMIClass {

public:

  XLR8_HDMIClass() {}

  ~XLR8_HDMIClass() {}

	// Set the audio volume attenuation
	// 0 = no attenuation (LOUD!)
	// 9 = sensible attenuation
	// 16 = maximum attenuation (mute)
	void __attribute__ ((noinline)) set_volume_attenuation(uint8_t val) {
    XLR8_HDMI_VOLUME = val;
	}

	// Set the Lo Byte of the RAM 2-PORT video memory address
	void __attribute__ ((noinline)) set_char_addr_lo(uint8_t val) {
	 XLR8_HDMI_CHAR_ADDR_LO = val;
	}
		
	// Set the Hi Byte of the RAM 2-PORT video memory address
	void __attribute__ ((noinline)) set_char_addr_hi(uint8_t val) {
	 XLR8_HDMI_CHAR_ADDR_HI = val;
	}
		
	// Load the video memory address with character val
	void __attribute__ ((noinline)) set_char_data(uint8_t val) {
	 XLR8_HDMI_CHAR_DATA = val;
	}
	
	// LOad the video memory address with the attribute val
	void __attribute__ ((noinline)) set_attr_data(uint8_t val) {
	 XLR8_HDMI_ATTR_DATA = val;
	}
	
	// Read the character val back from video memory
	uint8_t __attribute__ ((noinline)) get_char_data() {
    return XLR8_HDMI_CHAR_DATA;
	}
		
	// Read the attribute val back from video memory
	uint8_t __attribute__ ((noinline)) get_attr_data() {
    return XLR8_HDMI_ATTR_DATA;
	}
		
	// Set the video memory row offset to val
	void __attribute__ ((noinline)) set_row_offset(uint8_t val) {
	 XLR8_HDMI_ROW_OFFSET = val;
	}
	
	// Read the video memory row offset
	uint8_t __attribute__ ((noinline)) get_row_offset() {
    return XLR8_HDMI_ROW_OFFSET;
	}
		
private:

};

extern XLR8_HDMIClass XLR8_HDMI;

#endif

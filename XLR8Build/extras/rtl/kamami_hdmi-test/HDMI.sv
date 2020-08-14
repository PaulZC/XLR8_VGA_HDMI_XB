/*
	This module generates 640x480 60Hz video.
	
	Based on: KAMAMI's maXimator-HDMI-test
	
	This module expects the incoming clock (clkPixel) to be 252MHz.
	The MAX 10 10M16SAU169C8G on the Alorium Technology Sno only has one PLL module so we need to use that to generate the 252MHz pixel clock.
	pll16.vh has been edited to produce the pixel clock as "clk_option4".
	In a perfect world, the pixel clock should actually be 251.75MHz but the PLL can't generate that from 16MHz.
	CLKDivider U2 divides clkPixel down to 25.2MHz for the TMDS Clock.
	
	Six registers specify the on and off values for the RGB pixels.
	The values are set by the Arduino code.
	
	The heartbeat wire is used to flash the LED on pin 13 at 1Hz.
*/
module HDMI
(
	input logic clkPixel,
	output logic [2:0] datap,
	output logic [2:0] datan,
	output logic clkp,
	output logic clkn,
	input logic [7:0] red_on,
	input logic [7:0] red_off,
	input logic [7:0] green_on,
	input logic [7:0] green_off,
	input logic [7:0] blue_on,
	input logic [7:0] blue_off,
	output logic heartbeat
);

logic clkClock;
logic hSync, vSync, drawArea;
logic [7:0] red, green, blue;
logic [9:0] TMDSRed, TMDSGreen, TMDSBlue;
logic [9:0] TMDSShiftRed, TMDSShiftGreen, TMDSShiftBlue;

// Clock divider for the heartbeat. Divide by 252,000,000 to produce 1Hz with 5:95 on:off ratio
CLKDivider
#(.N(252000000), .R(20))
U1
(
	.in(clkPixel),
	.out(heartbeat)
);


// Clock divider for the TMDS clock. Divide by 10 to produce 25.2MHz with 50:50 on:off ratio
CLKDivider
#(.N(10), .R(2))
U2
(
	.in(clkPixel),
	.out(clkClock)
);

Video Vid
(
	.clk(clkClock),
	.vSync(vSync),
	.hSync(hSync),
	.drawArea(drawArea),
	.red(red),
	.green(green),
	.blue(blue),
	.red_on(red_on),
	.red_off(red_off),
	.green_on(green_on),
	.green_off(green_off),
	.blue_on(blue_on),
	.blue_off(blue_off)
);

///////////////////////////

TMDSEncoder TMDSR
(
	.clk(clkClock), 
	.VD(red), 
	.CD(2'b00), 
	.VDE(drawArea), 
	.TMDS(TMDSRed)
);

TMDSEncoder TMDSG
(
	.clk(clkClock), 
	.VD(green), 
	.CD(2'b00), 
	.VDE(drawArea), 
	.TMDS(TMDSGreen)
);

TMDSEncoder TMDSB
(
	.clk(clkClock), 
	.VD(blue), 
	.CD({vSync, hSync}), 
	.VDE(drawArea), 
	.TMDS(TMDSBlue)
);

///////////////////////////

ShiftRegister
#(.N(10))
SR0
(
	.clk(clkPixel),
	.in(TMDSRed),
	.out(TMDSShiftRed)
);

ShiftRegister
#(.N(10))
SR1
(
	.clk(clkPixel),
	.in(TMDSGreen),
	.out(TMDSShiftGreen)
);

ShiftRegister
#(.N(10))
SR2
(
	.clk(clkPixel),
	.in(TMDSBlue),
	.out(TMDSShiftBlue)
);

///////////////////////////

DifferentialSignal DS0
(
	.in(clkClock),
	.p(clkp),
	.n(clkn)
);

DifferentialSignal DS1
(
	.in(TMDSShiftRed[0]),
	.p(datap[2]),
	.n(datan[2])
);

DifferentialSignal DS2
(
	.in(TMDSShiftGreen[0]),
	.p(datap[1]),
	.n(datan[1])
);

DifferentialSignal DS3
(
	.in(TMDSShiftBlue[0]),
	.p(datap[0]),
	.n(datan[0])
);

endmodule 
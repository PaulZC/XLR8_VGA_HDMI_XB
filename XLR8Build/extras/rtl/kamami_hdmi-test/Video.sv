module Video
(
	input logic clk,
	output logic vSync,
	output logic hSync,
	output logic drawArea,
	output logic [7:0] red,
	output logic [7:0] green,
	output logic [7:0] blue,
	input logic [7:0] red_on,
	input logic [7:0] red_off,
	input logic [7:0] green_on,
	input logic [7:0] green_off,
	input logic [7:0] blue_on,
	input logic [7:0] blue_off
);

/*
	For 640x480 60Hz we need 25.175 MHz clock, so it's impossible to get that frequency with a crystal oscillator on board.
	If you will have any problems with generating video you can change these constants a little bit.
*/

const int LINES 			= 480;
const int LINES_FRONT 	= 10;
const int LINES_SYNC 	= 2;
const int LINES_BACK 	= 33;
const int LINES_ALL		= (LINES + LINES_FRONT + LINES_SYNC + LINES_BACK);

const int PIXELS 			= 650;
const int PIXELS_FRONT 	= 16;
const int PIXELS_SYNC 	= 96;
const int PIXELS_BACK 	= 48;
const int PIXELS_ALL		= (PIXELS + PIXELS_FRONT + PIXELS_SYNC + PIXELS_BACK);

logic [9:0] counterX, counterY;

always_ff@(posedge clk) 
	drawArea <= ((counterY > (LINES_SYNC + LINES_BACK)) && (counterY < (LINES_ALL - LINES_FRONT)) 
				&& (counterX > (PIXELS_SYNC + PIXELS_BACK)) && (counterX < (PIXELS_ALL - PIXELS_FRONT)));

always_ff@(posedge clk) 
	counterX <= (counterX == PIXELS_ALL) ? 1'b1 : counterX + 1'b1;
	
always_ff@(posedge clk) 
	if(counterX == PIXELS_ALL) 
		counterY <= (counterY == LINES_ALL) ? 1'b1 : counterY + 1'b1;

always_ff@(posedge clk) 
	hSync <= (counterX < PIXELS_SYNC);
	
always_ff@(posedge clk) 
	vSync <= (counterY < LINES_SYNC);
	

/*
	Here is a code to generate red, green and blue colors.
	You can modify this code to generate whatever you want. 
*/	

logic [2:0] index = 3'b000;
logic [2:0] rgb = 3'b000;

always_ff@(posedge clk)
	begin
		if (drawArea)
			begin
				index <= index + 1'b1; // This is the bit that will need to be replaced with video memory reads
				if (index == 3'b111) 
					rgb <= rgb + 1'b1;
			end
		else
			begin
				rgb <= 3'b000; // Display 'off' pixels outside drawArea
				index <= 3'b000;
			end
	end

always_ff@(posedge clk)
	begin
		if (rgb[0] == 1)
			red <= red_on;
		else
			red <= red_off;
	end
	
always_ff@(posedge clk)
	begin
		if (rgb[1] == 1)
			green <= green_on;
		else
			green <= green_off;
	end

always_ff@(posedge clk)
	begin
		if (rgb[2] == 1)
			blue <= blue_on;
		else
			blue <= blue_off;
	end

endmodule 
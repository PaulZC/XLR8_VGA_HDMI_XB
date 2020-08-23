module hdmi_demo
(
  input CLK_PIXEL,
  input CLK_AUDIO,

  // HDMI output
  output [2:0] HDMI_TX,
  output [2:0] HDMI_TX_N,
  output HDMI_CLK,
  output HDMI_CLK_N,
  //inout HDMI_SDA,
  //inout HDMI_SCL,
  //input HDMI_HPD,
  input [7:0] VOLUME, // Volume attenuation
  output heartbeat,
  // RAM for character storage
  output [12:0] RAM_ADDRESS,
  input [7:0] RAM_CHAR_DATA,
  input [7:0] RAM_ATTR_DATA,
  output RAM_CHAR_RE,
  output RAM_ATTR_RE
);

wire clk_pixel_x10;
wire clk_pixel;
wire clk_audio;

//hdmi_pll hdmi_pll(.inclk0(CLK_48MHZ), .c0(clk_pixel), .c1(clk_pixel_x5), .c2(clk_audio));
assign clk_pixel_x10 = CLK_PIXEL;

// Clock divider for the heartbeat. Divide by 252,000,000 to produce 1Hz with 5:95 on:off ratio
CLKDivider
#(.N(252000000), .R(20))
U1
(
	.in(clk_pixel_x10),
	.out(heartbeat)
);


// Clock divider for the TMDS clock. Divide by 10 to produce 25.2MHz with 50:50 on:off ratio
CLKDivider
#(.N(10), .R(2))
U2
(
	.in(clk_pixel_x10),
	.out(clk_pixel)
);


assign clk_audio = CLK_AUDIO;


localparam AUDIO_BIT_WIDTH = 16;
localparam AUDIO_RATE = 48000;
localparam WAVE_RATE = 480;

logic [AUDIO_BIT_WIDTH-1:0] audio_sample_word;
logic [AUDIO_BIT_WIDTH-1:0] audio_sample_word_dampened; // This is to avoid giving you a heart attack -- it'll be really loud if it uses the full dynamic range.

//assign audio_sample_word_dampened = audio_sample_word >> 9;
always @(posedge clk_pixel)
begin
	audio_sample_word_dampened = audio_sample_word >> VOLUME;
end

sawtooth #(.BIT_WIDTH(AUDIO_BIT_WIDTH), .SAMPLE_RATE(AUDIO_RATE), .WAVE_RATE(WAVE_RATE)) sawtooth (.clk_audio(clk_audio), .level(audio_sample_word));

logic [23:0] rgb;
logic [9:0] cx, cy;
hdmi #(.VIDEO_ID_CODE(1), .VIDEO_REFRESH_RATE(60.00), .DDRIO(0), .AUDIO_RATE(AUDIO_RATE), .AUDIO_BIT_WIDTH(AUDIO_BIT_WIDTH)) hdmi(.clk_pixel_x10(clk_pixel_x10), .clk_pixel(clk_pixel), .clk_audio(clk_audio), .rgb(rgb), .audio_sample_word('{audio_sample_word_dampened, audio_sample_word_dampened}), .tmds_p(HDMI_TX), .tmds_clock_p(HDMI_CLK), .tmds_n(HDMI_TX_N), .tmds_clock_n(HDMI_CLK_N), .cx(cx), .cy(cy));

////Original Character Generator
//logic [7:0] character = 8'h30;
//logic [5:0] prevcy = 6'd0;
//always @(posedge clk_pixel)
//begin
//    if (cy == 10'd0)
//    begin
//        character <= 8'h30;
//        prevcy <= 6'd0;
//    end
//    else if (prevcy != cy[9:4])
//    begin
//        character <= character + 8'h01;
//        prevcy <= cy[9:4];
//    end
//end

// Create the RAM Read Enables. This probably needs refining!
CLKDivider
#(.N(8), .R(2))
U3
(
	.in(clk_pixel_x10),
	.out(RAM_RE)
);
assign RAM_CHAR_RE = RAM_RE;
assign RAM_ATTR_RE = RAM_RE;

// Calculate the RAM address. This probably needs refining!
logic [9:0] cx_mem;
logic [9:0] cy_mem;
always @(posedge clk_pixel)
begin
	cx_mem <= cx + 10'd864; // Offset cx_mem by 160
	cy_mem <= cy + 10'd980; // Offset cy_mem by 45
end
assign RAM_ADDRESS = {cy_mem[9:4], cx_mem[9:3]}; // Construct RAM_ADDRESS using an assign to save a clock cycle

//console console(.clk_pixel(clk_pixel), .codepoint(character), .attribute({cx[9], cy[8:6], cx[8:5]}), .cx(cx), .cy(cy), .rgb(rgb)); // Original code
console console(.clk_pixel(clk_pixel), .codepoint(RAM_CHAR_DATA), .attribute(RAM_ATTR_DATA), .cx(cx), .cy(cy), .rgb(rgb)); // VGA HDMI
endmodule

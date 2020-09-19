module hdmi_demo
(
  input CLOCK_PIXEL,
  input CLOCK_AUDIO,
  input CLOCK_CORE,

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
  output RAM_ATTR_RE,
  input [7:0] RAM_ROW_OFFSET // RAM row offset for fast display updates
);

logic tmds_clock;
logic RAM_RE;

// Clock divider for the heartbeat. Divide by 252,000,000 to produce 1Hz with 5:95 on:off ratio
CLKDivider
#(.N(252000000), .R(20))
U1
(
	.in(CLOCK_PIXEL),
	.out(heartbeat)
);


// Clock divider for the TMDS clock. Divide by 10 to produce 25.2MHz with 50:50 on:off ratio
CLKDivider
#(.N(10), .R(2))
U2
(
	.in(CLOCK_PIXEL),
	.out(tmds_clock)
);


localparam AUDIO_BIT_WIDTH = 16;
localparam AUDIO_RATE = 48000;
localparam WAVE_RATE = 480;

logic [AUDIO_BIT_WIDTH-1:0] audio_sample_word;
logic [AUDIO_BIT_WIDTH-1:0] audio_sample_word_dampened; // This is to avoid giving you a heart attack -- it'll be really loud if it uses the full dynamic range.

//assign audio_sample_word_dampened = audio_sample_word >> 9;
always @(posedge CLOCK_CORE)
begin
	audio_sample_word_dampened = audio_sample_word >> VOLUME;
end

sawtooth #(.BIT_WIDTH(AUDIO_BIT_WIDTH), .SAMPLE_RATE(AUDIO_RATE), .WAVE_RATE(WAVE_RATE)) sawtooth (.clk_audio(CLOCK_AUDIO), .level(audio_sample_word));

logic [23:0] rgb;
logic [9:0] cx, cy;
hdmi #(.VIDEO_ID_CODE(1), .VIDEO_REFRESH_RATE(60.00), .DDRIO(0), .AUDIO_RATE(AUDIO_RATE), .AUDIO_BIT_WIDTH(AUDIO_BIT_WIDTH)) hdmi(.clk_pixel_x10(CLOCK_PIXEL), .clk_pixel(tmds_clock), .clk_audio(CLOCK_AUDIO), .rgb(rgb), .audio_sample_word('{audio_sample_word_dampened, audio_sample_word_dampened}), .tmds_p(HDMI_TX), .tmds_clock_p(HDMI_CLK), .tmds_n(HDMI_TX_N), .tmds_clock_n(HDMI_CLK_N), .cx(cx), .cy(cy));

// Create the RAM Read Enables. This probably needs refining!
CLKDivider
#(.N(8), .R(2))
U3
(
	.in(CLOCK_PIXEL),
	.out(RAM_RE)
);
assign RAM_CHAR_RE = RAM_RE;
assign RAM_ATTR_RE = RAM_RE;

// Calculate the RAM address. This probably needs refining!
logic [9:0] cx_mem;
logic [9:0] cy_mem;
always @(posedge tmds_clock)
begin
	cx_mem <= cx + 10'd865; // Offset cx_mem
	cy_mem <= cy + 10'd980 + (RAM_ROW_OFFSET << 4); // Offset cy_mem. Add the row offset
	RAM_ADDRESS <= {cy_mem[9:4], cx_mem[9:3]}; // Construct RAM_ADDRESS
end

console console(.clk_pixel(tmds_clock), .codepoint(RAM_CHAR_DATA), .attribute(RAM_ATTR_DATA), .cx(cx), .cy(cy), .rgb(rgb)); // VGA HDMI
endmodule

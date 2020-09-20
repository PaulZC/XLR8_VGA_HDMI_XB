module sawtooth 
#(
    parameter int BIT_WIDTH = 16,
    parameter int SAMPLE_RATE = 48000
)
(
    input logic clk_audio,
    output logic signed [BIT_WIDTH-1:0] level = BIT_WIDTH'(0),
	 input logic [10:0] wave_rate,
	 input logic enable
);

int NUM_PCM_STEPS;
assign NUM_PCM_STEPS = (BIT_WIDTH + 1)'(2)**(BIT_WIDTH + 1)'(BIT_WIDTH) - 1;

int FREQUENCY_RATIO;
//assign FREQUENCY_RATIO = SAMPLE_RATE / wave_rate;

logic [BIT_WIDTH-1:0] INCREMENT;
//assign INCREMENT = BIT_WIDTH'(NUM_PCM_STEPS / FREQUENCY_RATIO);

always @(posedge clk_audio) begin
	FREQUENCY_RATIO <= SAMPLE_RATE / wave_rate;
	INCREMENT <= BIT_WIDTH'(NUM_PCM_STEPS / FREQUENCY_RATIO);
	if (enable == 1'b1)
		begin
		level <= level + INCREMENT;
		end
	else
		begin
		level <= BIT_WIDTH'(0);
		end
	end
endmodule

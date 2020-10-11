module sawtooth 
#(
    parameter int BIT_WIDTH = 16
)
(
    input logic clk_audio,
	 input logic enable,
	 input logic [BIT_WIDTH-1:0] wave_increment,
    output logic signed [BIT_WIDTH-1:0] level = BIT_WIDTH'(0)
);

always @(posedge clk_audio) begin
	if (enable == 1'b1)
		begin
		level <= level + wave_increment;
		end
	else
		begin
		level <= BIT_WIDTH'(0);
		end
	end
endmodule

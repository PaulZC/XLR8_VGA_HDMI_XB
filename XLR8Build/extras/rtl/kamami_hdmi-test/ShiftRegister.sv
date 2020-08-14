module ShiftRegister
#(parameter N)
(
	input logic clk,
	input logic [N-1:0] in,
	output logic [N-1:0] out
);

int counter = 1;

always_ff@(posedge clk)
	begin
		counter <= counter + 1;
		if (counter == N)
			counter <= 1;
	end
	
always_ff@(posedge clk)
	out <= (counter == N) ? in : out[N-1:1];
	
endmodule

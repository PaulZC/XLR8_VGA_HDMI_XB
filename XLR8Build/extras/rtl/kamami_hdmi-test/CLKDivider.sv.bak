module CLKDivider
#(parameter N)
(
	input logic in,
	output logic out
);

int counter = 1;

always_ff@(posedge in)
	begin
		counter <= counter + 1;
		if (counter == (N / 2))
			begin
				counter <= 1;
				out <= ~out;
			end
	end

endmodule 
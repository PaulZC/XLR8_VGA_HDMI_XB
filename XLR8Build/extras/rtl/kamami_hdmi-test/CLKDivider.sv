// Clock Divider: Divide by N; use R to produce desired on:off ratio

module CLKDivider
#(parameter N, parameter R)
(
	input logic in,
	output logic out
);

int counter = 1;

always_ff@(posedge in)
	begin
		counter <= counter + 1;
		if (counter == N)
			begin
				counter <= 1;
				out <= 1;
			end
		else if (counter == (N / R))
			begin
				out <= 0;
			end
	end

endmodule 
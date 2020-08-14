module DifferentialSignal
(
	input logic in,
	output logic p,
	output logic n
);

always_comb
	begin
		if (in == 1)
			begin
				p <= 1;
				n <= 0;
			end
		else
			begin
				p <= 0;
				n <= 1;
			end
	end

endmodule 
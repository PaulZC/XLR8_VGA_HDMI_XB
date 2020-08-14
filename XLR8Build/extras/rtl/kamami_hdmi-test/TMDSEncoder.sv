module TMDSEncoder
(
	input logic clk,
	input logic [7:0] VD,  
	input logic [1:0] CD, 
	input logic VDE,  		
	output logic [9:0] TMDS = 0
);

logic [3:0] N1VD;
logic [3:0] N1q_m;
logic [3:0] N0q_m;
logic xnorr;
logic [8:0] q_m;
logic [4:0] cnt0 = 0;
logic [4:0] cnt1;
logic [9:0] TMDSData;
logic [9:0] TMDSCode;
logic [1:0] calcCnt;

assign N1VD = VD[0] + VD[1] + VD[2] + VD[3] + VD[4] + VD[5] + VD[6] + VD[7];
assign xnorr = (N1VD > 4'd4) || (N1VD == 4'd4 && VD[0] == 0);
assign q_m = {~xnorr, xnorr ? q_m[6:0] ~^ VD[7:1] : q_m[6:0] ^ VD[7:1], VD[0]};
assign N1q_m = q_m[0] + q_m[1] + q_m[2] + q_m[3] + q_m[4] + q_m[5] + q_m[6] + q_m[7] + q_m[8];
assign N0q_m = 4'd8 - N1q_m;
assign calcCnt = (cnt0 == 0 || N1q_m == N0q_m) ? 
					(q_m[8] == 0 ? 2'b11 : 2'b10) : 
					(((cnt0 > 0 && N1q_m > N0q_m) || (cnt0 < 0 && N0q_m > N1q_m)) ? 2'b01 : 2'b00);
assign cnt1 = calcCnt[1] ? 
				(calcCnt[0] ? cnt0 + (N0q_m - N1q_m) :  cnt0 + (N1q_m - N0q_m)) : 
				(calcCnt[0] ? cnt0 + q_m[8] + q_m[8] + (N0q_m - N1q_m) : cnt0 - (~q_m[8] + ~q_m[8]) + (N1q_m - N0q_m));
assign TMDSData = calcCnt[1] ? 
				({~q_m[8], q_m[8], q_m[8] ? q_m[7:0] : ~q_m[7:0]}) : 
				(calcCnt[0] ? {1'b1, q_m[8], ~q_m[7:0]} : {1'b0, q_m[8], q_m[7:0]});
assign TMDSCode = CD[1] ? (CD[0] ? 10'b1010101011 : 10'b0101010100) : (CD[0] ? 10'b0010101011 : 10'b1101010100);

always_ff@(posedge clk) 
	TMDS <= VDE ? TMDSData : TMDSCode;
	
always_ff@(posedge clk) 
	cnt0 <= VDE ? cnt1 : 5'd0;


endmodule 
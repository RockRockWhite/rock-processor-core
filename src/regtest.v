`include "Register.v"

module regtest(
        input[3:0] d,
        input clk,
        output[3:0] q
    );



    Register #(.N(4)) u_Register(.q(q), .d(d), .clk(clk));

endmodule

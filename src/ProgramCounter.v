`include "Register.v"

module ProgramCounter#(parameter N = 32)(
        input clk,
        output [N-1:0] instruction_address
    );


    wire [N-1:0] pc_val;
    reg [N-1:0] pc_next;
    Register #(.N(N)) pc(.q(pc_val), .d(pc_next), .clk(clk));

    // update pc register
    always @(*) begin
        pc_next = pc_val + 4;
    end



    assign instruction_address = pc_val;
endmodule

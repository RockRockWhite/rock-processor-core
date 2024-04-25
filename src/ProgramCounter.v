`include "Register_R.v"

module ProgramCounter#(parameter N = 32)(
        input clk,
        output [N-1:0] instruction_address
    );


    wire [N-1:0] pc_val;
    reg [N-1:0] pc_next;
    Register_R #(.N(N), .INIT(32'h80000000)) pc(.q(pc_val), .d(pc_next), .rst(1'b0), .clk(clk));

    // update pc register
    always @(*) begin
        pc_next = pc_val + 4;
    end

    assign instruction_address = pc_val;
endmodule

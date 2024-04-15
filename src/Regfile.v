module Regfile #(parameter N = 32)(
        input clk,
        input [4:0] rs1,
        input [4:0] rs2,
        input [4:0] rd,
        input [N-1:0] write_data,
        input write_enable,
        output [N-1:0] read_data1,
        output [N-1:0] read_data2
    );


endmodule

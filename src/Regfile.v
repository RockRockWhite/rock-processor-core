`include "Register_CE.v"

module Regfile #(
    parameter N = 32
) (
    input clk,
    input [4:0] rs1,
    input [4:0] rs2,
    input [4:0] rd,
    input [N-1:0] write_data,
    input write_enable,
    output [N-1:0] read_data1,
    output [N-1:0] read_data2
);

    reg register_enable[31:0];
    reg [N-1:0] register_read_data[31:0];

    // generate the register file
    generate
        for (genvar i = 0; i != 32; i = i + 1) begin : regfile_block
            Register_CE #(
                .N(N)
            ) regfile (
                .q  (register_read_data[i]),
                .d  (write_data),
                .ce (register_enable[i]),
                .clk(clk)
            );
        end
    endgenerate


    // set register enable
    always @(*) begin
        for (integer i = 0; i != 32; i = i + 1) begin
            register_enable[i] = (i[4:0] == rd) ? (write_enable && (rd != 0)) : 1'b0;
        end
    end

    // set read data
    assign read_data1 = register_read_data[rs1];
    assign read_data2 = register_read_data[rs2];

endmodule

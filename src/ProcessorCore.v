`include "ProgramCounter.v"
`include "Regfile.v"
`include "ALU.v"
`include "ControlLogic.v"

import "DPI-C" function void ebreak();

module ProcesserCore(
        input clk,
        input [31:0] instruction_test,
        output [31:0] pc_test
    );

    ProgramCounter pc(.clk(clk), .instruction_address(pc_test));

    wire [31:0] reg_read_data1;
    wire [31:0] reg_read_data2;
    wire [31:0] alu_result;
    
    wire cl_register_write_enable;
        
    Regfile regfile(
                .clk(clk),
                .rs1(instruction_test[19:15]),
                .rs2(instruction_test[24:20]),
                .rd(instruction_test[11:7]),
                .write_data(alu_result),
                .write_enable(cl_register_write_enable),
                .read_data1(reg_read_data1),
                .read_data2(reg_read_data2)
            );

    ALU alu(.a(reg_read_data1), .b({{20{instruction_test[31]}}, instruction_test[31:20]}), .alu_select(4'b0), .alu_result(alu_result));


    ControlLogic cl(.instruction(instruction_test), .register_write_enable(cl_register_write_enable));
    // TODO: ebreak
    always @(*) begin

        if(instruction_test == 32'h00100073) begin
            ebreak();
        end
    end
endmodule

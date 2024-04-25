`include "ProgramCounter.v"
`include "Regfile.v"
`include "ALU.v"
`include "ControlLogic.v"
`include "ImmediateGenerator.v"

import "DPI-C" function void ebreak();

module ProcesserCore(
        input clk,
        input [31:0] instruction_test,
        output [31:0] pc_test
    );

    wire[31:0] pc_val;
    ProgramCounter pc(.clk(clk), .instruction_address(pc_val));

    wire [31:0] reg_read_data1;
    wire [31:0] reg_read_data2;

    wire [31:0] immediate;

    wire [31:0] alu_result;
    
    wire [2:0] cl_immediate_select;
    wire cl_a_select;
    wire cl_b_select;
    wire [3:0] cl_alu_select;
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


    wire [31:0] alu_a;
    wire [31:0] alu_b;


    assign alu_a = cl_a_select ? pc_val : reg_read_data1;
    assign alu_b = cl_b_select ? immediate : reg_read_data2;

    ALU alu(.a(alu_a), .b(alu_b), .alu_select(cl_alu_select), .alu_result(alu_result));

    ControlLogic cl(.instruction(instruction_test), .immediate_select(cl_immediate_select), .a_select(cl_a_select), .b_select(cl_b_select), .alu_select(cl_alu_select), .register_write_enable(cl_register_write_enable));
    // outports wire
    ImmediateGenerator imm_gen(.instruction(instruction_test), .immediate_select(cl_immediate_select), .immediate(immediate));
    
    // TODO: ebreak
    always @(*) begin

        if(instruction_test == 32'h00100073) begin
            ebreak();
        end
    end

    assign pc_test = pc_val;
endmodule

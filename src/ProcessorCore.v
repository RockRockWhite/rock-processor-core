`include "ProgramCounter.v"
`include "Regfile.v"
`include "ALU.v"
`include "ControlLogic.v"
`include "ImmediateGenerator.v"
`include "Memory.v"
`include "DataSplitter.v"

import "DPI-C" function void ebreak();

module ProcessorCore (
    input clk
);

    wire cl_pc_select;
    wire [31:0] pc_val;
    wire [31:0] alu_result;

    ProgramCounter pc (
        .clk(clk),
        .pc_select(cl_pc_select),
        .alu_result(alu_result),
        .instruction_address(pc_val)
    );

    wire [31:0] instruction;
    Memory i_memory (
        .clk(0),
        .address(pc_val),
        .data_write(0),
        .write_enable(0),
        .data_read(instruction)
    );

    wire [31:0] reg_read_data1;
    wire [31:0] reg_read_data2;

    reg  [31:0] write_back_data;
    wire [ 1:0] cl_write_back_select;

    always @(*) begin
        case (cl_write_back_select)
            2'b00: write_back_data = memory_data_splitted;
            2'b01: write_back_data = alu_result;
            2'b10: write_back_data = pc_val + 4;
            2'b11: write_back_data = 32'h00000000;
        endcase
    end

    wire [31:0] immediate;
    wire [2:0] cl_immediate_select;
    wire cl_a_select;
    wire cl_b_select;
    wire [3:0] cl_alu_select;
    wire cl_register_write_enable;

    Regfile regfile (
        .clk(clk),
        .rs1(instruction[19:15]),
        .rs2(instruction[24:20]),
        .rd(instruction[11:7]),
        .write_data(write_back_data),
        .write_enable(cl_register_write_enable),
        .read_data1(reg_read_data1),
        .read_data2(reg_read_data2)
    );

    wire [31:0] alu_a;
    wire [31:0] alu_b;

    assign alu_a = cl_a_select ? pc_val : reg_read_data1;
    assign alu_b = cl_b_select ? immediate : reg_read_data2;

    ALU alu (
        .a(alu_a),
        .b(alu_b),
        .alu_select(cl_alu_select),
        .alu_result(alu_result)
    );

    wire [31:0] memory_data;
    wire [ 3:0] cl_memory_write_enable;
    Memory d_memory (
        .clk(clk),
        .address(alu_result),
        .data_write(reg_read_data2),
        .write_enable(cl_memory_write_enable),
        .data_read(memory_data)
    );

    wire [31:0] memory_data_splitted;
    wire [ 2:0] cl_memory_split_option;

    DataSplitter memory_data_splitter (
        .data_in (memory_data),
        .option  (cl_memory_split_option),
        .data_out(memory_data_splitted)
    );

    ControlLogic cl (
        .instruction(instruction),
        .pc_select(cl_pc_select),
        .immediate_select(cl_immediate_select),
        .a_select(cl_a_select),
        .b_select(cl_b_select),
        .alu_select(cl_alu_select),
        .register_write_enable(cl_register_write_enable),
        .write_back_select(cl_write_back_select),
        .memory_split_option(cl_memory_split_option),
        .memory_write_enable(cl_memory_write_enable)
    );
    // outports wire
    ImmediateGenerator imm_gen (
        .instruction(instruction),
        .immediate_select(cl_immediate_select),
        .immediate(immediate)
    );

    // TODO: ebreak
    always @(posedge clk) begin

        if (instruction == 32'h00100073) begin
            ebreak();
        end
    end

endmodule

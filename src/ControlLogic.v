module ControlLogic(
        input [31:0] instruction,
        output [2:0] immediate_select,
        output [3:0] alu_select,
        output register_write_enable
    );
    assign register_write_enable = 1'b1;
    assign alu_select = 4'b0000;
    assign immediate_select = 3'b001;
endmodule

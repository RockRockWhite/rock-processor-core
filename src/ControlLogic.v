module ControlLogic(
    input [31:0] instruction,
    output register_write_enable
);
    assign register_write_enable = 1'b1;
endmodule
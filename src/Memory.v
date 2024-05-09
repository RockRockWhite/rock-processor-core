import "DPI-C" function int memory_read(input int addr);
import "DPI-C" function void memory_write(
    input int addr,
    input int data,
    input int size
);

module Memory (
    input clk,
    input [31:0] address,
    input [31:0] data_write,
    input [3:0] write_enable,
    output reg [31:0] data_read
);
    always @(posedge clk) begin
        case (write_enable)
            4'b0001: memory_write(address, data_write, 1);
            4'b0011: memory_write(address, data_write, 2);
            4'b1111: memory_write(address, data_write, 4);
            default: ;
        endcase
    end

    always @(*) begin
        data_read = memory_read(address);
    end
endmodule

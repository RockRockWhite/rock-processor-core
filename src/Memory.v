import "DPI-C" function int memory_read(input int addr);
import "DPI-C" function void memory_write(input int addr, input int data);

module Memory #(parameter N = 32)(
        input clk,
        input [N-1:0] address,
        input [N-1:0] data_write,
        input write_enable,
        output reg [N-1:0] data_read
    );

    always @(posedge clk) begin
        if(write_enable) begin
            memory_write(address, data_write);
        end
    end

    always @(*) begin
        data_read = memory_read(address);
    end
endmodule
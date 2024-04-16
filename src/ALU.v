module ALU #(parameter N = 32)(
        input [N-1:0] a,
        input [N-1:0] b,
        input [3:0] alu_select,
        output reg [N-1:0] alu_result
    );

    always @(*) begin
        alu_result = a + b;
    end

endmodule

module ALU #(
    parameter N = 32
) (
    input [(N - 1):0] a,
    input [(N - 1):0] b,
    input [3:0] alu_select,
    output reg [(N - 1):0] alu_result
);

    always @(*) begin
        case (alu_select)
            4'd0:  // add
            alu_result = a + b;
            4'd1:  // sll
            alu_result = a << b;
            4'd2:  // slt
            alu_result = ($signed(a) < $signed(b)) ? 1 : 0;
            4'd3:  // sltu
            alu_result = (a < b) ? 1 : 0;
            4'd4:  // xor
            alu_result = a ^ b;
            4'd5:  // srl
            alu_result = a >> b;
            4'd6:  // or
            alu_result = a | b;
            4'd7:  // and
            alu_result = a & b;
            4'd12:  // sub
            alu_result = a - b;
            4'd13: begin
                // sra
                alu_result = a;

                for (integer i = 0; i != {27'b0, b[4:0]}; i = i + 1) begin
                    alu_result = {alu_result[31], alu_result[31:1]};
                end
            end
            4'd15:  // bsel
            alu_result = b;
            default: alu_result = 0;
        endcase
    end

endmodule

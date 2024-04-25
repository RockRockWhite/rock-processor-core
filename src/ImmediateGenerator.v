module ImmediateGenerator(
        input [31:0] instruction,
        input [2:0] immediate_select,
        output reg [31:0] immediate
    );

    // 001 I-type format
    // 010 S-type format
    // 011 B-type format
    // 100 U-type format
    // 101 J-type format
    // 110 CSR instructions
    always @(*) begin
        case(immediate_select)
            3'b001:
                // I-type format
                assign immediate = {{{20{instruction[31]}}, instruction[31:20]}};
            3'b010:
                // S-type format
                assign immediate = {{20{instruction[31]}}, instruction[31:25], instruction[11:7]};
            3'b011:
                // B-type format
                assign immediate = {{19{instruction[31]}},  instruction[31], instruction[7],  instruction[30:25],  instruction[11:8], 1'b0};
            3'b100:
                // U-type format
                assign immediate = {instruction[31:12], 12'b0};
            3'b101:
                // J-type format
                assign immediate = {{11{instruction[31]}}, instruction[31], instruction[19:12], instruction[20], instruction[30:21], 1'b0};
            default:
                assign immediate = 32'b0;
        endcase
    end
endmodule

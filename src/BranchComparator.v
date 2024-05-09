module BranchComparator #(
    parameter N = 32
) (
    input [N-1:0] data1,
    input [N-1:0] data2,
    input is_unsigned,
    output reg branch_equal,
    output reg branch_less_than
);
    always @(*) begin
        branch_equal = (data1 == data2) ? 1'b1 : 1'b0;

        if (!is_unsigned) begin
            // signed
            branch_less_than = ($signed(data1) < $signed(data2)) ? 1'b1 : 1'b0;
        end else begin
            // unsigned
            branch_less_than = (data1 < data2) ? 1'b1 : 1'b0;
        end
    end
endmodule

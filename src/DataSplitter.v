module DataSplitter (
    input [31:0] data_in,
    input [2:0] option,
    output reg [31:0] data_out
);
    // 000 : pass data_in
    // 001 : pass half of data_in with sign extension
    // 010 : pass half of data_in without sign extension
    // 011 : pass one low byte of data_in with sign extension
    // 100 : pass one low byte of data_in without sign extension
    always @(*) begin
        case (option)
            3'b000:  data_out = data_in;
            3'b001:  data_out = {{16{data_in[15]}}, data_in[15:0]};
            3'b010:  data_out = {16'b0, data_in[15:0]};
            3'b011:  data_out = {{24{data_in[7]}}, data_in[7:0]};
            3'b100:  data_out = {24'b0, data_in[7:0]};
            default: data_out = 32'b0;
        endcase
    end
endmodule

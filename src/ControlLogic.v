module ControlLogic(
        input [31:0] instruction,
        output reg [2:0] immediate_select,
        output reg a_select,
        output reg b_select,
        output reg [3:0] alu_select,
        output reg register_write_enable
    );

    wire [6:0] opcode;
    wire [2:0] funct3;
    wire [6:0] funct7;

    assign opcode = instruction[6:0];
    assign funct3 = instruction[14:12];
    assign funct7 = instruction[31:25];

    always @(*) begin
        case(opcode)
            7'b0100011: begin
                // R type algorithm and logic instructions
                if(funct3 == 3'b000) begin
                    // add

                end
                else if (funct3 == 3'b001) begin

                end
                else if (funct3 == 3'b010) begin

                end
                else if (funct3 == 3'b011) begin

                end
                else if (funct3 == 3'b100) begin

                end
                else if (funct3 == 3'b101) begin

                end
                else if (funct3 == 3'b110) begin

                end
                else if (funct3 == 3'b111) begin

                end
                else begin

                end
            end
            7'b0010011: begin
                // I type algorithm and logic instructions
                a_select = 1'b0;
                b_select = 1'b1;

                immediate_select = 3'b001;
                register_write_enable = 1'b1;

                if(funct3 == 3'b000) begin
                    // addi
                    alu_select = 4'b0000;
                end
            end

            default: begin
                immediate_select = 3'b000;
                a_select = 1'b0;
                b_select = 1'b0;
                alu_select = 4'b0000;
                register_write_enable = 1'b0;
            end
        endcase
    end

endmodule

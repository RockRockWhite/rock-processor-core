module ControlLogic(
        input [31:0] instruction,
        output reg pc_select,
        output reg [2:0] immediate_select,
        output reg a_select,
        output reg b_select,
        output reg [3:0] alu_select,
        output reg register_write_enable,
        output reg memory_write_enable,
        output reg [1:0] write_back_select
    );

    wire [6:0] opcode;
    wire [2:0] funct3;
    wire [6:0] funct7;

    assign opcode = instruction[6:0];
    assign funct3 = instruction[14:12];
    assign funct7 = instruction[31:25];



    always @(*) begin
        pc_select = 1'b0;
        immediate_select = 3'b000;
        a_select = 1'b0;
        b_select = 1'b0;
        alu_select = 4'b0000;
        register_write_enable = 1'b0;
        write_back_select = 2'b00;
        memory_write_enable = 1'b0;

        case(opcode)
            7'b0110011: begin
                pc_select = 1'b0;

                a_select = 1'b0;
                b_select = 1'b0;

                register_write_enable = 1'b1;
                write_back_select = 2'b01;

                // R type algorithm and logic instructions
                if(funct3 == 3'b000 && funct7 == 7'b0000000) begin
                    // add
                    alu_select = 4'd0;
                end
                if(funct3 == 3'b000 && funct7 == 7'b0100000) begin
                    // add
                    alu_select = 4'd12;
                end
                else if (funct3 == 3'b001) begin
                    // sll
                    alu_select = 4'd1;
                end
                else if (funct3 == 3'b010) begin
                    // slt
                    alu_select = 4'd2;
                end
                else if (funct3 == 3'b011) begin
                    // sltu
                    alu_select = 4'd3;
                end
                else if (funct3 == 3'b100) begin
                    // xor
                    alu_select = 4'd4;
                end
                else if (funct3 == 3'b101 && funct7 == 7'b0000000) begin
                    // srl
                    alu_select = 4'd5;
                end
                else if (funct3 == 3'b101 && funct7 == 7'b0100000) begin
                    // sra
                    alu_select = 4'd13;
                end
                else if (funct3 == 3'b110) begin
                    // or
                    alu_select = 4'd6;
                end
                else if (funct3 == 3'b111) begin
                    // and
                    alu_select = 4'd7;
                end
                else begin

                end
            end


            7'b0010011: begin
                // I type algorithm and logic instructions
                pc_select = 1'b0;

                a_select = 1'b0;
                b_select = 1'b1;

                immediate_select = 3'b001;
                register_write_enable = 1'b1;

                write_back_select = 2'b01;

                if(funct3 == 3'b000) begin
                    // addi
                    alu_select = 4'd0;
                end
                if (funct3 == 3'b010) begin
                    // slti
                    alu_select = 4'd2;
                end
                if (funct3 == 3'b011) begin
                    // sltiu
                    alu_select = 4'd3;
                end
                if (funct3 == 3'b100) begin
                    // xori
                    alu_select = 4'd4;
                end
                if (funct3 == 3'b110) begin
                    // ori
                    alu_select = 4'd6;
                end
                if (funct3 == 3'b111) begin
                    // andi
                    alu_select = 4'd7;
                end
                if (funct3 == 3'b001 && funct7 == 7'b0000000) begin
                    // slli
                    alu_select = 4'd1;
                end
                if (funct3 == 3'b101 && funct7 == 7'b0000000) begin
                    // srli
                    alu_select = 4'd5;
                end
                if (funct3 == 3'b101 && funct7 == 7'b0100000) begin
                    // srai
                    alu_select = 4'd13;
                end
            end

            7'b1100111: begin
                // I type jalr
                pc_select = 1'b1;

                a_select = 1'b0;
                b_select = 1'b1;

                immediate_select = 3'b001;
                register_write_enable = 1'b1;

                alu_select = 4'b0;

                write_back_select = 2'b10;
            end

            7'b0110111: begin
                // U type lui
                pc_select = 1'b0;

                b_select = 1'b1;

                immediate_select = 3'b100;
                register_write_enable = 1'b1;

                alu_select = 15;

                write_back_select = 2'b01;
            end
            7'b0010111: begin
                // U type auipc
                pc_select = 1'b0;

                a_select = 1'b1;
                b_select = 1'b1;

                immediate_select = 3'b100;
                register_write_enable = 1'b1;

                alu_select = 4'b0000;

                write_back_select = 2'b01;
            end
            7'b1101111: begin
                // J type jal
                pc_select = 1'b1;

                a_select = 1'b1;
                b_select = 1'b1;

                immediate_select = 3'b101;
                register_write_enable = 1'b1;

                alu_select = 4'd0;

                write_back_select = 2'b10;
            end

            default: begin
            end

        endcase
    end

endmodule

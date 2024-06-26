// Description: BSLU, MAJ3 + NOT, 2-REG
// Author: Deyuan Guo
// Date: 07/30/2023

module bslu_maj(
  input clk,
  input [2:0] rs1,
  input [2:0] rd,
  input [4:0] op,
  output reg sa
);

reg cr, pr;

always @(posedge clk) begin
  // mov
  if (op[0]) begin
    case (rd[2:0])
    3'b001:  sa <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr));
    3'b010:  cr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr));
    3'b100:  pr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr));
    endcase
    end

  // set
  if (op[1]) begin
    case (rd[2:0])
    3'b001:  sa <= op[2];
    3'b010:  cr <= op[2];
    3'b100:  pr <= op[2];
    endcase
    end

  // maj3
  if (op[3]) begin
    case (rd[2:0])
    3'b001:  sa <= (sa & cr) | (sa & pr) | (cr & pr);
    3'b010:  cr <= (sa & cr) | (sa & pr) | (cr & pr);
    3'b100:  pr <= (sa & cr) | (sa & pr) | (cr & pr);
    endcase
    end

  // not
  if (op[4]) begin
    case (rd[2:0])
    3'b001:  sa <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr));
    3'b010:  cr <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr));
    3'b100:  pr <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr));
    endcase
    end

end // clk
endmodule


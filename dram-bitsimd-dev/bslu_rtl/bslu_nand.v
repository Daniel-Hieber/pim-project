// Description: BSLU, NAND-only, 1-REG
// Author: Deyuan Guo
// Date: 07/30/2023

module bslu_nand(
  input clk,
  input [1:0] rs1,
  input [1:0] rd,
  input [5:0] op,
  output reg sa
);

reg cr;

always @(posedge clk) begin
  // mov
  if (op[0]) begin
    case (rd[1:0])
    2'b01:  sa <= ((rs1[0] & sa) | (rs1[1] & cr));
    2'b10:  cr <= ((rs1[0] & sa) | (rs1[1] & cr));
    endcase
    end

  // set
  if (op[1]) begin
    case (rd[1:0])
    2'b01:  sa <= op[2];
    2'b10:  cr <= op[2];
    endcase
    end

  // nand
  if (op[3]) begin
    case (rd[1:0])
    2'b01:  sa <= ~(sa & cr);
    2'b10:  cr <= ~(sa & cr);
    endcase
    end

end // clk
endmodule


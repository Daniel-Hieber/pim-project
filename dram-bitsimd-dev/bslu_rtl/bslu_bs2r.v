// Description: BSLU, BitSIMD, 2-REG
// Author: Deyuan Guo
// Date: 07/30/2023

module bslu_bs2r(
  input clk,
  input [2:0] rs1,
  input [2:0] rs2,
  input [2:0] rd,
  input [7:0] op,
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

  // not
  if (op[3]) begin
    case (rd[2:0])
    3'b001:  sa <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr));
    3'b010:  cr <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr));
    3'b100:  pr <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr));
    endcase
    end

  // and
  if (op[4]) begin
    case (rd[2:0])
    3'b001:  sa <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr)) & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr));
    3'b010:  cr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr)) & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr));
    3'b100:  pr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr)) & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr));
    endcase
    end

  // or
  if (op[5]) begin
    case (rd[2:0])
    3'b001:  sa <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr)) | ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr));
    3'b010:  cr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr)) | ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr));
    3'b100:  pr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr)) | ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr));
    endcase
    end

  // xor
  if (op[6]) begin
    case (rd[2:0])
    3'b001:  sa <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr)) ^ ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr));
    3'b010:  cr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr)) ^ ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr));
    3'b100:  pr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr)) ^ ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr));
    endcase
    end

  // sel
  if (op[7]) begin
    case (rd[2:0])
    3'b001:  sa <= (pr & ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr))) ^ ( ~pr & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr)));
    3'b010:  cr <= (pr & ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr))) ^ ( ~pr & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr)));
    3'b100:  pr <= (pr & ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr))) ^ ( ~pr & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr)));
    endcase
    end

end // clk
endmodule


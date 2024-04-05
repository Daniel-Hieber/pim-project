// Description: BSLU, BitSIMD, 3-REG
// Author: Deyuan Guo
// Date: 07/30/2023

module bslu_bs3r(
  input clk,
  input [3:0] rs1,
  input [3:0] rs2,
  input [3:0] rd,
  input [7:0] op,
  output reg sa
);

reg cr, pr, t1;

always @(posedge clk) begin
  // mov
  if (op[0]) begin
    case (rd[3:0])
    4'b0001:  sa <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1));
    4'b0010:  cr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1));
    4'b0100:  pr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1));
    4'b1000:  t1 <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1));
    endcase
    end

  // set
  if (op[1]) begin
    case (rd[3:0])
    4'b0001:  sa <= op[2];
    4'b0010:  cr <= op[2];
    4'b0100:  pr <= op[2];
    4'b1000:  t1 <= op[2];
    endcase
    end

  // not
  if (op[3]) begin
    case (rd[3:0])
    4'b0001:  sa <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1));
    4'b0010:  cr <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1));
    4'b0100:  pr <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1));
    4'b1000:  t1 <= ~((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1));
    endcase
    end

  // and
  if (op[4]) begin
    case (rd[3:0])
    4'b0001:  sa <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    4'b0010:  cr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    4'b0100:  pr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    4'b1000:  t1 <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    endcase
    end

  // or
  if (op[5]) begin
    case (rd[3:0])
    4'b0001:  sa <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) | ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    4'b0010:  cr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) | ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    4'b0100:  pr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) | ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    4'b1000:  t1 <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) | ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    endcase
    end

  // xor
  if (op[6]) begin
    case (rd[3:0])
    4'b0001:  sa <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) ^ ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    4'b0010:  cr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) ^ ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    4'b0100:  pr <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) ^ ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    4'b1000:  t1 <= ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1)) ^ ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1));
    endcase
    end

  // sel
  if (op[7]) begin
    case (rd[3:0])
    4'b0001:  sa <= (pr & ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1))) ^ ( ~pr & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1)));
    4'b0010:  cr <= (pr & ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1))) ^ ( ~pr & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1)));
    4'b0100:  pr <= (pr & ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1))) ^ ( ~pr & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1)));
    4'b1000:  t1 <= (pr & ((rs1[0] & sa) | (rs1[1] & cr) | (rs1[2] & pr) | (rs1[3] & t1))) ^ ( ~pr & ((rs2[0] & sa) | (rs2[1] & cr) | (rs2[2] & pr) | (rs2[3] & t1)));
    endcase
    end

end // clk
endmodule


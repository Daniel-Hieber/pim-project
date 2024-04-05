/* Copyright (c) 2007-2009, Stanford University
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Stanford University nor the names of its 
*       contributors may be used to endorse or promote products derived from 
*       this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/ 

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "stddefines.h"

#define DRAMAP 1
#if DRAMAP
#include "dram_ap_lib.h"
#endif

typedef struct {
   char x;
   char y;
} POINT_T;

int main(int argc, char *argv[]) {

   int fd;
   char * fdata;
   char * fname;
   struct stat finfo;
   long i;


   // Make sure a filename is specified
   if (argv[1] == NULL)
   {
      printf("USAGE: %s <filename>\n", argv[0]);
      exit(1);
   }
   
   fname = argv[1];

   printf("Linear Regression Serial: Running...\n");
   
   // Read in the file
   CHECK_ERROR((fd = open(fname, O_RDONLY)) < 0);
   // Get the file info (for file length)
   CHECK_ERROR(fstat(fd, &finfo) < 0);
   // Memory map the file
   CHECK_ERROR((fdata = mmap(0, finfo.st_size + 1, 
      PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == NULL);

#if DRAMAP
   /* +++++++++++++++++++++++++ START DRAM-AP +++++++++++++++++++++++++ */

   // emulated file handler returned by dram_ap_fopen(..., fname, ...);
   input_file_handler input_points;
   input_points.data = fdata;
   input_points.size = (unsigned long long) finfo.st_size;
   // input_points.size = (long long) finfo.st_size / sizeof(POINT_T);

   int group_id = 0;
   unsigned long long vl = (unsigned long long) finfo.st_size / sizeof(POINT_T);
   printf("finfo.st_size: %lld, vl: %lld \n", (unsigned long long) finfo.st_size, vl);

   int bit_len = 8;

   // declare dram_ap vectors
   char* x_v;
   char* x_v_cpy;
   char* y_v;
   char* y_v_cpy;
   long* xx_v;
   long* yy_v;
   long* xy_v;

   long long sx_ll = 0;
   long long sy_ll = 0;
   long long sxx_ll = 0;
   long long syy_ll = 0;
   long long sxy_ll = 0;

   dram_ap_valloc(&x_v, group_id, vl, bit_len);
   dram_ap_valloc(&x_v_cpy, group_id, vl, bit_len);
   dram_ap_valloc(&y_v, group_id, vl, bit_len);
   dram_ap_valloc(&y_v_cpy, group_id, vl, bit_len);
   dram_ap_valloc_l(&xx_v, group_id, vl, bit_len); // C does not support function overload
   dram_ap_valloc_l(&yy_v, group_id, vl, bit_len);
   dram_ap_valloc_l(&xy_v, group_id, vl, bit_len);

   dram_ap_fld(&input_points, 0, 2, x_v, vl, bit_len, 0); // raw data is xyxyxyxy,...
   dram_ap_fld(&input_points, 1, 2, y_v, vl, bit_len, 0);

   dram_ap_vcpy(x_v_cpy, x_v, vl, bit_len);
   dram_ap_vcpy(y_v_cpy, y_v, vl, bit_len);

   dram_ap_vmul(xx_v, x_v, x_v_cpy, vl, bit_len);
   dram_ap_vmul(yy_v, y_v, y_v_cpy, vl, bit_len);
   dram_ap_vmul(xy_v, x_v, y_v, vl, bit_len);

   dram_ap_vredsum_c(&sx_ll, x_v, vl, bit_len);
   dram_ap_vredsum_l(&sxx_ll, xx_v, vl, bit_len);
   dram_ap_vredsum_c(&sy_ll, y_v, vl, bit_len);
   dram_ap_vredsum_l(&syy_ll, yy_v, vl, bit_len);
   dram_ap_vredsum_l(&sxy_ll, xy_v, vl, bit_len);

   if (1) {
       double a, b, xbar, ybar, r2;
       double sx = (double)sx_ll;
       double sy = (double)sy_ll;
       double sxx= (double)sxx_ll;
       double syy= (double)syy_ll;
       double sxy= (double)sxy_ll;

       b = (double)(vl*sxy - sx*sy) / (vl*sxx - sx*sx);
       a = (sy_ll - b*sx_ll) / vl;
       xbar = (double)sx_ll / vl;
       ybar = (double)sy_ll / vl;
       r2 = (double)(vl*sxy - sx*sy) * (vl*sxy - sx*sy) / ((vl*sxx - sx*sx)*(vl*syy - sy*sy));

       printf("\nLinear Regression DRAM-AP Results:\n");

       printf("\ta    = %lf\n", a);
       printf("\tb    = %lf\n", b);
       printf("\txbar = %lf\n", xbar);
       printf("\tybar = %lf\n", ybar);
       printf("\tr2   = %lf\n", r2);
       printf("\tsx_ll: %lld \n", sx_ll);
       printf("\tsxx_ll: %lld \n", sxx_ll);
       printf("\tsy_ll: %lld \n", sy_ll);
       printf("\tsyy_ll: %lld \n", syy_ll);
       printf("\tsxy_ll: %lld \n", sxy_ll);
   }

   free(x_v);
   free(x_v_cpy);
   free(y_v);
   free(y_v_cpy);
   free(xx_v);
   free(yy_v);
   free(xy_v);

   /* END DRAM-AP */
#endif

   POINT_T *points = (POINT_T*)fdata;
   long long n = (long long) finfo.st_size / sizeof(POINT_T);
   long long SX_ll = 0, SY_ll = 0, SXX_ll = 0, SYY_ll = 0, SXY_ll = 0;
   
   // ADD UP RESULTS
   for (i = 0; i < n; i++)
   {
      //Compute SX, SY, SYY, SXX, SXY
      SX_ll  += points[i].x;
      SXX_ll += points[i].x*points[i].x;
      SY_ll  += points[i].y;
      SYY_ll += points[i].y*points[i].y;
      SXY_ll += points[i].x*points[i].y;
   }

   double a, b, xbar, ybar, r2;
   double SX = (double)SX_ll;
   double SY = (double)SY_ll;
   double SXX= (double)SXX_ll;
   double SYY= (double)SYY_ll;
   double SXY= (double)SXY_ll;

   b = (double)(n*SXY - SX*SY) / (n*SXX - SX*SX);
   a = (SY_ll - b*SX_ll) / n;
   xbar = (double)SX_ll / n;
   ybar = (double)SY_ll / n;
   r2 = (double)(n*SXY - SX*SY) * (n*SXY - SX*SY) / ((n*SXX - SX*SX)*(n*SYY - SY*SY));


   printf("Linear Regression Serial Results:\n");
   printf("\ta    = %lf\n", a);
   printf("\tb    = %lf\n", b);
   printf("\txbar = %lf\n", xbar);
   printf("\tybar = %lf\n", ybar);
   printf("\tr2   = %lf\n", r2);
   printf("\tSX   = %lld\n", SX_ll);
   printf("\tSY   = %lld\n", SY_ll);
   printf("\tSXX  = %lld\n", SXX_ll);
   printf("\tSYY  = %lld\n", SYY_ll);
   printf("\tSXY  = %lld\n", SXY_ll);

   CHECK_ERROR(munmap(fdata, finfo.st_size + 1) < 0);
   CHECK_ERROR(close(fd) < 0);

   return 0;
}



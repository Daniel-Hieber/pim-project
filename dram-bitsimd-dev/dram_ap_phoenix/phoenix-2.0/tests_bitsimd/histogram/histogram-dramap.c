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

#define IMG_DATA_OFFSET_POS 10
#define BITS_PER_PIXEL_POS 28

int swap;      // to indicate if we need to swap byte order of header information


/* test_endianess
 *
 */
void test_endianess() {
   unsigned int num = 0x12345678;
   char *low = (char *)(&(num));
   if (*low ==  0x78) {
      dprintf("No need to swap\n");
      swap = 0;
   }
   else if (*low == 0x12) {
      dprintf("Need to swap\n");
      swap = 1;
   }
   else {
      printf("Error: Invalid value found in memory\n");
      exit(1);
   } 
}

/* swap_bytes
 *
 */
void swap_bytes(char *bytes, int num_bytes) {
   int i;
   char tmp;
   
   for (i = 0; i < num_bytes/2; i++) {
      dprintf("Swapping %d and %d\n", bytes[i], bytes[num_bytes - i - 1]);
      tmp = bytes[i];
      bytes[i] = bytes[num_bytes - i - 1];
      bytes[num_bytes - i - 1] = tmp;   
   }
}

int main(int argc, char *argv[]) {
      
   int i;
   int fd;
   char *fdata;
   struct stat finfo;
   char * fname;
   int red[256];
   int green[256];
   int blue[256];


   // Make sure a filename is specified
   if (argv[1] == NULL) {
      printf("USAGE: %s <bitmap filename>\n", argv[0]);
      exit(1);
   }
   
   fname = argv[1];
   
   // Read in the file
   CHECK_ERROR((fd = open(fname, O_RDONLY)) < 0);
   // Get the file info (for file length)
   CHECK_ERROR(fstat(fd, &finfo) < 0);
   // Memory map the file
   CHECK_ERROR((fdata = mmap(0, finfo.st_size + 1, 
      PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == NULL);
   
   if ((fdata[0] != 'B') || (fdata[1] != 'M')) {
      printf("File is not a valid bitmap file. Exiting\n");
      exit(1);
   }
   
   test_endianess();    // will set the variable "swap"
   
   unsigned short *bitsperpixel = (unsigned short *)(&(fdata[BITS_PER_PIXEL_POS]));
   if (swap) {
      swap_bytes((char *)(bitsperpixel), sizeof(*bitsperpixel));
   }
   if (*bitsperpixel != 24) {    // ensure its 3 bytes per pixel
      printf("Error: Invalid bitmap format - ");
      printf("This application only accepts 24-bit pictures. Exiting\n");
      exit(1);
   }
   
   unsigned short *data_pos = (unsigned short *)(&(fdata[IMG_DATA_OFFSET_POS]));
   if (swap) {
      swap_bytes((char *)(data_pos), sizeof(*data_pos));
   }
   
   int imgdata_bytes = (int)finfo.st_size - (int)(*(data_pos));
   printf("This file has %d bytes of image data, %d pixels\n", imgdata_bytes,
                                                            imgdata_bytes / 3);
                                                            
   printf("Starting sequential histogram\n");                                                            

   
   memset(&(red[0]), 0, sizeof(int) * 256);
   memset(&(green[0]), 0, sizeof(int) * 256);
   memset(&(blue[0]), 0, sizeof(int) * 256);
   
   for (i=*data_pos; i < finfo.st_size; i+=3) {      
      unsigned char *val = (unsigned char *)&(fdata[i]);
      blue[*val]++;
      
      val = (unsigned char *)&(fdata[i+1]);
      green[*val]++;
      
      val = (unsigned char *)&(fdata[i+2]);
      red[*val]++;   
   }
   
   dprintf("\n\nBlue\n");
   dprintf("----------\n\n");
   for (i = 0; i < 256; i++) {
      dprintf("%d - %d\n", i, blue[i]);        
   }
   
   dprintf("\n\nGreen\n");
   dprintf("----------\n\n");
   for (i = 0; i < 256; i++) {
      dprintf("%d - %d\n", i, green[i]);        
   }
   
   dprintf("\n\nRed\n");
   dprintf("----------\n\n");
   for (i = 0; i < 256; i++) {
      dprintf("%d - %d\n", i, red[i]);        
   }

#if DRAMAP
   /* START DRAM_AP KERNEL */
   dprintf("\n\nStart DRAM-AP Kernel\n");

   // key DRAM_AP parameters
   int bit_len = 8; 
   int group_id = 0;
   unsigned long long vl = imgdata_bytes;

   // allocate DRAM_AP arrays
   unsigned char *img_v;
   unsigned char *key_v;
   unsigned char *blue_mask_v;
   unsigned char *green_mask_v;
   unsigned char *red_mask_v;
   unsigned char *result_v;
   unsigned char *tmp_result_v;

   dram_ap_valloc(&img_v, group_id, vl, bit_len); // allocate space for input image
   dram_ap_valloc(&key_v, group_id, vl, bit_len); // allocate space for search pixel pattern
   dram_ap_valloc(&blue_mask_v, group_id, vl, bit_len); // allocate space for R/G/B mask
   dram_ap_valloc(&green_mask_v, group_id, vl, bit_len);
   dram_ap_valloc(&red_mask_v, group_id, vl, bit_len);
   dram_ap_valloc(&result_v, group_id, vl, bit_len); // allocate space for match result
   dram_ap_valloc(&tmp_result_v, group_id, vl, bit_len); // allocate space for match result

   // load DRAM_AP operands
   file_handler img_file_handler;
   img_file_handler.img_data = fdata;
   img_file_handler.imgdata_bytes = imgdata_bytes;
   img_file_handler.bits_per_pixel_pos = BITS_PER_PIXEL_POS;
   img_file_handler.img_data_offset_pos = IMG_DATA_OFFSET_POS;
   img_file_handler.data_pos = *data_pos;

   dram_ap_fld(&img_file_handler, img_v, vl, 8);
   dram_ap_vfill(100, blue_mask_v, vl); // 100100100,... initialize blue mask
   dram_ap_vfill(10, green_mask_v, vl); // 010010010,... initialize green mask
   dram_ap_vfill(1, red_mask_v, vl); // 001001001,... initialize red mask

   for (int i = 0; i < 256; i++) { // search all pixel patterns

       int blue_counter = 0;
       int green_counter = 0;
       int red_counter = 0;

       dram_ap_brdcst(0, result_v, vl, 1); // initialize paralle search results
       dram_ap_brdcst((unsigned char)i, key_v, vl, 1);

       dram_ap_xnor(result_v, key_v, img_v, vl, bit_len); // bit-serial pixel comparison

       // aggregate blue/green/red hits
       // for (int i = 0; i < vl; i++){
       //   printf("%d ", tmp_result_v[i]);
       // }
       dram_ap_and(tmp_result_v, result_v, blue_mask_v, vl, 1); 
       dram_ap_pcl(&blue_counter, tmp_result_v, vl, 1);
       blue[i] = blue_counter;

       dram_ap_and(tmp_result_v, result_v, green_mask_v, vl, 1); 
       dram_ap_pcl(&green_counter, tmp_result_v, vl, 1);
       green[i] = green_counter;

       dram_ap_and(tmp_result_v, result_v, red_mask_v, vl, 1); 
       dram_ap_pcl(&red_counter, tmp_result_v, vl, 1);
       red[i] = red_counter;

        printf("blue_counter:%d \n", blue_counter);
        printf("green_counter:%d \n", green_counter);
        printf("red_counter:%d \n", red_counter);
   }

   free(img_v);
   free(key_v);
   free(blue_mask_v);
   free(green_mask_v);
   free(red_mask_v);
   free(result_v);
   free(tmp_result_v);

   dprintf("\n\nBlue\n");
   dprintf("----------\n\n");
   for (i = 0; i < 256; i++) {
      dprintf("%d - %d\n", i, blue[i]);
   }

   dprintf("\n\nGreen\n");
   dprintf("----------\n\n");
   for (i = 0; i < 256; i++) {
      dprintf("%d - %d\n", i, green[i]);
   }

   dprintf("\n\nRed\n");
   dprintf("----------\n\n");
   for (i = 0; i < 256; i++) {
      dprintf("%d - %d\n", i, red[i]);
   }

   /* END DRAM_AP KERNEL */
#endif

   CHECK_ERROR(munmap(fdata, finfo.st_size + 1) < 0);
   CHECK_ERROR(close(fd) < 0);

   return 0;
}

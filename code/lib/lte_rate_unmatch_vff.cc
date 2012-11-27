/* -*- c++ -*- */
/* 
 * Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <lte_rate_unmatch_vff.h>
#include <cstdio>


lte_rate_unmatch_vff_sptr
lte_make_rate_unmatch_vff ()
{
	return lte_rate_unmatch_vff_sptr (new lte_rate_unmatch_vff ());
}

// class constructor
lte_rate_unmatch_vff::lte_rate_unmatch_vff ()
	: gr_sync_block ("rate_unmatch_vff",
		gr_make_io_signature ( 1, 1, sizeof (float)*120),
		gr_make_io_signature ( 1, 1, sizeof (float)*120)),
		d_work_calls(0)
{
  subblock_interleaver_prototype(40);
}

// This is the destructor.
lte_rate_unmatch_vff::~lte_rate_unmatch_vff () 
{
}

// This is the permutation of the matrix as described in the LTE standard.
const int lte_rate_unmatch_vff::d_ic_perm[] = {1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31,0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30};

// WORK function
int
lte_rate_unmatch_vff::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)		
{
	const float *in = (const float *) input_items[0];
	float *out = (float *) output_items[0];
	
	// for debugging etc.
	d_work_calls++;
		
	int len = 40;
	// Process all possible input vectors
    for (int i = 0; i < noutput_items ; i++){
    	float in1[len];
	    float in2[len];
	    float in3[len];
	    memcpy(in1,in      ,sizeof(float)*len);
	    memcpy(in2,in+len  ,sizeof(float)*len);
	    memcpy(in3,in+2*len,sizeof(float)*len);
	
	    // Deinterleave subblocks
	    float out1[len];
	    float out2[len];
	    float out3[len];
	    for (int i = 0; i < len; i++){
	    	out1[d_interl_pos[i]]=in1[i];
	    	out2[d_interl_pos[i]]=in2[i];
	    	out3[d_interl_pos[i]]=in3[i];
	    }
	
        // The Viterbi-algorithm needs the the 3 subblock streams interleaved
	    for (int i = 0; i<len; i++){
	    	*(out+i*3  ) = out1[i];
	    	*(out+i*3+1) = out2[i];
	    	*(out+i*3+2) = out3[i];
	    }
	    
	    //Move input pointers one vector ahead!
	    in+= 120;
	    out+=120;
    }

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

/* Function generates a vector with the interleaved positions of an array with a specified length.
	unfortunately, it can't be used to generate a vector with a length different from 40. (not implemented and not required)
*/
void
lte_rate_unmatch_vff::subblock_interleaver_prototype(int len)
{
	// Set and Calculate variables according to standard.
	int csub = 32;
	int rsub = (int) ceil(1.0*len/csub);
	int Nd   = csub * rsub - len;
	
	// Initialize vector array with zeros for NULL symbols
	unsigned char y[rsub*csub];
	for (int i=0;i<Nd;i++){
		y[i]=0;	
	}
	
	// Initialize vector array with non-zero values according to its position.
	for (int i=Nd;i<csub*rsub;i++){
		y[i]=i-Nd+1;
	}
	
	// Transform vector array y into matrix with 32 columns (csub) rowwise.
	unsigned char mat[rsub][csub];
	for (int r = 0;r<rsub;r++){
		for (int c=0;c<csub;c++){
			mat[r][c]=y[csub*r+c];
		}	
	}
		
	// Perform interleaving on matrix according to standard.
	unsigned char interl_mat[rsub][csub];
	for (int i=0;i<csub;i++){
		interl_mat[0][i]=mat[0][d_ic_perm[i]];
		interl_mat[1][i]=mat[1][d_ic_perm[i]];
	}

	// Read out matrix to vector columnwise.
	unsigned char v[rsub*csub];
	for (int c=0;c<csub;c++){
		for (int r=0;r<rsub;r++){
			v[rsub*c+r]=interl_mat[r][c];
		}
	}
	
	int idx = 0;
	for (int i=0;i<csub*rsub;i++){
		if (v[i] != 0) {
			d_interl_pos[idx] = v[i]-1;
			idx++;	
		}
	}
}




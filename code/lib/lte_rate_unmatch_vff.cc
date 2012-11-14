/* -*- c++ -*- */
/* 
 * Copyright 2012 Johannes Demel
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
lte_make_rate_unmatch_vff ()//int cell_id
{
	return lte_rate_unmatch_vff_sptr (new lte_rate_unmatch_vff ());//cell_id
}

// class constructor
lte_rate_unmatch_vff::lte_rate_unmatch_vff ()//int cell_id
	: gr_sync_block ("rate_unmatch_vff",
		gr_make_io_signature ( 1, 1, sizeof (float)*120),
		gr_make_io_signature ( 1, 1, sizeof (float)*120)),
		//d_cell_id(cell_id),
		d_work_calls(0),
		d_tag_v_size(0)
{
  // d_matrix = (float*) malloc(sizeof(float)*32*4);
  subblock_interleaver_prototype(40);
 
}

// This is the destructor. It is unused!
lte_rate_unmatch_vff::~lte_rate_unmatch_vff () 
{
}

const int lte_rate_unmatch_vff::d_ic_perm[] = {1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31,0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30};

// WORK function!!!
int
lte_rate_unmatch_vff::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)		
{
	const float *in = (const float *) input_items[0];
	float *out = (float *) output_items[0];
	
	//tag_propagation_policy_t tpp=tag_propagation_policy();
	//printf("%s\t%i\n" , name().c_str(), tpp );
	
	d_work_calls++;
	std::vector <gr_tag_t> v;
	get_tags_in_range(v,0,nitems_written(0),nitems_written(0)+1);
	if (v.size() > 0){
		uint64_t offset = v[0].offset;
		std::string key = pmt::pmt_symbol_to_string(v[0].key);
		long value = pmt::pmt_to_long(v[0].value);
		std::string srcid = pmt::pmt_symbol_to_string(v[0].srcid);
		
        //printf("%s\t" , name().c_str() );
		//printf("call = %i,\tv.size() = %i,\tnitems_written(0) = %i\tkey = %s\tsrcid = %s\tvalue = %ld\n",d_work_calls, int(v.size()), int(nitems_written(0)), key.c_str(), srcid.c_str(), value);
	}


	int len = 40;
	float in1[len];
	float in2[len];
	float in3[len];
	
	memcpy(in1,in      ,sizeof(float)*len);
	memcpy(in2,in+len  ,sizeof(float)*len);
	memcpy(in3,in+2*len,sizeof(float)*len);
	
	float out1[len];
	float out2[len];
	float out3[len];
	for (int i = 0; i < len; i++){
		out1[d_interl_pos[i]]=in1[i];
		out2[d_interl_pos[i]]=in2[i];
		out3[d_interl_pos[i]]=in3[i];
	}

	for (int i = 0; i<len; i++){
		*(out+i*3  ) = out1[i];
		*(out+i*3+1) = out2[i];
		*(out+i*3+2) = out3[i];
	}


	// printf("noutput_items: %i\n\n", noutput_items);

	// Tell runtime system how many output items we produced.
	return 1; //noutput_items;
}

/* Function generates a vector with the interleaved positions of an array with a specified length.
	unfortunately, it can't be used to generate a vector with a length different from 40.
*/
void
lte_rate_unmatch_vff::subblock_interleaver_prototype(int len)
{

	// printf("len: %i\n",len);
	// Set and Calculate variables according to standard.
	int csub = 32;
	int rsub = (int) ceil(1.0*len/csub);
	int Nd   = csub * rsub - len;
	
	// printf("csub: %i,\trsub: %i,\tNd: %i\n",csub,rsub,Nd);
	
	// Initialize vector array with zeros for NULL symbols
	unsigned char y[rsub*csub];
	for (int i=0;i<Nd;i++){
		y[i]=0;	
	}
	// Initialize vector array with non-zero values according to its position.
	for (int i=Nd;i<csub*rsub;i++){
		y[i]=i-Nd+1;
	}
	
	// printf("Transform to matrix\n");
	// Transform vector array y into matrix with 32 columns (csub) rowwise.
	unsigned char mat[rsub][csub];
	for (int r = 0;r<rsub;r++){
		for (int c=0;c<csub;c++){
			mat[r][c]=y[csub*r+c];
			// printf("%i\t",y[csub*r+c]);
		}
		// printf("\n");	
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
			// printf("read out matrix: %i\t%i\t%i\n",v[rsub*c+r],rsub*c,r);
		}
	}
	
	int idx = 0;
	for (int i=0;i<csub*rsub;i++){
		//printf("final loop: %i\t%i",i,v[i]);
		if (v[i] != 0) {
			d_interl_pos[idx] = v[i]-1;
			//printf("\tidx %i,\tcsub*rsub: %i,\tval: %i",idx,csub*rsub,d_interl_pos[idx]);
			idx++;
			
		}
		//printf("\n");
	}
}




/* -*- c++ -*- */
/* 
 * Copyright 2013 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

#include <gnuradio/io_signature.h>
#include "subblock_deinterleaver_vfvf_impl.h"
#include <cstdio>

namespace gr {
  namespace lte {

    subblock_deinterleaver_vfvf::sptr
    subblock_deinterleaver_vfvf::make(int num_groups, int items_per_group, std::string name)
    {
      return gnuradio::get_initial_sptr
        (new subblock_deinterleaver_vfvf_impl(num_groups, items_per_group, name));
    }

    /*
     * The private constructor
     */
    subblock_deinterleaver_vfvf_impl::subblock_deinterleaver_vfvf_impl(int num_groups, int items_per_group, std::string& name)
      : gr::sync_block(name,
              gr::io_signature::make( 1, 1, sizeof(float)*num_groups*items_per_group ),
              gr::io_signature::make( 1, 1, sizeof(float)*num_groups*items_per_group)),
              d_num_groups(num_groups),
              d_items_per_group(items_per_group)
    {
		d_interleaved_pos = subblock_interleaver_prototype(num_groups);
	}
    
    const int subblock_deinterleaver_vfvf_impl::d_perm_vec[] = {1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31,0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30};

    /*
     * Our virtual destructor.
     */
    subblock_deinterleaver_vfvf_impl::~subblock_deinterleaver_vfvf_impl()
    {
    }

    int
    subblock_deinterleaver_vfvf_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0];
        float *out = (float *) output_items[0];

		for (int i = 0 ; i < noutput_items ; i++ ){
			for(int c = 0 ; c < d_num_groups ; c++){
				memcpy(out+d_interleaved_pos[c]*d_items_per_group, in+c*d_items_per_group, sizeof(float)*d_items_per_group );
			}
			in += (d_num_groups*d_items_per_group);
			out += (d_num_groups*d_items_per_group);
		}

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }
    
	std::vector<int>
	subblock_deinterleaver_vfvf_impl::subblock_interleaver_prototype(int len)
	{
		// Set and Calculate variables according to standard.
		int csub = 32;
		int rsub = (int) ceil(1.0*len/csub);
		int Nd   = csub * rsub - len;
		
		// Initialize vector array with zeros for NULL symbols
		unsigned int y[rsub*csub];
		for (int i=0;i<Nd;i++){
			y[i]=0;	
		}
		
		// Initialize vector array with non-zero values according to its position.
		for (int i=Nd;i<csub*rsub;i++){
			y[i]=i-Nd+1;
		}
		
		// Transform vector array y into matrix with 32 columns (csub) rowwise.
		unsigned int mat[rsub][csub];
		for (int r = 0;r<rsub;r++){
			for (int c=0;c<csub;c++){
				mat[r][c]=y[csub*r+c];
			}	
		}
			
		// Perform interleaving on matrix according to standard.
		unsigned int interl_mat[rsub][csub];
		for(int r = 0 ; r < rsub ; r++ ){
			for (int i=0;i<csub;i++){
				interl_mat[r][i]=mat[r][d_perm_vec[i]];
			}	
		}

		// Read out matrix to vector columnwise.
		unsigned int v[rsub*csub];
		for (int c=0;c<csub;c++){
			for (int r=0;r<rsub;r++){
				v[rsub*c+r]=interl_mat[r][c];
			}
		}
		
		int idx = 0;
		std::vector<int> interleaved_pos;
		for (int i=0;i<csub*rsub;i++){
			if (v[i] != 0) {
				//~ d_interl_pos[idx] = v[i]-1;
				idx++;
				interleaved_pos.push_back(v[i]-1);
			}
		}
		return interleaved_pos;
	}

  } /* namespace lte */
} /* namespace gr */


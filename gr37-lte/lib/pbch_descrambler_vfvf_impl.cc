/* -*- c++ -*- */
/* 
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
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
#include "pbch_descrambler_vfvf_impl.h"

#include <fftw3.h>
#include <volk/volk.h>

#include <cstdio>

namespace gr {
  namespace lte {

    pbch_descrambler_vfvf::sptr
    pbch_descrambler_vfvf::make(std::string key)
    {
      return gnuradio::get_initial_sptr
        (new pbch_descrambler_vfvf_impl(key));
    }

    /*
     * The private constructor
     */
    pbch_descrambler_vfvf_impl::pbch_descrambler_vfvf_impl(std::string key)
      : gr::sync_interpolator("pbch_descrambler_vfvf",
              gr::io_signature::make( 1, 1, sizeof(float) * 480),
              gr::io_signature::make( 1, 1, sizeof(float) * 120), 32),
              d_cell_id(-1),
			  d_work_call(0),
			  d_pn_seq_len(1920)
    {
		message_port_register_in(pmt::mp("cell_id"));
		set_msg_handler(pmt::mp("cell_id"), boost::bind(&pbch_descrambler_vfvf_impl::set_cell_id_msg, this, _1));

		// set PMT blob info
		d_key=pmt::string_to_symbol(key);
		d_tag_id=pmt::string_to_symbol(name() );

		d_scr = (float*)fftwf_malloc(sizeof(float)*d_pn_seq_len);
		d_pn_seq = (float*)fftwf_malloc(sizeof(float)*d_pn_seq_len);
		set_cell_id(124); // remove as soon as possible
	}

    /*
     * Our virtual destructor.
     */
    pbch_descrambler_vfvf_impl::~pbch_descrambler_vfvf_impl()
    {
    }

    int
    pbch_descrambler_vfvf_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
		const float *in = (const float *) input_items[0];
		float *out = (float *) output_items[0];

		// If Cell ID is not set, do not process anything!
		if(d_cell_id < 0){
			return 0;
		}

		d_work_call++;

		// Read in vector and make copies for a 1920-element vector.
		int len_in=d_pn_seq_len/4;
		memcpy(d_scr         ,in,sizeof(float)*len_in );
		memcpy(d_scr+len_in  ,in,sizeof(float)*len_in );
		memcpy(d_scr+2*len_in,in,sizeof(float)*len_in );
		memcpy(d_scr+3*len_in,in,sizeof(float)*len_in );

		// Calculate actual descrambled sequence
		// Write to output buffer
		volk_32f_x2_multiply_32f_a(d_scr, d_scr, d_pn_seq, d_pn_seq_len);
		memcpy(out+d_pn_seq_len, d_scr, sizeof(float)*d_pn_seq_len);

		// Soft combining
		const float quarter = 0.25f;
		for(int i = 0; i < 4; i++){
			for(int c = 1; c < 4; c++){
				volk_32f_x2_add_32f_a(d_scr+480*i, d_scr+480*i, d_scr+480*i+120*c, 120 );
			}
			volk_32f_s32f_multiply_32f_u(d_scr+480*i, d_scr+480*i, quarter, 120);
		}

		// Write to output buffer
		for (int i = 0 ; i < 4 ; i++ ){
			memcpy(out+120*i      ,d_scr+0    ,sizeof(float) *120);
			memcpy(out+120*i+480  ,d_scr+480  ,sizeof(float) *120);
			memcpy(out+120*i+480*2,d_scr+480*2,sizeof(float) *120);
			memcpy(out+120*i+480*3,d_scr+480*3,sizeof(float) *120);
		}

		for (int i = 0 ; i < 32 ; i++){
			add_item_tag(0,nitems_written(0)+i,d_key, pmt::from_long(i),d_tag_id);
		}

		// Tell runtime system how many output items we produced.
		return 32; // noutput_items;
    }
    
	char*
	pbch_descrambler_vfvf_impl::pn_seq_generator(int len, int cell_id)
	{
		const int Nc=1600; //Constant is defined in standard
		int cinit=cell_id;

		char x2[3*len+Nc];
		for (int i = 0; i<31; i++){
				char val = cinit%2;
				cinit = floor(cinit/2);
				x2[i] = val;
		}

		char *c=new char[len];
		char x1[3*len+Nc];
		// initialization for first 35 elements is needed. (Otherwise READ BEFORE WRITE ERROR)
		for (int i = 0 ; i < 35 ; i++){
			x1[i]=0;
		}
		x1[0]=1;

		for (int n=0; n <2*len+Nc-3;n++){
			x1[n+31]=(x1[n+3]+x1[n])%2;
			x2[n+31]=(x2[n+3]+x2[n+2]+x2[n+1]+x2[n])%2;
		}

		for (int n=0;n<len;n++){
			c[n]=(x1[n+Nc]+x2[n+Nc])%2;
		}

		return c;
	}

	std::vector<int>
	pbch_descrambler_vfvf_impl::get_pn_sequence()
	{
		std::vector<int> pn_seq;
		for(int i = 0 ; i < d_pn_seq_len; i++){
			pn_seq.push_back(int(d_pn_seq[i]) );
		}
		return pn_seq;
	}


	void
	pbch_descrambler_vfvf_impl::set_cell_id_msg(pmt::pmt_t msg)
	{
		int cell_id = int(pmt::to_long(msg));
		//printf("********%s INPUT MESSAGE***************\n", name().c_str() );
		//printf("\t%i\n", cell_id);
		//printf("********%s INPUT MESSAGE***************\n", name().c_str() );
		set_cell_id(cell_id);
	}

	void
	pbch_descrambler_vfvf_impl::set_cell_id(int id)
	{
		if(id == d_cell_id){return;}
		printf("%s\tset_cell_id = %i\n", name().c_str(), id);
		//int len=1920;
		d_pn_seq_len=1920;
		char *pn_seq0 = pn_seq_generator(d_pn_seq_len, id);
		//d_pn_seq = new char[len];
		//NRZ coding of pn sequence
		for (int i = 0 ; i<d_pn_seq_len; i++){
			d_pn_seq[i]=1-2*pn_seq0[i];
		}
		d_cell_id = id;
	}

  } /* namespace lte */
} /* namespace gr */


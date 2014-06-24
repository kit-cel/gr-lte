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
#include "mimo_pss_coarse_sync_impl.h"

#include <cstdio>
#include <cmath>
#include <fftw3.h>
#include <volk/volk.h>

namespace gr {
  namespace lte {

    mimo_pss_coarse_sync::sptr
    mimo_pss_coarse_sync::make(int syncl)
    {
      return gnuradio::get_initial_sptr
        (new mimo_pss_coarse_sync_impl(syncl));
    }

    /*
     * The private constructor
     */
    mimo_pss_coarse_sync_impl::mimo_pss_coarse_sync_impl(int syncl)
      : gr::sync_block("mimo_pss_coarse_sync",
              gr::io_signature::make(2, 2, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
              d_syncl(syncl),
	      d_work_call(0),
              d_posmax(0),
	      d_max(0)
    {

	//make sure that there are enough input items for
	//a 128 point correalation at position 9600 (index=9599)
	set_output_multiple(d_TIME_HYPO+d_CORRL-1);


        d_port_N_id_2 = pmt::string_to_symbol("N_id_2");
        message_port_register_out(d_port_N_id_2);
        d_port_coarse_pss = pmt::string_to_symbol("coarse_pss");
        message_port_register_out(d_port_coarse_pss);


	size_t alig = volk_get_alignment();
        //d_result = (gr_complex*) volk_malloc(sizeof(gr_complex)*d_TIME_HYPO, alig);
        d_a = (gr_complex*) volk_malloc(sizeof(gr_complex)*4, alig);

	//float/gr_complex can be set to 0 since binary representation of 0.0 is 0
	memset(d_result, 0, sizeof(float)*d_TIME_HYPO);

	prepare_corr_vecs();
    }

    /*
     * Our virtual destructor.
     */
    mimo_pss_coarse_sync_impl::~mimo_pss_coarse_sync_impl()
    {
        volk_free(d_result);
        volk_free(d_a);
    }    

   
    int
    mimo_pss_coarse_sync_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
	const gr_complex *in1 = (const gr_complex *) input_items[0];
	const gr_complex *in2 = (const gr_complex *) input_items[1];
	
	//deactivate block after synclen ist reached
	if(d_work_call==d_syncl)
           return noutput_items;

	//printf("---BEGIN coarse timing---\n");
	
	for(int d = 0; d < d_TIME_HYPO; d++){        		
	   //TODO use aligned vectors, faster?, memcpy(tmp, in,...)	
           d_result[d] += diff_corr2(in1+d, in2+d, d_pss012_t);

	   if(d_result[d]>d_max){
              d_max=d_result[d];
              d_posmax=d;
              printf("new coarse timing max found: val=%f \t pos=%i\t syncloop=%i\n", d_max, d_posmax, d_work_call+1);
	    }
	 }                    
           
	
	//printf("---END coarse timing---\n");

	d_work_call++;

	if(d_work_call!=d_syncl)
	    return d_TIME_HYPO;

	//obtain N_id_2 after coarse timinig sync
	//for now only last received vector is used, 
	d_N_id_2 = calc_N_id_2(in1, in2, d_posmax);
	message_port_pub(d_port_N_id_2, pmt::from_long((long)d_N_id_2));

	printf("found N_id_2=%i\n", d_N_id_2);

	printf("coarse pss-pos=%i\n", d_posmax);

	

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }



    int mimo_pss_coarse_sync_impl::calc_N_id_2(const gr_complex* in1, const gr_complex* in2, const int &mpos){

	float max0;
	max0=diff_corr2(in1+mpos, in2+mpos, d_pss0_t);

	float max1;
	max1=diff_corr2(in1+mpos, in2+mpos, d_pss1_t);

	float max2;
	max2=diff_corr2(in1+mpos, in2+mpos, d_pss2_t);
	
	int id=0;
	float max=max0;

	if(max1 > max){
	   id=1;
	   max=max1;
	}
	if(max2 > max){
	   id=2;
	}

	return id;
    }

    //for better readability, differential correlation for both streams
    float mimo_pss_coarse_sync_impl::diff_corr2(const gr_complex* x1, const gr_complex* x2, const gr_complex* y){	
	return diff_corr(x1, y) + diff_corr(x2, y);
    }
    

    //calculate 128 point differential correlation, 4parts
    //TODO: variable length?
   float mimo_pss_coarse_sync_impl::diff_corr(const gr_complex* x,const gr_complex* y)
   {
           volk_32fc_x2_dot_prod_32fc(d_a,   x,    y,    32); 
	   volk_32fc_x2_dot_prod_32fc(d_a+1, x+32, y+32, 32);
	   volk_32fc_x2_dot_prod_32fc(d_a+2, x+64, y+64, 32);
	   volk_32fc_x2_dot_prod_32fc(d_a+3, x+96, y+96, 32);

           return abs(d_a[0]*conj(d_a[1]) + d_a[1]*conj(d_a[2]) + d_a[2]*conj(d_a[3]));
    }


    // Define imaginary constant
    const gr_complex mimo_pss_coarse_sync_impl::d_C_I = gr_complex(0,1);

    // Define PI for use in this block
    const float mimo_pss_coarse_sync_impl::d_PI = float(M_PI);


    void
    mimo_pss_coarse_sync_impl::prepare_corr_vecs()
    {
	//init pss sequences in time domain
	gen_pss_t(d_pss0_t,0);
	gen_pss_t(d_pss1_t,1);
	gen_pss_t(d_pss2_t,2);

	//add pss for correlation	
	for(int i=0; i<128; i++){
           d_pss012_t[i]=conj(d_pss0_t[i]+d_pss1_t[i]+d_pss2_t[i]);
	}

    }


    //generate pss in time domain with fftw
    void 
    mimo_pss_coarse_sync_impl::gen_pss_t(gr_complex *zc_t, int cell_id)
    {
        gr_complex zc_f[62];
        zc(zc_f, cell_id);

        gr_complex* d_in  = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*128);
        gr_complex* d_out = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*128);

        memset(d_in, 0, sizeof(gr_complex)*128);
	memcpy(d_in+97, zc_f, sizeof(gr_complex)*31);
	memcpy(d_in+1, zc_f+31, sizeof(gr_complex)*31);
	
        fftwf_plan p = fftwf_plan_dft_1d(128, reinterpret_cast<fftwf_complex*>(d_in), reinterpret_cast<fftwf_complex*>(d_out), FFTW_BACKWARD, FFTW_ESTIMATE);

        fftwf_execute(p);

        memcpy(zc_t, d_out, sizeof(gr_complex)*128);
	
//	for(int i=0; i<128; i++){
//	    printf("%i: freq:%f+j%f   time:%f+j%f\n", i, real(d_in[i]), imag(d_in[i]), real(zc_t[i]), imag(zc_t[i])); 
//	}
	
        fftwf_destroy_plan(p);        
        fftwf_free(d_in);
        fftwf_free(d_out);
    }


    void
    mimo_pss_coarse_sync_impl::zc(gr_complex *zc, int cell_id)
    {
        // calculate value of variable u according to cell id number
        float u=0;
        switch (cell_id){
            case 0: u=25.0; break;
            case 1: u=29.0; break;
            case 2: u=34.0; break;
        }

        //generate zadoff-chu sequences 
        for(int n = 0; n < 31; n++){	
            zc[n]=exp(d_C_I* gr_complex(d_PI*u* float(-1*n*(n+1))/63.0 ) );
        }
        for(int n = 31; n < 62; n++){	
            zc[n]=exp(d_C_I* gr_complex(d_PI*u* float(-1*(n+1)*(n+2))/63.0 ) );
        }
    }

  } /* namespace lte */
} /* namespace gr */


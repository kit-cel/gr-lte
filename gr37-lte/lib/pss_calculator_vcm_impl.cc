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
#include "pss_calculator_vcm_impl.h"

#include <cstdio>
#include <cmath>
#include <fftw3.h>
#include <volk/volk.h>

namespace gr {
  namespace lte {

    pss_calculator_vcm::sptr
    pss_calculator_vcm::make(int fftl)
    {
      return gnuradio::get_initial_sptr
        (new pss_calculator_vcm_impl(fftl));
    }

    /*
     * The private constructor
     */
    pss_calculator_vcm_impl::pss_calculator_vcm_impl(int fftl)
      : gr::sync_block("pss_calculator_vcm",
              gr::io_signature::make(1, 1, sizeof(gr_complex) * 72),
              gr::io_signature::make(0, 0, 0)),
                //~ d_tag(tag),
                //~ d_sel(sel),
                d_fftl(fftl),
                d_cpl(144*fftl/2048),
                d_cpl0(160*fftl/2048),
                d_slotl(7*fftl+6*d_cpl+d_cpl0),
                d_N_id_2(-1),
                d_half_frame_start(0),
                d_corr_val(0.0),
                d_lock_count(0),
                d_is_locked(false)
    {
        d_port_lock = pmt::string_to_symbol("lock");
        message_port_register_out(d_port_lock);
        d_port_half_frame_start = pmt::string_to_symbol("half_frame");
        message_port_register_out(d_port_half_frame_start);
        d_port_N_id_2 = pmt::string_to_symbol("N_id_2");
        message_port_register_out(d_port_N_id_2);
        
        zc(d_chu0,0);
        zc(d_chu1,1);
        zc(d_chu2,2);

        d_corr_in1   = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*63 );
        d_corr_in2   = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*63 );
        d_corr_out   = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*63 );
        d_correlator = new correlator(d_corr_in1, d_corr_in2, d_corr_out, 63);
    }

    /*
     * Our virtual destructor.
     */
    pss_calculator_vcm_impl::~pss_calculator_vcm_impl()
    {
        fftwf_free(d_corr_in1);
        fftwf_free(d_corr_in2);
        fftwf_free(d_corr_out);
        delete d_correlator;
    }
    
    // Define imaginary constant
    const gr_complex
    pss_calculator_vcm_impl::d_C_I = gr_complex(0,1);

    // Define PI for use in this block
    const float
    pss_calculator_vcm_impl::d_PI = float(M_PI);

    int
    pss_calculator_vcm_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        	// This is a sink block. It does not compute any output.
        const gr_complex *in = (const gr_complex *) input_items[0];

        // variable used in multiple positions
        long nir = nitems_read(0);

        bool changed = false;
        for(int i = 0 ; i < noutput_items ; i++){
            //extract PSS from its carriers.
            gr_complex chuX[63] = {0};
            extract_pss(chuX, in);
            in += 72; //move pointer on input buffer by one input vector

            // tracking does need less cross correlation calculations!
            if(d_is_locked){ changed = tracking(chuX); }
            else{ changed = find_pss_symbol(chuX); }

            //Do things if new max is found!
            if(changed){
                d_lock_count = 0; // reset lock count!
                int half_frame_start = calculate_half_frame_start(nir+i);

                if(d_half_frame_start != half_frame_start ){
                    if(!d_is_locked){
                        printf("\n%s NEW half_frame_start = %ld\tN_id_2 = %i\tcorr_val = %f\n\n",name().c_str(), d_half_frame_start, d_N_id_2, d_corr_val );
                        //~ (*d_tag).set_N_id_2(d_N_id_2); // only set a new Cell ID number if not yet locked!
                        message_port_pub(d_port_N_id_2, pmt::from_long((long)d_N_id_2));
                        d_half_frame_start = half_frame_start;
                    }
                    else if( abs(d_half_frame_start-half_frame_start) < 2 ){ //only moves by one sample in tracking mode!
                        if( d_half_frame_start < half_frame_start ){d_half_frame_start++;}
                        else{d_half_frame_start--;}
                    }
                    set_half_frame_start();
                }
            }
            else{d_lock_count++;}
        }

        // After a certain amount of unchanged N_id_2 calculations. calculation is stopped and block has no further function.
        if( !d_is_locked && d_lock_count > 300 && d_N_id_2 >=0 ){
            printf("\n%s is locked! half_frame_start = %ld\tN_id_2 = %i\tcorr_val = %f\n\n",name().c_str(), d_half_frame_start, d_N_id_2, d_corr_val );
            d_is_locked = true;
            //~ (*d_tag).lock();
            message_port_pub( d_port_lock, pmt::PMT_T );
            //~ (*d_sel).lock();
        }

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }
    
    void
    pss_calculator_vcm_impl::zc(gr_complex *zc, int cell_id)
    {
        // calculate value of variable u according to cell id number
        float u=0;
        switch (cell_id){
            case 0: u=25.0; break;
            case 1: u=29.0; break;
            case 2: u=34.0; break;
        }

        // generate zadoff-chu sequences according to original algorithm
        //gr_complex zcs[63];
        for(int n = 0; n < 63; n++){
            zc[n]=exp(d_C_I* gr_complex(d_PI*u* float(-1*n    *(n+1))/63.0 ) );
        }
        //remove DC carrier (maybe changed in the future)
        //memcpy(zc, zcs, sizeof(gr_complex)*63);
    }

    void
    pss_calculator_vcm_impl::max_pos(float &max, int &pos, gr_complex *x,gr_complex *y, int len)
    {
        //This is the new correlation class with use of fftw
        memcpy(d_corr_in1, x, sizeof(gr_complex)*len );
        memcpy(d_corr_in2, y, sizeof(gr_complex)*len );
        d_correlator->execute();
        d_correlator->get_maximum(pos, max);
    }


    bool
    pss_calculator_vcm_impl::find_pss_symbol(gr_complex *chuX)
    {
        int len = 63;

        float max0 = 0.0;
        int pos0 = 0;
        max_pos(max0, pos0, d_chu0, chuX, len);

        float max1 = 0.0;
        int pos1 = 0;
        max_pos(max1, pos1, d_chu1, chuX, len);

        float max2 = 0.0;
        int pos2 = 0;
        max_pos(max2, pos2, d_chu2, chuX, len);

        int N_id_2 = -1;
        float max[3];
        max[0]=max0;
        max[1]=max1;
        max[2]=max2;

        float maxc = 0.0;
        for(int i = 0; i < 3; i++){
            if(max[i] > maxc){
                maxc = max[i];
                N_id_2 = i;
            }
        }

        //Calculate return value
        bool has_changed = false;
        if(d_corr_val < maxc){
            has_changed = true;
            d_N_id_2 = N_id_2;
            d_corr_val = maxc;
        }
        return has_changed;
    }

    bool
    pss_calculator_vcm_impl::tracking(gr_complex *chu)
    {
        int len = 63;
        int pos = 0;
        float max = 0.0;
        switch(d_N_id_2){
            case 0: max_pos(max, pos, d_chu0, chu, len); break;
            case 1: max_pos(max, pos, d_chu1, chu, len); break;
            case 2: max_pos(max, pos, d_chu2, chu, len); break;
        }
        if(d_corr_val < max){
            d_corr_val = max;
            return true;
        }
        else{
            d_corr_val = d_corr_val*0.999;
        }

        return false;
    }
    
    // convenience function fpr better readability
    void
    pss_calculator_vcm_impl::set_half_frame_start()
    {
        pmt::pmt_t msg = pmt::from_long( d_half_frame_start) ;
        message_port_pub( d_port_half_frame_start, msg );
        
        //~ (*d_tag).set_half_frame_start(d_half_frame_start);
        //~ (*d_sel).set_half_frame_start(d_half_frame_start);
    }

    //convenience method for better readability
    //get tag from stream --> get tag.offset --> do some calculation --> return half_frame_start
    int
    pss_calculator_vcm_impl::calculate_half_frame_start(long pos)
    {
        std::vector <gr::tag_t> v_off;
        get_tags_in_range(v_off, 0, pos, pos+1);
        long offset = pmt::to_long(v_off[0].value) - (6*d_fftl+5*d_cpl+d_cpl0);
        return int( offset%(10*d_slotl) );
    }

    //convenience method for better readability
    //copy PSS to array of length 63!
    void
    pss_calculator_vcm_impl::extract_pss(gr_complex *chu, const gr_complex *in)
    {
        memcpy(chu   , in+5   , sizeof(gr_complex)*31 );
        memcpy(chu+32, in+5+31, sizeof(gr_complex)*31 );
    }

  } /* namespace lte */
} /* namespace gr */


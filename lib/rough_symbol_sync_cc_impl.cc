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
#include "rough_symbol_sync_cc_impl.h"

#include <fftw3.h>
#include <volk/volk.h>

#include <cstdio>

namespace gr {
  namespace lte {

    rough_symbol_sync_cc::sptr
    rough_symbol_sync_cc::make(int fftl, int vlen, std::string name)
    {
      return gnuradio::get_initial_sptr
        (new rough_symbol_sync_cc_impl(fftl, vlen, name));
    }

    /*
     * The private constructor
     */
    rough_symbol_sync_cc_impl::rough_symbol_sync_cc_impl(int fftl, int vlen, std::string& name)
      : gr::sync_block(name,
              gr::io_signature::make( 1, 1, sizeof(gr_complex)*vlen),
              gr::io_signature::make( 1, 1, sizeof(gr_complex)*vlen)),
                d_fftl(fftl),
                d_cpl(144*fftl/2048),
                d_cpl0(160*fftl/2048),
                d_slotl(7*fftl+6*d_cpl+d_cpl0),
                d_sym_pos(0),
                d_corr_val(0.0),
                d_work_call(0),
		d_vlen(vlen)
    {
        d_key=pmt::string_to_symbol("symbol");
        d_tag_id=pmt::string_to_symbol(this->name() );

        d_cp0 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_cpl0*d_vlen);
        d_cp1 = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_cpl0*d_vlen);
        d_res = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_cpl0*d_vlen);
    }

    /*
     * Our virtual destructor.
     */
    rough_symbol_sync_cc_impl::~rough_symbol_sync_cc_impl()
    {
        fftwf_free(d_cp0);
        fftwf_free(d_cp1);
        fftwf_free(d_res);
    }




    void rough_symbol_sync_cc_impl::forecast(int noutput_items, 
				gr_vector_int &ninput_items_required)
    {
	//more than fftl+stp input items are required for correlation (standart too small)
	//maybe block before cp-sync requires bigger output buffer

       
	//~16*4 correlations in work function
	if(noutput_items< d_fftl+d_cpl*16){
	    ninput_items_required[0]=d_fftl+d_cpl*16;
	}
	else
	{
	    ninput_items_required[0]=noutput_items;
	}
    }



    int
    rough_symbol_sync_cc_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        printf("%s.work\tnoutput_items = %i\tnitems_read = %ld\n", name().c_str(), noutput_items, nitems_read(0) );

        if(nitems_read(0) > 100000){
            add_item_tag(0,nitems_read(0)+5,d_key, pmt::from_long(d_sym_pos),d_tag_id);
            d_work_call++;
            memcpy(out,in,sizeof(gr_complex)*noutput_items*d_vlen );
            return noutput_items;
        }

        int stp = d_cpl0/4;
        int nout = 0;

        int coarse_pos = 0;
        gr_complex it_val = 0;

        for(int i = 0; i < d_cpl*15-stp; i+=stp){  
      //for(int i = 0; i < d_fftl+d_cpl*16 - (d_fftl+d_cpl+stp); i+=stp){

            memcpy(d_cp0,in+i*d_vlen          ,sizeof(gr_complex)*d_cpl*d_vlen);
            memcpy(d_cp1,in+(i+d_fftl)*d_vlen ,sizeof(gr_complex)*d_cpl*d_vlen);
            gr_complex val = corr(d_res,d_cp0,d_cp1,d_cpl*d_vlen);

            if(abs(it_val) < abs(val) ){
                coarse_pos = i;
                if(d_corr_val < abs(val) ){
                    d_corr_val = abs(val);
                    d_sym_pos = (nitems_read(0) + coarse_pos)%d_slotl;
                  //printf("%s %i\tNEW coarse max!\tval = %f\tcoarse_pos = %i\t",name().c_str(), d_work_call, abs(val), coarse_pos );
                  //printf("corr_val = %f\tsym_pos = %ld\n", d_corr_val, d_sym_pos);
                }
                it_val = val;
            }
            //if(nitems_read(0) < 10000){
            //    printf("%s\tcorr_val = %f\tnitems_read = %ld\n", name().c_str(), abs(val), nitems_read(0) );
            //}
            nout = i;
        }

        if(nout > 0){
            if(coarse_pos < stp){
                coarse_pos = stp;
            }
            int fine_pos = coarse_pos;
            for(int i = coarse_pos-stp ; i < coarse_pos+stp; i++){
                memcpy(d_cp0,in+i*d_vlen         ,sizeof(gr_complex)*d_cpl*d_vlen);
                memcpy(d_cp1,in+(i+d_fftl)*d_vlen,sizeof(gr_complex)*d_cpl*d_vlen);
                gr_complex val = corr(d_res,d_cp0,d_cp1,d_cpl*d_vlen);

                if(abs(it_val) < abs(val) ){
                    fine_pos = i;
                    if(d_corr_val < abs(val) ){
                        d_corr_val = abs(val);
                        long abs_pos = nitems_read(0) + fine_pos;
                        d_sym_pos = (nitems_read(0) + fine_pos)%d_slotl;
                        //printf("%s\tfine corr sym_pos = %ld\n",name().c_str(), d_sym_pos );
                        //printf("corr_val = %f\tsym_pos = %ld\tabs_pos = %ld\n", d_corr_val, d_sym_pos, abs_pos);
                    }
                }
            }
        }

        //The next if-statement exists for the sole purpose to avoid an infinity loop caused by the scheduler and not enough input items.
        if(nout == 0){
            printf("%s\tnout = %i\tnoutput_items = %i\tnitems_read = %ld\titems dumped!\n",name().c_str(), nout, noutput_items, nitems_read(0) );
            nout = noutput_items;
        }

        // actually the next block doesn't care about the exact tag position. Only the value and key are important.
        memcpy(out, in, sizeof(gr_complex)*nout*d_vlen );
        add_item_tag(0,nitems_read(0)+5,d_key, pmt::from_long(d_sym_pos),d_tag_id);
        d_work_call++;
        // Tell runtime system how many output items we produced.
        return nout;
    }
    
    gr_complex
    rough_symbol_sync_cc_impl::corr(gr_complex *res, gr_complex *x, gr_complex *y, int len)
    {
        volk_32fc_conjugate_32fc_a(y, y, len);
        volk_32fc_x2_multiply_32fc_a(res, x, y, len);

        float* i_vector = (float*)fftwf_malloc(sizeof(float)*len);
        float* q_vector = (float*)fftwf_malloc(sizeof(float)*len);
        volk_32fc_deinterleave_32f_x2_a(i_vector, q_vector, res, len);
        float i_result = 0.0f;
        float q_result = 0.0f;
        volk_32f_accumulator_s32f_a(&i_result, i_vector, len);
        volk_32f_accumulator_s32f_a(&q_result, q_vector, len);

        return gr_complex(i_result, q_result);
    }

  } /* namespace lte */
} /* namespace gr */


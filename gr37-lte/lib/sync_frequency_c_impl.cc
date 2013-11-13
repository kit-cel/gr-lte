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
#include "sync_frequency_c_impl.h"
#include <fftw3.h>
#include <volk/volk.h>
#include <cmath>

namespace gr {
  namespace lte {

    sync_frequency_c::sptr
    sync_frequency_c::make(boost::shared_ptr<gr::analog::sig_source_c> &sig, int fftl)
    {
      return gnuradio::get_initial_sptr
        (new sync_frequency_c_impl(sig, fftl));
    }

    /*
     * The private constructor
     */
    sync_frequency_c_impl::sync_frequency_c_impl(boost::shared_ptr<gr::analog::sig_source_c> &sig, int fftl)
      : gr::sync_block("sync_frequency_c",
              gr::io_signature::make( 1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
                d_sig(sig),
                d_fftl(fftl),
                d_cpl(144*fftl/2048),
                d_cpl0(160*fftl/2048),
                d_slotl(7*fftl+6*d_cpl+d_cpl0),
                d_samp_rate(d_slotl/0.0005),
                d_samp_num(0),
                d_work_call(0),
                d_f_av(0.0),
                d_offset(0)
    {
        d_buffer = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_slotl);
    }

    /*
     * Our virtual destructor.
     */
    sync_frequency_c_impl::~sync_frequency_c_impl()
    {
        fftwf_free(d_buffer);
    }

    int
    sync_frequency_c_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        int samp_num = d_samp_num;
        int fftl = d_fftl;
        int cpl = d_cpl;
        int cpl0 = d_cpl0;
        int slotl = d_slotl;

        std::vector <gr::tag_t> v;
        get_tags_in_range(v, 0, nitems_read(0), nitems_read(0)+noutput_items);
        for (int m = 0 ; m < v.size() ; m++){
            int offset = int((v[m].offset)%slotl);
            if( offset != d_offset){
                printf("%s\tASYNC!\tnew offset = %i\told offset = %i\tsamp_num = %i\n", name().c_str(), offset, d_offset, samp_num);
                d_offset = offset;
                samp_num = slotl-( (v[m].offset-nitems_read(0))%slotl ) ;
            }
        }

        int nout = noutput_items;
        for(int i = 0 ; i < noutput_items ; i++){
            if(samp_num+nout > slotl){
                memcpy(d_buffer+samp_num, in, sizeof(gr_complex)*(slotl-samp_num) );
                i += (slotl-samp_num-1);
                nout -= (slotl-samp_num);
                samp_num += (slotl-samp_num);
                in += (slotl-samp_num);
            }
            else{
                memcpy(d_buffer+samp_num, in, sizeof(gr_complex)*(nout) );
                i += (nout-1);
                samp_num += (nout);
                nout -= nout;
                in += nout;
            }

            //d_buffer[samp_num] = *(in+i);

            samp_num = samp_num%slotl;
            //printf("%s.work\t%i\tsamp_num = %i\ti = %i\tnout = %i\tnoutput_items = %i\n", name().c_str(), d_work_call, samp_num, i, nout, noutput_items );
            if(samp_num == 0){
                calc_f_off_av();
            }
        }

        d_work_call++;
        d_samp_num = samp_num;

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

    void
    sync_frequency_c_impl::calc_f_off_av(){
        int fftl = d_fftl;
        int cpl = d_cpl;
        int cpl0 = d_cpl0;
        gr_complex *res = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*cpl0);

        //printf("%s.calc_f_off_av\n", name().c_str() );

        // The next few lines are predestined for volk usage / performance improvements.
        gr_complex corr_val[7] = {0};
        corr_val[0] = corr (res, d_buffer, d_buffer+cpl0, cpl0);

        for(int i = 0 ; i < 6; i++){
            corr_val[i+1] = corr(res, d_buffer+fftl+cpl0+i*(fftl+cpl), d_buffer+fftl+cpl0+i*(fftl+cpl)+fftl, cpl );
        }

        //find maximum correlation
        float max = abs(corr_val[0]);
        float abs_corr;
        int pos = 0;
        //gr_complex av = 0;
        for (int i = 0 ; i < 7 ; i++ ){
            //av = av + corr[i];
            abs_corr = abs(corr_val[i]);
            if (max < abs_corr ){
                max = abs_corr;
                pos = i;
            }
        }

        // The next line does the fancy stuff -> calculate the frequency offset.
        float f_off = arg(corr_val[pos]) * float(float(d_samp_rate)/(2*M_PI*float(d_fftl) ) );
        d_f_av=d_f_av - (0.01 * f_off);

        //f_vec.push_back(d_f_av);

        (*d_sig).set_frequency((-1)*double(d_f_av) );
    }

    gr_complex
    sync_frequency_c_impl::corr(gr_complex *res, gr_complex *x, gr_complex *y, int len)
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


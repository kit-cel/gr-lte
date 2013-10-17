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
#include <lte/correlator.h>

#include <cstdio>
#include <cmath>

#include <volk/volk.h>
#include <cstring>

namespace gr {
  namespace lte {

    correlator::correlator(gr_complex* in1, gr_complex* in2, gr_complex *out, int len):
            d_in1(in1),
            d_in2(in2),
            d_out_p(out),
            d_len(len)
    {    
        //internal inputs and outputs
        d_in  = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*d_len);
        d_out = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*d_len);
     
        // FFTW Planner
        d_plan_f = fftwf_plan_dft_1d(d_len, reinterpret_cast<fftwf_complex*>(d_in), reinterpret_cast<fftwf_complex*>(d_out), FFTW_FORWARD, FFTW_ESTIMATE);
        d_plan_r = fftwf_plan_dft_1d(d_len, reinterpret_cast<fftwf_complex*>(d_in), reinterpret_cast<fftwf_complex*>(d_out), FFTW_BACKWARD, FFTW_ESTIMATE);
        
        //results after fourier transform
        d_f1 = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*d_len);
        d_f2 = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*d_len);
        d_res_f = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*d_len);
        
        //end result
        d_res_t = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*(2*d_len-1) );
        d_abs   = (float*) fftwf_malloc(sizeof(float)*(2*d_len-1) );
    }

    correlator::~correlator()
    {
        fftwf_destroy_plan(d_plan_f);
        fftwf_destroy_plan(d_plan_r);
        
        fftwf_free(d_in);
        fftwf_free(d_out);
        fftwf_free(d_f1);
        fftwf_free(d_f2);
        fftwf_free(d_res_f);
        fftwf_free(d_res_t);
        fftwf_free(d_abs);
    }

    void
    correlator::execute()
    {
        // Transform first sequence
        memcpy(d_in+1, d_in1, sizeof(gr_complex)*(d_len-1) );
        d_in[0] = d_in1[d_len-1];//shift
        
        // Transform and conjugate first input
        fftwf_execute(d_plan_f);
        memcpy(d_f1, d_out, sizeof(gr_complex)*d_len);
        volk_32fc_conjugate_32fc_a(d_f1, d_f1, d_len); //numPoints (number of complex values)
        
        // Transform second sequence
        memcpy(d_in, d_in2, sizeof(gr_complex)*d_len);
        fftwf_execute(d_plan_f);
        memcpy(d_f2, d_out, sizeof(gr_complex)*d_len);
        
        // Multiply in frequency domain
        volk_32fc_x2_multiply_32fc_a(d_res_f, d_f1, d_f2, d_len);
        
        // Transform back to time domain
        memcpy(d_in, d_res_f, sizeof(gr_complex)*d_len);
        fftwf_execute(d_plan_r);
        
        // For now duplicate partly (should be changed later)
        memcpy(d_res_t      , d_out, sizeof(gr_complex)*d_len    );
        memcpy(d_res_t+d_len, d_out, sizeof(gr_complex)*(d_len-1));
        
        // make result available
        memcpy(d_out_p, d_out, sizeof(gr_complex)*d_len );
    }    

        
    void
    correlator::get_maximum(int &pos, float &max )
    {
        int rlen = (2*d_len-1);
        volk_32fc_magnitude_32f_a(d_abs, d_res_t, rlen);
        int fpos = 0;
        float fmax = 0.0;
        for (int i = 0 ; i < rlen; i++){
            if(fmax < d_abs[i]){
                fmax = d_abs[i];
                fpos = i;
            }
        }

        max = fmax;
        pos = fpos;

    }

  } /* namespace lte */
} /* namespace gr */


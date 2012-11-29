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

#include <gr_complex.h> 
#include <cstdio>
#include <cmath>
#include <fftw3.h>
#include <volk/volk.h>
#include <cstring>
 
 
class lte_correlation
{
    
    
    private:
        //external inputs and outputs
        gr_complex *d_in1;
        gr_complex *d_in2;
        gr_complex *d_out_p;
        //internal input and output buffers
        gr_complex *d_in;
        gr_complex *d_out;
        
        int d_len;
    
        // FFTW Planner
        fftwf_plan d_plan_f;
        fftwf_plan d_plan_r;
    
        //results after fourier transform
        gr_complex* d_f1;
        gr_complex* d_f2;
        gr_complex* d_res_f;
        gr_complex* d_res_t;
        float* d_abs;
        
    public:
        lte_correlation(gr_complex *in1, gr_complex *in2, gr_complex *out, int len);  
        ~lte_correlation();
        
        void execute();
        void get_maximum(int &pos, float &max );

};





















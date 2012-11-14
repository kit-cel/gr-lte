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
#include <lte_freq_estimate_c.h>
#include <cstdio>

#define PI 3.14159265


lte_freq_estimate_c_sptr
lte_make_freq_estimate_c (boost::shared_ptr<gr_sig_source_c> &sig, int fftl)
{
	return lte_freq_estimate_c_sptr (new lte_freq_estimate_c (sig, fftl));
}


lte_freq_estimate_c::lte_freq_estimate_c (boost::shared_ptr<gr_sig_source_c> &sig, int fftl)
	: gr_sync_block ("freq_estimate_c",
		gr_make_io_signature (1,1, sizeof (gr_complex)),
		gr_make_io_signature (0, 0, 0)),
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

    d_buffer = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*d_slotl);//new gr_complex[d_slotl];
    //memset(d_buffer,0,sizeof(gr_complex)*d_slotl);
    
    /*
    for (int i = 0 ; i < d_slotl ; i++){
        d_buffer[i] = 0;
    }
*/
    printf("\n%s constructor\t",name().c_str() );
    printf("fftl = %i\tcpl = %i\tcpl0 = %i\tslotl = %i\tsamp_rate = %i\n",d_fftl,d_cpl,d_cpl0,d_slotl,d_samp_rate);
    
    //double s_freq   = (*d_sig).sampling_freq();
    //gr_waveform_t wf = (*d_sig).waveform();
    //double freq     = (*d_sig).frequency();
    //double ampl     = (*d_sig).amplitude();
    //gr_complex os    = (*d_sig).offset();
    
    //f_vec.resize(2000);
    //printf("vector parameters\tsize = %li\tmax_size = %li\tcapacity = %li\n",f_vec.size(),f_vec.max_size(),f_vec.capacity() );
    //printf("s_freq %f\twaveform = %i\tfreq = %f\tampl = %f\toffset = %+f %+fj\n",s_freq,wf,freq,ampl,os.real(),os.imag() );
}


lte_freq_estimate_c::~lte_freq_estimate_c ()
{
    //delete[] d_buffer;
    fftwf_free(d_buffer);
}


int
lte_freq_estimate_c::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	int samp_num = d_samp_num;
	int fftl = d_fftl;
	int cpl = d_cpl;
	int cpl0 = d_cpl0;
	int slotl = d_slotl;
	
	std::vector <gr_tag_t> v;
    get_tags_in_range(v,0,nitems_read(0),nitems_read(0)+noutput_items);
    for (int m = 0 ; m < v.size() ; m++){
        int offset = int((v[m].offset)%slotl);
        if( offset != d_offset){
            
            d_offset = offset;
            
            samp_num = slotl-( (v[m].offset-nitems_read(0))%slotl ) ;
            
            printf("%s\tASYNC!\toffset = %ld\t%ld\tsamp_num = %i\n", name().c_str(), v[m].offset, nitems_read(0), samp_num);
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
lte_freq_estimate_c::calc_f_off_av(){
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
    float f_off = arg(corr_val[pos]) * float(float(d_samp_rate)/(2*PI*float(d_fftl) ) );
    d_f_av=d_f_av - (0.01 * f_off);

    //f_vec.push_back(d_f_av);

    (*d_sig).set_frequency((-1)*double(d_f_av) );
}
 

gr_complex
lte_freq_estimate_c::corr(gr_complex *res, gr_complex *x, gr_complex *y, int len)
{
    gr_complex val = 0;
    volk_32fc_conjugate_32fc_a(y, y, len);
    volk_32fc_x2_multiply_32fc_a(res, x, y, len);
    //printf("%s\tlen = %i\n", name().c_str(), len);
/*
    for(int i = 0; i < len; i++){
        val += *(res+i);
    }
*/
    int al_mult = volk_get_alignment() / sizeof(gr_complex) ;
    if(len%volk_get_alignment() != 0){
        int rest = len%volk_get_alignment();
        len -= len%volk_get_alignment();
        
        for(int i = len/2; i > al_mult ; i=i/2 ){
            volk_32f_x2_add_32f_a((float*)res, (float*)res, (float*)(res+i), i*2);
        }
        for(int i = 0; i < al_mult; i++){
            val += *(res+i);
        }
        for(int i = 0; i < rest; i++){
            val += *(res+len+i);
        }
    }
    else{
        for(int i = len/2; i > 3 ; i=i/2 ){
            volk_32f_x2_add_32f_a((float*)res, (float*)res, (float*)(res+i), i*2);
        }
        for(int i = 0; i < 4; i++){
            val += *(res+i);
        }
    }


    return val;
}
 
 

 

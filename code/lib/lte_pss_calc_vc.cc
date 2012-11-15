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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <lte_pss_calc_vc.h>
#include <cstdio>
#include <cmath>
#include <fftw3.h>
#include <volk/volk.h>
//#define NDEBUG


lte_pss_calc_vc_sptr
lte_make_pss_calc_vc (boost::shared_ptr<lte_pss_tagging_cc> &tag, boost::shared_ptr<lte_pss_selector_cvc> &sel, int fftl)
{
	return lte_pss_calc_vc_sptr (new lte_pss_calc_vc (tag, sel, fftl));
}


lte_pss_calc_vc::lte_pss_calc_vc (boost::shared_ptr<lte_pss_tagging_cc> &tag, boost::shared_ptr<lte_pss_selector_cvc> &sel, int fftl)
	: gr_sync_block ("pss_calc_vc",
		gr_make_io_signature (1,1, sizeof (gr_complex)*72),
		gr_make_io_signature (0, 0, 0)),
		d_tag(tag),
		d_sel(sel),
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
    zc(d_chu0,0);
    zc(d_chu1,1);
    zc(d_chu2,2);
    
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    
    printf("%s\tal_mult = %i\tget_al = %i\tsizeof = %ld\n", name().c_str(), alignment_multiple, volk_get_alignment(), sizeof(gr_complex) );
    //set_alignment(std::max(1,alignment_multiple));
//    d_uchu0.resize(62);
//    d_uchu1.resize(62);
//    d_uchu2.resize(62);
    
//    for (int i = 0 ; i < 62 ; i++){
//        d_uchu0[i] = d_chu0[i];
//        d_uchu1[i] = d_chu1[i];
//        d_uchu2[i] = d_chu2[i];
//    }
    
}




lte_pss_calc_vc::~lte_pss_calc_vc ()
{
}

const gr_complex
lte_pss_calc_vc::d_C_I = gr_complex(0,1);

const float
lte_pss_calc_vc::d_PI = float(3.14159265358979323846);

void
lte_pss_calc_vc::zc(gr_complex *zc, int cell_id)
{
    int Nzc=62;
    float u=0;
    if (cell_id==0){u=25.0;}
    if (cell_id==1){u=29.0;}
    if (cell_id==2){u=34.0;}

    //printf("complex = %+8.6f %+8.6fj\tPI = %f\n",d_C_I.real(),d_C_I.imag(),d_PI);
    for(int n = 0; n < 31; n++){
        zc[n]=exp(d_C_I* gr_complex(d_PI*u* float(-1*n    *(n+1))/63.0 ) );
        //printf("%+8.6f %+8.6f\texp(j*%f)\n",zc[n].real(),zc[n].imag(),d_PI*u* float(-1*n    *(n+1))/63.0);
    }
    for(int n = 31; n < 62 ; n++){
        zc[n]=exp(d_C_I* gr_complex(d_PI*u* float(-1*(n+1)*(n+2))/63.0 ) );
    }
    /*for(int n = 0 ; n < 62 ; n++){
        zc[n] = zc[n]/gr_complex(sqrt(62.0));
    }*/
}

/*
// be careful! input arrays must have the same size!
void
lte_pss_calc_vc::xcorr(std::vector<gr_complex> &v, gr_complex *x,gr_complex *y, int len)
{
    int N = len;
    int sizeof_gr_complex = sizeof(gr_complex);
    gr_complex *ax  = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len);
    gr_complex *ay  = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len);
    gr_complex *axc = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len);
    gr_complex *ayc = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*len);
    gr_complex *val = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*1);
    memcpy(axc,x,sizeof(gr_complex)*len);
    memcpy(ayc,y,sizeof(gr_complex)*len);
    //volk_32fc_conjugate_32fc_a(ay,ay,len);
/*    
    for (int i = 0 ; i < 2 * N - 1 ; i++){
        if(i < N){
            memcpy(ax, x+(N-1-i), sizeof_gr_complex*(i+1) );
            volk_32fc_x2_dot_prod_32fc_a(val, ax, ayc, i+1 );

            //volk_32fc_x2_dot_prod_32fc_a(val, ax+(N-1-i), ay, i+1 );
            v.push_back(*val);
        }
        else{
            memcpy(ay, y+(i-N), 2*N-1-i);
            volk_32fc_x2_dot_prod_32fc_a(val, axc, ay, 2*N-1-i );
            //v.push_back( corr(ax,ay+(i-N),2*N-1-i) );
            //volk_32fc_x2_dot_prod_32fc_a(val, ax, ay+(i-N), 2*N-1-i );
            v.push_back(*val);
        }
    }

    gr_complex gen_val = 0;
    for (int i = 0 ; i < 2 * N - 1 ; i++){
        if(i < N){
            gen_val = corr(x+(N-1-i), y, i+1);
            v.push_back( gen_val );
            //d_corr_vec.push_back(abs(gen_val));
        }
        else{
            gen_val = corr(x, y+(i-N), 2*N-1-i);
            v.push_back( gen_val );
            //d_corr_vec.push_back(abs(gen_val));
        }
    }
  
    fftwf_free(ax);
    fftwf_free(ay);
    fftwf_free(axc);
    fftwf_free(ayc);
    fftwf_free(val);
}
*/

// classic corr
gr_complex
lte_pss_calc_vc::corr(gr_complex *x, gr_complex *y, int len)
{
    gr_complex val = 0;

    for(int i = 0; i < len; i++){
        val += ( x[i]*conj(y[i]) );
    }
    
    return val;
}

// simple correlation between 2 arrays. returns complex value.
// input arrays must be aligned for volk!
// input arrays must be a multiple volk_get_alignment value.
gr_complex
lte_pss_calc_vc::corr(gr_complex *res, gr_complex *x, gr_complex *y, int len)
{
    gr_complex val = 0;

    volk_32fc_x2_multiply_32fc_a(res,x,y,len);
    for(int i = len/2; i > 3 ; i=i/2 ){
        volk_32f_x2_add_32f_a((float*)res, (float*)res, (float*)(res+i), i*2);
    }
    for(int i = 0; i < 4; i++){
        val += *(res+i);
    }
    return val;
}


void
lte_pss_calc_vc::cxcorr(std::vector<gr_complex> &v, gr_complex *x,gr_complex *y, int len)
{
    int N = len;
    int alen = len + (volk_get_alignment() - (len)%volk_get_alignment() );
    
    gr_complex shift = 0;
    gr_complex val = 0;
    gr_complex c_val = 0;
    gr_complex *ax   = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*alen);
    gr_complex *ay   = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*alen);
    gr_complex *res  = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*alen);
    memcpy(ax,x,sizeof(gr_complex)*len);
    memcpy(ay,y,sizeof(gr_complex)*len);
    
    volk_32fc_conjugate_32fc_a(ay,ay,len);
    
    for (int i = 0 ; i < 2*N-1 ; i++){
        //Shift
        shift = ay[0];
        memmove(ay, &ay[1] ,sizeof(gr_complex)*(len-1));
        ay[len-1]=shift;

        val = corr(res,ax,ay,alen);

        v.push_back( val );
        //d_corr_vec.push_back(abs(val));
    }
    /*
    for(int i = 0; i < 500 ; i++){
        d_corr_vec.push_back(0);
    }
    */
    
    fftwf_free(ax);
    fftwf_free(ay);
}

void
lte_pss_calc_vc::max_pos(float &max, int &pos, gr_complex *x,gr_complex *y, int len)
{
    std::vector<gr_complex> v;
    cxcorr(v, x, y, len);
    //float *res_mag  = (gr_complex*)fftwf_malloc(sizeof(float)*(2*len-1));
    //volk_32fc_magnitude_32f_a(res_mag, res, len);
    //unsigned int *max_p;
    //volk_32fc_index_max_16u_a(max_p, res, 2*len-1);
    //gr_complex* max_cp = (gr_complex*) max_p;
    //max = abs( *max_cp );
    //pos = (max_cp-res)/sizeof(gr_complex);
    max = 0.0;
    pos = -1;
    for(int i = 0; i < v.size() ; i++){
        if(max < abs(v[i]) ){
            max = abs(v[i]);
            pos = i;
        }
    }
    
    
}


bool
lte_pss_calc_vc::find_pss_symbol(gr_complex *chuX)
{
    int len = 62;
    
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
    
    bool has_changed = false;
    if(d_corr_val < maxc){
        has_changed = true;
        //if(d_N_id_2 != N_id_2){
        //    has_changed = true;
        //}
        d_N_id_2 = N_id_2;
        d_corr_val = maxc;
        //printf("max0 = %+10.6f\tmax1 = %+10.6f max2 = %+10.6f\t\tN_id_2 = %i\td_N_id_2 = %i\td_corr_val = %f\n",max0, max1, max2, N_id_2, d_N_id_2, d_corr_val);
        
    }
    
    
    
    return has_changed;
}

int
lte_pss_calc_vc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];

    if(d_is_locked){return noutput_items;}
    
    
    bool changed = false;
    for(int i = 0 ; i < noutput_items ; i++){
        gr_complex chuX[62] = {0};
        memcpy(chuX,in+5,sizeof(gr_complex)*62);
        in += 72;
    
        changed = find_pss_symbol(chuX);
        if(changed){
            d_lock_count = 0;
            
            std::vector <gr_tag_t> v_off;
            get_tags_in_range(v_off,0,nitems_read(0)+i,nitems_read(0)+i+1);
            long offset = pmt::pmt_to_long(v_off[0].value);
            std::string srcid = pmt::pmt_symbol_to_string(v_off[0].srcid);
            long abs_pos = offset;
            //printf("\noffset before %ld\n", offset);
            offset = offset - (6*d_fftl+5*d_cpl+d_cpl0);
            d_half_frame_start = offset%(10*d_slotl);
            //d_half_frame_start = d_half_frame_start - (6*d_fftl+5*d_cpl+d_cpl0);
            
            if(d_half_frame_start > 0 ){
                printf("\n%s NEW half_frame_start = %ld\tabs_pos = %ld\tN_id_2 = %i\tcorr_val = %f\n\n",name().c_str(), d_half_frame_start, abs_pos, d_N_id_2, d_corr_val );
                (*d_tag).set_half_frame_start(d_half_frame_start);
                (*d_sel).set_half_frame_start(d_half_frame_start);
                (*d_tag).set_N_id_2(d_N_id_2);
            }
            //else{
            //    printf("bad value? start < 0\n");
            //}
            
        }
        else{d_lock_count++;}
    }
    // After a certain amount of unchanged N_id_2 calculations. calculation is stopped and block has no further function.
    if(d_lock_count > 300 && d_N_id_2 >=0 ){
        std::vector <gr_tag_t> v_off;
        get_tags_in_range(v_off,0,nitems_read(0),nitems_read(0)+noutput_items);
        long abs_pos = pmt::pmt_to_long(v_off[0].value);
        printf("\n%s is locked! half_frame_start = %ld\tabs_pos = %ld\tN_id_2 = %i\tcorr_val = %f\n\n",name().c_str(), d_half_frame_start, abs_pos, d_N_id_2, d_corr_val );
        if(abs_pos > (30*d_slotl) ){
            d_is_locked = true;
            (*d_tag).lock();
            (*d_sel).lock();
        }
        
    }
    

	// Tell runtime system how many output items we produced.
	return noutput_items;
}


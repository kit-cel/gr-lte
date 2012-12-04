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
#include <lte_sss_calc_vc.h>
#include <cstdio>


lte_sss_calc_vc_sptr
lte_make_sss_calc_vc (tag_sptr &tag, daemon_sptr &daemon, int fftl)
{
	return lte_sss_calc_vc_sptr (new lte_sss_calc_vc (tag, daemon, fftl));
}


lte_sss_calc_vc::lte_sss_calc_vc (tag_sptr &tag, daemon_sptr &daemon, int fftl)
	: gr_sync_block ("sss_calc_vc",
		gr_make_io_signature (1,1, sizeof (gr_complex)*72),
		gr_make_io_signature (0,0,0)),
		d_fftl(fftl),
		d_slotl(7*fftl+6*(144*fftl/2048)+(160*fftl/2048) ),
		d_cell_id(-1),
		d_max_val_new(0.0),
		d_max_val_old(0.0),
		d_N_id_2(-1),
		d_sss_pos(0),
		d_tag(tag),
		d_daemon(daemon),
		d_frame_start(0),
		d_is_locked(false),
		d_unchanged_id(0)
{    
    //initialize d_cX
    char cX_x[31] = {0};
    cX_x[4] = 1;
    for(int i = 0; i < 26 ; i++ ){
        cX_x[i+5] = (cX_x[i+3] + cX_x[i])%2;
    }
    //Do NRZ coding (necessary here?)
    for(int i = 0 ; i < 31 ; i++){
        d_cX[i] = 1-2*cX_x[i];
    }
    
    //initialize d_sref
    int m0_ref=0;
    char sX_x[31]={0};
    sX_x[4]=1;
    for(int i = 0; i < 26 ; i++){
        sX_x[i+5] = (sX_x[i+2] + sX_x[i])%2;
    }
    gr_complex sX[31]={0};
    for(int i = 0 ; i < 31 ; i++){
        sX[i] = 1-2*sX_x[i];
    }
    int m0 = 0;
    for(int i = 0 ; i < 31 ; i++){
        d_sref[i   ]=sX[ (i+m0)%31 ];
        d_sref[i+31]=sX[ (i+m0)%31 ];
    }
    
    //initialize d_zX
    char zX_x[31] = {0};
    zX_x[4] = 1;
    for (int i = 0 ; i < 26 ; i++){
        zX_x[i+5] = (zX_x[i+4] + zX_x[i+2] + zX_x[i+1] + zX_x[i+0])%2;
    }
    for (int i = 0; i < 31 ; i++){
        d_zX[i] = 1 - (2*zX_x[i]);
    }
    
    //initialize m0/m1 pair lookup table
    for(int i = 0 ; i < 168 ; i++){
        int N = i;
        int q_prime = floor(N/30);
        int q = floor( ( N + (q_prime*(q_prime+1)/2) )/30 );
        int m_prime = N + q * (q+1)/2;
        int m0g = m_prime%31;
        int m1g = (m0g + int(floor(m_prime/31))+1 )%31;
        d_v_m0[i] = m0g;
        d_v_m1[i] = m1g;
    }
    

    
}


lte_sss_calc_vc::~lte_sss_calc_vc ()
{
}


int
lte_sss_calc_vc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	// This is a sink block! It does not produce any thus it doesn't have an output port!
	const gr_complex *in = (const gr_complex *) input_items[0];
	
    if(d_is_locked){
        return noutput_items;
    }

    pmt::pmt_t id_key=pmt::pmt_string_to_symbol("N_id_2");
    std::vector <gr_tag_t> v_id;
    get_tags_in_range(v_id,0,nitems_read(0),nitems_read(0)+1,id_key);
    if (v_id.size() > 0){
        d_N_id_2 = int(pmt::pmt_to_long(v_id[0].value));
        //printf("%s\tN_id_2 = %i\toff = %ld\n",name().c_str(), d_N_id_2, v_id[0].offset);
    }
    int N_id_2 = d_N_id_2;
    if(d_N_id_2 < 0){return 1;}
    
    
    long offset = 0;
    pmt::pmt_t offset_key=pmt::pmt_string_to_symbol("offset_marker");
    std::vector <gr_tag_t> v_off;
    get_tags_in_range(v_off,0,nitems_read(0),nitems_read(0)+1,offset_key);
    if (v_off.size() > 0){
        offset = pmt::pmt_to_long(v_off[0].value);
	}
    
    
    //printf("%s.work()\t",name().c_str() );
	//printf("%f\t%f\n",d_max_val_new, d_max_val_old);
	
	gr_complex sss[62] = {0};
	memcpy(sss,in+5,sizeof(gr_complex)*62);
	
	// extract the 2 half sss symbols which are interleaved differently bby their position within a frame.
	gr_complex d_even[31]={0};
    gr_complex d_odd [31]={0};
	for(int i = 0; i < 31 ; i++){
	    d_even[i]=sss[2*i+0];
	    d_odd[i] =sss[2*i+1];
	}
	
	// next 2 sequences depend on N_id_2
	gr_complex c0[31] = {0};
    gr_complex c1[31] = {0};
    for(int i = 0; i < 31 ; i++){
        c0[i] = d_cX[ (i+N_id_2  )%31 ];
        c1[i] = d_cX[ (i+N_id_2+3)%31 ];
    }
    
    gr_complex s0m0[31]={0};
    for (int i = 0 ; i < 31 ; i++){
        s0m0[i]=d_even[i]/gr_complex(c0[i]);
    }
    int m0 = calc_m(s0m0);
    
    //printf("m0 = %i\n",m0);
    
    char z1m0[31] = {0};
    for (int i = 0 ; i < 31 ; i++) {
        z1m0[i] = d_zX[ ( i+(m0%8) )%31 ];
    }
    gr_complex s1m1[31] = {0};
    for (int i = 0 ; i < 31 ; i++){
        s1m1[i] = d_odd[i] / (c1[i] * gr_complex(z1m0[i]) );
    }
    
    int m1 = calc_m(s1m1);
    //printf("m1 = %i\n",m1);
    
    int sss_pos = 0;
    if (m0 > m1){
        int mx = m0;
        m0 = m1;
        m1 = mx;
        sss_pos = 5;
    }
    //printf("sss_pos = %i\n",sss_pos);
    
    int N_id_1 = -1;
    for(int i = 0 ; i < 168 ; i++ ){
        if(d_v_m0[i] == m0 && d_v_m1[i] == m1){
            N_id_1 = i;
            break;
        }
    }
    
    //printf("N_id_1 = %i\n",N_id_1);
    if(N_id_1 >=0 && N_id_2 >=0 && d_max_val_new > d_max_val_old*0.8){
        long frame_start = offset%(20*d_slotl);
        if(sss_pos == 5){frame_start += (10*d_slotl);}
        int cell_id = 3*N_id_1 + N_id_2;
        
        d_cell_id = cell_id;
        //printf("NEW DATA!!! cell_id = %i\tN_id_1 = %i\tN_id_1 = %i\tm0 = %i m1 = %i\tframe_start = %ld\toffset = %ld\t", d_cell_id, N_id_1, N_id_2, m0, m1, frame_start, offset);
        //printf("%f\t%f\n",d_max_val_new, d_max_val_old);
        d_sss_pos = sss_pos;
        if(d_frame_start != frame_start){
            d_frame_start = frame_start;
            
        }
        if(cell_id == d_cell_id){
            d_unchanged_id++;
            if(d_unchanged_id > 2){
                d_frame_start = d_frame_start%(20*d_slotl);
                printf("\n%s is locked! frame_start = %ld\tabs_pos = %ld\tcell_id = %i\n\n", name().c_str(), d_frame_start, offset, d_cell_id );
                (*d_tag).set_frame_start( d_frame_start );
                (*d_daemon).set_cell_id(d_cell_id);
                d_is_locked = true;
            }
        }
        else{
            d_unchanged_id = 0;
        }
        
        
    }
    else{
        //printf("FAILED!!!!! cell_id = %i\tN_id_1 = %i\tN_id_2 = %i\tsss_pos = %i\toffset = %ld\told_val = %f\tnew_val = %f\n",d_cell_id,N_id_1,N_id_2,sss_pos,offset, d_max_val_old, d_max_val_new);
        //printf("%f\t%f\n",d_max_val_new, d_max_val_old);
        if(d_sss_pos == 0){d_sss_pos = 5;}
        else{d_sss_pos = 0;}
    }
    
    
    //*out = d_sss_pos;
    d_max_val_old = d_max_val_new;
	// Tell runtime system how many output items we produced.
	return 1;
}


int
lte_sss_calc_vc::calc_m(gr_complex *s0m0)
{
    int mX = -1;
    int N = 62;

    // length d_sref = 62
    // length s0m0 = 31
    std::vector<gr_complex> x_vec;
    
    
    gr_complex s0[62]={0};
    memcpy(s0,s0m0,sizeof(gr_complex)*31);
    gr_complex sr[62]={0};
    memcpy(sr,d_sref,sizeof(gr_complex)*62);
    
    xcorr(x_vec,s0,sr,N);
    
    corr_vec.clear();
    float max = 0;
    int pos = -1;
    for (int i = 0 ; i < 2*N-1 ; i++){
        float mag = abs(x_vec[i]);
        corr_vec.push_back(mag);
        //printf("%i\t%+f\n",i,mag);
        if (max < mag){
            max = mag;
            pos = i;
        }
    }
    
/*    if(!std::isfinite(max)){
        //printf("max of xcorr is infinite!\n");
        max = 100.0;
    }*/
    
    mX = abs(pos-31-31);
      
    d_max_val_new = (d_max_val_new + max)/2;
    
    //printf("m = %i\tval = %f\td_max_val = %f\n",mX, max,d_max_val_new);
    
    
    return mX;
}


// simple correlation between 2 arrays. returns complex value.
gr_complex
lte_sss_calc_vc::corr(gr_complex *x,gr_complex *y, int len)
{
    gr_complex val = 0;
    for(int i = 0 ; i < len ; i++){
        val = val + (x[i]*conj(y[i]) );
    }    
    return val;
}


// be careful! input arrays must have the same size!
void
lte_sss_calc_vc::xcorr(std::vector<gr_complex> &v, gr_complex *x,gr_complex *y, int len)
{
    int N = len;
    
    for (int i = 0 ; i < 2 * N - 1 ; i++){
        if(i < N){
            v.push_back( corr(x+(N-1-i),y,i+1) );
        }
        else{
            v.push_back( corr(x,y+(i-N),2*N-1-i) );
        }
    }
}










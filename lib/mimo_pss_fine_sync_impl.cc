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
#include "mimo_pss_fine_sync_impl.h"

#include <cstdio>
#include <cmath>
#include <fftw3.h>
#include <volk/volk.h>

namespace gr
{
namespace lte
{

mimo_pss_fine_sync::sptr
mimo_pss_fine_sync::make(int fftl)
{
    return gnuradio::get_initial_sptr
           (new mimo_pss_fine_sync_impl(fftl));
}

/*
 * The private constructor
 */
mimo_pss_fine_sync_impl::mimo_pss_fine_sync_impl(int fftl)
    : gr::sync_block("mimo_pss_fine_sync",
                     gr::io_signature::make(2, 2, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),
    d_N_id_2(-1),
    d_coarse_pos(-1),
    d_fine_pos(0),
    d_fine_corr_count(0),
    d_fftl(fftl),
    d_cpl(144*fftl/2048),
    d_cpl0(160*fftl/2048),
    d_slotl(7*fftl+6*d_cpl+d_cpl0),
    d_halffl(10*d_slotl),
    d_half_frame_start(0),
    d_corr_val(0),
    d_is_locked(false),
    d_decim(fftl/64),
    d_step(0)
{



    d_slot_key=pmt::string_to_symbol("slot");
    d_id_key = pmt::string_to_symbol("N_id_2");
    d_tag_id=pmt::string_to_symbol(this->name());

    size_t alig = volk_get_alignment();
    d_pssX_t = (gr_complex*) volk_malloc(sizeof(gr_complex)*d_fftl, alig);

    d_a = (gr_complex*)volk_malloc(sizeof(gr_complex)*4,alig);

    message_port_register_in(pmt::mp("N_id_2"));
    set_msg_handler(pmt::mp("N_id_2"), boost::bind(&mimo_pss_fine_sync_impl::handle_msg_N_id_2, this, _1));
    message_port_register_in(pmt::mp("coarse_pos"));
    set_msg_handler(pmt::mp("coarse_pos"), boost::bind(&mimo_pss_fine_sync_impl::handle_msg_coarse_pos, this, _1));


    d_port_half_frame = pmt::string_to_symbol("half_frame");
    message_port_register_out(d_port_half_frame);
    d_port_lock= pmt::string_to_symbol("lock");
    message_port_register_out(d_port_lock);



    //set_min_noutput_items(d_fftl);
    printf("fine");
}



/*
 * Our virtual destructor.
 */
mimo_pss_fine_sync_impl::~mimo_pss_fine_sync_impl()
{
    volk_free(d_pssX_t);
    volk_free(d_a);

}

const gr_complex
mimo_pss_fine_sync_impl::d_C_I = gr_complex(0,1);

// Define PI for use in this block
const float
mimo_pss_fine_sync_impl::d_PI = float(M_PI);


void
mimo_pss_fine_sync_impl::handle_msg_N_id_2(pmt::pmt_t msg)
{
    d_N_id_2 =  (int) pmt::to_long(msg);
    //init pss sequence in time domain
    gen_pss_t(d_pssX_t, d_N_id_2, d_fftl);
}

void
mimo_pss_fine_sync_impl::handle_msg_coarse_pos(pmt::pmt_t msg)
{
    d_coarse_pos=(int)pmt::to_long(msg);
    //fine correlation is done around this position, multiply for correct samplerate
    d_coarse_pos = d_coarse_pos*d_decim;
    d_is_locked=false;
    d_fine_corr_count=0;
}


void
mimo_pss_fine_sync_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
    unsigned int ninputs = ninput_items_required.size ();
    for (unsigned int i = 0; i < ninputs; i++)
        ninput_items_required[i] = noutput_items + d_fftl;
}



int mimo_pss_fine_sync_impl::work(int noutput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
{
    const gr_complex *in1 = (const gr_complex *) input_items[0];
    const gr_complex *in2 = (const gr_complex *) input_items[1];

    //no coarse sync yet
    if(d_coarse_pos == -1)
    {
        return noutput_items;
    }
    if(d_step-noutput_items>0){
        d_step-=noutput_items;
        return noutput_items;
    }


    long nir = nitems_read(0);
    int mod_pos=nir%d_halffl;
    int diff=0;
    int diff2=0;
    int consumed_items=0;
    float val=0;

    float val_early;
    float val_prompt;
    float val_late;
    int fine_pos;

    for(int i=0; i<noutput_items-d_fftl; i++)
    {
        mod_pos=(nir+i)%d_halffl;
        consumed_items++;
        if(!d_is_locked)
        {
            //do first fine sync
            int search_int=d_decim*2; //search intervall for correlation maximum

            //TODO: consider group-delay of the filter.
            //position of pss is saved as modulo value
            //-->search intervall can be within 2 following half frame lengths (rare case)
            diff=abs(d_coarse_pos-mod_pos);
            diff2=d_halffl-diff;
            diff=diff<diff2 ? diff : diff2;

            if(diff<search_int)
            {
                d_fine_corr_count++;
                val=diff_corr2(in1+i, in2+i, d_pssX_t, d_fftl);
                if(val>d_corr_val)
                {
                    d_fine_pos=mod_pos;
                    d_half_frame_start=calc_half_frame_start(mod_pos);
                    d_corr_val=val;
                    //printf("new fine timing: corr_val:%f\t half_frame_start: %li \t nitems_read: %li\n", d_corr_val, d_half_frame_start, nir);
                }
            }
            if(d_fine_corr_count==2*search_int-1)    //reached end of search intervall;
            {
                d_is_locked=true;
                message_port_pub(d_port_lock, pmt::PMT_T);
                message_port_pub(d_port_half_frame, pmt::from_long((long)d_half_frame_start));
                printf("fine timing is locked, now tracking\n");
                break;
            }
        }

        else
        {

            //do tracking after block is locked, 3 correlations around center
            //set_tags()
            if((d_fine_pos-1+d_halffl)%d_halffl==mod_pos)
            {
                d_corr_val=d_corr_val*0.99;
                val_early=diff_corr2(in1+i, in2+i, d_pssX_t, d_fftl);
            }
            else if (d_fine_pos==mod_pos)
            {
                val_prompt=diff_corr2(in1+i, in2+i, d_pssX_t, d_fftl);
            }
            else if ((d_fine_pos+1)%d_halffl==mod_pos)
            {
                val_late=diff_corr2(in1+i, in2+i, d_pssX_t, d_fftl);
                val=d_corr_val;
                fine_pos=d_fine_pos;

                if(val_early>val)
                {
                    val=val_early;
                    fine_pos=(d_fine_pos-1+d_halffl)%d_halffl;
                }
                if(val_prompt>val)
                {
                    val=val_prompt;
                    fine_pos=d_fine_pos;
                }
                if(val_late>val)
                {
                    val=val_late;
                    fine_pos=(d_fine_pos+1)%d_halffl;
                }


                printf("PSS-tracking: old_pos:%i\told_val:%f\tnew_pos:%i\tnew_val:%f\n", d_fine_pos, d_corr_val, fine_pos, val);
                d_fine_pos=fine_pos;
                d_corr_val=val;
                d_half_frame_start=calc_half_frame_start(fine_pos);
                message_port_pub(d_port_half_frame, pmt::from_long((long)d_half_frame_start));
                //step over next samples until next pss occurs
                d_step=d_halffl-noutput_items;

            }

        }


    }

    // Tell runtime system how many output items we produced.
    return noutput_items-d_fftl;
}


inline int mimo_pss_fine_sync_impl::calc_half_frame_start(int pss_pos){
    return (pss_pos-(6*d_fftl+6*d_cpl+d_cpl0)+d_halffl)%d_halffl;
}




//for better readability, differential correlation for 2 streams
float mimo_pss_fine_sync_impl::diff_corr2(const gr_complex* x1, const gr_complex* x2, const gr_complex* y, int len)
{
    return diff_corr(x1, y, len) + diff_corr(x2, y, len);
}


//calculate differential correlation, 4parts, returns absolute value
float mimo_pss_fine_sync_impl::diff_corr(const gr_complex* x,const gr_complex* y, int len)
{
    //loop?
    volk_32fc_x2_dot_prod_32fc(d_a,   x,         y,         len/4);
    volk_32fc_x2_dot_prod_32fc(d_a+1, x+len/4*1, y+len/4*1, len/4);
    volk_32fc_x2_dot_prod_32fc(d_a+2, x+len/4*2, y+len/4*2, len/4);
    volk_32fc_x2_dot_prod_32fc(d_a+3, x+len/4*3, y+len/4*3, len/4);

    return abs(d_a[0]*conj(d_a[1]) + d_a[1]*conj(d_a[2]) + d_a[2]*conj(d_a[3]));
}



//generate pss in time domain with fftw
void
mimo_pss_fine_sync_impl::gen_pss_t(gr_complex *zc_t, int cell_id, int len)
{
    gr_complex zc_f[62];
    zc(zc_f, cell_id);

    gr_complex* d_in  = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*len);
    gr_complex* d_out = (gr_complex*) fftwf_malloc(sizeof(gr_complex)*len);

    memset(d_in, 0, sizeof(gr_complex)*len);
    memcpy(d_in+len-31, zc_f, sizeof(gr_complex)*31);
    memcpy(d_in+1, zc_f+31, sizeof(gr_complex)*31);

    fftwf_plan p = fftwf_plan_dft_1d(len, reinterpret_cast<fftwf_complex*>(d_in), reinterpret_cast<fftwf_complex*>(d_out), FFTW_BACKWARD, FFTW_ESTIMATE);

    fftwf_execute(p);

    memcpy(zc_t, d_out, sizeof(gr_complex)*len);

    fftwf_destroy_plan(p);
    fftwf_free(d_in);
    fftwf_free(d_out);
}


void
mimo_pss_fine_sync_impl::zc(gr_complex *zc, int cell_id)
{
    // calculate value of variable u according to cell id number
    float u=0;
    switch (cell_id)
    {
    case 0:
        u=25.0;
        break;
    case 1:
        u=29.0;
        break;
    case 2:
        u=34.0;
        break;
    }

    //generate zadoff-chu sequences
    for(int n = 0; n < 31; n++)
    {
        zc[n]=exp(d_C_I* gr_complex(d_PI*u* float(-1*n*(n+1))/63.0 ) );
    }
    for(int n = 31; n < 62; n++)
    {
        zc[n]=exp(d_C_I* gr_complex(d_PI*u* float(-1*(n+1)*(n+2))/63.0 ) );
    }
}




} /* namespace lte */
} /* namespace gr */


/* -*- c++ -*- */
/*
 * Copyright 2014 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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
#include "mimo_pss_freq_sync_impl.h"
#include <lte/pss.h>
#include <volk/volk.h>
//#include <cstdio>

#include <cmath>

namespace gr
{
namespace lte
{

mimo_pss_freq_sync::sptr
mimo_pss_freq_sync::make(int fftl, int rxant, boost::shared_ptr<gr::analog::sig_source_c> &sig)
{
    return gnuradio::get_initial_sptr
           (new mimo_pss_freq_sync_impl(fftl, rxant, sig));
}

/*
 * The private constructor
 */
mimo_pss_freq_sync_impl::mimo_pss_freq_sync_impl(int fftl, int rxant, boost::shared_ptr<gr::analog::sig_source_c> &sig)
    : gr::sync_block("mimo_pss_freq_sync",
                     gr::io_signature::make(1, 8, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),

    d_fftl(fftl),
    d_rxant(rxant),
    d_pss_pos(-1),
    d_N_id_2(-1),
    d_sampl(0),
    d_f_count(0),
    d_f_est(0.0),
    d_sig(sig)
{
    int alig = volk_get_alignment();

    d_pssX = (gr_complex*)volk_malloc(sizeof(gr_complex)*d_fftl, alig);

    d_buf_pss = new gr_complex* [rxant];
    for(int i=0; i<d_rxant; i++)
        d_buf_pss[i] = (gr_complex*) volk_malloc(sizeof(gr_complex)*d_fftl, alig);

    //set_max_noutput_items(fftl*75);   work call for maximum one pss

    d_id_key = pmt::string_to_symbol("N_id_2");

}

/*
 * Our virtual destructor.
 */
mimo_pss_freq_sync_impl::~mimo_pss_freq_sync_impl()
{

    for(int i=0; i<d_rxant; i++)
        volk_free(d_buf_pss[i]);
    delete[] d_buf_pss;
    volk_free(d_pssX);
}

int
mimo_pss_freq_sync_impl::work(int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
{

    long nir = nitems_read(0);
    int nout = noutput_items;

    std::vector <gr::tag_t> v;
    get_tags_in_range(v, 0, nir, nir+nout, d_id_key);
    for (int m = 0 ; m < v.size() ; m++)
    {
        uint64_t pss_pos = v[m].offset;
        pss_pos += 6*d_fftl+(6*144+160)*d_fftl/2048;
        if( pss_pos != d_pss_pos)
        {
            //printf("\tnew psspos = %li\n", pss_pos );
            d_pss_pos = pss_pos;
        }
        int n_id_2 = int(pmt::to_long(v[m].value));
        if( n_id_2 != d_N_id_2)
        {
            //printf("%s\tASYNC!\tnew Nid2 = %i\t\n", name().c_str(), d_N_id_2);
            d_N_id_2 = n_id_2;
            pss::gen_conj_pss_t(d_pssX, d_N_id_2, d_fftl);
        }
    }

    //no pss sync yet
    if(d_N_id_2 == -1)
        return nout;

    //found part of pss in last workcall
    if(d_sampl>0 && d_sampl < d_fftl)
    {
        int n=d_fftl-d_sampl;
        if(nout<d_fftl-d_sampl)
            n=nout;
        mult_memcpy(d_buf_pss, input_items, d_sampl, 0, d_rxant, n);
        d_sampl+=n;
    }
    //complete pss is in buffer
    if(d_sampl==d_fftl)
    {
        d_sampl=0;
        calc_freq_off();
        return nout;
    }
    //test pss start in input range
    if(nir<=d_pss_pos && nir+nout>d_pss_pos)
    {
        d_sampl=nout-(int)(d_pss_pos-nir);   //max amount of pss samples in input range
        int input_pos = nout-d_sampl;        //pos of first pss sample in input range
        if(d_sampl>d_fftl)
            d_sampl=d_fftl;
        mult_memcpy(d_buf_pss, input_items, 0, input_pos, d_rxant, d_sampl);

    }
    // Tell runtime system how many output items we produced.
    return nout;
}

//for better readability
void
mimo_pss_freq_sync_impl::mult_memcpy(gr_complex** &out,
                                     const gr_vector_const_void_star &in,
                                     int out_pos,
                                     int in_pos,
                                     int multi,
                                     int n)
{
    for(int i=0; i<multi; i++)
        memcpy(out[i]+out_pos, (gr_complex*)in[i]+in_pos, n*sizeof(gr_complex));
}



void
mimo_pss_freq_sync_impl::calc_freq_off()
{
    gr_complex psscorr_a;
    gr_complex psscorr_b;
    gr_complex psscorr;

    gr_complex pss_power;
    float pss_power_abs;
    float sum_power = 0;

    float freq=0;


    for(int i=0; i<d_rxant; i++)
    {
        volk_32fc_x2_dot_prod_32fc(&psscorr_a, d_buf_pss[i],          d_pssX,          d_fftl/2);
        volk_32fc_x2_dot_prod_32fc(&psscorr_b, d_buf_pss[i]+d_fftl/2, d_pssX+d_fftl/2, d_fftl/2);
        psscorr = conj(psscorr_a) * psscorr_b;

        //add frequency estimate proportional to power of pss
        volk_32fc_x2_dot_prod_32fc(&pss_power, d_buf_pss[i], d_buf_pss[i], d_fftl);
        //printf( "POWER ant%i: %e\n", i, abs(pss_power*pss_power) );
        pss_power_abs = abs( pss_power * pss_power );
        sum_power += pss_power_abs;
        freq += pss_power_abs * arg( psscorr );
    }
    d_f_count++;
    freq=15000.0*freq/(M_PI * sum_power);

    //bigger steps when starting
    float a=0.8/d_f_count;
    a=a<0.01 ? 0.01 : a;

    d_f_est = d_f_est + (a * freq);

    (*d_sig).set_frequency((-1)*double(d_f_est) );
    //printf("FREQ SYNC: estimate=%f, new freq-compensate: %f\n", freq, d_f_est);
}


} /* namespace lte */
} /* namespace gr */


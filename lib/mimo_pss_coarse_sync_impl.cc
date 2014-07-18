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
#include "mimo_pss_coarse_sync_impl.h"


#include <cstdio>
#include <cmath>
#include <volk/volk.h>
#include "lte/pss.h"


namespace gr
{
namespace lte
{

mimo_pss_coarse_sync::sptr
mimo_pss_coarse_sync::make(int fftl, int syncl, int rxant)
{
    return gnuradio::get_initial_sptr
           (new mimo_pss_coarse_sync_impl(fftl, syncl, rxant));
}

/*
 * The private constructor
 */
mimo_pss_coarse_sync_impl::mimo_pss_coarse_sync_impl(int fftl, int syncl, int rxant)
    : gr::sync_block("mimo_pss_coarse_sync",
                     gr::io_signature::make(1, 8, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),
    d_syncl(syncl),
    d_rxant(rxant),
    d_work_call(0),
    d_posmax(0),
    d_max(0)
{
    //int decim = fftl/d_CORRL;
	//std::vector< float > tapsd(10,4);
	//d_fir = new filter::kernel::fir_filter_ccf(1, tapsd);
    //d_fir = new filter::kernel::fir_filter_ccf(decim, taps);


    //make sure that there are enough input items for
    //a 128 point correalation at position 9600 (index=9599)
    set_output_multiple(d_TIME_HYPO+d_CORRL-1);

    size_t alig = volk_get_alignment();
    d_a = (gr_complex*)volk_malloc(sizeof(gr_complex)*4,alig);

    d_port_N_id_2 = pmt::string_to_symbol("N_id_2");
    message_port_register_out(d_port_N_id_2);
    d_port_coarse_pos = pmt::string_to_symbol("coarse_pos");
    message_port_register_out(d_port_coarse_pos);
    d_port_control = pmt::string_to_symbol("control");
    message_port_register_out(d_port_control);

    //binary representation of 0.0 is 0
    memset(d_result, 0, sizeof(float)*d_TIME_HYPO);

    prepare_corr_vecs();
}

/*
 * Our virtual destructor.
 */
mimo_pss_coarse_sync_impl::~mimo_pss_coarse_sync_impl()
{
    volk_free(d_a);
}

int
mimo_pss_coarse_sync_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
{

    //deactivate block after synclen ist reached
    if(d_work_call==d_syncl)
        return noutput_items;


    //printf("---BEGIN coarse timing---\n");

    for(int d = 0; d < d_TIME_HYPO; d++)
    {
        //TODO use aligned vectors, faster?, memcpy(tmp, in,...)
        d_result[d] += diff_corr2(input_items, d_pss012_t, d_CORRL, d);

        if(d_result[d]>d_max)
        {
            d_max=d_result[d];
            d_posmax=d;
            printf("%s:new coarse timing max found: val=%f \t pos=%i\t syncloop=%i\n",name().c_str(), d_max, d_posmax, d_work_call+1);
        }
    }

    //printf("---END coarse timing---\n");

    d_work_call++;

    if(d_work_call!=d_syncl)
        return d_TIME_HYPO;

    //obtain N_id_2 after coarse timinig sync
    //for now only last received vector is used,
    d_N_id_2 = calc_N_id_2(input_items, d_posmax);

    //publish results
    message_port_pub(d_port_N_id_2, pmt::from_long((long)d_N_id_2));
    message_port_pub(d_port_coarse_pos, pmt::from_long((long)d_posmax) );

    //stop coarse calculation
    message_port_pub(d_port_control, pmt::PMT_T);

    printf("\n%s:found N_id_2=%i\n", name().c_str(), d_N_id_2);
    printf("%s:coarse pss-pos=%i\n", name().c_str(), d_posmax);

    //Tell runtime system how many output items we produced
    return noutput_items;
}



int
mimo_pss_coarse_sync_impl::calc_N_id_2(const gr_vector_const_void_star &in, const int &mpos)
{

    float max0;
    max0=diff_corr2(in, d_pss0_t, d_CORRL, mpos);

    float max1;
    max1=diff_corr2(in, d_pss1_t, d_CORRL, mpos);

    float max2;
    max2=diff_corr2(in, d_pss2_t, d_CORRL, mpos);

    int id=0;
    float max=max0;

    if(max1 > max)
    {
        id=1;
        max=max1;
    }
    if(max2 > max)
    {
        id=2;
    }

    return id;
}


void
mimo_pss_coarse_sync_impl::prepare_corr_vecs()
{
    //init pss sequences in time domain (conj for correlations)
    pss::gen_conj_pss_t(d_pss0_t, 0, d_CORRL);
    pss::gen_conj_pss_t(d_pss1_t, 1, d_CORRL);
    pss::gen_conj_pss_t(d_pss2_t, 2, d_CORRL);

    //add pss' for correlation
    for(int i=0; i<d_CORRL; i++)
    {
        d_pss012_t[i]=d_pss0_t[i]+d_pss1_t[i]+d_pss2_t[i];
    }




}




//differential correlation for n streams
float
mimo_pss_coarse_sync_impl::diff_corr2(const gr_vector_const_void_star &in, const gr_complex* y, int len, int cpos)
{
    float val=0;
    for(int rx=0; rx<d_rxant; rx++)
        val+=diff_corr((gr_complex*) in[rx]+cpos, y, len);
    return val;
}


//calculate differential correlation, 4parts, returns absolute value
float
mimo_pss_coarse_sync_impl::diff_corr(const gr_complex* x,const gr_complex* y, int len)
{
    volk_32fc_x2_dot_prod_32fc(d_a,   x,         y,         len/4);
    volk_32fc_x2_dot_prod_32fc(d_a+1, x+len/4*1, y+len/4*1, len/4);
    volk_32fc_x2_dot_prod_32fc(d_a+2, x+len/4*2, y+len/4*2, len/4);
    volk_32fc_x2_dot_prod_32fc(d_a+3, x+len/4*3, y+len/4*3, len/4);

    return abs(d_a[0]*conj(d_a[1]) + d_a[1]*conj(d_a[2]) + d_a[2]*conj(d_a[3]));
}

} /* namespace lte */
} /* namespace gr */


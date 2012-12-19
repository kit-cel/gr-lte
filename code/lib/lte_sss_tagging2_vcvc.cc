/* -*- c++ -*- */
/*
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
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
#include <lte_sss_tagging2_vcvc.h>


lte_sss_tagging2_vcvc_sptr
lte_make_sss_tagging2_vcvc (int fftl)
{
	return lte_sss_tagging2_vcvc_sptr (new lte_sss_tagging2_vcvc (fftl));
}


lte_sss_tagging2_vcvc::lte_sss_tagging2_vcvc (int fftl)
	: gr_sync_block ("sss_tagging2_vcvc",
		gr_make_io_signature (1, 1, sizeof(gr_complex)*fftl),
		gr_make_io_signature (1, 1, sizeof(gr_complex)*fftl)),
		d_fftl(fftl),
		d_frame_start(200)
{
    set_tag_propagation_policy(TPP_DONT);
    d_key=pmt::pmt_string_to_symbol("symbol");
	d_tag=pmt::pmt_string_to_symbol(name() );
}


lte_sss_tagging2_vcvc::~lte_sss_tagging2_vcvc ()
{
}


int
lte_sss_tagging2_vcvc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	if(d_frame_start > 140){ // as long as frame_start is not set --> do nothing!
        return noutput_items;
	}
    // Value needed mutiple times
    long nir = nitems_read(0);

    //Produce output
    memcpy(out, in, sizeof(gr_complex)*d_fftl * noutput_items );

    // Set Tags properly
    for(int i = 0; i < noutput_items ; i++){
        add_item_tag(0, nir+i,d_key, pmt::pmt_from_long( (nir+i+140-d_frame_start)%140 ),d_tag);
        //printf("%s\tsymbol = %ld\n", name().c_str(), (nir+i+140-d_frame_start)%140 );
    }

	// Tell runtime system how many output items we produced.
	return noutput_items;
}


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

#include <gr_io_signature.h>
#include "lte_channel_estimator_vcvc.h"

lte_channel_estimator_vcvc_sptr
lte_make_channel_estimator_vcvc (int subcarriers,
                               int N_ofdm_symbols,
                               std::string tag_key,
                               std::string msg_buf_name,
                               const std::vector<std::vector<int> > &pilot_carriers,
                               const std::vector<std::vector<gr_complex> > &pilot_symbols)
{
	return gnuradio::get_initial_sptr (new lte_channel_estimator_vcvc(subcarriers, N_ofdm_symbols, tag_key, msg_buf_name, pilot_carriers, pilot_symbols));
}


/*
 * The private constructor
 */
lte_channel_estimator_vcvc::lte_channel_estimator_vcvc (int subcarriers,
                                                    int N_ofdm_symbols,
                                                    std::string tag_key,
                                                    std::string msg_buf_name,
                                                    const std::vector<std::vector<int> > &pilot_carriers,
                                                    const std::vector<std::vector<gr_complex> > &pilot_symbols)
  : gr_sync_block ("channel_estimator_vcvc",
		   gr_make_io_signature(1, 1, sizeof(gr_complex) * subcarriers ),
		   gr_make_io_signature(1, 1, sizeof(gr_complex) * subcarriers )),
		   d_subcarriers(subcarriers),
		   d_N_ofdm_symbols(N_ofdm_symbols)
{
    d_key=pmt::pmt_string_to_symbol(tag_key); // specify key of tag.

    message_port_register_in(pmt::mp(msg_buf_name));
    //set_msg_handler(pmt::mp(tag_key), boost::bind(&lte_channel_estimator::set_cell_id_msg, this, _1));

    set_pilot_map(pilot_carriers, pilot_symbols);



}


/*
 * Our virtual destructor.
 */
lte_channel_estimator_vcvc::~lte_channel_estimator_vcvc()
{
	// Put in <+destructor stuff+> here
}


int
lte_channel_estimator_vcvc::work(int noutput_items,
		  gr_vector_const_void_star &input_items,
		  gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

    int slot_number = 5;
    std::vector <gr_tag_t> v_b;
    get_tags_in_range(v_b, 0, nitems_read(0), nitems_read(0)+noutput_items, d_key);
    for(int i = 0; i < v_b.size() ; i++){
        long tag_offset     = v_b[i].offset;
        int value           = int(pmt::pmt_to_long(v_b[i].value) );
        //printf("%s\t%i\t%ld\n", name().c_str(), value, tag_offset);
    }


	// Tell runtime system how many output items we produced.
	return noutput_items;
}

void
lte_channel_estimator_vcvc::set_pilot_map(const std::vector<std::vector<int> > &pilot_carriers,
                                        const std::vector<std::vector<gr_complex> > &pilot_symbols)
{
    for( int i = 0; i < pilot_symbols.size(); i++){
        for(int c = 0; c < pilot_symbols[i].size(); c++){
            printf("value = %+1.2f %+1.2f\n", pilot_symbols[i][c].real(), pilot_symbols[i][c].imag());
        }
    }
}






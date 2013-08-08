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
#include "lte_pcfich_demux_vcvc.h"

lte_pcfich_demux_vcvc_sptr
lte_make_pcfich_demux_vcvc (int N_rb_dl, std::string key, std::string msg_buf_name)
{
	return gnuradio::get_initial_sptr (new lte_pcfich_demux_vcvc(N_rb_dl, key, msg_buf_name));
}


/*
 * The private constructor
 */
lte_pcfich_demux_vcvc::lte_pcfich_demux_vcvc (int N_rb_dl, std::string key, std::string msg_buf_name)
  : gr_block ("pcfich_demux_vcvc",
		   gr_make_io_signature(3, 3, sizeof(gr_complex) * 12 * N_rb_dl),
		   gr_make_io_signature(3, 3, sizeof(gr_complex) * 16)),
		   d_cell_id(0),
		   d_N_rb_dl(N_rb_dl)
{
    d_key=pmt::pmt_string_to_symbol(key); // specify key of tag.
    d_msg_buf = pmt::mp(msg_buf_name);
    message_port_register_in(d_msg_buf);

    set_cell_id(0);

//    set_msg_handler(d_msg_buf, boost::bind(&lte_pcfich_demux_vcvc::handle_msg, this, _1));
}


/*
 * Our virtual destructor.
 */
lte_pcfich_demux_vcvc::~lte_pcfich_demux_vcvc()
{
	// Put in <+destructor stuff+> here
}


//void
//lte_pcfich_demux_vcvc::forecast (int noutput_items, gr_vector_int &ninput_items_required)
//{
//	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
//}

int
lte_pcfich_demux_vcvc::general_work (int noutput_items,
				   gr_vector_int &ninput_items,
				   gr_vector_const_void_star &input_items,
				   gr_vector_void_star &output_items)
{
	const gr_complex *in0 = (const gr_complex *) input_items[0];
	const gr_complex *in1 = (const gr_complex *) input_items[1];
	const gr_complex *in2 = (const gr_complex *) input_items[2];
	gr_complex *out0 = (gr_complex *) output_items[0];
	gr_complex *out1 = (gr_complex *) output_items[1];
	gr_complex *out2 = (gr_complex *) output_items[2];

	printf("this is work %i\n", noutput_items);

	// Do <+signal processing+>
	// Tell runtime system how many input items we consumed on
	// each input stream.
	consume_each (noutput_items);

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

void
lte_pcfich_demux_vcvc::set_cell_id(int id)
{
    d_cell_id = id;
    initialize_pcfich(d_N_rb_dl);
    update_pcfich_pos(d_N_rb_dl, d_cell_id);
    for(int i = 0; i < 12* d_N_rb_dl; i++){
        printf("%i\t%i\n", i, d_pcfich_pos[i]);
    }
}

void
lte_pcfich_demux_vcvc::initialize_pcfich(int N_rb_dl)
{
    d_pcfich_pos.clear();
    int n_subcarriers = 12 * N_rb_dl;
    for(int i = 0 ; i < n_subcarriers; i++){
        d_pcfich_pos.push_back(0);
    }
}

void
lte_pcfich_demux_vcvc::update_pcfich_pos(int N_rb_dl, int cell_id)
{
    const int N_SC_RB = 12;
    const int K_MEAN = (N_SC_RB/2) * (cell_id%(2*N_rb_dl));
    int n_subcarriers = N_rb_dl*N_SC_RB;
    //regs = 4
    int cell_id_mod3 = cell_id%3;
    for(int n = 0; n < 4; n++){
        int k = int( (K_MEAN + (N_SC_RB/2) * ((n*N_rb_dl/2)%n_subcarriers) ) );
        for(int i = 0; i < 6; i++){
            if(i%3 != cell_id_mod3){
                d_pcfich_pos[k+i] = 1;
            }
        }
    }
}




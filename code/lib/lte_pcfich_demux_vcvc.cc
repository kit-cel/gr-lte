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
lte_make_pcfich_demux_vcvc (int N_rb_dl, std::string key, std::string out_key, std::string msg_buf_name)
{
	return gnuradio::get_initial_sptr (new lte_pcfich_demux_vcvc(N_rb_dl, key, out_key, msg_buf_name));
}


/*
 * The private constructor
 */
lte_pcfich_demux_vcvc::lte_pcfich_demux_vcvc (int N_rb_dl, std::string key, std::string out_key, std::string msg_buf_name)
  : gr_block ("pcfich_demux_vcvc",
		   gr_make_io_signature(3, 3, sizeof(gr_complex) * 12 * N_rb_dl),
		   gr_make_io_signature(3, 3, sizeof(gr_complex) * 16)),
		   d_cell_id(0),
		   d_sym_num(0),
		   d_N_rb_dl(N_rb_dl)
{
    d_key = pmt::pmt_string_to_symbol(key); // specify key of tag.
    d_out_key = pmt::pmt_string_to_symbol(out_key);
    d_tag_id = pmt::pmt_string_to_symbol( name() );
    d_msg_buf = pmt::mp(msg_buf_name);
    message_port_register_in(d_msg_buf);
    set_msg_handler(d_msg_buf, boost::bind(&lte_pcfich_demux_vcvc::handle_msg, this, _1));

    set_tag_propagation_policy(TPP_DONT);

    set_cell_id(0);
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

	// In this case for configuration overview some variables declared right at the beginning.
    int out_port = 0;
    int n_subcarriers = 12*d_N_rb_dl;
    int ninitems = calculate_n_process_items(ninput_items);
    int nout_items = 0;

    std::vector<gr_tag_t> tags;
    get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+ninitems);
    int sym_num = get_sym_num(tags);

    for(int i = 0; i < ninitems && nout_items < noutput_items; i++){
        int curr_sym = sym_num+i;
        if((curr_sym)%14 == 0){
            int subframe = (curr_sym/14)%10;
            // copy REs of interest from input to output buffer
            extract_pcfich(out0, out1, out2, in0, in1, in2);
            // produced output. move output pointers.
            out0 += 16;
            out1 += 16;
            out2 += 16;

            // This should tag every sample with a tag containing a value which corresponds to the subframe!
            long offset = nitems_written(0) + nout_items;
            pmt::pmt_t value = pmt::pmt_from_long(subframe);
            //printf("port = %i\toffset = %ld\tkey = %s\tvalue = %ld\tid = %s\n", out_port, offset, pmt::pmt_symbol_to_string(d_out_key).c_str(), pmt::pmt_to_long(value), pmt::pmt_symbol_to_string(d_tag_id).c_str() );
            add_item_tag(out_port, offset, d_out_key, value, d_tag_id);

            nout_items++; // produced output!
        }
        //move input buffer pointers after every iteration.
        in0 += n_subcarriers;
        in1 += n_subcarriers;
        in2 += n_subcarriers;
    }

	// Tell runtime system how many input items we consumed on
	// each input stream.
	consume_each(ninitems);

	// Tell runtime system how many output items we produced.
	return nout_items;
}

void
lte_pcfich_demux_vcvc::handle_msg(pmt::pmt_t msg)
{
    set_cell_id( int(pmt::pmt_to_long(msg)) );
}

void
lte_pcfich_demux_vcvc::set_cell_id(int id)
{
    d_cell_id = id;
    update_pcfich_pos(d_N_rb_dl, d_cell_id);
//    for(int i = 0; i < d_pcfich_pos.size(); i++){
//        printf("%i\t%i\n", i, d_pcfich_pos[i]);
//    }
}

void
lte_pcfich_demux_vcvc::update_pcfich_pos(int N_rb_dl, int cell_id)
{
    d_pcfich_pos.clear();
    const int N_SC_RB = 12;
    const int K_MEAN = (N_SC_RB/2) * (cell_id%(2*N_rb_dl));
    int n_subcarriers = N_rb_dl*N_SC_RB;
    //regs = 4
    int cell_id_mod3 = cell_id%3;
    for(int n = 0; n < 4; n++){
        int k = int( (K_MEAN + (N_SC_RB/2) * (n*(N_rb_dl/2) ) )%n_subcarriers );
        for(int i = 0; i < 6; i++){
            if(i%3 != cell_id_mod3){
                d_pcfich_pos.push_back(k+i);
            }
        }
    }
}

inline int
lte_pcfich_demux_vcvc::calculate_n_process_items(gr_vector_int ninput_items)
{
    // get smallest number of input items
    int n_inputs = ninput_items.size();
    int ninitems = ninput_items[0];
    for(int i = 1 ; i < n_inputs ; i++){
        if(ninitems > ninput_items[i]){
            ninitems = ninput_items[i];
        }
    }
    return ninitems;
}

inline int
lte_pcfich_demux_vcvc::get_sym_num(std::vector<gr_tag_t> v)
{
    int sym_num = 0;
    if(v.size() > 0){
        int value = int(pmt::pmt_to_long(v[0].value) );
        int rel_offset = v[0].offset - nitems_read(0);
	    sym_num = (value+140-rel_offset)%140;
	}
	else{
        sym_num = d_sym_num;
	}
	return sym_num;
}

void
lte_pcfich_demux_vcvc::extract_pcfich(gr_complex* out0, gr_complex* out1, gr_complex* out2, const gr_complex* in0, const gr_complex* in1, const gr_complex* in2)
{
    int len = d_pcfich_pos.size();
    for(int i = 0; i < len; i++){
        *out0 = *(in0 + d_pcfich_pos[i]);
        *out1 = *(in1 + d_pcfich_pos[i]);
        *out2 = *(in2 + d_pcfich_pos[i]);
        out0++;
        out1++;
        out2++;
    }
}




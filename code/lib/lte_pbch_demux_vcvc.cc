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
#include <lte_pbch_demux_vcvc.h>
#include <cstdio>


lte_pbch_demux_vcvc_sptr
lte_make_pbch_demux_vcvc (int N_rb_dl) //int cell_id,
{
	return lte_pbch_demux_vcvc_sptr (new lte_pbch_demux_vcvc (N_rb_dl)); //cell_id,
}


lte_pbch_demux_vcvc::lte_pbch_demux_vcvc (int N_rb_dl) //int cell_id,
	: gr_block ("pbch_demux_vcvc",
		gr_make_io_signature (3,3, sizeof (gr_complex)*12*N_rb_dl),
		gr_make_io_signature (3,3, sizeof (gr_complex)*240)),
		d_cell_id(-1),
		d_N_rb_dl(N_rb_dl),
		d_sym_num(0)
{
    message_port_register_in(pmt::mp("cell_id"));
    set_msg_handler(pmt::mp("cell_id"), boost::bind(&lte_pbch_demux_vcvc::set_cell_id_msg, this, _1));
}


lte_pbch_demux_vcvc::~lte_pbch_demux_vcvc ()
{
}


int
lte_pbch_demux_vcvc::general_work (int noutput_items,
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

	// get smallest number of input items
    int ninitems = calculate_n_process_items(ninput_items, noutput_items);

    // No data is processed as long as the cell_id is not available
    if(d_cell_id < 0){
        consume_each(ninitems);
        return 0;
    }

  	// set noutput_items to zero. if output is produced, noutput_items is incremented.
	noutput_items = 0;

    int cell_id_mod3 = d_cell_id%3;
    int n_carriers = 12*d_N_rb_dl;

    //Read tags for updated sym_num
    std::vector<gr_tag_t> v;
    get_tags_in_range(v,0,nitems_read(0),nitems_read(0)+ninitems);
    int sym_num = get_sym_num(v);

    //This loop searches for the REs with the PBCH and copies them to the output stream.
    for (int i = 0 ; i < ninitems ; i++ ) {
        if(sym_num==7){
            if (ninitems-i < 4){
                ninitems = i;
                break;
            }
            extract_pbch_values(in0, in1, in2, out0, out1, out2);

            noutput_items++;
            out0+=240;
            out1+=240;
            out2+=240;
        }

        // update work values for next symbol
        sym_num = (sym_num+1)%140;
        in0+=n_carriers;
        in1+=n_carriers;
        in2+=n_carriers;
    }

    // update d_sym_num
    d_sym_num = sym_num;
    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each (ninitems);
    // Tell runtime system how many output items we produced.
    return noutput_items;
}

void
lte_pbch_demux_vcvc::set_cell_id_msg(pmt::pmt_t msg)
{
    int cell_id = int(pmt::pmt_to_long(msg));
    //printf("********%s INPUT MESSAGE***************\n", name().c_str() );
    //printf("\t%i\n", cell_id);
    //printf("********%s INPUT MESSAGE***************\n", name().c_str() );
    set_cell_id(cell_id);
}

void
lte_pbch_demux_vcvc::set_cell_id(int id)
{
    d_cell_id = id;
    printf("%s\t\tset_cell_id = %i\n", name().c_str(), d_cell_id );
}

inline int
lte_pbch_demux_vcvc::calculate_n_process_items(gr_vector_int ninput_items, int noutput_items)
{
    // get smallest number of input items
    int n_inputs = ninput_items.size();
    int ninitems = ninput_items[0];
    for(int i = 1 ; i < n_inputs ; i++){
        if(ninitems > ninput_items[i]){
            ninitems = ninput_items[i];
        }
    }
    if (ninitems > noutput_items){
        ninitems = noutput_items;
    }
    return ninitems;
}

void
lte_pbch_demux_vcvc::extract_pbch_values(const gr_complex* in0, const gr_complex* in1, const gr_complex* in2,
                                            gr_complex* out0, gr_complex* out1, gr_complex* out2)
{
    int cell_id_mod3 = d_cell_id%3;
    int n_carriers = 12*d_N_rb_dl;
    int pbch_pos = (n_carriers/2)-(72/2);
    int idx = 0;
    for (int c = 0 ; c < 72 ; c++ ) {
        if ( cell_id_mod3 != c%3 ){
            out0[idx]    = in0[pbch_pos+c];
            out0[idx+48] = in0[pbch_pos+c+n_carriers];
            out1[idx]    = in1[pbch_pos+c];
            out1[idx+48] = in1[pbch_pos+c+n_carriers];
            out2[idx]    = in2[pbch_pos+c];
            out2[idx+48] = in2[pbch_pos+c+n_carriers];
            idx++;
        }
    }
    //Copy PBCH values on symbol 9
    memcpy(out0+96, in0+2*n_carriers+pbch_pos, 72*sizeof(gr_complex) );
    memcpy(out1+96, in1+2*n_carriers+pbch_pos, 72*sizeof(gr_complex) );
    memcpy(out2+96, in2+2*n_carriers+pbch_pos, 72*sizeof(gr_complex) );
    //Copy PBCH values on symbol 10
    memcpy(out0+96+72, in0+3*n_carriers+pbch_pos, 72*sizeof(gr_complex) );
    memcpy(out1+96+72, in1+3*n_carriers+pbch_pos, 72*sizeof(gr_complex) );
    memcpy(out2+96+72, in2+3*n_carriers+pbch_pos, 72*sizeof(gr_complex) );
}

inline int
lte_pbch_demux_vcvc::get_sym_num(std::vector<gr_tag_t> v)
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





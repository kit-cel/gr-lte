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
#include "pbch_demux_vcvc_impl.h"

#include <cstdio>

namespace gr {
  namespace lte {

    pbch_demux_vcvc::sptr
    pbch_demux_vcvc::make(int N_rb_dl, std::string name)
    {
      return gnuradio::get_initial_sptr
        (new pbch_demux_vcvc_impl(N_rb_dl, name));
    }

    /*
     * The private constructor
     */
    pbch_demux_vcvc_impl::pbch_demux_vcvc_impl(int N_rb_dl, std::string& name)
      : gr::block(name,
              gr::io_signature::make( 1, 1, sizeof(gr_complex) * 12 * N_rb_dl),
              gr::io_signature::make( 1, 1, sizeof(gr_complex) * 240)),
              d_cell_id(-1),
			  d_N_rb_dl(N_rb_dl),
			  d_sym_num(0)
    {
		message_port_register_in(pmt::mp("cell_id"));
		set_msg_handler(pmt::mp("cell_id"), boost::bind(&pbch_demux_vcvc_impl::set_cell_id_msg, this, _1));

	}

    /*
     * Our virtual destructor.
     */
    pbch_demux_vcvc_impl::~pbch_demux_vcvc_impl()
    {
    }

    void
    pbch_demux_vcvc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        for(int i = 0 ; i < ninput_items_required.size() ; i++){
			ninput_items_required[0] = noutput_items;
		}
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    pbch_demux_vcvc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
		gr_complex *out = (gr_complex *) output_items[0];
		
		// get smallest number of input items
		int ninitems = calculate_n_process_items(ninput_items, noutput_items);
		//~ printf("this is a demux with %i items \n", ninitems);

		// No data is processed as long as the cell_id is not available
		if(d_cell_id < 0){
			return 0;
		}

		// set noutput_items to zero. if output is produced, noutput_items is incremented.
		noutput_items = 0;

		//Read tags for updated sym_num
//		std::vector<gr::tag_t> v;
		get_tags_in_range(d_tags_v, 0, nitems_read(0), nitems_read(0)+ninitems);
		int sym_num = get_sym_num(d_tags_v);

		int n_carriers = 12 * d_N_rb_dl;
		//This loop searches for the REs with the PBCH and copies them to the output stream.
		for (int i = 0 ; i < ninitems ; i++ ) {
			if(sym_num==7){ // found position of first PBCH values in frame.
				if (ninitems-i < 4){ // make sure whole PBCH is available.
					ninitems = i;
					break;
				}
				extract_pbch_values(out, in);

				noutput_items++;
				out += 240;
			}
			// update work values for next symbol
			sym_num = (sym_num+1)%140;
			in += n_carriers;
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
	pbch_demux_vcvc_impl::set_cell_id_msg(pmt::pmt_t msg)
	{
		int cell_id = int(pmt::to_long(msg));
		//printf("********%s INPUT MESSAGE***************\n", name().c_str() );
		//printf("\t%i\n", cell_id);
		//printf("********%s INPUT MESSAGE***************\n", name().c_str() );
		set_cell_id(cell_id);
	}

	void
	pbch_demux_vcvc_impl::set_cell_id(int id)
	{
		d_cell_id = id;
		//~ printf("%s\t\tset_cell_id = %i\n", name().c_str(), d_cell_id );
	}

	int
	pbch_demux_vcvc_impl::calculate_n_process_items(const gr_vector_int& ninput_items, const int noutput_items)
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
	pbch_demux_vcvc_impl::extract_pbch_values(gr_complex* out,
												const gr_complex* in)
	{
		int cell_id_mod3 = d_cell_id%3;
		int n_carriers = 12*d_N_rb_dl;
		int pbch_pos = (n_carriers/2)-(72/2);
		int idx = 0;
		for (int c = 0 ; c < 72 ; c++ ) {
			if ( cell_id_mod3 != c%3 ){
				out[idx]    = in[pbch_pos+c];
				out[idx+48] = in[pbch_pos+c+n_carriers];
				idx++;
			}
		}
		//Copy PBCH values on symbol 9
		memcpy(out+96, in+2*n_carriers+pbch_pos, 72*sizeof(gr_complex) );
		//Copy PBCH values on symbol 10
		memcpy(out+96+72, in+3*n_carriers+pbch_pos, 72*sizeof(gr_complex) );
	}

	int
	pbch_demux_vcvc_impl::get_sym_num(const std::vector<gr::tag_t>& v)
	{
		int sym_num = 0;
		if(v.size() > 0){
			int value = int(pmt::to_long(v[0].value) );
			int rel_offset = v[0].offset - nitems_read(0);
			sym_num = (value+140-rel_offset)%140;
		}
		else{
			sym_num = d_sym_num;
		}
		return sym_num;
	}


  } /* namespace lte */
} /* namespace gr */


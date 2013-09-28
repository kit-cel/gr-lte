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

#include <gnuradio/io_signature.h>
#include "bch_crc_check_ant_chooser_bb_impl.h"
#include<cstdio>

namespace gr {
  namespace lte {

    bch_crc_check_ant_chooser_bb::sptr
    bch_crc_check_ant_chooser_bb::make()
    {
      return gnuradio::get_initial_sptr
        (new bch_crc_check_ant_chooser_bb_impl());
    }

    /*
     * The private constructor
     */
    bch_crc_check_ant_chooser_bb_impl::bch_crc_check_ant_chooser_bb_impl()
      : gr::sync_block("bch_crc_check_ant_chooser_bb",
              gr::io_signature::make( 3, 3, sizeof(char)),
              gr::io_signature::make( 1, 1, sizeof(char)))
    {}

    /*
     * Our virtual destructor.
     */
    bch_crc_check_ant_chooser_bb_impl::~bch_crc_check_ant_chooser_bb_impl()
    {
    }

    int
    bch_crc_check_ant_chooser_bb_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const char *in1 = (const char *) input_items[0];
        const char *in2 = (const char *) input_items[1];
        const char *in4 = (const char *) input_items[2];
        char *out = (char *) output_items[0];
        
        //~ printf("call to selector %i\n", noutput_items);
        int ant = 0;
        for(int i = 0 ; i < noutput_items ; i++){
			if( (int) *in1 == 1){ ant = 1;}
			else if( (int) *in2 == 1){ ant = 2;}
			else if( (int) *in4 == 1){ ant = 4;}
			else{ ant = 0;}
			*out = ant;
			//~ printf("ant1 = %i\tant2 = %i\t ant4 = %i\tdetected antennas %i\n", (int)*in1, (int)*in2, (int)*in4, ant );
			in1++;
			in2++;
			in4++;
			out++;
		}
		//~ printf("call to selector processed items = %i\n", noutput_items);
		

        return noutput_items;
    }

  } /* namespace lte */
} /* namespace gr */


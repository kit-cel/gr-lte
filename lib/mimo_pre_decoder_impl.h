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

#ifndef INCLUDED_LTE_MIMO_PRE_DECODER_IMPL_H
#define INCLUDED_LTE_MIMO_PRE_DECODER_IMPL_H

#include <lte/mimo_pre_decoder.h>

namespace gr {
  namespace lte {

    class mimo_pre_decoder_impl : public mimo_pre_decoder
    {
     private:
		int d_N_ant;
		int d_rxant;
		int d_vlen;
		std::string d_style;

		void handle_msg(pmt::pmt_t msg);

		void decode_1_ant(gr_complex* out, const gr_complex* rx, const gr_complex* h, int len);

		void prepare_2_ant_vectors(gr_complex* h0,
										  gr_complex* h1,
										  gr_complex* r0,
										  gr_complex* r1,
										  const gr_complex* rx,
										  const gr_complex* ce0,
										  const gr_complex* ce1,
										  int len);

		void decode_2_ant(gr_complex* out0,
								 gr_complex* out1,
								 gr_complex* h0,
								 gr_complex* h1,
								 gr_complex* r0,
								 gr_complex* r1,
								 int len);

		void combine_output(gr_complex* out,
								   gr_complex* out0,
								   gr_complex* out1,
								   int len);

		gr_complex* d_h0;
		gr_complex* d_h1;
		gr_complex* d_r0;
		gr_complex* d_r1;
		gr_complex* d_out;
		gr_complex* d_out0;
		gr_complex* d_out1;
		gr_complex* d_mult0;
		gr_complex* d_mult1;
		void setup_volk_vectors(int len);

     public:
      mimo_pre_decoder_impl(int rxant, int N_ant, int vlen, std::string style);
      ~mimo_pre_decoder_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      void set_N_ant(int N_ant);
	  int get_N_ant(){return d_N_ant;}
	  void set_decoding_style(std::string style);
	  std::string get_decoding_style(){return d_style;}

    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_PRE_DECODER_IMPL_H */


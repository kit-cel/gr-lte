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

#ifndef INCLUDED_LTE_MIB_UNPACK_VBM_IMPL_H
#define INCLUDED_LTE_MIB_UNPACK_VBM_IMPL_H

#include <lte/mib_unpack_vbm.h>
#include <cstdio>

namespace gr {
  namespace lte {
	  
	struct cell_state_information {
		int N_ant;
		int N_rb_dl;
		int phich_duration;
		float phich_resources;
		void print_values(){
			printf("(N_ant=%i ", N_ant);
			printf("N_rb_dl=%i ", N_rb_dl);
			printf("PHICH: dur=%i res=%1.2f)\n", phich_duration, phich_resources);
		}
	};

    class mib_unpack_vbm_impl : public mib_unpack_vbm
    {
     private:
		cell_state_information d_state_info;
		int d_SFN;
		int d_unchanged_decodings;

		pmt::pmt_t d_port_N_ant;
		pmt::pmt_t d_port_N_rb_dl;
		pmt::pmt_t d_port_phich_duration;
		pmt::pmt_t d_port_phich_resources;
		pmt::pmt_t d_port_SFN;

		std::vector<int> d_SFN_vec;
		int d_work_calls;

		//private methods
		void decode_mib(char* mib);
		bool decode_state_mib(char* mib);
		int decode_N_rb_dl(char* mib);
		float decode_phich_resources(char* mib);
		int decode_sfn(char* mib);
		int extract_sfn_lsb_from_tag();

		void send_mib();
		void send_state_mib();
		void send_sfn();

     public:
      mib_unpack_vbm_impl();
      ~mib_unpack_vbm_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
	       
	  int   get_SFN       (){return d_SFN;}
	  std::vector<int> get_SFN_vec(){return d_SFN_vec;}
      int   get_N_ant     (){return d_state_info.N_ant;}
	  int   get_N_rb_dl   (){return d_state_info.N_rb_dl;}
	  int   get_phich_dur (){return d_state_info.phich_duration;}
	  float get_phich_res (){return d_state_info.phich_resources;}
	  float get_decoding_rate();
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIB_UNPACK_VBM_IMPL_H */


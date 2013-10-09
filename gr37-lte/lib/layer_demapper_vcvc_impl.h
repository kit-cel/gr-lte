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

#ifndef INCLUDED_LTE_LAYER_DEMAPPER_VCVC_IMPL_H
#define INCLUDED_LTE_LAYER_DEMAPPER_VCVC_IMPL_H

#include <lte/layer_demapper_vcvc.h>

namespace gr {
  namespace lte {

    class layer_demapper_vcvc_impl : public layer_demapper_vcvc
    {
     private:
      int d_N_ant;
      int d_vlen;
      std::string d_style;

      void handle_msg(pmt::pmt_t msg);

      void demap_1_ant(gr_complex* out, const gr_complex * in, int len);
      void demap_2_ant(gr_complex* out, const gr_complex * in, int len);
      void demap_4_ant(gr_complex* out, const gr_complex * in, int len);

     public:
      layer_demapper_vcvc_impl(int N_ant, int vlen, std::string style);
      ~layer_demapper_vcvc_impl();

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

#endif /* INCLUDED_LTE_LAYER_DEMAPPER_VCVC_IMPL_H */


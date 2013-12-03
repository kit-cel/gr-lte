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

#ifndef INCLUDED_LTE_QPSK_SOFT_DEMOD_VCVF_IMPL_H
#define INCLUDED_LTE_QPSK_SOFT_DEMOD_VCVF_IMPL_H

#include <lte/qpsk_soft_demod_vcvf.h>

namespace gr {
  namespace lte {

    class qpsk_soft_demod_vcvf_impl : public qpsk_soft_demod_vcvf
    {
     private:
		int d_vlen;
		const float d_SQRT2;
		float* d_demodulated;

     public:
      qpsk_soft_demod_vcvf_impl(int vlen);
      ~qpsk_soft_demod_vcvf_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_QPSK_SOFT_DEMOD_VCVF_IMPL_H */


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

#ifndef INCLUDED_LTE_MIMO_PSS_COARSE_CONTROL_IMPL_H
#define INCLUDED_LTE_MIMO_PSS_COARSE_CONTROL_IMPL_H

#include <lte/mimo_pss_coarse_control.h>

namespace gr {
  namespace lte {

    class mimo_pss_coarse_control_impl : public mimo_pss_coarse_control
    {
     private:
      bool d_control;

     public:
      mimo_pss_coarse_control_impl();
      ~mimo_pss_coarse_control_impl();

      void handle_msg_control(pmt::pmt_t msg);

      // Where all the action really happens
    int general_work(int noutput_items,
                   gr_vector_int &ninput_items,
                   gr_vector_const_void_star &input_items,
                   gr_vector_void_star &output_items);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_PSS_COARSE_CONTROL_IMPL_H */


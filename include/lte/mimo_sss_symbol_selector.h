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


#ifndef INCLUDED_LTE_MIMO_SSS_SYMBOL_SELECTOR_H
#define INCLUDED_LTE_MIMO_SSS_SYMBOL_SELECTOR_H

#include <lte/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief <+description of block+>
     * \ingroup lte
     *
     */
    class LTE_API mimo_sss_symbol_selector : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<mimo_sss_symbol_selector> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::mimo_sss_symbol_selector.
       *
       * To avoid accidental use of raw pointers, lte::mimo_sss_symbol_selector's
       * constructor is in a private implementation
       * class. lte::mimo_sss_symbol_selector::make is the public interface for
       * creating new instances.
       */
      static sptr make(int fftl, int rxant);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_SSS_SYMBOL_SELECTOR_H */


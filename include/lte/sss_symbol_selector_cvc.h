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


#ifndef INCLUDED_LTE_SSS_SYMBOL_SELECTOR_CVC_H
#define INCLUDED_LTE_SSS_SYMBOL_SELECTOR_CVC_H

#include <lte/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief <+description of block+>
     * \ingroup lte
     *
     */
    class LTE_API sss_symbol_selector_cvc : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<sss_symbol_selector_cvc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::sss_symbol_selector_cvc.
       *
       * To avoid accidental use of raw pointers, lte::sss_symbol_selector_cvc's
       * constructor is in a private implementation
       * class. lte::sss_symbol_selector_cvc::make is the public interface for
       * creating new instances.
       */
      static sptr make(int fftl, std::string name = "sss_symbol_selector_cvc");
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_SSS_SYMBOL_SELECTOR_CVC_H */


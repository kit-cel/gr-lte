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


#ifndef INCLUDED_LTE_REMOVE_CP_CVC_H
#define INCLUDED_LTE_REMOVE_CP_CVC_H

#include <lte/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief Remove LTE specific CP from stream
     * and output OFDM symbol vectors in time domain
     * \ingroup lte
     *
     */
    class LTE_API remove_cp_cvc : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<remove_cp_cvc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::remove_cp_cvc.
       *
       * To avoid accidental use of raw pointers, lte::remove_cp_cvc's
       * constructor is in a private implementation
       * class. lte::remove_cp_cvc::make is the public interface for
       * creating new instances.
       */
      static sptr make(int fftl, std::string key, std::string name = "remove_cp_cvc");
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_REMOVE_CP_CVC_H */


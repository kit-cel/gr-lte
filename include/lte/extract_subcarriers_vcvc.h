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


#ifndef INCLUDED_LTE_EXTRACT_SUBCARRIERS_VCVC_H
#define INCLUDED_LTE_EXTRACT_SUBCARRIERS_VCVC_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief Extract occupied subcarriers in frequency domain
     * \ingroup lte
     *
     */
    class LTE_API extract_subcarriers_vcvc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<extract_subcarriers_vcvc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::extract_subcarriers_vcvc.
       *
       * To avoid accidental use of raw pointers, lte::extract_subcarriers_vcvc's
       * constructor is in a private implementation
       * class. lte::extract_subcarriers_vcvc::make is the public interface for
       * creating new instances.
       */
      static sptr make(int N_rb_dl, int fftl);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_EXTRACT_SUBCARRIERS_VCVC_H */


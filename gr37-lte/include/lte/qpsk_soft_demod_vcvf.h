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


#ifndef INCLUDED_LTE_QPSK_SOFT_DEMOD_VCVF_H
#define INCLUDED_LTE_QPSK_SOFT_DEMOD_VCVF_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief QPSK soft demodulation. 1. item == real, 2. item == imaginary
     * \ingroup lte
     *
     */
    class LTE_API qpsk_soft_demod_vcvf : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<qpsk_soft_demod_vcvf> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::qpsk_soft_demod_vcvf.
       *
       * To avoid accidental use of raw pointers, lte::qpsk_soft_demod_vcvf's
       * constructor is in a private implementation
       * class. lte::qpsk_soft_demod_vcvf::make is the public interface for
       * creating new instances.
       */
      static sptr make(int vlen);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_QPSK_SOFT_DEMOD_VCVF_H */


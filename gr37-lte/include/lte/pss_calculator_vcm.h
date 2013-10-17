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


#ifndef INCLUDED_LTE_PSS_CALCULATOR_VCM_H
#define INCLUDED_LTE_PSS_CALCULATOR_VCM_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>
#include <lte/pss_tagger_cc.h>
#include <lte/pss_symbol_selector_cvc.h>

namespace gr {
  namespace lte {

    /*!
     * \brief <+description of block+>
     * \ingroup lte
     *
     */
    class LTE_API pss_calculator_vcm : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<pss_calculator_vcm> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::pss_calculator_vcm.
       *
       * To avoid accidental use of raw pointers, lte::pss_calculator_vcm's
       * constructor is in a private implementation
       * class. lte::pss_calculator_vcm::make is the public interface for
       * creating new instances.
       */
      static sptr make(int fftl);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PSS_CALCULATOR_VCM_H */


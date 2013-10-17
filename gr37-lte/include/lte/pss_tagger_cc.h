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


#ifndef INCLUDED_LTE_PSS_TAGGER_CC_H
#define INCLUDED_LTE_PSS_TAGGER_CC_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief <+description of block+>
     * \ingroup lte
     *
     */
    class LTE_API pss_tagger_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<pss_tagger_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::pss_tagger_cc.
       *
       * To avoid accidental use of raw pointers, lte::pss_tagger_cc's
       * constructor is in a private implementation
       * class. lte::pss_tagger_cc::make is the public interface for
       * creating new instances.
       */
      static sptr make(int fftl);
      
        virtual void set_half_frame_start(int start) = 0;
        virtual void set_N_id_2(int nid2) = 0;
        virtual void lock() = 0;
        virtual void unlock() = 0;
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PSS_TAGGER_CC_H */


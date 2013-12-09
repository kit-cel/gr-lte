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


#ifndef INCLUDED_LTE_DESCRAMBLER_VFVF_H
#define INCLUDED_LTE_DESCRAMBLER_VFVF_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>
#include <vector>

namespace gr {
  namespace lte {

    /*!
     * \brief LTE Descrambler
     * \ingroup lte
     * Scrambling sequences are setable on runtime
     * and chosen upon tag reception.
     */
    class LTE_API descrambler_vfvf : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<descrambler_vfvf> sptr;
      
      virtual void set_descr_seqs(std::vector<std::vector<float> > seqs) = 0;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::descrambler_vfvf.
       *
       * To avoid accidental use of raw pointers, lte::descrambler_vfvf's
       * constructor is in a private implementation
       * class. lte::descrambler_vfvf::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string tag_key, std::string msg_buf_name, int len);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_DESCRAMBLER_VFVF_H */


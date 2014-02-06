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


#ifndef INCLUDED_LTE_PCFICH_UNPACK_VFM_H
#define INCLUDED_LTE_PCFICH_UNPACK_VFM_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>
#include <vector>

namespace gr {
  namespace lte {

    /*!
     * \brief Unpack CFI and publish it on output message port
     * \ingroup lte
     *
     */
    class LTE_API pcfich_unpack_vfm : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<pcfich_unpack_vfm> sptr;
      
      virtual void activate_debug_mode(bool ena) = 0;
      virtual std::vector<int> cfi_results() = 0;
      
      /*!
       * \brief Return a shared_ptr to a new instance of lte::pcfich_unpack_vfm.
       *
       * To avoid accidental use of raw pointers, lte::pcfich_unpack_vfm's
       * constructor is in a private implementation
       * class. lte::pcfich_unpack_vfm::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string key, std::string msg_buf_name, std::string name = "pcfich_unpack_vfm");
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PCFICH_UNPACK_VFM_H */


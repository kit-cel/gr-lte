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


#ifndef INCLUDED_LTE_CRC_CHECK_VBVB_H
#define INCLUDED_LTE_CRC_CHECK_VBVB_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief Calculate CRC and check if it is equal to received CRC
     * \ingroup lte
     *
     */
    class LTE_API crc_check_vbvb : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<crc_check_vbvb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::crc_check_vbvb.
       *
       * To avoid accidental use of raw pointers, lte::crc_check_vbvb's
       * constructor is in a private implementation
       * class. lte::crc_check_vbvb::make is the public interface for
       * creating new instances.
       */
      static sptr make(int data_len, int final_xor, std::string name = "crc_check_vbvb");
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_CRC_CHECK_VBVB_H */


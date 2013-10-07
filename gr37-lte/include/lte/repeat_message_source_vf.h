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


#ifndef INCLUDED_LTE_REPEAT_MESSAGE_SOURCE_VF_H
#define INCLUDED_LTE_REPEAT_MESSAGE_SOURCE_VF_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief Repeat Vector in message infinitely
     * \ingroup lte
     *
     */
    class LTE_API repeat_message_source_vf : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<repeat_message_source_vf> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::repeat_message_source_vf.
       *
       * To avoid accidental use of raw pointers, lte::repeat_message_source_vf's
       * constructor is in a private implementation
       * class. lte::repeat_message_source_vf::make is the public interface for
       * creating new instances.
       */
      static sptr make(int vector_len);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_REPEAT_MESSAGE_SOURCE_VF_H */


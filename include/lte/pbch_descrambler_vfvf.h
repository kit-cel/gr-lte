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


#ifndef INCLUDED_LTE_PBCH_DESCRAMBLER_VFVF_H
#define INCLUDED_LTE_PBCH_DESCRAMBLER_VFVF_H

#include <lte/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace lte {

    /*!
     * \brief Block performs descrambling of PBCH with the given Cell ID
     * \ingroup lte
     * Cell ID is passed to block via message port on runtime
     */
    class LTE_API pbch_descrambler_vfvf : virtual public gr::sync_interpolator
    {
     public:
      typedef boost::shared_ptr<pbch_descrambler_vfvf> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::pbch_descrambler_vfvf.
       *
       * To avoid accidental use of raw pointers, lte::pbch_descrambler_vfvf's
       * constructor is in a private implementation
       * class. lte::pbch_descrambler_vfvf::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string key);
      
      virtual void set_cell_id(int id) = 0;
      
      virtual std::vector<int> pn_sequence() const = 0;
      
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PBCH_DESCRAMBLER_VFVF_H */


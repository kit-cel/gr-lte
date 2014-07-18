/* -*- c++ -*- */
/*
 * Copyright 2014 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

#ifndef INCLUDED_LTE_MIMO_CHANNEL_ESTIMATOR_H
#define INCLUDED_LTE_MIMO_CHANNEL_ESTIMATOR_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief <+description of block+>
     * \ingroup lte
     *
     */
    class LTE_API mimo_channel_estimator : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<mimo_channel_estimator> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::mimo_channel_estimator.
       *
       * To avoid accidental use of raw pointers, lte::mimo_channel_estimator's
       * constructor is in a private implementation
       * class. lte::mimo_channel_estimator::make is the public interface for
       * creating new instances.
       */
      static sptr make(int rxant, int subcarriers,
                    std::string tag_key,
                    std::string msg_buf_name,
                    const std::vector<std::vector<int> > &pilot_carriers,
                    const std::vector<std::vector<gr_complex> > &pilot_symbols);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_CHANNEL_ESTIMATOR_H */


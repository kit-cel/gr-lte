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


#ifndef INCLUDED_LTE_CHANNEL_ESTIMATOR_VCVC_H
#define INCLUDED_LTE_CHANNEL_ESTIMATOR_VCVC_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief Channel Estimator for OFDM
     * \ingroup lte
     * \param subcarriers A value to determine the vector size and number of used subcarriers
	 * \param tag_key A string value to set the name of the OFDM symbol number tag
	 * \param msg_buf_name Name of the in message port to reset pilot carriers and pilot symbol values
	 * \param pilot_carriers A vector of vectors of indices.
	 *                      first vector has length max OFDM symbols per frame.
	 *                      second vector contains indices of pilot carriers
	 * \param pilot_symbols A vector of vectors with pilot symbol values
	 *                      same as pilot_carriers but complex values.
     *
     */
    class LTE_API channel_estimator_vcvc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<channel_estimator_vcvc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::channel_estimator_vcvc.
       *
       * To avoid accidental use of raw pointers, lte::channel_estimator_vcvc's
       * constructor is in a private implementation
       * class. lte::channel_estimator_vcvc::make is the public interface for
       * creating new instances.
       */
      static sptr make(int subcarriers,
						std::string tag_key,
						std::string msg_buf_name,
						const std::vector<std::vector<int> > &pilot_carriers,
						const std::vector<std::vector<gr_complex> > &pilot_symbols,
                        std::string name = "channel_estimator_vcvc");
	
	  virtual void set_pilot_map(const std::vector<std::vector<int> > &pilot_carriers,
                       const std::vector<std::vector<gr_complex> > &pilot_symbols) = 0;

      virtual std::vector<std::vector<int> > get_pilot_carriers() = 0;
	  
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_CHANNEL_ESTIMATOR_VCVC_H */


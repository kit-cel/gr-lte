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


#ifndef INCLUDED_LTE_PRE_DECODER_VCVC_H
#define INCLUDED_LTE_PRE_DECODER_VCVC_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lte {

    /*!
     * \brief Pre Decoder for LTE
     * \ingroup lte
     * \param N_ant initial antenna setup. for now: 1 or 2 antennas
     * \param vlen length of the input/output vector
     * \param style decoding style as given by LTE standard. Only transmit diversity is supported
     * Block takes in vectors and channel estimates. Output is decoded according to Alamouti or standard zero-forcing depending on antenna configuration
     *
     */
    class LTE_API pre_decoder_vcvc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<pre_decoder_vcvc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::pre_decoder_vcvc.
       *
       * To avoid accidental use of raw pointers, lte::pre_decoder_vcvc's
       * constructor is in a private implementation
       * class. lte::pre_decoder_vcvc::make is the public interface for
       * creating new instances.
       */
      static sptr make(int N_ant, int vlen, std::string style);
      
      virtual void set_N_ant(int N_ant) = 0;
	  virtual int get_N_ant() = 0;
	  virtual void set_decoding_style(std::string style) = 0;
	  virtual std::string get_decoding_style() = 0;
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PRE_DECODER_VCVC_H */


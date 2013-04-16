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


#ifndef INCLUDED_LTE_CHANNEL_ESTIMATOR_VCVC_H
#define INCLUDED_LTE_CHANNEL_ESTIMATOR_VCVC_H

#include <lte_api.h>
#include <gr_sync_block.h>

#include <fftw3.h>
#include <volk/volk.h>
#include <cmath>
#include <cstdio>
#include <string>

//class LTE_API ofdm_pilot
//{
//    public:
//        int sym_num;
//        int carrier_num;
//        gr_complex value;
//};

class lte_channel_estimator_vcvc;

typedef boost::shared_ptr<lte_channel_estimator_vcvc> lte_channel_estimator_vcvc_sptr;

LTE_API lte_channel_estimator_vcvc_sptr lte_make_channel_estimator_vcvc (int subcarriers,
                                                                        int N_ofdm_symbols,
                                                                        std::string tag_key,
                                                                        std::string msg_buf_name,
                                                                        const std::vector<std::vector<int> > &pilot_carriers,
                                                                        const std::vector<std::vector<gr_complex> > &pilot_symbols);

/*!
 * \brief Channel Estimator for OFDM
 * \ingroup lte
 *
 */
class LTE_API lte_channel_estimator_vcvc : public gr_sync_block
{
 private:
	friend LTE_API lte_channel_estimator_vcvc_sptr lte_make_channel_estimator_vcvc (int subcarriers,
                                                                                 int N_ofdm_symbols,
                                                                                 std::string tag_key,
                                                                                 std::string msg_buf_name,
                                                                                 const std::vector<std::vector<int> > &pilot_carriers,
                                                                                 const std::vector<std::vector<gr_complex> > &pilot_symbols);

	lte_channel_estimator_vcvc(int subcarriers,
                            int N_ofdm_symbols,
                            std::string tag_key,
                            std::string msg_buf_name,
                            const std::vector<std::vector<int> > &pilot_carriers,
                            const std::vector<std::vector<gr_complex> > &pilot_symbols);

    static const gr_complex d_C_I;
    int d_subcarriers;
    int d_N_ofdm_symbols;
    int d_sym_num;
    int d_work_call;
    pmt::pmt_t d_key;



 public:
  ~lte_channel_estimator_vcvc();

	// Where all the action really happens
	int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

    void set_pilot_map(const std::vector<std::vector<int> > &pilot_carriers,
                    const std::vector<std::vector<gr_complex> > &pilot_symbols);
};

#endif /* INCLUDED_LTE_CHANNEL_ESTIMATOR_VCVC_H */


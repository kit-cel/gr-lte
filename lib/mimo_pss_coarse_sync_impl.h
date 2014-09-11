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

#ifndef INCLUDED_LTE_mimo_pss_coarse_sync_IMPL_H
#define INCLUDED_LTE_mimo_pss_coarse_sync_IMPL_H

#include <lte/mimo_pss_coarse_sync.h>
#include <gnuradio/filter/fir_filter.h>

namespace gr
{
namespace lte
{

class mimo_pss_coarse_sync_impl : public mimo_pss_coarse_sync
{
private:
    static const int d_CORRL=64;
    static const int d_TIME_HYPO=d_CORRL*75;


    int d_syncl;
    int d_rxant;
    int d_N_id_2;
    int d_work_call;
    int d_posmax;
    float d_max;

    //filter::kernel::fir_filter_ccf *d_fir;

    std::vector< gr_complex* > d_buffer;

    pmt::pmt_t d_port_coarse_pos;
    pmt::pmt_t d_port_N_id_2;
    pmt::pmt_t d_port_control;

    gr_complex d_pss0_t[d_CORRL];
    gr_complex d_pss1_t[d_CORRL];
    gr_complex d_pss2_t[d_CORRL];
    gr_complex d_pss012_t[d_CORRL];
    gr_complex* d_a;
    float d_result[d_TIME_HYPO];

    void prepare_corr_vecs();

    int calc_N_id_2(std::vector< gr_complex* > &buffer, int &mpos);
    float diff_corr(const gr_complex* x,const gr_complex* y, int len);


public:
    mimo_pss_coarse_sync_impl(int syncl, int rxant);
    ~mimo_pss_coarse_sync_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);
};

} // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_mimo_pss_coarse_sync_IMPL_H */


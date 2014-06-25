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


#ifndef INCLUDED_LTE_MIMO_PSS_FINE_SYNC_IMPL_H
#define INCLUDED_LTE_MIMO_PSS_FINE_SYNC_IMPL_H

#include <lte/mimo_pss_fine_sync.h>
#include <lte/mimo_pss_helper.h>

namespace gr
{
namespace lte
{

class mimo_pss_fine_sync_impl : public mimo_pss_fine_sync
{
private:
    static const gr_complex d_C_I;
    static const float d_PI;
    int d_N_id_2;
    int d_coarse_pss;
    int d_fine_pss;
    int d_fftl;
    int d_cpl;
    int d_cpl0;
    int d_slotl;
    int d_decim;
    int d_fine_count;
    long d_half_frame_start;
    float d_corr_val;
    bool d_is_locked;

    gr_complex* d_pssX_t;

    gr_complex* d_a;

    void zc(gr_complex *zc, int cell_id);
    void gen_pss_t(gr_complex *pss_t, int cell_id, int len);
    float diff_corr(const gr_complex* x,const gr_complex* y, int len);
    float diff_corr2(const gr_complex* x1, const gr_complex* x2, const gr_complex* y, int len);

public:
    mimo_pss_fine_sync_impl(int fftl);
    ~mimo_pss_fine_sync_impl();

    void handle_msg_N_id_2(pmt::pmt_t msg);
    void handle_msg_coarse_pss(pmt::pmt_t msg);

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);


};

} // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_PSS_FINE_SYNC_IMPL_H */


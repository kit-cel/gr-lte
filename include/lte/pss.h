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


#ifndef INCLUDED_LTE_PSS_H
#define INCLUDED_LTE_PSS_H

#include <lte/api.h>
#include <cmath>
#include <fftw3.h>
#include <cstring>

namespace gr
{
namespace lte
{

/*!
 * \brief helper class for pss blocks
 *
 */
class LTE_API pss
{
public:
    pss();
    ~pss();
    static void gen_pss_t(gr_complex *zc_t, int cell_id, int len);
    static void gen_conj_pss_t(gr_complex *zc_t, int cell_id, int len);
    static void zc(gr_complex *zc, int cell_id);

private:
    static const gr_complex d_C_I;
    static const float d_PI;

};

} // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_PSS_H */


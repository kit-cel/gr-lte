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


#ifndef INCLUDED_LTE_MIMO_PSS_HELPER_H
#define INCLUDED_LTE_MIMO_PSS_HELPER_H

#include <lte/api.h>

#include <gnuradio/gr_complex.h> 
#include <cstdio>
#include <fftw3.h>
#include <cstring>

namespace gr {
  namespace lte {

    /*!
     * \brief provides functions to use for pss sync
     *
     */
    class LTE_API mimo_pss_helper
    {
    public:
       
      mimo_pss_helper(); 
      ~mimo_pss_helper();  


      static void zc(gr_complex *zc, int cell_id);	
      static void gen_pss_t(gr_complex *pss_t, int cell_id, int len);
      static float diff_corr(const gr_complex* x,const gr_complex* y, int len);
      static float diff_corr2(const gr_complex* x1, const gr_complex* x2, const gr_complex* y, int len);
       

    private:  
        static gr_complex d_a[4];   
	static const gr_complex d_C_I;   
	static const float d_PI;   
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_PSS_HELPER_H */


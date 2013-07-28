/* -*- c++ -*- */
/*
 * Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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

#ifndef INCLUDED_LTE_LAYER_DEMAPPER_VCVC_H
#define INCLUDED_LTE_LAYER_DEMAPPER_VCVC_H

#include <lte_api.h>
#include <gr_sync_block.h>
//#include <string>

class lte_layer_demapper_vcvc;
typedef boost::shared_ptr<lte_layer_demapper_vcvc> lte_layer_demapper_vcvc_sptr;

LTE_API lte_layer_demapper_vcvc_sptr lte_make_layer_demapper_vcvc (int N_ant, int vlen, std::string style);

/*!
 * \brief Layer Demapper
 * \param N_ant number of antenna ports used in this system. Reconfigurable during runtime via message port
 * \param vlen vector length of in and output
 * \param style decoding style as given by LTE standard
 * This block performs layer demapping as given by the ETSI 136211 document.
 *
 */
class LTE_API lte_layer_demapper_vcvc : public gr_sync_block
{
	friend LTE_API lte_layer_demapper_vcvc_sptr lte_make_layer_demapper_vcvc (int N_ant, int vlen, std::string style);

	lte_layer_demapper_vcvc (int N_ant, int vlen, std::string style);

 public:
	~lte_layer_demapper_vcvc ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

	void set_N_ant(int N_ant);
	int get_N_ant(){return d_N_ant;}
	void set_decoding_style(std::string style);
	std::string get_decoding_style(){return d_style;}


 private:
    int d_N_ant;
    int d_vlen;
    std::string d_style;

    inline void demap_1_ant(gr_complex* out, const gr_complex * in, int len);
    inline void demap_2_ant(gr_complex* out, const gr_complex * in, int len);
    inline void demap_4_ant(gr_complex* out, const gr_complex * in, int len);
};

#endif /* INCLUDED_LTE_LAYER_DEMAPPER_VCVC_H */


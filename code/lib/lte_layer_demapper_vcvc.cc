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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <lte_layer_demapper_vcvc.h>
#include <cstdio>


lte_layer_demapper_vcvc_sptr
lte_make_layer_demapper_vcvc (int N_ant, int vlen, std::string style)
{
	return lte_layer_demapper_vcvc_sptr (new lte_layer_demapper_vcvc ( N_ant, vlen, style));
}


lte_layer_demapper_vcvc::lte_layer_demapper_vcvc (int N_ant, int vlen, std::string style)
	: gr_sync_block ("layer_demapper_vcvc",
		gr_make_io_signature (1,1, sizeof (gr_complex)*vlen ),
		gr_make_io_signature (1,1, sizeof (gr_complex)*vlen )),
		d_N_ant(0),
		d_vlen(vlen),
		d_style("null")
{
    set_N_ant(N_ant);
    set_decoding_style(style);

    pmt::pmt_t msg_buf = pmt::mp("N_ant");
    message_port_register_in(msg_buf);
    set_msg_handler(msg_buf, boost::bind(&lte_layer_demapper_vcvc::handle_msg, this, _1));
}


lte_layer_demapper_vcvc::~lte_layer_demapper_vcvc ()
{
}


int
lte_layer_demapper_vcvc::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

    for(int i = 0 ; i < noutput_items; i++){
        if(d_N_ant == 1){
            demap_1_ant(out, in, d_vlen);
        }
        else if(d_N_ant == 2){
            demap_2_ant(out, in, d_vlen);
        }
        else if(d_N_ant == 4){
            demap_4_ant(out, in, d_vlen);
        }
        in+=d_vlen;
        out+=d_vlen;
    }

	return noutput_items;
}

inline void
lte_layer_demapper_vcvc::demap_1_ant(gr_complex* out, const gr_complex * in, int len)
{
    memcpy(out, in, len*sizeof(gr_complex) );
}

inline void
lte_layer_demapper_vcvc::demap_2_ant(gr_complex* out, const gr_complex * in, int len)
{
    int len2 = len/2;
    for(int i = 0 ; i < len2 ; i++ ){
        *(out+2*i+0) = in[i    ];
        *(out+2*i+1) = in[i+len2];
    }
}

inline void
lte_layer_demapper_vcvc::demap_4_ant(gr_complex* out, const gr_complex * in, int len)
{
    int len4 = len/4;

    for(int i = 0 ; i < len4 ; i++ ){
        *(out+4*i+0) = in[i    ];
        *(out+4*i+1) = in[i+len4 ];
        *(out+4*i+2) = in[i+2*len4];
        *(out+4*i+3) = in[i+3*len4];
    }
}

void
lte_layer_demapper_vcvc::set_N_ant(int N_ant)
{
    if(N_ant != 1 && N_ant != 2 && N_ant != 4){
        printf("%s\t N_ant = %i is INVALID!\n", name().c_str(), N_ant);
    }
    else{
        printf("%s\tset N_ant to %i\n",name().c_str(), N_ant);
        d_N_ant = N_ant;
    }
}

void
lte_layer_demapper_vcvc::handle_msg(pmt::pmt_t msg)
{
    pmt::pmt_t cdr = pmt::pmt_cdr(msg);
    set_N_ant(int( pmt::pmt_to_long(cdr) ));
}

void
lte_layer_demapper_vcvc::set_decoding_style(std::string style)
{
    if(style != "tx_diversity"){
        if (style == "spatial_multiplexing"){
            printf("\"%s\" decoding style is valid but not supported\n", style.c_str() );
        }
        else{
            printf("\"%s\" decoding style is invalid\n", style.c_str() );
        }
    }
    else{
        printf("%s\tset decoding style to \"%s\"\n", name().c_str(), style.c_str() );
        d_style = style;
    }
}

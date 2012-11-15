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
#include <lte_vector_resize_vbvb.h>
#include <cstdio>


lte_vector_resize_vbvb_sptr
lte_make_vector_resize_vbvb (std::vector<int> map, int in_size, int out_size)
{
	return lte_vector_resize_vbvb_sptr (new lte_vector_resize_vbvb (map,in_size,out_size));
}


lte_vector_resize_vbvb::lte_vector_resize_vbvb (std::vector<int> map, int in_size, int out_size)
	: gr_sync_block ("vector_resize_vbvb",
		gr_make_io_signature (1,1, sizeof (char)*in_size ),
		gr_make_io_signature (1,1, sizeof (char)*out_size)),
		d_map(map),
		d_in_size(in_size),
		d_out_size(out_size)
{
}


lte_vector_resize_vbvb::~lte_vector_resize_vbvb ()
{
}


int
lte_vector_resize_vbvb::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
	const char *in = (const char *) input_items[0];
	char *out = (char *) output_items[0];

	// Do <+signal processing+>
	
	for (int i = 0 ; i < d_map.size(); i++){
	    
	    out[i] = in[d_map[i]];
	    //printf("cpp %i:\t out=%i\t in(map)=%i\t map=%i\n",i, out[i],in[d_map[i]], d_map[i]);
	}
	
	
	/*
	std::vector <gr_tag_t> v;
	get_tags_in_range(v,0,nitems_written(0),nitems_written(0)+1);
	if (v.size() > 0){
		uint64_t offset = v[0].offset;
		std::string key = pmt::pmt_symbol_to_string(v[0].key);
		long value = pmt::pmt_to_long(v[0].value);
		std::string srcid = pmt::pmt_symbol_to_string(v[0].srcid);
        printf("%s\t%i %i\t",name().c_str(),d_in_size,d_out_size);
		printf("v.size() = %i,\tnitems_written(0) = %i\tkey = %s\tsrcid = %s\tvalue = %ld\n", int(v.size()), int(nitems_written(0)), key.c_str(), srcid.c_str(), value);
	}
	*/
	
	
	noutput_items = 1;

	// Tell runtime system how many output items we produced.
	return noutput_items;
}


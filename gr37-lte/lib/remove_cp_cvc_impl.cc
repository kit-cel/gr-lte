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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "remove_cp_cvc_impl.h"

#include <cstdio>

namespace gr {
  namespace lte {

    remove_cp_cvc::sptr
    remove_cp_cvc::make(int fftl, std::string key)
    {
      return gnuradio::get_initial_sptr
        (new remove_cp_cvc_impl(fftl, key));
    }

    /*
     * The private constructor
     */
    remove_cp_cvc_impl::remove_cp_cvc_impl(int fftl, std::string key)
      : gr::block("remove_cp_cvc",
              gr::io_signature::make( 1, 1, sizeof(gr_complex)),
              gr::io_signature::make( 1, 1, sizeof(gr_complex) * fftl)),
              d_fftl(fftl),
			  d_cpl((144*fftl)/2048),
			  d_cpl0((160*fftl)/2048),
			  d_slotl(7*fftl+6*d_cpl+d_cpl0),
			  d_symb(0),
			  d_sym_num(0),
			  d_work_call(0),
			  d_found_frame_start(false),
			  d_frame_start(0),
              d_symbols_per_frame(140)
    {
		d_key=pmt::string_to_symbol(key);
		d_tag_id=pmt::string_to_symbol(name() );
		set_tag_propagation_policy(TPP_DONT);
	}

    /*
     * Our virtual destructor.
     */
    remove_cp_cvc_impl::~remove_cp_cvc_impl()
    {
    }

    void
    remove_cp_cvc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        unsigned ninputs = ninput_items_required.size();
		for (unsigned i = 0; i < ninputs; i++)
			ninput_items_required[i] = ( d_fftl + d_cpl0 ) * noutput_items;
    }

    int
    remove_cp_cvc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
		gr_complex *out = (gr_complex *) output_items[0];

		d_work_call++;

		// the following section removes the samples before the first frame start.
		std::vector <gr::tag_t> v;
		get_tags_in_range(v, 0, nitems_read(0), nitems_read(0)+ninput_items[0], d_key );

        if(!d_found_frame_start){
            if(v.size() == 0){
                consume_each(ninput_items[0]);
                return 0;
            }
            d_frame_start = get_frame_start(v);
            printf("first frame_start = %ld\n", d_frame_start);
            
            long framel = (d_symbols_per_frame / 7) * d_slotl;
            long consume = (nitems_read(0) + framel - d_frame_start) % framel;
            consume_each(consume);
            //~ d_frame_start = frame_start;
            d_found_frame_start = true;
            return 0;
        }

        d_frame_start = get_frame_start(v);
        sym_info info = get_sym_num_info(d_frame_start, nitems_read(0), d_symbols_per_frame );
        //~ printf("%i: left = %i\n", info.num % 7, info.dump);
        d_sym_num = info.num;
        
        if(info.dump != 0){ // align in buffer to symbols for next call
            printf("work_call = %i\tnot aligned, consum %i\n", d_work_call, info.dump);
            consume_each(info.dump);
            return 0;
        }

		// Copy the samples of interest from input to output buffer
		long consumed_items = copy_samples_from_in_to_out(out, in, noutput_items);

		// add item tags. Item tags for each vector/OFDM symbol.
		d_sym_num = add_tags_to_vectors(noutput_items, d_sym_num, d_symbols_per_frame);

		// Tell runtime system how many input items we consumed on
		// each input stream.
		consume_each (consumed_items);
		// Tell runtime system how many output items we produced.
		return noutput_items;
    }
    
    long
    remove_cp_cvc_impl::get_frame_start(std::vector <gr::tag_t> v)
    {
		long frame_start = d_frame_start;
		if(v.size() > 0){
			gr::tag_t tag = v.back();
			long value = pmt::to_long(tag.value);
			int slots = value; //value / 7;
			int items = slots * d_slotl;
			if(value % 7 != 0){
				items += d_cpl0 + (value % 7) * (d_cpl + d_fftl);
			}
			frame_start = (tag.offset + 20 * d_slotl - items) % (20 * d_slotl);
		}
		return frame_start;
	}
	
	sym_info
	remove_cp_cvc_impl::get_sym_num_info(long frame_start, long nitems_read, int symbols_per_frame )
	{
		sym_info info;
        int spf = symbols_per_frame / 7;
        int frame_items = (nitems_read + spf * d_slotl - frame_start) % (spf * d_slotl); 
		int slots = frame_items / d_slotl;
		int syms_in_slot = (frame_items % d_slotl) / (d_fftl + d_cpl);
        info.num = slots * 7 + syms_in_slot;
        info.dump = leading_items_to_dump(frame_items % d_slotl, info.num % 7);
        return info;
	}
    
    int
    remove_cp_cvc_impl::leading_items_to_dump(int slot_items, int slot_sym)
    {
        int dump = 0;
        if(slot_items == 0){ // is aligned!
            dump = 0;
        }
        else if(slot_items < d_fftl + d_cpl0){
            dump = d_fftl + d_cpl0 - slot_items;
        }
        else{
            int sym_items = (slot_items - d_fftl - d_cpl0) % (d_fftl + d_cpl);
            if(sym_items == 0){
                dump = 0;
            }
            else{
                dump = d_fftl + d_cpl - sym_items;
            }
        }

 
        //~ if(items != 0) {return items; }
        printf("%i: slot_items = %i\tdump = %i\n", slot_sym, slot_items, dump);
        return dump;
            
    }
    
    long
	remove_cp_cvc_impl::copy_samples_from_in_to_out(gr_complex* out, const gr_complex* in, int noutput_items)
	{
		long consumed_items = 0;
		int vector_byte_size = sizeof(gr_complex)*d_fftl;
		int syml0 = d_cpl0 + d_fftl;
		int syml1 = d_cpl + d_fftl;

		for (int i = 0 ; i < noutput_items ; i++){
            if(d_symb == 0){ // 0. symbol in each LTE slot is longer than the rest
				memcpy(out, in+d_cpl0, vector_byte_size);
				consumed_items += syml0;
				in += syml0;
			}
			else{
				memcpy(out, in+d_cpl,vector_byte_size);
				consumed_items += syml1;
				in += syml1;
			}
			out += d_fftl;
			d_symb =(d_symb+1)%7;
		}
		return consumed_items;
	}

	int
	remove_cp_cvc_impl::add_tags_to_vectors(int noutput_items, int sym_num, int symbols_per_frame)
	{
        for (int i = 0 ; i < noutput_items ; i++){
			if(d_sym_num%7 == 0){
                long offset = nitems_written(0) + i;
                //~ printf("offset = %ld\tsym_num = %i\n", offset, sym_num);
				add_item_tag(0, offset, d_key, pmt::from_long(sym_num), d_tag_id);
			}
			sym_num = (sym_num+1)%symbols_per_frame;
		}
        return sym_num;
	}

  } /* namespace lte */
} /* namespace gr */


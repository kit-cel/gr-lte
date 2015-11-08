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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "mimo_remove_cp_impl.h"

#include <cstdio>

namespace gr {
  namespace lte {

    mimo_remove_cp::sptr
    mimo_remove_cp::make(int fftl, int rxant, std::string key)
    {
      return gnuradio::get_initial_sptr(new mimo_remove_cp_impl(fftl, rxant, key));
    }

    /*
     * The private constructor
     */
    mimo_remove_cp_impl::mimo_remove_cp_impl(int fftl, int rxant, std::string key) :
            gr::block("mimo_remove_cp", gr::io_signature::make(1, 8, sizeof(gr_complex)),
                      gr::io_signature::make(1, 8, sizeof(gr_complex) * fftl)), d_fftl(fftl),
            d_rxant(rxant), d_cpl((144 * fftl) / 2048), d_cpl0((160 * fftl) / 2048),
            d_slotl(7 * fftl + 6 * d_cpl + d_cpl0), d_symb(0), d_sym_num(0), d_work_call(0),
            d_found_frame_start(false), d_half_frame_start(0), d_symbols_per_frame(140)
    {
      d_key = pmt::string_to_symbol(key);
      d_tag_id = pmt::string_to_symbol(this->name());
      set_tag_propagation_policy(TPP_DONT);
    }

    const int
    mimo_remove_cp_impl::find_smallest_ninput_items(gr_vector_int& ninput_item)
    {
      int n = ninput_item[0];
      for(int i = 0; i < ninput_item.size(); i++){
        n = std::min(n, ninput_item[i]);
      }
      return n;
    }

    const int
    mimo_remove_cp_impl::update_half_frame_start(const int ninput_items)
    {
      std::vector<gr::tag_t> v;
      get_tags_in_range(v, 0, nitems_read(0), nitems_read(0) + ninput_items);
      if(v.size() < 1){
        return 0;
      }
      const gr::tag_t tag = v.back();
      const long value = pmt::to_long(tag.value);

      // assume tags mark slots and only half_frame is detected. Thus value range [0,10).
      const long to_next_half_frame = (10 - value) * d_slotl;
      long half_frame_start = (tag.offset + to_next_half_frame) % (10 * d_slotl);
//      d_found_frame_start = true;
      int sync_offset = half_frame_start - d_half_frame_start;
      std::cout << "half_frame update=" << half_frame_start << ", sync_offset=" << sync_offset << std::endl;
      return sync_offset;
    }

    const int
    mimo_remove_cp_impl::calculate_item_offset(std::vector<gr::tag_t>& v)
    {
      const int halffl = 10 * d_slotl;
      const int size = v.size();
      int sync_delay = 0;

      for(int i = 0; i < size; i++){
        if(pmt::to_long(v[i].value) == 0){
          int half_frame_start = v[i].offset % halffl;
          if(half_frame_start != d_half_frame_start){

            if(half_frame_start == (d_half_frame_start - 1 + halffl) % halffl){
              sync_delay = -1;
            }
            else if(half_frame_start == (d_half_frame_start - 2 + halffl) % halffl){
              sync_delay = -2;
            }
            else if(half_frame_start == (d_half_frame_start + 1) % halffl){
              sync_delay = 1;
            }
            else if(half_frame_start == (d_half_frame_start + 2) % halffl){
              sync_delay = 2;
            }
            else{
              printf("%s OUT of sync!\n", name().c_str());
              d_found_frame_start = false;
              return 0;   //resync to frame start
            }
          }
          d_half_frame_start = half_frame_start;
        }
      }
      return sync_delay;
    }

    /*
     * Our virtual destructor.
     */
    mimo_remove_cp_impl::~mimo_remove_cp_impl()
    {
    }

    void
    mimo_remove_cp_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = (d_fftl + d_cpl0) * noutput_items + 4;
    }

    int
    mimo_remove_cp_impl::general_work(int noutput_items, gr_vector_int &ninput_items,
                                      gr_vector_const_void_star &input_items,
                                      gr_vector_void_star &output_items)
    {
      d_work_call++;

      const int smallest_ninput_items = find_smallest_ninput_items(ninput_items);
      update_half_frame_start(smallest_ninput_items);

      // Possible states: UNSYNCED, FOUND_HALF_FRAME_START, SYNCED.

      // the following section removes the samples before the first frame start.
      std::vector<gr::tag_t> v;
      get_tags_in_range(v, 0, nitems_read(0), nitems_read(0) + noutput_items * (d_fftl + d_cpl0));
      const int size = v.size();
      if(!d_found_frame_start){
        for(int i = 0; i < size; i++){
          long value = pmt::to_long(v[i].value);
          if(value == 0){
            //std::string key = pmt::pmt_symbol_to_string(v[0].key);
            //printf("%s found frame start offset = %li\trange = %i\tkey = %s\n",name().c_str(), v[0].offset,noutput_items*(fftl+cpl0),key.c_str() );
            int delay = int(v[i].offset - nitems_read(0));
            d_half_frame_start = (v[i].offset);
            printf("\n%s\n", name().c_str());
            //printf("nitems_read = %li\n",nitems_read(0) );
            //printf("delay       = %i\n",delay);
            //printf("a+b         = %li\n",nitems_read(0)+delay );
            printf("half frame_start = %ld\n", d_half_frame_start);
            d_half_frame_start = d_half_frame_start % (10 * d_slotl);
            printf("mod start   = %ld\n\n", d_half_frame_start);
            d_sym_num = 0;
            d_symb = 0;
            d_found_frame_start = true;
            consume_each(delay);
            return 0;
          }
          else{
            if(size > i + 1){
              continue;
            }
            else{
              consume_each(noutput_items * (d_fftl + d_cpl0));
              return 0;
            }
          }
        }

        if(size == 0){
          consume_each(noutput_items * (d_fftl + d_cpl0));
          return 0;
        }
      }

      //the following section syncs directly to new frame start if the delay is only 1 or 2 samples(tracking in time sync)
      int sync_delay = calculate_item_offset(v);

      // Copy the samples of interest from input to output buffer
      long consumed_items = copy_samples_from_in_to_out(output_items, input_items, noutput_items,
                                                        sync_delay);

      // add item tags. Item tags for each vector/OFDM symbol.
      add_tags_to_vectors(noutput_items);

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each(consumed_items + sync_delay);
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    long
    mimo_remove_cp_impl::copy_samples_from_in_to_out(gr_vector_void_star &output_items,
                                                     const gr_vector_const_void_star &input_items,
                                                     int noutput_items, int sync_delay)
    {
      const int vector_byte_size = sizeof(gr_complex) * d_fftl;
      long consumed_items = 0;
      int symb;

      for(int rx = 0; rx < d_rxant; rx++){
        symb = d_symb;
        gr_complex *out = (gr_complex*) output_items[rx];
        const gr_complex *in = (const gr_complex *) input_items[rx];
        consumed_items = 0;

        in += sync_delay;

        for(int i = 0; i < noutput_items; i++){
          const int cp_length = (symb == 0) ? d_cpl0 : d_cpl;
          memcpy(out, in + cp_length, vector_byte_size);
          consumed_items += d_fftl + cp_length;
          in += d_fftl + cp_length;
          out += d_fftl;
          symb = (symb + 1) % 7;
        }

      }
      d_symb = symb;
      return consumed_items;
    }

    void
    mimo_remove_cp_impl::add_tags_to_vectors(int noutput_items)
    {
      for(int i = 0; i < noutput_items; i++){
        if(d_sym_num % 7 == 0){
          add_item_tag(0, nitems_written(0) + i, d_key, pmt::from_long(d_sym_num), d_tag_id);
        }
        d_sym_num = (d_sym_num + 1) % 70;
      }
    }

  } /* namespace lte */
} /* namespace gr */


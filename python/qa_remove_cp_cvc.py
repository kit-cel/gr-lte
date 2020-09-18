#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2013 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest, blocks
import lte_swig as lte
import pmt
from . import lte_test

class qa_remove_cp_cvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

        # set variables
        self.fftl = fftl = 512
        self.key = key = "symbol"

        #set up fg
        intu = [0,]
        self.src = blocks.vector_source_c(intu, False, 1)
        self.rcp = lte.remove_cp_cvc(fftl, key)
        self.snk = blocks.vector_sink_c(fftl)

        self.tb.connect(self.src, self.rcp, self.snk)

        #self.dbg = blocks.tag_debug(gr.sizeof_gr_complex * fftl, "remove_cp_debug")
        #self.tb.connect(self.rcp, self.dbg)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        fftl = self.fftl
        key = self.key
        srcid = "src"
        frame_len = 70
        slots = 30
        cpl0 = 160*fftl/2048
        cpl1 = 144*fftl/2048
        slotl = 7 * fftl + 6*cpl1 + cpl0

        symvals = list(range(fftl)) #[1] * fftl
        data = []
        in_data = [0] * 1500
        items = len(in_data)
        tags = []
        for i in range(slots):
            value = (i * 7) % frame_len
            print(items, "\t", value)
            tags.append(lte_test.generate_tag(key, srcid, value, items))
            for sym in range(7):
                vec = []
                if sym == 0:
                    vec.extend([0]*cpl0)
                else:
                    vec.extend([0]*cpl1)
                vec.extend(symvals)
                in_data.extend(vec)
                data.extend(symvals)
                items += len(vec)


        tag_list = self.get_tag_list(frame_len, key, 140)

        # run fg with test data
        self.src.set_data(in_data, tags)
        self.tb.run ()

        # check data
        res = self.snk.data()
        print(len(res))

        # now really check if results is ok
        min_samps = min(len(res), len(data))
        self.assertFloatTuplesAlmostEqual(res[0:min_samps], data[0:min_samps])


    def get_tag_list(self, data_len, tag_key, N_ofdm_symbols):
        fftl = self.fftl
        slots = data_len / 7
        cpl0 = 160*fftl/2048
        cpl1 = 144*fftl/2048
        slotl = 7*fftl + 6*cpl1 + cpl0
        srcid = "test_src"
        tag_list = []
        for i in range(slots):
            for sym in range(7):
                offset = 0
                if sym == 0:
                    offset = i*slotl
                else:
                    offset = i*slotl + cpl0 + sym*fftl +(sym-1)*cpl1
                value = (i*7+sym)%N_ofdm_symbols
                tag = lte_test.generate_tag(tag_key, srcid, value, offset)
                tag_list.append(tag)
        return tag_list

    def print_tag(self, tag):
        my_string = "key = " + pmt.symbol_to_string(tag.key) + "\tsrcid = " + pmt.symbol_to_string(tag.srcid)
        my_string = my_string + "\tvalue = " + str(pmt.to_long(tag.value))
        my_string = my_string + "\toffset = " + str(tag.offset)
        print(my_string)


if __name__ == '__main__':
    gr_unittest.run(qa_remove_cp_cvc)

#!/usr/bin/env python
# 
# Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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
#

from gnuradio import gr, gr_unittest
from gruel import pmt
import lte as lte_swig
import numpy
import scipy.io
import time

class qa_remove_cp_cvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

        # set variables
        self.fftl = fftl = 512
        self.key = key = "symbol"

        #set up fg
        intu = [0,]
        self.src  = gr.vector_source_c(intu,False,1)
        self.rcp  = lte_swig.remove_cp_cvc(fftl, key)
        self.snk  = gr.vector_sink_c(fftl)
        
        self.tb.connect(self.src,self.rcp,self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        fftl = self.fftl
        key = self.key
        frame_len = 140
        slots = frame_len / 7
        #print slots
        cpl0 = 160*fftl/2048
        cpl1 = 144*fftl/2048
        slotl = 7*fftl + 6*cpl1 + cpl0
        #print cpl0
        #print cpl1
        #print slotl
        
        data = [1] * fftl * frame_len
        in_data = []
        for i in range(slots):
            for sym in range(7):
                if sym == 0:
                    in_data.extend([0]*cpl0)
                else:
                    in_data.extend([0]*cpl1)
                pos = i*slotl + sym * fftl
                in_data.extend(data[pos:pos+fftl])

        tag_list = self.get_tag_list(frame_len, key, 140)
        
        # run fg with test data           
        self.src.set_data(in_data, tag_list)
        self.tb.run ()
        
        # check data
        res = self.snk.data()
        self.assertFloatTuplesAlmostEqual(res[0:60000], data[0:60000])
        #print len(res)
        #print len(data)
        #print len(data)-len(res)

        

    def get_tag_list(self, data_len, tag_key, N_ofdm_symbols):
        fftl = self.fftl
        key = self.key
        frame_len = 140
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
                tag = self.generate_tag(tag_key, srcid, value, offset)
                tag_list.append(tag)
        return tag_list
    
    def generate_tag(self, tag_key, srcid, value, offset):
        tag = gr.gr_tag_t()
        tag.key = pmt.pmt_string_to_symbol(tag_key)
        tag.srcid = pmt.pmt_string_to_symbol(srcid)
        tag.value = pmt.pmt_from_long(value)
        tag.offset = offset
        return tag

    def print_tag(self, tag):
        my_string = "key = " + pmt.pmt_symbol_to_string(tag.key) + "\tsrcid = " + pmt.pmt_symbol_to_string(tag.srcid)
        my_string = my_string + "\tvalue = " + str(pmt.pmt_to_long(tag.value))
        my_string = my_string + "\toffset = " + str(tag.offset)
        print my_string


if __name__ == '__main__':
    gr_unittest.main ()

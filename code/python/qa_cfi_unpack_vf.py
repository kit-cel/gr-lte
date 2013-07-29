#!/usr/bin/env python
# 
# Copyright 2013 <+YOU OR YOUR COMPANY+>.
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
from gruel import pmt
import lte
from lte_test import *

class qa_cfi_unpack_vf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        key = "subframe"
        print "setup test"
        
        data = []
        for i in range(3):
            cfi = get_cfi_sequence(i+1)
            data.extend(nrz_encoding(cfi))
        
        taglist = self.get_tag_list(3, key, 10)
        
        data = [float(data[i]) for i in range(len(data))]
        
        self.src = blocks.vector_source_f(data, False, 32, taglist)
        self.cfi = lte.cfi_unpack_vf(key)
        self.dbg = blocks.message_debug()
        
        self.tb.connect(self.src, self.cfi)
        self.tb.msg_connect(self.cfi, "cfi", self.dbg, "print")
        print "setup finished"

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        print "run test"
        # set up fg
        self.tb.run ()
        # check data
        
    def get_tag_list(self, data_len, tag_key, N_ofdm_symbols):
        tag_list = []
        for i in range(data_len):
            tag = gr.gr_tag_t()
            tag.key = pmt.pmt_string_to_symbol(tag_key)
            tag.srcid = pmt.pmt_string_to_symbol("test_src")
            tag.value = pmt.pmt_from_long(i%N_ofdm_symbols)
            tag.offset = i
            tag_list.append(tag)
        return tag_list


if __name__ == '__main__':
    gr_unittest.run(qa_cfi_unpack_vf)

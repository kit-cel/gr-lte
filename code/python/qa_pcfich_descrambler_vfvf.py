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
import lte
from lte_test import *
import numpy as np

class qa_pcfich_descrambler_vfvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        tag_key = "subframe"
        msg_buf_name = "cell_id"
        vlen = 32
        
        data = [0]*vlen
        self.src = blocks.vector_source_f(data, False, vlen)
        self.descr = lte.pcfich_descrambler_vfvf(tag_key, msg_buf_name)
        
        self.snk  = blocks.vector_sink_f(vlen)
        
        self.tb.connect(self.src, self.descr, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        cell_id = 124        
        self.descr.set_cell_id(cell_id)
        #scr_mat = self.descr.get_descr_seqs()
        #print np.shape(scr.mat)
        data = scramble_cfi_sequence([0]*32, cell_id, 0)
        print data
        data = [1-2*data[i] for i in range(len(data))]
        self.src.set_data(data)
        # set up fg
        self.tb.run ()
        
        res = self.snk.data()
        
        for i in range(len(data)):
            print str(res[i]) + "\t" + str(data[i])
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_pcfich_descrambler_vfvf)

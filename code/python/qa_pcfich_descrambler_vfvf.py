#!/usr/bin/env python
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
import lte
from lte_test import *
import numpy as np

class qa_pcfich_descrambler_vfvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        self.tag_key = tag_key = "subframe"
        msg_buf_name = "cell_id"
        vlen = 32
        
        data = [0]*vlen
        self.src = blocks.vector_source_f(data, False, vlen)
        self.descr = lte.descrambler_vfvf(tag_key, msg_buf_name, 32)

 
#        self.descr = lte.pcfich_descrambler_vfvf(tag_key, msg_buf_name)
        self.snk  = blocks.vector_sink_f(vlen)
        
        self.tb.connect(self.src, self.descr, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        cell_id = 124
        
        seqs = []
        for i in range(10):
            seq = get_pcfich_scrambling_sequence(32, cell_id, 2*i)
            seqs.append(nrz_encoding(seq))
        print seqs
        self.descr.set_descr_seqs(seqs)
        
#        self.descr.set_cell_id(cell_id) # cell_id set because msg port not connected
        #scr_mat = self.descr.get_descr_seqs()
        
        exp_res = []
        data = []
        for cfi in range(3):
            cfi_seq = get_cfi_sequence(cfi+1)
            for ns in range(10):
                exp_res.extend(nrz_encoding(cfi_seq))
                scr_cfi_seq = scramble_cfi_sequence(cfi_seq, cell_id, 2*ns)
                data.extend(nrz_encoding(scr_cfi_seq))
        
        print np.shape(exp_res)
        print np.shape(data)
        
        taglist = get_tag_list(len(data)/32, self.tag_key, 10 )

        # set up fg
        self.src.set_data(data, taglist)
        self.tb.run ()
        
        # Check results
        res = self.snk.data()
        self.assertFloatTuplesAlmostEqual(res, exp_res)



if __name__ == '__main__':
    gr_unittest.run(qa_pcfich_descrambler_vfvf)

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

from gnuradio import gr, gr_unittest
import lte
import lte_test

class qa_decode_pbch_vcvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        self.pbch = lte.decode_pbch_vcvf()
        
        self.src0 = gr.vector_source_c([0]*240, False, 240)
        self.src1 = gr.vector_source_c([0]*240, False, 240)
        self.src2 = gr.vector_source_c([0]*240, False, 240)
        
        self.snk = gr.vector_sink_f(120)
        
        self.tb.connect(self.src0, (self.pbch, 0))
        self.tb.connect(self.src1, (self.pbch, 1))
        self.tb.connect(self.src2, (self.pbch, 2))
        self.tb.connect(self.pbch, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        cell_id = 124    
        N_ant = 2
        style= "tx_diversity"
        mib = lte_test.pack_mib(50,0,1.0, 511)
        bch = lte_test.encode_bch(mib, N_ant)
        pbch = lte_test.encode_pbch(bch, cell_id, N_ant, style)
        
        stream = [pbch[0][i]+pbch[1][i] for i in range(len(pbch[0]))]
        h0 = [complex(1,0)]*len(pbch[0])
        h1 = [complex(1,0)]*len(pbch[1])
        
        self.pbch.descr.set_cell_id(cell_id)
        self.src0.set_data(stream)
        self.src1.set_data(h0)
        self.src2.set_data(h1)

        self.tb.run ()
        
        # check data        
        res = self.snk.data()
        
        assert(len(res) > 0 )



if __name__ == '__main__':
    gr_unittest.run(qa_decode_pbch_vcvf, "qa_decode_pbch_vcvf.xml")

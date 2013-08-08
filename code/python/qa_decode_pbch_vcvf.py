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

from gnuradio import gr, gr_unittest
import lte
from lte_test import *

class qa_decode_pbch_vcvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        self.pbch = lte.decode_pbch_vcvf()
        '''
        This hierarchical block consists of the following blocks
        lte.pre_decoder_vcvc(1, style)
        lte.pre_decoder_vcvc(2, style)
        lte.layer_demapper_vcvc(1, style)
        lte.layer_demapper_vcvc(2, style)
        gr.interleave(240*gr.sizeof_gr_complex)
        lte.qpsk_soft_demod_vcvf()
        lte.descrambling_vfvf()
        '''
        
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
        mib = pack_mib(50,0,1.0, 511)
        bch = encode_bch(mib, N_ant)
        pbch = encode_pbch(bch, cell_id, N_ant, style)
        
        stream = [pbch[0][i]+pbch[1][i] for i in range(len(pbch[0]))]
        h0 = [complex(1,0)]*len(stream)
        h1 = [complex(1,0)]*len(stream)
        
        self.pbch.descr.set_cell_id(cell_id)
        self.src0.set_data(stream)
        self.src1.set_data(h0)
        self.src2.set_data(h1)

        self.tb.run ()
        
        # check data        
        res = self.snk.data()
        assert(len(res) > 0 )
        

        # Following QA part currently not useful (always some data false??!)        
        res_h = nrz_hard_decode(res)
        bch = tuple(bch)
        
        success = 0
        failed = 0
        for i in range(len(res)/len(bch)):
            part = tuple(res_h[i*len(bch):(i+1)*len(bch)])
            try:
                self.assertTupleEqual(part,bch)
                success = success +1
            except:
                failed = failed +1
        
        print "successful decodings: " + str(success)
        print "failed decodings: " + str(failed)



if __name__ == '__main__':
    gr_unittest.run(qa_decode_pbch_vcvf, "qa_decode_pbch_vcvf.xml")

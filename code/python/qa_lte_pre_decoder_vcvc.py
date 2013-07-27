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
import lte as lte_swig
from lte_test import *
import scipy.io
import math

class qa_pre_decoder_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        N_ant = 2
        vlen = 240
        style = 'tx_diversity'
        
        intu1 = [0]*vlen
        intu2 = [0]*vlen
        intu3 = [0]*vlen
                     
        self.src1 = gr.vector_source_c( intu1, False, vlen)
        self.src2 = gr.vector_source_c( intu2, False, vlen)
        self.src3 = gr.vector_source_c( intu3, False, vlen)
        
        self.pd = lte_swig.pre_decoder_vcvc(N_ant, vlen, style)
        
        self.snk = gr.vector_sink_c(vlen)
        
        self.tb.connect(self.src1,(self.pd,0) )
        self.tb.connect(self.src2,(self.pd,1) )
        self.tb.connect(self.src3,(self.pd,2) )
        self.tb.connect(self.pd,self.snk)

    def tearDown (self):
        self.tb = None
      
    def test_001_generated(self):
        print "\ntest_001_generated"
        cell_id = 124        
        N_ant = 2
        style= "tx_diversity"
        mib = pack_mib(50,0,1.0, 511)
        bch = encode_bch(mib, N_ant)
        
        scrambled = pbch_scrambling(bch, cell_id)
        qpsk_modulated = qpsk_modulation(scrambled)
        layer_mapped = layer_mapping(qpsk_modulated, N_ant, style)
        pre_coded = pre_coding(layer_mapped, N_ant, style)

        h0 = [complex(1,0)]*len(pre_coded[0])
        h1 = [complex(1,0)]*len(pre_coded[1])
        stream = [pre_coded[0][i]+pre_coded[1][i] for i in range(len(pre_coded[0]))]
        
        self.src1.set_data(stream)
        self.src2.set_data(h0)
        self.src3.set_data(h1)
        self.tb.run()        
        res = self.snk.data()
        
        exp_res = []
        for i in range(len(layer_mapped[0])):
            exp_res.append(layer_mapped[0][i])
            exp_res.append(layer_mapped[1][i])
        print self.assertComplexTuplesAlmostEqual(res, exp_res)
        
    def test_002_pcfich(self):
        print "test_002_pcfich"
        
        # some constants
        cell_id = 124
        N_ant = 2
        style= "tx_diversity"
        vlen = 16
        ns = 0
        
        # new top_block because even the interface changes
        self.tb2 = gr.top_block()
        
        # generate test data together with the expected output
        data = []
        exp_res = []
        for cfi in range(4):
            cfi_seq = get_cfi_sequence(cfi+1)
            scr_cfi_seq = scramble_cfi_sequence(cfi_seq, cell_id, ns)
            mod_cfi_seq = qpsk_modulation(scr_cfi_seq)
            lay_cfi_seq = layer_mapping(mod_cfi_seq, N_ant, style)
            for i in range(len(lay_cfi_seq[0])):
                exp_res.append(lay_cfi_seq[0][i])
                exp_res.append(lay_cfi_seq[1][i])
            pc_cfi_seq = pre_coding(lay_cfi_seq, N_ant, style)
            pc_cfi_seq = [pc_cfi_seq[0][i]+pc_cfi_seq[1][i] for i in range(len(pc_cfi_seq[0]))]
            data.extend(pc_cfi_seq)

        # dummy channel estimates        
        intu2 = [1]*len(data)
        intu3 = [1]*len(data)
        
        N_ant = 1        
        # get blocks
        self.src1 = gr.vector_source_c( data, False, vlen)
        self.src2 = gr.vector_source_c( intu2, False, vlen)
        self.src3 = gr.vector_source_c( intu3, False, vlen)
        self.pd = lte_swig.pre_decoder_vcvc(N_ant, vlen, style)
        self.snk = gr.vector_sink_c(vlen)
        
        # connect all blocks
        self.tb2.connect(self.src1,(self.pd,0) )
        self.tb2.connect(self.src2,(self.pd,1) )
        self.tb2.connect(self.src3,(self.pd,2) )
        self.tb2.connect(self.pd,self.snk)
        
        self.pd.set_N_ant(2)
        
        # run flowgraph
        self.tb2.run()
        
        # compare result with expected result
        res = self.snk.data()
        self.assertComplexTuplesAlmostEqual(res,exp_res)
        
            
        
        
        

        

        

if __name__ == '__main__':
    gr_unittest.main ()

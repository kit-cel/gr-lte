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
        
        intu1 = [0]*240
        intu2 = [0]*240
        intu3 = [0]*240
                     
        self.src1 = gr.vector_source_c( intu1, False, 240)
        self.src2 = gr.vector_source_c( intu2, False, 240)
        self.src3 = gr.vector_source_c( intu3, False, 240)
        
        self.pd = lte_swig.pre_decoder_vcvc(2,'tx_diversity')
        
        self.snk = gr.vector_sink_c(240)
        
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
        

        

        

if __name__ == '__main__':
    gr_unittest.main ()

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
#

from gnuradio import gr, gr_unittest
import lte as lte_swig
import random, math, cmath
from lte_test import *
import numpy as np

class qa_channel_estimator (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        self.N_rb_dl = N_rb_dl = 6
        
        # For now, generate a pseudo-random sequence and do QPSK modulation.
        values = range(140*12*N_rb_dl)
        randlist = range(2*len(values))
        for i in randlist:
            randlist[i] = 1-2*random.randint(0,1) 
        for i in values:
            values[i] = complex(randlist[2*i]/math.sqrt(2), randlist[2*i+1]/math.sqrt(2))
            #print values[i]
            
        #print "len(input) = " + str(len(values))
        self.src = gr.vector_source_c(values,False,12*N_rb_dl)
        
        self.snk0 = gr.vector_sink_c(12*N_rb_dl)
        self.snk1 = gr.vector_sink_c(12*N_rb_dl)
        self.snk2 = gr.vector_sink_c(12*N_rb_dl)
        
        self.snko0 = gr.vector_sink_c(12*N_rb_dl)
        self.snko1 = gr.vector_sink_c(12*N_rb_dl)
        self.snko2 = gr.vector_sink_c(12*N_rb_dl)
        
        # UUT
        self.cest0 = lte_swig.channel_estimator(N_rb_dl)
        
        self.tb.connect(self.src, self.cest0)
        self.tb.connect( (self.cest0,0), self.snk0)
        self.tb.connect( (self.cest0,1), self.snk1)
        self.tb.connect( (self.cest0,2), self.snk2)

        self.cest1 = lte_swig.linear_OFDM_estimator_vcvc(N_rb_dl)
        self.tb.connect(self.src, self.cest1)
        self.tb.connect( (self.cest1,0), self.snko0)
        self.tb.connect( (self.cest1,1), self.snko1)
        self.tb.connect( (self.cest1,2), self.snko2)
        
    def tearDown (self):
        self.tb = None
        
    def test_001_pn_sequence(self):
        pn_len = 220
        l = 4
        Ncp = 1
        for cell_id in range(0, 504, 10):
            for ns in range(0, 20, 3):
                cinit = 1024*(7*(ns+1)+l+1)*(2*cell_id+1)+2*cell_id+Ncp
                cpp_pn_seq = self.cest0.get_pn_sequence(pn_len, cinit)
                py_pn_seq = pn_generator(pn_len, cinit)
                self.assertListEqual(list(cpp_pn_seq), py_pn_seq)
                
    def test_002_rs_sequence(self):        
        Ncp = 1
        for ns in range(0,20, 4):
            for l in range(0,5,4):
                for cell_id in range(0,504,35):
                    cpp_rs_seq = self.cest0.get_rs_sequence(ns, l, cell_id, Ncp)
                    py_rs_seq = rs_generator(ns, l, cell_id, Ncp)
                    self.assertComplexTuplesAlmostEqual(cpp_rs_seq, tuple(py_rs_seq))
                
    def test_003_rs_frame(self):
        N_rb_dl = self.N_rb_dl
        cell_id = 124
        Ncp = 1
        p = 0
        self.cest0.set_cell_id(cell_id)
        
        cpp_matrix = self.cest0.get_frame_rs_symbols()
        for i in range(len(cpp_matrix)):
            mat = cpp_matrix[i]
            mat = np.abs(mat)
            comp = np.ones( (len(mat),), dtype=type(mat[0]) )
            self.assertFloatTuplesAlmostEqual(mat, comp)
            
        py_matrix = []
        for ns in range(20):
            sym0 = symbol_pilot_value_and_position(N_rb_dl, ns, 0, cell_id, Ncp, p)
            sym4 = symbol_pilot_value_and_position(N_rb_dl, ns, 4, cell_id, Ncp, p)
            py_matrix.extend([sym0[1], sym4[1] ])
                  
        for i in range(len(py_matrix)):
            pym = py_matrix[i]
            cppm = cpp_matrix[i]
            self.assertComplexTuplesAlmostEqual(cppm, pym)

    def test_004_t (self):
        # set up fg
        self.cest0.set_cell_id(124)
        self.cest1.set_cell_id(124)
        
        # run flowgraph
        self.tb.run ()
        # check data
        data00 = self.snk0.data()
        data10 = self.snko0.data()
        data01 = self.snk1.data()
        data11 = self.snko1.data()
       
        value = 0
        for i in range((len(data00)/(12*6))-7):
            vec0 = data00[(i*12*6):((i+1)*12*6)]
            vec1 = data10[((i+7)*12*6):((i+7+1)*12*6)]
            vec01 = data01[(i*12*6):((i+1)*12*6)]
            vec11 = data11[((i+7)*12*6):((i+7+1)*12*6)]
            self.assertComplexTuplesAlmostEqual(vec0, vec1, 4)
                
    def test_005_t(self):
        self.cest0.set_cell_id(124)
        self.cest1.set_cell_id(124)
        
        cell_id = 124
        N_ant = 2
        style= "tx_diversity"
        N_rb_dl = self.N_rb_dl
        sfn = 0

        mib = pack_mib(50,0,1.0, 511)
        bch = encode_bch(mib, N_ant)
        pbch = encode_pbch(bch, cell_id, N_ant, style)
        frame = generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant)
        stream = frame[0].flatten()
        stream = stream + frame[1].flatten()
        
        self.src.set_data(stream)
        self.tb.run()
        
        # get result and assert if rx samples are unchanged        
        res0 = self.snk0.data()
        self.assertComplexTuplesAlmostEqual(stream,res0)
        
        print "now interesting"
        res1 = self.snk1.data()
        res2 = self.snk2.data()
        
        print "prepare"
        exp_res12 = np.ones( (len(res1),) ,dtype=np.complex )
        print len(exp_res12)
        print type(exp_res12)
        
        partl = 10
        failed = 0
        for i in range(len(res1)/partl):
            partres = res1[partl*i:partl*(i+1)]
            partcom = exp_res12[partl*i:partl*(i+1)]
            try:
                self.assertComplexTuplesAlmostEqual(partcom, partres)
                print i*partl
            except:
                failed = failed +1 
                        
        print failed
        
        print res1[60:80]

        

            


if __name__ == '__main__':
    gr_unittest.main ()

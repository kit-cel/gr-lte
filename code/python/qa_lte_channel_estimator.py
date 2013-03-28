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
            
        print "len(input) = " + str(len(values))
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

    def test_001_t (self):
        # set up fg
        self.cest0.set_cell_id(124)
        #self.cest1.eq.set_cell_id(124)
        self.cest1.set_cell_id(124)
        
        # run flowgraph
        self.tb.run ()
        # check data
        data00 = self.snk0.data()
        data10 = self.snko0.data()
        data01 = self.snk1.data()
        data11 = self.snko1.data()
        print "\n\nlen data0 = " +str(len(data00)) + "\tlen data1 = " + str(len(data10))
        print "vec0's = " + str(len(data00)/(12*6)) + "\tvec1's = " + str(len(data10)/(12*6))
        
        value = 0
        for i in range((len(data00)/(12*6))-7):
            vec0 = data00[(i*12*6):((i+1)*12*6)]
            vec1 = data10[((i+7)*12*6):((i+7+1)*12*6)]
            vec01 = data01[(i*12*6):((i+1)*12*6)]
            vec11 = data11[((i+7)*12*6):((i+7+1)*12*6)]
            try:
                self.assertComplexTuplesAlmostEqual(vec0, vec1, 4)
                print "CORRECT"
                for cnt in range(len(vec0)):
                    print '{0:.4f}'.format(cmath.phase(vec01[cnt])-cmath.phase(vec11[cnt]))
                    #print str(cmath.phase(vec01[cnt])-cmath.phase(vec11[cnt]))
                    #print str(abs(vec01[cnt])) + "\t" + str(abs(vec11[cnt]))
                    #print "diff = " + str(abs(vec01[cnt]-vec11[cnt]))
                
            except:
                print "vec0\t\tvec1\tFAIL " + str(i)
                #for cnt in range(5):
                #    print '{0.real:.2f} + {0.imag:.2f}j\t{1.real:.2f} + {1.imag:.2f}j'.format(vec0[cnt],vec1[cnt])
                    
                for cnt in range(len(vec0)):
                    #print "diff = " + str(abs(vec0[cnt]-vec1[cnt]))
                    m=0
                #print '({0.real:.2f} + {0.imag:.2f}j)'.format(vec0)
                #print vec0[0:3]
                #print vec1[0:3]
                value = value+1
                
    def test_002_t(self):
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
        
        print "now interesstinr"
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
        
        self.assertComplexTuplesAlmostEqual(exp_res12, res1)

        


if __name__ == '__main__':
    gr_unittest.main ()

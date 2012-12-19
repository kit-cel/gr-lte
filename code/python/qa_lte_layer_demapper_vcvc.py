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
import lte_swig
import numpy
import scipy.io

class qa_layer_demapper_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        # Case 1 antenna port assumed
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_layer_demap1.mat')
        mat_u1=range(240)
        mat_u1=tuple(mod['pbch_layer_demap1'].flatten())
        
        # Case 2 antenna ports assumed
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_layer_demap2.mat')
        mat_u2=range(240)
        mat_u2=tuple(mod['pbch_layer_demap1'].flatten())
        
        # pack input into one tuple
        mat_d=range(480)
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=(mat_u1[idx])
            mat_d[idx+240]=mat_u2[idx]
        intu=tuple(mat_d)
        #print len(intu)
        #print type(intu)
        #print type(intu[8])
        #print intu[0:3]

        self.src = gr.vector_source_c(intu,False,240)
        self.ldm = lte_swig.layer_demapper_vcvc(1,'tx_diversity')
        self.snk = gr.vector_sink_c(240)
        
        self.tb.connect(self.src,self.ldm,self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_frame_demap1.mat')
        mat_u=range(240)
        mat_u=tuple(mod['pbch_frame_demap'].flatten())
        mat_d=range(240)
        for i in mat_d:
            mat_d[i]=(mat_u[i])
        outtu=tuple(mat_d)

        res = self.snk.data()
        res = res[0:240]
        #print res[0:5]
        
        #for idx, val in enumerate(res):
        #    print str(val) + "\t" + str(outtu[idx]) + "    \t" + str(val-outtu[idx])
        
        # Test output 1
        try:
            self.assertComplexTuplesAlmostEqual(outtu,res,5)
            print "first  run: Tuples MATCH!!!"
        except AssertionError:
            print "first  run: Tuples are unequal"
        #print self.assertComplexTuplesAlmostEqual(outtu,res,5)
        
        # rerun test with different antenna config.
        self.ldm.set_N_ant(2)
        self.tb.run()
        res2 = self.snk.data()
        res2 = res2[240:480]
        #print res2[0:5]
        
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_frame_demap2.mat')
        mat_u=range(240)
        mat_u=tuple(mod['pbch_frame_demap'].flatten())
        mat_d=range(240)
        for i in mat_d:
            mat_d[i]=(mat_u[i])
        outtu2=tuple(mat_d)
        
        for i in range(5):
            print str(res2[i]) + "\t" + str(outtu2[i])

        # Test output 2
        try:
            self.assertComplexTuplesAlmostEqual(outtu2,res2,5)
            print "second run: Tuples MATCH!!!"
        except AssertionError:
            print "second run: Tuples are unequal"
        #self.assertComplexTuplesAlmostEqual(outtu2,res2,5)

        
        

if __name__ == '__main__':
    gr_unittest.main ()

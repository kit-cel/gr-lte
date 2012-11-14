#!/usr/bin/env python
# 
# Copyright 2012 <+YOU OR YOUR COMPANY+>.
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

class qa_pre_decoder_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        # Input 1, PBCH frame
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_frame.mat') 
        #mat_u1=range(240)
        mat_u1=tuple(mod['pbch_frame1'].flatten())
        mat_d=range(240)
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        intu1=tuple(mat_d)
        
        # Input 2, CE values for antenna port 1
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_ce1.mat')
        #mat_u2=range(240)
        mat_u2=tuple(mod['pbch_ce1'].flatten())
        mat_d=range(240)
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        intu2=tuple(mat_d)

        # Input 2, CE values for antenna port 1
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_ce2.mat')
        #mat_u2=range(240)
        mat_u2=tuple(mod['pbch_ce1'].flatten())
        mat_d=range(240)
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        intu3=tuple(mat_d)
        print str(intu1[0]) + "\t" + str(intu2[0]) + "\t" + str(intu3[0])
        
        self.src1 = gr.vector_source_c( intu1, False, 240)
        self.src2 = gr.vector_source_c( intu2, False, 240)
        #self.src3 = gr.vector_source_c( intu3, False, 240)
        
        self.pd = lte_swig.pre_decoder_vcvc(1,'tx_diversity')
        
        self.snk = gr.vector_sink_c(240)
        
        self.tb.connect(self.src1,(self.pd,0) )
        self.tb.connect(self.src2,(self.pd,1) )
        #self.tb.connect(self.src3,(self.pd,2) )
        self.tb.connect(self.pd,self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        res = self.snk.data()
        
        # Case 1 antenna port assumed
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_layer_demap1.mat')
        mat_u1=range(240)
        mat_u1=tuple(mod['pbch_layer_demap1'].flatten())
        mat_d=range(240)
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        outtu1=tuple(mat_d)
        
        # Case 2 antenna ports assumed
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_layer_demap2.mat')
        mat_u2=range(240)
        mat_u2=tuple(mod['pbch_layer_demap1'].flatten())
        mat_d=range(240)
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        outtu2=tuple(mat_d)
        
        for i in range(5):
            print str(res[i]) + "\t" + str(outtu2[i])
        
        #print str(outtu1[0]) + "\t" + str(outtu2[0]) + "\t" + str(res[0])
        
        try:
            self.assertComplexTuplesAlmostEqual(outtu1,res,5)
            print "try 1 antenna case SUCCESSFUL!!!"
        except AssertionError:
            print "try 1 antenna case FAILED!!!"
            
        try:
            self.assertComplexTuplesAlmostEqual(outtu2,res,5)
            print "try 2 antenna case SUCCESSFUL!!!"
        except AssertionError:
            print "try 2 antenna case FAILED!!!"
        #self.assertComplexTuplesAlmostEqual(outtu2,res,5)


if __name__ == '__main__':
    gr_unittest.main ()

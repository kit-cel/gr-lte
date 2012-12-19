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

class qa_pbch_demux_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        print "qa_pbch_demux_vcvc START"
        
        # Input 1, PBCH frame
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_frame.mat') 
        mat_u1=tuple(mod['frame_mat'].flatten())
        mat_d=range(len(mat_u1))
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        intu1=tuple(mat_d)
        
        # Input 2, CE values for antenna port 1
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_ce_frame_mat1.mat')
        mat_u2=tuple(mod['ce_frame_mat1'].flatten())
        mat_d=range(len(mat_u2))
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        intu2=tuple(mat_d)

        # Input 2, CE values for antenna port 1
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_ce_frame_mat2.mat')
        mat_u2=tuple(mod['ce_frame_mat2'].flatten())
        mat_d=range(len(mat_u2))
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        intu3=tuple(mat_d)

        
        self.src1 = gr.vector_source_c( intu1, False, 72)
        self.src2 = gr.vector_source_c( intu2, False, 72)
        self.src3 = gr.vector_source_c( intu3, False, 72)
        
        cell_id = 124
        N_rb_dl = 6
        self.demux = lte_swig.pbch_demux_vcvc(N_rb_dl) # cell_id,
        self.demux.set_cell_id(cell_id)
        
        self.snk1 = gr.vector_sink_c(240)
        self.snk2 = gr.vector_sink_c(240)
        self.snk3 = gr.vector_sink_c(240)
        
        self.tb.connect(self.src1,(self.demux,0) )
        self.tb.connect( (self.demux,0),self.snk1)
        self.tb.connect(self.src2,(self.demux,1) )
        self.tb.connect( (self.demux,1),self.snk2)
        self.tb.connect(self.src3,(self.demux,2) )
        self.tb.connect( (self.demux,2),self.snk3)
        
        

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        res1 = self.snk1.data()
        res2 = self.snk2.data()
        res3 = self.snk3.data()
        
        # Input 1, PBCH frame
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_frame.mat') 
        mat_u1=tuple(mod['pbch_frame1'].flatten())
        mat_d=range(240)
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        outtu1=tuple(mat_d)
        
        # Input 2, CE values for antenna port 1
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_ce1.mat')
        mat_u2=tuple(mod['pbch_ce1'].flatten())
        mat_d=range(240)
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        outtu2=tuple(mat_d)

        # Input 2, CE values for antenna port 1
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_pbch_ce2.mat')
        mat_u2=tuple(mod['pbch_ce1'].flatten())
        mat_d=range(240)
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        outtu3=tuple(mat_d)
        
        #for i in range(240):
        #    print str(i) + "\t" + str(res3[i]) + "\t" + str(outtu3[i])
            
            
        try:
            self.assertComplexTuplesAlmostEqual(res1,outtu1,5)
            print "res1 SUCCESSFUL!!!"
        except AssertionError:
            print "res1 FAILED!!!"

        try:
            self.assertComplexTuplesAlmostEqual(res2,outtu2,5)
            print "res2 SUCCESSFUL!!!"
        except AssertionError:
            print "res2 FAILED!!!"
        
        try:
            self.assertComplexTuplesAlmostEqual(res3,outtu3,5)
            print "res3 SUCCESSFUL!!!"
        except AssertionError:
            print "res3 FAILED!!!"

        print "len(res1) = " + str(len(res1)) + "\nlen(res2) = " + str(len(res2)) + "\nlen(res3) = " + str(len(res3))
        



if __name__ == '__main__':
    gr_unittest.main ()

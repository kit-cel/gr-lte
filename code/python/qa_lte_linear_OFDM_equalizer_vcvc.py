#!/usr/bin/env python
# 
# Copyright 2012 Johannes Demel
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
import os

class qa_linear_OFDM_equalizer_vcvc (gr_unittest.TestCase):

    def setUp (self):
        print os.getpid()
        raw_input("Press the ANY key to continue")
    
        self.tb = gr.top_block ()
        
        print "\nqa_lte_linear_OFDM_equalizer_vcvc START"
        
        
        # Input 1, PBCH frame
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_frame.mat') 
        mat_u1=tuple(mod['frame_mat'].flatten())
        mat_d=range(len(mat_u1))
        for idx, val in enumerate(mat_u1):
            mat_d[idx]=val
        intu1o=tuple(mat_d)
        intu1=intu1o[0:140*72]
        #intu1=intu1[72*126:140*72]
        self.intu1=intu1
        print "len(intu1) = " + str(len(intu1))
        
        self.src = gr.vector_source_c( intu1, True, 72)
        
        cell_id = 124
        N_rb_dl = 6
        self.eq = lte_swig.linear_OFDM_equalizer_vcvc(N_rb_dl) #cell_id,
        self.eq.set_cell_id(cell_id)
        
        self.sh1 = gr.skiphead(gr.sizeof_gr_complex*12*N_rb_dl,7)
        self.sh2 = gr.skiphead(gr.sizeof_gr_complex*12*N_rb_dl,7)
        self.sh3 = gr.skiphead(gr.sizeof_gr_complex*12*N_rb_dl,7)
        self.hd1 = gr.head(gr.sizeof_gr_complex*12*N_rb_dl,280)
        self.hd2 = gr.head(gr.sizeof_gr_complex*12*N_rb_dl,280)
        self.hd3 = gr.head(gr.sizeof_gr_complex*12*N_rb_dl,280)
        
        self.snk1 = gr.vector_sink_c(72)
        self.snk2 = gr.vector_sink_c(72)
        self.snk3 = gr.vector_sink_c(72)
        
        self.tb.connect(self.src,self.eq)
        self.tb.connect( (self.eq,0) ,self.sh1,self.hd1,self.snk1)
        self.tb.connect( (self.eq,1) ,self.sh2,self.hd2,self.snk2)
        self.tb.connect( (self.eq,2) ,self.sh3,self.hd3,self.snk3)
        
        
    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # get output data
        res1=self.snk1.data()
        res2=self.snk2.data()
        res3=self.snk3.data()
        # check data
        outtu1=self.intu1
        
        # Input 2, CE values for antenna port 1
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_ce_frame_mat1.mat')
        mat_u2=tuple(mod['ce_frame_mat1'].flatten())
        mat_d=range(len(mat_u2))
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        outtu2=tuple(mat_d)

        # Input 2, CE values for antenna port 1
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_ce_frame_mat2.mat')
        mat_u2=tuple(mod['ce_frame_mat2'].flatten())
        mat_d=range(len(mat_u2))
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        outtu3=tuple(mat_d)
        
        
        '''
        # RS vec input
        mod=scipy.io.loadmat('/home/demel/exchange/matlab_rs_vec.mat')
        mat_u2=tuple(mod['rs_vec'].flatten())
        mat_d=range(len(mat_u2))
        for idx, val in enumerate(mat_u2):
            mat_d[idx]=val
        rs_vec=tuple(mat_d)
        '''
        
        #print len(res1)
        #print len(outtu2)
        #print len(res3)
        #print len(outtu3)
        #self.assertComplexTuplesAlmostEqual(res1,outtu1[0:10080],5)
        
        c2=0  
        for i in range(2*10080/72):
            o=i%140
            try:
                self.assertComplexTuplesAlmostEqual(res2[72*i:72*(i+1)],outtu2[72*o:72*(o+1)],5)
                print str(i) + "\tres2 == outtu2"
            except AssertionError:
                c2=c2+1
                print str(i) + "\tres2 != outtu2 FAILED!!!"
                if (i == 126):
                    for n in range(72):
                        print str(n) + "\t" + str(res2[72*i+n]) + "    \t" + str(outtu2[72*i+n])

        c3=0
        for i in range(10080/72):
            try:
                self.assertComplexTuplesAlmostEqual(res3[72*i:72*(i+1)],outtu3[72*i:72*(i+1)],3)
                #print str(i) + "\tres3 == outtu3"
            except AssertionError:
                #print str(i) + "\tres3 != outtu3 FAILED!!!"
                c3=c3+1
        
        print c3
        


if __name__ == '__main__':
    gr_unittest.main ()

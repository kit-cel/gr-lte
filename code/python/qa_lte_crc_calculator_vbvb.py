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

class qa_crc_calculator_vbvb (gr_unittest.TestCase):

    def setUp (self):
    
        print "qa_crc_calculator_vbvb START"
        self.tb = gr.top_block ()
        
        # Read in binary Matlab data from viterbi decoder output.
        vit=scipy.io.loadmat('/home/demel/exchange/matlab_vit.mat')
        mat_u=range(40)
        mat_u=tuple(vit['vit'].flatten())
        mat_vit=range(40)
        for i in mat_vit:
            mat_vit[i]=int(mat_u[i])
        mat_vit=tuple(mat_vit)
        self.in_data=mat_vit
        intu=mat_vit
        self.src = gr.vector_source_b( intu, False, 40)
        #self.conv = gr.float_to_char(24,1)
        print type(intu[8])
        print type(intu)
        print intu
        
        
        
        crc_poly   = (1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1)
        init_state = (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
        final_xor1 = (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
        final_xor2 = (1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)
        final_xor4 = (0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1)
        block_len = 24
        
        self.crc = lte_swig.crc_calculator_vbvb()#crc_poly,init_state,final_xor1,block_len)
        
        
        self.snk = gr.vector_sink_b(24)
        self.snk2= gr.vector_sink_b(1)
        self.tb.connect(self.src,(self.crc,0),self.snk)
        self.tb.connect((self.crc,1),self.snk2)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        res = self.snk.data()
        print res
        print self.in_data[0:24]
        #print len(res)
        N_ant = self.snk2.data()[0]
        print "N_ant = " + str(N_ant)
        
        # Read in binary Matlab data from viterbi decoder output.
        dec1s=scipy.io.loadmat('/home/demel/exchange/matlab_dec1.mat')
        dec2s=scipy.io.loadmat('/home/demel/exchange/matlab_dec2.mat')
        dec4s=scipy.io.loadmat('/home/demel/exchange/matlab_dec4.mat')
        dec1u=tuple(dec1s['matdect'].flatten())
        dec2u=tuple(dec2s['matdect'].flatten())
        dec4u=tuple(dec4s['matdect'].flatten())
        #self.assertEqual(dec1u,dec2u)
        #self.assertEqual(dec1u,dec4u)
        #print "len(dec1u) = " + str(len(dec1u))
        #mat_u=tuple(vit['vit'].flatten())
        #mat_vit=range(40)
        #for i in mat_vit:
        #    mat_vit[i]=int(mat_u[i])
        #mat_vit=tuple(mat_vit)

        
        
        print "qa_crc_calculator_vbvb END"


if __name__ == '__main__':
    gr_unittest.main ()

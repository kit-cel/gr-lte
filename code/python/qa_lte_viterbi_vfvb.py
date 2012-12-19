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

from gnuradio import gr, gr_unittest, trellis
import lte
import numpy
import scipy.io

class qa_viterbi_vfvb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        # Read in successfully decoded live data from Matlab
        linf=open('/home/demel/exchange/matlab_d.txt')
        lintu=range(120)
        for i in lintu:
            lintu[i]=float(linf.readline())
        #print lintu
        # source for live data
        self.srcl = gr.vector_source_f(lintu,False,120)
        
        # Read in .txt file with example MIB encoded + CRC checksum
        inf=open('/home/demel/exchange/crc.txt')
        self.intu=range(40)
        for i in self.intu:
            self.intu[i]=float(inf.readline())
                
        #inf=open('/home/demel/exchange/matlab_d.txt')
        #intu=range(120)
    	#for i in range(120):
    	#	intu[i]=float(inf.readline())
        
        # Source and conversions
        self.src  = gr.vector_source_f(self.intu,False,40)
        self.conv = gr.float_to_char(40,1)

        # Resize vector with repetition of last part
        # Vector to stream for encoder
        my_map1=range(46)
        for i in range(40):
            my_map1[i+6]=i
        for i in range(6):
            my_map1[i]=i+40
        self.map1 = lte.vector_resize_vbvb(my_map1,40,46)
        self.vtos = gr.vector_to_stream(1*gr.sizeof_char,46)
        
        # Encoding of input data
        self.fsm  = trellis.fsm(1,3,[91,121,117])
        self.enc  = trellis.encoder_bb(self.fsm,0)
        # unpack packed bits from encoder
        self.unp  = gr.unpack_k_bits_bb(3)
        # stream to vector
        self.stov = gr.stream_to_vector(1*gr.sizeof_char,138)
        # Remove first part which contains tail-biting init stuff
        map2 = range(120)
        for i in map2:
            map2[i]= i+18
        self.map2 = lte.vector_resize_vbvb(map2,138,120)
        # conversion from char to float to match input of decoder
        self.conv2= gr.char_to_float(120,1)
        
        
        ###############################################
        # From here on only "receiver side" processing
        ###############################################
        
        # like QPSK demodulation: NRZ coding.
        vec2=range(120)
        for i in vec2:
            vec2[i]=float(-2.0)
        self.mult = gr.multiply_const_vff(vec2)
        vec=range(120)
        for i in vec:
            vec[i]=1
        self.add  = gr.add_const_vff(vec)
        
        # this is the actual unit under test
        self.vit = lte.viterbi_vfvb()
        
        # Sinks
        self.snk = gr.vector_sink_b(40)
        self.snk2 = gr.vector_sink_f(120)
        
        # connecting blocks
        self.tb.connect(self.src,self.conv,self.map1,self.vtos,self.enc,self.unp)
        self.tb.connect(self.unp,self.stov,self.map2,self.conv2)
        self.tb.connect(self.conv2,self.mult,self.add)
        self.tb.connect(self.srcl,self.vit,self.snk)
        self.tb.connect(self.add,self.snk2)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        
        # Read in binary Matlab data from viterbi decoder output.
        vit=scipy.io.loadmat('/home/demel/exchange/matlab_vit.mat')
        mat_u=range(40)
        mat_u=tuple(vit['vit'].flatten())
        mat_vit=range(40)
        for i in mat_vit:
            mat_vit[i]=int(mat_u[i])
        mat_vit=tuple(mat_vit)
        #print type(mat_u)
        #print type(mat_u[8])
        #print type(mat_vit)
        #print type(mat_vit[8])
        
        res2 = self.snk2.data()
        in_data=range(len(self.intu))
        for i in in_data:
            in_data[i]=int(self.intu[i])
        in_data = tuple(in_data)
        
        #for i in range(120/3):
        #    print str(res2[i*3:3*(i+1)]) + "\t" + str(in_data[i])
        
        res = self.snk.data()
        #print type(res[5])
        #print type(res2)
        #print type(in_data)
        print "result   = " + str(res)
        #print "expected = " + str(in_data)
        print "mat_vit  = " + str(mat_vit)
        print self.assertTupleEqual(mat_vit,res)
        #print "NRZ signal"
        #print res


if __name__ == '__main__':
    gr_unittest.main ()

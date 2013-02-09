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
import lte_test
import scipy.io

class qa_crc_calculator_vbvb (gr_unittest.TestCase):

    def setUp (self):
    
        print "qa_crc_calculator_vbvb START"
        self.tb = gr.top_block ()
        #crc_poly   = (1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1)
        #init_state = (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
        #final_xor1 = (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
        #final_xor2 = (1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)
        #final_xor4 = (0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1)
        #block_len = 24
        
        intu = [0]*40
        self.src = gr.vector_source_b( intu, False, 40)
        self.crc = lte_swig.crc_calculator_vbvb()#crc_poly,init_state,final_xor1,block_len)
        
        self.snk = gr.vector_sink_b(24)
        self.snk2= gr.vector_sink_b(1)
        self.tb.connect(self.src,(self.crc,0),self.snk)
        self.tb.connect((self.crc,1),self.snk2)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
            
        N_ant = [1,2,3,4]
        N_rb_dl = 50
        phich_dur = 0
        phich_res = 1.0
        sfn = 511
        mib = lte_test.pack_mib(N_rb_dl, phich_dur, phich_res, sfn)
        input_data = []
        for i in N_ant:
            mib_crc = lte_test.crc_checksum(mib, i)
            input_data.extend(mib_crc)
        
        self.src.set_data(input_data)

        # set up fg
        self.tb.run ()
        # check data
        res0 = self.snk.data()
        res0 = tuple([int(i) for i in res0]) # just to ensure data types
        res1 = self.snk2.data()
        for i in range(4):
            try:
                self.assertEqual(tuple(mib),res0[i*24:(i+1)*24])
                self.assertEqual(N_ant[i], res1[i])
            except:
                print res0[i*24:(i+1)*24]
                print "result = " + str(res1[i]) + "\tinput = " + str(N_ant[i])

            

        print "qa_crc_calculator_vbvb END"


if __name__ == '__main__':
    gr_unittest.main ()

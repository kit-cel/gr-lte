#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

from gnuradio import gr, gr_unittest, blocks
import lte_swig as lte
import lte_test

class qa_crc_check_vbvb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        data_len = 24
        self.N_ant = 4
        final_xor = {1:0, 2:65535, 4:21845}
        #final_xor = 0 # 1 antenna
        #final_xor = 65535 # 2 antennas
        #final_xor = 21845 # 4 antennas
        block_len = data_len + 16
        data = [0]*block_len
        self.src = blocks.vector_source_b(data, False, block_len)
        self.crc = lte.crc_check_vbvb(data_len, final_xor[self.N_ant])
        self.snk0 = blocks.vector_sink_b(data_len)  
        self.snk1 = blocks.vector_sink_b(1)
        
        self.tb.connect(self.src, (self.crc,0), self.snk0)
        self.tb.connect( (self.crc,1), self.snk1 )
        
    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        N_rb_dl = 50
        phich_dur = 0
        phich_res = 1.0
        sfn = 511
        data = []
        test_len = 20
        myinput = []
        for i in range(test_len):
            mib = lte_test.pack_mib(N_rb_dl, phich_dur, phich_res, i*4)
            myinput.extend(mib)            
            data.extend(lte_test.crc_checksum(mib, self.N_ant) )
            
        self.src.set_data(data)        
        self.tb.run ()
        res = self.snk0.data()
        print "data bits"
        self.assertTupleEqual(tuple(res), tuple(myinput))        
        check = self.snk1.data()
        crcres = [int(check[i]) for i in range(len(check))]
        print check
        self.assertTupleEqual(tuple(crcres), tuple([1]*test_len))
        print crcres
        # check data


if __name__ == '__main__':
    #gr_unittest.run(qa_crc_check_vbvb, "qa_crc_check_vbvb.xml")
    gr_unittest.run(qa_crc_check_vbvb)

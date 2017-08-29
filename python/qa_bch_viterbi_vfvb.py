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
from bch_viterbi_vfvb import bch_viterbi_vfvb
import lte_test

class qa_bch_viterbi_vfvb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        mib = lte_test.pack_mib(50,0,1.0, 511)
        mib_crc = lte_test.crc_checksum(mib, 2)
        c_encoded = lte_test.convolutional_encoder(mib_crc)
        data = lte_test.nrz_encoding(c_encoded)

        self.src = blocks.vector_source_f(data,False, 120)
        self.vit = bch_viterbi_vfvb()
        self.snk = blocks.vector_sink_b(40)
        
        # connecting blocks
        self.tb.connect(self.src,self.vit, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        print "Viterbi decoder test"
        test_range = 1030
        
        data = []
        my_input = []
        for sfn in range(test_range):
            mib = lte_test.pack_mib(50,0,1.0, sfn%1024 )
            mib_crc = lte_test.crc_checksum(mib, 2)
            my_input.extend(mib_crc)
            c_encoded = lte_test.convolutional_encoder(mib_crc)
            nrz_encoded = lte_test.nrz_encoding(c_encoded)
            data.extend(nrz_encoded)

        self.src.set_data(data)
        self.tb.run ()
        
        count = 0
        res = self.snk.data()
        for i in range(test_range):
            try:
                self.assertEqual(tuple(my_input[i*40:(i+1)*40]), res[i*40:(i+1)*40])
#                print "SUCCESS " + str(i)
                count = count+1
            except:
                pass
#                print "FAILED  " + str(i)
     
        rate = float(count)/float(test_range)  
        print (rate)
        if not rate == 1.0:
            self.assertEqual( [1] , [0] ) #throws always n error
        print "Viterbi decoder test END"


if __name__ == '__main__':
    gr_unittest.run(qa_bch_viterbi_vfvb, "qa_bch_viterbi_vfvb.xml")

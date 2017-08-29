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
import numpy as np

class qa_subblock_deinterleaver_vfvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.vlen = 80
        self.group_items = 1
        data = lte_test.interleave(range(self.vlen))
        self.src = blocks.vector_source_f(data, False, self.vlen)
        self.subdeint = lte.subblock_deinterleaver_vfvf(self.vlen, self.group_items)
        self.snk = blocks.vector_sink_f(self.vlen)
        
        self.tb.connect(self.src, self.subdeint, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data
        res = self.snk.data()
        for i in range(80):
            if not i == int(res[i]):
                print "fail"
                assert(1 == 0)
        print res
        
    def test_002_t(self):
        self.tb2 = gr.top_block()
        N_rb_dl = 50
        phich_dur = 0
        phich_res = 1.0
        N_ant = 2
        data = []
        test_len = 100
        my_res = []
        for i in range(test_len):
            mib = lte_test.pack_mib(N_rb_dl, phich_dur, phich_res, i)
            crc = lte_test.crc_checksum(mib, N_ant)
            conv = lte_test.convolutional_encoder_sorted(crc)
            convit = lte_test.convolutional_encoder(crc)
            my_res.extend(lte_test.nrz_encoding(convit) )
            rated = lte_test.rate_match(conv)
            nrz = lte_test.nrz_encoding(rated)
            data.extend(nrz)
        
        src1 = blocks.vector_source_f(data, False, 120)
                
        vts0 = blocks.vector_to_stream(40*gr.sizeof_float, 3)
        sts0 = blocks.stream_to_streams(40*gr.sizeof_float, 3)        
        self.tb2.connect(src1, vts0, sts0)
        
        
        inter = blocks.interleave(gr.sizeof_float)
        for i in range(3):
            deint = lte.subblock_deinterleaver_vfvf(40, 1)
            vts1 = blocks.vector_to_stream(1*gr.sizeof_float, 40)
            self.tb2.connect( (sts0, i), deint, vts1, (inter, i))
        stv = blocks.stream_to_vector(1*gr.sizeof_float, 120)
        snk = blocks.vector_sink_f(120)        
        self.tb2.connect(inter, stv, snk)
        
        self.tb2.run()
        res = snk.data()
        print res[110:130]
        
        self.assertFloatTuplesAlmostEqual(tuple(my_res), res)
#        for i in range(test_len):
#            rpart = res[120*i:120*(i+1)]
#            mpart = my_res[120*i:120*(i+1)]
#            print i
#            self.assertFloatTuplesAlmostEqual(rpart, mpart)
#            
#        print my_res
#        print res


if __name__ == '__main__':
    gr_unittest.run(qa_subblock_deinterleaver_vfvf)

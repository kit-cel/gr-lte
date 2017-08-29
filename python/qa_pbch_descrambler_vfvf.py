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
#import lte_swig as lte
import lte_swig as lte
import lte_test

class qa_pbch_descrambler_vfvf (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        print "begin setup"


        key = "descr_part"
        self.src = blocks.vector_source_f([0.0]*480, False, 480)
        self.rpt = blocks.repeat(gr.sizeof_float*480, 4)
        self.stv = blocks.stream_to_vector(gr.sizeof_float*480, 4)
        self.descr = lte.pbch_descrambler_vfvf(key)
        self.snk = blocks.vector_sink_f(120)

        self.tb.connect(self.src, self.rpt, self.stv, self.descr, self.snk)
        print "end setup"

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        cell_id = 124
        N_ant = 2
        mib = lte_test.pack_mib(50,0,1.0, 511)
        bch = tuple(lte_test.encode_bch(mib, N_ant))
        p_scrambled = lte_test.pbch_scrambling(bch, cell_id)
        p_scrambled = lte_test.nrz_encoding(p_scrambled)
        n_bch = tuple(lte_test.nrz_encoding(bch))

        #print len(p_scrambled)
        #print p_scrambled[0:20]

        self.src.set_data(p_scrambled)#[0:480])
        self.descr.set_cell_id(cell_id)

        # set up fg
        self.tb.run()
        res = self.snk.data()
        print len(res)

        count = 0
        for i in range(len(res)/len(n_bch)):
            part = res[len(n_bch)*i:(i+1)*len(n_bch)]
            try:
                self.assertEqual(part, n_bch,3)
                print str(i) + "\tSUCCESS"
            except:
                print str(i)
                count = count +1
        print "\nresult"
        print count
        print len(res)/len(n_bch)

    def test_002_data(self):
        cell_id = 124
        N_ant = 2
        tl = 1024
        data = []
        ref_bch = []
        for i in range(tl/4):
            mib = lte_test.pack_mib(50,0,1.0, i*4)
            bch = tuple(lte_test.encode_bch(mib, N_ant))
            p_scrambled = lte_test.pbch_scrambling(bch, cell_id)
            p_scrambled = lte_test.nrz_encoding(p_scrambled)
            data.extend(p_scrambled)
            bch = tuple(lte_test.nrz_encoding(bch))
            ref_bch.extend(bch)

        self.src.set_data(data)
        self.descr.set_cell_id(cell_id)
        self.tb.run()
        
        res = self.snk.data()
        print len(res)

        count = 0
        for i in range(len(res)/len(ref_bch)):
            part = res[len(ref_bch)*i:(i+1)*len(ref_bch)]
            try:
                self.assertEqual(part, ref_bch,3)
                print str(i) + "\tSUCCESS"
            except:
                print str(i)
                count = count +1
        print "\nresult"
        print count
        print len(res)/len(ref_bch)



if __name__ == '__main__':
    gr_unittest.run(qa_pbch_descrambler_vfvf)

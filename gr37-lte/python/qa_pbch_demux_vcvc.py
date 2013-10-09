#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2013 <+YOU OR YOUR COMPANY+>.
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
import numpy as np
import lte_test
import pmt

class qa_pbch_demux_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

        self.N_rb_dl = N_rb_dl = 50
        self.key = key = "symbol"
        n_carriers = 12*N_rb_dl
        intu = np.zeros(n_carriers,dtype=np.complex)

        self.src1 = blocks.vector_source_c( intu, False, n_carriers)
        self.src2 = blocks.vector_source_c( intu, False, n_carriers)
        self.src3 = blocks.vector_source_c( intu, False, n_carriers)

        self.demux = lte.pbch_demux_vcvc(N_rb_dl) # cell_id,

        self.snk1 = blocks.vector_sink_c(240)
        self.snk2 = blocks.vector_sink_c(240)
        self.snk3 = blocks.vector_sink_c(240)

        self.tb.connect(self.src1,(self.demux,0) )
        self.tb.connect( (self.demux,0),self.snk1)
        self.tb.connect(self.src2,(self.demux,1) )
        self.tb.connect( (self.demux,1),self.snk2)
        self.tb.connect(self.src3,(self.demux,2) )
        self.tb.connect( (self.demux,2),self.snk3)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        cell_id = 220
        N_ant = 2
        style= "tx_diversity"
        N_rb_dl = self.N_rb_dl
        sim_frames = 2
        tlen = 140 * sim_frames
        self.demux.set_cell_id(cell_id)

        mib = lte_test.pack_mib(50,0,1.0, 511)
        bch = lte_test.encode_bch(mib, N_ant)
        pbch = lte_test.encode_pbch(bch, cell_id, N_ant, style)

        stream = []
        for i in range(sim_frames):
            frame = lte_test.generate_frame(pbch, N_rb_dl, cell_id, i+20, N_ant)
            stream.extend(frame[0].flatten())

        print len(stream)


        key = self.key
        srcid = "source"
        tag_list = []
        for i in range(len(stream)/(12*N_rb_dl)):
            if i%7 == 0:
                tag = gr.tag_t()
                tag.key = pmt.string_to_symbol(key)
                tag.value = pmt.from_long(i%140)
                tag.offset = i*12*N_rb_dl
                tag.srcid = srcid
                tag_list.append(tag)

        tags = lte_test.get_tag_list(tlen, key, 140)
        print len (tag_list)


        self.src1.set_data(stream, tuple(tag_list))
        self.src2.set_data(np.ones(len(stream), dtype=np.complex))
        self.src3.set_data(np.ones(len(stream), dtype=np.complex))

        # set up fg
        self.tb.run ()
        # check data
        res1 = self.snk1.data()
        res2 = self.snk2.data()
        res3 = self.snk3.data()

        print len(res1)
        compare = res1[0:len(pbch[0])]

        #'''
        #partl = 10
        #for i in range(len(res1)/partl):
        #    partres = compare[partl*i:partl*(i+1)]
        #    partcom = pbch[0][partl*i:partl*(i+1)]
        #    try:
        #        self.assertComplexTuplesAlmostEqual(partcom,partres)
        #        print str(i*partl) + "\tsuccess"
        #    except:
        #        #print "\n\n\n\n\n\n"
        #        print str(i*partl) + "\t" + str(partres)
        #'''

        self.assertComplexTuplesAlmostEqual(compare, tuple(pbch[0]))

        self.assertComplexTuplesAlmostEqual(res2,tuple(np.ones(len(res2), dtype=np.complex)))
        self.assertComplexTuplesAlmostEqual(res3,tuple(np.ones(len(res3), dtype=np.complex)))


if __name__ == '__main__':
    gr_unittest.run(qa_pbch_demux_vcvc, "qa_pbch_demux_vcvc.xml")

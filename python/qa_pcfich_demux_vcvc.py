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

class qa_pcfich_demux_vcvc (gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

        self.N_rb_dl = N_rb_dl = 6
        self.key = "symbol"
        self.out_key = "subframe"
        self.msg_buf_name = "cell_id"
        n_carriers = 12 * N_rb_dl
        n_cfi_vec = 16
        intu = np.zeros(n_carriers, dtype=np.complex)

        self.src0 = blocks.vector_source_c(intu, False, n_carriers)
        self.demux = lte.pcfich_demux_vcvc(N_rb_dl, self.key, self.out_key, self.msg_buf_name)
        self.snk0 = blocks.vector_sink_c(n_cfi_vec)

        self.tag = blocks.tag_debug(n_cfi_vec * 8, "TAG")

        self.tb.connect(self.src0, (self.demux, 0))
        self.tb.connect((self.demux, 0), self.snk0)
        self.tb.connect((self.demux, 0), self.tag)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        cell_id = 124
        srcid = "test_src"
        N_ant = 2
        style = "tx_diversity"
        N_rb_dl = self.N_rb_dl
        sfn = 0
        Ncp = 1
        cfi = 2

        mib = lte_test.pack_mib(50,0,1.0, 511)
        bch = lte_test.encode_bch(mib, N_ant)
        pbch = lte_test.encode_pbch(bch, cell_id, N_ant, style)

        exp_res = []
        for sub in range(10):
            ns = sub * 2
            pcfich = lte_test.encode_pcfich(cfi, cell_id, ns, N_ant)
            exp_res.append(pcfich[0])

        frame = lte_test.generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant)
        data = frame[0].flatten()
        taglist = lte_test.get_tag_list(len(frame[0]), 140, self.key, srcid)
        self.src0.set_data(data, taglist)

        self.demux.set_cell_id(cell_id)
        #print calculate_pcfich_pos(N_rb_dl, cell_id)
        # set up fg
        self.tb.run()
        # check data

        res = self.snk0.data()

        print "test 001 check res"
        for i in range(len(res)/len(exp_res[0])):
            part = res[i*16:(i+1)*16]
            self.assertComplexTuplesAlmostEqual(part, exp_res[i])

    # def test_002_msg(self):
    #     print "test_002"
    #     msg = pmt.from_long(220)
    #     self.strobe = blocks.message_strobe(msg, 1000)
    #     self.tb.msg_connect(self.strobe, "strobe", self.demux, self.msg_buf_name)
    #     self.tb.run()


if __name__ == '__main__':
    gr_unittest.run(qa_pcfich_demux_vcvc)

# !/usr/bin/env python
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


class qa_layer_demapper_vcvc(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()
        vlen = 240
        self.src = blocks.vector_source_c([0] * vlen, False, vlen)
        self.demapper = lte.layer_demapper_vcvc(0, vlen, "tx_diversity")
        self.snk = blocks.vector_sink_c(vlen)
        self.tb.connect(self.src, self.demapper, self.snk)

    def tearDown(self):
        self.tb = None

    #def test_001_t (self):
    #    # set up fg
    #    self.tb.run ()
    #    # check data
    def test_001_setup_N_ant(self):
        print "\ntest_001_setup_N_ant"
        N_ant = [0, 1, 2, 4]

        for n in range(len(N_ant)):
            self.demapper.set_N_ant(N_ant[n])
            self.assertEqual(N_ant[n], self.demapper.get_N_ant())

    def test_002_setup_decoding_style(self):
        print "\ntest_002_setup_decoding_style"
        style = {0: "tx_diversity", 1: "spatial_multiplexing", 2: "nonsense"}

        for s in range(len(style)):
            self.demapper.set_decoding_style(style[s])
            try:
                self.assertEqual(style[0], self.demapper.get_decoding_style())
            except:
                if self.demapper.get_decoding_style() != style[0]:
                    print style[s]
                else:
                    self.assertEqual(style[1], self.demapper.get_decoding_style())


    def test_003_demapping(self):
        print "\ntest_003_demapping"
        N_ant = [1, 2, 4]
        cell_id = 124
        mib = lte_test.pack_mib(50, 0, 1.0, 511)
        bch = tuple(lte_test.encode_bch(mib, N_ant[0]))
        data = lte_test.pbch_scrambling(bch, cell_id)
        style = "tx_diversity"
        self.demapper.set_decoding_style(style)

        mapped = [[], [], []]
        mapped[0] = lte_test.layer_mapping(data, 1, style)[0]
        m2 = lte_test.layer_mapping(data, 2, style)
        m2a = []
        for i in range(len(m2[0]) / 120):
            m2a.extend(m2[0][120 * i:(i + 1) * 120])
            m2a.extend(m2[1][120 * i:(i + 1) * 120])
        mapped[1] = m2a

        m4 = lte_test.layer_mapping(data, 4, style)
        m4a = []
        for i in range(len(m4[0]) / 60):
            m4a.extend(m4[0][i * 60:(i + 1) * 60])
            m4a.extend(m4[1][i * 60:(i + 1) * 60])
            m4a.extend(m4[2][i * 60:(i + 1) * 60])
            m4a.extend(m4[3][i * 60:(i + 1) * 60])
        mapped[2] = m4a

        exp_res = [complex(data[i]) for i in range(len(data))]

        for i in range(3):
            self.demapper.set_N_ant(N_ant[i])
            print "N_ant = " + str(self.demapper.get_N_ant()), np.shape(mapped[i])
            self.src.set_data(mapped[i])
            self.snk.reset()
            self.tb.run()
            res = self.snk.data()
            try:
                self.assertComplexTuplesAlmostEqual(res, tuple(exp_res))
            except:
                print "FAILED N_ant = " + str(self.demapper.get_N_ant())
                self.assertComplexTuplesAlmostEqual(res, tuple(exp_res))

    def test_004_pcfich(self):
        print "\ntest_004_pcfich"
        cell_id = 124
        ns = 0
        N_ant = 2
        vlen = 16
        style = "tx_diversity"
        self.tb2 = gr.top_block()
        self.src = blocks.vector_source_c([0] * vlen, False, vlen)
        self.demapper = lte.layer_demapper_vcvc(0, vlen, style)
        self.snk = blocks.vector_sink_c(vlen)
        self.tb2.connect(self.src, self.demapper, self.snk)
        self.demapper.set_N_ant(N_ant)

        data = []
        exp_res = []
        for cfi in range(4):
            cfi_seq = lte_test.get_cfi_sequence(cfi + 1)
            scr_cfi_seq = lte_test.scramble_cfi_sequence(cfi_seq, cell_id, ns)
            mod_cfi_seq = lte_test.qpsk_modulation(scr_cfi_seq)
            exp_res.extend(mod_cfi_seq)
            lay_cfi_seq = lte_test.layer_mapping(mod_cfi_seq, N_ant, style)
            for i in range(len(lay_cfi_seq)):
                data.extend(lay_cfi_seq[i])

        self.src.set_data(data)
        self.tb2.run()
        res = self.snk.data()
        self.assertComplexTuplesAlmostEqual(res, exp_res)


if __name__ == '__main__':
    gr_unittest.run(qa_layer_demapper_vcvc)

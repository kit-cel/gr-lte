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


class qa_pre_decoder_vcvc(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

        N_ant = 2
        vlen = 240
        style = 'tx_diversity'

        intu1 = [0] * vlen
        intu2 = [0] * vlen
        intu3 = [0] * vlen

        self.src1 = blocks.vector_source_c(intu1, False, vlen)
        self.src2 = blocks.vector_source_c(intu2, False, vlen)
        self.src3 = blocks.vector_source_c(intu3, False, vlen)

        self.pd = lte.pre_decoder_vcvc(1, N_ant, vlen, style)

        self.snk = blocks.vector_sink_c(vlen)

        self.tb.connect(self.src1, (self.pd, 0))
        self.tb.connect(self.src2, (self.pd, 1))
        self.tb.connect(self.src3, (self.pd, 2))
        self.tb.connect(self.pd, self.snk)

    def tearDown(self):
        self.tb = None

    def test_001_generated(self):
        print "\ntest_001_generated"
        cell_id = 124
        N_ant = 2
        style = "tx_diversity"
        mib = lte_test.pack_mib(50, 0, 1.0, 511)
        bch = lte_test.encode_bch(mib, N_ant)

        scrambled = lte_test.pbch_scrambling(bch, cell_id)
        qpsk_modulated = lte_test.qpsk_modulation(scrambled)
        #print np.shape(qpsk_modulated)
        layer_mapped = lte_test.layer_mapping(qpsk_modulated, N_ant, style)
        pre_coded = lte_test.pre_coding(layer_mapped, N_ant, style)
        #print np.shape(pre_coded)

        h0 = [complex(1, 0)] * len(pre_coded[0])
        h1 = [complex(1, 0)] * len(pre_coded[1])
        stream = [pre_coded[0][i] + pre_coded[1][i] for i in range(len(pre_coded[0]))]

        self.src1.set_data(stream)
        self.src2.set_data(h0)
        self.src3.set_data(h1)
        self.tb.run()
        res = self.snk.data()

        exp_res = []
        for i in range(len(stream) / 240):
            print i
            lay0 = layer_mapped[0][i * 120:(i + 1) * 120]
            lay1 = layer_mapped[1][i * 120:(i + 1) * 120]
            comb = [lay0, lay1]
            exp_res.extend(lte_test.prepare_for_demapper_block(comb, N_ant, style))

        print "test 001 final ASSERT!"
        print self.assertComplexTuplesAlmostEqual(res, exp_res)

    def test_002_pcfich(self):
        print "test_002_pcfich"

        # some constants
        cell_id = 124
        N_ant = 2
        style = "tx_diversity"
        vlen = 16
        ns = 0

        # new top_block because even the interface changes
        self.tb2 = gr.top_block()

        # generate test data together with the expected output
        data = []
        exp_res = []
        for cfi in range(4):
            cfi_seq = lte_test.get_cfi_sequence(cfi + 1)
            scr_cfi_seq = lte_test.scramble_cfi_sequence(cfi_seq, cell_id, ns)
            mod_cfi_seq = lte_test.qpsk_modulation(scr_cfi_seq)
            lay_cfi_seq = lte_test.layer_mapping(mod_cfi_seq, N_ant, style)
            lay_cfi_prep = lte_test.prepare_for_demapper_block(lay_cfi_seq, N_ant, style)
            exp_res.extend(lay_cfi_prep)
            pc_cfi_seq = lte_test.pre_coding(lay_cfi_seq, N_ant, style)
            pc_cfi_seq = [pc_cfi_seq[0][i] + pc_cfi_seq[1][i] for i in range(len(pc_cfi_seq[0]))]
            data.extend(pc_cfi_seq)

        # dummy channel estimates
        intu2 = [complex(1, 0)] * len(data)
        intu3 = [complex(1, 0)] * len(data)

        # get blocks
        self.src1 = blocks.vector_source_c(data, False, vlen)
        self.src2 = blocks.vector_source_c(intu2, False, vlen)
        self.src3 = blocks.vector_source_c(intu3, False, vlen)
        self.pd = lte.pre_decoder_vcvc(1, 1, vlen, style)
        self.snk = blocks.vector_sink_c(vlen)

        # connect all blocks
        self.tb2.connect(self.src1, (self.pd, 0))
        self.tb2.connect(self.src2, (self.pd, 1))
        self.tb2.connect(self.src3, (self.pd, 2))
        self.tb2.connect(self.pd, self.snk)

        self.pd.set_N_ant(N_ant)

        # run flowgraph
        self.tb2.run()

        # compare result with expected result
        res = self.snk.data()

        self.assertComplexTuplesAlmostEqual(res, exp_res)


if __name__ == '__main__':
    gr_unittest.run(qa_pre_decoder_vcvc, "qa_pre_decoder_vcvc.xml")

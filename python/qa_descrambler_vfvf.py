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
from . import lte_test


class qa_descrambler_vfvf(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

        self.tag_key = "subframe"
        msg_buf_name = "sequences"
        reg_len = 8 # 4 REGs complex QPSK modulated, thus 8 float values
        self.cce_len = cce_len = 9 * reg_len
        data = [0] * cce_len
        self.src = blocks.vector_source_f(data, False, cce_len)
        self.descr = lte.descrambler_vfvf(self.tag_key, msg_buf_name, cce_len)
        self.snk = blocks.vector_sink_f(cce_len)

        self.tb.connect(self.src, self.descr, self.snk)

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        print("test_001_t")
        # set up fg
        cell_id = 124
        max_len = 84 * self.cce_len
        n_cce = 20
        pdcch_format = 3

        # set up descrambling block
        seqs = []
        for sub in range(10):
            seq = lte_test.get_pdcch_scrambling_sequence(max_len, cell_id, 2 * sub)
            seqs.append(lte_test.nrz_encoding(seq))
        self.descr.set_descr_seqs(seqs)

        # generate test data
        data = []
        exp_res = []
        for i in range(n_cce):
            pdcch = lte_test.get_pdcch(pdcch_format)
            exp_res.extend(lte_test.nrz_encoding(pdcch))
            scr = lte_test.scramble_pdcch(pdcch, 2 * (i % 10), cell_id)
            data.extend(lte_test.nrz_encoding(scr))
            #data = [0] * self.cce_len * n_cce
        tag_list = lte_test.get_tag_list(n_cce, 10, self.tag_key, "source")
        for i in range(len(tag_list)):
            tag_list[i].offset *= (2 ** pdcch_format)
        self.src.set_data(data, tag_list)

        self.tb.run()
        # check data
        res = self.snk.data()

        #        self.assertFloatTuplesAlmostEqual(res, exp_res)
        for i in range(self.cce_len * n_cce):
        #            print i
            resp = res[i * n_cce:(i + 1) * n_cce]
            expp = exp_res[i * n_cce:(i + 1) * n_cce]
            self.assertFloatTuplesAlmostEqual(resp, expp)

    def test_002_pcfich(self):
        print("pcfich test")
        cfi = 2
        cell_id = 387
        vlen = 32

        # Generate descrambling sequences.
        seqs = []
        for ns in range(10):
            #scr = lte_test.scramble_cfi_sequence([0] * 32, cell_id, ns)
            scr = lte_test.get_pcfich_scrambling_sequence(cell_id, ns)
            seqs.append(lte_test.nrz_encoding(scr))

        # Generate a CFI sequence
        cfi_seq = lte_test.get_cfi_sequence(cfi)
        nrz_cfi_seq = lte_test.nrz_encoding(cfi_seq)

        # Generate scrambled sequences
        nvecs = 100
        scr_cfi_seqs = []
        expres = []
        for ns in range(nvecs):
            scr_cfi_seqs.extend(lte_test.nrz_encoding(lte_test.scramble_cfi_sequence(cfi_seq, cell_id, ns % 10)))
            expres.extend(nrz_cfi_seq)

        # Get tags
        tags = lte_test.get_tag_list(nvecs, 10, self.tag_key, "test_src")

        # Get blocks for test flowgraph
        src = blocks.vector_source_f(scr_cfi_seqs, False, vlen)
        src.set_data(scr_cfi_seqs, tags)
        descr = lte.descrambler_vfvf(self.tag_key, "cfi_seqs", vlen)
        descr.set_descr_seqs(seqs)
        snk = blocks.vector_sink_f(vlen)

        # Create, connect, run fg
        tb = gr.top_block()
        tb.connect(src, descr, snk)
        tb.run()

        # Get result
        res = snk.data()

        # Check results!
        self.assertFloatTuplesAlmostEqual(res, expres)


if __name__ == '__main__':
    gr_unittest.run(qa_descrambler_vfvf)

#!/usr/bin/env python
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
from decode_pdcch_vcvf import decode_pdcch_vcvf
import lte
from lte_test import *
import numpy as np

class qa_decode_pdcch_vcvf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        reg_len = 8 # 4 REGs complex QPSK modulated, thus 8 float values
        self.cce_len = cce_len = 9*reg_len
        self.style = "tx_diversity"
        self.tag_key = "subframe"
        self.msg_cell_id_name = "cell_id"

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # Test number one is supposed to be an integration test!
        # set up fg
        tb2 = gr.top_block()
        cell_id = 124
        N_ant = 2

        cce_len = self.cce_len
        max_len = 84 * self.cce_len
        n_cce = 20
        pdcch_format = 3
        vlen = cce_len/2
        #data = [0] * (vlen)

        # generate test data
        data = []
        exp_res = []
        for i in range(n_cce):
            print "gen {0}".format(i)
            pdcch = get_pdcch(pdcch_format)
            print np.shape(pdcch)
            exp_res.extend(nrz_encoding(pdcch))
            scr = scramble_pdcch(pdcch, 2*(i%10), cell_id)
            print np.shape(scr)
#            data.extend(nrz_encoding(scr))
            mod = qpsk_modulation(scr)
            print np.shape(mod)
#            data.extend(mod)
            n_parts = 2**pdcch_format
            for n in range(n_parts):
                print "pdcch part {0}".format(n)
                part = mod[n*vlen:(n+1)*vlen]
                print np.shape(part)
                lmap = layer_mapping(part, N_ant, self.style)
                print np.shape(lmap)
                code = pre_coding(lmap, N_ant, self.style)
                indat = [code[0][i]+code[1][i] for i in range(len(code[0]))]
                print np.shape(indat)
                lmaporg = []
                for i in range(len(lmap)):
                    lmaporg.extend(lmap[i])
#                print np.shape(lmaporg)
                data.extend(indat)
#            code = pre_coding(lmap, N_ant, self.style)
#            indat = [code[0][i]+code[1][i] for i in range(len(code[0]))]
#            data.extend(indat)

        tag_list = get_tag_list(n_cce, self.tag_key, 10)
        for i in range(len(tag_list)):
            tag_list[i].offset = tag_list[i].offset*(2**pdcch_format)        
        
        print np.shape(data)      

        est = [1] * (vlen) 
        self.src0 = blocks.vector_source_c(data, False, vlen, tag_list)
#        self.srct = blocks.vector_source_f(data, False, cce_len, tag_list)
        self.src1 = blocks.vector_source_c(est, False, vlen)
        self.src2 = blocks.vector_source_c(est, False, vlen)
        self.predecoder = lte.pre_decoder_vcvc(N_ant, vlen, self.style)
        self.demapper = lte.layer_demapper_vcvc(N_ant, vlen, self.style)
        self.demod = lte.qpsk_soft_demod_vcvf(vlen)
        self.descr = lte.descrambler_vfvf(self.tag_key, self.msg_cell_id_name, cce_len)
        # set up descrambling block
        seqs = []
        for sub in range(10):
            seq = get_pdcch_scrambling_sequence(max_len, cell_id, 2*sub)
            seqs.append(nrz_encoding(seq))
        self.descr.set_descr_seqs(seqs)
        self.snk = blocks.vector_sink_f(cce_len)
        
        # connect blocks
        tb2.connect(self.src0, (self.predecoder, 0))
        tb2.connect(self.src1, (self.predecoder, 1))
        tb2.connect(self.src2, (self.predecoder, 2))
        tb2.connect(self.predecoder, self.demapper, self.demod, self.descr, self.snk)
        tb2.run()
        
        res = self.snk.data()
#        self.assertFloatTuplesAlmostEqual(res, exp_res, 5)
        for i in range(self.cce_len * n_cce):
            print i
            resp = res[i*n_cce:(i+1)*n_cce]
            expp = exp_res[i*n_cce:(i+1)*n_cce]
            self.assertFloatTuplesAlmostEqual(resp, expp,5)
        # check data


if __name__ == '__main__':
    #gr_unittest.run(qa_decode_pdcch_vcvf, "qa_decode_pdcch_vcvf.xml")
    gr_unittest.run(qa_decode_pdcch_vcvf)

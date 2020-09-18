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
import pmt
from . import lte_test
import numpy as np
import os

class qa_channel_estimator_vcvc (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

        self.N_rb_dl = N_rb_dl = 50
        self.subcarriers = subcarriers =  12*N_rb_dl
        self.N_ofdm_symbols = N_ofdm_symbols = 140
        self.tag_key = tag_key = "symbol"
        self.msg_buf_name = msg_buf_name = "cell_id"

        rxant = 1

        data = subcarriers * [1]
        pilot_carriers = [[1,2,3],[4,5,6]]
        pilot_symbols = [[1j,2j,3j],[4j,5j,6j]]
        self.src = blocks.vector_source_c(data, False, subcarriers)
        self.estimator = lte.channel_estimator_vcvc(rxant,
                                                    subcarriers,
                                                    tag_key,
                                                    msg_buf_name,
                                                    pilot_carriers,
                                                    pilot_symbols)
        self.snk = blocks.vector_sink_c(subcarriers)

        self.tb.connect(self.src, self.estimator, self.snk)

    def tearDown(self):
        self.tb = None

    def test_001_set_pilot_map(self):
        print("test_001_set_pilot_map BEGIN")
        N_rb_dl = self.N_rb_dl
        cell_id = 124
        Ncp = 1
        [rs_pos_frame, rs_val_frame] = lte_test.frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, 0)
        self.estimator.set_pilot_map(rs_pos_frame, rs_val_frame)
        res_carriers = self.estimator.get_pilot_carriers()
        for i in range(len(res_carriers)):
            self.assertEqual(res_carriers[i], tuple(rs_pos_frame[i]) )
        print("test_001_set_pilot_map END")

    def test_002_t (self):
        print("test_002_t BEGIN")
        N_rb_dl = self.N_rb_dl
        subcarriers = self.subcarriers
        N_ofdm_symbols = self.N_ofdm_symbols
        tag_key = self.tag_key
        msg_buf_name = self.msg_buf_name
        cell_id = 124
        Ncp = 1
        N_ant = 2
        style= "tx_diversity"
        sfn = 0
        srcid = "source"

        print("get and set data")
        stream = self.get_data_stream(N_ant, cell_id, style, N_rb_dl, sfn, subcarriers)
        data_len = len(stream)/subcarriers
        tag_list = lte_test.get_tag_list(data_len, N_ofdm_symbols, tag_key, srcid)
        self.src.set_data(stream, tag_list)

        print("get and set pilot map")
        [rs_pos_frame, rs_val_frame] = lte_test.frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, 0)
        self.estimator.set_pilot_map(rs_pos_frame, rs_val_frame)
        print("pilot Map set")
        self.tb.run ()
        # check data
        #print pmt.pmt_symbol_to_string(tag_list[0].key)
        expected = np.ones((subcarriers,), dtype=np.complex)
        res = self.snk.data()

        failed = 0
        first_failed = -1
        for i in range(len(res)/subcarriers):
            #print i
            vec = res[i*subcarriers:(i+1)*subcarriers]
            try:
                self.assertComplexTuplesAlmostEqual(vec, expected, 5)
                #print "success"
            except:
                print(str(i) +  "\tfail")
                print(vec[0:20])
                first_failed = i
                #print vec
                failed = failed +1
        print("failed vectors: " + str(failed))

        if first_failed > -1:
            i = first_failed
            vec = res[i*subcarriers:(i+1)*subcarriers]
            self.assertComplexTuplesAlmostEqual(vec, expected, 5)

        print("test_002_t END\n\n")

    def test_003_data_gen(self):
        N_rb_dl = self.N_rb_dl
        subcarriers = self.subcarriers
        N_ofdm_symbols = self.N_ofdm_symbols
        tag_key = self.tag_key
        msg_buf_name = self.msg_buf_name
        cell_id = 124
        Ncp = 1
        N_ant = 2
        style= "tx_diversity"
        sfn = 0
        srcid = "source"

        nf = 4
        data = []
        for sfn in range(nf):
            mib = lte_test.pack_mib(N_rb_dl, 0, 1.0, sfn)
            bch = lte_test.encode_bch(mib, N_ant)
            pbch = lte_test.encode_pbch(bch, cell_id, N_ant, style)
            frame = lte_test.generate_phy_frame(cell_id, N_rb_dl, N_ant)
            for p in range(len(frame)):
                frame[p] = lte_test.map_pbch_to_frame_layer(frame[p], pbch[p], cell_id, sfn, p)

            stream = frame[0].flatten().tolist()
            stream = np.add(stream, frame[1].flatten().tolist() )
            data.extend(stream)
        print(len(data))

        tags = lte_test.get_tag_list(140 * nf, 140, self.tag_key, srcid)

        tb2 = gr.top_block ()
        src = blocks.vector_source_c(data, False, subcarriers)
        tb2.run()

    def get_data_stream(self, N_ant, cell_id, style, N_rb_dl, sfn, subcarriers):
        #mib = lte_test.pack_mib(N_rb_dl, 0, 1.0, 511)
        #bch = lte_test.encode_bch(mib, N_ant)
        #pbch = lte_test.encode_pbch(bch, cell_id, N_ant, style)

        framep = lte_test.generate_phy_frame(cell_id, N_rb_dl, N_ant)
        streamp = framep[0].flatten().tolist()
        streamp = np.add(streamp, framep[1].flatten().tolist() )
        symbolp = streamp[0:subcarriers*5]
        streamp = np.append(streamp, symbolp)

        #frame = lte_test.generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant)
        #stream = frame[0].flatten()
        #stream = stream + frame[1].flatten()
        #symbol = stream[0:subcarriers*5]
        #stream = np.append(stream, symbol)

        return streamp


if __name__ == '__main__':
    gr_unittest.run(qa_channel_estimator_vcvc)


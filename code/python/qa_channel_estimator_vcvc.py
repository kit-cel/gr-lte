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

from gnuradio import gr, gr_unittest
from gruel import pmt
#import lte_swig as lte
import lte
from lte_test import *
import os

class qa_channel_estimator_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        self.N_rb_dl = N_rb_dl = 6
        self.subcarriers = subcarriers =  12*N_rb_dl
        self.N_ofdm_symbols = N_ofdm_symbols = 140
        self.tag_key = tag_key = "symbol"
        self.msg_buf_name = msg_buf_name = "cell_id"
        
        data = subcarriers * [1]
        pilot_carriers = [[1,2,3],[4,5,6]]
        pilot_symbols = [[1j,2j,3j],[4j,5j,6j]]
        self.src = gr.vector_source_c(data, False, subcarriers)        
        self.estimator = lte.channel_estimator_vcvc(subcarriers,
                                                    tag_key,
                                                    msg_buf_name,
                                                    pilot_carriers,
                                                    pilot_symbols)
        self.snk = gr.vector_sink_c(subcarriers)
        
        self.tb.connect(self.src, self.estimator, self.snk)

    def tearDown (self):
        self.tb = None
        
    def test_001_symbols(self):
        print "test_001_symbols BEGIN"
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

        [rs_pos_frame, rs_val_frame] = frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, 0)

        com_est = lte.channel_estimator(N_rb_dl)
        com_est.set_cell_id(cell_id)
        rs_vals = com_est.get_frame_rs_symbols(N_rb_dl, cell_id, Ncp)
        
        for i in range(len(rs_vals)):
            self.assertComplexTuplesAlmostEqual(rs_vals[i], rs_val_frame[i])
        print "test_001_symbols END\n\n"
        
    def test_002_positions(self):
        print "test_002_positions BEGIN"
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

        [rs_pos_frame, rs_val_frame] = frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, 0)
        com_est = lte.channel_estimator(N_rb_dl)
        com_est.set_cell_id(cell_id)
        
        failed = 0
        for p in range(2):
            [rs_pos_frame, rs_val_frame] = frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, p)
            rs_pos = com_est.get_frame_rs_positions(p)
            for i in range(len(rs_pos)):
                #print "ant = " + str(p) + "\tsym = " + str(i)
                if len(rs_pos[i]) != len(rs_pos_frame[i]):
                    print "size mismatch!"
                    print "len(rs_pos[i]) = " + str(len(rs_pos[i]))
                    print "len(rs_pos_frame[i]) = " + str(len(rs_pos_frame[i]))
                    failed = failed +1
                elif len(rs_pos[i]) > 0 and len(rs_pos_frame[i]) > 0 and len(rs_pos[i]) == len(rs_pos_frame[i]):
                    try:
                        self.assertEqual(rs_pos[i], tuple(rs_pos_frame[i]) )
                    except:
                        print "unequal"
                        print "len(rs_pos[i]) = " + str(len(rs_pos[i]))
                        print type(rs_pos[i])
                        print type(rs_pos_frame[i])
                        failed = failed +1
        print "faulty vectors = " + str(failed)
        if failed > 0:
            print "\n\n"
            raise Exception("Failed")
        print "test_002_positions END\n\n"
        
    def test_003_set_pilot_map(self):
        print "test_003_set_pilot_map BEGIN"
        N_rb_dl = self.N_rb_dl
        cell_id = 124
        Ncp = 1
        [rs_pos_frame, rs_val_frame] = frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, 0)
        print "got python generated values"
        self.estimator.set_pilot_map(rs_pos_frame, rs_val_frame)
        print "passed values to estimator"
        res_carriers = self.estimator.get_pilot_carriers()
        for i in range(len(res_carriers)):
            print i
            print self.assertEqual(res_carrirers[i], rs_pos_frame[i])
        print res_carriers
        print "test_003_set_pilot_map END"
        

    def test_004_t (self):
        print "test_004_t BEGIN"
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
        
        print "get and set data"
        stream = self.get_data_stream(N_ant, cell_id, style, N_rb_dl, sfn, subcarriers) 
        data_len = len(stream)/subcarriers
        tag_list = self.get_tag_list(data_len, tag_key, N_ofdm_symbols)
        self.src.set_data(stream, tag_list)

        print "get and set pilot map"        
        [rs_pos_frame, rs_val_frame] = frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, 0)
        self.estimator.set_pilot_map(rs_pos_frame, rs_val_frame)
        print "pilot Map set"
        self.tb.run ()
        # check data
        #print pmt.pmt_symbol_to_string(tag_list[0].key)
        expected = np.ones((subcarriers,), dtype=np.complex)
        res = self.snk.data()
        failed = 0
        for i in range(len(res)/subcarriers):
            #print i
            vec = res[i*subcarriers:(i+1)*subcarriers]
            try:
                self.assertComplexTuplesAlmostEqual(vec, expected, 6)
                #print "success"
            except:
                print str(i) +  "\tfail"
                #print vec
                failed = failed +1                
        print "failed vectors: " + str(failed)
        
        print "test_003_t END\n\n"
        

    def get_data_stream(self, N_ant, cell_id, style, N_rb_dl, sfn, subcarriers):
        mib = pack_mib(50,0,1.0, 511)
        bch = encode_bch(mib, N_ant)
        pbch = encode_pbch(bch, cell_id, N_ant, style)
        frame = generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant)
        stream = frame[0].flatten()
        stream = stream + frame[1].flatten()
        symbol = stream[0:subcarriers*5]
        stream = np.append(stream, symbol)
        return stream

    def get_tag_list(self, data_len, tag_key, N_ofdm_symbols):
        tag_list = []
        for i in range(data_len):
                tag = gr.gr_tag_t()
                tag.key = pmt.pmt_string_to_symbol(tag_key)
                tag.srcid = pmt.pmt_string_to_symbol("test_src")
                tag.value = pmt.pmt_from_long(i%N_ofdm_symbols)
                tag.offset = i
                tag_list.append(tag)
        return tag_list
        
        
        
        

"""
    def test_002_ieee80211a(self):
        print "\n\nieee80211a"
        
        data_len = 42
        subcarriers = 52
        tag_key = "symbol"
        msg_buf_name = "cell_id"
        N_frame_syms = 42
        
        
        pilot_pos = [-21,-7,7,21]
        pilot_pos = [pilot_pos[i]+52/2 for i in range(len(pilot_pos))]
        pilot_carriers = [pilot_pos,]*N_frame_syms
        
        pilot_syms = [1, -1, 1, -1]
        pilot_syms = [complex(pilot_syms[i]) for i in range(len(pilot_syms))]
        pilot_sym_vals = [pilot_syms,]*N_frame_syms
        
        #print np.shape(pilot_carriers)
        

        tag_list = []
        for i in range(data_len):
                tag = gr.gr_tag_t()
                tag.key = pmt.pmt_string_to_symbol(tag_key)
                tag.srcid = pmt.pmt_string_to_symbol("test_src")
                tag.value = pmt.pmt_from_long(i%N_frame_syms)
                tag.offset = i
                tag_list.append(tag)
                
        in_data = [0]*subcarriers
        
        #self.src.set_data(in_data, tag_list)
        
        #self.estimator.set_pilot_map(pilot_carriers, pilot_sym_vals)

        #self.tb.run()
        
        res = self.snk.data()
        print res
        print "end test ieee80211a"
"""

if __name__ == '__main__':
    gr_unittest.main()
    #gr_unittest.run(qa_channel_estimator_vcvc, "qa_channel_estimator_vcvc.xml")

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
                                                    N_ofdm_symbols,
                                                    tag_key,
                                                    msg_buf_name,
                                                    pilot_carriers,
                                                    pilot_symbols)
        self.snk = gr.vector_sink_c(subcarriers)
        
        self.tb.connect(self.src, self.estimator, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):        
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
        

                
        

        mib = pack_mib(50,0,1.0, 511)
        bch = encode_bch(mib, N_ant)
        pbch = encode_pbch(bch, cell_id, N_ant, style)
        frame = generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant)
        stream = frame[0].flatten()
        stream = stream + frame[1].flatten()
        symbol = stream[0:subcarriers*5]
        stream = np.append(stream, symbol)
        
        data_len = len(stream)/subcarriers
        #print data_len
        tag_list = []
        for i in range(data_len):
                tag = gr.gr_tag_t()
                tag.key = pmt.pmt_string_to_symbol(tag_key)
                tag.srcid = pmt.pmt_string_to_symbol("test_src")
                tag.value = pmt.pmt_from_long(i%N_ofdm_symbols)
                tag.offset = i
                tag_list.append(tag)
        

        self.src.set_data(stream, tag_list)
        
        [rs_pos_frame, rs_val_frame] = frame_pilot_value_and_position(N_rb_dl, cell_id, Ncp, 0)
        self.estimator.set_pilot_map(rs_pos_frame, rs_val_frame)
        

        self.tb.run ()
        # check data
        #print pmt.pmt_symbol_to_string(tag_list[0].key)
        expected = np.ones((subcarriers,), dtype=np.complex)
        res = self.snk.data()

        for i in range(len(res)/subcarriers):
            #print i
            vec = res[i*subcarriers:(i+1)*subcarriers]
            try:
                self.assertComplexTuplesAlmostEqual(vec, expected, 6)
                #print "success"
            except:
                print str(i) +  "\tfail"
                #print vec
                m= 0




if __name__ == '__main__':
    gr_unittest.main()
    #gr_unittest.run(qa_channel_estimator_vcvc, "qa_channel_estimator_vcvc.xml")

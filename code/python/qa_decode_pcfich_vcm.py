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
from decode_pcfich_vcm import decode_pcfich_vcm
import lte
from lte_test import *

class qa_decode_pcfich_vcm (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        self.N_rb_dl = 50
        self.key = "symbol"
        self.out_key = "subframe"
        self.msg_cell_id_name = "cell_id"
        self.msg_N_ant_name = "N_ant"
        self.msg_cfi_name = "CFI"
        vlen = 12 * self.N_rb_dl
        est = data = [0] * vlen
        self.src0 = blocks.vector_source_c(data, False, vlen)
        self.src1 = blocks.vector_source_c(est, False, vlen)
        self.src2 = blocks.vector_source_c(est, False, vlen)
        
        self.pcfich = lte.decode_pcfich_vcm(self.N_rb_dl, self.key, self.out_key, self.msg_cell_id_name, self.msg_N_ant_name, self.msg_cfi_name)
        
        self.tb.connect(self.src0, (self.pcfich, 0) )
        self.tb.connect(self.src1, (self.pcfich, 1) )
        self.tb.connect(self.src2, (self.pcfich, 2) )
        # pcfich is not connected to any output because its message port is optional.
        # only testing correct screen print
        # tests don't stop if message queue is in use :(
        

    def tearDown (self):
        self.tb = None
        
    def test_001_parts(self):
        print "test 001 PARTS!!!"
        # This test is for integration
        # all parts expect Demux are tested if they decode CFI correctly
        
        self.tb2 = gr.top_block ()
        
        cell_id = 124
        N_ant = 2
        style = "tx_diversity"
        cvlen = 16
        key = "subframe"
        msg_buf_name = "cfi"
        cfi = 1
        
        data = []
        exp_pc = []
        for ns in range(10):
            cfi_seq = get_cfi_sequence(cfi)
            scr = scramble_cfi_sequence(cfi_seq, cell_id, 2*ns)
            mod = qpsk_modulation(scr)
            lay = layer_mapping(mod, N_ant, style)
            prc = pre_coding(lay, N_ant, style)
            prc_prep = [prc[0][i]+prc[1][i] for i in range(len(prc[0]))]
            #print np.shape(prc)
            prep = prepare_for_demapper_block(lay, N_ant, style)
            exp_pc.extend(prep)
            data.extend(prc_prep)
              
        taglist = get_tag_list(len(data)/16, key, 10)
        self.src = blocks.vector_source_c(data, False, cvlen, taglist)
      
        h0 = [complex(1,0)]*len(data)
        h1 = [complex(1,0)]*len(data)
        self.src1 = blocks.vector_source_c(h0, False, cvlen)
        self.src2 = blocks.vector_source_c(h1, False, cvlen)
        self.predecoder = lte.pre_decoder_vcvc(N_ant, cvlen, style)
        self.snk = blocks.vector_sink_c(cvlen)
        self.demapper = lte.layer_demapper_vcvc(N_ant, cvlen, style)
        self.qpsk = lte.qpsk_soft_demod_vcvf(cvlen)
        self.descr = lte.pcfich_descrambler_vfvf(key, msg_buf_name)
        self.descr.set_cell_id(cell_id)
        self.cfi = lte.cfi_unpack_vf(key, msg_buf_name)
        
        self.tb2.connect(self.src, (self.predecoder, 0))
        self.tb2.connect(self.src1, (self.predecoder, 1))
        self.tb2.connect(self.src2, (self.predecoder, 2))
        self.tb2.connect(self.predecoder, self.demapper)
        self.tb2.connect(self.predecoder, self.snk)
        self.tb2.connect(self.demapper, self.qpsk, self.descr, self.cfi)
        
        self.tb2.run()
        print "test 001 PARTS FINISHED!!!\n"
        

    def test_002_t (self):
        print "\ntest 002 hier block test"
        # set up fg
        cell_id = 124
        N_ant = 2
        style= "tx_diversity"
        N_rb_dl = self.N_rb_dl
        
        # generate test data frame
        mib = pack_mib(50,0,1.0, 511)
        bch = encode_bch(mib, N_ant)
        pbch = encode_pbch(bch, cell_id, N_ant, style)
        frame = generate_frame(pbch, N_rb_dl, cell_id, 20, N_ant)

        
        stream = generate_stream_frame(frame, N_ant)
        
        print len(stream)
        syms = len(stream)/ (12*self.N_rb_dl)
        print syms
        taglist = get_tag_list(syms, self.key, 140)
        
        # no messages sent, set cell_id here
#        self.pcfich.demux.set_cell_id(cell_id)
#        self.pcfich.descrambler.set_cell_id(cell_id)
        self.pcfich.set_cell_id(cell_id)
        self.pcfich.set_N_ant(N_ant)
        
        # set generated test data
        est = [complex(1,0)] * len(stream)
        self.src0.set_data(stream, taglist)
        self.src1.set_data(est)
        self.src2.set_data(est)
        
        print "run test!!!"
        self.tb.run ()
        print "test finish"
        # check data        
        


if __name__ == '__main__':
    #gr_unittest.run(qa_decode_pcfich_vcm, "qa_decode_pcfich_vcm.xml")
    gr_unittest.run(qa_decode_pcfich_vcm)
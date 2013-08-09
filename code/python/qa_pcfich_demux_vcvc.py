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
import lte
from lte_test import *

class qa_pcfich_demux_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        self.N_rb_dl = N_rb_dl = 6
        self.key = "symbol"
        self.out_key = "subframe"
        self.msg_buf_name = "cell_id"
        n_carriers = 12*N_rb_dl
        n_cfi_vec = 16
        intu = np.zeros(n_carriers,dtype=np.complex)
        
        self.src0 = blocks.vector_source_c( intu, False, n_carriers)
        self.src1 = blocks.vector_source_c( intu, False, n_carriers)
        self.src2 = blocks.vector_source_c( intu, False, n_carriers)
        
        self.demux = lte.pcfich_demux_vcvc(N_rb_dl, self.key, self.out_key, self.msg_buf_name)
                
        self.snk0 = blocks.vector_sink_c(n_cfi_vec)
        self.snk1 = blocks.vector_sink_c(n_cfi_vec)
        self.snk2 = blocks.vector_sink_c(n_cfi_vec)
        
        self.tag = blocks.tag_debug(n_cfi_vec * 8, "TAG")
        
        self.tb.connect(self.src0, (self.demux,0) )
        self.tb.connect( (self.demux,0), self.snk0)
        self.tb.connect(self.src1, (self.demux,1) )
        self.tb.connect( (self.demux,1), self.snk1)
        self.tb.connect(self.src2, (self.demux,2) )
        self.tb.connect( (self.demux,2), self.snk2)
        self.tb.connect( (self.demux,0), self.tag)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        cell_id = 124
        N_ant = 2
        style= "tx_diversity"
        N_rb_dl = self.N_rb_dl
        sfn = 0
        Ncp = 1

        mib = pack_mib(50,0,1.0, 511)
        bch = encode_bch(mib, N_ant)
        pbch = encode_pbch(bch, cell_id, N_ant, style)
        
        # FIX ME, where do these parameters come from?
        pcfich = encode_pcfich(2, cell_id, 0, N_ant)
        exp_res = pcfich[0]

        frame = generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant)
        data = frame[0].flatten()
        taglist = get_tag_list(len(frame[0]), self.key, 140)
        self.src0.set_data(data, taglist)
        self.src1.set_data(data)
        self.src2.set_data(data)
        
        self.demux.set_cell_id(cell_id)
        #print calculate_pcfich_pos(N_rb_dl, cell_id)
        # set up fg
        self.tb.run ()
        # check data
        
        res = self.snk0.data()
        
        for i in range(len(res)/len(exp_res)):
            part = res[i*16:(i+1)*16]
            self.assertComplexTuplesAlmostEqual(part, exp_res)


if __name__ == '__main__':
    #gr_unittest.run(qa_pcfich_demux_vcvc, "qa_pcfich_demux_vcvc.xml")
    gr_unittest.run(qa_pcfich_demux_vcvc)
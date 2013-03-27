#!/usr/bin/env python
# 
# Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
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
#

from gnuradio import gr, gr_unittest
import lte as lte_swig
import numpy as np
import scipy.io
from lte_test import *

class qa_pbch_demux_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        intu = np.zeros(72,dtype=np.complex)
        
        self.src1 = gr.vector_source_c( intu, False, 72)
        self.src2 = gr.vector_source_c( intu, False, 72)
        self.src3 = gr.vector_source_c( intu, False, 72)
        
        #cell_id = 124
        N_rb_dl = 6
        self.demux = lte_swig.pbch_demux_vcvc(N_rb_dl) # cell_id,
        
        
        self.snk1 = gr.vector_sink_c(240)
        self.snk2 = gr.vector_sink_c(240)
        self.snk3 = gr.vector_sink_c(240)
        
        self.tb.connect(self.src1,(self.demux,0) )
        self.tb.connect( (self.demux,0),self.snk1)
        self.tb.connect(self.src2,(self.demux,1) )
        self.tb.connect( (self.demux,1),self.snk2)
        self.tb.connect(self.src3,(self.demux,2) )
        self.tb.connect( (self.demux,2),self.snk3)
        
        

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        cell_id = 125
        N_ant = 2
        style= "tx_diversity"
        N_rb_dl = 6
        sfn = 0
        self.demux.set_cell_id(cell_id)
        
        mib = pack_mib(50,0,1.0, 511)
        bch = encode_bch(mib, N_ant)
        pbch = encode_pbch(bch, cell_id, N_ant, style)
        
        stream = []
        for i in range(4):
            frame = generate_frame(pbch, N_rb_dl, cell_id, i+16, N_ant)
            stream.extend(frame[0].flatten())
            
        print len(stream)
        #frame = generate_frame(pbch, N_rb_dl, cell_id, sfn, N_ant)
        #stream = frame[0].flatten()
        #print np.shape(frame[0].flatten())
        self.src1.set_data(stream)
        self.src2.set_data(np.ones(len(stream), dtype=np.complex))
        self.src3.set_data(np.ones(len(stream), dtype=np.complex))
        # set up fg
        self.tb.run ()
        # check data
        res1 = self.snk1.data()
        res2 = self.snk2.data()
        res3 = self.snk3.data()
        
        print len(res1)
        self.assertComplexTuplesAlmostEqual(res1,tuple(pbch[0]))
        


if __name__ == '__main__':
    gr_unittest.main ()

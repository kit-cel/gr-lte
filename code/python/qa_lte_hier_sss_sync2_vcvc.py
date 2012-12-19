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
import lte

class qa_hier_sss_sync2_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        fftl = 2048
        N_rb_dl = 6
        
        # These blocks are just "dummy" blocks for this QA
        self.eq = lte.linear_OFDM_estimator_vcvc(N_rb_dl)
        self.demux = lte.pbch_demux_vcvc(N_rb_dl)
        self.descr = lte.descrambling_vfvf()
        self.daemon = lte.cell_id_daemon(self.eq, self.demux, self.descr)
        
        # Source 'n' Sink
        data = range(2*fftl)
        self.src = gr.vector_source_c(data, False, fftl)
        self.snk = gr.vector_sink_c(fftl)
        self.tagsnk = gr.tag_debug(gr.sizeof_gr_complex*fftl, "SYMBOL TAG")
        # This is the UUT
        self.sss = lte.hier_sss_sync2_vcvc(self.daemon, fftl)
        
        self.tb.connect(self.src, self.sss, self.snk)
        self.tb.connect(self.sss, self.tagsnk)

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data


if __name__ == '__main__':
    gr_unittest.main ()

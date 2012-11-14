#!/usr/bin/env python
# 
# Copyright 2012 Johannes Demel
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
import lte_swig

class qa_cell_id_daemon (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        
        N_rb_dl = 6
        
        self.eq   = lte_swig.linear_OFDM_equalizer_vcvc(N_rb_dl)
        self.demux= lte_swig.pbch_demux_vcvc(N_rb_dl)
        self.ld   = lte_swig.descrambling_vfvf()
        self.daemon = lte_swig.cell_id_daemon(self.eq,self.demux,self.ld)
        
        self.daemon.set_cell_id(124)
        
        

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data


if __name__ == '__main__':
    gr_unittest.main ()

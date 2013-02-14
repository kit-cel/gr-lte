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
import lte_test

class qa_layer_demapper_vcvc (gr_unittest.TestCase):

    def setUp (self):
        print "setUp"
        self.tb = gr.top_block ()
        self.src = gr.vector_source_c([0]*240,False,240)
        self.demapper = lte_swig.layer_demapper_vcvc(0, "tx_diversity")
        self.snk = gr.vector_sink_c(240)
        self.tb.connect(self.src, self.demapper, self.snk)

    def tearDown (self):
        self.tb = None

    def test_001_setup_N_ant(self):
        print "\ntest_001_setup_N_ant"
        N_ant = [0,1,2,4]

        for n in range(len(N_ant)):
            self.demapper.set_N_ant(N_ant[n])
            self.assertEqual(N_ant[n], self.demapper.get_N_ant())

    def test_002_setup_decoding_style(self):
        print "\ntest_002_setup_decoding_style"
        style = {0:"tx_diversity", 1:"spatial_multiplexing", 2:"nonsense"}
        
        for s in range(len(style)):
            self.demapper.set_decoding_style(style[s])
            try:
                self.assertEqual(style[0], self.demapper.get_decoding_style() )
            except:
                if(self.demapper.get_decoding_style() != style[0]):
                    print style[s]
                else:
                    self.assertEqual(style[1], layer_demapper.get_decoding_style() )
        

    def test_003_demapping (self):
        print "\ntest_003_demapping"
        N_ant = 1
        cell_id = 124
        mib = lte_test.pack_mib(50,0,1.0, 511)
        bch = tuple(lte_test.encode_bch(mib, N_ant))
        data = lte_test.pbch_scrambling(bch, cell_id)
        style = "tx_diversity"
        self.demapper.set_decoding_style(style)
        
        mapped = [[],[]]
        mapped[0] = lte_test.layer_mapping(data, 1 , style)
        m2 = lte_test.layer_mapping(data, 2 , style)
        mapped_interleaved = []
        for i in range(len(m2[0])):
            mapped_interleaved.extend([m2[0][i], m2[1][i]])
        mapped[1] = mapped_interleaved
        
        exp_res = [complex(data[i]) for i in range(len(data))]
        print len(exp_res)
        print type(exp_res)
        print type(exp_res[0])
        
        print "run"
        for i in range(2):
            self.demapper.set_N_ant(i+1)
            print "N_ant = " +str(self.demapper.get_N_ant())
            self.src.set_data(mapped[i])
            self.snk.reset()
            self.tb.run()
            res = self.snk.data()
            try:
                self.assertComplexTuplesAlmostEqual(res, tuple(exp_res) )
            except:
                print "FAILED N_ant = " +str(self.demapper.get_N_ant())
                self.assertComplexTuplesAlmostEqual(res, tuple(exp_res) )


            
if __name__ == '__main__':
    gr_unittest.main ()

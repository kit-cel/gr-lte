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

from gnuradio import gr, gr_unittest, trellis, window,blks2
import lte_swig
import lte
import numpy
import scipy.io
import time
import os
from pylab import *

class qa_multi_block_test (gr_unittest.TestCase):

    def setUp (self):
    
        #print os.getpid()
        #raw_input("Press the ANY key to continue")
        setuptime = time.clock()
    
        self.tb = gr.top_block ()
        
        print "\nqa_multi_block_test START"
 

        offset =  1133
        fftl = 512
        cell_id = 124
        N_rb_dl = 6
        I = 768
        D = 1000

        # This first part is for syncing!
        #self.src  = gr.vector_source_c(intu,False,1)
        
        infile1 = '/home/demel/Dokumente/Messungen/Messung_LTE_2012-05-23_12:48:19.dat' # successful!
        infile2 = '/home/demel/Dokumente/Messungen/Messung_LTE_2012-05-23_12:48:07.dat' # failed!
        infile3 = '/home/demel/Dokumente/Messungen/Messung_LTE_2012-05-23_12:46:30.dat' # successful!
        infile4 = '/home/johannes/lte/data/Messung_LTE_2012-05-23_12:47:32.dat' # successful!
        infile5 = '/home/demel/Dokumente/Messungen/Messung_LTE_2012-05-23_12:49:57.dat' # successful!
        
        samps = (fftl*7*2*10+10*fftl)*300 +offset
        input_file = infile3
        print input_file
        
        self.src = gr.file_source(gr.sizeof_gr_complex,input_file, False)
        self.resample = blks2.rational_resampler_ccc(I, D)
        
        self.head1 = gr.head(gr.sizeof_gr_complex, samps)
        #self.tag  = lte_swig.tag_symbol_cc(offset,fftl)
        self.sync = lte_swig.cp_time_freq_sync_cc(fftl)
        self.pss = lte.pss_sync_hier_cc(fftl)
        self.est = lte.hier_freq_estimate_cc(fftl)
        # This is the actual place to initialize self.sss
        # Nevertheless it it the last block to be initialized because it needs pointers to some others.
        # self.sss = lte.hier_sss_sync_cc(fftl)
        
        #This part does still process all data (more ore less)
        self.rcp  = lte_swig.remove_cp_cvc(fftl)        
        self.fft  = gr.fft_vcc(fftl,True,window.rectangular(fftl),False,1)
        self.ext  = lte_swig.extract_occupied_tones_vcvc(N_rb_dl,fftl)
        
        self.eq   = lte.linear_OFDM_equalizer_hier_vcvc(N_rb_dl)#cell_id,
        #self.eq.set_cell_id(cell_id)
        #self.sh1  = gr.skiphead(gr.sizeof_gr_complex*12*N_rb_dl,7)
        #self.sh2  = gr.skiphead(gr.sizeof_gr_complex*12*N_rb_dl,7)
        #self.sh3  = gr.skiphead(gr.sizeof_gr_complex*12*N_rb_dl,7)
        
        # After the next block only the PBCH is processed!
        self.demux= lte_swig.pbch_demux_vcvc(N_rb_dl)#cell_id,
        #self.demux.set_cell_id(cell_id)
        self.pd1  = lte_swig.pre_decoder_vcvc(1,'tx_diversity')
        self.pd2  = lte_swig.pre_decoder_vcvc(2,'tx_diversity')
        self.ldm1 = lte_swig.layer_demapper_vcvc(1,'tx_diversity')
        self.ldm2 = lte_swig.layer_demapper_vcvc(2,'tx_diversity')
        self.int  = gr.interleave(240*8)
        self.dmd  = lte_swig.qpsk_soft_demod_vcvf()
       	self.ld   = lte_swig.descrambling_vfvf()
       	#self.ld.set_cell_id(cell_id)
        self.lru  = lte_swig.rate_unmatch_vff()
        

        # After Viterbi decoding, only bits are processed!
        self.vit  = lte.viterbi_vfvb()
        self.crc  = lte_swig.crc_calculator_vbvb()
        self.mib  = lte_swig.mib_unpack_vb()
        
        #############################################################
        # This last block is just a daemon to propagate the calculated cell id
        #############################################################
        
        self.daemon = lte.cell_id_daemon(self.eq.get_eq(),self.demux, self.ld)
        self.sss = lte.hier_sss_sync_cc(self.daemon, fftl)
        
        # declaration of additional sink blocks for tests
        #self.snk1 = gr.vector_sink_f(120)
        #self.snk2 = gr.vector_sink_b(40)
        
        
        # Connect all blocks together!
        self.tb.connect(self.src,self.resample,self.head1,self.sync,self.pss,self.est,self.sss,self.rcp,self.fft,self.ext,self.eq)#
        
        self.tb.connect( (self.eq,0), (self.demux,0) )
        self.tb.connect( (self.eq,1), (self.demux,1) )
        self.tb.connect( (self.eq,2), (self.demux,2) )
        
        self.tb.connect( (self.demux,0) ,(self.pd1,0) )
        self.tb.connect( (self.demux,1) ,(self.pd1,1) )
        self.tb.connect( (self.demux,0) ,(self.pd2,0) )
        self.tb.connect( (self.demux,1) ,(self.pd2,1) )
        self.tb.connect( (self.demux,2) ,(self.pd2,2) )
        self.tb.connect(self.pd1,self.ldm1,(self.int,0) )
        self.tb.connect(self.pd2,self.ldm2,(self.int,1) )
        self.tb.connect(self.int,self.dmd,self.ld,self.lru,self.vit,self.crc)
        self.tb.connect( (self.crc,0),(self.mib,0) )
        self.tb.connect( (self.crc,1),(self.mib,1) )
        
        # additional sink blocks for testing
        #self.tb.connect(self.lru,self.snk1)
        #self.tb.connect(self.vit,self.snk2)
        
        print "\n\nsetuptime = " + str(time.clock() - setuptime) + "\n\n"

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        
        # set up fg
        myruntime = time.clock()
        self.tb.run ()
        print "flowgraph runtime = " + str(time.clock() - myruntime)
        
        # check data
        sfn_vec = self.mib.get_SFN_vec()
        #rate = (len(sfn_vec)*1.0) / ( ((sfn_vec[len(sfn_vec)-1]-sfn_vec[0]))*1.0 )
        rate = 0
        
        print "\nSSS sync block"
        print "cell_id   = " + str(self.sss.get_cell_id())
        
        print "\nstatic MIB information"
        print "N_ant     = " + str(self.mib.get_N_ant())
        print "N_rb_dl   = " + str(self.mib.get_N_rb_dl())
        print "phich_dur = " + str(self.mib.get_phich_dur())
        print "phich_res = " + str(self.mib.get_phich_res())
        
        print "\nperiodic MIB information"
        print "last SFN  = " + str(self.mib.get_SFN())
        print "mib_count = " + str(self.mib.get_mib_count())
        print "rate      = " + str(rate)
        print "SFN vec   = " + str(sfn_vec)
        
        
       
        corr_vec = self.sss.calc.get_corr_vec()
        plot(corr_vec)
        show()
        
        
          
        
        # next line marks end of test!
        print "\nqa_multi_block_test END"

if __name__ == '__main__':
    gr_unittest.main ()

#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: lte_mimo_pss_sync
# Author: Kristian Maier
# Description: hier block for pss time sync
# Generated: Mon Jun 30 01:26:13 2014
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import lte
import wx

class top_block(grc_wxgui.top_block_gui):

    def __init__(self, fftlen=1024):
        grc_wxgui.top_block_gui.__init__(self, title="lte_mimo_pss_sync")

        ##################################################
        # Parameters
        ##################################################
        self.fftlen = fftlen

        ##################################################
        # Variables
        ##################################################
        self.taps = taps = filter.optfir.low_pass(1, fftlen*15e3, 472.5e3, 960e3, 0.2, 20)
        self.synclen = synclen = 2
        self.samp_rate = samp_rate = 32000

        ##################################################
        # Blocks
        ##################################################
        self.lte_mimo_pss_tagger_0 = lte.mimo_pss_tagger(fftlen)
        self.lte_mimo_pss_fine_sync_1 = lte.mimo_pss_fine_sync(fftlen)
        self.lte_mimo_pss_coarse_sync_0 = lte.mimo_pss_coarse_sync(synclen)
        self.lte_mimo_pss_coarse_control_0 = lte.mimo_pss_coarse_control()
        self.fir_filter_xxx_0_0 = filter.fir_filter_ccc(fftlen/64, (taps))
        self.fir_filter_xxx_0_0.declare_sample_delay(0)
        self.fir_filter_xxx_0 = filter.fir_filter_ccc(fftlen/64, (taps))
        self.fir_filter_xxx_0.declare_sample_delay(0)
        self.blocks_vector_to_streams_0 = blocks.vector_to_streams(gr.sizeof_gr_complex*1, 2)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*2, 1e6,True)
        self.blocks_null_sink_1 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*2, "/home/kris/gr-lte/test/lte5framesFadingChannelETU_vec2.dat", False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.blocks_vector_to_streams_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 0), (self.lte_mimo_pss_tagger_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 1), (self.lte_mimo_pss_tagger_0, 1))
        self.connect((self.blocks_vector_to_streams_0, 0), (self.lte_mimo_pss_coarse_control_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 1), (self.lte_mimo_pss_coarse_control_0, 1))
        self.connect((self.lte_mimo_pss_tagger_0, 1), (self.blocks_null_sink_1, 0))
        self.connect((self.lte_mimo_pss_tagger_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.lte_mimo_pss_coarse_control_0, 1), (self.fir_filter_xxx_0_0, 0))
        self.connect((self.lte_mimo_pss_coarse_control_0, 0), (self.fir_filter_xxx_0, 0))
        self.connect((self.fir_filter_xxx_0, 0), (self.lte_mimo_pss_coarse_sync_0, 0))
        self.connect((self.fir_filter_xxx_0_0, 0), (self.lte_mimo_pss_coarse_sync_0, 1))
        self.connect((self.blocks_vector_to_streams_0, 0), (self.lte_mimo_pss_fine_sync_1, 0))
        self.connect((self.blocks_vector_to_streams_0, 1), (self.lte_mimo_pss_fine_sync_1, 1))

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "N_id_2", self.lte_mimo_pss_tagger_0, "N_id_2")
        self.msg_connect(self.lte_mimo_pss_fine_sync_1, "half_frame", self.lte_mimo_pss_tagger_0, "half_frame")
        self.msg_connect(self.lte_mimo_pss_fine_sync_1, "lock", self.lte_mimo_pss_tagger_0, "lock")
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "N_id_2", self.lte_mimo_pss_fine_sync_1, "N_id_2")
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "coarse_pos", self.lte_mimo_pss_fine_sync_1, "coarse_pos")
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "control", self.lte_mimo_pss_coarse_control_0, "control")


    def get_fftlen(self):
        return self.fftlen

    def set_fftlen(self, fftlen):
        self.fftlen = fftlen
        self.set_taps(filter.optfir.low_pass(1, self.fftlen*15e3, 472.5e3, 960e3, 0.2, 20))

    def get_taps(self):
        return self.taps

    def set_taps(self, taps):
        self.taps = taps
        self.fir_filter_xxx_0.set_taps((self.taps))
        self.fir_filter_xxx_0_0.set_taps((self.taps))

    def get_synclen(self):
        return self.synclen

    def set_synclen(self, synclen):
        self.synclen = synclen

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option("", "--fftlen", dest="fftlen", type="intx", default=1024,
        help="Set FFT length [default=%default]")
    (options, args) = parser.parse_args()
    tb = top_block(fftlen=options.fftlen)
    tb.Start(True)
    tb.Wait()

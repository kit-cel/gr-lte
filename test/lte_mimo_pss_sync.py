#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: LTE MIMO PSS sync
# Author: Kristian Maier
# Description: hier block for pss time sync
# Generated: Mon Jun 30 16:25:03 2014
##################################################

from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import lte

class lte_mimo_pss_sync(gr.top_block):

    def __init__(self, fftlen=1024):
        gr.top_block.__init__(self, "LTE MIMO PSS sync")

        ##################################################
        # Parameters
        ##################################################
        self.fftlen = fftlen

        ##################################################
        # Variables
        ##################################################
        self.taps = taps = filter.optfir.low_pass(1, fftlen*15e3, 472.5e3, 960e3, 0.2, 20)
        self.synclen = synclen = 2

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

        ##################################################
        # Connections
        ##################################################
        self.connect((self.lte_mimo_pss_tagger_0, 1), (self, 0))
        self.connect((self.lte_mimo_pss_tagger_0, 0), (self, 0))
        self.connect((self.lte_mimo_pss_coarse_control_0, 1), (self.fir_filter_xxx_0_0, 0))
        self.connect((self.lte_mimo_pss_coarse_control_0, 0), (self.fir_filter_xxx_0, 0))
        self.connect((self.fir_filter_xxx_0, 0), (self.lte_mimo_pss_coarse_sync_0, 0))
        self.connect((self.fir_filter_xxx_0_0, 0), (self.lte_mimo_pss_coarse_sync_0, 1))
        self.connect((self, 0), (self.lte_mimo_pss_tagger_0, 0))
        self.connect((self, 0), (self.lte_mimo_pss_tagger_0, 1))
        self.connect((self, 0), (self.lte_mimo_pss_fine_sync_1, 0))
        self.connect((self, 0), (self.lte_mimo_pss_fine_sync_1, 1))
        self.connect((self, 0), (self.lte_mimo_pss_coarse_control_0, 0))
        self.connect((self, 0), (self.lte_mimo_pss_coarse_control_0, 1))

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.msg_connect(self.lte_mimo_pss_fine_sync_1, "half_frame", self.lte_mimo_pss_tagger_0, "half_frame")
        self.msg_connect(self.lte_mimo_pss_fine_sync_1, "lock", self.lte_mimo_pss_tagger_0, "lock")
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "N_id_2", self.lte_mimo_pss_fine_sync_1, "N_id_2")
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "coarse_pos", self.lte_mimo_pss_fine_sync_1, "coarse_pos")
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "control", self.lte_mimo_pss_coarse_control_0, "control")
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "N_id_2", self.lte_mimo_pss_tagger_0, "N_id_2")

# QT sink close method reimplementation

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

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option("", "--fftlen", dest="fftlen", type="intx", default=1024,
        help="Set FFT length [default=%default]")
    (options, args) = parser.parse_args()
    tb = lte_mimo_pss_sync(fftlen=options.fftlen)
    tb.start()
    raw_input('Press Enter to quit: ')
    tb.stop()
    tb.wait()


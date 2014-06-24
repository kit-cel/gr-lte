#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: lte_pss_mimo_sync
# Author: Kristian Maier
# Description: hier block for pss time sync
# Generated: Tue Jun 24 14:35:58 2014
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import lte

class top_block(gr.top_block):

    def __init__(self, fftlen=1024):
        gr.top_block.__init__(self, "lte_pss_mimo_sync")

        ##################################################
        # Parameters
        ##################################################
        self.fftlen = fftlen

        ##################################################
        # Variables
        ##################################################
        self.taps_fftl_1024 = taps_fftl_1024 = [-0.00141422616403692464996655342446274517, 0.015642606095112098624211327546618122142, 0.030243226636871359258584845974837662652, 0.052594337944074170954067426464462187141, 0.078507642615830444676738864018261665478, 0.103921182343643836443192185470252297819, 0.124279358836934944454455376217083539814, 0.135614264806182049927230082175810821354, 0.135614264806182049927230082175810821354, 0.124279358836934944454455376217083539814, 0.103921182343643836443192185470252297819, 0.078507642615830444676738864018261665478, 0.052594337944074170954067426464462187141,0.030243226636871359258584845974837662652,0.015642606095112098624211327546618122142,-0.00141422616403692464996655342446274517]
        self.synclen = synclen = 2
        self.samp_rate = samp_rate = 32000

        ##################################################
        # Blocks
        ##################################################
        self.lte_mimo_pss_coarse_sync_0 = lte.mimo_pss_coarse_sync(synclen)
        self.fir_filter_xxx_0_0 = filter.fir_filter_ccc(fftlen/128, (taps_fftl_1024))
        self.fir_filter_xxx_0_0.declare_sample_delay(0)
        (self.fir_filter_xxx_0_0).set_min_output_buffer(9800)
        self.fir_filter_xxx_0 = filter.fir_filter_ccf(fftlen/128, (taps_fftl_1024))
        self.fir_filter_xxx_0.declare_sample_delay(0)
        (self.fir_filter_xxx_0).set_min_output_buffer(9800)
        self.blocks_vector_to_streams_0 = blocks.vector_to_streams(gr.sizeof_gr_complex*1, 2)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*2, samp_rate*100,True)
        self.blocks_head_0_0 = blocks.head(gr.sizeof_gr_complex*2, 1550000)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*2, "/home/maier/Schreibtisch/lte5framesFadingChannelETU.dat", False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_head_0_0, 0), (self.blocks_vector_to_streams_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.blocks_head_0_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 1), (self.fir_filter_xxx_0_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 0), (self.fir_filter_xxx_0, 0))
        self.connect((self.fir_filter_xxx_0, 0), (self.lte_mimo_pss_coarse_sync_0, 0))
        self.connect((self.fir_filter_xxx_0_0, 0), (self.lte_mimo_pss_coarse_sync_0, 1))


# QT sink close method reimplementation

    def get_fftlen(self):
        return self.fftlen

    def set_fftlen(self, fftlen):
        self.fftlen = fftlen

    def get_taps_fftl_1024(self):
        return self.taps_fftl_1024

    def set_taps_fftl_1024(self, taps_fftl_1024):
        self.taps_fftl_1024 = taps_fftl_1024
        self.fir_filter_xxx_0.set_taps((self.taps_fftl_1024))
        self.fir_filter_xxx_0_0.set_taps((self.taps_fftl_1024))

    def get_synclen(self):
        return self.synclen

    def set_synclen(self, synclen):
        self.synclen = synclen

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate*100)

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option("", "--fftlen", dest="fftlen", type="intx", default=1024,
        help="Set FFT length [default=%default]")
    (options, args) = parser.parse_args()
    tb = top_block(fftlen=options.fftlen)
    tb.start()
    raw_input('Press Enter to quit: ')
    tb.stop()
    tb.wait()


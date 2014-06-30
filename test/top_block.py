#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Mon Jun 30 16:29:27 2014
##################################################

execfile("/home/maier/.grc_gnuradio/lte_mimo_pss_sync.py")
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser

class top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")

        ##################################################
        # Variables
        ##################################################
        self.fftl = fftl = 1024
        self.samp_rate = samp_rate = fftl*15e3

        ##################################################
        # Blocks
        ##################################################
        self.lte_mimo_pss_sync_0 = lte_mimo_pss_sync(
            fftlen=fftl,
        )
        self.blocks_vector_to_streams_0 = blocks.vector_to_streams(gr.sizeof_gr_complex*1, 2)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*2, samp_rate,True)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*2, "/home/maier/gr-lte/test/lte5framesFadingChannelETU_vec2.dat", False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.blocks_vector_to_streams_0, 0))
        self.connect((self.lte_mimo_pss_sync_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.lte_mimo_pss_sync_0, 1), (self.blocks_null_sink_0, 1))
        self.connect((self.blocks_vector_to_streams_0, 0), (self.lte_mimo_pss_sync_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 1), (self.lte_mimo_pss_sync_0, 1))


# QT sink close method reimplementation

    def get_fftl(self):
        return self.fftl

    def set_fftl(self, fftl):
        self.fftl = fftl
        self.set_samp_rate(self.fftl*15e3)
        self.lte_mimo_pss_sync_0.set_fftlen(self.fftl)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    tb = top_block()
    tb.start()
    raw_input('Press Enter to quit: ')
    tb.stop()
    tb.wait()


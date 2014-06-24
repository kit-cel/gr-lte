#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Thu Jun 12 15:29:25 2014
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import lte

class top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 1000

        ##################################################
        # Blocks
        ##################################################
        self.pre_blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*2, "/home/maier/Schreibtisch/testii", False)
        self.lte_rough_symbol_sync_cc_0 = lte.rough_symbol_sync_cc(2048, 2, "lte_rough_symbol_sync_cc_0")
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*2, samp_rate,True)
        (self.blocks_throttle_0).set_min_output_buffer(5000)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*2)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.pre_blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.lte_rough_symbol_sync_cc_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.lte_rough_symbol_sync_cc_0, 0))


# QT sink close method reimplementation

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


#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Resample measured data
# Author: Johannes Demel
# Generated: Wed Feb 20 00:31:23 2013
##################################################

from gnuradio import blks2
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser

class resample(gr.top_block):

	def __init__(self):
		gr.top_block.__init__(self, "Resample measured data")

		##################################################
		# Variables
		##################################################
		self.samp_rate = samp_rate = 32000

		##################################################
		# Blocks
		##################################################
		self.gr_file_source_0 = gr.file_source(gr.sizeof_gr_complex*1, "/home/johannes/gr-lte/data/Messung_LTE_2012-05-23_12:47:32.dat", False)
		self.gr_file_sink_0 = gr.file_sink(gr.sizeof_gr_complex*1, "/home/johannes/gr-lte/data/Resampled_LTE_2012_47:32.dat")
		self.gr_file_sink_0.set_unbuffered(False)
		self.blks2_rational_resampler_xxx_0 = blks2.rational_resampler_ccc(
			interpolation=3072,
			decimation=1000,
			taps=None,
			fractional_bw=None,
		)

		##################################################
		# Connections
		##################################################
		self.connect((self.blks2_rational_resampler_xxx_0, 0), (self.gr_file_sink_0, 0))
		self.connect((self.gr_file_source_0, 0), (self.blks2_rational_resampler_xxx_0, 0))


	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = resample()
	tb.start()
	raw_input('Press Enter to quit: ')
	tb.stop()


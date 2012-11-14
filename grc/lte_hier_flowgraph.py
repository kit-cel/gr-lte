#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: LTE hierarchical Flowgraph
# Author: Johannes Demel
# Description: LTE Flowgraph grouped into hierarchical Blocks
# Generated: Mon Oct  1 14:58:38 2012
##################################################

execfile("/home/demel/.grc_gnuradio/demod_and_eq.py")
execfile("/home/demel/.grc_gnuradio/lte_bch_decode.py")
execfile("/home/demel/.grc_gnuradio/lte_pbch_extraction.py")
execfile("/home/demel/.grc_gnuradio/synchronization.py")
from gnuradio import blks2
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import lte

class lte_hier_flowgraph(gr.top_block):

	def __init__(self):
		gr.top_block.__init__(self, "LTE hierarchical Flowgraph")

		##################################################
		# Variables
		##################################################
		self.style = style = "tx_diversity"
		self.samp_rate = samp_rate = 7.68e6
		self.fftlen = fftlen = 512
		self.N_rb_dl = N_rb_dl = 6

		##################################################
		# Blocks
		##################################################
		self.ext = lte_pbch_extraction(
			N_rb_dl=6,
			style=style,
		)
		self.eq = demod_and_eq(
			fftlen=512,
			N_rb_dl=6,
		)
		self.daemon = lte.cell_id_daemon(self.eq.eq.eq, self.ext.demux, self.ext.descr)
		self.synchronization_0 = synchronization(
			fftlen=512,
			daemon="daemon",
		)
		self.lte_mib_unpack_vb_0 = lte.mib_unpack_vb()
		self.lte_bch_decode_0 = lte_bch_decode()
		self.gr_throttle_0 = gr.throttle(gr.sizeof_gr_complex*1, samp_rate)
		self.gr_file_source_0 = gr.file_source(gr.sizeof_gr_complex*1, "/home/demel/Dokumente/Messungen/Messung_LTE_2012-05-23_12:47:32.dat", False)
		self.blks2_rational_resampler_xxx_0 = blks2.rational_resampler_ccc(
			interpolation=768,
			decimation=1000,
			taps=None,
			fractional_bw=None,
		)

		##################################################
		# Connections
		##################################################
		self.connect((self.eq, 2), (self.ext, 2))
		self.connect((self.eq, 1), (self.ext, 1))
		self.connect((self.eq, 0), (self.ext, 0))
		self.connect((self.synchronization_0, 0), (self.eq, 0))
		self.connect((self.ext, 0), (self.lte_bch_decode_0, 0))
		self.connect((self.lte_bch_decode_0, 1), (self.lte_mib_unpack_vb_0, 1))
		self.connect((self.lte_bch_decode_0, 0), (self.lte_mib_unpack_vb_0, 0))
		self.connect((self.gr_throttle_0, 0), (self.synchronization_0, 0))
		self.connect((self.blks2_rational_resampler_xxx_0, 0), (self.gr_throttle_0, 0))
		self.connect((self.gr_file_source_0, 0), (self.blks2_rational_resampler_xxx_0, 0))

	def get_style(self):
		return self.style

	def set_style(self, style):
		self.style = style
		self.ext.set_style(self.style)

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate

	def get_fftlen(self):
		return self.fftlen

	def set_fftlen(self, fftlen):
		self.fftlen = fftlen

	def get_N_rb_dl(self):
		return self.N_rb_dl

	def set_N_rb_dl(self, N_rb_dl):
		self.N_rb_dl = N_rb_dl

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = lte_hier_flowgraph()
	tb.start()
	raw_input('Press Enter to quit: ')
	tb.stop()


#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Zadoff Chu Sequence Correlator
# Author: Johannes Demel
# Generated: Thu Sep  6 14:48:28 2012
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import lte
import wx

class zadoff_chu(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="Zadoff Chu Sequence Correlator")
		_icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
		self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

		##################################################
		# Variables
		##################################################
		self.samp_rate = samp_rate = 32000
		self.length = length = 63
		self.init_val = init_val = 29

		##################################################
		# Blocks
		##################################################
		self.lte_zadoff_chu_seq_generator_vc_0 = lte.zadoff_chu_seq_generator_vc(length, init_val)
		self.gr_vector_sink_x_0 = gr.vector_sink_c(length)
		self.gr_head_0 = gr.head(gr.sizeof_gr_complex*length, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.lte_zadoff_chu_seq_generator_vc_0, 0), (self.gr_head_0, 0))
		self.connect((self.gr_head_0, 0), (self.gr_vector_sink_x_0, 0))

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate

	def get_length(self):
		return self.length

	def set_length(self, length):
		self.length = length

	def get_init_val(self):
		return self.init_val

	def set_init_val(self, init_val):
		self.init_val = init_val

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = zadoff_chu()
	tb.Run(True)


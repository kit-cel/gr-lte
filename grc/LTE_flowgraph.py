#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: LTE flowgraph
# Author: Johannes Demel
# Description: top level LTE flowgraph
# Generated: Fri Jul 12 11:42:39 2013
##################################################

execfile("/home/demel/.grc_gnuradio/lte_channel_est_hier.py")
execfile("/home/demel/.grc_gnuradio/lte_decode_bch_hier.py")
execfile("/home/demel/.grc_gnuradio/lte_source_c.py")
from PyQt4 import Qt
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import gr
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import lte
import sys

class LTE_flowgraph(gr.top_block, Qt.QWidget):

	def __init__(self):
		gr.top_block.__init__(self, "LTE flowgraph")
		Qt.QWidget.__init__(self)
		self.setWindowTitle("LTE flowgraph")
		self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
		self.top_scroll_layout = Qt.QVBoxLayout()
		self.setLayout(self.top_scroll_layout)
		self.top_scroll = Qt.QScrollArea()
		self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
		self.top_scroll_layout.addWidget(self.top_scroll)
		self.top_scroll.setWidgetResizable(True)
		self.top_widget = Qt.QWidget()
		self.top_scroll.setWidget(self.top_widget)
		self.top_layout = Qt.QVBoxLayout(self.top_widget)
		self.top_grid_layout = Qt.QGridLayout()
		self.top_layout.addLayout(self.top_grid_layout)


		##################################################
		# Variables
		##################################################
		self.fft_len = fft_len = 2048
		self.cpl0 = cpl0 = 160*fft_len/2048
		self.cpl = cpl = 144*fft_len/2048
		self.slotl = slotl = 7*fft_len+6*cpl+cpl0
		self.tag_key = tag_key = "symbol"
		self.samp_rate = samp_rate = int(slotl/0.0005)
		self.N_rb_dl = N_rb_dl = 6

		##################################################
		# Blocks
		##################################################
		self.lte_source_c_0 = lte_source_c(
			samp_rate=30.72e6,
		)
		self.lte_remove_cp_cvc_0 = lte.remove_cp_cvc(fft_len, tag_key)
		self.lte_pbch_demux_vcvc_1 = lte.pbch_demux_vcvc(N_rb_dl)
		self.lte_mib_unpack_vb_0 = lte.mib_unpack_vb()
		self.lte_hier_sss_sync_cc_1 = lte.hier_sss_sync_cc(fft_len)
		self.lte_hier_pss_sync_cc_0 = lte.hier_pss_sync_cc(fft_len)
		self.lte_hier_freq_estimate_cc_0 = lte.hier_freq_estimate_cc(fft_len)
		self.lte_extract_occupied_tones_vcvc_0 = lte.extract_occupied_tones_vcvc(N_rb_dl,fft_len)
		self.lte_decode_pbch_vcvf_0 = lte.decode_pbch_vcvf()
		self.lte_decode_bch_hier_0 = lte_decode_bch_hier()
		self.lte_cp_time_freq_sync_cc_0 = lte.cp_time_freq_sync_cc(fft_len)
		self.lte_channel_est_hier_0 = lte_channel_est_hier(
			N_rb_dl=6,
			tag_key=tag_key,
		)
		self.fft_vxx_0 = fft.fft_vcc(fft_len, True, (window.rectangular(fft_len)), False, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.lte_source_c_0, 0), (self.lte_cp_time_freq_sync_cc_0, 0))
		self.connect((self.lte_channel_est_hier_0, 1), (self.lte_pbch_demux_vcvc_1, 2))
		self.connect((self.lte_channel_est_hier_0, 0), (self.lte_pbch_demux_vcvc_1, 1))
		self.connect((self.lte_extract_occupied_tones_vcvc_0, 0), (self.lte_pbch_demux_vcvc_1, 0))
		self.connect((self.lte_extract_occupied_tones_vcvc_0, 0), (self.lte_channel_est_hier_0, 0))
		self.connect((self.fft_vxx_0, 0), (self.lte_extract_occupied_tones_vcvc_0, 0))
		self.connect((self.lte_hier_freq_estimate_cc_0, 0), (self.lte_hier_sss_sync_cc_1, 0))
		self.connect((self.lte_pbch_demux_vcvc_1, 1), (self.lte_decode_pbch_vcvf_0, 1))
		self.connect((self.lte_pbch_demux_vcvc_1, 2), (self.lte_decode_pbch_vcvf_0, 2))
		self.connect((self.lte_remove_cp_cvc_0, 0), (self.fft_vxx_0, 0))
		self.connect((self.lte_hier_sss_sync_cc_1, 0), (self.lte_remove_cp_cvc_0, 0))
		self.connect((self.lte_hier_pss_sync_cc_0, 0), (self.lte_hier_freq_estimate_cc_0, 0))
		self.connect((self.lte_cp_time_freq_sync_cc_0, 0), (self.lte_hier_pss_sync_cc_0, 0))
		self.connect((self.lte_pbch_demux_vcvc_1, 0), (self.lte_decode_pbch_vcvf_0, 0))
		self.connect((self.lte_decode_pbch_vcvf_0, 0), (self.lte_decode_bch_hier_0, 0))
		self.connect((self.lte_decode_bch_hier_0, 0), (self.lte_mib_unpack_vb_0, 0))
		self.connect((self.lte_decode_bch_hier_0, 1), (self.lte_mib_unpack_vb_0, 1))

		##################################################
		# Asynch Message Connections
		##################################################
		self.msg_connect(self.lte_hier_sss_sync_cc_1, "cell_id", self.lte_channel_est_hier_0, "cell_id")
		self.msg_connect(self.lte_hier_sss_sync_cc_1, "cell_id", self.lte_pbch_demux_vcvc_1, "cell_id")
		self.msg_connect(self.lte_hier_sss_sync_cc_1, "cell_id", self.lte_decode_pbch_vcvf_0, "cell_id")

	def get_fft_len(self):
		return self.fft_len

	def set_fft_len(self, fft_len):
		self.fft_len = fft_len
		self.set_cpl(144*self.fft_len/2048)
		self.set_cpl0(160*self.fft_len/2048)
		self.set_slotl(7*self.fft_len+6*self.cpl+self.cpl0)

	def get_cpl0(self):
		return self.cpl0

	def set_cpl0(self, cpl0):
		self.cpl0 = cpl0
		self.set_slotl(7*self.fft_len+6*self.cpl+self.cpl0)

	def get_cpl(self):
		return self.cpl

	def set_cpl(self, cpl):
		self.cpl = cpl
		self.set_slotl(7*self.fft_len+6*self.cpl+self.cpl0)

	def get_slotl(self):
		return self.slotl

	def set_slotl(self, slotl):
		self.slotl = slotl
		self.set_samp_rate(int(self.slotl/0.0005))

	def get_tag_key(self):
		return self.tag_key

	def set_tag_key(self, tag_key):
		self.tag_key = tag_key
		self.lte_channel_est_hier_0.set_tag_key(self.tag_key)

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate

	def get_N_rb_dl(self):
		return self.N_rb_dl

	def set_N_rb_dl(self, N_rb_dl):
		self.N_rb_dl = N_rb_dl

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	qapp = Qt.QApplication(sys.argv)
	tb = LTE_flowgraph()
	tb.start()
	tb.show()
	qapp.exec_()
	tb.stop()


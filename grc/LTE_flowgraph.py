#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: LTE flowgraph
# Author: Johannes Demel
# Description: top level LTE flowgraph
# Generated: Tue Jul 23 14:08:27 2013
##################################################

execfile("/home/johannes/.grc_gnuradio/lte_decode_bch_hier.py")
execfile("/home/johannes/.grc_gnuradio/lte_source_c.py")
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import gr
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import lte

class LTE_flowgraph(gr.top_block):

	def __init__(self):
		gr.top_block.__init__(self, "LTE flowgraph")

		##################################################
		# Variables
		##################################################
		self.fft_len = fft_len = 2048
		self.cpl0 = cpl0 = 160*fft_len/2048
		self.cpl = cpl = 144*fft_len/2048
		self.cell_id = cell_id = 364
		self.N_rb_dl = N_rb_dl = 15
		self.slotl = slotl = 7*fft_len+6*cpl+cpl0
		self.frame_pilots_1 = frame_pilots_1 = lte.frame_pilot_value_and_position(N_rb_dl, cell_id, 1, 1)
		self.frame_pilots_0 = frame_pilots_0 = lte.frame_pilot_value_and_position(N_rb_dl, cell_id, 1, 0)
		self.tag_key = tag_key = "symbol"
		self.samp_rate = samp_rate = int(slotl/0.0005)
		self.pilot_symbols = pilot_symbols = frame_pilots_0[1]
		self.pilot_carriers_p1 = pilot_carriers_p1 = frame_pilots_1[0]
		self.pilot_carriers_p0 = pilot_carriers_p0 = frame_pilots_0[0]

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
		self.lte_estimator_parameterizer_msg_0_0 = lte.estimator_parameterizer_msg("cell_id", "pilots", N_rb_dl, 1)
		self.lte_estimator_parameterizer_msg_0 = lte.estimator_parameterizer_msg("cell_id", "pilots", N_rb_dl, 0)
		self.lte_decode_pbch_vcvf_0 = lte.decode_pbch_vcvf()
		self.lte_decode_bch_hier_0 = lte_decode_bch_hier()
		self.lte_cp_time_freq_sync_cc_0 = lte.cp_time_freq_sync_cc(fft_len)
		self.lte_channel_estimator_ant_1 = lte.channel_estimator_vcvc(12*N_rb_dl, tag_key, "pilots", pilot_carriers_p1, pilot_symbols)
		self.lte_channel_estimator_ant_0 = lte.channel_estimator_vcvc(12*N_rb_dl, tag_key, "pilots", pilot_carriers_p0, pilot_symbols)
		self.fft_vxx_0 = fft.fft_vcc(fft_len, True, (window.rectangular(fft_len)), False, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.lte_extract_occupied_tones_vcvc_0, 0), (self.lte_pbch_demux_vcvc_1, 0))
		self.connect((self.fft_vxx_0, 0), (self.lte_extract_occupied_tones_vcvc_0, 0))
		self.connect((self.lte_hier_freq_estimate_cc_0, 0), (self.lte_hier_sss_sync_cc_1, 0))
		self.connect((self.lte_pbch_demux_vcvc_1, 1), (self.lte_decode_pbch_vcvf_0, 1))
		self.connect((self.lte_pbch_demux_vcvc_1, 2), (self.lte_decode_pbch_vcvf_0, 2))
		self.connect((self.lte_remove_cp_cvc_0, 0), (self.fft_vxx_0, 0))
		self.connect((self.lte_hier_sss_sync_cc_1, 0), (self.lte_remove_cp_cvc_0, 0))
		self.connect((self.lte_hier_pss_sync_cc_0, 0), (self.lte_hier_freq_estimate_cc_0, 0))
		self.connect((self.lte_cp_time_freq_sync_cc_0, 0), (self.lte_hier_pss_sync_cc_0, 0))
		self.connect((self.lte_pbch_demux_vcvc_1, 0), (self.lte_decode_pbch_vcvf_0, 0))
		self.connect((self.lte_source_c_0, 0), (self.lte_cp_time_freq_sync_cc_0, 0))
		self.connect((self.lte_decode_pbch_vcvf_0, 0), (self.lte_decode_bch_hier_0, 0))
		self.connect((self.lte_decode_bch_hier_0, 0), (self.lte_mib_unpack_vb_0, 0))
		self.connect((self.lte_decode_bch_hier_0, 1), (self.lte_mib_unpack_vb_0, 1))
		self.connect((self.lte_extract_occupied_tones_vcvc_0, 0), (self.lte_channel_estimator_ant_0, 0))
		self.connect((self.lte_extract_occupied_tones_vcvc_0, 0), (self.lte_channel_estimator_ant_1, 0))
		self.connect((self.lte_channel_estimator_ant_0, 0), (self.lte_pbch_demux_vcvc_1, 1))
		self.connect((self.lte_channel_estimator_ant_1, 0), (self.lte_pbch_demux_vcvc_1, 2))

		##################################################
		# Asynch Message Connections
		##################################################
		self.msg_connect(self.lte_hier_sss_sync_cc_1, "cell_id", self.lte_pbch_demux_vcvc_1, "cell_id")
		self.msg_connect(self.lte_hier_sss_sync_cc_1, "cell_id", self.lte_decode_pbch_vcvf_0, "cell_id")
		self.msg_connect(self.lte_estimator_parameterizer_msg_0, "pilots", self.lte_channel_estimator_ant_0, "pilots")
		self.msg_connect(self.lte_estimator_parameterizer_msg_0_0, "pilots", self.lte_channel_estimator_ant_1, "pilots")
		self.msg_connect(self.lte_hier_sss_sync_cc_1, "cell_id", self.lte_estimator_parameterizer_msg_0, "cell_id")
		self.msg_connect(self.lte_hier_sss_sync_cc_1, "cell_id", self.lte_estimator_parameterizer_msg_0_0, "cell_id")

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

	def get_cell_id(self):
		return self.cell_id

	def set_cell_id(self, cell_id):
		self.cell_id = cell_id
		self.set_frame_pilots_0(lte.frame_pilot_value_and_position(self.N_rb_dl, self.cell_id, 1, 0))
		self.set_frame_pilots_1(lte.frame_pilot_value_and_position(self.N_rb_dl, self.cell_id, 1, 1))

	def get_N_rb_dl(self):
		return self.N_rb_dl

	def set_N_rb_dl(self, N_rb_dl):
		self.N_rb_dl = N_rb_dl
		self.set_frame_pilots_0(lte.frame_pilot_value_and_position(self.N_rb_dl, self.cell_id, 1, 0))
		self.set_frame_pilots_1(lte.frame_pilot_value_and_position(self.N_rb_dl, self.cell_id, 1, 1))

	def get_slotl(self):
		return self.slotl

	def set_slotl(self, slotl):
		self.slotl = slotl
		self.set_samp_rate(int(self.slotl/0.0005))

	def get_frame_pilots_1(self):
		return self.frame_pilots_1

	def set_frame_pilots_1(self, frame_pilots_1):
		self.frame_pilots_1 = frame_pilots_1
		self.set_pilot_carriers_p1(self.frame_pilots_1[0])

	def get_frame_pilots_0(self):
		return self.frame_pilots_0

	def set_frame_pilots_0(self, frame_pilots_0):
		self.frame_pilots_0 = frame_pilots_0
		self.set_pilot_symbols(self.frame_pilots_0[1])
		self.set_pilot_carriers_p0(self.frame_pilots_0[0])

	def get_tag_key(self):
		return self.tag_key

	def set_tag_key(self, tag_key):
		self.tag_key = tag_key

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate

	def get_pilot_symbols(self):
		return self.pilot_symbols

	def set_pilot_symbols(self, pilot_symbols):
		self.pilot_symbols = pilot_symbols

	def get_pilot_carriers_p1(self):
		return self.pilot_carriers_p1

	def set_pilot_carriers_p1(self, pilot_carriers_p1):
		self.pilot_carriers_p1 = pilot_carriers_p1

	def get_pilot_carriers_p0(self):
		return self.pilot_carriers_p0

	def set_pilot_carriers_p0(self, pilot_carriers_p0):
		self.pilot_carriers_p0 = pilot_carriers_p0

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = LTE_flowgraph()
	tb.start()
	raw_input('Press Enter to quit: ')
	tb.stop()


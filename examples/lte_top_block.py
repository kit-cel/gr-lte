#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: LTE_test
# Author: Johannes Demel
# Generated: Wed Jun 25 15:47:31 2014
##################################################

execfile("/home/maier/.grc_gnuradio/decode_bch_hier_gr37.py")
execfile("/home/maier/.grc_gnuradio/decode_pbch_37.py")
execfile("/home/maier/.grc_gnuradio/lte_cp_freq_sync.py")
execfile("/home/maier/.grc_gnuradio/lte_estimator_hier.py")
execfile("/home/maier/.grc_gnuradio/lte_ofdm_hier.py")
execfile("/home/maier/.grc_gnuradio/lte_pss_sync_37.py")
execfile("/home/maier/.grc_gnuradio/lte_sss_sync_hier.py")
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import lte

class lte_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "LTE_test")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 30.72e6/2
        self.pbch_descr_key = pbch_descr_key = "descr_part"
        self.interp_val = interp_val = int(samp_rate/1e4)
        self.frame_key = frame_key = "slot"
        self.fftlen = fftlen = 1024
        self.N_rb_dl = N_rb_dl = 50

        ##################################################
        # Blocks
        ##################################################
        self.sync_lte_sss_sync_hier_0 = lte_sss_sync_hier(
            fftlen=fftlen,
            N_rb_dl=N_rb_dl,
            group_key="N_id_2",
            offset_key="offset_marker",
        )
        self.sync_lte_rough_symbol_sync_cc_0 = lte.rough_symbol_sync_cc(fftlen, 1, "sync_lte_rough_symbol_sync_cc_0")
        self.sync_lte_pss_sync_37_0 = lte_pss_sync_37(
            fftlen=fftlen,
        )
        self.sync_lte_cp_freq_sync_0 = lte_cp_freq_sync(
            fftlen=fftlen,
        )
        self.pre_blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, "/home/maier/Schreibtisch/ltetest5framesETUvec1.dat", True)
        self.pbch_decode_pbch_37_0 = decode_pbch_37(
            N_rb_dl=N_rb_dl,
        )
        self.ofdm_lte_ofdm_hier_0 = lte_ofdm_hier(
            N_rb_dl=50,
            fftlen=2048,
            ofdm_key=frame_key,
        )
        self.ofdm_estimator_lte_estimator_hier_0 = lte_estimator_hier(
            initial_id=387,
            estimator_key="slot",
            N_rb_dl=50,
        )
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        (self.blocks_throttle_0).set_min_output_buffer(5000)
        self.bch_decode_bch_hier_gr37_0 = decode_bch_hier_gr37()
        self.MIB = lte.mib_unpack_vbm("MIB")

        ##################################################
        # Connections
        ##################################################
        self.connect((self.bch_decode_bch_hier_gr37_0, 0), (self.MIB, 0))
        self.connect((self.bch_decode_bch_hier_gr37_0, 1), (self.MIB, 1))
        self.connect((self.pbch_decode_pbch_37_0, 0), (self.bch_decode_bch_hier_gr37_0, 0))
        self.connect((self.ofdm_estimator_lte_estimator_hier_0, 0), (self.pbch_decode_pbch_37_0, 1))
        self.connect((self.ofdm_lte_ofdm_hier_0, 0), (self.pbch_decode_pbch_37_0, 0))
        self.connect((self.ofdm_estimator_lte_estimator_hier_0, 1), (self.pbch_decode_pbch_37_0, 2))
        self.connect((self.sync_lte_pss_sync_37_0, 0), (self.sync_lte_cp_freq_sync_0, 0))
        self.connect((self.ofdm_lte_ofdm_hier_0, 0), (self.ofdm_estimator_lte_estimator_hier_0, 0))
        self.connect((self.sync_lte_cp_freq_sync_0, 0), (self.sync_lte_sss_sync_hier_0, 0))
        self.connect((self.sync_lte_sss_sync_hier_0, 0), (self.ofdm_lte_ofdm_hier_0, 0))
        self.connect((self.pre_blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.sync_lte_rough_symbol_sync_cc_0, 0))
        self.connect((self.sync_lte_rough_symbol_sync_cc_0, 0), (self.sync_lte_pss_sync_37_0, 0))

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.msg_connect(self.sync_lte_sss_sync_hier_0, "cell_id", self.ofdm_estimator_lte_estimator_hier_0, "cell_id")
        self.msg_connect(self.sync_lte_sss_sync_hier_0, "cell_id", self.pbch_decode_pbch_37_0, "cell_id")

# QT sink close method reimplementation

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.set_interp_val(int(self.samp_rate/1e4))
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)

    def get_pbch_descr_key(self):
        return self.pbch_descr_key

    def set_pbch_descr_key(self, pbch_descr_key):
        self.pbch_descr_key = pbch_descr_key

    def get_interp_val(self):
        return self.interp_val

    def set_interp_val(self, interp_val):
        self.interp_val = interp_val

    def get_frame_key(self):
        return self.frame_key

    def set_frame_key(self, frame_key):
        self.frame_key = frame_key
        self.ofdm_lte_ofdm_hier_0.set_ofdm_key(self.frame_key)

    def get_fftlen(self):
        return self.fftlen

    def set_fftlen(self, fftlen):
        self.fftlen = fftlen
        self.sync_lte_cp_freq_sync_0.set_fftlen(self.fftlen)
        self.sync_lte_pss_sync_37_0.set_fftlen(self.fftlen)
        self.sync_lte_sss_sync_hier_0.set_fftlen(self.fftlen)

    def get_N_rb_dl(self):
        return self.N_rb_dl

    def set_N_rb_dl(self, N_rb_dl):
        self.N_rb_dl = N_rb_dl
        self.pbch_decode_pbch_37_0.set_N_rb_dl(self.N_rb_dl)
        self.sync_lte_sss_sync_hier_0.set_N_rb_dl(self.N_rb_dl)

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    tb = lte_top_block()
    tb.start()
    raw_input('Press Enter to quit: ')
    tb.stop()
    tb.wait()


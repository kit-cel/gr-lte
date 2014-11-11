#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: MIMO TOP FLOW
# Author: Kristian Maier, Johannes Demel
# Description: LTE decoding up to 2 tx antennas
# Generated: Tue Nov 11 12:05:03 2014
##################################################

execfile("/home/johannes/.grc_gnuradio/decode_bch_hier_gr37.py")
execfile("/home/johannes/.grc_gnuradio/lte_mimo_decode_pbch.py")
execfile("/home/johannes/.grc_gnuradio/lte_mimo_estimator.py")
execfile("/home/johannes/.grc_gnuradio/lte_mimo_ofdm_rx.py")
execfile("/home/johannes/.grc_gnuradio/lte_mimo_pss_based_frey_sync.py")
execfile("/home/johannes/.grc_gnuradio/lte_mimo_pss_sync.py")
execfile("/home/johannes/.grc_gnuradio/lte_mimo_sss_sync.py")
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import lte

class MIMO_TOP_FLOW(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "MIMO TOP FLOW")

        ##################################################
        # Variables
        ##################################################
        self.fftl = fftl = 128
        self.samp_rate = samp_rate = fftl*15e3
        self.rxant = rxant = 2
        self.resampler = resampler = 400
        self.frame_key = frame_key = "slot"
        self.N_rb_dl = N_rb_dl = 50

        ##################################################
        # Blocks
        ##################################################
        self.lte_mimo_sss_sync_0 = lte_mimo_sss_sync(
            N_rb_dl=N_rb_dl,
            rxant=rxant,
        )
        self.lte_mimo_pss_sync_0 = lte_mimo_pss_sync(
            rxant=rxant,
            synclen=5,
            fftlen=fftl,
        )
        self.lte_mimo_pss_based_frey_sync_0 = lte_mimo_pss_based_frey_sync(
            fftlen=fftl,
            rxant=rxant,
        )
        self.lte_mimo_ofdm_rx_0 = lte_mimo_ofdm_rx(
            N_rb_dl=N_rb_dl,
            ofdm_key=frame_key,
            fftlen=fftl,
            rxant=rxant,
        )
        self.lte_mimo_estimator_0 = lte_mimo_estimator(
            N_rb_dl=N_rb_dl,
            estimator_key=frame_key,
            initial_id=2,
            rxant=rxant,
        )
        self.lte_mimo_decode_pbch_0 = lte_mimo_decode_pbch(
            N_rb_dl=N_rb_dl,
            rxant=rxant,
        )
        self.blocks_vector_to_streams_0 = blocks.vector_to_streams(gr.sizeof_gr_complex*1, 2)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*2, samp_rate,True)
        self.blocks_streams_to_vector_0_0 = blocks.streams_to_vector(gr.sizeof_gr_complex*12 * N_rb_dl, 2)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*2, "/home/johannes/src/gr-lte/tests/lte_test_data_RX2_NRBDL6.dat", True)
        self.bch_decode_bch_hier_gr37_0 = decode_bch_hier_gr37()
        self.MIB = lte.mib_unpack_vbm("MIB")

        ##################################################
        # Connections
        ##################################################
        self.connect((self.lte_mimo_pss_sync_0, 1), (self.lte_mimo_pss_based_frey_sync_0, 1))
        self.connect((self.lte_mimo_pss_sync_0, 0), (self.lte_mimo_pss_based_frey_sync_0, 0))
        self.connect((self.lte_mimo_pss_based_frey_sync_0, 0), (self.lte_mimo_ofdm_rx_0, 0))
        self.connect((self.lte_mimo_pss_based_frey_sync_0, 1), (self.lte_mimo_ofdm_rx_0, 1))
        self.connect((self.blocks_throttle_0, 0), (self.blocks_vector_to_streams_0, 0))
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 0), (self.lte_mimo_pss_sync_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 1), (self.lte_mimo_pss_sync_0, 1))
        self.connect((self.lte_mimo_ofdm_rx_0, 0), (self.lte_mimo_sss_sync_0, 0))
        self.connect((self.lte_mimo_ofdm_rx_0, 1), (self.lte_mimo_sss_sync_0, 1))
        self.connect((self.lte_mimo_decode_pbch_0, 0), (self.bch_decode_bch_hier_gr37_0, 0))
        self.connect((self.lte_mimo_estimator_0, 0), (self.lte_mimo_decode_pbch_0, 1))
        self.connect((self.lte_mimo_estimator_0, 1), (self.lte_mimo_decode_pbch_0, 2))
        self.connect((self.bch_decode_bch_hier_gr37_0, 1), (self.MIB, 1))
        self.connect((self.bch_decode_bch_hier_gr37_0, 0), (self.MIB, 0))
        self.connect((self.lte_mimo_sss_sync_0, 1), (self.blocks_streams_to_vector_0_0, 1))
        self.connect((self.lte_mimo_sss_sync_0, 0), (self.blocks_streams_to_vector_0_0, 0))
        self.connect((self.blocks_streams_to_vector_0_0, 0), (self.lte_mimo_estimator_0, 0))
        self.connect((self.blocks_streams_to_vector_0_0, 0), (self.lte_mimo_decode_pbch_0, 0))

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.msg_connect(self.lte_mimo_sss_sync_0, "cell_id", self.lte_mimo_estimator_0, "cell_id")
        self.msg_connect(self.lte_mimo_pss_sync_0, "N_id_2", self.lte_mimo_sss_sync_0, "N_id_2")
        self.msg_connect(self.lte_mimo_sss_sync_0, "cell_id", self.lte_mimo_decode_pbch_0, "cell_id")


    def get_fftl(self):
        return self.fftl

    def set_fftl(self, fftl):
        self.fftl = fftl
        self.set_samp_rate(self.fftl*15e3)
        self.lte_mimo_pss_based_frey_sync_0.set_fftlen(self.fftl)
        self.lte_mimo_pss_sync_0.set_fftlen(self.fftl)
        self.lte_mimo_ofdm_rx_0.set_fftlen(self.fftl)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)

    def get_rxant(self):
        return self.rxant

    def set_rxant(self, rxant):
        self.rxant = rxant
        self.lte_mimo_pss_based_frey_sync_0.set_rxant(self.rxant)
        self.lte_mimo_pss_sync_0.set_rxant(self.rxant)
        self.lte_mimo_ofdm_rx_0.set_rxant(self.rxant)
        self.lte_mimo_sss_sync_0.set_rxant(self.rxant)
        self.lte_mimo_estimator_0.set_rxant(self.rxant)
        self.lte_mimo_decode_pbch_0.set_rxant(self.rxant)

    def get_resampler(self):
        return self.resampler

    def set_resampler(self, resampler):
        self.resampler = resampler

    def get_frame_key(self):
        return self.frame_key

    def set_frame_key(self, frame_key):
        self.frame_key = frame_key
        self.lte_mimo_ofdm_rx_0.set_ofdm_key(self.frame_key)
        self.lte_mimo_estimator_0.set_estimator_key(self.frame_key)

    def get_N_rb_dl(self):
        return self.N_rb_dl

    def set_N_rb_dl(self, N_rb_dl):
        self.N_rb_dl = N_rb_dl
        self.lte_mimo_ofdm_rx_0.set_N_rb_dl(self.N_rb_dl)
        self.lte_mimo_sss_sync_0.set_N_rb_dl(self.N_rb_dl)
        self.lte_mimo_estimator_0.set_N_rb_dl(self.N_rb_dl)
        self.lte_mimo_decode_pbch_0.set_N_rb_dl(self.N_rb_dl)

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    tb = MIMO_TOP_FLOW()
    tb.start()
    raw_input('Press Enter to quit: ')
    tb.stop()
    tb.wait()

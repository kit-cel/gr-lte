#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Tue Jul 15 22:04:17 2014
##################################################

execfile("/home/maier/.grc_gnuradio/decode_bch_hier_gr37.py")
execfile("/home/maier/.grc_gnuradio/decode_pbch_37.py")
execfile("/home/maier/.grc_gnuradio/lte_estimator_hier.py")
execfile("/home/maier/.grc_gnuradio/lte_mimo_ofdm_rx.py")
execfile("/home/maier/.grc_gnuradio/lte_mimo_pss_based_frey_sync.py")
execfile("/home/maier/.grc_gnuradio/lte_mimo_pss_sync.py")
execfile("/home/maier/.grc_gnuradio/lte_mimo_sss_sync.py")
from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import lte
import sys

class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
        try:
             self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
             pass
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

        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.fftl = fftl = 1024
        self.vlen = vlen = 240
        self.style = style = "tx_diversity"
        self.slot = slot = "slot"
        self.samp_rate = samp_rate = fftl*15e3
        self.rxant = rxant = 2
        self.pbch_descr_key = pbch_descr_key = "descr_part"
        self.frame_key = frame_key = "slot"
        self.N_rb_dl = N_rb_dl = 50

        ##################################################
        # Blocks
        ##################################################
        self.pbch_decode_pbch_37_0 = decode_pbch_37(
            N_rb_dl=50,
        )
        self.ofdm_estimator_lte_estimator_hier_0 = lte_estimator_hier(
            initial_id=1,
            estimator_key="slot",
            N_rb_dl=50,
        )
        self.lte_mimo_sss_sync_0 = lte_mimo_sss_sync(
            rxant=2,
            fftlen=1024,
        )
        self.lte_mimo_pss_sync_0 = lte_mimo_pss_sync(
            fftlen=fftl,
            rxant=rxant,
            synclen=5,
        )
        self.lte_mimo_pss_based_frey_sync_0 = lte_mimo_pss_based_frey_sync(
            fftlen=1024,
            rxant=rxant,
        )
        self.lte_mimo_ofdm_rx_0 = lte_mimo_ofdm_rx(
            rxant=rxant,
            fftlen=fftl,
            ofdm_key=slot,
            N_rb_dl=50,
        )
        self.blocks_vector_to_streams_0 = blocks.vector_to_streams(gr.sizeof_gr_complex*1, rxant)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*2, samp_rate,True)
        self.blocks_skiphead_0 = blocks.skiphead(gr.sizeof_gr_complex*2, 0)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*600)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*2, "/home/maier/Schreibtisch/lte200framesFadingChannelOWN2Mul_fOff2000.dat", False)
        self.bch_decode_bch_hier_gr37_0 = decode_bch_hier_gr37()
        self.MIB = lte.mib_unpack_vbm("MIB")

        ##################################################
        # Connections
        ##################################################
        self.connect((self.lte_mimo_ofdm_rx_0, 0), (self.blocks_null_sink_0, 1))
        self.connect((self.lte_mimo_ofdm_rx_0, 1), (self.blocks_null_sink_0, 0))
        self.connect((self.ofdm_estimator_lte_estimator_hier_0, 1), (self.pbch_decode_pbch_37_0, 2))
        self.connect((self.ofdm_estimator_lte_estimator_hier_0, 0), (self.pbch_decode_pbch_37_0, 1))
        self.connect((self.pbch_decode_pbch_37_0, 0), (self.bch_decode_bch_hier_gr37_0, 0))
        self.connect((self.bch_decode_bch_hier_gr37_0, 1), (self.MIB, 1))
        self.connect((self.bch_decode_bch_hier_gr37_0, 0), (self.MIB, 0))
        self.connect((self.lte_mimo_sss_sync_0, 1), (self.lte_mimo_ofdm_rx_0, 1))
        self.connect((self.lte_mimo_sss_sync_0, 0), (self.lte_mimo_ofdm_rx_0, 0))
        self.connect((self.lte_mimo_pss_sync_0, 0), (self.lte_mimo_pss_based_frey_sync_0, 0))
        self.connect((self.lte_mimo_pss_sync_0, 1), (self.lte_mimo_pss_based_frey_sync_0, 1))
        self.connect((self.lte_mimo_pss_based_frey_sync_0, 0), (self.lte_mimo_sss_sync_0, 0))
        self.connect((self.lte_mimo_pss_based_frey_sync_0, 1), (self.lte_mimo_sss_sync_0, 1))
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 0), (self.lte_mimo_pss_sync_0, 1))
        self.connect((self.blocks_vector_to_streams_0, 1), (self.lte_mimo_pss_sync_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.blocks_skiphead_0, 0))
        self.connect((self.blocks_skiphead_0, 0), (self.blocks_vector_to_streams_0, 0))
        self.connect((self.lte_mimo_ofdm_rx_0, 0), (self.ofdm_estimator_lte_estimator_hier_0, 0))
        self.connect((self.lte_mimo_ofdm_rx_0, 0), (self.pbch_decode_pbch_37_0, 0))

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.msg_connect(self.lte_mimo_sss_sync_0, "cell_id", self.pbch_decode_pbch_37_0, "cell_id")
        self.msg_connect(self.lte_mimo_sss_sync_0, "cell_id", self.ofdm_estimator_lte_estimator_hier_0, "cell_id")

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_fftl(self):
        return self.fftl

    def set_fftl(self, fftl):
        self.fftl = fftl
        self.set_samp_rate(self.fftl*15e3)
        self.lte_mimo_pss_sync_0.set_fftlen(self.fftl)
        self.lte_mimo_ofdm_rx_0.set_fftlen(self.fftl)

    def get_vlen(self):
        return self.vlen

    def set_vlen(self, vlen):
        self.vlen = vlen

    def get_style(self):
        return self.style

    def set_style(self, style):
        self.style = style

    def get_slot(self):
        return self.slot

    def set_slot(self, slot):
        self.slot = slot
        self.lte_mimo_ofdm_rx_0.set_ofdm_key(self.slot)

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

    def get_pbch_descr_key(self):
        return self.pbch_descr_key

    def set_pbch_descr_key(self, pbch_descr_key):
        self.pbch_descr_key = pbch_descr_key

    def get_frame_key(self):
        return self.frame_key

    def set_frame_key(self, frame_key):
        self.frame_key = frame_key

    def get_N_rb_dl(self):
        return self.N_rb_dl

    def set_N_rb_dl(self, N_rb_dl):
        self.N_rb_dl = N_rb_dl

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    Qt.QApplication.setGraphicsSystem(gr.prefs().get_string('qtgui','style','raster'))
    qapp = Qt.QApplication(sys.argv)
    tb = top_block()
    tb.start()
    tb.show()
    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()
    tb = None #to clean up Qt widgets

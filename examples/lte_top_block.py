#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: LTE_test
# Author: Johannes Demel
# Generated: Mon Apr 20 13:54:38 2015
##################################################

# Call XInitThreads as the _very_ first thing.
# After some Qt import, it's too late
import ctypes
import sys
if sys.platform.startswith('linux'):
    try:
        x11 = ctypes.cdll.LoadLibrary('libX11.so')
        x11.XInitThreads()
    except:
        print "Warning: failed to XInitThreads()"

execfile("/home/johannes/.grc_gnuradio/decode_bch_hier_gr37.py")
execfile("/home/johannes/.grc_gnuradio/decode_pbch_37.py")
execfile("/home/johannes/.grc_gnuradio/lte_cp_freq_sync.py")
execfile("/home/johannes/.grc_gnuradio/lte_estimator_hier.py")
execfile("/home/johannes/.grc_gnuradio/lte_ofdm_hier.py")
execfile("/home/johannes/.grc_gnuradio/lte_pss_sync_37.py")
execfile("/home/johannes/.grc_gnuradio/lte_sss_sync_hier.py")
from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import lte
import sys

from distutils.version import StrictVersion
class lte_top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "LTE_test")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("LTE_test")
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

        self.settings = Qt.QSettings("GNU Radio", "lte_top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.fftlen = fftlen = 2048
        self.samp_rate = samp_rate = (7*fftlen + 160* fftlen/2048 + 6 * (144 * fftlen/2048)) / 0.0005
        self.pbch_descr_key = pbch_descr_key = "descr_part"
        self.frame_key = frame_key = "slot"
        self.N_rb_dl = N_rb_dl = 6

        ##################################################
        # Blocks
        ##################################################
        self.sync_lte_sss_sync_hier_0 = lte_sss_sync_hier(
            N_rb_dl=N_rb_dl,
            group_key="N_id_2",
            offset_key="offset_marker",
            fftlen=fftlen,
        )
        self.sync_lte_rough_symbol_sync_cc_0 = lte.rough_symbol_sync_cc(fftlen, 1, "sync_lte_rough_symbol_sync_cc_0")
        self.sync_lte_pss_sync_37_0 = lte_pss_sync_37(
            fftlen=fftlen,
        )
        self.sync_lte_cp_freq_sync_0 = lte_cp_freq_sync(
            fftlen=max(256,fftlen),
        )
        self.pre_blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, "/home/johannes/src/gr-lte/tests/lte_test_data_RX1_NRBDL100.dat", True)
        self.pbch_decode_pbch_37_0 = decode_pbch_37(
            N_rb_dl=N_rb_dl,
        )
        self.ofdm_lte_ofdm_hier_0 = lte_ofdm_hier(
            fftlen=fftlen,
            ofdm_key=frame_key,
            N_rb_dl=N_rb_dl,
        )
        self.ofdm_estimator_lte_estimator_hier_0 = lte_estimator_hier(
            initial_id=124,
            estimator_key=frame_key,
            N_rb_dl=N_rb_dl,
        )
        self.bch_decode_bch_hier_gr37_0 = decode_bch_hier_gr37()
        self.MIB = lte.mib_unpack_vbm("MIB")

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.sync_lte_sss_sync_hier_0, 'cell_id'), (self.ofdm_estimator_lte_estimator_hier_0, 'cell_id'))    
        self.msg_connect((self.sync_lte_sss_sync_hier_0, 'cell_id'), (self.pbch_decode_pbch_37_0, 'cell_id'))    
        self.connect((self.bch_decode_bch_hier_gr37_0, 1), (self.MIB, 1))    
        self.connect((self.bch_decode_bch_hier_gr37_0, 0), (self.MIB, 0))    
        self.connect((self.ofdm_estimator_lte_estimator_hier_0, 0), (self.pbch_decode_pbch_37_0, 1))    
        self.connect((self.ofdm_estimator_lte_estimator_hier_0, 1), (self.pbch_decode_pbch_37_0, 2))    
        self.connect((self.ofdm_lte_ofdm_hier_0, 0), (self.ofdm_estimator_lte_estimator_hier_0, 0))    
        self.connect((self.ofdm_lte_ofdm_hier_0, 0), (self.pbch_decode_pbch_37_0, 0))    
        self.connect((self.pbch_decode_pbch_37_0, 0), (self.bch_decode_bch_hier_gr37_0, 0))    
        self.connect((self.pre_blocks_file_source_0, 0), (self.sync_lte_rough_symbol_sync_cc_0, 0))    
        self.connect((self.sync_lte_cp_freq_sync_0, 0), (self.sync_lte_sss_sync_hier_0, 0))    
        self.connect((self.sync_lte_pss_sync_37_0, 0), (self.sync_lte_cp_freq_sync_0, 0))    
        self.connect((self.sync_lte_rough_symbol_sync_cc_0, 0), (self.sync_lte_pss_sync_37_0, 0))    
        self.connect((self.sync_lte_sss_sync_hier_0, 0), (self.ofdm_lte_ofdm_hier_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "lte_top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_fftlen(self):
        return self.fftlen

    def set_fftlen(self, fftlen):
        self.fftlen = fftlen
        self.set_samp_rate((7*self.fftlen + 160* self.fftlen/2048 + 6 * (144 * self.fftlen/2048)) / 0.0005)
        self.sync_lte_pss_sync_37_0.set_fftlen(self.fftlen)
        self.sync_lte_cp_freq_sync_0.set_fftlen(max(256,self.fftlen))
        self.sync_lte_sss_sync_hier_0.set_fftlen(self.fftlen)
        self.ofdm_lte_ofdm_hier_0.set_fftlen(self.fftlen)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_pbch_descr_key(self):
        return self.pbch_descr_key

    def set_pbch_descr_key(self, pbch_descr_key):
        self.pbch_descr_key = pbch_descr_key

    def get_frame_key(self):
        return self.frame_key

    def set_frame_key(self, frame_key):
        self.frame_key = frame_key
        self.ofdm_lte_ofdm_hier_0.set_ofdm_key(self.frame_key)
        self.ofdm_estimator_lte_estimator_hier_0.set_estimator_key(self.frame_key)

    def get_N_rb_dl(self):
        return self.N_rb_dl

    def set_N_rb_dl(self, N_rb_dl):
        self.N_rb_dl = N_rb_dl
        self.sync_lte_sss_sync_hier_0.set_N_rb_dl(self.N_rb_dl)
        self.ofdm_lte_ofdm_hier_0.set_N_rb_dl(self.N_rb_dl)
        self.pbch_decode_pbch_37_0.set_N_rb_dl(self.N_rb_dl)
        self.ofdm_estimator_lte_estimator_hier_0.set_N_rb_dl(self.N_rb_dl)

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    if(StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0")):
        Qt.QApplication.setGraphicsSystem(gr.prefs().get_string('qtgui','style','raster'))
    qapp = Qt.QApplication(sys.argv)
    tb = lte_top_block()
    tb.start()
    tb.show()
    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()
    tb = None #to clean up Qt widgets

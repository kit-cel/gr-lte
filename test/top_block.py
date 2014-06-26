#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: lte_mimo_pss_sync
# Author: Kristian Maier
# Description: hier block for pss time sync
# Generated: Thu Jun 26 02:54:32 2014
##################################################

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import lte
import sys

class top_block(gr.top_block, Qt.QWidget):

    def __init__(self, fftlen=1024):
        gr.top_block.__init__(self, "lte_mimo_pss_sync")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("lte_mimo_pss_sync")
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
        # Parameters
        ##################################################
        self.fftlen = fftlen

        ##################################################
        # Variables
        ##################################################
        self.taps_fftl_1024 = taps_fftl_1024 = [-0.00141422616403692464996655342446274517, 0.015642606095112098624211327546618122142, 0.030243226636871359258584845974837662652, 0.052594337944074170954067426464462187141, 0.078507642615830444676738864018261665478, 0.103921182343643836443192185470252297819, 0.124279358836934944454455376217083539814, 0.135614264806182049927230082175810821354, 0.135614264806182049927230082175810821354, 0.124279358836934944454455376217083539814, 0.103921182343643836443192185470252297819, 0.078507642615830444676738864018261665478, 0.052594337944074170954067426464462187141,0.030243226636871359258584845974837662652,0.015642606095112098624211327546618122142,-0.00141422616403692464996655342446274517]
        self.synclen = synclen = 2
        self.samp_rate = samp_rate = 32000

        ##################################################
        # Blocks
        ##################################################
        self.lte_mimo_pss_fine_sync_0 = lte.mimo_pss_fine_sync(fftlen)
        self.lte_mimo_pss_coarse_sync_0 = lte.mimo_pss_coarse_sync(synclen)
        self.fir_filter_xxx_0_0 = filter.fir_filter_ccc(fftlen/128, (taps_fftl_1024))
        self.fir_filter_xxx_0_0.declare_sample_delay(0)
        self.fir_filter_xxx_0 = filter.fir_filter_ccc(fftlen/128, (taps_fftl_1024))
        self.fir_filter_xxx_0.declare_sample_delay(0)
        self.blocks_vector_to_streams_0 = blocks.vector_to_streams(gr.sizeof_gr_complex*1, 2)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*2, 1e6,True)
        self.blocks_null_sink_1 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_head_0_0_0 = blocks.head(gr.sizeof_gr_complex*1, 400000)
        self.blocks_head_0_0 = blocks.head(gr.sizeof_gr_complex*1, 400000)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*2, "/home/maier/gr-lte/test/lte5framesFadingChannelETU_vec2.dat", False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_throttle_0, 0), (self.blocks_vector_to_streams_0, 0))
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 0), (self.lte_mimo_pss_fine_sync_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 1), (self.lte_mimo_pss_fine_sync_0, 1))
        self.connect((self.lte_mimo_pss_fine_sync_0, 1), (self.blocks_null_sink_1, 0))
        self.connect((self.lte_mimo_pss_fine_sync_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 1), (self.blocks_head_0_0_0, 0))
        self.connect((self.blocks_vector_to_streams_0, 0), (self.blocks_head_0_0, 0))
        self.connect((self.blocks_head_0_0, 0), (self.fir_filter_xxx_0, 0))
        self.connect((self.blocks_head_0_0_0, 0), (self.fir_filter_xxx_0_0, 0))
        self.connect((self.fir_filter_xxx_0_0, 0), (self.lte_mimo_pss_coarse_sync_0, 1))
        self.connect((self.fir_filter_xxx_0, 0), (self.lte_mimo_pss_coarse_sync_0, 0))

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "coarse_pos", self.lte_mimo_pss_fine_sync_0, "coarse_pos")
        self.msg_connect(self.lte_mimo_pss_coarse_sync_0, "N_id_2", self.lte_mimo_pss_fine_sync_0, "N_id_2")

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_fftlen(self):
        return self.fftlen

    def set_fftlen(self, fftlen):
        self.fftlen = fftlen

    def get_taps_fftl_1024(self):
        return self.taps_fftl_1024

    def set_taps_fftl_1024(self, taps_fftl_1024):
        self.taps_fftl_1024 = taps_fftl_1024
        self.fir_filter_xxx_0.set_taps((self.taps_fftl_1024))
        self.fir_filter_xxx_0_0.set_taps((self.taps_fftl_1024))

    def get_synclen(self):
        return self.synclen

    def set_synclen(self, synclen):
        self.synclen = synclen

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

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
    parser.add_option("", "--fftlen", dest="fftlen", type="intx", default=1024,
        help="Set FFT length [default=%default]")
    (options, args) = parser.parse_args()
    Qt.QApplication.setGraphicsSystem(gr.prefs().get_string('qtgui','style','raster'))
    qapp = Qt.QApplication(sys.argv)
    tb = top_block(fftlen=options.fftlen)
    tb.start()
    tb.show()
    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()
    tb = None #to clean up Qt widgets

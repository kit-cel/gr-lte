#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Fri Jan 25 12:06:56 2013
##################################################

from PyQt4 import Qt
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from gnuradio.qtgui import qtgui
from optparse import OptionParser
import PyQt4.Qwt5 as Qwt
import datetime
import sip
import sys

class top_block(gr.top_block, Qt.QWidget):

	def __init__(self):
		gr.top_block.__init__(self, "Top Block")
		Qt.QWidget.__init__(self)
		self.setWindowTitle("Top Block")
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
		self.samp_rate = samp_rate = 10e6
		self.gain = gain = 10
		self.fc = fc = 806e6

		##################################################
		# Blocks
		##################################################
		self._gain_layout = Qt.QVBoxLayout()
		self._gain_tool_bar = Qt.QToolBar(self)
		self._gain_layout.addWidget(self._gain_tool_bar)
		self._gain_tool_bar.addWidget(Qt.QLabel("gain"+": "))
		self._gain_counter = Qwt.QwtCounter()
		self._gain_counter.setRange(0, 31, 1)
		self._gain_counter.setNumButtons(2)
		self._gain_counter.setValue(self.gain)
		self._gain_tool_bar.addWidget(self._gain_counter)
		self._gain_counter.valueChanged.connect(self.set_gain)
		self._gain_slider = Qwt.QwtSlider(None, Qt.Qt.Horizontal, Qwt.QwtSlider.BottomScale, Qwt.QwtSlider.BgSlot)
		self._gain_slider.setRange(0, 31, 1)
		self._gain_slider.setValue(self.gain)
		self._gain_slider.setMinimumWidth(200)
		self._gain_slider.valueChanged.connect(self.set_gain)
		self._gain_layout.addWidget(self._gain_slider)
		self.top_layout.addLayout(self._gain_layout)
		self._fc_layout = Qt.QVBoxLayout()
		self._fc_tool_bar = Qt.QToolBar(self)
		self._fc_layout.addWidget(self._fc_tool_bar)
		self._fc_tool_bar.addWidget(Qt.QLabel("fc"+": "))
		self._fc_counter = Qwt.QwtCounter()
		self._fc_counter.setRange(805e6, 807e6, 1)
		self._fc_counter.setNumButtons(2)
		self._fc_counter.setValue(self.fc)
		self._fc_tool_bar.addWidget(self._fc_counter)
		self._fc_counter.valueChanged.connect(self.set_fc)
		self._fc_slider = Qwt.QwtSlider(None, Qt.Qt.Horizontal, Qwt.QwtSlider.BottomScale, Qwt.QwtSlider.BgSlot)
		self._fc_slider.setRange(805e6, 807e6, 1)
		self._fc_slider.setValue(self.fc)
		self._fc_slider.setMinimumWidth(200)
		self._fc_slider.valueChanged.connect(self.set_fc)
		self._fc_layout.addWidget(self._fc_slider)
		self.top_layout.addLayout(self._fc_layout)
		self.uhd_usrp_source_0 = uhd.usrp_source(
			device_addr="ip=192.168.10.6",
			stream_args=uhd.stream_args(
				cpu_format="fc32",
				channels=range(1),
			),
		)
		self.uhd_usrp_source_0.set_samp_rate(samp_rate)
		self.uhd_usrp_source_0.set_center_freq(fc, 0)
		self.uhd_usrp_source_0.set_gain(gain, 0)
		self.uhd_usrp_source_0.set_antenna("RX2", 0)
		self.qtgui_sink_x_0 = qtgui.sink_c(
			1024, #fftsize
			firdes.WIN_BLACKMAN_hARRIS, #wintype
			0, #fc
			samp_rate, #bw
			"QT GUI Plot", #name
			True, #plotfreq
			False, #plotwaterfall
			False, #plottime
			True, #plotconst
		)
		self._qtgui_sink_x_0_win = sip.wrapinstance(self.qtgui_sink_x_0.pyqwidget(), Qt.QWidget)
		self.top_layout.addWidget(self._qtgui_sink_x_0_win)
		self.gr_file_sink_0 = gr.file_sink(gr.sizeof_gr_complex*1, "/home/demel/gr-lte/data/Measure_LTE_GAIN"+str(gain)+"_DATE"+str(datetime.datetime.now())+".dat")
		self.gr_file_sink_0.set_unbuffered(False)

		##################################################
		# Connections
		##################################################
		self.connect((self.uhd_usrp_source_0, 0), (self.qtgui_sink_x_0, 0))
		self.connect((self.uhd_usrp_source_0, 0), (self.gr_file_sink_0, 0))

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
		self.qtgui_sink_x_0.set_frequency_range(0, self.samp_rate)

	def get_gain(self):
		return self.gain

	def set_gain(self, gain):
		self.gain = gain
		self.uhd_usrp_source_0.set_gain(self.gain, 0)
		self._gain_counter.setValue(self.gain)
		self._gain_slider.setValue(self.gain)
		self.gr_file_sink_0.open("/home/demel/gr-lte/data/Measure_LTE_GAIN"+str(self.gain)+"_DATE"+str(datetime.datetime.now())+".dat")

	def get_fc(self):
		return self.fc

	def set_fc(self, fc):
		self.fc = fc
		self.uhd_usrp_source_0.set_center_freq(self.fc, 0)
		self._fc_counter.setValue(self.fc)
		self._fc_slider.setValue(self.fc)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	qapp = Qt.QApplication(sys.argv)
	tb = top_block()
	tb.start()
	tb.show()
	qapp.exec_()
	tb.stop()


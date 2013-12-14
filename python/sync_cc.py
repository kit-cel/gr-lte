#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2013 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, blocks, window, fft, analog
import lte

class sync_cc(gr.hier_block2):
    """
    docstring for block sync_cc
    """
    def __init__(self, fftl):
        gr.hier_block2.__init__(self,
            "sync_cc",
            gr.io_signature(1, 1, gr.sizeof_gr_complex),  # Input signature
            gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self.message_port_register_hier_out("cell_id")
        N_rb_dl = 6
        # Define blocks and connect them
        self.sym = lte.rough_symbol_sync_cc(fftl)

        # initialize PSS sync blocks
        self.pss_tag = lte.pss_tagger_cc(fftl)
        self.connect(self, self.sym, self.pss_tag)

        self.pss_sel = lte.pss_symbol_selector_cvc(fftl)
        self.pfft = fft.fft_vcc(fftl, True, window.rectangular(fftl), False, 1)
        self.pext = lte.extract_subcarriers_vcvc(N_rb_dl, fftl)
        self.pcal = lte.pss_calculator_vcm(fftl)

        # connect PSS sync blocks
        self.connect(self.sym, self.pss_sel, self.pfft, self.pext, self.pcal)
        self.msg_connect(self.pcal, "lock", self.pss_sel, "lock")
        self.msg_connect(self.pcal, "lock", self.pss_tag, "lock")
        self.msg_connect(self.pcal, "half_frame", self.pss_sel, "half_frame")
        self.msg_connect(self.pcal, "half_frame", self.pss_tag, "half_frame")
        self.msg_connect(self.pcal, "N_id_2", self.pss_tag, "N_id_2")

        # Here is supposed to be the part with frequency sync!
        #waveform = analog.GR_COS_WAVE
        #
        #wave_freq = 0.0
        #ampl = 1.0
        #offset = 0.0
        #cpl = 144 * fftl / 2048
        #cpl0 = 160 * fftl / 2048
        #slotl = 7 * fftl + 6 * cpl + cpl0
        #samp_rate = slotl / 0.0005
        #self.sig = analog.sig_source_c(samp_rate, waveform, wave_freq, ampl, offset)
        #self.multi = blocks.multiply_cc(1)
        #self.est = lte.sync_frequency_c(self.sig, fftl)



        # initialize SSS sync blocks
        #self.sss_tag = lte.sss_tagger_cc(fftl)
        #self.connect(self.sig, (self.multi, 1))
        #self.connect(self.multi, self.est)

        #self.connect(self.pss_tag, self.sss_tag, self)
        #self.connect(self.pss_tag, (self.multi, 0), self.sss_tag, self)
        #self.connect(self.pss_tag, (self.multi, 0), self)
        self.connect(self.pss_tag, self)

        ## connect SSS sync blocks
        #self.sss_sel = lte.sss_symbol_selector_cvc(fftl)
        #self.sfft = fft.fft_vcc(fftl, True, window.rectangular(fftl), False, 1)
        #self.sext = lte.extract_subcarriers_vcvc(N_rb_dl, fftl)
        #self.scal = lte.sss_calculator_vcm(fftl, "N_id_2", "offset_marker")
        #self.connect(self.multi, self.sss_sel, self.sfft, self.sext, self.scal)
        #self.msg_connect(self.scal, "frame_start", self.sss_tag, "frame_start")

        # Why does this not work so far?
        #self.msg_connect(self.scal, "cell_id", self, "cell_id")






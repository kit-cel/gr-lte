#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2013 <+YOU OR YOUR COMPANY+>.
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

from gnuradio import gr, blocks, window, fft
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

        N_rb_dl = 6
        # Define blocks and connect them
        self.sym = lte.rough_symbol_sync_cc(fftl)

        self.pss_tag = lte.pss_tagger_cc(fftl)
        self.connect(self, self.sym, self.pss_tag)

        self.pss_sel = lte.pss_symbol_selector_cvc(fftl)
        self.pfft = fft.fft_vcc(fftl, True, window.rectangular(fftl), False, 1)
        self.pext = lte.extract_subcarriers_vcvc(N_rb_dl, fftl)
        self.pcal = lte.pss_calculator_vcm(fftl)

        self.connect(self.sym, self.pss_sel, self.pfft, self.pext, self.pcal)
        self.msg_connect(self.pcal, "lock", self.pss_sel, "lock")
        self.msg_connect(self.pcal, "lock", self.pss_tag, "lock")
        self.msg_connect(self.pcal, "half_frame", self.pss_sel, "half_frame")
        self.msg_connect(self.pcal, "half_frame", self.pss_tag, "half_frame")
        self.msg_connect(self.pcal, "N_id_2", self.pss_tag, "N_id_2")


        self.sss_tag = lte.sss_tagger_cc(fftl)
        self.connect(self.pss_tag, self.sss_tag, self)

        self.sss_sel = lte.sss_symbol_selector_cvc(fftl)
        self.sfft = fft.fft_vcc(fftl, True, window.rectangular(fftl), False, 1)
        self.sext = lte.extract_subcarriers_vcvc(N_rb_dl, fftl)
        self.scal = lte.sss_calculator_vcm(fftl, "N_id_2", "offset_marker")
        self.connect(self.pss_tag, self.sss_sel, self.sfft, self.sext, self.scal)

        self.message_port_register_hier_out("cell_id")
        self.msg_connect(self.scal, "cell_id", self, "cell_id")
        self.msg_connect(self.scal, "frame_start", self.sss_tag, "frame_start")





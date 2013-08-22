# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Install public header files
########################################################################
install(FILES
    lte_api.h
    lte_correlation.h
    lte_rate_unmatch_vff.h
    lte_descrambling_vfvf.h
    lte_vector_resize_vbvb.h
    lte_block_tests.h
    lte_crc_calculator_vbvb.h
    lte_mib_unpack_vb.h
    lte_qpsk_soft_demod_vcvf.h
    lte_layer_demapper_vcvc.h
    lte_pre_decoder_vcvc.h
    lte_pbch_demux_vcvc.h
    lte_extract_occupied_tones_vcvc.h
    lte_remove_cp_cvc.h
    lte_tag_symbol_cc.h
    lte_freq_estimate_c.h
    lte_sss_selector_cvc.h
    lte_sss_calc_vc.h
    lte_sss_tagging_cc.h
    lte_pss_selector_cvc.h
    lte_pss_calc_vc.h
    lte_pss_tagging_cc.h
    lte_cp_time_freq_sync_cc.h
    lte_zadoff_chu_seq_generator_vc.h
    lte_channel_estimator_vcvc.h
    lte_pcfich_descrambler_vfvf.h
    lte_cfi_unpack_vf.h
    lte_pcfich_demux_vcvc.h
    lte_descrambler_vfvf.h DESTINATION include/lte
)

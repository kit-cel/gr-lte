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

install(FILES
    lte_mib_unpack_vbm.block.yml
    lte_crc_check_vbvb.block.yml
    lte_bch_crc_check_ant_chooser_bb.block.yml
    lte_bch_viterbi_vfvb.block.yml
    lte_subblock_deinterleaver_vfvf.block.yml
    lte_pbch_descrambler_vfvf.block.yml
    lte_repeat_message_source_vf.block.yml
    lte_pbch_scramble_sequencer_m.block.yml
    lte_qpsk_soft_demod_vcvf.block.yml
    lte_layer_demapper_vcvc.block.yml
    lte_pre_decoder_vcvc.block.yml
    lte_pbch_demux_vcvc.block.yml
    lte_channel_estimator_vcvc.block.yml
    lte_rs_map_generator_m.block.yml
    lte_extract_subcarriers_vcvc.block.yml
    lte_remove_cp_cvc.block.yml
    lte_sss_calculator_vcm.block.yml
    lte_pcfich_unpack_vfm.block.yml
    lte_rough_symbol_sync_cc.block.yml
    lte_pss_symbol_selector_cvc.block.yml
    lte_pss_calculator_vcm.block.yml
    lte_pss_tagger_cc.block.yml
    lte_sss_symbol_selector_cvc.block.yml
    lte_sss_tagger_cc.block.yml
    lte_sync_frequency_c.block.yml
    lte_descrambler_vfvf.block.yml
    lte_pcfich_scramble_sequencer_m.block.yml
    lte_pcfich_demux_vcvc.block.yml
    lte_mimo_pss_coarse_sync.block.yml
    lte_mimo_pss_fine_sync.block.yml
    lte_mimo_pss_tagger.block.yml
    lte_mimo_pss_coarse_control.block.yml
    lte_mimo_pss_freq_sync.block.yml
    lte_mimo_sss_symbol_selector.block.yml
    lte_mimo_sss_calculator.block.yml
    lte_mimo_sss_tagger.block.yml
    lte_mimo_remove_cp.block.yml DESTINATION share/gnuradio/grc/blocks
   
)

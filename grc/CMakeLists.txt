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
    lte_mib_unpack_vbm.xml
    lte_crc_check_vbvb.xml
    lte_bch_crc_check_ant_chooser_bb.xml
    lte_bch_viterbi_vfvb.xml
    lte_subblock_deinterleaver_vfvf.xml
    lte_pbch_descrambler_vfvf.xml
    lte_repeat_message_source_vf.xml
    lte_pbch_scramble_sequencer_m.xml
    lte_qpsk_soft_demod_vcvf.xml
    lte_layer_demapper_vcvc.xml
    lte_pre_decoder_vcvc.xml
    lte_pbch_demux_vcvc.xml
    lte_channel_estimator_vcvc.xml
    lte_rs_map_generator_m.xml
    lte_extract_subcarriers_vcvc.xml
    lte_remove_cp_cvc.xml
    lte_sss_calculator_vcm.xml
    lte_pcfich_unpack_vfm.xml
    lte_rough_symbol_sync_cc.xml
    lte_pss_symbol_selector_cvc.xml
    lte_pss_calculator_vcm.xml
    lte_pss_tagger_cc.xml
    lte_sss_symbol_selector_cvc.xml
    lte_sss_tagger_cc.xml
    lte_sync_frequency_c.xml
    lte_descrambler_vfvf.xml
    lte_pcfich_scramble_sequencer_m.xml
    lte_pcfich_demux_vcvc.xml
    lte_mimo_pss_coarse_sync.xml
    lte_mimo_pss_fine_sync.xml
    lte_mimo_pss_tagger.xml
    lte_mimo_pss_coarse_control.xml DESTINATION share/gnuradio/grc/blocks    
   
)

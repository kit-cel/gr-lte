# Copyright 2011,2012 Free Software Foundation, Inc.
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
    api.h
    mib_unpack_vbm.h
    crc_check_vbvb.h
    bch_crc_check_ant_chooser_bb.h
    subblock_deinterleaver_vfvf.h
    pbch_descrambler_vfvf.h
    repeat_message_source_vf.h
    qpsk_soft_demod_vcvf.h
    layer_demapper_vcvc.h
    pre_decoder_vcvc.h
    pbch_demux_vcvc.h
    channel_estimator_vcvc.h
    extract_subcarriers_vcvc.h
    remove_cp_cvc.h
    sss_calculator_vcm.h
    pcfich_unpack_vfm.h
    rough_symbol_sync_cc.h
    pss_symbol_selector_cvc.h
    pss_calculator_vcm.h
    pss_tagger_cc.h
    correlator.h
    sss_symbol_selector_cvc.h
    sss_tagger_cc.h DESTINATION include/lte
)

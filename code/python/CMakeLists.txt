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
# Include python install macros
########################################################################
include(GrPython)
if(NOT PYTHONINTERP_FOUND)
    return()
endif()

########################################################################
# Install python sources
########################################################################
GR_PYTHON_INSTALL(
    FILES
    __init__.py viterbi_vfvb.py hier_freq_estimate_cc.py hier_sss_sync_cc.py hier_pss_sync_cc.py decode_bch_vfvb.py
    decode_pbch_vcvf.py
    estimator_parameterizer_msg.py
    utils.py
    decode_pcfich_vcm.py DESTINATION ${GR_PYTHON_DIR}/lte
)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

set(GR_TEST_TARGET_DEPS gnuradio-lte)
set(GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/swig)
GR_ADD_TEST(qa_rate_unmatch_vff ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_rate_unmatch_vff.py)
GR_ADD_TEST(qa_descrambling_vfvf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_descrambling_vfvf.py)
GR_ADD_TEST(qa_viterbi_vfvb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_viterbi_vfvb.py)
GR_ADD_TEST(qa_vector_resize_vbvb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_vector_resize_vbvb.py)
GR_ADD_TEST(qa_crc_calculator_vbvb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_crc_calculator_vbvb.py)
GR_ADD_TEST(qa_mib_unpack_vb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_mib_unpack_vb.py)
GR_ADD_TEST(qa_qpsk_soft_demod_vcvf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_qpsk_soft_demod_vcvf.py)
GR_ADD_TEST(qa_layer_demapper_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_layer_demapper_vcvc.py)
GR_ADD_TEST(qa_pre_decoder_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_pre_decoder_vcvc.py)
GR_ADD_TEST(qa_pbch_demux_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_pbch_demux_vcvc.py)
GR_ADD_TEST(qa_extract_occupied_tones_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_extract_occupied_tones_vcvc.py)
GR_ADD_TEST(qa_remove_cp_cvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_remove_cp_cvc.py)
GR_ADD_TEST(qa_tag_symbol_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_tag_symbol_cc.py)
GR_ADD_TEST(qa_hier_freq_estimate_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_hier_freq_estimate_cc.py)
GR_ADD_TEST(qa_freq_estimate_c ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_freq_estimate_c.py)
GR_ADD_TEST(qa_hier_sss_sync_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_hier_sss_sync_cc.py)
GR_ADD_TEST(qa_sss_selector_cvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_sss_selector_cvc.py)
GR_ADD_TEST(qa_sss_calc_vc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_sss_calc_vc.py)
GR_ADD_TEST(qa_sss_tagging_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_sss_tagging_cc.py)
GR_ADD_TEST(qa_pss_selector_cvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_pss_selector_cvc.py)
GR_ADD_TEST(qa_pss_calc_vc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_pss_calc_vc.py)
GR_ADD_TEST(qa_pss_tagging_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_pss_tagging_cc.py)
GR_ADD_TEST(qa_hier_pss_sync_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_hier_pss_sync_cc.py)
GR_ADD_TEST(qa_cp_time_freq_sync_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_cp_time_freq_sync_cc.py)
GR_ADD_TEST(qa_zadoff_chu_seq_generator_vc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_zadoff_chu_seq_generator_vc.py)
GR_ADD_TEST(qa_decode_bch_vfvb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte_decode_bch_vfvb.py)
GR_ADD_TEST(qa_decode_pbch_vcvf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_decode_pbch_vcvf.py)
GR_ADD_TEST(qa_channel_estimator_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_channel_estimator_vcvc.py)
GR_ADD_TEST(qa_estimator_parameterizer_msg ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_estimator_parameterizer_msg.py)
GR_ADD_TEST(qa_pcfich_descrambler_vfvf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_pcfich_descrambler_vfvf.py)
GR_ADD_TEST(qa_cfi_unpack_vf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_cfi_unpack_vf.py)
GR_ADD_TEST(qa_pcfich_demux_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_pcfich_demux_vcvc.py)
GR_ADD_TEST(qa_decode_pcfich_vcm ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_decode_pcfich_vcm.py)
GR_ADD_TEST(qa_descrambler_vfvf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_descrambler_vfvf.py)

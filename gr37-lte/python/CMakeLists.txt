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
    __init__.py
    utils.py
    bch_viterbi_vfvb.py
    pbch_scramble_sequencer_m.py
    rs_map_generator_m.py DESTINATION ${GR_PYTHON_DIR}/lte
)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

set(GR_TEST_TARGET_DEPS gnuradio-lte)
set(GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/swig)
GR_ADD_TEST(qa_mib_unpack_vbm ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_mib_unpack_vbm.py)
GR_ADD_TEST(qa_crc_check_vbvb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_crc_check_vbvb.py)
GR_ADD_TEST(qa_bch_crc_check_ant_chooser_bb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_bch_crc_check_ant_chooser_bb.py)
GR_ADD_TEST(qa_bch_viterbi_vfvb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_bch_viterbi_vfvb.py)
GR_ADD_TEST(qa_subblock_deinterleaver_vfvf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_subblock_deinterleaver_vfvf.py)
GR_ADD_TEST(qa_pbch_descrambler_vfvf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_pbch_descrambler_vfvf.py)
GR_ADD_TEST(qa_repeat_message_source_vf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_repeat_message_source_vf.py)
GR_ADD_TEST(qa_pbch_scramble_sequencer_m ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_pbch_scramble_sequencer_m.py)
GR_ADD_TEST(qa_qpsk_soft_demod_vcvf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_qpsk_soft_demod_vcvf.py)
GR_ADD_TEST(qa_layer_demapper_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_layer_demapper_vcvc.py)
GR_ADD_TEST(qa_pre_decoder_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_pre_decoder_vcvc.py)
GR_ADD_TEST(qa_pbch_demux_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_pbch_demux_vcvc.py)
GR_ADD_TEST(qa_channel_estimator_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_channel_estimator_vcvc.py)
GR_ADD_TEST(qa_rs_map_generator_m ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_rs_map_generator_m.py)
GR_ADD_TEST(qa_extract_subcarriers_vcvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_extract_subcarriers_vcvc.py)
GR_ADD_TEST(qa_remove_cp_cvc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_remove_cp_cvc.py)
GR_ADD_TEST(qa_sss_calculator_vcm ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_sss_calculator_vcm.py)
GR_ADD_TEST(qa_pcfich_unpack_vfm ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_pcfich_unpack_vfm.py)

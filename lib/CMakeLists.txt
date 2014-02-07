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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND lte_sources
    mib_unpack_vbm_impl.cc
    crc_check_vbvb_impl.cc
    bch_crc_check_ant_chooser_bb_impl.cc
    subblock_deinterleaver_vfvf_impl.cc
    pbch_descrambler_vfvf_impl.cc
    repeat_message_source_vf_impl.cc
    qpsk_soft_demod_vcvf_impl.cc
    layer_demapper_vcvc_impl.cc
    pre_decoder_vcvc_impl.cc
    pbch_demux_vcvc_impl.cc
    channel_estimator_vcvc_impl.cc
    extract_subcarriers_vcvc_impl.cc
    remove_cp_cvc_impl.cc
    sss_calculator_vcm_impl.cc
    pcfich_unpack_vfm_impl.cc
    rough_symbol_sync_cc_impl.cc
    pss_symbol_selector_cvc_impl.cc
    pss_calculator_vcm_impl.cc
    pss_tagger_cc_impl.cc
    correlator.cc
    sss_symbol_selector_cvc_impl.cc
    sss_tagger_cc_impl.cc
    sync_frequency_c_impl.cc
    descrambler_vfvf_impl.cc
    pcfich_demux_vcvc_impl.cc
)

add_library(gnuradio-lte SHARED ${lte_sources})
target_link_libraries(gnuradio-lte ${Boost_LIBRARIES} ${GNURADIO_RUNTIME_LIBRARIES} ${FFTW3F_LIBRARIES})
set_target_properties(gnuradio-lte PROPERTIES DEFINE_SYMBOL "gnuradio_lte_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-lte
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_lte_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_lte.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_lte.cc
#    ${CMAKE_CURRENT_SOURCE_DIR}/qa_correlator.cc
)

add_executable(test-lte ${test_lte_sources})

target_link_libraries(
  test-lte
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-lte
)

GR_ADD_TEST(test_lte test-lte)

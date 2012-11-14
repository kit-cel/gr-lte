# Copyright 2011 Free Software Foundation, Inc.
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
add_library(gnuradio-lte SHARED lte_rate_unmatch_vff.cc lte_descrambling_vfvf.cc lte_vector_resize_vbvb.cc lte_block_tests.cc lte_crc_calculator_vbvb.cc lte_mib_unpack_vb.cc lte_qpsk_soft_demod_vcvf.cc lte_layer_demapper_vcvc.cc lte_pre_decoder_vcvc.cc lte_pbch_demux_vcvc.cc lte_linear_OFDM_equalizer_vcvc.cc lte_extract_occupied_tones_vcvc.cc lte_remove_cp_cvc.cc lte_tag_symbol_cc.cc lte_freq_estimate_c.cc lte_sss_selector_cvc.cc lte_sss_calc_vci.cc lte_sss_tagging_cc.cc lte_pss_selector_cvc.cc lte_pss_calc_vc.cc lte_pss_tagging_cc.cc lte_cp_time_freq_sync_cc.cc lte_cell_id_daemon.cc lte_zadoff_chu_seq_generator_vc.cc )
target_link_libraries(gnuradio-lte ${Boost_LIBRARIES} ${GRUEL_LIBRARIES} ${GNURADIO_CORE_LIBRARIES})
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
find_package(Boost COMPONENTS unit_test_framework)

include(GrTest)
set(GR_TEST_TARGET_DEPS gnuradio-lte)
#turn each test cpp file into an executable with an int main() function
add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN)

#add_executable(qa_lte_rate_unmatch_vff qa_lte_rate_unmatch_vff.cc)
#target_link_libraries(qa_lte_rate_unmatch_vff gnuradio-lte ${Boost_LIBRARIES})
#GR_ADD_TEST(qa_lte_rate_unmatch_vff qa_lte_rate_unmatch_vff)

#add_executable(qa_lte_descrambling_vfvf qa_lte_descrambling_vfvf.cc)
#target_link_libraries(qa_lte_descrambling_vfvf gnuradio-lte ${Boost_LIBRARIES})
#GR_ADD_TEST(qa_lte_descrambling_vfvf qa_lte_descrambling_vfvf)

#add_executable(qa_lte_vector_resize_vbvb qa_lte_vector_resize_vbvb.cc)
#target_link_libraries(qa_lte_vector_resize_vbvb gnuradio-lte ${Boost_LIBRARIES})
#GR_ADD_TEST(qa_lte_vector_resize_vbvb qa_lte_vector_resize_vbvb)

#add_executable(qa_lte_block_tests qa_lte_block_tests.cc)
#target_link_libraries(qa_lte_block_tests gnuradio-lte ${Boost_LIBRARIES})
#GR_ADD_TEST(qa_lte_block_tests qa_lte_block_tests)

#add_executable(qa_lte_crc_calculator_vbvb qa_lte_crc_calculator_vbvb.cc)
#target_link_libraries(qa_lte_crc_calculator_vbvb gnuradio-lte ${Boost_LIBRARIES})
#GR_ADD_TEST(qa_lte_crc_calculator_vbvb qa_lte_crc_calculator_vbvb)

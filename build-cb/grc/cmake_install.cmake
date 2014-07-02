# Install script for directory: /home/maier/gr-lte/grc

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks" TYPE FILE FILES
    "/home/maier/gr-lte/grc/lte_mib_unpack_vbm.xml"
    "/home/maier/gr-lte/grc/lte_crc_check_vbvb.xml"
    "/home/maier/gr-lte/grc/lte_bch_crc_check_ant_chooser_bb.xml"
    "/home/maier/gr-lte/grc/lte_bch_viterbi_vfvb.xml"
    "/home/maier/gr-lte/grc/lte_subblock_deinterleaver_vfvf.xml"
    "/home/maier/gr-lte/grc/lte_pbch_descrambler_vfvf.xml"
    "/home/maier/gr-lte/grc/lte_repeat_message_source_vf.xml"
    "/home/maier/gr-lte/grc/lte_pbch_scramble_sequencer_m.xml"
    "/home/maier/gr-lte/grc/lte_qpsk_soft_demod_vcvf.xml"
    "/home/maier/gr-lte/grc/lte_layer_demapper_vcvc.xml"
    "/home/maier/gr-lte/grc/lte_pre_decoder_vcvc.xml"
    "/home/maier/gr-lte/grc/lte_pbch_demux_vcvc.xml"
    "/home/maier/gr-lte/grc/lte_channel_estimator_vcvc.xml"
    "/home/maier/gr-lte/grc/lte_rs_map_generator_m.xml"
    "/home/maier/gr-lte/grc/lte_extract_subcarriers_vcvc.xml"
    "/home/maier/gr-lte/grc/lte_remove_cp_cvc.xml"
    "/home/maier/gr-lte/grc/lte_sss_calculator_vcm.xml"
    "/home/maier/gr-lte/grc/lte_pcfich_unpack_vfm.xml"
    "/home/maier/gr-lte/grc/lte_rough_symbol_sync_cc.xml"
    "/home/maier/gr-lte/grc/lte_pss_symbol_selector_cvc.xml"
    "/home/maier/gr-lte/grc/lte_pss_calculator_vcm.xml"
    "/home/maier/gr-lte/grc/lte_pss_tagger_cc.xml"
    "/home/maier/gr-lte/grc/lte_sss_symbol_selector_cvc.xml"
    "/home/maier/gr-lte/grc/lte_sss_tagger_cc.xml"
    "/home/maier/gr-lte/grc/lte_sync_frequency_c.xml"
    "/home/maier/gr-lte/grc/lte_descrambler_vfvf.xml"
    "/home/maier/gr-lte/grc/lte_pcfich_scramble_sequencer_m.xml"
    "/home/maier/gr-lte/grc/lte_pcfich_demux_vcvc.xml"
    "/home/maier/gr-lte/grc/lte_mimo_pss_coarse_sync.xml"
    "/home/maier/gr-lte/grc/lte_mimo_pss_fine_sync.xml"
    "/home/maier/gr-lte/grc/lte_mimo_pss_tagger.xml"
    "/home/maier/gr-lte/grc/lte_mimo_pss_coarse_control.xml"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")


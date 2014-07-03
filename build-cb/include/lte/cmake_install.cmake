# Install script for directory: /home/maier/gr-lte/include/lte

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lte" TYPE FILE FILES
    "/home/maier/gr-lte/include/lte/api.h"
    "/home/maier/gr-lte/include/lte/mib_unpack_vbm.h"
    "/home/maier/gr-lte/include/lte/crc_check_vbvb.h"
    "/home/maier/gr-lte/include/lte/bch_crc_check_ant_chooser_bb.h"
    "/home/maier/gr-lte/include/lte/subblock_deinterleaver_vfvf.h"
    "/home/maier/gr-lte/include/lte/pbch_descrambler_vfvf.h"
    "/home/maier/gr-lte/include/lte/repeat_message_source_vf.h"
    "/home/maier/gr-lte/include/lte/qpsk_soft_demod_vcvf.h"
    "/home/maier/gr-lte/include/lte/layer_demapper_vcvc.h"
    "/home/maier/gr-lte/include/lte/pre_decoder_vcvc.h"
    "/home/maier/gr-lte/include/lte/pbch_demux_vcvc.h"
    "/home/maier/gr-lte/include/lte/channel_estimator_vcvc.h"
    "/home/maier/gr-lte/include/lte/extract_subcarriers_vcvc.h"
    "/home/maier/gr-lte/include/lte/remove_cp_cvc.h"
    "/home/maier/gr-lte/include/lte/sss_calculator_vcm.h"
    "/home/maier/gr-lte/include/lte/pcfich_unpack_vfm.h"
    "/home/maier/gr-lte/include/lte/rough_symbol_sync_cc.h"
    "/home/maier/gr-lte/include/lte/pss_symbol_selector_cvc.h"
    "/home/maier/gr-lte/include/lte/pss_calculator_vcm.h"
    "/home/maier/gr-lte/include/lte/pss_tagger_cc.h"
    "/home/maier/gr-lte/include/lte/correlator.h"
    "/home/maier/gr-lte/include/lte/sss_symbol_selector_cvc.h"
    "/home/maier/gr-lte/include/lte/sss_tagger_cc.h"
    "/home/maier/gr-lte/include/lte/sync_frequency_c.h"
    "/home/maier/gr-lte/include/lte/descrambler_vfvf.h"
    "/home/maier/gr-lte/include/lte/pcfich_demux_vcvc.h"
    "/home/maier/gr-lte/include/lte/mimo_pss_coarse_sync.h"
    "/home/maier/gr-lte/include/lte/mimo_pss_fine_sync.h"
    "/home/maier/gr-lte/include/lte/mimo_pss_tagger.h"
    "/home/maier/gr-lte/include/lte/mimo_pss_coarse_control.h"
    "/home/maier/gr-lte/include/lte/pss.h"
    "/home/maier/gr-lte/include/lte/mimo_pss_freq_sync.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")


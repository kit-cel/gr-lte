# Install script for directory: /home/maier/gr-lte/python

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/lte" TYPE FILE FILES
    "/home/maier/gr-lte/python/__init__.py"
    "/home/maier/gr-lte/python/utils.py"
    "/home/maier/gr-lte/python/bch_viterbi_vfvb.py"
    "/home/maier/gr-lte/python/pbch_scramble_sequencer_m.py"
    "/home/maier/gr-lte/python/rs_map_generator_m.py"
    "/home/maier/gr-lte/python/pcfich_scramble_sequencer_m.py"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/lte" TYPE FILE FILES
    "/home/maier/gr-lte/build-cb/python/__init__.pyc"
    "/home/maier/gr-lte/build-cb/python/utils.pyc"
    "/home/maier/gr-lte/build-cb/python/bch_viterbi_vfvb.pyc"
    "/home/maier/gr-lte/build-cb/python/pbch_scramble_sequencer_m.pyc"
    "/home/maier/gr-lte/build-cb/python/rs_map_generator_m.pyc"
    "/home/maier/gr-lte/build-cb/python/pcfich_scramble_sequencer_m.pyc"
    "/home/maier/gr-lte/build-cb/python/__init__.pyo"
    "/home/maier/gr-lte/build-cb/python/utils.pyo"
    "/home/maier/gr-lte/build-cb/python/bch_viterbi_vfvb.pyo"
    "/home/maier/gr-lte/build-cb/python/pbch_scramble_sequencer_m.pyo"
    "/home/maier/gr-lte/build-cb/python/rs_map_generator_m.pyo"
    "/home/maier/gr-lte/build-cb/python/pcfich_scramble_sequencer_m.pyo"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")


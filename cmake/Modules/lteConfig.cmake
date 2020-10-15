INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_LTE lte)

FIND_PATH(
    LTE_INCLUDE_DIRS
    NAMES lte/api.h
    HINTS $ENV{LTE_DIR}/include
        ${PC_LTE_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    LTE_LIBRARIES
    NAMES gnuradio-lte
    HINTS $ENV{LTE_DIR}/lib
        ${PC_LTE_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/lteTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LTE DEFAULT_MSG LTE_LIBRARIES LTE_INCLUDE_DIRS)
MARK_AS_ADVANCED(LTE_LIBRARIES LTE_INCLUDE_DIRS)

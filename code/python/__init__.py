#
# Copyright 2008,2009 Free Software Foundation, Inc.
#
# This application is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This application is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

# The presence of this file turns this directory into a Python package

'''
This is the GNU Radio LTE module. Place your Python package
description here (python/__init__.py).
'''

# ----------------------------------------------------------------
# Temporary workaround for ticket:181 (swig+python problem)
import sys
_RTLD_GLOBAL = 0
try:
    from dl import RTLD_GLOBAL as _RTLD_GLOBAL
except ImportError:
    try:
	from DLFCN import RTLD_GLOBAL as _RTLD_GLOBAL
    except ImportError:
	pass

if _RTLD_GLOBAL != 0:
    _dlopenflags = sys.getdlopenflags()
    sys.setdlopenflags(_dlopenflags|_RTLD_GLOBAL)
# ----------------------------------------------------------------


# import swig generated symbols into the lte namespace
from lte_swig import *

# import any pure python here
#
from viterbi_vfvb import viterbi_vfvb
from hier_freq_estimate_cc import hier_freq_estimate_cc
from hier_sss_sync_cc import hier_sss_sync_cc
from hier_sss_sync2_vcvc import hier_sss_sync2_vcvc
from hier_pss_sync_cc import hier_pss_sync_cc
from linear_OFDM_estimator_hier_vcvc import linear_OFDM_estimator_hier_vcvc
from decode_bch_vfvb import decode_bch_vfvb
from decode_pbch_vcvf import decode_pbch_vcvf
from estimator_parameterizer_msg import estimator_parameterizer_msg


# ----------------------------------------------------------------
# Tail of workaround
if _RTLD_GLOBAL != 0:
    sys.setdlopenflags(_dlopenflags)      # Restore original flags
# ----------------------------------------------------------------

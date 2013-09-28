/* -*- c++ -*- */

#define LTE_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "lte_swig_doc.i"

%{
#include "lte/mib_unpack_vbm.h"
#include "lte/crc_check_vbvb.h"
#include "lte/bch_crc_check_ant_chooser_bb.h"
%}


%include "lte/mib_unpack_vbm.h"
GR_SWIG_BLOCK_MAGIC2(lte, mib_unpack_vbm);

%include "lte/crc_check_vbvb.h"
GR_SWIG_BLOCK_MAGIC2(lte, crc_check_vbvb);
%include "lte/bch_crc_check_ant_chooser_bb.h"
GR_SWIG_BLOCK_MAGIC2(lte, bch_crc_check_ant_chooser_bb);

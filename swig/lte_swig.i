/* -*- c++ -*- */

#define LTE_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "lte_swig_doc.i"

%{
#include "lte/mib_unpack_vbm.h"
#include "lte/crc_check_vbvb.h"
#include "lte/bch_crc_check_ant_chooser_bb.h"
#include "lte/subblock_deinterleaver_vfvf.h"
#include "lte/pbch_descrambler_vfvf.h"
#include "lte/repeat_message_source_vf.h"
#include "lte/qpsk_soft_demod_vcvf.h"
#include "lte/layer_demapper_vcvc.h"
#include "lte/pre_decoder_vcvc.h"
#include "lte/pbch_demux_vcvc.h"
#include "lte/channel_estimator_vcvc.h"
#include "lte/extract_subcarriers_vcvc.h"
#include "lte/remove_cp_cvc.h"
#include "lte/sss_calculator_vcm.h"
#include "lte/pcfich_unpack_vfm.h"
#include "lte/rough_symbol_sync_cc.h"
#include "lte/pss_symbol_selector_cvc.h"
#include "lte/pss_calculator_vcm.h"
#include "lte/pss_tagger_cc.h"
#include "lte/correlator.h"
#include "lte/sss_symbol_selector_cvc.h"
#include "lte/sss_tagger_cc.h"
#include "lte/sync_frequency_c.h"
#include "lte/descrambler_vfvf.h"
#include "lte/pcfich_demux_vcvc.h"
#include "lte/mimo_pss_coarse_sync.h"
#include "lte/mimo_pss_fine_sync.h"
#include "lte/mimo_pss_tagger.h"
#include "lte/mimo_pss_coarse_control.h"
#include "lte/pss.h"
#include "lte/mimo_pss_freq_sync.h"
#include "lte/mimo_sss_symbol_selector.h"
#include "lte/mimo_sss_calculator.h"
#include "lte/mimo_sss_tagger.h"
#include "lte/mimo_remove_cp.h"
#include "lte/mimo_channel_estimator.h"
#include "lte/mimo_pbch_demux.h"
#include "lte/mimo_pre_decoder.h"
%}


%include "lte/mib_unpack_vbm.h"
GR_SWIG_BLOCK_MAGIC2(lte, mib_unpack_vbm);

%include "lte/crc_check_vbvb.h"
GR_SWIG_BLOCK_MAGIC2(lte, crc_check_vbvb);
%include "lte/bch_crc_check_ant_chooser_bb.h"
GR_SWIG_BLOCK_MAGIC2(lte, bch_crc_check_ant_chooser_bb);
%include "lte/subblock_deinterleaver_vfvf.h"
GR_SWIG_BLOCK_MAGIC2(lte, subblock_deinterleaver_vfvf);
%include "lte/pbch_descrambler_vfvf.h"
GR_SWIG_BLOCK_MAGIC2(lte, pbch_descrambler_vfvf);
%include "lte/repeat_message_source_vf.h"
GR_SWIG_BLOCK_MAGIC2(lte, repeat_message_source_vf);
%include "lte/qpsk_soft_demod_vcvf.h"
GR_SWIG_BLOCK_MAGIC2(lte, qpsk_soft_demod_vcvf);
%include "lte/layer_demapper_vcvc.h"
GR_SWIG_BLOCK_MAGIC2(lte, layer_demapper_vcvc);
%include "lte/pre_decoder_vcvc.h"
GR_SWIG_BLOCK_MAGIC2(lte, pre_decoder_vcvc);
%include "lte/pbch_demux_vcvc.h"
GR_SWIG_BLOCK_MAGIC2(lte, pbch_demux_vcvc);
%include "lte/channel_estimator_vcvc.h"
GR_SWIG_BLOCK_MAGIC2(lte, channel_estimator_vcvc);
%include "lte/extract_subcarriers_vcvc.h"
GR_SWIG_BLOCK_MAGIC2(lte, extract_subcarriers_vcvc);
%include "lte/remove_cp_cvc.h"
GR_SWIG_BLOCK_MAGIC2(lte, remove_cp_cvc);
%include "lte/sss_calculator_vcm.h"
GR_SWIG_BLOCK_MAGIC2(lte, sss_calculator_vcm);
%include "lte/pcfich_unpack_vfm.h"
GR_SWIG_BLOCK_MAGIC2(lte, pcfich_unpack_vfm);
%include "lte/rough_symbol_sync_cc.h"
GR_SWIG_BLOCK_MAGIC2(lte, rough_symbol_sync_cc);
%include "lte/pss_symbol_selector_cvc.h"
GR_SWIG_BLOCK_MAGIC2(lte, pss_symbol_selector_cvc);
%include "lte/pss_calculator_vcm.h"
GR_SWIG_BLOCK_MAGIC2(lte, pss_calculator_vcm);
%include "lte/pss_tagger_cc.h"
GR_SWIG_BLOCK_MAGIC2(lte, pss_tagger_cc);
%include "lte/correlator.h"
%include "lte/sss_symbol_selector_cvc.h"
GR_SWIG_BLOCK_MAGIC2(lte, sss_symbol_selector_cvc);
%include "lte/sss_tagger_cc.h"
GR_SWIG_BLOCK_MAGIC2(lte, sss_tagger_cc);
%include "lte/sync_frequency_c.h"
GR_SWIG_BLOCK_MAGIC2(lte, sync_frequency_c);
%include "lte/descrambler_vfvf.h"
GR_SWIG_BLOCK_MAGIC2(lte, descrambler_vfvf);
%include "lte/pcfich_demux_vcvc.h"
GR_SWIG_BLOCK_MAGIC2(lte, pcfich_demux_vcvc);
%include "lte/mimo_pss_coarse_sync.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_pss_coarse_sync);
%include "lte/mimo_pss_fine_sync.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_pss_fine_sync);

%include "lte/mimo_pss_tagger.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_pss_tagger);
%include "lte/mimo_pss_coarse_control.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_pss_coarse_control);
%include "lte/pss.h"
%include "lte/mimo_pss_freq_sync.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_pss_freq_sync);
%include "lte/mimo_sss_symbol_selector.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_sss_symbol_selector);
%include "lte/mimo_sss_calculator.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_sss_calculator);

%include "lte/mimo_sss_tagger.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_sss_tagger);
%include "lte/mimo_remove_cp.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_remove_cp);

%include "lte/mimo_channel_estimator.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_channel_estimator);
%include "lte/mimo_pbch_demux.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_pbch_demux);
%include "lte/mimo_pre_decoder.h"
GR_SWIG_BLOCK_MAGIC2(lte, mimo_pre_decoder);

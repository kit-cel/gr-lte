/* -*- c++ -*- */

#define LTE_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "lte_swig_doc.i"


%{
#include "lte_rate_unmatch_vff.h"
#include "lte_descrambling_vfvf.h"
#include "lte_vector_resize_vbvb.h"
#include "lte_block_tests.h"
#include "lte_crc_calculator_vbvb.h"
#include "lte_mib_unpack_vb.h"
#include "lte_qpsk_soft_demod_vcvf.h"
#include "lte_layer_demapper_vcvc.h"
#include "lte_pre_decoder_vcvc.h"
#include "lte_pbch_demux_vcvc.h"
#include "lte_extract_occupied_tones_vcvc.h"
#include "lte_remove_cp_cvc.h"
#include "lte_tag_symbol_cc.h"
#include "lte_freq_estimate_c.h"
#include "lte_sss_selector_cvc.h"
#include "lte_sss_calc_vc.h"
#include "lte_sss_tagging_cc.h"
#include "lte_pss_selector_cvc.h"
#include "lte_pss_calc_vc.h"
#include "lte_pss_tagging_cc.h"
#include "lte_cp_time_freq_sync_cc.h"
#include "lte_zadoff_chu_seq_generator_vc.h"
#include "lte_channel_estimator_vcvc.h"
#include "lte_pcfich_descrambler_vfvf.h"
#include "lte_cfi_unpack_vf.h"
#include "lte_pcfich_demux_vcvc.h"
%}


GR_SWIG_BLOCK_MAGIC(lte,rate_unmatch_vff);
%include "lte_rate_unmatch_vff.h"

GR_SWIG_BLOCK_MAGIC(lte,descrambling_vfvf);
%include "lte_descrambling_vfvf.h"

GR_SWIG_BLOCK_MAGIC(lte,vector_resize_vbvb);
%include "lte_vector_resize_vbvb.h"

GR_SWIG_BLOCK_MAGIC(lte,block_tests);
%include "lte_block_tests.h"

GR_SWIG_BLOCK_MAGIC(lte,crc_calculator_vbvb);
%include "lte_crc_calculator_vbvb.h"

GR_SWIG_BLOCK_MAGIC(lte,mib_unpack_vb);
%include "lte_mib_unpack_vb.h"

GR_SWIG_BLOCK_MAGIC(lte,qpsk_soft_demod_vcvf);
%include "lte_qpsk_soft_demod_vcvf.h"

GR_SWIG_BLOCK_MAGIC(lte,layer_demapper_vcvc);
%include "lte_layer_demapper_vcvc.h"

GR_SWIG_BLOCK_MAGIC(lte,pre_decoder_vcvc);
%include "lte_pre_decoder_vcvc.h"

GR_SWIG_BLOCK_MAGIC(lte,pbch_demux_vcvc);
%include "lte_pbch_demux_vcvc.h"

GR_SWIG_BLOCK_MAGIC(lte,extract_occupied_tones_vcvc);
%include "lte_extract_occupied_tones_vcvc.h"

GR_SWIG_BLOCK_MAGIC(lte,remove_cp_cvc);
%include "lte_remove_cp_cvc.h"

GR_SWIG_BLOCK_MAGIC(lte,tag_symbol_cc);
%include "lte_tag_symbol_cc.h"

GR_SWIG_BLOCK_MAGIC(lte,freq_estimate_c);
%include "lte_freq_estimate_c.h"

GR_SWIG_BLOCK_MAGIC(lte,sss_selector_cvc);
%include "lte_sss_selector_cvc.h"

GR_SWIG_BLOCK_MAGIC(lte,sss_calc_vc);
%include "lte_sss_calc_vc.h"

GR_SWIG_BLOCK_MAGIC(lte,sss_tagging_cc);
%include "lte_sss_tagging_cc.h"

GR_SWIG_BLOCK_MAGIC(lte,pss_selector_cvc);
%include "lte_pss_selector_cvc.h"

GR_SWIG_BLOCK_MAGIC(lte,pss_calc_vc);
%include "lte_pss_calc_vc.h"

GR_SWIG_BLOCK_MAGIC(lte,pss_tagging_cc);
%include "lte_pss_tagging_cc.h"

GR_SWIG_BLOCK_MAGIC(lte,cp_time_freq_sync_cc);
%include "lte_cp_time_freq_sync_cc.h"

GR_SWIG_BLOCK_MAGIC(lte,zadoff_chu_seq_generator_vc);
%include "lte_zadoff_chu_seq_generator_vc.h"

GR_SWIG_BLOCK_MAGIC(lte, channel_estimator_vcvc);
%include "lte_channel_estimator_vcvc.h"
GR_SWIG_BLOCK_MAGIC(lte, pcfich_descrambler_vfvf);
%include "lte_pcfich_descrambler_vfvf.h"
GR_SWIG_BLOCK_MAGIC(lte, cfi_unpack_vf);
%include "lte_cfi_unpack_vf.h"
GR_SWIG_BLOCK_MAGIC(lte, pcfich_demux_vcvc);
%include "lte_pcfich_demux_vcvc.h"

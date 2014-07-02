
/*
 * This file was automatically generated using swig_doc.py.
 *
 * Any changes to it will be lost next time it is regenerated.
 */




%feature("docstring") gr::lte::bch_crc_check_ant_chooser_bb "Depending on the results of the CRC block return number of antennas detected."

%feature("docstring") gr::lte::bch_crc_check_ant_chooser_bb::make "Return a shared_ptr to a new instance of lte::bch_crc_check_ant_chooser_bb.

To avoid accidental use of raw pointers, lte::bch_crc_check_ant_chooser_bb's constructor is in a private implementation class. lte::bch_crc_check_ant_chooser_bb::make is the public interface for creating new instances.

Params: (name)"

%feature("docstring") gr::lte::channel_estimator_vcvc "Channel Estimator for OFDM."

%feature("docstring") gr::lte::channel_estimator_vcvc::make "Return a shared_ptr to a new instance of lte::channel_estimator_vcvc.

To avoid accidental use of raw pointers, lte::channel_estimator_vcvc's constructor is in a private implementation class. lte::channel_estimator_vcvc::make is the public interface for creating new instances.

Params: (subcarriers, tag_key, msg_buf_name, pilot_carriers, pilot_symbols, name)"

%feature("docstring") gr::lte::channel_estimator_vcvc::set_pilot_map "

Params: (pilot_carriers, pilot_symbols)"

%feature("docstring") gr::lte::channel_estimator_vcvc::get_pilot_carriers "

Params: (NONE)"

%feature("docstring") gr::lte::correlator "This is a helper class to provide Fast Correlation using FFTW3f."

%feature("docstring") gr::lte::correlator::correlator "

Params: (in1, in2, out, len)"

%feature("docstring") gr::lte::correlator::~correlator "

Params: (NONE)"

%feature("docstring") gr::lte::correlator::execute "

Params: (NONE)"

%feature("docstring") gr::lte::correlator::get_maximum "

Params: (pos, max)"

%feature("docstring") gr::lte::crc_check_vbvb "Calculate CRC and check if it is equal to received CRC."

%feature("docstring") gr::lte::crc_check_vbvb::make "Return a shared_ptr to a new instance of lte::crc_check_vbvb.

To avoid accidental use of raw pointers, lte::crc_check_vbvb's constructor is in a private implementation class. lte::crc_check_vbvb::make is the public interface for creating new instances.

Params: (data_len, final_xor, name)"

%feature("docstring") gr::lte::descrambler_vfvf "LTE Descrambler

Scrambling sequences are setable on runtime and chosen upon tag reception."

%feature("docstring") gr::lte::descrambler_vfvf::set_descr_seqs "

Params: (seqs)"

%feature("docstring") gr::lte::descrambler_vfvf::make "Return a shared_ptr to a new instance of lte::descrambler_vfvf.

To avoid accidental use of raw pointers, lte::descrambler_vfvf's constructor is in a private implementation class. lte::descrambler_vfvf::make is the public interface for creating new instances.

Params: (tag_key, msg_buf_name, len, name)"

%feature("docstring") gr::lte::extract_subcarriers_vcvc "Extract occupied subcarriers in frequency domain."

%feature("docstring") gr::lte::extract_subcarriers_vcvc::make "Return a shared_ptr to a new instance of lte::extract_subcarriers_vcvc.

To avoid accidental use of raw pointers, lte::extract_subcarriers_vcvc's constructor is in a private implementation class. lte::extract_subcarriers_vcvc::make is the public interface for creating new instances.

Params: (N_rb_dl, fftl, name)"

%feature("docstring") gr::lte::layer_demapper_vcvc "Layer Demapper."

%feature("docstring") gr::lte::layer_demapper_vcvc::make "Return a shared_ptr to a new instance of lte::layer_demapper_vcvc.

To avoid accidental use of raw pointers, lte::layer_demapper_vcvc's constructor is in a private implementation class. lte::layer_demapper_vcvc::make is the public interface for creating new instances.

Params: (N_ant, vlen, style, name)"

%feature("docstring") gr::lte::layer_demapper_vcvc::set_N_ant "

Params: (N_ant)"

%feature("docstring") gr::lte::layer_demapper_vcvc::get_N_ant "

Params: (NONE)"

%feature("docstring") gr::lte::layer_demapper_vcvc::set_decoding_style "

Params: (style)"

%feature("docstring") gr::lte::layer_demapper_vcvc::get_decoding_style "

Params: (NONE)"

%feature("docstring") gr::lte::mib_unpack_vbm "Block unpacks MIB and publishes the parameters as messages."

%feature("docstring") gr::lte::mib_unpack_vbm::make "Return a shared_ptr to a new instance of lte::mib_unpack_vbm.

To avoid accidental use of raw pointers, lte::mib_unpack_vbm's constructor is in a private implementation class. lte::mib_unpack_vbm::make is the public interface for creating new instances.

Params: (name)"

%feature("docstring") gr::lte::mimo_pss_coarse_control "<+description of block+>"

%feature("docstring") gr::lte::mimo_pss_coarse_control::make "Return a shared_ptr to a new instance of lte::mimo_pss_coarse_control.

To avoid accidental use of raw pointers, lte::mimo_pss_coarse_control's constructor is in a private implementation class. lte::mimo_pss_coarse_control::make is the public interface for creating new instances.

Params: (NONE)"

%feature("docstring") gr::lte::mimo_pss_coarse_sync "<+description of block+>"

%feature("docstring") gr::lte::mimo_pss_coarse_sync::make "Return a shared_ptr to a new instance of lte::mimo_pss_coarse_sync.

To avoid accidental use of raw pointers, lte::mimo_pss_coarse_sync's constructor is in a private implementation class. lte::mimo_pss_coarse_sync::make is the public interface for creating new instances.

Params: (syncl, rxant)"

%feature("docstring") gr::lte::mimo_pss_fine_sync "<+description of block+>"

%feature("docstring") gr::lte::mimo_pss_fine_sync::make "Return a shared_ptr to a new instance of lte::mimo_pss_fine_sync.

To avoid accidental use of raw pointers, lte::mimo_pss_fine_sync's constructor is in a private implementation class. lte::mimo_pss_fine_sync::make is the public interface for creating new instances.

Params: (fftl, rxant, grpdelay)"

%feature("docstring") gr::lte::mimo_pss_tagger "<+description of block+>"

%feature("docstring") gr::lte::mimo_pss_tagger::make "Return a shared_ptr to a new instance of lte::mimo_pss_tagger.

To avoid accidental use of raw pointers, lte::mimo_pss_tagger's constructor is in a private implementation class. lte::mimo_pss_tagger::make is the public interface for creating new instances.

Params: (fftl)"

%feature("docstring") gr::lte::pbch_demux_vcvc "Demultiplex PBCH data from resource grid."

%feature("docstring") gr::lte::pbch_demux_vcvc::make "Return a shared_ptr to a new instance of lte::pbch_demux_vcvc.

To avoid accidental use of raw pointers, lte::pbch_demux_vcvc's constructor is in a private implementation class. lte::pbch_demux_vcvc::make is the public interface for creating new instances.

Params: (N_rb_dl, name)"

%feature("docstring") gr::lte::pbch_demux_vcvc::set_cell_id "

Params: (id)"

%feature("docstring") gr::lte::pbch_descrambler_vfvf "Block performs descrambling of PBCH with the given Cell ID

Cell ID is passed to block via message port on runtime."

%feature("docstring") gr::lte::pbch_descrambler_vfvf::make "Return a shared_ptr to a new instance of lte::pbch_descrambler_vfvf.

To avoid accidental use of raw pointers, lte::pbch_descrambler_vfvf's constructor is in a private implementation class. lte::pbch_descrambler_vfvf::make is the public interface for creating new instances.

Params: (key)"

%feature("docstring") gr::lte::pbch_descrambler_vfvf::set_cell_id "

Params: (id)"

%feature("docstring") gr::lte::pbch_descrambler_vfvf::pn_sequence "

Params: (NONE)"

%feature("docstring") gr::lte::pcfich_demux_vcvc "Demux PCFICH from resource grid."

%feature("docstring") gr::lte::pcfich_demux_vcvc::set_cell_id "

Params: (id)"

%feature("docstring") gr::lte::pcfich_demux_vcvc::make "Return a shared_ptr to a new instance of lte::pcfich_demux_vcvc.

To avoid accidental use of raw pointers, lte::pcfich_demux_vcvc's constructor is in a private implementation class. lte::pcfich_demux_vcvc::make is the public interface for creating new instances.

Params: (N_rb_dl, key, out_key, msg_buf_name, name)"

%feature("docstring") gr::lte::pcfich_unpack_vfm "Unpack CFI and publish it on output message port."

%feature("docstring") gr::lte::pcfich_unpack_vfm::activate_debug_mode "

Params: (ena)"

%feature("docstring") gr::lte::pcfich_unpack_vfm::cfi_results "

Params: (NONE)"

%feature("docstring") gr::lte::pcfich_unpack_vfm::make "Return a shared_ptr to a new instance of lte::pcfich_unpack_vfm.

To avoid accidental use of raw pointers, lte::pcfich_unpack_vfm's constructor is in a private implementation class. lte::pcfich_unpack_vfm::make is the public interface for creating new instances.

Params: (key, msg_buf_name, name)"

%feature("docstring") gr::lte::pre_decoder_vcvc "Pre Decoder for LTE."

%feature("docstring") gr::lte::pre_decoder_vcvc::make "Return a shared_ptr to a new instance of lte::pre_decoder_vcvc.

To avoid accidental use of raw pointers, lte::pre_decoder_vcvc's constructor is in a private implementation class. lte::pre_decoder_vcvc::make is the public interface for creating new instances.

Params: (N_ant, vlen, style, name)"

%feature("docstring") gr::lte::pre_decoder_vcvc::set_N_ant "

Params: (N_ant)"

%feature("docstring") gr::lte::pre_decoder_vcvc::get_N_ant "

Params: (NONE)"

%feature("docstring") gr::lte::pre_decoder_vcvc::set_decoding_style "

Params: (style)"

%feature("docstring") gr::lte::pre_decoder_vcvc::get_decoding_style "

Params: (NONE)"

%feature("docstring") gr::lte::pss "helper class for pss blocks"

%feature("docstring") gr::lte::pss::pss "

Params: (NONE)"

%feature("docstring") gr::lte::pss::~pss "

Params: (NONE)"

%feature("docstring") gr::lte::pss::gen_pss_t "

Params: (zc_t, cell_id, len)"

%feature("docstring") gr::lte::pss::zc "

Params: (zc, cell_id)"

%feature("docstring") gr::lte::pss_calculator_vcm "<+description of block+>"

%feature("docstring") gr::lte::pss_calculator_vcm::make "Return a shared_ptr to a new instance of lte::pss_calculator_vcm.

To avoid accidental use of raw pointers, lte::pss_calculator_vcm's constructor is in a private implementation class. lte::pss_calculator_vcm::make is the public interface for creating new instances.

Params: (fftl, name)"

%feature("docstring") gr::lte::pss_symbol_selector_cvc "Extract symbols where PSS is assumed."

%feature("docstring") gr::lte::pss_symbol_selector_cvc::make "Return a shared_ptr to a new instance of lte::pss_symbol_selector_cvc.

To avoid accidental use of raw pointers, lte::pss_symbol_selector_cvc's constructor is in a private implementation class. lte::pss_symbol_selector_cvc::make is the public interface for creating new instances.

Params: (fftl, name)"

%feature("docstring") gr::lte::pss_tagger_cc "<+description of block+>"

%feature("docstring") gr::lte::pss_tagger_cc::make "Return a shared_ptr to a new instance of lte::pss_tagger_cc.

To avoid accidental use of raw pointers, lte::pss_tagger_cc's constructor is in a private implementation class. lte::pss_tagger_cc::make is the public interface for creating new instances.

Params: (fftl, name)"

%feature("docstring") gr::lte::pss_tagger_cc::set_half_frame_start "

Params: (start)"

%feature("docstring") gr::lte::pss_tagger_cc::set_N_id_2 "

Params: (nid2)"

%feature("docstring") gr::lte::pss_tagger_cc::lock "

Params: (NONE)"

%feature("docstring") gr::lte::pss_tagger_cc::unlock "

Params: (NONE)"

%feature("docstring") gr::lte::qpsk_soft_demod_vcvf "QPSK soft demodulation. 1. item == real, 2. item == imaginary."

%feature("docstring") gr::lte::qpsk_soft_demod_vcvf::make "Return a shared_ptr to a new instance of lte::qpsk_soft_demod_vcvf.

To avoid accidental use of raw pointers, lte::qpsk_soft_demod_vcvf's constructor is in a private implementation class. lte::qpsk_soft_demod_vcvf::make is the public interface for creating new instances.

Params: (vlen, name)"

%feature("docstring") gr::lte::remove_cp_cvc "Remove LTE specific CP from stream and output OFDM symbol vectors in time domain."

%feature("docstring") gr::lte::remove_cp_cvc::make "Return a shared_ptr to a new instance of lte::remove_cp_cvc.

To avoid accidental use of raw pointers, lte::remove_cp_cvc's constructor is in a private implementation class. lte::remove_cp_cvc::make is the public interface for creating new instances.

Params: (fftl, key, name)"

%feature("docstring") gr::lte::repeat_message_source_vf "Repeat Vector in message infinitely."

%feature("docstring") gr::lte::repeat_message_source_vf::make "Return a shared_ptr to a new instance of lte::repeat_message_source_vf.

To avoid accidental use of raw pointers, lte::repeat_message_source_vf's constructor is in a private implementation class. lte::repeat_message_source_vf::make is the public interface for creating new instances.

Params: (vector_len, name)"

%feature("docstring") gr::lte::rough_symbol_sync_cc "CP based rough sync to OFDM symbols."

%feature("docstring") gr::lte::rough_symbol_sync_cc::make "Return a shared_ptr to a new instance of lte::rough_symbol_sync_cc.

To avoid accidental use of raw pointers, lte::rough_symbol_sync_cc's constructor is in a private implementation class. lte::rough_symbol_sync_cc::make is the public interface for creating new instances.

Params: (fftl, vlen, name)"

%feature("docstring") gr::lte::sss_calculator_vcm "Detect SSS in symbol."

%feature("docstring") gr::lte::sss_calculator_vcm::make "Return a shared_ptr to a new instance of lte::sss_calculator_vcm.

To avoid accidental use of raw pointers, lte::sss_calculator_vcm's constructor is in a private implementation class. lte::sss_calculator_vcm::make is the public interface for creating new instances.

Params: (fftl, key_id, key_offset, name)"

%feature("docstring") gr::lte::sss_calculator_vcm::get_cell_id "

Params: (NONE)"

%feature("docstring") gr::lte::sss_calculator_vcm::get_frame_start "

Params: (NONE)"

%feature("docstring") gr::lte::sss_symbol_selector_cvc "<+description of block+>"

%feature("docstring") gr::lte::sss_symbol_selector_cvc::make "Return a shared_ptr to a new instance of lte::sss_symbol_selector_cvc.

To avoid accidental use of raw pointers, lte::sss_symbol_selector_cvc's constructor is in a private implementation class. lte::sss_symbol_selector_cvc::make is the public interface for creating new instances.

Params: (fftl, name)"

%feature("docstring") gr::lte::sss_tagger_cc "<+description of block+>"

%feature("docstring") gr::lte::sss_tagger_cc::make "Return a shared_ptr to a new instance of lte::sss_tagger_cc.

To avoid accidental use of raw pointers, lte::sss_tagger_cc's constructor is in a private implementation class. lte::sss_tagger_cc::make is the public interface for creating new instances.

Params: (fftl, name)"

%feature("docstring") gr::lte::subblock_deinterleaver_vfvf "Deinterleave groups of items according to LTE subblock interleaver."

%feature("docstring") gr::lte::subblock_deinterleaver_vfvf::make "Return a shared_ptr to a new instance of lte::subblock_deinterleaver_vfvf.

To avoid accidental use of raw pointers, lte::subblock_deinterleaver_vfvf's constructor is in a private implementation class. lte::subblock_deinterleaver_vfvf::make is the public interface for creating new instances.

Params: (num_groups, items_per_group, name)"

%feature("docstring") gr::lte::sync_frequency_c "Calculate fractional frequency offset

This block calculates FFO by correlating CPs and sets the frequency of a signal source block."

%feature("docstring") gr::lte::sync_frequency_c::make "Return a shared_ptr to a new instance of lte::sync_frequency_c.

To avoid accidental use of raw pointers, lte::sync_frequency_c's constructor is in a private implementation class. lte::sync_frequency_c::make is the public interface for creating new instances.

Params: (sig, fftl, name)"
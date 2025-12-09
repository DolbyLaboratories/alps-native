/***********************************************************************************************************************
 * Copyright (C) 2024-2025 by Dolby International AB.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 *    disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *    following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************************************************************/

#ifndef RAW_AC4_FRAME_TOC_ONLY_H
#define RAW_AC4_FRAME_TOC_ONLY_H

#include <bam.h>
struct BAMSyntax *  F_raw_ac4_frame_toc_only_create();
bam_error F_raw_ac4_frame_toc_only_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_toc_create();
bam_error F_ac4_ac4_toc_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_variable_bits_create();
bam_error F_ac4_variable_bits_parse(BAMint * BAM_result, BAMSyntax * BAM_syntax, BAMint ac4_n_bits, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_presentation_info_create();
bam_error F_ac4_ac4_presentation_info_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_presentation_version_create();
bam_error F_ac4_presentation_version_parse(BAMint * BAM_result, BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_frame_rate_multiply_info_create();
bam_error F_ac4_frame_rate_multiply_info_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_substream_info_create();
bam_error F_ac4_ac4_substream_info_parse(BAMSyntax * BAM_syntax, BAMint ac4_si_b_associated, BAMint ac4_si_b_dialog, BAMint ac4_si_ps_index, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_define_channel_modes_create();
bam_error F_ac4_define_channel_modes_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_channel_mode_create();
bam_error F_ac4_channel_mode_parse(BAMint * BAM_result, BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_bitrate_indicator_create();
bam_error F_ac4_bitrate_indicator_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_content_type_create();
bam_error F_ac4_content_type_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_channel_mode_to_ch_mode_create();
bam_error F_ac4_channel_mode_to_ch_mode_parse(BAMint * BAM_result, BAMSyntax * BAM_syntax, BAMint ac4_cm, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_get_num_channels_create();
bam_error F_ac4_get_num_channels_parse(BAMint * BAM_result, BAMSyntax * BAM_syntax, BAMint ac4_cm, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_channel_mode_contains_LFE_create();
bam_error F_ac4_channel_mode_contains_LFE_parse(BAMint * BAM_result, BAMSyntax * BAM_syntax, BAMint ac4_cm, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_hsf_ext_substream_info_create();
bam_error F_ac4_ac4_hsf_ext_substream_info_parse(BAMSyntax * BAM_syntax, BAMint ac4_ahsf_b_substreams_present, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_presentation_config_ext_info_create();
bam_error F_ac4_presentation_config_ext_info_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_personalization_info_create();
bam_error F_ac4_personalization_info_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_emdf_info_create();
bam_error F_ac4_emdf_info_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_emdf_payloads_substream_info_create();
bam_error F_ac4_emdf_payloads_substream_info_parse(BAMSyntax * BAM_syntax, BAMint ac4_toc_i, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_emdf_protection_create();
bam_error F_emdf_protection_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_presentation_v2_info_create();
bam_error F_ac4_ac4_presentation_v2_info_parse(BAMSyntax * BAM_syntax, BAMint ac4_apv_toc_i, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_frame_rate_fractions_info_create();
bam_error F_ac4_frame_rate_fractions_info_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_sgi_specifier_create();
bam_error F_ac4_ac4_sgi_specifier_parse(BAMSyntax * BAM_syntax, BAMint ac4_asgi_b_associated, BAMint ac4_asgi_b_dialog, BAMint ac4_group_in_presentation_index, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_presentation_substream_info_create();
bam_error F_ac4_ac4_presentation_substream_info_parse(BAMSyntax * BAM_syntax, BAMint ac4_aps_presentation_version, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_substream_group_info_create();
bam_error F_ac4_ac4_substream_group_info_parse(BAMSyntax * BAM_syntax, BAMint ac4_si_group_index, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_substream_info_chan_create();
bam_error F_ac4_ac4_substream_info_chan_parse(BAMSyntax * BAM_syntax, BAMint ac4_asio_group_index, BAMint ac4_asio_gs_index, BAMint ac4_asio_b_substreams_present_chan, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_define_oamd_create();
bam_error F_ac4_define_oamd_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_oamd_substream_info_create();
bam_error F_ac4_oamd_substream_info_parse(BAMSyntax * BAM_syntax, BAMint ac4_oamd_b_substreams_present, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_substream_info_ajoc_create();
bam_error F_ac4_ac4_substream_info_ajoc_parse(BAMSyntax * BAM_syntax, BAMint ac4_asio_group_index, BAMint ac4_asio_gs_index_ajoc, BAMint ac4_asio_b_substreams_present, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_oamd_common_data_create();
bam_error F_ac4_oamd_common_data_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_trim_create();
bam_error F_ac4_trim_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_bed_render_info_create();
bam_error F_ac4_bed_render_info_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_stereo_dmx_coeff_create();
bam_error F_ac4_stereo_dmx_coeff_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_tool_t2_to_f_s_b_create();
bam_error F_ac4_tool_t2_to_f_s_b_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_tool_t2_to_f_s_create();
bam_error F_ac4_tool_t2_to_f_s_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_tool_tb_to_f_s_b_create();
bam_error F_ac4_tool_tb_to_f_s_b_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_tool_tb_to_f_s_create();
bam_error F_ac4_tool_tb_to_f_s_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_tool_tf_to_f_s_b_create();
bam_error F_ac4_tool_tf_to_f_s_b_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_tool_tf_to_f_s_create();
bam_error F_ac4_tool_tf_to_f_s_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_common_headphone_md_create();
bam_error F_ac4_common_headphone_md_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_bed_dyn_obj_assignment_create();
bam_error F_ac4_bed_dyn_obj_assignment_parse(BAMSyntax * BAM_syntax, BAMint ac4_n_signals, BAMint ac4_b_upmix, BAMint ac4_bd_lfe, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_ac4_substream_info_obj_create();
bam_error F_ac4_ac4_substream_info_obj_parse(BAMSyntax * BAM_syntax, BAMint ac4_asio_group_index, BAMint ac4_asio_gs_index, BAMint ac4_asio_gs_index_obj, BAMint ac4_asio_b_substreams_present, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_get_ch_mode_create();
bam_error F_ac4_get_ch_mode_parse(BAMint * BAM_result, BAMSyntax * BAM_syntax, BAMint ac4_cm, BAMSource * BAM_source, BAMSink * BAM_sink);
struct BAMSyntax *  F_ac4_substream_index_table_create();
bam_error F_ac4_substream_index_table_parse(BAMSyntax * BAM_syntax, BAMSource * BAM_source, BAMSink * BAM_sink);

#endif /* RAW_AC4_FRAME_TOC_ONLY_H */

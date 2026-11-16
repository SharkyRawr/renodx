#ifndef SRC_GAMES_OBENSEUER_SHARED_H_
#define SRC_GAMES_OBENSEUER_SHARED_H_

struct ShaderInjectData {
  float peak_white_nits;
  float diffuse_white_nits;
  float graphics_white_nits;
  float output_mode;
  float tone_map_type;
  float exposure;
  float highlights;
  float shadows;
  float contrast;
  float saturation;
  float highlight_saturation;
  float gamma_correction;
};

#ifndef __cplusplus
#if __SHADER_TARGET_MAJOR >= 6
cbuffer shader_injection : register(b13, space50) {
#else
cbuffer shader_injection : register(b13) {
#endif
  ShaderInjectData shader_injection : packoffset(c0);
}
#define RENODX_PEAK_WHITE_NITS               shader_injection.peak_white_nits
#define RENODX_DIFFUSE_WHITE_NITS            shader_injection.diffuse_white_nits
#define RENODX_GRAPHICS_WHITE_NITS           shader_injection.graphics_white_nits
#define RENODX_TONE_MAP_TYPE                 shader_injection.tone_map_type
#define RENODX_TONE_MAP_EXPOSURE             shader_injection.exposure
#define RENODX_TONE_MAP_HIGHLIGHTS           shader_injection.highlights
#define RENODX_TONE_MAP_SHADOWS              shader_injection.shadows
#define RENODX_TONE_MAP_CONTRAST             shader_injection.contrast
#define RENODX_TONE_MAP_SATURATION           shader_injection.saturation
#define RENODX_TONE_MAP_HIGHLIGHT_SATURATION shader_injection.highlight_saturation
#define RENODX_RENO_DRT_TONE_MAP_METHOD      renodx::tonemap::renodrt::config::tone_map_method::NEUTWO
#define RENODX_RENO_DRT_SCALING_METHOD       renodx::tonemap::renodrt::config::scaling_method::MAX_CHANNEL
#define RENODX_TONE_MAP_WORKING_COLOR_SPACE  renodx::color::convert::COLOR_SPACE_BT2020
#define RENODX_TONE_MAP_CLAMP_PEAK           renodx::color::convert::COLOR_SPACE_NONE
#define RENODX_TONE_MAP_HUE_CORRECTION       0.f
#define RENODX_GAMMA_CORRECTION              shader_injection.gamma_correction
#define RENODX_INTERMEDIATE_ENCODING         0.f
#define RENODX_SWAP_CHAIN_DECODING           0.f
#define RENODX_SWAP_CHAIN_OUTPUT_PRESET      shader_injection.output_mode
#include "../../shaders/renodx.hlsl"
#endif
#endif  // SRC_GAMES_OBENSEUER_SHARED_H_

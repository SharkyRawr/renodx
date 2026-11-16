#include "./shared.h"
Texture2D<float4> t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 position : SV_POSITION, float2 uv : TEXCOORD0) : SV_TARGET {
  float4 color = t0.Sample(s0, uv);
  // The swapchain is always HDR10/PQ (same as the generic Unity addon), so SDR
  // content is simply left in SDR range inside the HDR10 container.
  // Scale to nits, then use the generic addon's hue-preserving LMS gamut compression.
  float3 hdr = max(0.f, color.rgb) * shader_injection.graphics_white_nits;
  hdr = renodx::color::lms::from::BT709(hdr);
  hdr = renodx::color::gamut::GamutCompressLMSBoundBT2020(hdr);
  hdr = renodx::color::bt709::from::LMS(hdr);
  float max_channel = max(max(max(hdr.r, hdr.g), hdr.b), shader_injection.peak_white_nits);
  hdr *= shader_injection.peak_white_nits / max_channel;
  hdr = renodx::color::bt2020::from::BT709(hdr);
  hdr = renodx::color::pq::EncodeSafe(hdr, 1.f);
  return float4(hdr, color.a);
}

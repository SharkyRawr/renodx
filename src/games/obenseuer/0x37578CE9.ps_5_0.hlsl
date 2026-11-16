#include "./shared.h"

// ---- Created with 3Dmigoto v1.3.16 on Thu Nov 12 19:48:34 2026
Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[8];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[17];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xw = float2(0,0);
  r0.yz = cb0[2].yx;
  r1.xyz = v2.xyw + -r0.xyx;
  r1.xyzw = t1.SampleLevel(s1_s, r1.xy, r1.z).xyzw;
  r1.x = cb1[7].x * r1.x + cb1[7].y;
  r1.x = 1 / r1.x;
  r1.yzw = v2.xyw + r0.xyx;
  r2.xyzw = t1.SampleLevel(s1_s, r1.yz, r1.w).xyzw;
  r1.y = cb1[7].x * r2.x + cb1[7].y;
  r1.y = 1 / r1.y;
  r1.z = max(r1.y, r1.x);
  r1.x = min(r1.y, r1.x);
  r2.xyz = v2.xyw + -r0.zww;
  r2.xyzw = t1.SampleLevel(s1_s, r2.xy, r2.z).xyzw;
  r1.y = cb1[7].x * r2.x + cb1[7].y;
  r1.y = 1 / r1.y;
  r1.z = max(r1.z, r1.y);
  r2.xyz = v2.xyw + r0.zww;
  r2.xyzw = t1.SampleLevel(s1_s, r2.xy, r2.z).xyzw;
  r1.w = cb1[7].x * r2.x + cb1[7].y;
  r1.w = 1 / r1.w;
  r1.z = max(r1.z, r1.w);
  r1.x = min(r1.x, r1.y);
  r1.x = min(r1.x, r1.w);
  r1.x = r1.z + -r1.x;
  r1.x = 9.99999975e-006 + r1.x;
  r1.x = saturate(cb0[6].y / r1.x);
  r2.xyzw = v1.xyxy + r0.zwxy;
  r0.xyzw = v1.xyxy + -r0.xyzw;
  r3.xyzw = t0.Sample(s0_s, r2.zw).xyzw;
  r2.xyzw = t0.Sample(s0_s, r2.xy).xyzw;
  r1.z = dot(r2.xyz, float3(0.298999995,0.587000012,0.114));
  r1.w = dot(r3.xyz, float3(0.298999995,0.587000012,0.114));
  r2.xyzw = t0.Sample(s0_s, r0.xy).xyzw;
  r0.xyzw = t0.Sample(s0_s, r0.zw).xyzw;
  r0.x = dot(r0.xyz, float3(0.298999995,0.587000012,0.114));
  r0.y = dot(r2.xyz, float3(0.298999995,0.587000012,0.114));
  r0.z = min(r1.w, r0.y);
  r0.y = max(r1.w, r0.y);
  r0.y = max(r0.y, r0.x);
  r0.x = min(r0.z, r0.x);
  r0.x = min(r0.x, r1.z);
  r0.y = max(r0.y, r1.z);
  r0.x = -9.99999997e-007 + r0.x;
  r0.z = r0.y + -r0.x;
  r0.z = saturate(cb0[6].w / r0.z);
  r2.xyzw = t0.Sample(s0_s, v1.xy).xyzw;
  r0.w = dot(r2.xyz, float3(0.298999995,0.587000012,0.114));
  r0.x = r0.w * 2 + -r0.x;
  r0.x = r0.x + -r0.y;
  r0.x = r0.x * r0.z;
  r0.x = r0.x * r1.x;
  r0.x = cb0[6].x * r0.x;
  r0.x = max(-cb0[6].z, r0.x);
  r0.x = min(cb0[6].z, r0.x);
  r0.y = -cb0[7].z + r1.y;
  r0.z = cmp(r1.y >= cb0[7].y);
  r0.z = r0.z ? 1.000000 : 0;
  r0.z = cb0[7].x * r0.z;
  r0.y = cmp(abs(r0.y) < cb0[7].w);
  r0.y = r0.y ? 1.000000 : 0;
  r0.x = r0.x * r0.y + 1;
  r1.xyz = saturate(r2.xyz);
  r1.xyz = float3(1,1,1) + -r1.xyz;
  r3.xyz = r2.xyz * r1.yxx;
  r1.xyz = r3.xyz * r1.zzy;
  r1.xyz = r1.xyz * cb0[5].www + float3(1,1,1);
  r1.xyz = r2.xyz * r1.xyz;
  o0.w = r2.w;
  r0.y = dot(r1.xyz, float3(0.298999995,0.587000012,0.114));
  r0.y = 9.99999975e-005 + r0.y;
  r0.y = r0.w / r0.y;
  r1.xyz = r1.xyz * r0.yyy;
  r2.xyzw = t2.Sample(s2_s, v1.xy).xyzw;
  r2.xyz = cb0[12].xxx * r2.xyz;
  r0.xyw = r1.xyz * r0.xxx + r2.xyz;
  r1.x = dot(r0.xyw, float3(0.298999995,0.587000012,0.114));
  r1.x = 1 + r1.x;
  r1.x = log2(r1.x);
  r1.x = 0.693147182 * r1.x;
  r2.xyzw = t3.Sample(s4_s, float2(0.5,0.5)).xyzw;
  r1.y = 9.99999975e-005 + r2.x;
  r1.x = r1.x / r1.y;
  r1.x = log2(r1.x);
  r2.xyzw = t4.Sample(s3_s, float2(0.5,0.5)).xyzw;
  r1.y = r2.x / r1.y;
  r1.y = -1 + r1.y;
  r1.x = abs(r1.y) * r1.x;
  r1.x = exp2(r1.x);
  r1.x = max(cb0[16].x, r1.x);
  r1.x = min(cb0[16].y, r1.x);
  r1.yzw = r1.xxx * r0.xyw;
  r2.x = max(r1.z, r1.w);
  r2.x = max(r2.x, r1.y);
  r2.y = min(r1.z, r1.w);
  r2.y = min(r2.y, r1.y);
  r2.x = saturate(r2.x + -r2.y);
  r2.x = 1 + -r2.x;
  r2.x = cb0[5].z * r2.x;
  r2.y = dot(r1.yzw, float3(0.298999995,0.587000012,0.114));
  r0.xyw = r0.xyw * r1.xxx + -r2.yyy;
  r0.xyw = r2.xxx * r0.xyw + float3(1,1,1);
  r0.xyw = r1.yzw * r0.xyw;
  r1.xyz = r0.xyw * cb0[9].xyz + -r0.xyw;
  r0.xyw = cb0[9].www * r1.xyz + r0.xyw;
  r0.xyw = float3(-0.5,-0.5,-0.5) + r0.xyw;
  r0.xyw = r0.xyw * cb0[5].yyy + float3(0.5,0.5,0.5);
  r0.xyw = cb0[5].xxx * r0.xyw;
  r1.xy = cb1[6].xy * v1.xy;
  r1.x = dot(float2(171,231), r1.xy);
  r1.xyz = float3(0.00970873795,0.0140845068,0.010309278) * r1.xxx;
  r1.xyz = frac(r1.xyz);
  r1.xyz = float3(-0.5,-0.5,-0.5) + r1.xyz;
  r1.xyz = r0.zzz * r1.xyz + float3(1,1,1);
  o0.xyz = r1.xyz * r0.xyw;
  if (shader_injection.output_mode != 0.f) {
    o0.rgb = renodx::draw::ToneMapPass(o0.rgb);
    o0.rgb = renodx::draw::RenderIntermediatePass(o0.rgb);
  }
  return;
}

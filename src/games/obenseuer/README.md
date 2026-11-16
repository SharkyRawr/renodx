# Obenseuer

Experimental D3D11 HDR10 support for Obenseuer v0.5.0 (Unity 6000.0.77f1, x64 Mono).
The addon leaves the game's SDR pipeline and 3D LUT intact and applies RenoDRT to
the game's graded SDR output, after sharpening, color effects and vignette. The
swapchain stays HDR10/PQ for both content modes; SDR content is kept in SDR range
inside the HDR10 container instead of switching the DXGI color space at runtime.

## Reverse-engineered rendering path

The game renders through a Unity postprocessing chain. Shaders below were dumped
and decompiled with 3Dmigoto 1.3.16.

- `0x7FBDAFA3` is the gameplay 3D LUT pass. It samples the RGBA16F scene at `t0`,
  clips to `[0, 1]`, converts to sRGB, samples a 16x16x16 UNORM 3D LUT at `t1`,
  and decodes and blends the grade. `cb0[2].xy` supplies the LUT scale/offset and
  `.z` supplies grade strength. The HDR branch compresses through the same LUT
  and reconstructs scene range before downstream processing. The addon no longer
  replaces this shader.
- `0xB58368D6` is the gameplay final scene pass. It sharpens and applies the
  game's color and vignette effects before writing to the original sRGB
  backbuffer. Its decompiled `float2 TEXCOORD2` was emitted as `w1.yz`; the
  original DXBC `v1.wz` proves the correct swizzle is `w1.yx`.
- `0x37578CE9` is the title-scene variant: sharpening, bloom, exposure and
  grading. `0x49E25D6C` then copies its RGBA16F result to the backbuffer before
  menu text and version UI. Neither gameplay replacement is used on this path.

UI shaders `0x066C98CB` and `0x55B0DCB7` draw after the scene. Their original
linear outputs are preserved in the floating-point backbuffer proxy, and UI white
is applied at final encoding.

Two LUT candidates reported by the generic Unity detector were false positives:
`0xC32E5F94` composites two textures and `0xCC8B6ACF` performs FXAA/dithering;
neither reads `t4`. The file named `uber_old_0x066C98CB.ps_4_0.cso` hashes to
`0xCEEAA577`, so it is not an original baseline.

## Addon behavior

The addon upgrades `0xB58368D6` and `0x37578CE9` with `ToneMapPass` followed by
`RenderIntermediatePass` when HDR10 content is selected. `0x7FBDAFA3` remains the
game's original pass. RenoDRT uses BT.2020 max-channel scaling, so the output peak
limit scales RGB together and preserves saturated highlight colors instead of
clipping individual channels.

Controls: SDR/HDR10 content, Vanilla/RenoDRT, peak brightness, scene brightness,
UI brightness, exposure, highlights, shadows, contrast, saturation, highlight
saturation, and gamma correction. Game and UI white auto-calibrate to the
display's SDR white level on first swapchain creation while still at their
defaults. SDR content is the default; Preset Off selects SDR content and Vanilla.
HDR10 output is experimental.

Deploy `renodx-obenseuer.addon64` beside the game executable and load it with
ReShade. Do not load the generic Unity addon for the same game.

## Linux build

Only the Obenseuer addon and its build-time DXBC compiler are built. Use
`clang-cl` and `lld-link` with the Microsoft CRT and Windows SDK headers and
libraries provided by [xwin](https://github.com/Jake-Shadle/xwin). MinGW builds
are ABI-incompatible with ReShade's C++ structure-return methods; use the
Microsoft target even when cross-compiling from Linux.

Prepare an xwin SDK. The CMake default is `~/.cache/renodx-toolchain/msvc`;
override it with `XWIN_ROOT`:

```sh
xwin --accept-license --cache-dir ~/.cache/renodx-toolchain/download \
  splat --output ~/.cache/renodx-toolchain/msvc
```

Place the **x64** `d3dcompiler_46.dll` from the upstream 3Dmigoto 1.3.16 archive
at `bin/D3DCOMPILER_46_x64.dll`. The cmd_Decompiler archive contains a 32-bit
compiler DLL and cannot be used by this build-time executable.

```sh
cmake -S src/games/obenseuer -B build/obenseuer-clang -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DWINE_EXECUTABLE=/path/to/proton/files/bin/wine \
  -DWINE_PREFIX=/path/to/wine-prefix
cmake --build build/obenseuer-clang --target obenseuer -j2
ctest --test-dir build/obenseuer-clang --output-on-failure
```

The build-time shader compiler runs under Wine and only needs a working prefix.
Output: `build/obenseuer-clang/renodx-obenseuer.addon64`. Generated shader
headers and `compile_commands.json` are written to the build directory. The addon
statically links the Microsoft runtime and has no shader-compiler DLL dependency.

The root Windows build can discover this folder through `addon.cpp`, although it
was not tested here. The local CMake entry point keeps the cross-compilation
setup scoped to this addon.

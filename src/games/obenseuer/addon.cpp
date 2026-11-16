/*
 * Copyright (C) 2024 Carlos Lopez
 * SPDX-License-Identifier: MIT
 */
#define ImTextureID ImU64
#define DEBUG_LEVEL_0
#include <deps/imgui/imgui.h>
#include <embed/shaders.h>
#include <windows.h>
#include <include/reshade.hpp>
#include "../../mods/shader.hpp"
#include "../../mods/swapchain.hpp"
#include "../../utils/settings.hpp"
#include "./shared.h"

namespace {
ShaderInjectData shader_injection;
static_assert(sizeof(ShaderInjectData) == 12 * sizeof(float));
renodx::mods::shader::CustomShaders custom_shaders = {
    {0x37578CE9, renodx::mods::shader::CreateCustomShader(0x37578CE9, __0x37578CE9)},
    {0xB58368D6, renodx::mods::shader::CreateCustomShader(0xB58368D6, __0xB58368D6)},
};
renodx::utils::settings::Settings settings = {
    new renodx::utils::settings::Setting{
        .key = "PeakNits",
        .binding = &shader_injection.peak_white_nits,
        .default_value = 1000.f,
        .label = "Peak Brightness",
        .min = 80.f,
        .max = 4000.f,
        .format = "%.1f",
    },
    new renodx::utils::settings::Setting{
        .key = "GameNits",
        .binding = &shader_injection.diffuse_white_nits,
        .default_value = 203.f,
        .label = "Game Brightness",
        .min = 48.f,
        .max = 500.f,
        .format = "%.1f",
    },
    new renodx::utils::settings::Setting{
        .key = "UINits",
        .binding = &shader_injection.graphics_white_nits,
        .default_value = 203.f,
        .label = "UI Brightness",
        .min = 48.f,
        .max = 500.f,
        .format = "%.1f",
    },
    new renodx::utils::settings::Setting{
        .key = "Exposure",
        .binding = &shader_injection.exposure,
        .default_value = 1.f,
        .label = "Exposure",
        .min = 0.f,
        .max = 4.f,
        .format = "%.2f",
    },
    new renodx::utils::settings::Setting{
        .key = "Highlights",
        .binding = &shader_injection.highlights,
        .default_value = 1.f,
        .label = "Highlights",
        .min = 0.f,
        .max = 2.f,
        .format = "%.2f",
    },
    new renodx::utils::settings::Setting{
        .key = "Shadows",
        .binding = &shader_injection.shadows,
        .default_value = 1.f,
        .label = "Shadows",
        .min = 0.f,
        .max = 2.f,
        .format = "%.2f",
    },
    new renodx::utils::settings::Setting{
        .key = "Contrast",
        .binding = &shader_injection.contrast,
        .default_value = 1.f,
        .label = "Contrast",
        .min = 0.f,
        .max = 2.f,
        .format = "%.2f",
    },
    new renodx::utils::settings::Setting{
        .key = "Saturation",
        .binding = &shader_injection.saturation,
        .default_value = 1.f,
        .label = "Saturation",
        .min = 0.f,
        .max = 2.f,
        .format = "%.2f",
    },
    new renodx::utils::settings::Setting{
        .key = "HighlightSaturation",
        .binding = &shader_injection.highlight_saturation,
        .default_value = 1.f,
        .label = "Highlight Saturation",
        .min = 0.f,
        .max = 2.f,
        .format = "%.2f",
    },
    new renodx::utils::settings::Setting{
        .key = "GammaCorrection",
        .binding = &shader_injection.gamma_correction,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f,
        .label = "Gamma Correction",
        .tooltip = "Match the SDR display response in HDR; Off keeps linear scene light.",
        .labels = {"Off", "2.2", "2.4"},
    },
    new renodx::utils::settings::Setting{
        .key = "OutputMode",
        .binding = &shader_injection.output_mode,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Content",
        .tooltip = "SDR keeps the game's vanilla SDR grade inside the HDR10 container; HDR10 applies the RenoDRT upgrade.",
        .labels = {"SDR", "HDR10"},
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapType",
        .binding = &shader_injection.tone_map_type,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f,
        .label = "Tone Mapper",
        .labels = {"Vanilla", "RenoDRT"},
        .parse = [](float value) { return value == 0.f ? 0.f : 3.f; },
    },
};
void OnPresetOff() {
  renodx::utils::settings::UpdateSetting("OutputMode", 0.f);
  renodx::utils::settings::UpdateSetting("ToneMapType", 0.f);
}
// Match the game/UI white to the display's SDR white level once per session.
// A fixed 203-nit default is visibly dimmer than SDR on displays that report higher.
void CalibrateWhiteNits(reshade::api::swapchain* swapchain) {
  static bool calibrated = false;
  if (calibrated) return;
  calibrated = true;
  const auto sdr_white_nits = renodx::utils::swapchain::GetSDRWhiteNits(swapchain);
  if (!sdr_white_nits.has_value() || sdr_white_nits.value() < 80.f) return;
  for (const auto* key : {"GameNits", "UINits"}) {
    auto* setting = renodx::utils::settings::FindSetting(key);
    if (setting != nullptr && setting->GetValue() == setting->default_value) {
      renodx::utils::settings::UpdateSetting(key, sdr_white_nits.value());
    }
  }
}
void OnInitSwapchain(reshade::api::swapchain* swapchain, bool) {
  CalibrateWhiteNits(swapchain);
}
}  // namespace

extern "C" __declspec(dllexport) constexpr const char* NAME = "RenoDX for Obenseuer";
extern "C" __declspec(dllexport) constexpr const char* DESCRIPTION = "Obenseuer HDR10, display calibration and RenoDRT upgrade (experimental)";

BOOL APIENTRY DllMain(HMODULE h_module, DWORD fdw_reason, LPVOID) {
  switch (fdw_reason) {
    case DLL_PROCESS_ATTACH:
      if (!reshade::register_addon(h_module)) return FALSE;
      // Keep this addon's settings separate from the generic Unity addon's shared "renodx" section.
      renodx::utils::settings::global_name = "renodx-obenseuer";
      renodx::mods::shader::force_pipeline_cloning = true;
      renodx::mods::shader::expected_constant_buffer_index = 13;
      renodx::mods::shader::expected_constant_buffer_space = 50;
      renodx::mods::shader::allow_multiple_push_constants = true;
      renodx::mods::swapchain::SetUseHDR10(true);
      renodx::mods::swapchain::use_resource_cloning = true;
      renodx::mods::swapchain::expected_constant_buffer_index = 13;
      renodx::mods::swapchain::expected_constant_buffer_space = 50;
      renodx::mods::swapchain::swap_chain_proxy_shaders = {
          {reshade::api::device_api::d3d11, {
                                                .vertex_shader = __swap_chain_proxy_vertex_shader,
                                                .pixel_shader = __swap_chain_proxy_pixel_shader,
                                            }},
      };
      reshade::register_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
      break;
    case DLL_PROCESS_DETACH:
      reshade::unregister_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
      reshade::unregister_addon(h_module);
      break;
  }
  renodx::utils::settings::Use(fdw_reason, &settings, &OnPresetOff);
  renodx::mods::swapchain::Use(fdw_reason, &shader_injection);
  renodx::mods::shader::Use(fdw_reason, custom_shaders, &shader_injection);
  return TRUE;
}

#include <d3dcompiler.h>
#include <windows.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

namespace {
class Includes final : public ID3DInclude {
 public:
  std::filesystem::path root;
  struct Source {
    std::filesystem::path path;
    std::vector<char> data;
  };
  std::list<Source> sources;
  HRESULT STDMETHODCALLTYPE Open(D3D_INCLUDE_TYPE, LPCSTR name, LPCVOID parent,
                                 LPCVOID* data, UINT* size) noexcept override {
    try {
      auto directory = root;
      for (const auto& source : sources) {
        if (source.data.data() == parent) {
          directory = source.path.parent_path();
          break;
        }
      }
      auto path = (directory / name).lexically_normal();
      std::ifstream input(path, std::ios::binary);
      if (!input) return E_FAIL;
      sources.push_back({.path = path, .data = std::vector<char>(std::istreambuf_iterator<char>(input), {})});
      *data = sources.back().data.data();
      *size = static_cast<UINT>(sources.back().data.size());
      return S_OK;
    } catch (...) {
      return E_FAIL;
    }
  }
  HRESULT STDMETHODCALLTYPE Close(LPCVOID) noexcept override {
    // Keep parent buffers alive until compilation finishes so nested paths remain available.
    return S_OK;
  }
};
}  // namespace

// Build-only DXBC compiler; the addon has no compiler DLL dependency.
int main(int argc, char** argv) {
  if (argc != 5) {
    std::cerr << "Usage: compile_shader input profile output-header symbol\n";
    return 1;
  }
  const auto compiler = LoadLibraryW(L"D3DCompiler_46.dll");
  if (compiler == nullptr) {
    std::cerr << "D3DCompiler_46.dll is required beside this executable\n";
    return 1;
  }
  const auto compile = reinterpret_cast<decltype(&D3DCompileFromFile)>(GetProcAddress(compiler, "D3DCompileFromFile"));
  if (compile == nullptr) {
    FreeLibrary(compiler);
    return 1;
  }
  ID3DBlob* code = nullptr;
  ID3DBlob* errors = nullptr;
  const D3D_SHADER_MACRO defines[] = {{"__SHADER_TARGET_MAJOR", "5"}, {"__SHADER_TARGET_MINOR", "0"}, {nullptr, nullptr}};
  Includes includes;
  includes.root = std::filesystem::path(argv[1]).parent_path();
  const auto result = compile(std::filesystem::path(argv[1]).make_preferred().c_str(), defines,
                              &includes, "main", argv[2],
                              D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &code, &errors);
  if (errors != nullptr) {
    std::cerr.write(static_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize());
    errors->Release();
  }
  if (FAILED(result)) {
    if (code != nullptr) {
      code->Release();
    }
    FreeLibrary(compiler);
    return 1;
  }
  std::ofstream output(argv[3], std::ios::binary);
  output << "#pragma once\n#include <cstdint>\n#include <span>\ninline constexpr std::uint8_t " << argv[4] << "_data[] = {";
  const auto* bytes = static_cast<const unsigned char*>(code->GetBufferPointer());
  for (size_t i = 0; i < code->GetBufferSize(); ++i) {
    output << static_cast<unsigned>(bytes[i]) << ',';
  }
  output << "};\ninline constexpr std::span<const std::uint8_t> " << argv[4] << "{" << argv[4] << "_data};\n";
  output.close();
  code->Release();
  FreeLibrary(compiler);
  return output ? 0 : 1;
}

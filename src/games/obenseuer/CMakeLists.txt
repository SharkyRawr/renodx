cmake_minimum_required(VERSION 3.27)

# ReShade's C++ API requires the Microsoft ABI, including member-function struct returns.
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)
set(CMAKE_LINKER lld-link)
set(CMAKE_RC_COMPILER llvm-rc)
set(CMAKE_MT llvm-mt)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
set(XWIN_ROOT "$ENV{HOME}/.cache/renodx-toolchain/msvc" CACHE PATH "xwin CRT and Windows SDK root")
if(NOT EXISTS "${XWIN_ROOT}/crt/include/vcruntime.h")
  message(FATAL_ERROR "Set XWIN_ROOT to an xwin splat directory containing crt/ and sdk/")
endif()
set(CMAKE_CXX_FLAGS_INIT "/imsvc${XWIN_ROOT}/crt/include /imsvc${XWIN_ROOT}/sdk/include/ucrt /imsvc${XWIN_ROOT}/sdk/include/um /imsvc${XWIN_ROOT}/sdk/include/shared")
set(CMAKE_EXE_LINKER_FLAGS_INIT "/libpath:${XWIN_ROOT}/crt/lib/x86_64 /libpath:${XWIN_ROOT}/sdk/lib/ucrt/x86_64 /libpath:${XWIN_ROOT}/sdk/lib/um/x86_64 /manifest:no")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(renodx_obenseuer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
get_filename_component(REPO_ROOT ../../.. ABSOLUTE)
find_program(WINE_EXECUTABLE wine REQUIRED)
set(WINE_PREFIX "$ENV{WINEPREFIX}" CACHE PATH "Wine prefix for the build-only shader compiler")
if(NOT WINE_PREFIX)
  message(FATAL_ERROR "Set WINE_PREFIX to an existing Wine/Proton prefix")
endif()
if(NOT EXISTS "${REPO_ROOT}/bin/D3DCOMPILER_46_x64.dll")
  message(FATAL_ERROR "Place 3Dmigoto's d3dcompiler_46.dll in repo bin/D3DCOMPILER_46_x64.dll")
endif()

add_executable(obenseuer_compile_shader compile_shader.cpp)
configure_file("${REPO_ROOT}/bin/D3DCOMPILER_46_x64.dll" "${CMAKE_CURRENT_BINARY_DIR}/D3DCompiler_46.dll" COPYONLY)

file(GLOB_RECURSE SHARED_SHADER_HEADERS CONFIGURE_DEPENDS "${REPO_ROOT}/src/shaders/*.hlsl")
set(SHADER_HEADERS)
set(SHADER_INCLUDES "#pragma once\n")
foreach(SHADER IN ITEMS 0x37578CE9.ps_5_0 0xB58368D6.ps_5_0 swap_chain_proxy_pixel_shader.ps_5_0 swap_chain_proxy_vertex_shader.vs_5_0)
  string(REPLACE "." ";" PARTS "${SHADER}")
  list(GET PARTS 0 NAME)
  list(GET PARTS 1 PROFILE)
  set(SYMBOL "${NAME}")
  set(HEADER "${CMAKE_CURRENT_BINARY_DIR}/embed/${NAME}.h")
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/embed")
  add_custom_command(OUTPUT "${HEADER}"
    COMMAND ${CMAKE_COMMAND} -E env "WINEPREFIX=${WINE_PREFIX}" "WINEDEBUG=-all" "WINEDLLOVERRIDES=d3dcompiler_46=n" "${WINE_EXECUTABLE}"
      $<TARGET_FILE:obenseuer_compile_shader> "Z:${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}.hlsl" "${PROFILE}" "Z:${HEADER}" "__${SYMBOL}"
    DEPENDS obenseuer_compile_shader "${SHADER}.hlsl" shared.h ${SHARED_SHADER_HEADERS}
    VERBATIM)
  list(APPEND SHADER_HEADERS "${HEADER}")
  string(APPEND SHADER_INCLUDES "#include \"${NAME}.h\"\n")
endforeach()
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/embed/shaders.h" CONTENT "${SHADER_INCLUDES}")

add_library(obenseuer MODULE addon.cpp ${SHADER_HEADERS})
target_compile_definitions(obenseuer PRIVATE NOMINMAX WIN32=1)
target_include_directories(obenseuer PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(obenseuer SYSTEM PRIVATE
  "${REPO_ROOT}/external/reshade"
  "${REPO_ROOT}/external/json/include" "${REPO_ROOT}/external/gtl/include"
  "${REPO_ROOT}/external/frozen/include")
target_link_libraries(obenseuer PRIVATE dxgi d3d11 d3d12 version ole32 uuid shlwapi)
set_target_properties(obenseuer PROPERTIES PREFIX "renodx-" SUFFIX ".addon64")

# Exercise the compiler failure path; a failed shader must stop addon generation.
enable_testing()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/invalid.hlsl" "this is not HLSL;\n")
add_test(NAME obenseuer_rejects_invalid_shader
  COMMAND ${CMAKE_COMMAND} -E env "WINEPREFIX=${WINE_PREFIX}" "WINEDEBUG=-all" "WINEDLLOVERRIDES=d3dcompiler_46=n"
    "${WINE_EXECUTABLE}" $<TARGET_FILE:obenseuer_compile_shader>
    "Z:${CMAKE_CURRENT_BINARY_DIR}/invalid.hlsl" ps_5_0 "Z:${CMAKE_CURRENT_BINARY_DIR}/invalid.h" invalid)
set_tests_properties(obenseuer_rejects_invalid_shader PROPERTIES WILL_FAIL TRUE)

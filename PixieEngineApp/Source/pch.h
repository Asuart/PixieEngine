#pragma once
#include "../PixieEngineCore/Source/PixieEngineCore.h"

#include <chrono>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#define USE_MSVC_RUNTIME_LIBRARY_DLL
#include <GLFW/glfw3.h>

#ifdef PIXIE_ENGINE_DOUBLE_PRECISION
static constexpr ImGuiDataType_ ImGuiFloat = ImGuiDataType_::ImGuiDataType_Double;
#else
static constexpr ImGuiDataType_ ImGuiFloat = ImGuiDataType_::ImGuiDataType_Float;
#endif;

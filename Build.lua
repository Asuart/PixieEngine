-- premake5.lua
workspace "PixieEngine"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "PixieEngineApp"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "PixieEngineCore/Build-Core.lua"

include "PixieEngineApp/Build-App.lua"

filter {}
   postbuildcommands {
   	"{COPYDIR} %[../Resources] %[../Build/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}/%{prj.name}/Resources]"
   }

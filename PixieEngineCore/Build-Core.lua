project "PixieEngineCore"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "Build/%{cfg.buildcfg}"
   staticruntime "off"

   pchheader "pch.h"
   pchsource "Source/pch.cpp"

   files { 
      "Source/**.h",
      "Source/**.cpp",
      "../Dependencies/glad/src/**.c",
      "../Dependencies/stb/**.h",
    }

   includedirs
   {
      "Source",
      "Source/ShaderGraph",
      "Source/ShaderGraph/Nodes",
      "Source/Scene",
      "Source/Scene/Components",
      "Source/Rendering",
      "Source/RayTracing",
      "Source/Math",
      "Source/Resources",
      "Source/Animation",
      "../Dependencies/glm",
      "../Dependencies/glad/include",
      "../Dependencies/stb",
      "../Dependencies/freetype/include",
   }

   links
   {
	  "assimp-vc143-mt.lib",
      "freetype.lib"
   }

   libdirs { "../Dependencies/freetype/lib" }

   targetdir ("../Build/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Build/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter { 'files:../Dependencies/glad/src/**.c' }
       flags { 'NoPCH' }

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"
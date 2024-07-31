project "PixieEngineApp"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Build/%{cfg.buildcfg}"
   staticruntime "off"

   files 
   { 
      "Source/**.h", 
      "Source/**.cpp", 
      "../Dependencies/imgui/**.h", 
      "../Dependencies/imgui/**.cpp", 
      "../Dependencies/imgui/**.cpp", 
      "../Dependencies/imgui/backends/imgui_impl_opengl3_loader.h", 
      "../Dependencies/imgui/backends/imgui_impl_opengl3.h", 
      "../Dependencies/imgui/backends/imgui_impl_opengl3.cpp", 
      "../Dependencies/imgui/backends/imgui_impl_glfw.h", 
      "../Dependencies/imgui/backends/imgui_impl_glfw.cpp"
   }

   includedirs
   {
      "Source",
      "../PixieEngineCore/Source",
      "../Dependencies/glad/include",
      "../Dependencies",
      "../Dependencies/imgui/backends",
      "../Dependencies/imgui/examples/libs/glfw/include",
   }

   links
   {
      "PixieEngineCore",
	  "glfw3.lib"
   }

   libdirs { "../Dependencies/imgui/examples/libs/glfw/lib-vc2010-64" }

   targetdir ("../Build/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Build/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

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
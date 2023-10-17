-- GLES programs

-- reset to workspace to change global settings for GLES programs
workspace "gles"
	configurations { "Debug", "Release" }
	platforms { "linux", "gmake", "windows" }

	--configurations { "Debug", "Release" }
	location "../../build/gles"
	targetdir "../../build/gles"
	includedirs { "../../inc", "../glcommon" }

	filter "platforms:linux"
		links { "SDL2", "GLESv2", "m" }

	
	  -- no idea
	--configuration "windows"
		--links { "mingw32", "SDL2main", "SDL2" }

	filter "configurations:Debug"
		defines { "DEBUG", "USING_GLES2" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG", "USING_GLES2" }
		optimize "On"
	
	project "gles_triangle"
		kind "ConsoleApp"
		language "C"
		files {
			"gles_triangle.c",
			"../glcommon/gltools.c",
			"glad_gles2.c"
		}
		buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type", "-Wimplicit", "`pkg-config --cflags sdl2`" }
		


-- A solution contains projects, and defines the available configurations
solution "opengl_reference"
	configurations { "Debug", "Release" }
	--location "build"
	includedirs { "inc", "src/glcommon" }

	configuration "linux"
		links { "SDL2", "GLEW", "GL", "m" }
	
	configuration "windows"
		links { "mingw32", "SDL2main", "SDL2", "glew32", "opengl32" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		defines { "Optimize" }
	

	project "ex1"
		location "build"
		kind "ConsoleApp"
		language "C"
		files {
			"src/ex1.c"
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "ex2"
		location "build"
		kind "ConsoleApp"
		language "C"
		files {
			"src/ex2.c"
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "texturing"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/texturing.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/gltools.cpp"
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

	project "modelviewer"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/modelviewer.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/rsw_primitives.cpp",
			"src/glcommon/rsw_halfedge.cpp"
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "grass"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/grass.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/rsw_glframe.cpp",
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "sphereworld_color"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/sphereworld_color.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/rsw_glframe.cpp",
			"src/glcommon/rsw_primitives.cpp",
			"src/glcommon/rsw_halfedge.cpp",
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

	project "flying"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/flying.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/rsw_glframe.cpp",
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

	
	project "glm_modelviewer"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/glm_modelviewer.cpp",
			"src/glcommon/c_utils.cpp",
			"src/glcommon/glm_primitives.cpp",
			"src/glcommon/glm_halfedge.cpp"
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "glm_texturing"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/glm_texturing.cpp",
			"src/glcommon/c_utils.cpp",
			"src/glcommon/gltools.cpp"
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

	project "point_sprites"
		location "build"
		kind "ConsoleApp"
		language "C"
		files {
			"src/point_sprites.c",
			"src/glcommon/gltools.c"
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

	project "glm_sphereworld_color"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/glm_sphereworld_color.cpp",
			"src/glcommon/glm_glframe.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/glm_primitives.cpp",
			"src/glcommon/glm_halfedge.cpp",
			"src/glcommon/c_utils.cpp",
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

	project "glm_grass"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/glm_grass.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/glm_glframe.cpp",
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "glm_flying"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/glm_flying.cpp",
			"src/glcommon/glm_glframe.cpp",
			"src/glcommon/gltools.cpp",
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "left_handed"
		location "build"
		kind "ConsoleApp"
		language "C"
		files {
			"src/left_handed.c"
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "gears"
		location "build"
		kind "ConsoleApp"
		language "C"
		files {
			"src/gears.c"
		}
		targetdir "build"
		
		configuration { "gmake" }
			buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

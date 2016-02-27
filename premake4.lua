-- A solution contains projects, and defines the available configurations
solution "opengl_reference"
	configurations { "Debug", "Release" }
	--location "build"
	includedirs { "inc", "inc/glcommon" }
	links { "SDL2", "GLEW", "GL" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		defines { "Optimize" }
	

	project "c_ex1"
		location "build"
		kind "ConsoleApp"
		language "C"
		files {
			"src/ex1.c"
		}
		targetdir "build"
		
		configuration { "linux", "gmake" }
			buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "c_ex2"
		location "build"
		kind "ConsoleApp"
		language "C"
		files {
			"src/ex2.c"
		}
		targetdir "build"
		
		configuration { "linux", "gmake" }
			buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }



	project "ex1"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/ex1.cpp"
		}
		targetdir "build"
		
		configuration { "linux", "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "ex2"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/ex2.cpp"
		}
		targetdir "build"

		configuration { "linux", "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

	
	project "ex3"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/ex3.cpp",
			"src/glcommon/c_utils.cpp",
			"src/glcommon/primitives.cpp",
			"src/glcommon/glm_halfedge.cpp"
		}
		targetdir "build"
		
		configuration { "linux", "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "ex4"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/ex4.cpp",
			"src/glcommon/gltools.cpp"
		}
		targetdir "build"
		
		configuration { "linux", "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "flying"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/flying.cpp",
			"src/glcommon/glm_glframe.cpp",
			"src/glcommon/c_utils.cpp",
			"src/glcommon/primitives.cpp"
		}
		targetdir "build"
		
		configuration { "linux", "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "left_handed"
		location "build"
		kind "ConsoleApp"
		language "C"
		files {
			"src/left_handed.c"
		}
		targetdir "build"
		
		configuration { "linux", "gmake" }
			buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

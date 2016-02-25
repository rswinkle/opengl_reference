-- A solution contains projects, and defines the available configurations
solution "opengl_reference"
	configurations { "Debug", "Release" }
	--location "build"
	includedirs { "inc" }
	
	project "ex1"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/ex1.cpp"
		}
		--includedirs { }
		links { "SDL2", "GLEW", "GL" }
		targetdir "build"
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			defines { "Optimize" }

		configuration { "linux", "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

	project "ex2"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/ex2.cpp"
		}
		--includedirs { }
		links { "SDL2", "GLEW", "GL" }
		targetdir "build"
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			defines { "Optimize" }

		configuration { "linux", "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

	
	project "ex3"
		location "build"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/ex3.cpp",
			"src/c_utils.cpp",
			"src/primitives.cpp"
		}
		--includedirs { }
		links { "SDL2", "GLEW", "GL" }
		targetdir "build"
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			defines { "Optimize" }

		configuration { "linux", "gmake" }
			buildoptions { "-ansi", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }


	project "left_handed"
		location "build"
		kind "ConsoleApp"
		language "C"
		files {
			"src/left_handed.c"
		}
		--includedirs { }
		links { "SDL2", "GLEW", "GL" }
		targetdir "build"
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			defines { "Optimize" }

		configuration { "linux", "gmake" }
			buildoptions { "-std=c99", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type" }

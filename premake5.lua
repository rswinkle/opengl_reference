-- A solution contains projects, and defines the available configurations
workspace "opengl_reference"
	configurations { "Debug", "Release" }
	location "build"
	targetdir "build"
	includedirs { "inc", "src/glcommon" }

	platforms { "linux", "windows" }

	filter "platforms:linux"
		links { "SDL2", "GLEW", "GL", "m" }
	
	filter "platforms:windows"
		links { "mingw32", "SDL2main", "SDL2", "glew32", "opengl32" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	filter { "action:gmake", "Release" }
		buildoptions { "-O3" }

	filter { "action:gmake", "language:C" }
		buildoptions { "-std=c99", "-pedantic-errors", "-Wall", "-Wextra", "-Wno-sign-compare", "-Wno-unused-parameter" }
	filter { "action:gmake", "language:C++" }
		buildoptions { "-fno-rtti", "-fno-exceptions", "-fno-strict-aliasing", "-Wunused-variable", "-Wreturn-type", "-Wall", "-Wextra", "-Wno-sign-compare", "-Wno-unused-parameter", "-Wno-missing-field-initializers" }
	
	

	project "ex1"
		kind "ConsoleApp"
		language "C"
		files {
			"src/ex1.c"
		}

	project "ex2"
		kind "ConsoleApp"
		language "C"
		files {
			"src/ex2.c"
		}

	project "lesson1"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson1.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "lesson2"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson2.cpp",
			"src/glcommon/gltools.cpp",
		}


	project "lesson3"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson3.cpp",
			"src/glcommon/gltools.cpp",
		}


	project "lesson4"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson4.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "lesson5"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson5.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "lesson6"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson6.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "lesson7"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson7.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "lesson8"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson8.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "lesson9"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson9.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "lesson10"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson10.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/c_utils.cpp"
		}

	project "lesson11"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson11.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "lesson12"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/lesson12.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "multidraw"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/multidraw.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
		}

	project "multidraw2"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/multidraw2.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
		}

	project "texturing"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/texturing.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/gltools.cpp"
		}

	project "modelviewer"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/modelviewer.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/rsw_primitives.cpp",
			"src/glcommon/rsw_halfedge.cpp"
		}

	project "testprimitives"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/testprimitives.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/rsw_primitives.cpp",
			"src/glcommon/rsw_halfedge.cpp"
		}

	project "grass"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/grass.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/rsw_glframe.cpp",
		}


	project "sphereworld_color"
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

	project "flying"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/flying.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp",
			"src/glcommon/rsw_glframe.cpp",
		}

	project "glm_modelviewer"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/glm_modelviewer.cpp",
			"src/glcommon/c_utils.cpp",
			"src/glcommon/glm_primitives.cpp",
			"src/glcommon/glm_halfedge.cpp"
		}


	project "glm_texturing"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/glm_texturing.cpp",
			"src/glcommon/c_utils.cpp",
			"src/glcommon/gltools.cpp"
		}

	project "point_sprites"
		kind "ConsoleApp"
		language "C"
		files {
			"src/point_sprites.c",
			"src/glcommon/gltools.c"
		}


	project "glm_sphereworld_color"
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

	project "glm_grass"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/glm_grass.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/glm_glframe.cpp",
		}

	project "glm_flying"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/glm_flying.cpp",
			"src/glcommon/glm_glframe.cpp",
			"src/glcommon/gltools.cpp",
		}

	project "left_handed"
		kind "ConsoleApp"
		language "C"
		files {
			"src/left_handed.c"
		}

	project "gears"
		kind "ConsoleApp"
		language "C"
		files {
			"src/gears.c"
		}

-- programs just to test things
	project "test_poly_modes"
		kind "ConsoleApp"
		language "C"
		files {
			"src/tests/poly_modes.c"
		}

	project "test_scissoring"
		kind "ConsoleApp"
		language "C"
		files {
			"src/tests/scissoring.c"
		}

	project "test_viewport"
		kind "ConsoleApp"
		language "C"
		files {
			"src/tests/viewport.c"
		}

	project "test_clipping"
		kind "ConsoleApp"
		language "C"
		files {
			"src/tests/clipping.c"
		}

	project "test_depthclamp"
		kind "ConsoleApp"
		language "C"
		files {
			"src/tests/depthclamp.c"
		}

	project "test_line_depth"
		kind "ConsoleApp"
		language "C"
		files {
			"src/tests/thick_line_depth.c"
		}

	project "test_aliasing_lines"
		kind "ConsoleApp"
		language "C"
		files {
			"src/tests/lines.c"
		}

	project "polylines"
		kind "ConsoleApp"
		language "C++"
		files {
			"src/tests/polylines.cpp",
			"src/glcommon/gltools.cpp",
			"src/glcommon/rsw_math.cpp"
		}

	project "client_arrays"
		kind "ConsoleApp"
		language "C"
		files {
			"src/tests/client_arrays.c"
		}

	project "color_masking"
		kind "ConsoleApp"
		language "C"
		files {
			"src/tests/color_masking.c"
		}

-- GLES programs have their own premake5.lua in src/tests/ and a build directory
-- in build/gles


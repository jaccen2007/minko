project "minko-webgl"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs {
		"src",
		"../../framework/src",
		"../../framework/lib/glew/include"
	}
	links { "minko-framework" }

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"

	-- linux
	configuration { "linux" }
		buildoptions "-std=c++0x"

	-- windows
	configuration { "windows", "x32" }

	-- macos
	configuration { "debug", "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }

	-- emscripten
	configuration { "emscripten", "release" }
		buildoptions { "-std=c++11" }

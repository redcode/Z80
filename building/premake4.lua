solution "Z80"
	configurations {
		"release-dynamic", "release-dynamic-module", "release-static", "release-static-module",
		"debug-dynamic", "debug-dynamic-module", "debug-static", "debug-static-module"
	}

	project "Z80"
		language "C"
		flags {"ExtraWarnings"}
		files {"../sources/**.c"}
		includedirs {"../API"}
		--buildoptions {"-std=c89 -pedantic -Wall -Weverything"}

		configuration "release*"
			targetdir "lib/release"

		configuration "debug*"
			flags {"Symbols"}
			targetdir "lib/debug"

		configuration "*dynamic*"
			kind "SharedLib"

		configuration "*dynamic-module"
			defines {"CPU_Z80_BUILD_MODULE_ABI"}

		configuration "*static*"
			kind "StaticLib"
			defines {"CPU_Z80_STATIC"}

		configuration "*static-module"
			defines {"CPU_Z80_BUILD_ABI"}

solution "Z80"
	configurations {
		"Release-Dynamic", "Release-Dynamic-Module", "Release-Static", "Release-Static-Module",
		"Debug-Dynamic", "Debug-Dynamic-Module", "Debug-Static", "Debug-Static-Module"
	}

	project "Z80"
		language "C"
		flags {"ExtraWarnings"}
		files {"../sources/**.c"}
		includedirs {"../API/C"}
		--buildoptions {"-std=c89 -pedantic"}

		configuration "Release*"
			targetdir "lib/release"

		configuration "Debug*"
			flags {"Symbols"}
			targetdir "lib/debug"

		configuration "*Dynamic*"
			kind "SharedLib"

		configuration "*Dynamic-Module"
			defines {"CPU_Z80_BUILD_MODULE_ABI"}
			targetprefix ""
			targetextension ".CPU"

		configuration "*Static*"
			kind "StaticLib"
			defines {"CPU_Z80_STATIC"}

		configuration "*Static-Module"
			defines {"CPU_Z80_BUILD_ABI"}

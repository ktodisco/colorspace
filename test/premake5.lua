project "colorspace-tests"
	language "C"
	cdialect "C17"
	kind "ConsoleApp"
	warnings "Everything"

	flags { "MultiProcessorCompile" }
	
	files {
		"main.c"
	}
	
	includedirs {
		"../include"
	}
	
	links {
		"colorspace"
	}
workspace "colorspace"
	configurations { "Debug", "Release" }
	platforms { "x64" }
	location "build/win64"
	characterset "ASCII"
	
	targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
	
	filter { "configurations:Debug" }
		defines { "_DEBUG" }
		optimize "Off"
		symbols "On"
		
	filter { "configurations:Release" }
		defines { "_RELEASE" }
		optimize "Full"
		symbols "Off"
		
	filter { "platforms:x64" }
		architecture "x86_64"
		
	filter {}
	
project "colorspace"
	language "C"
	cdialect "C17"
	kind "StaticLib"
	warnings "Everything"

	flags { "MultiProcessorCompile" }
	
	files {
		"include/colorspace.h",
		"src/colorspace.c"
	}
	
	includedirs {
		"include"
	}
	
include("test/premake5.lua")
project "protobuf"
    kind "StaticLib"
    language "C++"

    targetdir ""
    objdir "bin-int/%{cfg.buildcfg}"

    -- google being silly
    warnings "off"

    defines {
        -- google being double silly
        "_SCL_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS",
    }

    includedirs {
        "%{wks.location}/%{IncludeDir.google}",
        "%{wks.location}/%{IncludeDir.google}/google/protobuf"
    }

    files {
        "include/google/protobuf/**.cc",
        "include/google/protobuf/**.h",
        "include/google/protobuf/**.proto",
    }

    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"

    filter "configurations:Server"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
        optimize "on"

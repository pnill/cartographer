project "opus"
    kind "StaticLib"
    language "C"

    targetdir ""
    objdir "bin-int/%{cfg.buildcfg}"

    disablewarnings {
        "4244",
    }

    defines {
        "USE_ALLOCA",
        "OPUS_BUILD",
        "_CRT_SECURE_NO_WARNINGS",
    }

    includedirs {
        "%{wks.location}/xlive/3rdparty/opus/celt",
        "%{wks.location}/xlive/3rdparty/opus/include",
        "%{wks.location}/xlive/3rdparty/opus/silk",
    }

    files {
        "include/**.h",

        "src/**.c",
        "src/**.h",
    }

    -- Opus provides some "demo" files that define a "main" function, so we remove them from the build but keep on the file system
    removefiles {
        "src/opus_compare.c",
        "src/opus_demo.c",
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

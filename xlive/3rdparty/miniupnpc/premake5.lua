project "miniupnpc"
    kind "StaticLib"
    language "C"

    targetdir ""
    objdir "bin-int/%{cfg.buildcfg}"

    disablewarnings {
        "4996",
    }

    defines {
        "MINIUPNP_STATICLIB",
        "_CRT_SECURE_NO_WARNINGS",
    }

    includedirs {
        "%{wks.location}/%{IncludeDir.miniupnpc}/miniupnpc",
    }

    files {
        "include/miniupnpc/**.h",
        "src/**.c",
    }

    -- mini includes some extra stuff that does not need to build with xlive
    removefiles {
        "src/listdevices.c",

        "src/minihttptestserver.c",
        "src/miniupnpcmodule.c",
        "src/minixmlvalid.c",

        "src/test**.c",

        "src/upnpc.c",

        "src/wingenminiupnpcstrings.c",
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

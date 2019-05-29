project "discord"
    kind "StaticLib"
    language "C++"

    targetdir ""
    objdir "bin-int/%{cfg.buildcfg}"

    includedirs {
        "%{wks.location}/%{IncludeDir.discord}",
        "%{wks.location}/%{IncludeDir.rapidjson}",
    }

    files {
        "include/discord_register.h",
        "include/discord_rpc.h",

        "src/**.cpp",
        "src/**.h",
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

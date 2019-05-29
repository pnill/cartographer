project "portaudio"
    kind "StaticLib"
    language "C"

    targetdir ""
    objdir "bin-int/%{cfg.buildcfg}"

    defines {
        "_CRT_SECURE_NO_WARNINGS",
    }

    includedirs {
        "%{wks.location}/%{IncludeDir.portaudio}",

        "%{wks.location}/xlive/3rdparty/portaudio/src/common",

        "%{wks.location}/xlive/3rdparty/portaudio/src/hostapi/dsound",
        "%{wks.location}/xlive/3rdparty/portaudio/src/hostapi/wdmks",
        "%{wks.location}/xlive/3rdparty/portaudio/src/hostapi/wmme",
        "%{wks.location}/xlive/3rdparty/portaudio/src/hostapi/wasapi",

        "%{wks.location}/xlive/3rdparty/portaudio/src/os/win",
    }

    files {
        "include/**.h",

        "src/common/**.c",
        "src/common/**.h",

        "src/hostapi/dsound/**.c",
        "src/hostapi/dsound/**.h",

        "src/hostapi/wasapi/pa_win_wasapi.c",
        "src/hostapi/wdmks/pa_win_wdmks.c",
        "src/hostapi/wmme/pa_win_wmme.c",

        "src/os/win/**.c",
        "src/os/win/**.h",
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

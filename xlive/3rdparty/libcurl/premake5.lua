project "libcurl"
    kind "StaticLib"
    language "C"

    targetdir ""
    objdir "bin-int/%{cfg.buildcfg}"
    implibdir ""

    defines {
        "WIN32",
        "_CRT_SECURE_NO_DEPRECATE",
        "BUILDING_LIBCURL",
        "CURL_DISABLE_LDAP",
        "CURL_STATICLIB",
        --"HAVE_CONFIG_H",
    }

    includedirs {
        "%{wks.location}/%{IncludeDir.libcurl}",
        "%{wks.location}/xlive/3rdparty/libcurl/lib",
        "%{wks.location}/xlive/3rdparty/libcurl/src",
    }

    files {
        "include/curl/**.h",

        "lib/libcurl.rc",
        "lib/**.c",
        "lib/**.h",

        "src/**.c",
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

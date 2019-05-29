workspace "Project Cartographer"
	architecture "x86"
	configurations { "Debug", "Server", "Release" }
	startproject "H2MOD"

-- These config suffix only apply to third party libs, H2MOD disables them
configuration "Debug"
	targetsuffix "_debug"

configuration "Server"
	targetsuffix "_server"

configuration "Release"
	targetsuffix "_release"

directxpath = "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)"
tppath = "xlive/3rdparty"

-- Relative to solution file
halo2dir = "Halo2"

-- Should be an absolute path
halo2server = "C:/Program Files (x86)/Microsoft Games/Halo 2 Dedicated Server"

-- Third party includes
IncludeDir = {}
IncludeDir["directx"]   = "%{directxpath}/Include"
IncludeDir["discord"]   = "%{tppath}/discord/include"
IncludeDir["google"]    = "%{tppath}/google/include" -- this contains protobuf only right now
IncludeDir["libcurl"]   = "%{tppath}/libcurl/include"
IncludeDir["miniupnpc"] = "%{tppath}/miniupnpc/include"
IncludeDir["opus"]      = "%{tppath}/opus/include"
IncludeDir["portaudio"] = "%{tppath}/portaudio/include"
IncludeDir["rapidjson"] = "%{tppath}/rapidjson/include"

-- Lib directories
LibDir = {}
LibDir["blam"]      = "xlive/Blam"
LibDir["directx"]   = "%{directxpath}/Lib/x86"
LibDir["discord"]   = "%{tppath}/discord"
LibDir["google"]    = "%{tppath}/google"
LibDir["libcurl"]   = "%{tppath}/libcurl"
LibDir["miniupnpc"] = "%{tppath}/miniupnpc"
LibDir["opus"]      = "%{tppath}/opus"
LibDir["portaudio"] = "%{tppath}/portaudio"

-- Third party debug libs
DLib = {}
DLib["blam"]      = "blam_debug.lib"
DLib["discord"]   = "discord_debug.lib"
DLib["libcurl"]   = "libcurl_debug.lib"
DLib["miniupnpc"] = "miniupnpc_debug.lib"
DLib["opus"]      = "opus_debug.lib"
DLib["protobuf"]  = "protobuf_debug.lib"
DLib["portaudio"] = "portaudio_debug.lib"

-- Dedicated server libs
SLib = {}
SLib["blam"]      = "blam_server.lib"
SLib["discord"]   = "discord_server.lib"
SLib["libcurl"]   = "libcurl_server.lib"
SLib["opus"]      = "opus_server.lib"
SLib["protobuf"]  = "protobuf_server.lib"

-- Third party release libs
RLib = {}
RLib["blam"]      = "blam_release.lib"
RLib["discord"]   = "discord_release.lib"
RLib["libcurl"]   = "libcurl_release.lib"
RLib["miniupnpc"] = "miniupnpc_release.lib"
RLib["opus"]      = "opus_release.lib"
RLib["protobuf"]  = "protobuf_release.lib"
RLib["portaudio"] = "portaudio_release.lib"

-- Windows libs
WLib = {}
WLib["adv"]      = "advapi32.lib"
WLib["comdlg"]   = "comdlg32.lib"
WLib["crypt"]    = "crypt32.lib"
WLib["d3d9"]     = "d3d9.lib"
WLib["d3dx9"]    = "d3dx9.lib"
WLib["dbghelp"]  = "dbghelp.lib"
WLib["gdi"]      = "gdi32.lib"
WLib["iphlp"]    = "iphlpapi.lib"
WLib["mswsock"]  = "mswsock.lib"
WLib["odbc"]     = "odbc32.lib"
WLib["odbccp"]   = "odbccp32.lib"
WLib["ole"]      = "ole32.lib"
WLib["oleaut"]   = "oleaut32.lib"
WLib["shell"]    = "shell32.lib"
WLib["shlw"]     = "shlwapi.lib"
WLib["user"]     = "user32.lib"
WLib["uuid"]     = "uuid.lib"
WLib["version"]  = "version.lib"
WLib["winmm"]    = "winmm.lib"
WLib["winspool"] = "winspool.lib"
WLib["wintrust"] = "wintrust.lib"
WLib["ws2"]      = "ws2_32.lib"

-- This nicely groups the dependencies away in VS
group "Dependencies"
	include (tppath .. "/discord")
	include (tppath .. "/google")
	include (tppath .. "/libcurl")
	include (tppath .. "/miniupnpc")
	include (tppath .. "/opus")
	include (tppath .. "/portaudio")
	include (tppath .. "/rapidjson")

-- H2MOD and Blam have no group
group ""

project "H2MOD"
	location "xlive"
	kind "SharedLib" -- .dll extension
	language "C++"
	cppdialect "C++17"

	-- xlive.dll goes here, and gets copied to /Halo2 post-build
	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin-int/%{cfg.buildcfg}"

	-- xlive.dll
	targetname "xlive"

	pchheader "stdafx.h"
	pchsource "xlive/stdafx.cpp"

	files {
		"xlive/dllmain.cpp",
		"xlive/Globals.cpp",
		"xlive/Globals.h",
		"xlive/H2MOD.cpp",
		"xlive/H2MOD.h",
		"xlive/Ini.h",
		"xlive/resource.h",
		"xlive/Resource.rc",
		"xlive/stdafx.cpp",
		"xlive/stdafx.h",
		"xlive/xlive.def",
		"xlive/xlivedefs.h",
		"xlive/xliveless.cpp",
		"xlive/xliveless.h",

		"xlive/H2MOD/**.cc",
        "xlive/H2MOD/**.cpp",
		"xlive/H2MOD/**.h",
		"xlive/H2MOD/**.proto",

        "xlive/Util/**.cc",
        "xlive/Util/**.cpp",
        "xlive/Util/**.h",
		"xlive/Util/**.proto",

        "xlive/XLive/**.cc",
        "xlive/XLive/**.cpp",
        "xlive/XLive/**.h",
        "xlive/XLive/**.proto",
	}

	includedirs {
		"xlive",
		"xlive/H2MOD",
		"%{IncludeDir.directx}",
		"%{IncludeDir.discord}",
		"%{IncludeDir.google}",
		"%{IncludeDir.libcurl}",
		"%{IncludeDir.miniupnpc}",
		"%{IncludeDir.opus}",
		"%{IncludeDir.portaudio}",
		"%{IncludeDir.rapidjson}",
	}

	libdirs {
		"%{LibDir.blam}",
		"%{LibDir.directx}",
		"%{LibDir.discord}",
		"%{LibDir.google}",
		"%{LibDir.libcurl}",
		"%{LibDir.miniupnpc}",
		"%{LibDir.opus}",
		"%{LibDir.portaudio}",
	}

	-- All H2MOD configurations link these
	links {
		"%{WLib.crypt}",
		"%{WLib.d3d9}",
		"%{WLib.d3dx9}",
		"%{WLib.dbghelp}",
		"%{WLib.odbc}",
		"%{WLib.odbccp}",
		"%{WLib.version}",
		"%{WLib.winmm}",
		"%{WLib.ws2}",
	}

	-- Disable PCH use on the protobuf file
	filter "files:xlive/H2MOD/h2protobuf/h2mod.pb.cc"
		flags "NoPCH"

	filter "system:windows"
		systemversion "8.1"

		defines {
			"WIN32",
			"_CRT_SECURE_NO_WARNINGS",
			"_SCL_SECURE_NO_WARNINGS",
			"_WINDOWS",
			"_WINSOCK_DEPRECATED_NO_WARNINGS",
			"CURL_STATICLIB",
			"xliveless_EXPORTS",
		}

	filter "configurations:Debug"
		targetsuffix ""
		defines {
			"_DEBUG",
			"MINIUPNP_STATICLIB",
		}
		postbuildcommands {
			-- copy new xlive.dll, renaming current one to xlive-OLD.dll if it doesn't already exist, only works if output to root of /bin folder
			("IF NOT EXIST \"../" .. halo2dir .. "/xlive-OLD.dll\" {MOVE} \"../" .. halo2dir .. "/xlive.dll\" \"../" .. halo2dir .. "/xlive-OLD.dll\""),
			("{COPY} %{cfg.buildtarget.relpath} \"../" .. halo2dir .. "/\""),
		}
		links {
			"%{DLib.blam}",
			"%{DLib.discord}",
			"%{DLib.libcurl}",
			"%{DLib.miniupnpc}",
			"%{DLib.opus}",
			"%{DLib.portaudio}",
			"%{DLib.protobuf}",

			"%{WLib.adv}",
			"%{WLib.comdlg}",
			"%{WLib.gdi}",
			"%{WLib.iphlp}",
			"%{WLib.mswsock}",
			"%{WLib.ole}",
			"%{WLib.oleaut}",
			"%{WLib.shell}",
			"%{WLib.shlw}",
			"%{WLib.user}",
			"%{WLib.uuid}",
			"%{WLib.winspool}",
			"%{WLib.wintrust}",
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Server"
		targetsuffix ""
		defines {
			"_DEBUG",
		}
		postbuildcommands {
			-- copy new xlive.dll, renaming current one to xlive-OLD.dll if it doesn't already exist, only works if output to root of /bin folder
			("IF NOT EXIST \"" .. halo2server .. "/xlive-OLD.dll\" {MOVE} \"" .. halo2server .. "/xlive.dll\" \"" .. halo2server .. "/xlive-OLD.dll\""),
			("{COPY} %{cfg.buildtarget.relpath} \"" .. halo2server .. "/\""),
		}
		links {
			"%{SLib.blam}",
			"%{SLib.discord}",
			"%{SLib.libcurl}",
			"%{SLib.opus}",
			"%{SLib.protobuf}",

			"%{WLib.adv}",
			"%{WLib.comdlg}",
			"%{WLib.gdi}",
			"%{WLib.ole}",
			"%{WLib.oleaut}",
			"%{WLib.shell}",
			"%{WLib.shlw}",
			"%{WLib.user}",
			"%{WLib.uuid}",
			"%{WLib.winspool}",
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		targetsuffix ""
		defines {
			"NDEBUG",
			"MINIUPNP_STATICLIB",
		}
		postbuildcommands {
			-- copy new xlive.dll, renaming current one to xlive-OLD.dll if it doesn't already exist, only works if output to root of /bin folder
			("IF NOT EXIST \"../" .. halo2dir .. "/xlive-OLD.dll\" {MOVE} \"../" .. halo2dir .. "/xlive.dll\" \"../" .. halo2dir .. "/xlive-OLD.dll\""),
			("{COPY} %{cfg.buildtarget.relpath} \"../" .. halo2dir .. "/\""),
		}
		links {
			"%{RLib.blam}",
			"%{RLib.discord}",
			"%{RLib.libcurl}",
			"%{RLib.miniupnpc}",
			"%{RLib.opus}",
			"%{RLib.protobuf}",
			"%{RLib.portaudio}",

			"%{WLib.iphlp}",
			"%{WLib.shlw}",
		}
		runtime "Release"
		optimize "on"

-- Just to help organize the sidebar in VS
project "Blam"
	location "xlive/Blam"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	-- kablamo
	targetdir "xlive/Blam"
	objdir "xlive/Blam/bin-int/%{cfg.buildcfg}"

	includedirs {
		"xlive"
	}

	files {
		"xlive/Blam/**.cpp",
		"xlive/Blam/**.h",
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

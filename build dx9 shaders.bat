@echo off
GOTO:MAIN

:compile_shader
	SETLOCAL ENABLEDELAYEDEXPANSION
		:: The part where we compile the shader bytecode
		.\bin\fxc.exe /O3 /T %~1 %~3 /Fo ./xlive/Blam/Engine/rasterizer/dx9/shaders/compiled/%~2_%~1.pso ./xlive/Blam/Engine/rasterizer/dx9/shaders/%~2.fx
    ENDLOCAL
EXIT /B 0

:replace_content
	SETLOCAL ENABLEDELAYEDEXPANSION
		:: Instead of "unsigned char" we want "static const unsigned char"
		:: Also, remove __compiled prefix
		.\bin\w64devkit\bin\sed.exe -i "s/unsigned char __xlive_Blam_Engine_rasterizer_dx9_shaders_compiled_/static const unsigned char k_/g" %~2
		
		:: Remove _pso suffix
		.\bin\w64devkit\bin\sed.exe -i "s/_pso\[/\[/g" %~2
	ENDLOCAL
EXIT /B 0

:add_shader_bytecode_to_file
	SETLOCAL ENABLEDELAYEDEXPANSION
		:: Get a C array of file data for our compiled shader
		.\bin\w64devkit\bin\xxd.exe -i ./xlive/Blam/Engine/rasterizer/dx9/shaders/compiled/%~2_%~1.pso >> %~3 
		:: Delete the last line in the file
		.\bin\w64devkit\bin\sed.exe -i "$d" %~3						
		call::replace_content %~1 %~3
	ENDLOCAL
EXIT /B 0

:add_shader
	SETLOCAL ENABLEDELAYEDEXPANSION
		call:compile_shader "ps_2_0" %~1 %~2
		call:compile_shader "ps_3_0" %~1 %~2
		
		set file="./xlive/Blam/Engine/rasterizer/dx9/shaders/compiled/%~1.h"
		echo #pragma once > %file%
		echo. >> %file%
		
		call:add_shader_bytecode_to_file "ps_2_0" %~1 %file%
		echo. >> %file%
		call:add_shader_bytecode_to_file "ps_3_0" %~1 %file%
		
		set "file="
    ENDLOCAL
EXIT /B 0

:cleanup
	SETLOCAL ENABLEDELAYEDEXPANSION
		del xlive\Blam\Engine\rasterizer\dx9\shaders\compiled\*.pso
    ENDLOCAL
EXIT /B 0

:MAIN
cd 

call:add_shader "fog_atmospheric_apply" "/Gpp"
call:add_shader "motion_sensor_sweep"
call:add_shader "sun_glow_convolve"
call:add_shader "weather_particle_generic"
call:add_shader "weather_particle_rain"
call:add_shader "weather_plate"
call:add_shader "bloom_simple"
call:add_shader "zprime"

call::cleanup
pause

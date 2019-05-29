@ECHO off
ECHO.
ECHO This script only adds protoc temporarily, this will not change your system PATH!
ECHO Therefore, you must run this with a command terminal to actually be able to use it.

SET PATH=%PATH%;%~dp0bin

ECHO You can now use the command "protoc" in this window only
ECHO.
PAUSE
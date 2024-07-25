@ECHO OFF
REM the last parameter is the directory of the games install.

:start
SET /P path=Please enter your Halo 2 Sercer " Install path: 
IF "%path%"=="" GOTO Error
mklink /J Halo2Server "%path%"
GOTO End

:Error
ECHO "You did't enter a path"
GOTO start

:End
pause 

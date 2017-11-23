@ECHO OFF
REM the last parameter is the directory of the games install.

:start
SET /P path=Please enter your Halo " Install path: 
IF "%path%"=="" GOTO Error
mklink /J Halo2 "%path%"
GOTO End

:Error
ECHO "You did't enter a path"
GOTO start

:End
pause 

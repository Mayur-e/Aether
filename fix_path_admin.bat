@echo off
echo Removing C:\MinGW\bin from System PATH...
for /F "skip=2 tokens=2*" %%A in ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path') do set "OLDPATH=%%B"
set "NEWPATH=%OLDPATH:;C:\MinGW\bin=%" 
set "NEWPATH=%NEWPATH:C:\MinGW\bin;=%" 
reg add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%NEWPATH%" /f
echo Done. Restart your IDE for changes to take effect.
pause
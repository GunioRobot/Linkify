@echo off
set DRIVER_DIR=%CD%
set INSTALL_DIR=%TEMP%\Driver

if "%1" == "/autorun" (
    set DRIVER_DIR=%DRIVER_DIR%Driver
)

if NOT "%DRIVER_DIR%" == "%INSTALL_DIR%" (
    xcopy /E /I /Y "%DRIVER_DIR%" "%INSTALL_DIR%"
    %HOMEDRIVE%
    cd "%INSTALL_DIR%"
    echo.
    start "/D%INSTALL_DIR%" /B "%INSTALL_DIR%\install.bat"
    exit
)

devcon disable "@USBSTOR\DISK&VEN_CORSAIR&PROD_VOYAGER_MINI&REV_0.00\*"
if %ERRORLEVEL% NEQ 0 goto Failure
echo.

DPInst.exe /c /sw
if %ERRORLEVEL% NEQ 1 if %ERRORLEVEL% NEQ 2 goto Failure
echo.

devcon enable "@USBSTOR\DISK&VEN_CORSAIR&PROD_VOYAGER_MINI&REV_0.00\*"
if %ERRORLEVEL% NEQ 0 goto Failure
echo.

goto Success

:Failure
echo.
pause

:Success
exit

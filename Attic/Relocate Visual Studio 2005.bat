@echo off

set location=%APPDATA%\Visual Studio 2005
set branch=HKCU\Software\Microsoft

reg ADD "%branch%\MSDN\8.0" /v VisualStudioLocation /d "%location%" /f
reg ADD "%branch%\VisualStudio\8.0" /v VisualStudioLocation /d "%location%" /f
reg ADD "%branch%\VisualStudio\8.0" /v VisualStudioProjectsLocation /d "%location%\Projects" /f
reg ADD "%branch%\VisualStudio\8.0" /v UserItemTemplatesLocation /d "%location%\Templates" /f
reg ADD "%branch%\VisualStudio\8.0" /v UserProjectTemplatesLocation /d "%location%\Templates" /f

echo.
pause

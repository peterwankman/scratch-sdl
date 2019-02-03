@ECHO OFF
SET SDL_DIR=SDL2
SET IMG_DIR=SDL2_image

CALL :cpdlls bin\win32\release x86
CALL :cpdlls bin\win32\debug x86
CALL :cpdlls bin\x64\release x64
CALL :cpdlls bin\x64\debug x64
EXIT /B 0

:cpdlls
SETLOCAL
echo Copying %2 dependencies to %1...
PUSHD ..
CALL :cpfile deps\%SDL_DIR%\lib\%2\SDL2.dll %1
CALL :cpfile deps\%IMG_DIR%\lib\%2\libjpeg-9.dll %1
CALL :cpfile deps\%IMG_DIR%\lib\%2\libpng16-16.dll %1
CALL :cpfile deps\%IMG_DIR%\lib\%2\libtiff-5.dll %1
CALL :cpfile deps\%IMG_DIR%\lib\%2\libwebp-7.dll %1
CALL :cpfile deps\%IMG_DIR%\lib\%2\zlib1.dll %1
CALL :cpfile deps\%IMG_DIR%\lib\%2\SDL2_image.dll %1

POPD
ENDLOCAL
EXIT /B

:cpfile
SETLOCAL
copy %1 %2 >NUL
if %errorlevel%. equ 1. echo Failed to copy %1
ENDLOCAL
EXIT /B

@echo off
call "C:/Program files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat" x64
cl -FC -Zi ../src/win32layer.cpp ../src/mandelbrot.cpp user32.lib gdi32.lib Xinput.lib
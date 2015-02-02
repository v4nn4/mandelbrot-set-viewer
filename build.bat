@echo off
call "C:/Program files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat" x64
cl -FC -Zi win32layer.cpp mandelbrot.cpp user32.lib gdi32.lib Xinput.lib
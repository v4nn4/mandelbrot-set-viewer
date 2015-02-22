#include <windows.h>
#include <stdint.h>
#include <xinput.h>
#include "mandelbrot.hpp"

#define internal static
#define local_persist static
#define global_variable static

struct win32_offscreen_buffer
{
	BITMAPINFO Info;
	void *Memory;
	int Width;
	int Height;
	int BytesPerPixel;
	int Pitch;
};

global_variable bool Running;
global_variable win32_offscreen_buffer GlobalBackBuffer;
global_variable CoordinateSystem globalCoordinateSystem = {-0.5,0,3,2};

struct win32_window_dimension
{
	int Width;
	int Height;
};

win32_window_dimension
GetWindowDimension(HWND Window)
{
	win32_window_dimension Result;
	RECT ClientRect;
	GetClientRect(Window,&ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;
	return Result;
}

internal void
DrawMandelbrot(win32_offscreen_buffer* Buffer, int BlueOffset, int GreenOffset)
{
	auto Row = static_cast<uint8_t *>(Buffer->Memory);  
	double x,y,m;
	uint8_t Blue;
	uint8_t Green;
	for(auto Y=0;Y<Buffer->Height;++Y)
	{
		auto Pixel = reinterpret_cast<uint32_t*>(Row);
		for(auto X=0;X<Buffer->Width;++X)
		{
			x = static_cast<double>(X)/Buffer->Width;
			y = static_cast<double>(Y)/Buffer->Height;
			TransformInplace(globalCoordinateSystem,x,y);
			m = ComputeMandelbrotRadius(x,y);
			if(m<=4)
			{
				Blue = static_cast<uint8_t>(100)*m;
				Green = static_cast<uint8_t>(200)*m+30;
				*Pixel++ = (Green << 8)|Blue;
			}
			else
			{
				*Pixel++ = 0x00000000;
			}
		}
		Row += Buffer->Pitch;
	}
}

internal void
TranslateBackBuffer(win32_offscreen_buffer* Buffer, int XOffset, int YOffset)
{
	auto Row = static_cast<uint8_t*>(Buffer->Memory);  
	uint32_t tempPixel;
	for(auto Y=0;Y<Buffer->Height;++Y)
	{
		auto Pixel = reinterpret_cast<uint32_t*>(Row);
		for(auto X=0;X<Buffer->Width-1;++X)
		{
			tempPixel = *Pixel;
			*(++Pixel) = tempPixel;
		}
		Row += Buffer->Pitch;
	}	
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
	if(Buffer->Memory)
	{
		VirtualFree(Buffer->Memory,0,MEM_RELEASE);
	}

	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->BytesPerPixel = 4;

	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	Buffer->BytesPerPixel = 4;
	int BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;
	Buffer->Memory = VirtualAlloc(nullptr,BitmapMemorySize,MEM_COMMIT,PAGE_READWRITE);
	Buffer->Pitch = Buffer->Width*Buffer->BytesPerPixel;
	DrawMandelbrot(Buffer,0,0);
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, int Width, int Height, win32_offscreen_buffer Buffer)
{
	StretchDIBits(DeviceContext,
				  0, 0, Width, Height,
				  0, 0, Buffer.Width, Buffer.Height,
		          Buffer.Memory,
		          &Buffer.Info,
		          DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                   UINT Message,
                   WPARAM WParam,
                   LPARAM LParam)
{
	LRESULT Result = 0;
	switch(Message)
	{
		case WM_SIZE:
		{
		} break;
		case WM_DESTROY:
		{
			OutputDebugStringA("WM_DESTROY\n");
		} break;
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			OutputDebugStringA("WM_CLOSE\n");
		} break;
		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			auto VKCode = WParam;
			auto WasDown = ((LParam & (1 << 30))!=0);
			auto IsDown = ((LParam & (1 << 32))==0);
			switch(VKCode)
			{
				case VK_UP:
				{
					if(IsDown && !WasDown)
					{
						TranslateCoordinateSystem(globalCoordinateSystem,0,0.01);
  						TranslateBackBuffer(&GlobalBackBuffer,0,1);
					}
				} break;
				case VK_DOWN:
				{
					if(IsDown && !WasDown)
					{
						TranslateCoordinateSystem(globalCoordinateSystem,0,-0.01);
  						TranslateBackBuffer(&GlobalBackBuffer,0,-1);
					}
					//
				} break;
				case VK_LEFT:
				{
					if(IsDown && !WasDown)
					{
						TranslateCoordinateSystem(globalCoordinateSystem,0.01,0);
						TranslateBackBuffer(&GlobalBackBuffer,1,0);
					}
					//
				} break;
				case VK_RIGHT:
				{
					if(IsDown && !WasDown)
					{
						TranslateCoordinateSystem(globalCoordinateSystem,-0.01,0);
  						TranslateBackBuffer(&GlobalBackBuffer,-1,0);
					}
					//
				} break;
				case VK_SPACE:
				{
					if(IsDown && !WasDown)
					{
						ZoomCoordinateSystem(globalCoordinateSystem,1.5);
					}
				} break;
			}
		} break;
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			auto DeviceContext = BeginPaint(Window,&Paint);
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			auto Dimension = GetWindowDimension(Window);
			Win32DisplayBufferInWindow(DeviceContext,Dimension.Width,Dimension.Height,GlobalBackBuffer);
			EndPaint(Window,&Paint);
		}
		default:
		{
			Result = DefWindowProc(Window,Message,WParam,LParam);
		} break;
	}
	return Result;
}

int CALLBACK
WinMain(HINSTANCE hInstance,
HINSTANCE hPrevInstance,
LPSTR lpCmdLine,
int nCmdShow)
{
	WNDCLASS WindowClass = {};

	Win32ResizeDIBSection(&GlobalBackBuffer, 1288, 720);

	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = hInstance;
	WindowClass.lpszClassName = "MandelbrotViewerWindowClass";

	if (RegisterClass(&WindowClass))
	{
		auto Window =
			CreateWindowEx(
			0,
			WindowClass.lpszClassName,
			"MandelbrotViewer",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			hInstance,
			nullptr);
		if (Window)
		{
			auto XOffset = 0;
			auto YOffset = 0;
			Running = true;
			while (Running)
			{
				MSG Message;
				while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}

				for (DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex)
				{
					XINPUT_STATE ControllerState;
					if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
					{
						switch (ControllerState.Gamepad.wButtons)
						{
						case XINPUT_GAMEPAD_DPAD_UP:
						{
							// Translate by 1 px
							TranslateCoordinateSystem(globalCoordinateSystem, 0, 0.01);
							TranslateBackBuffer(&GlobalBackBuffer, 0, 1);
						} break;
						case XINPUT_GAMEPAD_DPAD_DOWN:
						{
							TranslateCoordinateSystem(globalCoordinateSystem, 0, -0.01);
							TranslateBackBuffer(&GlobalBackBuffer, 0, -1);
						} break;
						case XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_LEFT:
						{
							TranslateCoordinateSystem(globalCoordinateSystem, 0.01, 0.01);
							TranslateBackBuffer(&GlobalBackBuffer, 1, 1);
						} break;
						case XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_RIGHT:
						{
							TranslateCoordinateSystem(globalCoordinateSystem, -0.01, 0.01);
							TranslateBackBuffer(&GlobalBackBuffer, -1, 1);
						} break;
						case XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT:
						{
							TranslateCoordinateSystem(globalCoordinateSystem, 0.01, -0.01);
							TranslateBackBuffer(&GlobalBackBuffer, 1, -1);
						} break;
						case XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_RIGHT:
						{
							TranslateCoordinateSystem(globalCoordinateSystem, -0.01, -0.01);
							TranslateBackBuffer(&GlobalBackBuffer, -1, -1);
						} break;
						case XINPUT_GAMEPAD_DPAD_LEFT:
						{
							TranslateCoordinateSystem(globalCoordinateSystem, 0.01, 0);
							TranslateBackBuffer(&GlobalBackBuffer, 1, 0);
						} break;
						case XINPUT_GAMEPAD_DPAD_RIGHT:
						{
							TranslateCoordinateSystem(globalCoordinateSystem, -0.01, 0);
							TranslateBackBuffer(&GlobalBackBuffer, -1, 0);
						} break;
						case XINPUT_GAMEPAD_A:
						{
							ZoomCoordinateSystem(globalCoordinateSystem, 1.5);
							DrawMandelbrot(&GlobalBackBuffer, XOffset, YOffset);
						} break;
						case XINPUT_GAMEPAD_B:
						{
							ZoomCoordinateSystem(globalCoordinateSystem, 0.5);
							DrawMandelbrot(&GlobalBackBuffer, XOffset, YOffset);
						} break;
						}
					}
					else
					{
						// Controller not available
					}
				}
				auto DeviceContext = GetDC(Window);
				auto Dimension = GetWindowDimension(Window);
				Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackBuffer);
				ReleaseDC(Window, DeviceContext);

				++XOffset;
				YOffset += 2;
			}
		}
		else
		{
			// TODO : Logging
		}
	}
	else
	{
		// TODO : Logging
	}

	return 0;
}
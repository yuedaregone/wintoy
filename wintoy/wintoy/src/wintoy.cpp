#include "wintoy.h"
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include "GL\glew.h"
#include "GL\wglew.h"
#include "console.h"
#include "framework.h"
#include "ftime.h"

#define WINDOW_NAME "WINTOY"
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 300

#define GET_X(Param) ((int)(Param & 0xffff))
#define GET_Y(Param) ((int)((Param >> 16) & 0xffff))

struct SBmpInfo
{
	BITMAPINFO	m_BitmapInfo;
	RGBQUAD		m_bmiColors[2];	// 为BITMAPINFO的m_bmiColors补充两个元素空间
};

bool capture = false;

LRESULT CALLBACK WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_RBUTTONDOWN:
		capture = TouchBegin(GET_X(lParam), GET_Y(lParam));
		if (capture)
		{
			SetCapture(hWnd);
		}
		break;
	case WM_RBUTTONUP:
		if (capture)
		{
			TouchEnd(GET_X(lParam), GET_Y(lParam));
			ReleaseCapture();
		}
		capture = false;
		break;
	case WM_LBUTTONDOWN:						
		::SendMessageA(hWnd, WM_SYSCOMMAND, 0xF012, 0);		
		break;
	case WM_MOUSEMOVE:
		if (capture)
		{
			TouchMove(GET_X(lParam), GET_Y(lParam));
		}
		break;
	case WM_LBUTTONUP:		
		break;
	case WM_SYSCHAR:
	case WM_CHAR:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		break;	
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam == VK_UP || wParam == VK_LEFT)
		{
			TouchBack();
		}
		else if (wParam == VK_DOWN || wParam == VK_RIGHT)
		{
			TouchNext();
		}
		break;
	case WM_PAINT:
		break;
	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);		
	}
	return 1;	
}

#define CONSOLE_DEBUG
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

#ifdef CONSOLE_DEBUG
	start_console();
#endif	

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProcHandler;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, NULL);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = WINDOW_NAME;
	wcex.hIconSm = LoadIcon(hInstance, NULL);

	if (!RegisterClassEx(&wcex))
	{
		printf("RegisterClassEx %s Error: %d\n", WINDOW_NAME, GetLastError());
		return -1;
	}

	UINT width = WINDOW_WIDTH;
	UINT height = WINDOW_HEIGHT;

	DWORD style = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
	HWND hwnd = CreateWindowEx(style, WINDOW_NAME, WINDOW_NAME, WS_VISIBLE,
		0, 0, width, height, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
		return -1;

	UINT* bgBuff = (UINT*)malloc(width * height * sizeof(UINT));
	memset(bgBuff, 0, width * height * sizeof(UINT));

	HDC dc = ::GetDC(hwnd);
	HDC layerDc = ::CreateCompatibleDC(dc);
	HBITMAP bitmap = ::CreateCompatibleBitmap(dc, width, height);

	SBmpInfo* sbmpInfo = (SBmpInfo*)malloc(sizeof(SBmpInfo));
	BITMAPINFO& bmpInfo = sbmpInfo->m_BitmapInfo;
	bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_BITFIELDS;

	*(UINT*)(bmpInfo.bmiColors + 0) = 0xFF0000;
	*(UINT*)(bmpInfo.bmiColors + 1) = 0x00FF00;
	*(UINT*)(bmpInfo.bmiColors + 2) = 0x0000FF;

	::SelectObject(layerDc, bitmap);

	DWORD dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;// | PFD_DRAW_TO_BITMAP ;
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = dwFlags;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int pixelFormat = ChoosePixelFormat(dc, &pfd);
	if (pixelFormat == 0) {
		return -1;
	}
	BOOL bResult = SetPixelFormat(dc, pixelFormat, &pfd);
	if (bResult == FALSE) {
		return -1;
	}
	HGLRC glrc = wglCreateContext(dc);
	if (!glrc) {
		return -1;
	}
	if (FALSE == wglMakeCurrent(dc, glrc))
		return -1;

	//glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { //init glew
		fprintf(stderr, "Failed to init glew!");
		return -1;
	}
	int attribList[] =
	{
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // allow rendering to the pbuffer
		WGL_SUPPORT_OPENGL_ARB,  TRUE,      // associate with OpenGL
		WGL_DOUBLE_BUFFER_ARB,   TRUE,     // single buffered
		WGL_SAMPLE_BUFFERS_ARB, TRUE,
		WGL_RED_BITS_ARB,   8,              // minimum 8-bits for red channel
		WGL_GREEN_BITS_ARB, 8,              // minimum 8-bits for green channel
		WGL_BLUE_BITS_ARB, 8,              // minimum 8-bits for blue channel
		WGL_ALPHA_BITS_ARB, 8,              // minimum 8-bits for alpha channel
		WGL_DEPTH_BITS_ARB, 16,             // minimum 16-bits for depth buffer
		0
	};
	int format = 0;
	UINT matchingFormats = 0;

	if (!wglChoosePixelFormatARB(dc, attribList, 0, 1, &format, &matchingFormats))
	{
		return -1;
	}

	HPBUFFERARB hpBuffer = wglCreatePbufferARB(dc, format, width, height, 0);
	if (!hpBuffer)
	{
		return -1;
	}

	HDC pbufferDC = wglGetPbufferDCARB(hpBuffer);
	if (!pbufferDC)
	{
		return FALSE;
	}

	//HGLRC pbufferGLRC = wglCreateContext(pbufferDC);
	//if (!pbufferGLRC)
	//{
	//	return FALSE;
	//}
	GLint attribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
						WGL_CONTEXT_MINOR_VERSION_ARB, 3,
						WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,//要求返回兼容模式环境,如果不指定或指定为WGL_CONTEXT_CORE_PROFILE_BIT_ARB会返回只包含核心功能的环境 
						0 };
	HGLRC pbufferGLRC = wglCreateContextAttribsARB(pbufferDC, NULL, attribs);
	if (!pbufferGLRC)
	{
		return FALSE;
	}

	wglMakeCurrent(dc, 0);
	wglMakeCurrent(pbufferDC, pbufferGLRC);

	InitFramework((int)width, (int)height, hwnd);

	LARGE_INTEGER freq;
	LARGE_INTEGER now;
	LARGE_INTEGER last;
	LONGLONG interval;
	LONGLONG frameDelta;

	double delta = 1.0 / GetFPS();
	QueryPerformanceFrequency(&freq);
	frameDelta = delta * freq.QuadPart;
	Time::SetFrequency(freq.QuadPart);

	QueryPerformanceCounter(&last);
	
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			QueryPerformanceCounter(&now);
			interval = now.QuadPart - last.QuadPart;
			if (interval < frameDelta)
			{
				Sleep(1);
				continue;
			}
			last.QuadPart = now.QuadPart;
			Time::UpdateTime(interval);

			//TODO: Update And Render			
			UpdateFramework();

			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glReadPixels(0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bgBuff);

			::SetDIBitsToDevice(layerDc,
				0, 0, width, height,
				0, 0, 0, height,
				bgBuff, &sbmpInfo->m_BitmapInfo, DIB_RGB_COLORS);

			BLENDFUNCTION blend;
			blend.BlendOp = AC_SRC_OVER;
			blend.BlendFlags = 0;
			blend.AlphaFormat = 1;
			blend.SourceConstantAlpha = 255;

			RECT rect;
			GetWindowRect(hwnd, &rect);

			POINT p = { rect.left, rect.top };
			SIZE s = { (LONG)width, (LONG)height };

			POINT m_originPos = { 0, 0 };
			::UpdateLayeredWindow(hwnd, dc, &p, &s, layerDc, &m_originPos, 0, &blend, ULW_ALPHA);
		}
	}
	EndFramework();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(pbufferGLRC);
	wglDeleteContext(glrc);

	ReleaseDC(hwnd, dc);
	DeleteObject(bitmap);
	ReleaseDC(hwnd, layerDc);

	free(bgBuff);
	free(sbmpInfo);

#ifdef CONSOLE_DEBUG
	stop_console();
#endif	
	return 0;
}

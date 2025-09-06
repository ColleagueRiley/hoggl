#include <windows.h>

#define HOGGL_IMPLEMENTATION
#include "hoggl.h"

#include <GL/gl.h>

BOOL shouldClose = FALSE;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
			shouldClose = TRUE;
            PostQuitMessage(0);
            return 0;
	}
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    const char CLASS_NAME[] = "hoggl example";

	HDC dc = GetDC(NULL);
	hoggl_display hogDpy;
	hoggl_display_init(dc, &hogDpy);

	hoggl_format format;
	hoggl_format_hints formatHints;
	hoggl_format_hints_copy_default(&formatHints);

	hoggl_format_init(&hogDpy, &formatHints, &format);

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(CLASS_NAME, "Win32 Example", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, GetModuleHandle(NULL), NULL);
	HDC hdc = GetDC(hwnd);

	hoggl_context context;
	hoggl_context_hints contextHints;
	hoggl_context_hints_copy_default(&contextHints);

	hoggl_context_init(&contextHints, &format, &context);

	ShowWindow(hwnd, SW_SHOW);

	hoggl_surface surface;
	hoggl_surface_init(&surface, (void*)hdc);

	hoggl_context_make_current(&context, &surface);

    MSG msg = {0};
    while (shouldClose == FALSE) {
	    while (PeekMessageA(&msg, NULL, 0u, 0u, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		hoggl_context_swap_buffers(&context, &surface);
	}

	hoggl_context_delete(&context);
	hoggl_display_free(&hogDpy);

	ReleaseDC(hwnd, hdc);
	DestroyWindow(hwnd);
}

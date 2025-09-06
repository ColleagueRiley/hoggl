#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOGGL_IMPLEMENTATION
#include "hoggl.h"

int main(void) {
    Display* display = XOpenDisplay(NULL);

    int s = DefaultScreen(display);

	hoggl_format format;
	hoggl_format_hints formatHints;
	hoggl_format_hints_copy_default(&formatHints);

	hoggl_format_set_display(&format, (struct Display*)display);
	hoggl_format_init(&formatHints, &format);

	XVisualInfo visual = format.visual;

	XSetWindowAttributes swa;
    memset(&swa, 0, sizeof(swa));

	u32 event_mask = ExposureMask | KeyPressMask;

	Colormap cmap;
	swa.colormap = cmap = XCreateColormap(display, DefaultRootWindow(display), visual.visual, AllocNone);
	swa.event_mask = event_mask;

    Window window = XCreateWindow(display, DefaultRootWindow(display), 10, 10, 200, 200,
									0, visual.depth, InputOutput, visual.visual,
									CWColormap | CWBorderPixel | CWEventMask, &swa);


	XFreeColors(display, cmap, NULL, 0, 0);


    XSelectInput(display, window, event_mask);

	hoggl_context context;
	hoggl_context_hints contextHints;
	hoggl_context_hints_copy_default(&contextHints);

	hoggl_context_init(&contextHints, &format, &context);

    XMapWindow(display, window);

	hoggl_surface surface;
	hoggl_surface_init(&surface, (void*)window);

	hoggl_context_make_current(&context, &surface);

	XEvent event;
	while (1) {
		XNextEvent(display, &event);

        if (event.type == KeyPress && event.xkey.keycode == XKeysymToKeycode(display, XK_Escape))
            break;

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		hoggl_context_swap_buffers(&context, &surface);
    }

	hoggl_context_delete(&context);
    XCloseDisplay(display);
 }

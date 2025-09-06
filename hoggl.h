/*
* hoggl 0.01-dev
*
* Copyright (C) 2025 Riley Mabb (@ColleagueRiley)
*
* libpng license
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.

* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
*
*/

#if !defined(HOGGL_GLX) && !defined(HOGGL_WGL) && !defined(HOGGL_WEBGL) && !defined(HOGGL_NSGL)
	#ifdef __APPLE__
		#define HOGGL_NSGL
	#elif defined(_WIN32)
		#define HOGGL_WGL
	#elif defined(__EMSCRIPTEN__)
		#define HOGGL_WEBGL
	#elif defined(__linux__) || defined(__unix__)
		#define HOGGL_GLX
	#else
		#error No platform defined or detected
	#endif
#endif

#if defined(HOGGL_EXPORT) ||  defined(HOGGL_IMPORT)
	#if defined(_WIN32)
		#if defined(__TINYC__) && (defined(HOGGL_EXPORT) ||  defined(HOGGL_IMPORT))
			#define __declspec(x) __attribute__((x))
		#endif

		#if defined(HOGGL_EXPORT)
			#define HOGGLDEF __declspec(dllexport)
		#else
			#define HOGGLDEF __declspec(dllimport)
		#endif
	#else
		#if defined(HOGGL_EXPORT)
			#define HOGGLDEF __attribute__((visibility("default")))
		#endif
	#endif
    #ifndef HOGGLDEF
        #define HOGGLDEF
    #endif
#endif

#ifndef HOGGL_INT_DEFINED
	#ifdef HOGGL_USE_INT /* optional for any system that might not have stdint.h */
		typedef unsigned char       u8;
		typedef signed char         i8;
		typedef unsigned short     u16;
		typedef signed short 	   i16;
		typedef unsigned long int  u32;
		typedef signed long int    i32;
		typedef unsigned long long u64;
		typedef signed long long   i64;
	#else /* use stdint standard types instead of c "standard" types */
		#include <stdint.h>

		typedef uint8_t     u8;
		typedef int8_t      i8;
		typedef uint16_t   u16;
		typedef int16_t    i16;
		typedef uint32_t   u32;
		typedef int32_t    i32;
		typedef uint64_t   u64;
		typedef int64_t    i64;
	#endif
	#define HOGGL_INT_DEFINED
#endif

#ifndef HOGGL_BOOL_DEFINED
    #define HOGGL_BOOL_DEFINED
    typedef u8 hoggl_bool;
#endif

#ifndef HOGGL_ASSERT
	#include <assert.h>
	#define HOGGL_ASSERT assert
#endif

#ifndef HOGGLDEF
	#ifdef HOGGL_C89
		#define HOGGLDEF __inline
	#else
		#define HOGGLDEF inline
	#endif
#endif

#ifndef HOGGL_HEADER
#define HOGGL_HEADER

#define HOGGL_ENUM(type, name) type name; enum

#define HOGGL_BOOL(x) (hoggl_bool)((x) != 0) /* force a value to be 0 or 1 */
#define HOGGL_TRUE (hoggl_bool)1
#define HOGGL_FALSE (hoggl_bool)0

typedef struct hoggl_display {
	void* src;
} hoggl_display;

HOGGLDEF void hoggl_display_init(void* src, hoggl_display* display);
HOGGLDEF void hoggl_display_free(hoggl_display* display);

#ifdef HOGGL_GLX

typedef struct hoggl_format {
	hoggl_display* display;
	XVisualInfo visual;
	void* fbc;
} hoggl_format;

#elif defined(HOGGL_WGL)

typedef struct hoggl_format {
	hoggl_display* display;
} hoggl_format;

#endif

typedef struct hoggl_context {
	hoggl_display* display;
	void* ctx;
} hoggl_context;

typedef struct hoggl_surface {
	void* src;
} hoggl_surface;

typedef HOGGL_ENUM(i32, hoggl_releaseBehavior)   {
	hoggl_release_flush = 0,
	hoggl_release_none
};
typedef HOGGL_ENUM(i32, hoggl_profile)  { hoggl_core = 0, hoggl_compatibility, hoggl_es};
typedef HOGGL_ENUM(i32, hoggl_renderer)  { hoggl_accelerated = 0, hoggl_software };

typedef struct hoggl_format_hints {
	i32 stencil;  /*!< set stencil buffer bit size (0 by default) */
	i32 samples; /*!< set number of sample buffers (0 by default) */
	i32 stereo; /*!< hint the context to use stereoscopic frame buffers for 3D (false by default) */
	i32 auxBuffers; /*!< number of aux buffers (0 by default) */
	i32 doubleBuffer; /*!< request double buffering (true by default) */
	i32 red, green, blue, alpha; /*!< set color bit sizes (all 8 by default) */
	i32 depth; /*!< set depth buffer bit size (24 by default) */
	i32 accumRed, accumGreen, accumBlue, accumAlpha; /*!< set accumulated RGBA bit sizes (all 0 by default) */
	hoggl_bool sRGB; /*!< request sRGA format (false by default) */
	hoggl_renderer renderer; /*!< renderer to use e.g. accelerated or software defaults to accelerated */
} hoggl_format_hints;

typedef struct hoggl_context_hints {
	hoggl_bool robustness; /*!< request a "robust" (as in memory-safe) context (false by default). For more information check the overview section: https://registry.khronos.org/OpenGL/extensions/EXT/EXT_robustness.txt */
	hoggl_bool debug; /*!< request OpenGL debugging (false by default). */
	hoggl_bool noError; /*!< request no OpenGL errors (false by default). This causes OpenGL errors to be undefined behavior. For more information check the overview section: https://registry.khronos.org/OpenGL/extensions/KHR/KHR_no_error.txt */
	hoggl_releaseBehavior releaseBehavior; /*!< hint how the OpenGL driver should behave when changing contexts (hoggl_ReleaseNone by default). For more information check the overview section: https://registry.khronos.org/OpenGL/extensions/KHR/KHR_context_flush_control.txt */
	hoggl_profile profile; /*!< set OpenGL API profile (hoggl_Core by default) */
	i32 major, minor;  /*!< set the OpenGL API profile version (by default hoggl_Major is 1, hoggl_Minor is 0) */
	hoggl_context* share; /*!< Share this OpenGL context with newly created OpenGL contexts; defaults to NULL. */
} hoggl_context_hints;

/*! copy the default hints to your hint struct */
HOGGLDEF void hoggl_format_hints_copy_default(hoggl_format_hints* hints);
HOGGLDEF void hoggl_context_hints_copy_default(hoggl_context_hints* hints);

HOGGLDEF hoggl_bool hoggl_format_init(hoggl_display* display, hoggl_format_hints* hints, hoggl_format* ctx);

/*!< create an OpenGL context using the user supplied context struct */
HOGGLDEF hoggl_bool hoggl_context_init(hoggl_context_hints* hints, hoggl_format* format, hoggl_context* ctx);
/*!< deletes the opengl context | this will be automatically called by `hoggl_context_close` if the window's context is not NULL */
HOGGLDEF void hoggl_context_delete(hoggl_context* ctx);

HOGGLDEF void hoggl_surface_init(hoggl_surface* surf, void* src);

HOGGLDEF void* hoggl_context_get_source(hoggl_context* ctx);

HOGGLDEF void hoggl_context_make_current(hoggl_context* ctx, hoggl_surface* surf); /*!< to be called by hoggl_context_makeCurrent */
HOGGLDEF void hoggl_context_swap_buffers(hoggl_context* ctx, hoggl_surface* surf); /*!< swap OpenGL buffer (only) called by hoggl_context_swapInterval  */

HOGGLDEF void* hoggl_context_current(void); /*!< get the current context (OpenGL backend (GLX) (WGL) (cocoa) (webgl))*/

/*! set swapInterval / enable vsync */
HOGGLDEF void hoggl_context_swap_interval(hoggl_context* ctx, hoggl_surface* surf, i32 swapInterval);

typedef void (*hoggl_proc)(void); /* function pointer equivalent of void* */

HOGGLDEF hoggl_proc hoggl_get_proc_address(const char* procname); /*!< get native OpenGL proc address */
HOGGLDEF hoggl_bool hoggl_extension_supported_platform(hoggl_display* display, const char* extension, size_t len);	/*!< check if whether the specified platform-specific API extension is supported by the current OpenGL or OpenGL ES context */

typedef struct hoggl_attribStack {
	i32* attribs;
	size_t count;
	size_t max;
} hoggl_attribStack;
HOGGLDEF void hoggl_attribStack_init(hoggl_attribStack* stack, i32* attribs, size_t max);
HOGGLDEF void hoggl_attribStack_pushAttrib(hoggl_attribStack* stack, i32 attrib);
HOGGLDEF void hoggl_attribStack_pushAttribs(hoggl_attribStack* stack, i32 attrib1, i32 attrib2);

HOGGLDEF hoggl_bool hoggl_extension_supported_str(const char* extensions, const char* ext, size_t len);

#endif /* HOGGL_HEADER */

#ifdef HOGGL_IMPLEMENTATION

void hoggl_format_hints_copy_default(hoggl_format_hints* hints) {
	static hoggl_format_hints defaultHints = {
		/* Stencil         */ 0,
		/* Samples         */ 0,
		/* Stereo          */ HOGGL_FALSE,
		/* AuxBuffers      */ 0,
		/* DoubleBuffer    */ HOGGL_TRUE,
		/* Red             */ 8,
		/* Green           */ 8,
		/* Blue            */ 8,
		/* Alpha           */ 8,
		/* Depth           */ 24,
		/* AccumRed        */ 0,
		/* AccumGreen      */ 0,
		/* AccumBlue       */ 0,
		/* AccumAlpha      */ 0,
		/* SRGB            */ HOGGL_FALSE,
		/* renderer */ hoggl_accelerated
	};

	*hints = defaultHints;
}

void hoggl_context_hints_copy_default(hoggl_context_hints* hints) {
	static hoggl_context_hints defaultHints = {
		/* Robustness      */ HOGGL_FALSE,
		/* Debug           */ HOGGL_FALSE,
		/* NoError         */ HOGGL_FALSE,
		/* ReleaseBehavior */ hoggl_release_none,
		/* Profile         */ hoggl_core,
		/* Major           */ 1,
		/* Minor           */ 0,
		/* Share */ NULL,
	};

	*hints = defaultHints;
}

void hoggl_attribStack_init(hoggl_attribStack* stack, i32* attribs, size_t max) {
	stack->attribs = attribs;
	stack->count = 0;
	stack->max = max;
}

void hoggl_attribStack_pushAttrib(hoggl_attribStack* stack, i32 attrib) {
	HOGGL_ASSERT(stack->count < stack->max);
	stack->attribs[stack->count] = attrib;
	stack->count += 1;
}

void hoggl_attribStack_pushAttribs(hoggl_attribStack* stack, i32 attrib1, i32 attrib2) {
	hoggl_attribStack_pushAttrib(stack, attrib1);
	hoggl_attribStack_pushAttrib(stack, attrib2);
}

hoggl_bool hoggl_extension_supported_str(const char* extensions, const char* ext, size_t len) {
    const char *start = extensions;
    const char *where;
    const char* terminator;

    if (extensions == NULL || ext == NULL) {
        return HOGGL_FALSE;
	}

	while (ext[len - 1] == '\0' && len > 3) {
		len--;
	}

    where = strstr(extensions, ext);
    while (where) {
		terminator = where + len;
        if ((where == start || *(where - 1) == ' ') &&
            (*terminator == ' ' || *terminator == '\0')) {
			return HOGGL_TRUE;
        }
        where = strstr(terminator, ext);
    }

    return HOGGL_FALSE;
}

void hoggl_surface_init(hoggl_surface* surf, void* src) {
	surf->src = src;
}

#if defined(HOGGL_GLX)

#include <GL/glx.h>

typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

void hoggl_display_init(void* src, hoggl_display* display) {
	display->src = src;
}

void hoggl_display_free(hoggl_display* display) {

}

hoggl_bool hoggl_format_init(hoggl_display* display, hoggl_format_hints* hints, hoggl_format* format) {
	format->display = display;
	/* for checking extensions later */
	const char sRGBARBstr[] = "GLX_ARB_framebuffer_sRGB";
	const char sRGBEXTstr[] = "GLX_EXT_framebuffer_sRGB";

	hoggl_bool transparent = HOGGL_FALSE;

	i32 visual_attribs[40];
	hoggl_attribStack stack;
	hoggl_attribStack_init(&stack, visual_attribs, 40);
	hoggl_attribStack_pushAttribs(&stack, GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR);
	hoggl_attribStack_pushAttribs(&stack, GLX_X_RENDERABLE, 1);
	hoggl_attribStack_pushAttribs(&stack, GLX_RENDER_TYPE, GLX_RGBA_BIT);
	hoggl_attribStack_pushAttribs(&stack, GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT);
	hoggl_attribStack_pushAttribs(&stack, GLX_DOUBLEBUFFER, 1);
	hoggl_attribStack_pushAttribs(&stack, GLX_ALPHA_SIZE, hints->alpha);
	hoggl_attribStack_pushAttribs(&stack, GLX_DEPTH_SIZE, hints->depth);
	hoggl_attribStack_pushAttribs(&stack, GLX_STENCIL_SIZE, hints->stencil);
	hoggl_attribStack_pushAttribs(&stack, GLX_STEREO, hints->stereo);
	hoggl_attribStack_pushAttribs(&stack, GLX_AUX_BUFFERS, hints->auxBuffers);
	hoggl_attribStack_pushAttribs(&stack, GLX_RED_SIZE, hints->red);
	hoggl_attribStack_pushAttribs(&stack, GLX_GREEN_SIZE, hints->green);
	hoggl_attribStack_pushAttribs(&stack, GLX_BLUE_SIZE, hints->blue);
	hoggl_attribStack_pushAttribs(&stack, GLX_ACCUM_RED_SIZE, hints->accumRed);
	hoggl_attribStack_pushAttribs(&stack, GLX_ACCUM_GREEN_SIZE, hints->accumGreen);
	hoggl_attribStack_pushAttribs(&stack, GLX_ACCUM_BLUE_SIZE, hints->accumBlue);
	hoggl_attribStack_pushAttribs(&stack, GLX_ACCUM_ALPHA_SIZE, hints->accumAlpha);

	if (hints->sRGB) {
		if (hoggl_extension_supported_platform(format->display, sRGBARBstr, sizeof(sRGBARBstr)))
			hoggl_attribStack_pushAttribs(&stack, GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, hints->sRGB);
		if (hoggl_extension_supported_platform(format->display, sRGBEXTstr, sizeof(sRGBEXTstr)))
			hoggl_attribStack_pushAttribs(&stack, GLX_FRAMEBUFFER_SRGB_CAPABLE_EXT, hints->sRGB);
	}

	hoggl_attribStack_pushAttribs(&stack, 0, 0);

	/* find the configs */
	i32 fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig((Display*)format->display->src, DefaultScreen((Display*)format->display->src), visual_attribs, &fbcount);

	i32 best_fbc = -1;
	i32 best_depth = 0;
	i32 best_samples = 0;

	if (fbcount == 0) {
		fprintf(stderr, "Failed to find any valid GLX visual configs.");
		return 0;
	}

	/* search through all found configs to find the best match */
	i32 i;
	for (i = 0; i < fbcount; i++) {
		XVisualInfo* vi = glXGetVisualFromFBConfig((Display*)format->display->src, fbc[i]);
		if (vi == NULL)
			continue;

		i32 samp_buf, samples;
		glXGetFBConfigAttrib((Display*)format->display->src, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
		glXGetFBConfigAttrib((Display*)format->display->src, fbc[i], GLX_SAMPLES, &samples);

		if (best_fbc == -1) best_fbc = i;
		if ((!(transparent) || vi->depth == 32)  && best_depth == 0) {
			best_fbc = i;
			best_depth = vi->depth;
		}
		if ((!(transparent) || vi->depth == 32) && samples <= hints->samples && samples > best_samples) {
			best_fbc = i;
			best_depth = vi->depth;
			best_samples = samples;
		}
		XFree(vi);
	}

	if (best_fbc == -1) {
		fprintf(stderr, "Failed to get a valid GLX visual.");
		return 0;
	}

	/* we found a config */
	format->fbc = fbc[best_fbc];
	XVisualInfo* vi = glXGetVisualFromFBConfig((Display*)format->display->src, format->fbc);
	if (vi->depth != 32 && transparent)
		fprintf(stderr, "Failed to to find a matching visual with a 32-bit depth.");

	if (best_samples < hints->samples)
		fprintf(stderr, "Failed to load a matching sample count.");

	XFree(fbc);
	format->visual = *vi;
	XFree(vi);

	return 1;
}

hoggl_bool hoggl_context_init(hoggl_context_hints* hints, hoggl_format* format, hoggl_context* ctx) {
	const char noErorrStr[]  = "GLX_ARB_create_context_no_error";
	const char flushStr[] = "GLX_ARB_context_flush_control";
	const char robustStr[]	= "GLX_ARB_create_context_robustness";

	ctx->display = format->display;

	/* create the actual OpenGL context  */
	i32 context_attribs[40];

	hoggl_attribStack stack;
	hoggl_attribStack_init(&stack, context_attribs, 40);

	i32 mask = 0;
	switch (hints->profile) {
		case hoggl_es: mask |= GLX_CONTEXT_ES_PROFILE_BIT_EXT; break;
		case hoggl_compatibility: mask |= GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB; break;
		case hoggl_core: mask |= GLX_CONTEXT_CORE_PROFILE_BIT_ARB; break;
		default: mask |= GLX_CONTEXT_CORE_PROFILE_BIT_ARB; break;
	}

	hoggl_attribStack_pushAttribs(&stack, GLX_CONTEXT_PROFILE_MASK_ARB, mask);

	if (hints->minor || hints->major) {
		hoggl_attribStack_pushAttribs(&stack, GLX_CONTEXT_MAJOR_VERSION_ARB, hints->major);
		hoggl_attribStack_pushAttribs(&stack, GLX_CONTEXT_MINOR_VERSION_ARB, hints->minor);
	}


	if (hoggl_extension_supported_platform(format->display, flushStr, sizeof(flushStr))) {
		if (hints->releaseBehavior == hoggl_release_flush) {
			hoggl_attribStack_pushAttribs(&stack, GLX_CONTEXT_RELEASE_BEHAVIOR_ARB, GLX_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB);
		} else if (hints->releaseBehavior == hoggl_release_none) {
			hoggl_attribStack_pushAttribs(&stack, GLX_CONTEXT_RELEASE_BEHAVIOR_ARB, GLX_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB);
		}
	}

	i32 flags = 0;
	if (hints->debug) flags |= GLX_CONTEXT_FLAGS_ARB;
	if (hints->robustness && hoggl_extension_supported_platform(format->display, robustStr, sizeof(robustStr))) flags |= GLX_CONTEXT_ROBUST_ACCESS_BIT_ARB;
	if (flags)
		hoggl_attribStack_pushAttribs(&stack, GLX_CONTEXT_FLAGS_ARB, flags);

	if (hoggl_extension_supported_platform(format->display, noErorrStr, sizeof(noErorrStr)))
		hoggl_attribStack_pushAttribs(&stack, GLX_CONTEXT_OPENGL_NO_ERROR_ARB, hints->noError);

	hoggl_attribStack_pushAttribs(&stack, 0, 0);

	/*  create the context */
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
		glXGetProcAddressARB((u8*) "glXCreateContextAttribsARB");

	GLXContext share = NULL;
	if (hints->share)
		share = hints->share->ctx;

	if (glXCreateContextAttribsARB == NULL) {
		fprintf(stderr, "Failed to load proc address 'glXCreateContextAttribsARB', loading a generic OpenGL context.");
		ctx->ctx = glXCreateContext((Display*)ctx->display->src, &format->visual, share, True);
	} else {
		ctx->ctx = glXCreateContextAttribsARB((Display*)ctx->display->src, format->fbc, share, True, context_attribs);
		if (ctx->ctx == NULL) {
			fprintf(stderr, "Failed to create an OpenGL context with AttribsARB, loading a generic OpenGL context.");
			ctx->ctx = glXCreateContext((Display*)ctx->display->src, &format->visual, share, True);
		}
	}

	return HOGGL_TRUE;
}

void hoggl_context_swap_buffers(hoggl_context* ctx, hoggl_surface* surf) {
	glXSwapBuffers((Display*)ctx->display->src, (Window)surf->src);
}

void hoggl_context_delete(hoggl_context* ctx) {
	glXDestroyContext((Display*)ctx->display->src, (GLXContext)ctx->ctx);
}

void* hoggl_context_get_source(hoggl_context* ctx) {
	return ctx->ctx;
}

void hoggl_context_make_current(hoggl_context* ctx, hoggl_surface* surf) {
	glXMakeCurrent((Display*)ctx->display->src, (Window)surf->src, (GLXContext)ctx->ctx);
}

void* hoggl_context_current(void) {
	return glXGetCurrentContext();
}

void hoggl_context_swap_interval(hoggl_context* ctx, hoggl_surface* surf, i32 swapInterval) {
	static PFNGLXSWAPINTERVALEXTPROC pfn = (PFNGLXSWAPINTERVALEXTPROC)-1;
	static int (*pfn2)(int) = NULL;

	if (pfn == (PFNGLXSWAPINTERVALEXTPROC)-1) {
		pfn = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress((u8*)"glXSwapIntervalEXT");
		if (pfn == NULL)  {
			const char* array[] = {"GLX_MESA_swap_control", "GLX_SGI_swap_control"};

			size_t i;
			for (i = 0; i < sizeof(array) / sizeof(char*) && pfn2 == NULL; i++) {
				pfn2 = (int(*)(int))glXGetProcAddress((u8*)array[i]);
			}

			if (pfn2 != NULL) {
				fprintf(stderr, "Failed to load swap interval function, fallingback to the native swapinterval function");
			} else {
				fprintf(stderr, "Failed to load swap interval function");
			}
		}
	}

	if (pfn != NULL) {
		pfn((Display*)ctx->display->src, (Window)surf->src, swapInterval);
	}
	else if (pfn2 != NULL) {
		pfn2(swapInterval);
	}
}

hoggl_proc hoggl_get_proc_address(const char* procname) {
	return glXGetProcAddress(procname);
}

hoggl_bool hoggl_extension_supported_platform(hoggl_display* display, const char* extension, size_t len) {
	/* TODO : fix */
	const char* extensions = glXQueryExtensionsString((Display*)display->src, XDefaultScreen((Display*)display->src));
	return (extensions != NULL) && hoggl_extension_supported_str(extensions, extension, len);
}

#elif defined(HOGGL_WGL)

#define WGL_ACCELERATION_ARB             0x2003
#define WGL_FULL_ACCELERATION_ARB        0x2027
#define WGL_DRAW_TO_WINDOW_ARB           0x2001
#define WGL_PIXEL_TYPE_ARB               0x2013
#define WGL_TYPE_RGBA_ARB                0x202b
#define WGL_SUPPORT_OPENGL_ARB           0x2010
#define WGL_COLOR_BITS_ARB               0x2014
#define WGL_DOUBLE_BUFFER_ARB            0x2011
#define WGL_ALPHA_BITS_ARB               0x201b
#define WGL_DEPTH_BITS_ARB               0x2022
#define WGL_STENCIL_BITS_ARB             0x2023
#define WGL_STEREO_ARB                  0x2012
#define WGL_AUX_BUFFERS_ARB              0x2024
#define WGL_RED_BITS_ARB                 0x2015
#define WGL_GREEN_BITS_ARB               0x2017
#define WGL_BLUE_BITS_ARB                0x2019
#define WGL_ACCUM_RED_BITS_ARB           0x201e
#define WGL_ACCUM_GREEN_BITS_ARB         0x201f
#define WGL_ACCUM_BLUE_BITS_ARB          0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB         0x2021
#define WGL_COLORSPACE_SRGB_EXT          0x3089
#define WGL_CONTEXT_OPENGL_NO_ERROR_ARB  0x31b3
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB         0x2097
#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB    0x0000
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB   0x2098
#define WGL_CONTEXT_FLAGS_ARB            0x2094
#define WGL_ACCESS_READ_WRITE_NV         0x00000001
#define WGL_COVERAGE_SAMPLES_NV          0x2042
#define WGL_CONTEXT_ES_PROFILE_BIT_EXT   0x00000004
#define WGL_CONTEXT_PROFILE_MASK_ARB               0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB            0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB   0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB               0x2092
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB             0x20A9

#include <stdio.h>

typedef HRESULT (APIENTRY* PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;

typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALEXTPROC)(int interval);
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hdc, HGLRC hglrc, const int *attribList);
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

HMODULE hoggl_wgl_dll = NULL;
void hoggl_win32_load_funcs(void) {
     if (hoggl_wgl_dll != NULL && wglSwapIntervalEXT != NULL && wglChoosePixelFormatARB != NULL && wglChoosePixelFormatARB != NULL)
		return;

    HWND dummyWin = CreateWindowA("dummyhoggl", "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);

	if (hoggl_wgl_dll == NULL)
		hoggl_wgl_dll = LoadLibraryA("opengl32.dll");

	HDC dummy_dc = GetDC(dummyWin);
	u32 pfd_flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

	PIXELFORMATDESCRIPTOR pfd = {sizeof(pfd), 1, pfd_flags, PFD_TYPE_RGBA, 32, 8, PFD_MAIN_PLANE, 32, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 32, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0};

	int dummy_pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
	SetPixelFormat(dummy_dc, dummy_pixel_format, &pfd);

	HGLRC dummy_context = wglCreateContext(dummy_dc);

	HGLRC cur = wglGetCurrentContext();
	wglMakeCurrent(dummy_dc, dummy_context);

	wglCreateContextAttribsARB = ((PFNWGLCREATECONTEXTATTRIBSARBPROC(WINAPI *)(const char*)) wglGetProcAddress)("wglCreateContextAttribsARB");
	wglChoosePixelFormatARB = ((PFNWGLCHOOSEPIXELFORMATARBPROC(WINAPI *)(const char*)) wglGetProcAddress)("wglChoosePixelFormatARB");

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)(hoggl_proc)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT == NULL) {
        fprintf(stderr, "Failed to load swap interval function");
    }

	wglMakeCurrent(dummy_dc, cur);
	wglDeleteContext(dummy_context);
	ReleaseDC(dummyWin, dummy_dc);
	DestroyWindow(dummyWin);
}

void hoggl_display_init(void* src, hoggl_display* display) {
	display->src = src;
	hoggl_win32_load_funcs();
}

void hoggl_display_free(hoggl_display* display) {
	FreeLibrary(hoggl_wgl_dll);
	hoggl_wgl_dll = NULL;
	wglSwapIntervalEXT = NULL;
	wglChoosePixelFormatARB = NULL;
	wglChoosePixelFormatARB = NULL;
}

hoggl_bool hoggl_format_init(hoggl_display* display, hoggl_format_hints* hints, hoggl_format* format) {
	format->display = display;

	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType   = PFD_TYPE_RGBA;
	pfd.iLayerType   = PFD_MAIN_PLANE;
	pfd.cColorBits   = 32;
	pfd.cAlphaBits   = 8;
	pfd.cDepthBits   = 24;
	pfd.cStencilBits = (BYTE)hints->stencil;
	pfd.cAuxBuffers  = (BYTE)hints->auxBuffers;
	if (hints->stereo) pfd.dwFlags |= PFD_STEREO;

	/* try to create the pixel format we want for OpenGL and then try to create an OpenGL context for the specified version */
	if (hints->renderer == hoggl_software)
		pfd.dwFlags |= PFD_GENERIC_FORMAT | PFD_GENERIC_ACCELERATED;

	/* get pixel format, default to a basic pixel format */
	int pixel_format = ChoosePixelFormat(display->src, &pfd);
	if (wglChoosePixelFormatARB != NULL) {
		i32 pixel_format_attribs[50];
		hoggl_attribStack stack;
		hoggl_attribStack_init(&stack, pixel_format_attribs, 50);

		hoggl_attribStack_pushAttribs(&stack, WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB);
		hoggl_attribStack_pushAttribs(&stack, WGL_DRAW_TO_WINDOW_ARB, 1);
		hoggl_attribStack_pushAttribs(&stack, WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB);
		hoggl_attribStack_pushAttribs(&stack, WGL_SUPPORT_OPENGL_ARB, 1);
		hoggl_attribStack_pushAttribs(&stack, WGL_COLOR_BITS_ARB, 32);
		hoggl_attribStack_pushAttribs(&stack, WGL_DOUBLE_BUFFER_ARB, 1);
		hoggl_attribStack_pushAttribs(&stack, WGL_ALPHA_BITS_ARB, hints->alpha);
		hoggl_attribStack_pushAttribs(&stack, WGL_DEPTH_BITS_ARB, hints->depth);
		hoggl_attribStack_pushAttribs(&stack, WGL_STENCIL_BITS_ARB, hints->stencil);
		hoggl_attribStack_pushAttribs(&stack, WGL_STEREO_ARB, hints->stereo);
		hoggl_attribStack_pushAttribs(&stack, WGL_AUX_BUFFERS_ARB, hints->auxBuffers);
		hoggl_attribStack_pushAttribs(&stack, WGL_RED_BITS_ARB, hints->red);
		hoggl_attribStack_pushAttribs(&stack, WGL_GREEN_BITS_ARB, hints->blue);
		hoggl_attribStack_pushAttribs(&stack, WGL_BLUE_BITS_ARB, hints->green);
		hoggl_attribStack_pushAttribs(&stack, WGL_ACCUM_RED_BITS_ARB, hints->accumRed);
		hoggl_attribStack_pushAttribs(&stack, WGL_ACCUM_GREEN_BITS_ARB, hints->accumBlue);
		hoggl_attribStack_pushAttribs(&stack, WGL_ACCUM_BLUE_BITS_ARB, hints->accumGreen);
		hoggl_attribStack_pushAttribs(&stack, WGL_ACCUM_ALPHA_BITS_ARB, hints->accumAlpha);

		if(hints->sRGB) {
			hoggl_attribStack_pushAttribs(&stack, WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, 1);
		}

		hoggl_attribStack_pushAttribs(&stack, WGL_COVERAGE_SAMPLES_NV, hints->samples);

		hoggl_attribStack_pushAttribs(&stack, 0, 0);

		int new_pixel_format;
		UINT num_formats;
		wglChoosePixelFormatARB(display->src, pixel_format_attribs, 0, 1, &new_pixel_format, &num_formats);
		if (!num_formats)
			fprintf(stderr, "Failed to create a pixel format for WGL\n");
		else pixel_format = new_pixel_format;
	}

	PIXELFORMATDESCRIPTOR suggested;
	if (!DescribePixelFormat(display->src, pixel_format, sizeof(suggested), &suggested) ||
		!SetPixelFormat(display->src, pixel_format, &pfd))
			fprintf(stderr, "Failed to set the WGL pixel format\n");
}

hoggl_bool hoggl_context_init(hoggl_context_hints* hints, hoggl_format* format, hoggl_context* ctx) {
	ctx->display = format->display;

	if (wglCreateContextAttribsARB != NULL) {
		/* create OpenGL/WGL context for the specified version */
		i32 attribs[40];
		hoggl_attribStack stack;
		hoggl_attribStack_init(&stack, attribs, 40);

		i32 mask = 0;
		switch (hints->profile) {
			case hoggl_es: mask |= WGL_CONTEXT_ES_PROFILE_BIT_EXT; break;
			case hoggl_compatibility: mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB; break;
			case hoggl_core: mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB; break;
			default: mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB; break;
		}

		hoggl_attribStack_pushAttribs(&stack, WGL_CONTEXT_PROFILE_MASK_ARB, mask);

		if (hints->minor || hints->major) {
			hoggl_attribStack_pushAttribs(&stack, WGL_CONTEXT_MAJOR_VERSION_ARB, hints->major);
			hoggl_attribStack_pushAttribs(&stack, WGL_CONTEXT_MINOR_VERSION_ARB, hints->minor);
		}

		hoggl_attribStack_pushAttribs(&stack, WGL_CONTEXT_OPENGL_NO_ERROR_ARB, hints->noError);

		if (hints->releaseBehavior == hoggl_release_flush) {
			hoggl_attribStack_pushAttribs(&stack, 0x2097, WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB); // WGL_CONTEXT_RELEASE_BEHAVIOR_ARB
		} else if (hints->releaseBehavior == hoggl_release_none) {
			hoggl_attribStack_pushAttribs(&stack, 0x2097, 0x0000); // WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB
		}

		i32 flags = 0;
		if (hints->debug) flags |= WGL_ACCESS_READ_WRITE_NV; // substitute for debug bit, not exact
		if (hints->robustness) flags |= WGL_CONTEXT_ES_PROFILE_BIT_EXT; // robustness placeholder
		hoggl_attribStack_pushAttribs(&stack, WGL_CONTEXT_FLAGS_ARB, flags);

		hoggl_attribStack_pushAttribs(&stack, 0, 0);

		ctx->ctx = (HGLRC)wglCreateContextAttribsARB(ctx->display->src, NULL, attribs);
	} else { /* fall back to a default context (probably OpenGL 2 or something) */
		fprintf(stderr, "Failed to create an accelerated OpenGL Context.");
		ctx->ctx = wglCreateContext(ctx->display->src);
	}

	if (hints->share)
		wglShareLists(ctx->ctx, hints->share->ctx);
}

void hoggl_context_swap_buffers(hoggl_context* ctx, hoggl_surface* surf) {
	SwapBuffers(surf->src);
}

void hoggl_context_delete(hoggl_context* ctx) {
	wglDeleteContext((HGLRC) ctx->ctx);
}

void* hoggl_context_get_source(hoggl_context* ctx) {
	return ctx->ctx;
}

void hoggl_context_make_current(hoggl_context* ctx, hoggl_surface* surf) {
	if (ctx == NULL && surf == NULL)
		wglMakeCurrent(NULL, NULL);
	else
		wglMakeCurrent(surf->src, ctx->ctx);
}

void* hoggl_context_current(void) {
	return wglGetCurrentContext();
}

void hoggl_context_swap_interval(hoggl_context* ctx, hoggl_surface* surf, i32 swapInterval) {
    if (wglSwapIntervalEXT == NULL || wglSwapIntervalEXT(swapInterval) == FALSE)
		fprintf(stderr, "Failed to set swap interval\n");
}

hoggl_proc hoggl_get_proc_address(const char* procname) {
    hoggl_proc proc = (hoggl_proc)wglGetProcAddress(procname);
    if (proc)
        return proc;

    return (hoggl_proc) GetProcAddress(hoggl_wgl_dll, procname);

}

hoggl_bool hoggl_extension_supported_platform(hoggl_display* display, const char* extension, size_t len) {
	const char* extensions = NULL;

    hoggl_proc proc = hoggl_get_proc_address("wglGetExtensionsStringARB");
    hoggl_proc proc2 = hoggl_get_proc_address("wglGetExtensionsStringEXT");

    if (proc)
        extensions = ((const char* (*)(HDC))proc)(wglGetCurrentDC());
    else if (proc2)
        extensions = ((const char*(*)(void))proc2)();
    return extensions != NULL && hoggl_extension_supported_str(extensions, extension, len);
}

#elif defined(HOGGL_NSGL)

void hoggl_display_init(void* src, hoggl_display* display) {
	display->src = src;
}

void hoggl_display_free(hoggl_display* display) {

}

hoggl_bool hoggl_format_init(hoggl_display* display, hoggl_format_hints* hints, hoggl_format* format) {
	format->display = display;
}

hoggl_bool hoggl_context_init(hoggl_context_hints* hints, hoggl_format* format, hoggl_context* ctx) {
	ctx->display = format->display;
}

hoggl_context* hoggl_context_get_context(hoggl_context* ctx) {

}

void hoggl_context_delete(hoggl_context* ctx) {

}

void* hoggl_context_get_source(hoggl_context* ctx) {

}

void hoggl_context_swap_buffers(hoggl_context* ctx, hoggl_surface* surf) {

}

void hoggl_context_make_current(hoggl_context* ctx, hoggl_surface* surf) {

}

void* hoggl_context_current(void) {

}

void hoggl_context_swap_interval(hoggl_context* ctx, hoggl_surface* surf, i32 swapInterval) {

}

hoggl_proc hoggl_get_proc_address(const char* procname) {

}

hoggl_bool hoggl_extension_supported_platform(hoggl_display* display, const char* extension, size_t len) {

}

#elif defined(HOGGL_WEBGL)

void hoggl_display_init(void* src, hoggl_display* display) {
	display->src = src;
}

void hoggl_display_free(hoggl_display* display) {

}

hoggl_bool hoggl_format_init(hoggl_display* display, hoggl_format_hints* hints, hoggl_format* format) {
	format->display = display;
}

hoggl_bool hoggl_context_init(hoggl_context_hints* hints, hoggl_format* format, hoggl_context* ctx) {
	ctx->display = format->display;
}

void hoggl_context_swap_buffers(hoggl_context* ctx, hoggl_surface* surf) {

}

hoggl_context* hoggl_context_get_context(hoggl_context* ctx) {

}

void hoggl_context_delete(hoggl_context* ctx) {

}

void* hoggl_context_get_source(hoggl_context* ctx) {

}

void hoggl_context_make_current(hoggl_context* ctx, hoggl_surface* surf) {

}

void* hoggl_context_current(void) {

}

void hoggl_context_swap_interval(hoggl_context* ctx, hoggl_surface* surf, i32 swapInterval) {

}

hoggl_proc hoggl_get_proc_address(const char* procname) {

}

hoggl_bool hoggl_extension_supported_platform(hoggl_display* display, const char* extension, size_t len) {

}

#endif

#endif

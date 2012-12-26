/*
 * egl.c
 *
 *  Created on: Dec 24, 2012
 *      Author: Vincent Simonetti
 */

#include <dlfcn.h>

#include <EGL.h>
#include <EGL/eglext.h>

#include "DebugMacros.h"

//Structure
typedef struct _eglError {
	const char* name;
	EGLint value;
} EGLError;

#define EGL_MAX_ERRORS ((EGL_CONTEXT_LOST - EGL_SUCCESS) + 1)

typedef struct _eglFunc {
	const char* name;
	__eglMustCastToProperFunctionPointerType function;
	__eglMustCastToProperFunctionPointerType* functionPtr;
	EGLError errors[EGL_MAX_ERRORS];

	struct _eglFunc* next;
} EGLFunction;

//Macros
#define EGL_DEBUG_PREFIX "EGL Debug"

#define EGL_DEBUG_MESSAGE(func,message) GEN_DEBUG_MESSAGE(EGL_DEBUG_PREFIX,func,message)
#define EGL_DEBUG_MESSAGE_FORMAT(func,format,...) GEN_DEBUG_MESSAGE_FORMAT(EGL_DEBUG_PREFIX,func,format,__VA_ARGS__)

#define EGL_DEBUG_FUNCCALL_RET_STRING(func,ret) GEN_DEBUG_FUNCCALL_RET_STRING(EGL_DEBUG_PREFIX,func,ret)
#define EGL_DEBUG_FUNCCALL_RET_BOOL(func,ret) GEN_DEBUG_FUNCCALL_RET_BOOL(EGL_DEBUG_PREFIX,EGL_TRUE,EGL_FALSE,func,ret)

#define EGL_DEBUG_FUNCCALL_FORMAT(func,paramFormat,...) GEN_DEBUG_FUNCCALL_FORMAT(EGL_DEBUG_PREFIX,func,paramFormat,__VA_ARGS__)
#define EGL_DEBUG_FUNCCALL_FORMAT_RET_STRING(func,paramFormat,ret,...) GEN_DEBUG_FUNCCALL_FORMAT_RET_STRING(EGL_DEBUG_PREFIX,func,paramFormat,ret,__VA_ARGS__)
#define EGL_DEBUG_FUNCCALL_FORMAT_RET_PTR(func,paramFormat,ret,...) GEN_DEBUG_FUNCCALL_FORMAT_RET_PTR(EGL_DEBUG_PREFIX,func,paramFormat,ret,__VA_ARGS__)
#define EGL_DEBUG_FUNCCALL_FORMAT_RET_BOOL(func,paramFormat,ret,...) GEN_DEBUG_FUNCCALL_FORMAT_RET_BOOL(EGL_DEBUG_PREFIX,EGL_TRUE,EGL_FALSE,func,paramFormat,ret,__VA_ARGS__)

#define EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_STRING(x,paramFormat,ret,...) EGL_DEBUG_FUNCCALL_FORMAT_RET_STRING(x,paramFormat,ret,__VA_ARGS__)\
		EGL_CHECK_ERROR(x);\
		return ret;
#define EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_PTR(x,paramFormat,ret,...) EGL_DEBUG_FUNCCALL_FORMAT_RET_PTR(x,paramFormat,ret,__VA_ARGS__)\
		EGL_CHECK_ERROR(x);\
		return ret;
#define EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_BOOL(x,paramFormat,ret,...) EGL_DEBUG_FUNCCALL_FORMAT_RET_BOOL(x,paramFormat,ret,__VA_ARGS__)\
		EGL_CHECK_ERROR(x);\
		return ret;

#define EGLRUN_NRET(x,a) if(eglInit()) \
{\
	(x a);\
}\
	EGL_DEBUG_MESSAGE(x,"Init failed")

#define EGLRUN_RET(x,a,r) if(eglInit()) \
{\
	return (x a);\
}\
	EGL_DEBUG_MESSAGE(x,"Init failed")\
	return (r);

#define EGLINIT_GETPTR(x) (x##Ptr) = (x##Function)dlsym(library, #x);\
	if(!(x##Ptr))\
	{\
		EGL_DEBUG_MESSAGE(x,"Failed to get ptr")\
		goto INIT_ERROR;\
	}

#define EGLFUNC_ERROR(x) {#x, x}

#define EGLFUNCGROUP_NOPTR(x,next) {GEN_NON_STR_TO_STR(x), (__eglMustCastToProperFunctionPointerType)(x), NULL, {}, next}
#define EGLFUNCGROUP(x,next) {GEN_NON_STR_TO_STR(x), (__eglMustCastToProperFunctionPointerType)(x), (__eglMustCastToProperFunctionPointerType*)&GEN_FUNCPTR(x), {}, next}
#define EGLFUNCGROUP_ERRORS(x,next,...) {GEN_NON_STR_TO_STR(x), (__eglMustCastToProperFunctionPointerType)(x), (__eglMustCastToProperFunctionPointerType*)&GEN_FUNCPTR(x), {__VA_ARGS__}, next}

#define EGL_FIND_FUNC(x) __eglFindFunc(#x)
#define EGL_CHECK_ERROR(x) __eglCheckErrors(#x, EGL_FIND_FUNC(x))

//Function definitions
GEN_FUNC_DEF(EGLint,eglGetError,void)
GEN_FUNC_DEF(EGLDisplay,eglGetDisplay,EGLNativeDisplayType display_id)
GEN_FUNC_DEF(EGLBoolean,eglInitialize,EGLDisplay dpy, EGLint* major, EGLint* minor)
GEN_FUNC_DEF(EGLBoolean,eglTerminate,EGLDisplay dpy)
GEN_FUNC_DEF(const char*,eglQueryString,EGLDisplay dpy, EGLint name)
GEN_FUNC_DEF(EGLBoolean,eglGetConfigs,EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config)
GEN_FUNC_DEF(EGLBoolean,eglChooseConfig,EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config)
GEN_FUNC_DEF(EGLBoolean,eglGetConfigAttrib,EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value)
GEN_FUNC_DEF(EGLSurface,eglCreateWindowSurface,EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list)
GEN_FUNC_DEF(EGLSurface,eglCreatePbufferSurface,EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list)
GEN_FUNC_DEF(EGLSurface,eglCreatePixmapSurface,EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list)
GEN_FUNC_DEF(EGLBoolean,eglDestroySurface,EGLDisplay dpy, EGLSurface surface)
GEN_FUNC_DEF(EGLBoolean,eglQuerySurface,EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value)
GEN_FUNC_DEF(EGLBoolean,eglBindAPI,EGLenum api)
GEN_FUNC_DEF(EGLenum,eglQueryAPI,void)
GEN_FUNC_DEF(EGLBoolean,eglWaitClient,void)
GEN_FUNC_DEF(EGLBoolean,eglReleaseThread,void)
GEN_FUNC_DEF(EGLSurface,eglCreatePbufferFromClientBuffer,EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list)
GEN_FUNC_DEF(EGLBoolean,eglSurfaceAttrib,EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
GEN_FUNC_DEF(EGLBoolean,eglBindTexImage,EGLDisplay dpy, EGLSurface surface, EGLint buffer)
GEN_FUNC_DEF(EGLBoolean,eglReleaseTexImage,EGLDisplay dpy, EGLSurface surface, EGLint buffer)
GEN_FUNC_DEF(EGLBoolean,eglSwapInterval,EGLDisplay dpy, EGLint interval)
GEN_FUNC_DEF(EGLContext,eglCreateContext,EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint* attrib_list)
GEN_FUNC_DEF(EGLBoolean,eglDestroyContext,EGLDisplay dpy, EGLContext ctx)
GEN_FUNC_DEF(EGLBoolean,eglMakeCurrent,EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
GEN_FUNC_DEF(EGLContext,eglGetCurrentContext,void)
GEN_FUNC_DEF(EGLSurface,eglGetCurrentSurface,EGLint readdraw)
GEN_FUNC_DEF(EGLDisplay,eglGetCurrentDisplay,void)
GEN_FUNC_DEF(EGLBoolean,eglQueryContext,EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value)
GEN_FUNC_DEF(EGLBoolean,eglWaitGL,void)
GEN_FUNC_DEF(EGLBoolean,eglWaitNative,EGLint engine)
GEN_FUNC_DEF(EGLBoolean,eglSwapBuffers,EGLDisplay dpy, EGLSurface surface);
GEN_FUNC_DEF(EGLBoolean,eglCopyBuffers,EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target)
GEN_FUNC_DEF(__eglMustCastToProperFunctionPointerType,eglGetProcAddress,const char* procname)

//Function ptrs
GEN_FUNCPTR_DEF(eglGetError)
GEN_FUNCPTR_DEF(eglGetDisplay)
GEN_FUNCPTR_DEF(eglInitialize)
GEN_FUNCPTR_DEF(eglTerminate)
GEN_FUNCPTR_DEF(eglQueryString)
GEN_FUNCPTR_DEF(eglGetConfigs)
GEN_FUNCPTR_DEF(eglChooseConfig)
GEN_FUNCPTR_DEF(eglGetConfigAttrib)
GEN_FUNCPTR_DEF(eglCreateWindowSurface)
GEN_FUNCPTR_DEF(eglCreatePbufferSurface)
GEN_FUNCPTR_DEF(eglCreatePixmapSurface)
GEN_FUNCPTR_DEF(eglDestroySurface)
GEN_FUNCPTR_DEF(eglQuerySurface)
GEN_FUNCPTR_DEF(eglBindAPI)
GEN_FUNCPTR_DEF(eglQueryAPI)
GEN_FUNCPTR_DEF(eglWaitClient)
GEN_FUNCPTR_DEF(eglReleaseThread)
GEN_FUNCPTR_DEF(eglCreatePbufferFromClientBuffer)
GEN_FUNCPTR_DEF(eglSurfaceAttrib)
GEN_FUNCPTR_DEF(eglBindTexImage)
GEN_FUNCPTR_DEF(eglReleaseTexImage)
GEN_FUNCPTR_DEF(eglSwapInterval)
GEN_FUNCPTR_DEF(eglCreateContext)
GEN_FUNCPTR_DEF(eglDestroyContext)
GEN_FUNCPTR_DEF(eglMakeCurrent)
GEN_FUNCPTR_DEF(eglGetCurrentContext)
GEN_FUNCPTR_DEF(eglGetCurrentSurface)
GEN_FUNCPTR_DEF(eglGetCurrentDisplay)
GEN_FUNCPTR_DEF(eglQueryContext)
GEN_FUNCPTR_DEF(eglWaitGL)
GEN_FUNCPTR_DEF(eglWaitNative)
GEN_FUNCPTR_DEF(eglSwapBuffers)
GEN_FUNCPTR_DEF(eglCopyBuffers)
GEN_FUNCPTR_DEF(eglGetProcAddress)

//Library
GLOBAL_FIELDS(EGLBoolean,EGL_TRUE)

EGLFunction functions[] = {
		EGLFUNCGROUP(eglGetError, &functions[1]),
		EGLFUNCGROUP(eglGetDisplay, &functions[2]),
		EGLFUNCGROUP_ERRORS(eglInitialize, &functions[3],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED)),
		EGLFUNCGROUP_ERRORS(eglTerminate, &functions[4], EGLFUNC_ERROR(EGL_BAD_DISPLAY)),
		EGLFUNCGROUP_ERRORS(eglQueryString, &functions[5],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_PARAMETER)),
		EGLFUNCGROUP_ERRORS(eglGetConfigs, &functions[6],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_PARAMETER)),
		EGLFUNCGROUP_ERRORS(eglChooseConfig, &functions[7],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_PARAMETER)),
		EGLFUNCGROUP_ERRORS(eglGetConfigAttrib, &functions[8],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_CONFIG), EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE)),
		EGLFUNCGROUP_ERRORS(eglCreateWindowSurface, &functions[9],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_CONFIG), EGLFUNC_ERROR(EGL_BAD_NATIVE_WINDOW), EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE),
				EGLFUNC_ERROR(EGL_BAD_ALLOC), EGLFUNC_ERROR(EGL_BAD_MATCH)),
		EGLFUNCGROUP_ERRORS(eglCreatePbufferSurface, &functions[10],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_CONFIG), EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE), EGLFUNC_ERROR(EGL_BAD_ALLOC),
				EGLFUNC_ERROR(EGL_BAD_MATCH)),
		EGLFUNCGROUP_ERRORS(eglCreatePixmapSurface, &functions[11],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_CONFIG), EGLFUNC_ERROR(EGL_BAD_NATIVE_PIXMAP), EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE),
				EGLFUNC_ERROR(EGL_BAD_ALLOC), EGLFUNC_ERROR(EGL_BAD_MATCH)),
		EGLFUNCGROUP_ERRORS(eglDestroySurface, &functions[12],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_SURFACE)),
		EGLFUNCGROUP_ERRORS(eglQuerySurface, &functions[13],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_SURFACE), EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE)),
		EGLFUNCGROUP_ERRORS(eglBindAPI, &functions[14], EGLFUNC_ERROR(EGL_BAD_PARAMETER)),
		EGLFUNCGROUP(eglQueryAPI, &functions[15]),
		EGLFUNCGROUP_ERRORS(eglWaitClient, &functions[16], EGLFUNC_ERROR(EGL_BAD_CURRENT_SURFACE)),
		EGLFUNCGROUP(eglReleaseThread, &functions[17]),
		EGLFUNCGROUP_ERRORS(eglCreatePbufferFromClientBuffer, &functions[18],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_CONFIG), EGLFUNC_ERROR(EGL_BAD_PARAMETER), EGLFUNC_ERROR(EGL_BAD_ACCESS),
				EGLFUNC_ERROR(EGL_BAD_ALLOC), EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE), EGLFUNC_ERROR(EGL_BAD_MATCH)),
		EGLFUNCGROUP_ERRORS(eglSurfaceAttrib, &functions[19],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_BAD_MATCH), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_SURFACE), EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE)),
		EGLFUNCGROUP_ERRORS(eglBindTexImage, &functions[19],
				EGLFUNC_ERROR(EGL_BAD_ACCESS), EGLFUNC_ERROR(EGL_BAD_MATCH), EGLFUNC_ERROR(EGL_BAD_SURFACE)),
		EGLFUNCGROUP_ERRORS(eglReleaseTexImage, &functions[20],
				EGLFUNC_ERROR(EGL_BAD_MATCH), EGLFUNC_ERROR(EGL_BAD_SURFACE)),
		EGLFUNCGROUP_ERRORS(eglSwapInterval, &functions[21],
				EGLFUNC_ERROR(EGL_BAD_CONTEXT), EGLFUNC_ERROR(EGL_BAD_SURFACE)),
		EGLFUNCGROUP_ERRORS(eglCreateContext, &functions[22],
				EGLFUNC_ERROR(EGL_BAD_MATCH), EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_CONFIG), EGLFUNC_ERROR(EGL_BAD_CONTEXT),
				EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE), EGLFUNC_ERROR(EGL_BAD_ALLOC)),
		EGLFUNCGROUP_ERRORS(eglDestroyContext, &functions[23],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_CONTEXT)),
		EGLFUNCGROUP_ERRORS(eglMakeCurrent, &functions[24],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_SURFACE), EGLFUNC_ERROR(EGL_BAD_CONTEXT), EGLFUNC_ERROR(EGL_BAD_MATCH),
				EGLFUNC_ERROR(EGL_BAD_ACCESS), EGLFUNC_ERROR(EGL_BAD_NATIVE_PIXMAP), EGLFUNC_ERROR(EGL_BAD_NATIVE_WINDOW), EGLFUNC_ERROR(EGL_BAD_CURRENT_SURFACE),
				EGLFUNC_ERROR(EGL_BAD_ALLOC), EGLFUNC_ERROR(EGL_CONTEXT_LOST)),
		EGLFUNCGROUP(eglGetCurrentContext, &functions[25]),
		EGLFUNCGROUP(eglGetCurrentSurface, &functions[26]),
		EGLFUNCGROUP(eglGetCurrentDisplay, &functions[27]),
		EGLFUNCGROUP_ERRORS(eglQueryContext, &functions[28],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_CONTEXT), EGLFUNC_ERROR(EGL_BAD_ATTRIBUTE)),
		EGLFUNCGROUP_ERRORS(eglWaitGL, &functions[29], EGLFUNC_ERROR(EGL_BAD_CURRENT_SURFACE)),
		EGLFUNCGROUP_ERRORS(eglWaitNative, &functions[30], EGLFUNC_ERROR(EGL_BAD_CURRENT_SURFACE)),
		EGLFUNCGROUP_ERRORS(eglSwapBuffers, &functions[31],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_SURFACE), EGLFUNC_ERROR(EGL_CONTEXT_LOST)),
		EGLFUNCGROUP_ERRORS(eglCopyBuffers, &functions[32],
				EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED), EGLFUNC_ERROR(EGL_BAD_SURFACE), EGLFUNC_ERROR(EGL_BAD_NATIVE_PIXMAP), EGLFUNC_ERROR(EGL_BAD_MATCH),
				EGLFUNC_ERROR(EGL_CONTEXT_LOST)),
		EGLFUNCGROUP(eglGetProcAddress, &functions[33]),
		EGLFUNCGROUP_NOPTR(eglIsDebugLibraryInitialized, &functions[34]),
		EGLFUNCGROUP_NOPTR(eglDebugEnableLogging, &functions[35]),
		EGLFUNCGROUP_NOPTR(eglDebugLog, NULL)
};

//Setup
int eglInit()
{
	EGLFunction* func;
	if(library)
	{
		return TRUE;
	}
	slog2c(debugBuffer, 0, SLOG2_INFO, EGL_DEBUG_PREFIX "-Initializing");
	library = dlopen("libEGL.so", RTLD_LAZY);
	if(library)
	{
		for(func = functions; func; func = func->next)
		{
			if(func->functionPtr)
			{
				*(func->functionPtr) = (__eglMustCastToProperFunctionPointerType)dlsym(library, func->name);
				if(!(*(func->functionPtr)))
				{
					if(loggingEnabled)
					{
						slog2fa(debugBuffer, 0, SLOG2_INFO, EGL_DEBUG_PREFIX "-%s-Failed to get ptr", SLOG2_FA_STRING(func->name), SLOG2_FA_END);
					}
					goto INIT_ERROR;
				}
			}
		}
		slog2c(debugBuffer, 0, SLOG2_INFO, EGL_DEBUG_PREFIX "-Initialize complete");
		return TRUE;

	INIT_ERROR:
		slog2c(debugBuffer, 0, SLOG2_ERROR, EGL_DEBUG_PREFIX "-Initialize error");
		dlclose(library);
		library = NULL;
	}
	slog2c(debugBuffer, 0, SLOG2_ERROR, EGL_DEBUG_PREFIX "-Initialization failed");
	return FALSE;
}

GEN_LIB_CLEANUP(eglCleanup,EGL_DEBUG_PREFIX)

void __eglCheckErrors(const char* funcName, EGLFunction* func)
{
	EGLError* err;
	if(loggingEnabled && funcName && func && func->errors[0].value != NULL)
	{
		EGLint error = GEN_FUNCPTR(eglGetError)();
		if(error != EGL_SUCCESS)
		{
			for(err = func->errors; err->value != NULL; err++)
			{
				if(err->value == error)
				{
					slog2fa(debugBuffer, 0, SLOG2_ERROR, EGL_DEBUG_PREFIX "-%s-Error: %s", SLOG2_FA_STRING(funcName), SLOG2_FA_STRING(err->name), SLOG2_FA_END);
					return;
				}
			}
			slog2fa(debugBuffer, 0, SLOG2_ERROR, EGL_DEBUG_PREFIX "-%s-Unknown error: %d", SLOG2_FA_STRING(funcName), SLOG2_FA_SIGNED(error), SLOG2_FA_END);
		}
	}
}

EGLFunction* __eglFindFunc(const char* funcName)
{
	EGLFunction* funcs;
	for(funcs = functions; funcs; funcs = funcs->next)
	{
			if(strcmp(funcs->name, funcName) == 0)
			{
				return funcs;
			}
	}
	return NULL;
}

//Functions
EGLint eglGetError()
{
	EGLRUN_RET(GEN_FUNCPTR(eglGetError), (), EGL_NOT_INITIALIZED)
}

EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id)
{
	if(eglInit())
	{
		EGLDisplay disp = GEN_FUNCPTR(eglGetDisplay)(display_id);
		EGL_DEBUG_FUNCCALL_FORMAT_RET_PTR(eglGetDisplay, "%p", disp, SLOG2_FA_STAR(display_id))
		return disp;
	}
	EGL_DEBUG_MESSAGE(eglGetProcAddress, "Init failed")
	return EGL_NO_DISPLAY;
}

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglInitialize)(dpy, major, minor);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_BOOL(eglInitialize, "%p, %p, %p", ret, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(major), SLOG2_FA_STAR(minor))
	}
	EGL_DEBUG_MESSAGE(eglInitialize, "Init failed")
	return EGL_FALSE;
}

EGLBoolean eglTerminate(EGLDisplay dpy)
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglTerminate)(dpy);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_BOOL(eglTerminate, "%p", ret, SLOG2_FA_STAR(dpy))
	}
	EGL_DEBUG_MESSAGE(eglTerminate, "Init failed")
	return EGL_FALSE;
}

const char* eglQueryString(EGLDisplay dpy, EGLint name)
{
	if(eglInit())
	{
		const char* ret = GEN_FUNCPTR(eglQueryString)(dpy, name);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_STRING(eglQueryString, "%p, %d", ret, SLOG2_FA_STAR(dpy), SLOG2_FA_SIGNED(name))
	}
	EGL_DEBUG_MESSAGE(eglQueryString, "Init failed")
	return NULL;
}

EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config)
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglGetConfigs)(dpy, configs, config_size, num_config);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_BOOL(eglGetConfigs, "%p, %p, %d, %p", ret, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(configs), SLOG2_FA_SIGNED(config_size), SLOG2_FA_STAR(num_config))
	}
	EGL_DEBUG_MESSAGE(eglGetConfigs, "Init failed")
	return EGL_FALSE;
}

EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config)
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglChooseConfig)(dpy, attrib_list, configs, config_size, num_config);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_BOOL(eglChooseConfig, "%p, %p, %p, %d, %p", ret, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(attrib_list), SLOG2_FA_STAR(configs), SLOG2_FA_SIGNED(config_size), SLOG2_FA_STAR(num_config))
	}
	EGL_DEBUG_MESSAGE(eglChooseConfig, "Init failed")
	return EGL_FALSE;
}

EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value)
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglGetConfigAttrib)(dpy, config, attribute, value);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_BOOL(eglGetConfigAttrib, "%p, %p, %d, %p", ret, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(config), SLOG2_FA_SIGNED(attribute), SLOG2_FA_STAR(value))
	}
	EGL_DEBUG_MESSAGE(eglGetConfigAttrib, "Init failed")
	return EGL_FALSE;
}

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list)
{
	if(eglInit())
	{
		EGLSurface surf = GEN_FUNCPTR(eglCreateWindowSurface)(dpy, config, win, attrib_list);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_PTR(eglCreateWindowSurface, "%p, %p, %p, %p", surf, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(config), SLOG2_FA_STAR(win), SLOG2_FA_STAR(attrib_list))
	}
	EGL_DEBUG_MESSAGE(eglCreateWindowSurface, "Init failed")
	return EGL_NO_SURFACE;
}

EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list)
{
	if(eglInit())
	{
		EGLSurface surf = GEN_FUNCPTR(eglCreatePbufferSurface)(dpy, config, attrib_list);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_PTR(eglCreatePbufferSurface, "%p, %p, %p", surf, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(config), SLOG2_FA_STAR(attrib_list))
	}
	EGL_DEBUG_MESSAGE(eglCreatePbufferSurface, "Init failed")
	return EGL_NO_SURFACE;
}

EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list)
{
	if(eglInit())
	{
		EGLSurface surf = GEN_FUNCPTR(eglCreatePixmapSurface)(dpy, config, pixmap, attrib_list);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_PTR(eglCreatePixmapSurface, "%p, %p, %p, %p", surf, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(config), SLOG2_FA_STAR(pixmap), SLOG2_FA_STAR(attrib_list))
	}
	EGL_DEBUG_MESSAGE(eglCreatePixmapSurface, "Init failed")
	return EGL_NO_SURFACE;
}

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglDestroySurface)(dpy, surface);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_BOOL(eglDestroySurface, "%p, %p", ret, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(surface))
	}
	EGL_DEBUG_MESSAGE(eglDestroySurface, "Init failed")
	return EGL_FALSE;
}

EGLBoolean eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value)
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglQuerySurface)(dpy, surface, attribute, value);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_BOOL(eglQuerySurface, "%p, %p, %d, %p", ret, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(surface), SLOG2_FA_SIGNED(attribute), SLOG2_FA_STAR(value))
	}
	EGL_DEBUG_MESSAGE(eglQuerySurface, "Init failed")
	return EGL_FALSE;
}

EGLBoolean eglBindAPI(EGLenum api)
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglBindAPI)(api);
		EGL_DEBUG_FUNCCALL_ECHECK_FORMAT_RET_BOOL(eglBindAPI, "%d", ret, SLOG2_FA_SIGNED(api))
	}
	EGL_DEBUG_MESSAGE(eglBindAPI, "Init failed")
	return EGL_FALSE;
}

EGLenum eglQueryAPI()
{
	const char* api;
	if(eglInit())
	{
		EGLenum ret = GEN_FUNCPTR(eglQueryAPI)();
		switch(ret)
		{
			case EGL_OPENGL_ES_API:
				api = GEN_STR_HELPER(EGL_OPENGL_ES_API);
				break;
			case EGL_OPENVG_API:
				api = GEN_STR_HELPER(EGL_OPENVG_API);
				break;
			case EGL_OPENGL_API:
				api = GEN_STR_HELPER(EGL_OPENGL_API);
				break;
			default:
				api = "?";
				break;
		}
		EGL_DEBUG_FUNCCALL_RET_STRING(eglQueryAPI,api)
		EGL_CHECK_ERROR(eglQueryAPI);
		return ret;
	}
	EGL_DEBUG_MESSAGE(eglQueryAPI, "Init failed")
	return 0;
}

EGLBoolean eglWaitClient()
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglWaitClient)();
		EGL_DEBUG_FUNCCALL_RET_BOOL(eglWaitClient, ret)
		EGL_CHECK_ERROR(eglWaitClient);
		return ret;
	}
	EGL_DEBUG_MESSAGE(eglWaitClient, "Init failed")
	return EGL_FALSE;
}

EGLBoolean eglReleaseThread()
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglReleaseThread)();
		EGL_DEBUG_FUNCCALL_RET_BOOL(eglReleaseThread,ret)
		eglCleanup();
		return ret;
	}
	EGL_DEBUG_MESSAGE(eglReleaseThread, "Never init")
	return EGL_FALSE;
}

EGLSurface EGLAPIENTRY eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list)
{
	//TODO
	return EGL_NO_SURFACE;
}

EGLBoolean eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
	//TODO
	return EGL_FALSE;
}

EGLBoolean eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	//TODO
	return EGL_FALSE;
}

EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	//TODO
	return EGL_FALSE;
}

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	//TODO
	return EGL_FALSE;
}

EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint* attrib_list)
{
	//TODO
	return EGL_NO_CONTEXT;
}

EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
	//TODO
	return EGL_FALSE;
}

EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	//TODO
	return EGL_FALSE;
}

EGLContext eglGetCurrentContext()
{
	//TODO
	return EGL_NO_CONTEXT;
}

EGLSurface eglGetCurrentSurface(EGLint readdraw)
{
	//TODO
	return EGL_NO_SURFACE;
}

EGLDisplay eglGetCurrentDisplay()
{
	//TODO
	return EGL_NO_DISPLAY;
}

EGLBoolean eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value)
{
	//TODO
	return EGL_FALSE;
}

EGLBoolean eglWaitGL()
{
	//TODO
	return EGL_FALSE;
}

EGLBoolean eglWaitNative(EGLint engine)
{
	//TODO
	return EGL_FALSE;
}

EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	//TODO
	return EGL_FALSE;
}

EGLBoolean eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target)
{
	//TODO
	return EGL_FALSE;
}

__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char* procname)
{
	EGLFunction* func;
	if(!procname)
	{
		const char* p = "NULL";
		EGL_DEBUG_FUNCCALL_FORMAT_RET_STRING(eglGetProcAddress, "%s", "NULL", SLOG2_FA_STRING(p));
		return NULL;
	}
	if(eglInit())
	{
		for(func = functions; func; func = func->next)
		{
			if(strcmp(func->name, procname) == 0)
			{
				EGL_DEBUG_FUNCCALL_FORMAT_RET_PTR(eglGetProcAddress, "%s", func->function, SLOG2_FA_STRING(procname))
				return func->function;
			}
		}
		EGL_DEBUG_MESSAGE_FORMAT(eglGetProcAddress, "Could not find %s", SLOG2_FA_STRING(procname))
		return NULL;
	}
	EGL_DEBUG_MESSAGE(eglGetProcAddress, "Init failed")
	return NULL;
}

//Debug functions

EGLBoolean eglIsDebugLibraryInitialized()
{
	const char* enabled = library == NULL ? "EGL_FALSE" : "EGL_TRUE";
	EGL_DEBUG_MESSAGE_FORMAT(eglIsDebugLibraryInitialized, "Library initialized: %s", SLOG2_FA_STRING(enabled))
	return library != NULL;
}

void eglDebugEnableLogging(EGLBoolean le)
{
	if(loggingEnabled)
	{
		const char* change = GEN_BOOL_COMP_TO_STRING(le, EGL_TRUE, EGL_FALSE);
		EGL_DEBUG_MESSAGE_FORMAT(eglDebugEnableLogging, "Changing logging enabled to: %s", SLOG2_FA_STRING(change))
	}
	loggingEnabled = le;
}

void eglDebugLog(slog2_buffer_t buffer)
{
	if(loggingEnabled)
	{
		EGL_DEBUG_MESSAGE_FORMAT(eglDebugLog, "Changing log to: %p", SLOG2_FA_STAR(buffer))
	}
	debugBuffer = buffer;
}

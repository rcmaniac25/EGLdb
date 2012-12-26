/*
 * egl.c
 *
 *  Created on: Dec 24, 2012
 *      Author: Vincent Simonetti
 */

#include <dlfcn.h>

#include <EGL.h>

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

#define EGL_DEBUG_FUNCCALL_FORMAT(func,paramFormat,...) GEN_DEBUG_FUNCCALL_FORMAT(EGL_DEBUG_PREFIX,func,paramFormat,__VA_ARGS__)
#define EGL_DEBUG_FUNCCALL_FORMAT_RET_STRING(func,paramFormat,ret,...) GEN_DEBUG_FUNCCALL_FORMAT_RET_STRING(EGL_DEBUG_PREFIX,func,paramFormat,ret,__VA_ARGS__)
#define EGL_DEBUG_FUNCCALL_FORMAT_RET_PTR(func,paramFormat,ret,...) GEN_DEBUG_FUNCCALL_FORMAT_RET_PTR(EGL_DEBUG_PREFIX,func,paramFormat,ret,__VA_ARGS__)
#define EGL_DEBUG_FUNCCALL_FORMAT_RET_BOOL(func,paramFormat,ret,...) GEN_DEBUG_FUNCCALL_FORMAT_RET_BOOL(EGL_DEBUG_PREFIX,EGL_TRUE,EGL_FALSE,func,paramFormat,ret,__VA_ARGS__)

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
GEN_FUNC_DEF(eglGetError,EGLint,void)
GEN_FUNC_DEF(eglGetDisplay,EGLDisplay,EGLNativeDisplayType display_id)
GEN_FUNC_DEF(eglInitialize,EGLBoolean,EGLDisplay dpy, EGLint *major, EGLint *minor)
GEN_FUNC_DEF(eglTerminate,EGLBoolean,EGLDisplay dpy)
//TODO
GEN_FUNC_DEF(eglReleaseThread,EGLBoolean,void)
//TODO
GEN_FUNC_DEF(eglGetProcAddress,__eglMustCastToProperFunctionPointerType,const char* procname)

//Function ptrs
GEN_FUNCPTR_DEF(eglGetError)
GEN_FUNCPTR_DEF(eglGetDisplay)
GEN_FUNCPTR_DEF(eglInitialize)
GEN_FUNCPTR_DEF(eglTerminate)
//TODO
GEN_FUNCPTR_DEF(eglReleaseThread)
//TODO
GEN_FUNCPTR_DEF(eglGetProcAddress)

//Library
GLOBAL_FIELDS(EGLBoolean,EGL_TRUE)

EGLFunction functions[] = {
		EGLFUNCGROUP(eglGetError, &functions[1]),
		EGLFUNCGROUP(eglGetDisplay, &functions[2]),
		EGLFUNCGROUP_ERRORS(eglInitialize, &functions[3], EGLFUNC_ERROR(EGL_BAD_DISPLAY), EGLFUNC_ERROR(EGL_NOT_INITIALIZED)),
		EGLFUNCGROUP_ERRORS(eglTerminate, &functions[4], EGLFUNC_ERROR(EGL_BAD_DISPLAY)),
		//TODO
		EGLFUNCGROUP(eglReleaseThread, &functions[5]),
		//TODO
		EGLFUNCGROUP(eglGetProcAddress, &functions[6]),
		EGLFUNCGROUP_NOPTR(eglIsDebugLibraryInitialized, &functions[7]),
		EGLFUNCGROUP_NOPTR(eglDebugEnableLogging, &functions[8]),
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
	if(funcName && func && func->errors[0].value != NULL)
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
		EGL_DEBUG_FUNCCALL_FORMAT_RET_BOOL(eglInitialize, "%p, %p, %p", ret, SLOG2_FA_STAR(dpy), SLOG2_FA_STAR(major), SLOG2_FA_STAR(minor))
		EGL_CHECK_ERROR(eglInitialize);
		return ret;
	}
	EGL_DEBUG_MESSAGE(eglInitialize, "Init failed")
	return EGL_FALSE;
}

EGLBoolean eglTerminate(EGLDisplay dpy)
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglTerminate)(dpy);
		EGL_DEBUG_FUNCCALL_FORMAT_RET_BOOL(eglTerminate, "%p", ret, SLOG2_FA_STAR(dpy))
		EGL_CHECK_ERROR(eglInitialize);
		return ret;
	}
	EGL_DEBUG_MESSAGE(eglTerminate, "Init failed")
	return EGL_FALSE;
}

//TODO

EGLBoolean eglReleaseThread()
{
	if(eglInit())
	{
		EGLBoolean ret = GEN_FUNCPTR(eglReleaseThread)();
		const char* result = ret == EGL_FALSE ? "EGL_FALSE" : "EGL_TRUE";
		EGL_DEBUG_MESSAGE_FORMAT(eglReleaseThread, "%s", SLOG2_FA_STRING(result))
		eglCleanup();
		return ret;
	}
	EGL_DEBUG_MESSAGE(eglReleaseThread, "Never init")
	return EGL_FALSE;
}

//TODO

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

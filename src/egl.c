/*
 * egl.c
 *
 *  Created on: Dec 24, 2012
 *      Author: Vincent Simonetti
 */

#include <dlfcn.h>

#include <slog2.h>

#include <EGL.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

//Macros
#define STR_HELPER(x) #x
#define STR_TO_STR(x) x
#define NON_STR_TO_STR(x) STR_HELPER(x)

#define EGL_DEBUG_MESSAGE(func,message) if(loggingEnabled) {slog2c(NULL, 0, SLOG2_INFO, "EGL Debug-" NON_STR_TO_STR(func) "-" STR_TO_STR(message));}
#define EGL_DEBUG_MESSAGE_FORMAT(func,format,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = NON_STR_TO_STR(func);\
	slog2fa(NULL, 0, SLOG2_INFO, "EGL Debug-%s-" STR_TO_STR(format), SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__);}

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

#define EGLINIT_DEF_FIELD(x) x##Function x##Ptr = NULL;

#define EGLFUNC(x) x##Ptr

//Function definitions
typedef EGLint (*eglGetErrorFunction)(void);
//TODO
typedef EGLBoolean (*eglReleaseThreadFunction)(void);
//TODO
typedef __eglMustCastToProperFunctionPointerType (*eglGetProcAddressFunction)(const char*);

//Function ptrs
EGLINIT_DEF_FIELD(eglGetError)
//TODO
EGLINIT_DEF_FIELD(eglReleaseThread)
//TODO
EGLINIT_DEF_FIELD(eglGetProcAddress)

//Library
void* library = NULL;
EGLBoolean loggingEnabled = EGL_TRUE;

//Setup
int eglInit()
{
	if(library)
	{
		return TRUE;
	}
	slog2c(NULL, 0, SLOG2_INFO, "EGL Debug-Initializing");
	library = dlopen("libEGL.so", RTLD_LAZY);
	if(library)
	{
		EGLINIT_GETPTR(eglGetError)
		//TODO
		EGLINIT_GETPTR(eglReleaseThread)
		//TODO
		EGLINIT_GETPTR(eglGetProcAddress)
		slog2c(NULL, 0, SLOG2_INFO, "EGL Debug-Initialize complete");
		return TRUE;

	INIT_ERROR:
		slog2c(NULL, 0, SLOG2_ERROR, "EGL Debug-Initialize error");
		dlclose(library);
		library = NULL;
	}
	slog2c(NULL, 0, SLOG2_ERROR, "EGL Debug-Initialization failed");
	return FALSE;
}

void eglCleanup()
{
	if(library)
	{
		slog2c(NULL, 0, SLOG2_INFO, "EGL Debug-Cleaning up");
		dlclose(library);
		library = NULL;
	}
}

//Functions
EGLint eglGetError()
{
	EGLRUN_RET(EGLFUNC(eglGetError), (), EGL_NOT_INITIALIZED)
}

//TODO

EGLBoolean eglReleaseThread()
{
	if(eglInit())
	{
		EGLBoolean ret = EGLFUNC(eglReleaseThread)();
		const char* result = ret == EGL_FALSE ? "EGL_FALSE" : "EGL_TRUE";
		EGL_DEBUG_MESSAGE_FORMAT(eglReleaseThread, "%s", SLOG2_FA_STRING(result))
		eglCleanup();
		return ret;
	}
	return EGL_FALSE;
}

//TODO

__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char* procname)
{
	if(eglInit())
	{
		//TODO
	}
	EGL_DEBUG_MESSAGE(eglGetProcAddress,"Init failed")
	return NULL;
}

void eglDebugEnableLogging(EGLBoolean le)
{
	if(loggingEnabled)
	{
		const char* change = le == EGL_FALSE ? "EGL_FALSE" : "EGL_TRUE";
		EGL_DEBUG_MESSAGE_FORMAT(eglDebugEnableLogging, "Changing logging enabled to: %s", SLOG2_FA_STRING(change))
	}
	loggingEnabled = le;
}

/*
 * egl.c
 *
 *  Created on: Dec 24, 2012
 *      Author: Vincent Simonetti
 */

#include <dlfcn.h>

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

//Structure
typedef struct _eglFunc {
	const char* name;
	__eglMustCastToProperFunctionPointerType function;
	__eglMustCastToProperFunctionPointerType* functionPtr;

	struct _eglFunc* next;
} EGLFunction;

//Macros
#define STR_HELPER(x) #x
#define STR_TO_STR(x) x
#define NON_STR_TO_STR(x) STR_HELPER(x)

#define EGL_DEBUG_MESSAGE(func,message) if(loggingEnabled) {slog2c(debugBuffer, 0, SLOG2_INFO, "EGL Debug-" NON_STR_TO_STR(func) "-" STR_TO_STR(message));}
#define EGL_DEBUG_MESSAGE_FORMAT(func,format,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = NON_STR_TO_STR(func);\
	slog2fa(debugBuffer, 0, SLOG2_INFO, "EGL Debug-%s-" STR_TO_STR(format), SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_END);}

#define EGL_DEBUG_FUNCCALL(func) if(loggingEnabled) {slog2c(debugBuffer, 0, SLOG2_INFO, "EGL Debug-" NON_STR_TO_STR(func) "()");}
#define EGL_DEBUG_FUNCCALL_RET_STRING(func,ret) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = NON_STR_TO_STR(func);\
	const char* retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, "EGL Debug-%s() = %s", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), SLOG2_FA_STRING(retValue), SLOG2_FA_END);}
#define EGL_DEBUG_FUNCCALL_RET_SIGNED(func,ret) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = NON_STR_TO_STR(func);\
	int retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, "EGL Debug-%s() = %d", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), SLOG2_FA_SIGNED(retValue), SLOG2_FA_END);}

#define EGL_DEBUG_FUNCCALL_FORMAT(func,paramFormat,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = NON_STR_TO_STR(func);\
	slog2fa(debugBuffer, 0, SLOG2_INFO, "EGL Debug-%s(" STR_TO_STR(paramFormat) ")", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_END);}
#define EGL_DEBUG_FUNCCALL_FORMAT_RET_STRING(func,paramFormat,ret,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = NON_STR_TO_STR(func);\
	const char* retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, "EGL Debug-%s(" STR_TO_STR(paramFormat) ") = %s", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_STRING(retValue), SLOG2_FA_END);}
#define EGL_DEBUG_FUNCCALL_FORMAT_RET_SIGNED(func,paramFormat,ret,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = NON_STR_TO_STR(func);\
	int retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, "EGL Debug-%s(" STR_TO_STR(paramFormat) ") = %d", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_SIGNED(retValue), SLOG2_FA_END);}
#define EGL_DEBUG_FUNCCALL_FORMAT_RET_PTR(func,paramFormat,ret,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = NON_STR_TO_STR(func);\
	void* retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, "EGL Debug-%s(" STR_TO_STR(paramFormat) ") = %p", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_STAR(retValue), SLOG2_FA_END);}

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

#define EGLFUNCGROUP_NOPTR(x) NON_STR_TO_STR(x), (__eglMustCastToProperFunctionPointerType)(x), NULL
#define EGLFUNCGROUP(x) NON_STR_TO_STR(x), (__eglMustCastToProperFunctionPointerType)(x), (__eglMustCastToProperFunctionPointerType*)&EGLFUNC(x)

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
slog2_buffer_t debugBuffer = NULL;
EGLBoolean loggingEnabled = EGL_TRUE;

EGLFunction functions[] = {
		{EGLFUNCGROUP(eglGetError), &functions[1]},
		//TODO
		{EGLFUNCGROUP(eglReleaseThread), &functions[2]},
		//TODO
		{EGLFUNCGROUP(eglGetProcAddress), &functions[3]},
		{EGLFUNCGROUP_NOPTR(eglIsDebugLibraryInitialized), &functions[4]},
		{EGLFUNCGROUP_NOPTR(eglDebugEnableLogging), &functions[5]},
		{EGLFUNCGROUP_NOPTR(eglDebugLog), NULL}
};

//Setup
int eglInit()
{
	EGLFunction* func;
	if(library)
	{
		return TRUE;
	}
	slog2c(debugBuffer, 0, SLOG2_INFO, "EGL Debug-Initializing");
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
						slog2fa(debugBuffer, 0, SLOG2_INFO, "EGL Debug-%s-Failed to get ptr", SLOG2_FA_STRING(func->name), SLOG2_FA_END);
					}
					goto INIT_ERROR;
				}
			}
		}
		slog2c(debugBuffer, 0, SLOG2_INFO, "EGL Debug-Initialize complete");
		return TRUE;

	INIT_ERROR:
		slog2c(debugBuffer, 0, SLOG2_ERROR, "EGL Debug-Initialize error");
		dlclose(library);
		library = NULL;
	}
	slog2c(debugBuffer, 0, SLOG2_ERROR, "EGL Debug-Initialization failed");
	return FALSE;
}

void eglCleanup()
{
	if(library)
	{
		slog2c(debugBuffer, 0, SLOG2_INFO, "EGL Debug-Cleaning up");
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
		const char* change = le == EGL_FALSE ? "EGL_FALSE" : "EGL_TRUE";
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

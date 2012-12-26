/*
 * DebugMacros.h
 *
 *  Created on: Dec 25, 2012
 *      Author: Vincent Simonetti
 */

#ifndef DEBUGMACROS_H_
#define DEBUGMACROS_H_

//General defines
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

//General Macros
#define GEN_STR_HELPER(x) #x
#define GEN_STR_TO_STR(x) x
#define GEN_NON_STR_TO_STR(x) GEN_STR_HELPER(x)

//Utility Macros
#define GEN_BOOL_COMP_TO_STRING(compValue,trueValue,falseValue) ((compValue) == falseValue ? #falseValue : #trueValue)

//Debug Macros
#define GEN_DEBUG_MESSAGE(prefix,func,message) if(loggingEnabled) {slog2c(debugBuffer, 0, SLOG2_INFO, prefix "-" GEN_NON_STR_TO_STR(func) "-" GEN_STR_TO_STR(message));}
#define GEN_DEBUG_MESSAGE_FORMAT(prefix,func,format,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s-" GEN_STR_TO_STR(format), SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_END);}

#define GEN_DEBUG_FUNCCALL(prefix,func) if(loggingEnabled) {slog2c(debugBuffer, 0, SLOG2_INFO, prefix "-" GEN_NON_STR_TO_STR(func) "()");}
#define GEN_DEBUG_FUNCCALL_RET_STRING(prefix,func,ret) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	const char* retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s() = %s", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), SLOG2_FA_STRING(retValue), SLOG2_FA_END);}
#define GEN_DEBUG_FUNCCALL_RET_SIGNED(prefix,func,ret) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	int retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s() = %d", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), SLOG2_FA_SIGNED(retValue), SLOG2_FA_END);}
#define GEN_DEBUG_FUNCCALL_RET_PTR(prefix,func,ret) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	void* retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s() = %p", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), SLOG2_FA_STAR(retValue), SLOG2_FA_END);}
#define GEN_DEBUG_FUNCCALL_RET_BOOL(prefix,trueValue,falseValue,func,ret) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	const char* retValue = ret == falseValue ? #falseValue : #trueValue;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s() = %s", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), SLOG2_FA_STRING(retValue), SLOG2_FA_END);}

#define GEN_DEBUG_FUNCCALL_FORMAT(prefix,func,paramFormat,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s(" GEN_STR_TO_STR(paramFormat) ")", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_END);}
#define GEN_DEBUG_FUNCCALL_FORMAT_RET_STRING(prefix,func,paramFormat,ret,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	const char* retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s(" GEN_STR_TO_STR(paramFormat) ") = %s", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_STRING(retValue), SLOG2_FA_END);}
#define GEN_DEBUG_FUNCCALL_FORMAT_RET_SIGNED(prefix,func,paramFormat,ret,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	int retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s(" GEN_STR_TO_STR(paramFormat) ") = %d", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_SIGNED(retValue), SLOG2_FA_END);}
#define GEN_DEBUG_FUNCCALL_FORMAT_RET_PTR(prefix,func,paramFormat,ret,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	void* retValue = ret;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s(" GEN_STR_TO_STR(paramFormat) ") = %p", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_STAR(retValue), SLOG2_FA_END);}
#define GEN_DEBUG_FUNCCALL_FORMAT_RET_BOOL(prefix,trueValue,falseValue,func,paramFormat,ret,...) if(loggingEnabled) {const char* __eglDebugMessageFormatFunc__ = GEN_NON_STR_TO_STR(func);\
	const char* retValue = ret == falseValue ? #falseValue : #trueValue;\
	slog2fa(debugBuffer, 0, SLOG2_INFO, prefix "-%s(" GEN_STR_TO_STR(paramFormat) ") = %s", SLOG2_FA_STRING(__eglDebugMessageFormatFunc__), __VA_ARGS__, SLOG2_FA_STRING(retValue), SLOG2_FA_END);}

//Field macros
#define GLOBAL_FIELDS(logEnabledType,logEnabledInitalValue) void* library = NULL;\
	slog2_buffer_t debugBuffer = NULL;\
	logEnabledType loggingEnabled = (logEnabledInitalValue);

#define GEN_FUNCPTR(x) x##Ptr
#define GEN_FUNC(x) x##Function
#define GEN_FUNC_NORET_DEF(x,...) typedef void (*GEN_FUNC(x))(__VA_ARGS__);
#define GEN_FUNC_DEF(ret,x,...) typedef ret (*GEN_FUNC(x))(__VA_ARGS__);
#define GEN_FUNCPTR_DEF(x) GEN_FUNC(x) GEN_FUNCPTR(x) = NULL;
#define GEN_FUNCPTR_SPEC_DEF(t,x) t GEN_FUNCPTR(x) = NULL;

#define GEN_LIB_CLEANUP(x,msg) void x()\
{\
	if(library)\
	{\
		slog2c(debugBuffer, 0, SLOG2_INFO, msg "-Cleaning up");\
		dlclose(library);\
		library = NULL;\
	}\
}

#endif

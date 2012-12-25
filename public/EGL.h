/*
 * EGL.h
 *
 *  Created on: Dec 24, 2012
 *      Author: Vincent Simonetti
 */

#ifndef EGL_H_
#define EGL_H_

#include <EGL/egl.h>

#include <slog2.h>

#define EGL_DEBUG 1

/*
 * Check if the debug library is initialized or not
 */
EGLBoolean eglIsDebugLibraryInitialized();

/*
 * Enable or disable logging.
 */
void eglDebugEnableLogging(EGLBoolean loggingEnabled);

/*
 * The log to use, or NULL if the default log should be used
 */
void eglDebugLog(slog2_buffer_t buffer);

#endif

/*
 * EGL.h
 *
 *  Created on: Dec 24, 2012
 *      Author: Vincent Simonetti
 */

#ifndef EGL_H_
#define EGL_H_

#include <EGL/egl.h>

#define EGL_DEBUG 1

/*
 * Enable or disable logging.
 *
 * Logging is written to SLOG2's default buffer.
 */
void eglDebugEnableLogging(EGLBoolean loggingEnabled);

#endif

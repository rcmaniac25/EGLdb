EGLdb
=====

Simple interception library to trace EGL calls on BlackBerry 10

All debug information is written to slog2, with the prefix "EGL Debug"


Usage
=====
To use, change the library you reference from "-lEGL" to "-lEGLdb" and be sure to add the library as a asset in your descriptor file.

Depending on where you locate the file, you may have to change your library path so the linker is happy.


Additional functionality
=====
If you'd like, you can also reference the "EGL.h" file included. This simply adds a define for you to detect that the debug EGL is in use (EGL_DEBUG) and 3 helper functions:
* eglIsDebugLibraryInitialized - determine if the library is initialized
* eglDebugEnableLogging - enable/disable the logging capability
* eglDebugLog - specify the specific slog2 buffer to log information to. By default, the default slog2 buffer is used.


Hidden gem
=====
If used with a Cascades application, Cascades will use this library too. It doesn't so much, but you will know when it's init. and when it does it's swap buffers and other small calls.

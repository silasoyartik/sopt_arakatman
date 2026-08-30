|                     |                   |
| ------------------- | ----------------- |
| **Module Name**     | EGL10             |
| **Sub-Module Name** | BufferPosting     |
| **Target Function** | eglSwapBuffers    |
| **Safety Level**    | DAL A             |
| **Rationale**       | Not derived       |
| **Create Date**     | 30.08.2026        |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-BP-SB-001 | The graphics library shall provide API function (eglSwapBuffers) that posts the color buffer of an EGLSurface. | | EGLBoolean eglSwapBuffers ( EGLDisplay **dpy**, EGLSurface **surface** );<br/>_Input Parameters:_<br/>&emsp;**dpy:** Specifies the initialized EGL display connection.<br/>&emsp;**surface:** Specifies the current EGLSurface whose color buffer is to be posted. | EGL 1.0 Full Specification, 3.8.1 Posting to a Window, p. 27 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-002 | The eglSwapBuffers function shall copy the color buffer to the native window associated with _surface_ and return EGL_TRUE when _surface_ is a valid current window surface and the call succeeds. | The implementation may realize posting using a platform-specific mechanism. | | EGL 1.0 Full Specification, 3.8.1 Posting to a Window, p. 27 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-003 | The eglSwapBuffers function shall have no posting effect and shall return EGL_TRUE when _surface_ is a valid current pixmap or pbuffer surface and no error condition occurs. | | | EGL 1.0 Full Specification, 3.8.1 Posting to a Window, p. 27; 3.8.4 Posting Errors, p. 29 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-004 | The eglSwapBuffers function shall leave the color buffer of _surface_ in an undefined state after a successful call. | A client shall not rely on preservation of previous color-buffer contents. | | EGL 1.0 Full Specification, 3.8.1 Posting to a Window, p. 27 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-005 | The eglSwapBuffers function shall ensure that a window surface is resized to match its native window before copying pixels when the native window was resized before the call. | This requirement applies when resizing was not already performed transparently by the implementation. | | EGL 1.0 Full Specification, 3.8.1 Posting to a Window, pp. 27-28 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-006 | The eglSwapBuffers function shall leave newly allocated buffer contents undefined when a window surface grows and shall discard pixels outside the new extent when the surface shrinks. | The client remains responsible for updating viewport and scissor regions. | | EGL 1.0 Full Specification, 3.8.1 Posting to a Window, p. 28 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-007 | The eglSwapBuffers function shall require _surface_ to be bound to the calling thread's current context. | This is an EGL 1.0 posting restriction. | | EGL 1.0 Full Specification, 3.8.3 Posting Semantics, p. 28 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-008 | The eglSwapBuffers function shall perform an implicit glFlush when _dpy_ and _surface_ are the display and surface for the calling thread's current context. | Subsequent OpenGL ES commands may be issued immediately but need not execute until posting completes. | | EGL 1.0 Full Specification, 3.8.3 Posting Semantics, p. 28 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-009 | The eglSwapBuffers function shall return EGL_FALSE and generate EGL_BAD_SURFACE when _surface_ does not name a valid EGLSurface. | | | EGL 1.0 Full Specification, 3.8.4 Posting Errors, p. 29 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-010 | The eglSwapBuffers function shall return EGL_FALSE and generate EGL_BAD_SURFACE when _surface_ is not bound to the calling thread's current context. | | | EGL 1.0 Full Specification, 3.8.4 Posting Errors, p. 29 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-011 | The eglSwapBuffers function shall return EGL_FALSE and generate EGL_BAD_NATIVE_WINDOW when the native window associated with _surface_ is no longer valid and the invalid native handle can be detected. | | | EGL 1.0 Full Specification, 3.8.4 Posting Errors, p. 29 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-012 | The eglSwapBuffers function shall return EGL_FALSE and generate EGL_NOT_INITIALIZED when EGL is not initialized on _dpy_. | | | EGL 1.0 Full Specification, 3.1 Errors, pp. 8-9 | No | Req-EGL-001 | Test |
| GS-EGL10-BP-SB-013 | The eglSwapBuffers function shall return EGL_FALSE and generate EGL_BAD_DISPLAY when _dpy_ does not name a valid EGLDisplay. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |

|                     |                   |
| ------------------- | ----------------- |
| **Module Name**     | EGL10             |
| **Sub-Module Name** | RenderingSurfaces |
| **Target Function** | eglDestroySurface |
| **Safety Level**    | DAL A             |
| **Rationale**       | Not derived       |
| **Create Date**     | 30.08.2026        |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-RS-DS-001 | The graphics library shall provide API function (eglDestroySurface) that destroys an EGLSurface. | | EGLBoolean eglDestroySurface ( EGLDisplay **dpy**, EGLSurface **surface** );<br/>_Input Parameters:_<br/>&emsp;**dpy:** Specifies the initialized EGL display connection.<br/>&emsp;**surface:** Specifies the window, pbuffer, or pixmap surface to destroy. | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-002 | The eglDestroySurface function shall mark all resources associated with _surface_ for deletion as soon as possible and return EGL_TRUE when the call succeeds. | This behavior applies to window, pbuffer, and pixmap surfaces. | | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-003 | The eglDestroySurface function shall defer the actual release of _surface_ while _surface_ is current to any thread. | | | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-004 | The graphics library shall keep future references to a surface marked for deletion valid only while that surface remains current. | | | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-005 | The graphics library shall destroy a surface marked for deletion and invalidate all future references to it when the thread to which it is bound makes any otherwise valid eglMakeCurrent call. | | | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-006 | The eglDestroySurface function shall return EGL_FALSE and generate EGL_BAD_SURFACE when _surface_ is not a valid rendering surface. | | | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-007 | The eglDestroySurface function shall return EGL_FALSE and generate EGL_NOT_INITIALIZED when EGL is not initialized on _dpy_. | | | EGL 1.0 Full Specification, 3.1 Errors, pp. 8-9 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-008 | The eglDestroySurface function shall return EGL_FALSE and generate EGL_BAD_DISPLAY when _dpy_ does not name a valid EGLDisplay. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |

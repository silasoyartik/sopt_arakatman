|                     |                   |
| ------------------- | ----------------- |
| **Module Name**     | EGL10             |
| **Sub-Module Name** | RenderingSurfaces |
| **Target Function** | `eglDestroySurface` |
| **Safety Level**    | DAL A             |
| **Rationale**       | Not derived       |
| **Create Date**     | 30.08.2026        |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-RS-DS-001 | The graphics library shall provide the `eglDestroySurface` API function that requests the destruction of an EGLSurface. | | `EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface);`<br/>_Input Parameters:_<br/>&emsp;`dpy`: Specifies the initialized EGL display connection.<br/>&emsp;`surface`: Specifies the window, pbuffer, or pixmap surface to destroy. | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-002 | When `dpy` names an initialized EGLDisplay and `surface` names a valid EGLSurface, the `eglDestroySurface` function shall return `EGL_TRUE` and cause `surface` to become invalid when `surface` is no longer current to any thread. | This behavior applies to window, pbuffer, and pixmap surfaces. Resources associated with `surface` are released when they are no longer needed. | | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-003 | The `eglDestroySurface` function shall defer the destruction of `surface` while `surface` is current to any thread. | | | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-004 | The graphics library shall treat references to a surface marked for deletion as valid only while that surface remains current. | | | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-005 | The `eglDestroySurface` function shall return `EGL_FALSE` and generate `EGL_BAD_SURFACE` when `surface` is not a valid rendering surface. | | | EGL 1.0 Full Specification, 3.5.4 Destroying Rendering Surfaces, p. 22 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-006 | The `eglDestroySurface` function shall return `EGL_FALSE` and generate `EGL_NOT_INITIALIZED` when EGL is not initialized on `dpy`. | | | EGL 1.0 Full Specification, 3.1 Errors, pp. 8-9 | No | Req-EGL-001 | Test |
| GS-EGL10-RS-DS-007 | The `eglDestroySurface` function shall return `EGL_FALSE` and generate `EGL_BAD_DISPLAY` when `dpy` does not name a valid EGLDisplay. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |

|  |  |
| --- | --- |
| Module Name | EGL10 |
| Sub-Module Name | Initialization |
| Target Function | eglTerminate |
| Safety Level | DAL A |
| Rationale | Not derived |
| Create Date | 31.08.2026 |

---

| Requirement ID | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GS-EGL10-IN-TER-001 | The graphics library shall provide API function `eglTerminate` that releases EGL and OpenGL ES resources associated with a display. |  | `EGLBoolean eglTerminate(EGLDisplay dpy);`<br><br>Input Parameters:<br>`dpy`: Specifies the EGL display connection to terminate. | EGL 1.0 Full Specification, 3.2 Initialization, pp. 10-11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-002 | The `eglTerminate` function shall mark all EGL-specific resources associated with `dpy` for deletion. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-003 | The `eglTerminate` function shall defer the actual release of a context or surface that is current to any thread while that object remains current. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-004 | The graphics library shall destroy a context or surface marked for deletion by `eglTerminate` when the thread to which it is bound makes any otherwise valid `eglMakeCurrent` call. | All future references to the destroyed object shall then be invalid. |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-005 | The `eglTerminate` function shall return `EGL_TRUE` when termination succeeds. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-006 | The `eglTerminate` function shall return `EGL_FALSE` and generate `EGL_BAD_DISPLAY` when `dpy` does not refer to a valid `EGLDisplay`. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-007 | The `eglTerminate` function shall return `EGL_TRUE` without releasing resources when `dpy` is valid but has already been terminated or has not yet been initialized. | No EGL resources are associated with the display in this case. |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-008 | The graphics library shall allow a terminated display to be reinitialized by `eglInitialize`. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-009 | The graphics library shall keep resources that were marked for deletion by an earlier termination marked for deletion when the display is reinitialized. | References to those resources shall remain invalid. |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |

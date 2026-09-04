| Field | Value |
| --- | --- |
| Module Name | EGL10 |
| Sub-Module Name | Initialization |
| Target Function | `eglTerminate` |
| Safety Level | DAL A |
| Rationale | Not derived |
| Create Date | 31.08.2026 |

| Requirement ID | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GS-EGL10-IN-TER-001 | The graphics library shall provide API function `eglTerminate` that terminates EGL use on a specified display. |  | `EGLBoolean eglTerminate(EGLDisplay dpy);`<br><br>_Input Parameter:_<br>`dpy`: Specifies the EGL display connection to terminate.<br><br>_Return Value:_<br>Returns `EGL_TRUE` on success and `EGL_FALSE` on failure. | EGL 1.0 Full Specification, 3.2 Initialization, pp. 10-11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-002 | When `dpy` is a valid initialized `EGLDisplay`, `eglTerminate` shall return `EGL_TRUE` and EGL shall treat that display as uninitialized until the display is successfully initialized again. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-003 | If `dpy` does not name a valid `EGLDisplay`, `eglTerminate` shall return `EGL_FALSE` and set the calling thread's EGL error state to `EGL_BAD_DISPLAY`. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-004 | If `dpy` names a valid `EGLDisplay` on which EGL is not initialized, `eglTerminate` shall return `EGL_TRUE`. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-005 | After a successful `eglTerminate(dpy)` call, EGL shall allow the same `dpy` to be initialized again; prior termination alone shall not make `dpy` invalid. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-006 | If a context or surface associated with `dpy` is current to a thread when `eglTerminate(dpy)` succeeds, EGL shall defer releasing that resource until the bound thread performs an otherwise valid operation that changes its current context or surface. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |

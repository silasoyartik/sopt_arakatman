| Field | Value |
| --- | --- |
| Module Name | EGL10 |
| Sub-Module Name | Initialization |
| Target Function | _eglTerminate_ |
| Safety Level | DAL A |
| Rationale | Not derived |
| Create Date | 31.08.2026 |

| Requirement ID | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GS-EGL10-IN-TER-001 | The graphics library shall provide API function _eglTerminate_ that terminates EGL use on a specified display. |  | _EGLBoolean eglTerminate(EGLDisplay dpy);_<br><br>_Input Parameter:_<br>**dpy:** Specifies the EGL display connection to terminate.<br><br>_Return Value:_<br>Returns _EGL_TRUE_ on success and _EGL_FALSE_ on failure. | EGL 1.0 Full Specification, 3.2 Initialization, pp. 10-11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-002 | When _dpy_ is a valid initialized _EGLDisplay_, _eglTerminate_ shall return _EGL_TRUE_ and EGL shall treat that display as uninitialized until the display is successfully initialized again. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-003 | If _dpy_ does not name a valid _EGLDisplay_, _eglTerminate_ shall return _EGL_FALSE_ and set the calling thread's EGL error state to _EGL_BAD_DISPLAY_. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-004 | If _dpy_ names a valid _EGLDisplay_ on which EGL is not initialized, _eglTerminate_ shall return _EGL_TRUE_. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-005 | After a successful _eglTerminate(dpy)_ call, EGL shall allow the same _dpy_ to be initialized again; prior termination alone shall not make _dpy_ invalid. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-006 | If a context or surface associated with _dpy_ is current to a thread when _eglTerminate(dpy)_ succeeds, EGL shall defer releasing that resource until the bound thread performs an otherwise valid operation that changes its current context or surface. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |

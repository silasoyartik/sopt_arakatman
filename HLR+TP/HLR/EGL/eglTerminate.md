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
| GS-EGL10-IN-TER-002 | When _dpy_ is a valid initialized _EGLDisplay_, _eglTerminate_ shall return _EGL_TRUE_. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-003 | After _eglTerminate(dpy)_ returns _EGL_TRUE_ for an initialized display, EGL shall treat that display as uninitialized until a subsequent successful _eglInitialize(dpy, ...)_ call. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-004 | If _dpy_ does not name a valid _EGLDisplay_, _eglTerminate_ shall return _EGL_FALSE_ and make _eglGetError_ return _EGL_BAD_DISPLAY_. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-005 | If _dpy_ names a valid _EGLDisplay_ on which EGL is not initialized, _eglTerminate_ shall return _EGL_TRUE_. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-006 | After a successful _eglTerminate(dpy)_ call, EGL shall allow a subsequent _eglInitialize(dpy, ...)_ call to initialize the same _dpy_ again; prior termination alone shall not make _dpy_ invalid. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-TER-007 | If a context or surface associated with _dpy_ is current to a thread when _eglTerminate(dpy)_ succeeds, EGL shall defer releasing that resource until an otherwise valid _eglMakeCurrent_ call is made by the thread to which the resource is bound. |  |  | EGL 1.0 Full Specification, 3.2 Initialization, p. 11 | No | Req-EGL-001 | Test |

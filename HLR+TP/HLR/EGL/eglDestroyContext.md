|                     |                   |
| ------------------- | ----------------- |
| **Module Name**     | EGL10             |
| **Sub-Module Name** | RenderingContexts |
| **Target Function** | eglDestroyContext |
| **Safety Level**    | DAL A             |
| **Rationale**       | Not derived       |
| **Create Date**     | 30.08.2026        |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-RC-DC-001 | The graphics library shall provide API function (eglDestroyContext) that destroys an EGL rendering context. | | EGLBoolean eglDestroyContext ( EGLDisplay **dpy**, EGLContext **ctx** );<br/>_Input Parameters:_<br/>&emsp;**dpy:** Specifies the initialized EGL display connection.<br/>&emsp;**ctx:** Specifies the EGL rendering context to destroy. | EGL 1.0 Full Specification, 3.6.2 Destroying Rendering Contexts, p. 24 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-DC-002 | The eglDestroyContext function shall mark all resources associated with _ctx_ for deletion as soon as possible and return EGL_TRUE when the call succeeds. | | | EGL 1.0 Full Specification, 3.6.2 Destroying Rendering Contexts, p. 24 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-DC-003 | The eglDestroyContext function shall defer the actual release of _ctx_ while _ctx_ is current to any thread. | | | EGL 1.0 Full Specification, 3.6.2 Destroying Rendering Contexts, p. 24 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-DC-004 | The graphics library shall keep future references to a context marked for deletion valid only while that context remains current. | | | EGL 1.0 Full Specification, 3.6.2 Destroying Rendering Contexts, p. 24 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-DC-005 | The graphics library shall destroy a context marked for deletion and invalidate all future references to it when the thread to which it is bound makes any otherwise valid eglMakeCurrent call. | | | EGL 1.0 Full Specification, 3.6.2 Destroying Rendering Contexts, p. 24 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-DC-006 | The eglDestroyContext function shall return EGL_FALSE and generate EGL_BAD_CONTEXT when _ctx_ is not a valid rendering context. | | | EGL 1.0 Full Specification, 3.6.2 Destroying Rendering Contexts, p. 24 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-DC-007 | The eglDestroyContext function shall return EGL_FALSE and generate EGL_NOT_INITIALIZED when EGL is not initialized on _dpy_. | | | EGL 1.0 Full Specification, 3.1 Errors, pp. 8-9 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-DC-008 | The eglDestroyContext function shall return EGL_FALSE and generate EGL_BAD_DISPLAY when _dpy_ does not name a valid EGLDisplay. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |

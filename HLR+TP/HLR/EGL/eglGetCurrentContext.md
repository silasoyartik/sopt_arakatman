| Field | Value |
| --- | --- |
| Module Name | EGL10 |
| Sub-Module Name | RenderingContexts |
| Target Function | _eglGetCurrentContext_ |
| Safety Level | DAL A |
| Rationale | Not derived |
| Create Date | 31.08.2026 |

| Requirement ID | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GS-EGL10-RC-GCC-001 | The graphics library shall provide _eglGetCurrentContext_ with the EGL 1.0 function interface. |  | _EGLContext eglGetCurrentContext(void);_<br><br>The function has no input parameters. It returns the context current in the calling thread, or _EGL_NO_CONTEXT_ when the calling thread has no current context. | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 26 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCC-002 | After a successful _eglMakeCurrent_ call binds an _EGLContext_ to the calling thread, _eglGetCurrentContext_ shall return that same _EGLContext_ handle. |  |  | EGL 1.0 Full Specification, 2.2.1 Using Rendering Contexts, p. 4; 3.6.3 Binding Contexts and Drawables, pp. 25-26 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCC-003 | When the calling thread has no current _EGLContext_, _eglGetCurrentContext_ shall return _EGL_NO_CONTEXT_. |  |  | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 26 | No | Req-EGL-001 | Test |

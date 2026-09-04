| Field | Value |
| --- | --- |
| Module Name | EGL10 |
| Sub-Module Name | RenderingContexts |
| Target Function | `eglGetCurrentContext` |
| Safety Level | DAL A |
| Rationale | Not derived |
| Create Date | 31.08.2026 |

| Requirement ID | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GS-EGL10-RC-GCC-001 | The graphics library shall provide API function `eglGetCurrentContext(void)`. |  | `EGLContext eglGetCurrentContext(void);`<br><br>_Input Parameters:_<br>None.<br><br>_Return Value:_<br>An `EGLContext` handle. | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 26 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCC-002 | After an `EGLContext` is successfully bound to the calling thread, `eglGetCurrentContext` shall return that same `EGLContext` handle. |  |  | EGL 1.0 Full Specification, 2.2.1 Using Rendering Contexts, p. 4; 3.6.3 Binding Contexts and Drawables, pp. 25-26 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCC-003 | When the calling thread has no current `EGLContext`, `eglGetCurrentContext` shall return `EGL_NO_CONTEXT` and shall not generate an EGL error. |  |  | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 26 | No | Req-EGL-001 | Test |

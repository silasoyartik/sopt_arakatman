|                           |                      |
| ------------------------- | -------------------- |
| **Module Name**     | EGL10                |
| **Sub-Module Name** | RenderingContexts    |
| **Target Function** | eglGetCurrentContext |
| **Safety Level**    | DAL A                |
| **Rationale**       | Not derived          |
| **Create Date**     | 31.08.2026           |

---

| Requirement ID      | Requirement                                                                                                                                                                                              | Information                                                                                                                | Definitions                                                                                            | Reference                                                                                                     | Impl.-Dep. | Source      | Verification Method |
| ------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------- | ---------- | ----------- | ------------------- |
| GS-EGL10-RC-GCC-001 | The graphics library shall provide the `eglGetCurrentContext` API with the interface defined by EGL 1.0. | This requirement is verified by compiling and linking a test that calls `eglGetCurrentContext`. | `EGLContext eglGetCurrentContext(void);`<br><br>Input parameters: none.<br><br>Return value: returns the current `EGLContext` of the calling thread, or `EGL_NO_CONTEXT` when no context is current. | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 26 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCC-002 | The `eglGetCurrentContext` function shall return the `EGLContext` bound to the calling thread when a context has been made current by a successful `eglMakeCurrent` call. | This requirement is verified by making a context current and comparing the value returned by `eglGetCurrentContext` with the bound context. | Current context: the context made current by a successful `eglMakeCurrent` call for the calling thread. | EGL 1.0 Full Specification, 2.2.1 Using Rendering Contexts, p. 4; 3.6.3 Binding Contexts and Drawables, pp. 25-26 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCC-003 | The `eglGetCurrentContext` function shall return `EGL_NO_CONTEXT` when no `EGLContext` is current on the calling thread. | This requirement is verified by detaching the current context using `eglMakeCurrent` with `EGL_NO_CONTEXT` and checking that `eglGetCurrentContext` returns `EGL_NO_CONTEXT`. | `EGL_NO_CONTEXT` | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 26 | No | Req-EGL-001 | Test |

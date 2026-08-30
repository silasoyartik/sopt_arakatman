|                     |                   |
| ------------------- | ----------------- |
| **Module Name**     | EGL10             |
| **Sub-Module Name** | RenderingContexts |
| **Target Function** | eglGetCurrentContext |
| **Safety Level**    | DAL A             |
| **Rationale**       | Not derived       |
| **Create Date**     | 30.08.2026        |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-RC-GCC-001 | The graphics library shall provide API function (eglGetCurrentContext) that obtains the calling thread's current EGL rendering context. | | EGLContext eglGetCurrentContext ( void ); | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 25 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCC-002 | The eglGetCurrentContext function shall return the EGLContext bound to the calling thread when that thread has a current context. | Current-context state is thread-specific. | | EGL 1.0 Full Specification, 2.2.1 Using Rendering Contexts, p. 4; 3.6.3, p. 25 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCC-003 | The eglGetCurrentContext function shall return EGL_NO_CONTEXT without generating an error when the calling thread has no current context. | | | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 25 | No | Req-EGL-001 | Test |

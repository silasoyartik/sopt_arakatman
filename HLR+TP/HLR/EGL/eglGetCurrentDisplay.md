|                     |                   |
| ------------------- | ----------------- |
| **Module Name**     | EGL10             |
| **Sub-Module Name** | RenderingContexts |
| **Target Function** | eglGetCurrentDisplay |
| **Safety Level**    | DAL A             |
| **Rationale**       | Not derived       |
| **Create Date**     | 30.08.2026        |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-RC-GCD-001 | The graphics library shall provide API function (eglGetCurrentDisplay) that obtains the display associated with the calling thread's current context. | | EGLDisplay eglGetCurrentDisplay ( void ); | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 26 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCD-002 | The eglGetCurrentDisplay function shall return the EGLDisplay associated with the calling thread's current context when that thread has a current context. | Current-context state is thread-specific. | | EGL 1.0 Full Specification, 2.2.1 Using Rendering Contexts, p. 4; 3.6.3, p. 26 | No | Req-EGL-001 | Test |
| GS-EGL10-RC-GCD-003 | The eglGetCurrentDisplay function shall return EGL_NO_DISPLAY without generating an error when the calling thread has no current context. | | | EGL 1.0 Full Specification, 3.6.3 Binding Contexts and Drawables, p. 26 | No | Req-EGL-001 | Test |

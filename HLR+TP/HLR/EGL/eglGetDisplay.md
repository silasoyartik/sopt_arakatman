
| **Module Name**     | EGL10          |
| ------------------------- | -------------- |
| **Sub-Module Name** | Initialization |
| **Target Function** | eglGetDisplay  |
| **Safety Level**    | DAL A          |
| **Rationale**       | Not derived    |
| **Create Date**     | 30.08.2026     |

---

| Requirement ID     | Requirement                                                                                                                                               | Information                                                       | Definitions                                                                                                                                                        | Reference                                             | Impl.-Dep. | Source      | Verification Method |
| :----------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------- | :---------------------------------------------------------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------- | :---------------------------------------------------- | :--------- | :---------- | :------------------ |
| GS-EGL10-IN-GD-001 | The graphics library shall provide API function (eglGetDisplay) that obtains an EGL display associated with a native display.                             |                                                                   | EGLDisplay eglGetDisplay ( NativeDisplayType**display_id** );*Input Parameters:* **display_id:** Specifies a native display or EGL_DEFAULT_DISPLAY. | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No         | Req-EGL-001 | Test                |
| GS-EGL10-IN-GD-002 | The eglGetDisplay function shall return an EGLDisplay associated with the native display specified by*display_id* when a matching display is available. | The type and format of*display_id* are implementation-specific. |                                                                                                                                                                    | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No         | Req-EGL-001 | Test                |
| GS-EGL10-IN-GD-003 | The eglGetDisplay function shall return the default EGL display when*display_id* is EGL_DEFAULT_DISPLAY and a default display is available.             |                                                                   |                                                                                                                                                                    | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No         | Req-EGL-001 | Test                |
| GS-EGL10-IN-GD-004 | The eglGetDisplay function shall return EGL_NO_DISPLAY when no display matching*display_id* is available.                                               | This result shall not raise an EGL error condition.               |                                                                                                                                                                    | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No         | Req-EGL-001 | Test                |

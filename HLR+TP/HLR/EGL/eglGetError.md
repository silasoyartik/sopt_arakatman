|                     |               |
| ------------------- | ------------- |
| **Module Name**     | EGL10         |
| **Sub-Module Name** | Errors        |
| **Target Function** | eglGetError   |
| **Safety Level**    | DAL A         |
| **Rationale**       | Not derived   |
| **Create Date**     | 30.08.2026    |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-ER-GE-001 | The graphics library shall provide API function (eglGetError) that returns error information for the most recent EGL function called by the calling thread. | | EGLint eglGetError ( void ); | EGL 1.0 Full Specification, 3.1 Errors, p. 8 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-002 | The eglGetError function shall return EGL_SUCCESS when the relevant EGL function succeeded. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 8 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-003 | The eglGetError function shall return EGL_NOT_INITIALIZED when EGL is not initialized, or could not be initialized, for the specified display. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 8 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-004 | The eglGetError function shall return EGL_BAD_ACCESS when EGL cannot access a requested resource. | A context bound in another thread is an example of an inaccessible resource. | | EGL 1.0 Full Specification, 3.1 Errors, p. 8 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-005 | The eglGetError function shall return EGL_BAD_ALLOC when EGL failed to allocate resources for the requested operation. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 8 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-006 | The eglGetError function shall return EGL_BAD_ATTRIBUTE when an unrecognized attribute or attribute value was passed in an attribute list. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-007 | The eglGetError function shall return EGL_BAD_CONTEXT when an EGLContext argument does not name a valid EGLContext. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-008 | The eglGetError function shall return EGL_BAD_CONFIG when an EGLConfig argument does not name a valid EGLConfig. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-009 | The eglGetError function shall return EGL_BAD_CURRENT_SURFACE when the current surface of the calling thread is no longer valid. | The surface may be a window, pbuffer, or pixmap. | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-010 | The eglGetError function shall return EGL_BAD_DISPLAY when an EGLDisplay argument does not name a valid EGLDisplay or EGL is not initialized on the specified EGLDisplay. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-011 | The eglGetError function shall return EGL_BAD_SURFACE when an EGLSurface argument does not name a valid surface configured for OpenGL ES rendering. | The surface may be a window, pbuffer, or pixmap. | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-012 | The eglGetError function shall return EGL_BAD_MATCH when function arguments are inconsistent. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-013 | The eglGetError function shall return EGL_BAD_PARAMETER when one or more argument values are invalid. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-014 | The eglGetError function shall return EGL_BAD_NATIVE_PIXMAP when a NativePixmapType argument does not refer to a valid native pixmap and the invalid native handle can be detected. | Detection of invalid native objects may not always be possible. | | EGL 1.0 Full Specification, 3.1 Errors, pp. 9-10 | No | Req-EGL-001 | Test |
| GS-EGL10-ER-GE-015 | The eglGetError function shall return EGL_BAD_NATIVE_WINDOW when a NativeWindowType argument does not refer to a valid native window and the invalid native handle can be detected. | Detection of invalid native objects may not always be possible. | | EGL 1.0 Full Specification, 3.1 Errors, pp. 9-10 | No | Req-EGL-001 | Test |

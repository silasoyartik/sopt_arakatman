|                     |               |
| ------------------- | ------------- |
| **Module Name**     | EGL10         |
| **Sub-Module Name** | Initialization |
| **Target Function** | eglInitialize |
| **Safety Level**    | DAL A         |
| **Rationale**       | Not derived   |
| **Create Date**     | 30.08.2026    |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-IN-INI-001 | The graphics library shall provide API function (eglInitialize) that initializes EGL for a specified display. | | EGLBoolean eglInitialize ( EGLDisplay **dpy**, EGLint * **major**, EGLint * **minor** );<br/>_Input Parameters:_<br/>&emsp;**dpy:** Specifies the EGL display connection.<br/><br/>_Output Parameters:_<br/>&emsp;**major:** Returns the major EGL version number when not NULL.<br/>&emsp;**minor:** Returns the minor EGL version number when not NULL. | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-INI-002 | The eglInitialize function shall initialize EGL for _dpy_ and return EGL_TRUE when initialization succeeds. | Initialization is required once for each display before most other EGL functions are called. | | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-INI-003 | The eglInitialize function shall write the EGL implementation major version number to _major_ and the minor version number to _minor_ on success when the corresponding output pointer is not NULL. | | | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-INI-004 | The eglInitialize function shall not update _major_ when _major_ is NULL and shall not update _minor_ when _minor_ is NULL. | Either output pointer may independently be NULL. | | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-INI-005 | The eglInitialize function shall return EGL_FALSE and shall not update _major_ or _minor_ when initialization fails. | | | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-INI-006 | The eglInitialize function shall generate EGL_BAD_DISPLAY when _dpy_ does not refer to a valid EGLDisplay. | | | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-INI-007 | The eglInitialize function shall generate EGL_NOT_INITIALIZED when EGL cannot be initialized for an otherwise valid _dpy_. | | | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-INI-008 | The eglInitialize function shall return EGL_TRUE and update each non-NULL version output when _dpy_ is already initialized. | No other initialization side effect is required for an already-initialized display. | | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No | Req-EGL-001 | Test |
| GS-EGL10-IN-INI-009 | The graphics library shall allow an initialized EGLDisplay to be used by other threads in the same address space without initializing that display again in those threads. | | | EGL 1.0 Full Specification, 3.2 Initialization, p. 10 | No | Req-EGL-001 | Test |

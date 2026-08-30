|                     |                         |
| ------------------- | ----------------------- |
| **Module Name**     | EGL10                   |
| **Sub-Module Name** | ConfigurationManagement |
| **Target Function** | eglGetConfigAttrib      |
| **Safety Level**    | DAL A                   |
| **Rationale**       | Not derived             |
| **Create Date**     | 30.08.2026              |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-CM-GCA-001 | The graphics library shall provide API function (eglGetConfigAttrib) that obtains an attribute value from an EGLConfig. | | EGLBoolean eglGetConfigAttrib ( EGLDisplay **dpy**, EGLConfig **config**, EGLint **attribute**, EGLint * **value** );<br/>_Input Parameters:_<br/>&emsp;**dpy:** Specifies the initialized EGL display connection.<br/>&emsp;**config:** Specifies the EGLConfig to query.<br/>&emsp;**attribute:** Specifies the attribute to query.<br/><br/>_Output Parameters:_<br/>&emsp;**value:** Returns the requested attribute value. | EGL 1.0 Full Specification, 3.4.3 Querying Configuration Attributes, p. 19 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCA-002 | The eglGetConfigAttrib function shall return EGL_TRUE and write the value of _attribute_ for _config_ to the memory referenced by _value_ when the query succeeds. | | | EGL 1.0 Full Specification, 3.4.3 Querying Configuration Attributes, p. 19 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCA-003 | The eglGetConfigAttrib function shall support querying every EGL 1.0 EGLConfig attribute defined in Table 3.1. | Valid attributes are EGL_BUFFER_SIZE, EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE, EGL_ALPHA_SIZE, EGL_CONFIG_CAVEAT, EGL_CONFIG_ID, EGL_DEPTH_SIZE, EGL_LEVEL, EGL_MAX_PBUFFER_WIDTH, EGL_MAX_PBUFFER_HEIGHT, EGL_MAX_PBUFFER_PIXELS, EGL_NATIVE_RENDERABLE, EGL_NATIVE_VISUAL_ID, EGL_NATIVE_VISUAL_TYPE, EGL_SAMPLE_BUFFERS, EGL_SAMPLES, EGL_STENCIL_SIZE, EGL_SURFACE_TYPE, EGL_TRANSPARENT_TYPE, EGL_TRANSPARENT_RED_VALUE, EGL_TRANSPARENT_GREEN_VALUE, and EGL_TRANSPARENT_BLUE_VALUE. | | EGL 1.0 Full Specification, 3.4 Configuration Management, Table 3.1, p. 13 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCA-004 | The eglGetConfigAttrib function shall return a bitmask composed of supported surface-type bits when _attribute_ is EGL_SURFACE_TYPE. | Valid EGL 1.0 bits are EGL_WINDOW_BIT, EGL_PIXMAP_BIT, and EGL_PBUFFER_BIT. | | EGL 1.0 Full Specification, 3.4 Configuration Management, Table 3.2, p. 13 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCA-005 | The eglGetConfigAttrib function shall return zero for EGL_NATIVE_VISUAL_ID and EGL_NONE for EGL_NATIVE_VISUAL_TYPE when _config_ does not support windows or has no associated native visual type. | | | EGL 1.0 Full Specification, 3.4 Configuration Management, p. 14 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCA-006 | The eglGetConfigAttrib function shall return EGL_FALSE and generate EGL_BAD_ATTRIBUTE when _attribute_ is not a valid EGLConfig attribute. | | | EGL 1.0 Full Specification, 3.4.3 Querying Configuration Attributes, p. 19 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCA-007 | The eglGetConfigAttrib function shall return EGL_FALSE and generate EGL_BAD_CONFIG when _config_ does not name a valid EGLConfig for _dpy_. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCA-008 | The eglGetConfigAttrib function shall return EGL_FALSE and generate EGL_NOT_INITIALIZED when EGL is not initialized on _dpy_. | | | EGL 1.0 Full Specification, 3.1 Errors, pp. 8-9 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCA-009 | The eglGetConfigAttrib function shall return EGL_FALSE and generate EGL_BAD_DISPLAY when _dpy_ does not name a valid EGLDisplay. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |

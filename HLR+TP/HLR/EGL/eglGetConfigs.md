|                     |                         |
| ------------------- | ----------------------- |
| **Module Name**     | EGL10                   |
| **Sub-Module Name** | ConfigurationManagement |
| **Target Function** | eglGetConfigs           |
| **Safety Level**    | DAL A                   |
| **Rationale**       | Not derived             |
| **Create Date**     | 30.08.2026              |
---

| Requirement ID       | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| :------------------- | :---------- | :---------- | :---------- | :-------- | :--------- | :----- | :------------------ |
| GS-EGL10-CM-GCS-001 | The graphics library shall provide API function (eglGetConfigs) that obtains the EGLConfigs available on a specified display. | | EGLBoolean eglGetConfigs ( EGLDisplay **dpy**, EGLConfig * **configs**, EGLint **config_size**, EGLint * **num_config** );<br/>_Input Parameters:_<br/>&emsp;**dpy:** Specifies the initialized EGL display connection.<br/>&emsp;**config_size:** Specifies the capacity of the configs array.<br/><br/>_Output Parameters:_<br/>&emsp;**configs:** Returns EGLConfig handles when not NULL.<br/>&emsp;**num_config:** Returns the number of configurations. | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-002 | The eglGetConfigs function shall return EGL_TRUE and write the number of returned configurations to _num_config_ when the call succeeds and _configs_ is not NULL. | | | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-003 | The eglGetConfigs function shall write valid EGLConfig handles into elements zero through _num_config_ minus one of _configs_ when the call succeeds and _configs_ is not NULL. | | | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-004 | The eglGetConfigs function shall return no more than _config_size_ EGLConfig handles in _configs_. | This applies even when more configurations are available on _dpy_. | | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-005 | The eglGetConfigs function shall return EGL_TRUE, return no configuration handles, and write the total number of available configurations to _num_config_ when _configs_ is NULL. | This supports a two-call count-and-fetch sequence. | | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-006 | The eglGetConfigs function shall return EGL_FALSE and generate EGL_NOT_INITIALIZED when EGL is not initialized on _dpy_. | | | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-007 | The eglGetConfigs function shall return EGL_FALSE and generate EGL_BAD_PARAMETER when _num_config_ is NULL. | | | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-008 | The eglGetConfigs function shall return EGL_FALSE and generate EGL_BAD_DISPLAY when _dpy_ does not name a valid EGLDisplay. | | | EGL 1.0 Full Specification, 3.1 Errors, p. 9 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-009 | The graphics library shall keep EGLConfig handles returned by eglGetConfigs valid until the EGLDisplay from which they were obtained is terminated. | | | EGL 1.0 Full Specification, 3.4.2 Lifetime of Configurations, p. 18 | No | Req-EGL-001 | Test |

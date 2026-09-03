| Field | Value |
| --- | --- |
| Module Name | EGL10 |
| Sub-Module Name | ConfigurationManagement |
| Target Function | _eglGetConfigs_ |
| Safety Level | DAL A |
| Rationale | Not derived |
| Create Date | 31.08.2026 |

| Requirement ID | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GS-EGL10-CM-GCS-001 | The graphics library shall provide API function _eglGetConfigs_ that obtains the list of EGL configurations available on a specified display. |  | _EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config);_<br><br>_Input Parameters:_<br>**dpy:** Specifies the EGL display connection.<br>**configs:** Is _NULL_ for a count-only query or points to a buffer of _config_size_ _EGLConfig_ elements.<br>**config_size:** Specifies the non-negative capacity of _configs_.<br><br>_Output Parameter:_<br>**num_config:** Shall not be _NULL_ and receives the number of configurations returned by the call. | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-002 | When _dpy_ is initialized and _num_config_ is not _NULL_, _eglGetConfigs(dpy, NULL, 0, num_config)_ shall return _EGL_TRUE_. |  |  | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-003 | On a successful count-only call, _eglGetConfigs_ shall write the total number of configurations available on _dpy_ to the memory area pointed to by _num_config_. |  |  | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-004 | When _dpy_ is initialized, _config_size_ is non-negative, _configs_ points to a buffer of _config_size_ elements, and _num_config_ is not _NULL_, _eglGetConfigs_ shall return _EGL_TRUE_. |  |  | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-005 | On a successful call with _configs_ not _NULL_, _eglGetConfigs_ shall write valid _EGLConfig_ handles for _dpy_ to elements _0_ through _*num_config - 1_ of _configs_. |  |  | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-006 | On a successful call with _configs_ not _NULL_, _eglGetConfigs_ shall not write more than _config_size_ _EGLConfig_ handles to _configs_. |  |  | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-007 | On a successful call with _configs_ not _NULL_, _eglGetConfigs_ shall write the number of returned _EGLConfig_ handles to the memory area pointed to by _num_config_. |  |  | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-008 | When _dpy_ is initialized, _configs_ points to a buffer of _config_size_ elements, _num_config_ is not _NULL_, and the total available configuration count is greater than _config_size_, _eglGetConfigs_ shall return _EGL_TRUE_ and write no more than _config_size_ handles. |  |  | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-009 | When _dpy_ is initialized, _configs_ points to a buffer whose _config_size_ equals the total available configuration count, and _num_config_ is not _NULL_, _eglGetConfigs_ shall return _EGL_TRUE_, write every available _EGLConfig_ to _configs_, and write that total count to the memory area pointed to by _num_config_. |  |  | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-010 | If _dpy_ does not name a valid EGL display connection, _eglGetConfigs_ shall return _EGL_FALSE_ and make _eglGetError_ return _EGL_BAD_DISPLAY_. |  |  | EGL 1.0 Full Specification, 3.1 Errors, p. 9; 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-011 | If EGL is not initialized on a valid _dpy_, _eglGetConfigs_ shall return _EGL_FALSE_ and make _eglGetError_ return _EGL_NOT_INITIALIZED_. |  |  | EGL 1.0 Full Specification, 3.1 Errors, pp. 8-9; 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |
| GS-EGL10-CM-GCS-012 | If _num_config_ is _NULL_, _eglGetConfigs_ shall return _EGL_FALSE_ and make _eglGetError_ return _EGL_BAD_PARAMETER_. |  |  | EGL 1.0 Full Specification, 3.4.1 Querying Configurations, p. 15 | No | Req-EGL-001 | Test |

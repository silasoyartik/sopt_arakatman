| | |
| --- | --- |
| **Module Name** | GLES31 |
| **Sub-Module Name** | ComputeShaders |
| **Target Function** | `glBufferData` |
| **Safety Level** | DAL A |
| **Rationale** | Not derived |
| **Create Date** | 11.09.2026 |

Scope: buffer target and usage enums used by the GLES 3.1 compute shader
tests, plus the explicitly requested `GL_DYNAMIC_COPY` usage.
`GL_DYNAMIC_READ` and `GL_STATIC_READ` occur in the current compute TPs;
`GL_DYNAMIC_COPY` does not yet occur in their calls. Usage values are
performance hints and do not restrict actual buffer access.

| Requirement ID | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GS-GLES31-CS-BD-001 | The `glBufferData` function shall support `GL_SHADER_STORAGE_BUFFER` and allocate `size` bytes for its generically bound buffer. | Verify `GL_BUFFER_SIZE` with a representable test size. Indexed bindings do not select the object modified by this call. | `void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);` | GLES 3.1, Section 6.2, Tables 6.1 and 6.3 | No | Req-GL-002 | Test |
| GS-GLES31-CS-BD-002 | The `glBufferData` function shall accept `GL_DYNAMIC_COPY` as `usage` when `target` is `GL_SHADER_STORAGE_BUFFER` and record it as `GL_BUFFER_USAGE`. | Hint: GL-produced data, repeatedly replaced and consumed many times by GL operations, including compute dispatches. | | GLES 3.1, Section 6.2, pp. 51–52 | No | Req-GL-002 | Test |
| GS-GLES31-CS-BD-003 | The `glBufferData` function shall accept `GL_DYNAMIC_READ` as `usage` when `target` is `GL_SHADER_STORAGE_BUFFER` and record it as `GL_BUFFER_USAGE`. | Hint: GL-produced data, repeatedly replaced and read many times by the application. | | GLES 3.1, Section 6.2 | No | Req-GL-002 | Test |
| GS-GLES31-CS-BD-004 | The `glBufferData` function shall accept `GL_STATIC_READ` as `usage` when `target` is `GL_SHADER_STORAGE_BUFFER` and record it as `GL_BUFFER_USAGE`. | Hint: GL-produced data, supplied once and read many times by the application. | | GLES 3.1, Section 6.2 | No | Req-GL-002 | Test |

Reference: [OpenGL ES 3.1 Specification, November 3, 2016](https://registry.khronos.org/OpenGL/specs/es/3.1/es_spec_3.1.pdf).

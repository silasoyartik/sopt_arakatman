| | |
| --- | --- |
| **Module Name** | GLES31 |
| **Sub-Module Name** | ComputeShaders |
| **Target Function** | `glBindBuffer` |
| **Safety Level** | DAL A |
| **Rationale** | Not derived |
| **Create Date** | 11.09.2026 |

Scope: buffer target used by the GLES 3.1 compute shader tests.

| Requirement ID | Requirement | Information | Definitions | Reference | Impl.-Dep. | Source | Verification Method |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GS-GLES31-CS-BB-001 | The graphics library shall provide `glBindBuffer` with support for the `GL_SHADER_STORAGE_BUFFER` target. | Target used by the compute shader buffer setup. | `void glBindBuffer(GLenum target, GLuint buffer);` | GLES 3.1, Section 6.1, Table 6.1, pp. 48–50 | No | Req-GL-002 | Test |

Reference: [OpenGL ES 3.1 Specification, November 3, 2016](https://registry.khronos.org/OpenGL/specs/es/3.1/es_spec_3.1.pdf).

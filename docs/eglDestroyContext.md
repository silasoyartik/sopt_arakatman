# EGL 1.0: `eglDestroyContext`

```c
EGLBoolean eglDestroyContext(EGLDisplay dpy,
                             EGLContext ctx);
```

`eglDestroyContext`, daha önce oluşturulmuş bir `EGLContext` nesnesini yok edilmek üzere işaretler.

EGL 1.0'a göre context o anda herhangi bir thread üzerinde current değilse kaynakları mümkün olan en kısa sürede serbest bırakılır. Context current durumdaysa `eglDestroyContext` çağrısından sonra hemen ortadan kalkmaz; current kaldığı sürece kullanılmaya devam eder ve ilgili thread üzerinde sonraki geçerli `eglMakeCurrent` çağrısında gerçek anlamda yok edilir.

## Kavramsal Akış

Normal context yaşam döngüsü:

```text
eglCreateContext()
       |
       v
  EGLContext
       |
       v
eglMakeCurrent()
       |
       v
OpenGL ES rendering
       |
       v
eglMakeCurrent(... EGL_NO_CONTEXT)
       |
       v
eglDestroyContext()
       |
       v
Context kaynakları serbest bırakılır
```

Context current iken destroy edilirse:

```text
EGLContext current
       |
       v
eglDestroyContext()
       |
       v
"silinmek üzere işaretli"
       |
       | current olduğu sürece yaşamaya devam eder
       v
sonraki geçerli eglMakeCurrent()
       |
       v
gerçek kaynak serbest bırakma
```

## Parametreler

### `dpy`

`dpy`, yok edilecek context'in ait olduğu `EGLDisplay` nesnesidir.

| Değer                                            | Sonuç                                       |
| ------------------------------------------------- | -------------------------------------------- |
| Context'in oluşturulduğu geçerli`EGLDisplay` | Normal kullanım.                            |
| GBM tabanlı initialized`EGLDisplay`            | Bu projede kullanılacak display türüdür. |

Bu projede:

```text
/dev/dri/card*
      |
      v
gbm_device
      |
      v
EGLDisplay
      |
      +-- EGLContext
```

Örnek:

```c
EGLBoolean ok = eglDestroyContext(
    egl_display,
    egl_context
);
```

Pratik olarak:

```text
dpy = egl_context'in ait olduğu initialized EGLDisplay
```

olmalıdır.

### `ctx`

`ctx`, yok edilmek üzere işaretlenecek rendering context handle'ıdır.

Normal olarak daha önce:

```c
EGLContext egl_context = eglCreateContext(
    egl_display,
    egl_config,
    EGL_NO_CONTEXT,
    NULL
);
```

ile oluşturulmuş bir context verilir.

Ardından:

```c
eglDestroyContext(
    egl_display,
    egl_context
);
```

çağrılır.

#### Context current değilse

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);

eglDestroyContext(
    egl_display,
    egl_context
);
```

Bu en temiz kapanış biçimidir.

#### Context current ise

EGL 1.0'a göre:

```c
eglDestroyContext(
    egl_display,
    egl_context
);
```

çağrısı context'i hemen kullanılamaz hale getirmez.

Context:

```text
current
  +
destroy çağrılmış
```

durumunda ise kaynakları current kaldığı sürece tutulur.

Thread üzerinde daha sonra geçerli bir:

```c
eglMakeCurrent(...)
```

çağrısı yapıldığında eski context artık current olmaktan çıkar ve silme işlemi tamamlanabilir.

## Dönüş Değeri

Fonksiyonun dönüş tipi:

```c
EGLBoolean
```

Başarılı çağrı:

```text
EGL_TRUE
```

Başarısız çağrı:

```text
EGL_FALSE
```

Örnek:

```c
if (!eglDestroyContext(egl_display, egl_context)) {
    EGLint err = eglGetError();
}
```

## Parametre Matrisi

| `dpy`             | `ctx`                             | Sonuç                                                                        |
| ------------------- | ----------------------------------- | ----------------------------------------------------------------------------- |
| Geçerli EGLDisplay | Current olmayan geçerli EGLContext | Context yok edilir.                                                           |
| Geçerli EGLDisplay | Current olan geçerli EGLContext    | Silinmek üzere işaretlenir; current kaldığı sürece kaynakları tutulur. |
| Geçerli EGLDisplay | Geçersiz context                   | Çağrı başarısız olur.                                                   |

## EGL 1.0 Hata Kodu

EGL 1.0 bu fonksiyon için doğrudan şu hatayı tanımlar:

| Hata                | Ne zaman                                             |
| ------------------- | ---------------------------------------------------- |
| `EGL_BAD_CONTEXT` | `ctx` geçerli bir EGL rendering context değilse. |

Fonksiyon başarısız olduğunda:

```c
EGLint err = eglGetError();
```

ile hata kodu okunabilir.

## GBM / DRM ile İlişkisi

`eglDestroyContext` yalnızca EGL context nesnesini yönetir.

Şunları yok etmez:

```text
gbm_surface
gbm_device
GBM BO
DRM framebuffer
DRM connector
DRM CRTC
DRM file descriptor
```

Yani:

```text
eglDestroyContext()
        |
        +-- EGLContext temizliği
        |
        X-- GBM surface temizliği değil
        X-- DRM/KMS temizliği değil
```

GBM için ayrıca:

```c
gbm_surface_destroy(gbm_surface);
gbm_device_destroy(gbm);
```

gibi ilgili GBM fonksiyonları kullanılır.

## Temel Kullanım

```c
/* Context'i thread'den ayır */
if (!eglMakeCurrent(
        egl_display,
        EGL_NO_SURFACE,
        EGL_NO_SURFACE,
        EGL_NO_CONTEXT)) {
    EGLint err = eglGetError();
}

/* Context'i yok et */
if (!eglDestroyContext(
        egl_display,
        egl_context)) {
    EGLint err = eglGetError();
}
```

## Bölüm Özeti

- `eglDestroyContext`, bir `EGLContext` nesnesini yok edilmek üzere işaretler.
- `dpy`, context'in ait olduğu `EGLDisplay` nesnesidir.
- `ctx`, yok edilecek rendering context'tir.
- Başarılı çağrı `EGL_TRUE`, başarısız çağrı `EGL_FALSE` döndürür.
- `ctx` current değilse kaynakları serbest bırakılabilir.
- `ctx` current ise hemen yok edilmez.
- Current context'in gerçek silinmesi, thread üzerinde sonraki geçerli `eglMakeCurrent` çağrısıyla tamamlanır.
- Bu fonksiyon GBM veya DRM/KMS kaynaklarını yok etmez.


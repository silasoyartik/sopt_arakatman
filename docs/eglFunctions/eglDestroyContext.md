# EGL 1.0: `eglDestroyContext`

```c
EGLBoolean eglDestroyContext(
    EGLDisplay dpy,
    EGLContext ctx
);
```

## 1. Bu Fonksiyon Ne Yapar?

`eglDestroyContext`, bir `EGLContext` nesnesini silinmek üzere işaretler.
Context herhangi bir thread'de current değilse kaynakları mümkün olan en
kısa sürede serbest bırakılabilir. Context current ise gerçek silme işlemi
ertelenir.

```text
Current değil -> eglDestroyContext() -> silinebilir
Current       -> eglDestroyContext() -> silme için işaretlenir
                                      -> current kaldığı sürece geçerlidir
```

Fonksiyon iki parametre alır:

```text
dpy -> Context'in ait olduğu, initialize edilmiş EGLDisplay
ctx -> Silinecek EGLContext
```

---

# 2. Birinci Parametre: `dpy`

## 2.1 Senaryo A - Geçerli `EGLDisplay`

`dpy`, `ctx` nesnesinin oluşturulduğu initialize edilmiş display olmalıdır.

```c
EGLBoolean result = eglDestroyContext(
    egl_display,
    egl_context
);
```

`ctx` de geçerliyse beklenen sonuç:

```text
result = EGL_TRUE
```

## 2.2 Senaryo B - `EGL_NO_DISPLAY`

```c
EGLBoolean result = eglDestroyContext(
    EGL_NO_DISPLAY,
    egl_context
);

EGLint error = eglGetError();
```

Beklenen sonuç:

```text
result = EGL_FALSE
error  = EGL_BAD_DISPLAY
```

## 2.3 Initialize Edilmemiş Display

Handle geçerli olsa bile EGL ilgili display üzerinde initialize edilmemişse
işlem başarısız olur:

```text
result = EGL_FALSE
error  = EGL_NOT_INITIALIZED
```

![eglDestroyContext dpy senaryoları](image/eglDestroyContext/dpy-flow.svg)

---

# 3. İkinci Parametre: `ctx`

## 3.1 Senaryo A - Geçerli ve Current Olmayan Context

Context herhangi bir thread'de current değilse silme isteği başarılı olur ve
kaynakları mümkün olan en kısa sürede serbest bırakılabilir.

```c
EGLBoolean result = eglDestroyContext(
    egl_display,
    egl_context
);
```

Beklenen sonuç:

```text
result = EGL_TRUE
```

Bu çağrıdan sonra uygulama `egl_context` handle'ını tekrar kullanmamalıdır.

## 3.2 Senaryo B - Geçerli ve Current Context

Current context için `eglDestroyContext` yine başarılı olur; fakat context
hemen serbest bırakılmaz.

```c
eglMakeCurrent(
    egl_display,
    egl_surface,
    egl_surface,
    egl_context
);

EGLBoolean result = eglDestroyContext(
    egl_display,
    egl_context
);
```

Beklenen davranış:

```text
result = EGL_TRUE
context = silme için işaretli, ancak hala current
```

Thread sonraki geçerli `eglMakeCurrent` çağrısıyla context'i bıraktığında
gerçek silme tamamlanabilir:

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

## 3.3 Senaryo C - Geçersiz `EGLContext`

```c
EGLContext invalid_context = (EGLContext)0;

EGLBoolean result = eglDestroyContext(
    egl_display,
    invalid_context
);

EGLint error = eglGetError();
```

Beklenen sonuç:

```text
result = EGL_FALSE
error  = EGL_BAD_CONTEXT
```

> `(EGLContext)0` yalnızca geçersiz handle senaryosunu göstermek içindir.

![eglDestroyContext ctx senaryoları](image/eglDestroyContext/context-flow.svg)

---

# 4. Current ve Current Olmayan Context Karşılaştırması

| Context durumu | `eglDestroyContext` sonucu | Gerçek silme |
| --- | --- | --- |
| Current değil | `EGL_TRUE` | Mümkün olan en kısa sürede |
| Current | `EGL_TRUE` | Sonraki geçerli `eglMakeCurrent` sonrası |
| Geçersiz handle | `EGL_FALSE` | Silinecek geçerli nesne yok |

`eglMakeCurrent(..., EGL_NO_CONTEXT)` context'i thread'den ayırır;
`eglDestroyContext()` ise context'i silinmek üzere işaretler. Bunlar aynı
işlem değildir.

---

# 5. Dönüş Değeri

```text
EGL_TRUE  -> Silme isteği kabul edildi
EGL_FALSE -> İşlem başarısız; eglGetError() ile hata okunmalı
```

`EGL_TRUE`, current context kaynaklarının o anda fiziksel olarak serbest
bırakıldığı anlamına gelmez.

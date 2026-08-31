# EGL 1.0: `eglDestroySurface`

```c
EGLBoolean eglDestroySurface(
    EGLDisplay dpy,
    EGLSurface surface
);
```

## 1. Bu Fonksiyon Ne Yapar?

`eglDestroySurface`, window, pbuffer veya pixmap türündeki bir `EGLSurface`
nesnesini silinmek üzere işaretler.

```text
Current değil -> eglDestroySurface() -> silinebilir
Current       -> eglDestroySurface() -> silme için işaretlenir
                                      -> current kaldığı sürece geçerlidir
```

Bu projede EGL surface ile GBM native surface ayrı nesnelerdir:

```text
eglDestroySurface()       -> EGLSurface nesnesini yönetir
gbm_surface_destroy()     -> struct gbm_surface nesnesini yönetir
```

Fonksiyon iki parametre alır:

```text
dpy     -> Surface'in ait olduğu, initialize edilmiş EGLDisplay
surface -> Silinecek EGLSurface
```

---

# 2. Birinci Parametre: `dpy`

## 2.1 Senaryo A - Geçerli `EGLDisplay`

```c
EGLBoolean result = eglDestroySurface(
    egl_display,
    egl_surface
);
```

`surface` aynı display'a ait ve geçerliyse beklenen sonuç:

```text
result = EGL_TRUE
```

## 2.2 Senaryo B - `EGL_NO_DISPLAY`

```c
EGLBoolean result = eglDestroySurface(
    EGL_NO_DISPLAY,
    egl_surface
);

EGLint error = eglGetError();
```

Beklenen sonuç:

```text
result = EGL_FALSE
error  = EGL_BAD_DISPLAY
```

## 2.3 Initialize Edilmemiş Display

```text
result = EGL_FALSE
error  = EGL_NOT_INITIALIZED
```

![eglDestroySurface dpy senaryoları](image/eglDestroySurface/dpy-flow.svg)

---

# 3. İkinci Parametre: `surface`

## 3.1 Senaryo A - Geçerli ve Current Olmayan Surface

Surface hiçbir thread'de current draw veya current read surface değilse silme
isteği başarılı olur ve kaynakları mümkün olan en kısa sürede serbest
bırakılabilir.

```c
EGLBoolean result = eglDestroySurface(
    egl_display,
    egl_surface
);
```

Beklenen sonuç:

```text
result = EGL_TRUE
```

## 3.2 Senaryo B - Geçerli ve Current Surface

Current draw veya read surface için silme isteği kabul edilir, fakat gerçek
silme ertelenir.

```c
eglMakeCurrent(
    egl_display,
    egl_surface,
    egl_surface,
    egl_context
);

EGLBoolean result = eglDestroySurface(
    egl_display,
    egl_surface
);
```

Beklenen davranış:

```text
result  = EGL_TRUE
surface = silme için işaretli, ancak hala current
```

Sonraki geçerli `eglMakeCurrent` çağrısı eski surface'i current durumdan
çıkardığında gerçek silme tamamlanabilir:

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

## 3.3 Senaryo C - Geçersiz `EGLSurface`

```c
EGLSurface invalid_surface = (EGLSurface)0;

EGLBoolean result = eglDestroySurface(
    egl_display,
    invalid_surface
);

EGLint error = eglGetError();
```

Beklenen sonuç:

```text
result = EGL_FALSE
error  = EGL_BAD_SURFACE
```

> `(EGLSurface)0` yalnızca geçersiz handle senaryosunu göstermek içindir.

![eglDestroySurface surface senaryoları](image/eglDestroySurface/surface-flow.svg)

---

# 4. Current ve Current Olmayan Surface Karşılaştırması

| Surface durumu | `eglDestroySurface` sonucu | Gerçek silme |
| --- | --- | --- |
| Current değil | `EGL_TRUE` | Mümkün olan en kısa sürede |
| Current | `EGL_TRUE` | Sonraki geçerli `eglMakeCurrent` sonrası |
| Geçersiz handle | `EGL_FALSE` | Silinecek geçerli nesne yok |

EGL surface silindikten sonra ona karşılık gelen native window veya GBM
surface gerekiyorsa kendi platform API'siyle ayrıca temizlenmelidir.

---

# 5. Dönüş Değeri

```text
EGL_TRUE  -> Silme isteği kabul edildi
EGL_FALSE -> İşlem başarısız; eglGetError() ile hata okunmalı
```

`EGL_TRUE`, current surface kaynaklarının o anda fiziksel olarak serbest
bırakıldığı anlamına gelmez.

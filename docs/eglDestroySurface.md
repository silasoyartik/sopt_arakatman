# EGL 1.0: `eglDestroySurface`

```c
EGLBoolean eglDestroySurface(EGLDisplay dpy,
                             EGLSurface surface);
```

`eglDestroySurface`, daha önce oluşturulmuş bir `EGLSurface` nesnesini yok edilmek üzere işaretler.

Fonksiyon window, pbuffer veya pixmap türündeki EGL surface'lerde kullanılabilir. Bu projede ise `eglCreateWindowSurface` ile GBM native surface üzerinde oluşturulan window `EGLSurface` yok edilir.

## Mental Model

Bu projedeki surface zinciri:

```text
struct gbm_surface *
        |
        v
eglCreateWindowSurface()
        |
        v
    EGLSurface
        |
        v
eglMakeCurrent()
        |
        v
OpenGL ES rendering
        |
        v
eglDestroySurface()
```

Önemli ayrım:

```text
eglDestroySurface()  -> EGLSurface'i yönetir
gbm_surface_destroy() -> struct gbm_surface * nesnesini yönetir
```

Bunlar aynı nesne değildir.

## Parametreler

### `dpy`

`dpy`, yok edilecek surface'in ait olduğu `EGLDisplay` nesnesidir.

Bu projede:

```text
GBM native platform
       |
       v
   EGLDisplay
       |
       +-- EGLSurface
```

Örnek:

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

Pratik kullanım:

```text
dpy = egl_surface'in oluşturulduğu initialized EGLDisplay
```

### `surface`

`surface`, yok edilmek üzere işaretlenecek `EGLSurface` handle'ıdır.

Bu projede daha önce:

```c
EGLSurface egl_surface =
    eglCreateWindowSurface(
        egl_display,
        egl_config,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );
```

ile oluşturulmuştur.

Sonra:

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

çağrılır.

#### Surface current değilse

Surface hiçbir thread'de current draw/read surface değilse, kaynakları mümkün olan en kısa sürede bırakılabilir.

```text
EGLSurface
   |
   | current değil
   v
eglDestroySurface()
   |
   v
kaynaklar serbest bırakılır
```

#### Surface current ise

EGL 1.0'a göre current bir surface destroy edildiğinde kaynakları hemen serbest bırakılmaz.

```text
surface current
      |
      v
eglDestroySurface()
      |
      v
silinmek üzere işaretlenir
      |
      | current kaldığı sürece yaşamaya devam eder
      v
sonraki geçerli eglMakeCurrent()
      |
      v
gerçek yok etme tamamlanabilir
```

Bu nedenle güvenli kullanımda önce context/surface bağı kaldırılır.

## Dönüş Değeri

Fonksiyon:

```c
EGLBoolean
```

döndürür.

Başarı:

```text
EGL_TRUE
```

Başarısızlık:

```text
EGL_FALSE
```

Örnek:

```c
if (!eglDestroySurface(
        egl_display,
        egl_surface)) {
    EGLint err = eglGetError();
}
```

## Kullanım Senaryoları

### 1. Normal surface kapatma

Önce current bağ kaldırılır:

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

Ardından:

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

Bu proje için en temiz kullanım şeklidir.

### 2. Current surface üzerinde destroy

Örneğin:

```c
eglMakeCurrent(
    egl_display,
    egl_surface,
    egl_surface,
    egl_context
);

eglDestroySurface(
    egl_display,
    egl_surface
);
```

EGL 1.0'a göre surface current kaldığı için kaynakları hemen bırakılmaz.

Daha sonra:

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

çağrısıyla current bağı değiştiğinde surface'in yok edilmesi tamamlanabilir.

### 3. GBM native surface ile ilişki

Bu projede:

```text
gbm_surface
     |
     v
EGLSurface
```

ilişkisi vardır.

Ancak:

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

yalnızca EGL katmanını temizler.

Native GBM surface ayrıca:

```c
gbm_surface_destroy(gbm_surface);
```

ile temizlenmelidir.

Doğru mantık:

```text
EGLSurface'i bırak
      |
      v
eglDestroySurface()
      |
      v
GBM surface artık EGL tarafından kullanılmıyor
      |
      v
gbm_surface_destroy()
```

## Parametre Matrisi

| `dpy` | `surface` | Sonuç |
|---|---|---|
| Geçerli EGLDisplay | Current olmayan geçerli EGLSurface | Surface yok edilir. |
| Geçerli EGLDisplay | Current geçerli EGLSurface | Silinmek üzere işaretlenir; current kaldığı sürece kaynakları tutulur. |
| Geçerli EGLDisplay | Geçersiz EGLSurface | Başarısız. |

## EGL 1.0 Hata Kodu

EGL 1.0 bu fonksiyon için doğrudan:

| Hata | Ne zaman |
|---|---|
| `EGL_BAD_SURFACE` | `surface` geçerli bir EGL rendering surface değilse. |

tanımlar.

Başarısız çağrı:

```c
EGL_FALSE
```

döndürür.

Ardından:

```c
EGLint err = eglGetError();
```

ile hata alınabilir.

## Direct-to-Display Projede Kapanış

Örneğin:

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

eglDestroySurface(
    egl_display,
    egl_surface
);

eglTerminate(
    egl_display
);

gbm_surface_destroy(
    gbm_surface
);

gbm_device_destroy(
    gbm
);
```

DRM/KMS tarafındaki framebuffer, connector/CRTC state ve file descriptor da kendi API'leriyle ayrıca temizlenir.

## GBM BO Notu

Direct-to-display akışında render sonrasında:

```c
struct gbm_bo *bo =
    gbm_surface_lock_front_buffer(gbm_surface);
```

ile front buffer kilitlenmiş olabilir.

Bu buffer'ın yaşam döngüsü `EGLSurface` yaşam döngüsünden farklıdır.

Dolayısıyla cleanup sırasında:

```text
EGLSurface
GBM surface
GBM BO
DRM framebuffer
```

nesnelerinin aynı şey olmadığı unutulmamalıdır.

`eglDestroySurface()` yalnızca ilkini yönetir.

## Minimal Doğru Kullanım

```c
/* EGL surface/context bağını kaldır */
if (!eglMakeCurrent(
        egl_display,
        EGL_NO_SURFACE,
        EGL_NO_SURFACE,
        EGL_NO_CONTEXT)) {
    EGLint err = eglGetError();
}

/* EGL surface'i yok et */
if (!eglDestroySurface(
        egl_display,
        egl_surface)) {
    EGLint err = eglGetError();
}

/* EGL surface artık GBM surface'i kullanmıyor */
gbm_surface_destroy(gbm_surface);
```

## Parametre Değiştirme Örnekleri

### 1. Current olmayan surface

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

```text
surface = geçerli ve current olmayan EGLSurface
Sonuç = surface kaynakları serbest bırakılabilir
```

### 2. Current surface

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

```text
surface = current EGLSurface
Sonuç = hemen tamamen yok edilmez;
        silinmek üzere işaretlenir
```

### 3. Farklı surface'ler

```c
eglDestroySurface(
    egl_display,
    surface_a
);

eglDestroySurface(
    egl_display,
    surface_b
);
```

```text
surface_a ve surface_b bağımsız EGLSurface handle'larıdır.
Hangi handle verilirse o surface yok edilmek üzere işaretlenir.
```

## EGL 1.0 İçin Pratik Özet

- `eglDestroySurface`, herhangi bir türdeki `EGLSurface` nesnesini yok edilmek üzere işaretler.
- `dpy`, surface'in ait olduğu `EGLDisplay` nesnesidir.
- `surface`, yok edilecek EGL rendering surface'tir.
- Başarıda `EGL_TRUE`, başarısızlıkta `EGL_FALSE` döner.
- Current olmayan surface mümkün olan en kısa sürede serbest bırakılır.
- Current surface hemen silinmez; current kaldığı sürece kaynakları tutulur.
- Bu projede `eglDestroySurface` GBM surface'i yok etmez.
- `struct gbm_surface *` ayrıca GBM API ile temizlenmelidir.

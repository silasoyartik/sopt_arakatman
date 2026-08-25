# EGL 1.0: `eglGetError`

```c
EGLint eglGetError(void);
```

`eglGetError`, çağıran thread üzerinde en son EGL çağrısıyla ilişkili EGL hata durumunu döndürür.

Fonksiyon parametre almaz. Bunun nedeni hata bilgisinin belirli bir `EGLDisplay`, `EGLContext` veya `EGLSurface` handle'ı üzerinden değil, çağıran thread'in EGL hata durumu üzerinden okunmasıdır.

## Mental Model

```text
Thread
  |
  +-- EGL fonksiyon çağrısı
  |       |
  |       +-- başarılı / başarısız
  |       |
  |       v
  |    error state
  |
  +-- eglGetError()
          |
          v
      EGLint hata kodu
```

Örnek:

```c
EGLSurface surface =
    eglCreateWindowSurface(
        egl_display,
        egl_config,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );

if (surface == EGL_NO_SURFACE) {
    EGLint err = eglGetError();
}
```

## Parametreler

`eglGetError` parametre almaz:

```c
eglGetError();
```

Dolayısıyla bu fonksiyonda diğer EGL fonksiyonlarındaki gibi:

```text
dpy
config
surface
context
attribute
```

parametreleri yoktur.

İncelenecek ana konu fonksiyonun döndürebileceği `EGLint` değerleridir.

## Dönüş Değeri

Fonksiyonun dönüş tipi:

```c
EGLint
```

EGL 1.0'da dönebilecek hata kodları:

| Değer | Anlamı |
|---|---|
| `EGL_SUCCESS` | Son ilgili EGL işlemi başarılıdır / hata yoktur. |
| `EGL_NOT_INITIALIZED` | EGL ilgili display için initialize edilmemiştir veya initialize edilememiştir. |
| `EGL_BAD_ACCESS` | İstenen kaynağa erişilememiştir. |
| `EGL_BAD_ALLOC` | İstenen işlem için gerekli kaynak ayrılamamıştır. |
| `EGL_BAD_ATTRIBUTE` | Tanınmayan attribute veya attribute değeri kullanılmıştır. |
| `EGL_BAD_CONTEXT` | Bir `EGLContext` argümanı geçerli context değildir. |
| `EGL_BAD_CONFIG` | Bir `EGLConfig` argümanı geçerli config değildir. |
| `EGL_BAD_CURRENT_SURFACE` | Thread'in current surface'i artık geçerli değildir. |
| `EGL_BAD_DISPLAY` | Bir `EGLDisplay` argümanı geçerli değildir veya display initialize edilmemiştir. |
| `EGL_BAD_SURFACE` | Bir `EGLSurface` argümanı geçerli surface değildir. |
| `EGL_BAD_MATCH` | Fonksiyon argümanları birbirleriyle uyumlu değildir. |
| `EGL_BAD_PARAMETER` | Bir veya daha fazla argüman değeri geçersizdir. |
| `EGL_BAD_NATIVE_PIXMAP` | Native pixmap geçerli değildir. |
| `EGL_BAD_NATIVE_WINDOW` | Native window geçerli değildir. |

## `EGL_SUCCESS`

```c
EGLint err = eglGetError();

if (err == EGL_SUCCESS) {
    /* EGL hata durumu yok */
}
```

Anlamı:

```text
EGL_SUCCESS
    |
    v
Fonksiyon başarılı / kayıtlı EGL hatası yok
```

## `EGL_NOT_INITIALIZED`

Örneğin ilgili EGLDisplay initialize edilmeden EGL işlemi yapılmaya çalışılırsa görülebilir.

Doğru temel sıra:

```c
EGLDisplay dpy = /* display'i elde et */;

EGLint major;
EGLint minor;

if (!eglInitialize(dpy, &major, &minor)) {
    EGLint err = eglGetError();

    if (err == EGL_NOT_INITIALIZED) {
        /* Initialize başarısız */
    }
}
```

Bu projede EGL display GBM platformuyla ilişkili olduğundan GBM/EGL platform kurulumunun başarılı olması gerekir.

## `EGL_BAD_ACCESS`

Bir EGL kaynağına erişim kuralları ihlal edildiğinde dönebilir.

Örneğin bir context başka thread üzerinde current ise:

```text
Thread A
  |
  +-- ctx current

Thread B
  |
  +-- aynı ctx'yi current yapmaya çalışır
          |
          v
      EGL_BAD_ACCESS
```

Örnek kontrol:

```c
if (!eglMakeCurrent(dpy, surface, surface, ctx)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_ACCESS) {
        /* Kaynak başka thread tarafından kullanılıyor olabilir */
    }
}
```

## `EGL_BAD_ALLOC`

EGL istenen işlem için yeterli kaynak ayıramazsa döner.

Örneğin:

```c
EGLSurface surface =
    eglCreateWindowSurface(
        dpy,
        config,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );

if (surface == EGL_NO_SURFACE) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_ALLOC) {
        /* Surface için gerekli kaynak ayrılamadı */
    }
}
```

Bu, GBM fonksiyonlarının kendi hata modelinden ayrıdır. `eglGetError()` yalnızca EGL hata durumunu verir.

## `EGL_BAD_ATTRIBUTE`

EGL attribute listesinde geçersiz attribute veya değer kullanıldığında oluşabilir.

Örnek genel EGL modeli:

```c
const EGLint attribs[] = {
    /* EGL 1.0 tarafından desteklenmeyen/geçersiz bir attribute */
    0x12345678, 1,
    EGL_NONE
};
```

İlgili EGL fonksiyonu başarısız olduktan sonra:

```c
EGLint err = eglGetError();

if (err == EGL_BAD_ATTRIBUTE) {
    /* Attribute listesi geçersiz */
}
```

## `EGL_BAD_CONTEXT`

Bir EGL fonksiyonuna geçerli olmayan `EGLContext` verildiğinde oluşur.

Örneğin `eglDestroyContext`:

```c
if (!eglDestroyContext(dpy, invalid_context)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_CONTEXT) {
        /* ctx geçerli EGLContext değil */
    }
}
```

## `EGL_BAD_CONFIG`

Geçersiz `EGLConfig` handle'ı kullanıldığında oluşur.

Örneğin:

```c
EGLSurface surface =
    eglCreateWindowSurface(
        dpy,
        invalid_config,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );

if (surface == EGL_NO_SURFACE) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_CONFIG) {
        /* config geçersiz */
    }
}
```

## `EGL_BAD_CURRENT_SURFACE`

Thread üzerinde current olan surface artık geçerli olmadığında bazı EGL işlemlerinde görülebilir.

Mental model:

```text
Thread
  |
  +-- current EGLSurface
          |
          X native/current surface geçersiz hale geldi
          |
          v
EGL_BAD_CURRENT_SURFACE
```

## `EGL_BAD_DISPLAY`

Geçersiz veya uygun şekilde initialized edilmemiş display ile işlem yapılırsa görülebilir.

Örneğin:

```c
if (!eglInitialize(dpy, &major, &minor)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_DISPLAY) {
        /* dpy geçerli EGLDisplay değil */
    }
}
```

## `EGL_BAD_SURFACE`

Bir EGL fonksiyonuna geçerli olmayan surface verilirse oluşur.

Bu grubunuzdaki `eglDestroySurface` için doğrudan örnek:

```c
if (!eglDestroySurface(dpy, invalid_surface)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_SURFACE) {
        /* surface geçerli değil */
    }
}
```

## `EGL_BAD_MATCH`

Argümanlar tek tek geçerli olsa bile birbirleriyle uyumsuz olduğunda oluşabilir.

Örneğin:

```text
EGLContext geçerli
EGLSurface geçerli
        |
        v
ancak birbirleriyle uyumsuz
        |
        v
EGL_BAD_MATCH
```

`eglMakeCurrent` çağrısında context ile surface uyumsuzsa:

```c
if (!eglMakeCurrent(dpy, surface, surface, ctx)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_MATCH) {
        /* context / surface eşleşmesi uygun değil */
    }
}
```

Bu projede `eglCreateWindowSurface` sırasında native GBM surface ile EGLConfig uyumsuzluğu da `EGL_BAD_MATCH` üretebilir.

## `EGL_BAD_PARAMETER`

Bir veya daha fazla parametre değeri ilgili fonksiyon açısından geçersiz olduğunda kullanılır.

```c
EGLint err = eglGetError();

if (err == EGL_BAD_PARAMETER) {
    /* İlgili EGL fonksiyonunda parametre değeri geçersiz */
}
```

Hangi fonksiyonun hangi durumda bu hatayı ürettiği o fonksiyonun EGL 1.0 tanımına göre değerlendirilmelidir.

## `EGL_BAD_NATIVE_PIXMAP`

Geçerli olmayan bir native pixmap handle'ı kullanıldığında oluşur.

Bu direct-to-display GBM projesinde native pixmap kullanılmadığından normal akışta beklenen bir hata değildir.

```text
Bu proje:
GBM native window -> kullanılıyor
Native pixmap     -> kullanılmıyor
```

## `EGL_BAD_NATIVE_WINDOW`

`NativeWindowType` geçerli native window'u temsil etmiyorsa oluşur.

Bu projede `eglCreateWindowSurface` çağrısında native window:

```c
(EGLNativeWindowType)gbm_surface
```

olduğu için GBM surface native window rolündedir.

Örneğin:

```c
EGLSurface surface =
    eglCreateWindowSurface(
        dpy,
        config,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );

if (surface == EGL_NO_SURFACE) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_NATIVE_WINDOW) {
        /* Native window geçerli değil */
    }
}
```

## Bu Projede Kullanım Modeli

En faydalı kullanım, her EGL fonksiyonunun dönüş değerini önce kontrol edip sadece başarısız durumda `eglGetError()` çağırmaktır.

Örneğin initialize:

```c
if (!eglInitialize(egl_display, &major, &minor)) {
    EGLint err = eglGetError();
}
```

Surface oluşturma:

```c
EGLSurface egl_surface =
    eglCreateWindowSurface(
        egl_display,
        egl_config,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );

if (egl_surface == EGL_NO_SURFACE) {
    EGLint err = eglGetError();
}
```

Context oluşturma:

```c
EGLContext egl_context =
    eglCreateContext(
        egl_display,
        egl_config,
        EGL_NO_CONTEXT,
        NULL
    );

if (egl_context == EGL_NO_CONTEXT) {
    EGLint err = eglGetError();
}
```

Make current:

```c
if (!eglMakeCurrent(
        egl_display,
        egl_surface,
        egl_surface,
        egl_context)) {
    EGLint err = eglGetError();
}
```

Swap:

```c
if (!eglSwapBuffers(
        egl_display,
        egl_surface)) {
    EGLint err = eglGetError();
}
```

## Hata Yazdırma Yardımcı Fonksiyonu

Projede okunabilirlik için küçük bir yardımcı fonksiyon yazılabilir:

```c
const char *egl_error_string(EGLint error)
{
    switch (error) {
        case EGL_SUCCESS:
            return "EGL_SUCCESS";

        case EGL_NOT_INITIALIZED:
            return "EGL_NOT_INITIALIZED";

        case EGL_BAD_ACCESS:
            return "EGL_BAD_ACCESS";

        case EGL_BAD_ALLOC:
            return "EGL_BAD_ALLOC";

        case EGL_BAD_ATTRIBUTE:
            return "EGL_BAD_ATTRIBUTE";

        case EGL_BAD_CONTEXT:
            return "EGL_BAD_CONTEXT";

        case EGL_BAD_CONFIG:
            return "EGL_BAD_CONFIG";

        case EGL_BAD_CURRENT_SURFACE:
            return "EGL_BAD_CURRENT_SURFACE";

        case EGL_BAD_DISPLAY:
            return "EGL_BAD_DISPLAY";

        case EGL_BAD_SURFACE:
            return "EGL_BAD_SURFACE";

        case EGL_BAD_MATCH:
            return "EGL_BAD_MATCH";

        case EGL_BAD_PARAMETER:
            return "EGL_BAD_PARAMETER";

        case EGL_BAD_NATIVE_PIXMAP:
            return "EGL_BAD_NATIVE_PIXMAP";

        case EGL_BAD_NATIVE_WINDOW:
            return "EGL_BAD_NATIVE_WINDOW";

        default:
            return "UNKNOWN_EGL_ERROR";
    }
}
```

Kullanım:

```c
EGLint err = eglGetError();

printf(
    "EGL error: %s (0x%x)\n",
    egl_error_string(err),
    err
);
```

## GBM / DRM Hatalarından Farkı

Bu proje üç ayrı API ailesi içerir:

```text
OpenGL ES / EGL
GBM
DRM/KMS
```

`eglGetError()` sadece:

```text
EGL hata state
```

bilgisini verir.

Şunların hata sistemini okumaz:

```text
gbm_surface_create()
drmModeGetResources()
drmModeAddFB()
drmModeSetCrtc()
drmModePageFlip()
open("/dev/dri/card*")
```

DRM/Linux hatalarında genellikle fonksiyon dönüş değeri ve gerektiğinde `errno` kullanılır.

Yani:

```text
EGL fonksiyonu başarısız -> eglGetError()
DRM/Linux çağrısı        -> return value / errno
GBM çağrısı              -> GBM fonksiyonunun dönüş kontrolü
```

## Minimal Doğru Kullanım

```c
if (!eglMakeCurrent(
        egl_display,
        egl_surface,
        egl_surface,
        egl_context)) {

    EGLint err = eglGetError();

    printf(
        "eglMakeCurrent failed: 0x%x\n",
        err
    );
}
```

## Farklı Dönüş Değerlerine Göre Örnekler

### 1. `EGL_SUCCESS`

```c
EGLint err = eglGetError();

if (err == EGL_SUCCESS) {
    printf("EGL error yok\n");
}
```

### 2. `EGL_BAD_CONTEXT`

```c
if (!eglDestroyContext(dpy, invalid_ctx)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_CONTEXT) {
        printf("Context gecersiz\n");
    }
}
```

### 3. `EGL_BAD_SURFACE`

```c
if (!eglDestroySurface(dpy, invalid_surface)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_SURFACE) {
        printf("Surface gecersiz\n");
    }
}
```

### 4. `EGL_BAD_NATIVE_WINDOW`

```c
EGLSurface surface =
    eglCreateWindowSurface(
        dpy,
        config,
        native_window,
        NULL
    );

if (surface == EGL_NO_SURFACE) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_NATIVE_WINDOW) {
        printf("Native window gecersiz\n");
    }
}
```

## EGL 1.0 İçin Pratik Özet

- `eglGetError()` parametre almaz.
- Dönüş tipi `EGLint`'tir.
- Thread'in EGL hata durumunu okumak için kullanılır.
- `EGL_SUCCESS`, EGL hata durumu olmadığını belirtir.
- EGL 1.0 tüm temel EGL hata kodlarını bu fonksiyon üzerinden raporlar.
- En doğru kullanım, önce ilgili EGL fonksiyonunun başarısız olup olmadığını kontrol etmek ve ardından `eglGetError()` çağırmaktır.
- GBM veya DRM/KMS hataları `eglGetError()` ile alınmaz.
- Direct-to-display projede EGL, GBM ve DRM/KMS hata kontrolleri birbirinden ayrı tutulmalıdır.

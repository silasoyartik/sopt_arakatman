# EGL 1.0: `eglInitialize`

```c
EGLBoolean eglInitialize(EGLDisplay dpy,
                         EGLint *major,
                         EGLint *minor);
```

`eglInitialize`, bir `EGLDisplay` bağlantısını EGL kullanımı için initialize eder ve istenirse EGL implementation sürümünü `major` ve `minor` output parametreleri üzerinden döndürür.

Kısa özet:

- `dpy`: initialize edilecek EGL display.
- `major`: major sürüm output pointer'ı.
- `minor`: minor sürüm output pointer'ı.
- Başarılı çağrı: `EGL_TRUE`
- Başarısız çağrı: `EGL_FALSE`
- Geçersiz display deneyinde: `EGL_BAD_DISPLAY`

## Mental Model

```text
Native display
    |
    v
eglGetDisplay
    |
    v
EGLDisplay
    |
    v
eglInitialize
    |
    +-- EGL bağlantısı initialize edilir
    |
    +-- major/minor istenirse sürüm bilgisi alınır
```

## Parametreler

### `dpy`

`dpy`, initialize edilecek geçerli bir `EGLDisplay` olmalıdır.

| `dpy`                      | Sonuç                                     |
| ---------------------------- | ------------------------------------------ |
| Geçerli fresh`EGLDisplay` | Initialization yapılabilir.               |
| `EGL_NO_DISPLAY`           | Deneyde`EGL_FALSE`, `EGL_BAD_DISPLAY`. |

Normal akış:

```c
EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
eglInitialize(dpy, &major, &minor);
```

`eglGetDisplay` için ilgili bölüme bakınız.

### `major`

`major`, major EGL sürüm numarasının yazılabileceği `EGLint *` output parametresidir.

```c
EGLint major = -1;
eglInitialize(dpy, &major, &minor);
```

Deneylerde `-1` yalnızca değişkenin çağrı sırasında değiştirilip değiştirilmediğini görmek için sentinel değer olarak kullanıldı.

### `minor`

`minor`, minor EGL sürüm numarasının yazılabileceği `EGLint *` output parametresidir.

```c
EGLint minor = -1;
eglInitialize(dpy, &major, &minor);
```

Normal testte:

```text
major = 1
minor = 5
```

elde edildi.

Bu değer test edilen Mesa EGL 1.5 runtime'ına aittir.

## `NULL` Kullanımı

Deneylerde şu kombinasyonlar test edildi:

```c
eglInitialize(dpy, &major, &minor);
eglInitialize(dpy, NULL, &minor);
eglInitialize(dpy, &major, NULL);
eglInitialize(dpy, NULL, NULL);
```

Her test için fresh bir Wayland native display bağlantısı oluşturuldu. Böylece önceki initialization çağrılarının sonraki deneylere etkisi azaltıldı.

## Deneyler

### Test 1 — `major` ve `minor` birlikte

```c
EGLint major = -1;
EGLint minor = -1;

EGLBoolean result =
    eglInitialize(display, &major, &minor);
```

Gerçek çıktı:

```text
major before     : -1
minor before     : -1
Return value     : EGL_TRUE
major after      : 1
minor after      : 5
EGL error        : EGL_SUCCESS (0x3000)
```

### Test 2 — `major = NULL`

```c
EGLint minor = -1;

EGLBoolean result =
    eglInitialize(display, NULL, &minor);
```

Gerçek çıktı:

```text
major            : NULL
minor            : &minor
Return value     : EGL_TRUE
minor after      : -1
EGL error        : EGL_SUCCESS (0x3000)
```

Test edilen Mesa implementasyonunda çağrı başarılı oldu ancak `minor` değeri değişmedi.

### Test 3 — `minor = NULL`

```c
EGLint major = -1;

EGLBoolean result =
    eglInitialize(display, &major, NULL);
```

Gerçek çıktı:

```text
major            : &major
minor            : NULL
Return value     : EGL_TRUE
major after      : -1
EGL error        : EGL_SUCCESS (0x3000)
```

Test edilen Mesa implementasyonunda çağrı başarılı oldu ancak `major` değeri değişmedi.

> Bu iki sonuç implementation-specific deney gözlemidir. “Bütün EGL implementasyonları tek output pointer verildiğinde aynı davranışı gösterir” şeklinde genellenmemelidir.

### Test 4 — `major = NULL`, `minor = NULL`

```c
EGLBoolean result =
    eglInitialize(display, NULL, NULL);
```

Gerçek çıktı:

```text
major            : NULL
minor            : NULL
Return value     : EGL_TRUE
EGL error        : EGL_SUCCESS (0x3000)
```

Bu test, sürüm output'ları alınmadan da initialization yapılabildiğini gösterdi.

### Test 5 — `dpy = EGL_NO_DISPLAY`

```c
EGLint major = -1;
EGLint minor = -1;

EGLBoolean result =
    eglInitialize(EGL_NO_DISPLAY, &major, &minor);
```

Gerçek çıktı:

```text
dpy              : EGL_NO_DISPLAY
Return value     : EGL_FALSE
major after      : -1
minor after      : -1
EGL error        : EGL_BAD_DISPLAY (0x3008)
```

## Deney Sonuçları

| `dpy`            | `major` | `minor` | Sonuç                                     |
| ------------------ | --------- | --------- | ------------------------------------------ |
| Geçerli           | pointer   | pointer   | `EGL_TRUE`, `1.5`                      |
| Geçerli           | `NULL`  | pointer   | `EGL_TRUE`, testte `minor` değişmedi |
| Geçerli           | pointer   | `NULL`  | `EGL_TRUE`, testte `major` değişmedi |
| Geçerli           | `NULL`  | `NULL`  | `EGL_TRUE`                               |
| `EGL_NO_DISPLAY` | pointer   | pointer   | `EGL_FALSE`, `EGL_BAD_DISPLAY`         |

## Hata Matrisi

| Durum                                                 | Sonuç                                         |
| ----------------------------------------------------- | ---------------------------------------------- |
| Geçerli display, iki output pointer                  | `EGL_TRUE`; testte sürüm `1.5`           |
| Geçerli display, iki output pointer`NULL`          | `EGL_TRUE`                                   |
| `dpy == EGL_NO_DISPLAY`                             | `EGL_FALSE`, `EGL_BAD_DISPLAY`             |
| Initialization sırasında driver warning görülmesi | Tek başına EGL API hatası anlamına gelmez. |

## Mesa / Driver Warning Notu

Bazı testlerde:

```text
MESA: error: ZINK: failed to choose pdev
libEGL warning: egl: failed to create dri2 screen
libEGL warning: DRI3 error: Could not get DRI3 device
```

mesajları görüldü.

Buna rağmen ilgili EGL çağrılarında:

```text
EGL_TRUE
EGL_SUCCESS
```

elde edilebildi.

Bu nedenle driver warning ile EGL API hata sonucu birbirinden ayrılmalıdır.

## Minimal Kullanım

```c
EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

if (dpy == EGL_NO_DISPLAY) {
    return 1;
}

EGLint major;
EGLint minor;

if (!eglInitialize(dpy, &major, &minor)) {
    EGLint err = eglGetError();
    return 1;
}

printf("EGL version: %d.%d\n", major, minor);
```

## Bilerek Hata Üretme Örneği

```c
EGLint major = -1;
EGLint minor = -1;

if (!eglInitialize(EGL_NO_DISPLAY, &major, &minor)) {
    EGLint err = eglGetError(); /* Testte EGL_BAD_DISPLAY */
}
```

## EGL 1.0 İçin Pratik Özet

- `eglInitialize`, `EGLDisplay` bağlantısını EGL kullanımı için initialize eder.
- `dpy`, geçerli bir `EGLDisplay` olmalıdır.
- `major` ve `minor`, sürüm bilgisini almak için kullanılan output pointer'larıdır.
- Testte normal kullanım `1.5` döndürdü; bu runtime sürümüdür.
- `major` ve `minor` birlikte `NULL` verildiğinde test edilen implementasyonda çağrı başarılı oldu.
- `EGL_NO_DISPLAY` kullanımı `EGL_FALSE` ve `EGL_BAD_DISPLAY` üretti.
- Driver warning mesajları ile EGL API hata sonucu aynı şey değildir.


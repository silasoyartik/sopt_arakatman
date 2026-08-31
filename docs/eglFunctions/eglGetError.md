# EGL 1.0: `eglGetError`

```c
EGLint eglGetError(void);
```

## 1. Bu Fonksiyon Ne Yapar?

`eglGetError`, çağrı yapan thread için kaydedilmiş son EGL hata kodunu
döndürür. Parametre almadığı için senaryolar parametre değişikliğine göre
değil, daha önce çağrılan EGL fonksiyonunun oluşturduğu duruma göre kurulur.

```text
EGL fonksiyonu çağrılır
        |
        v
Dönüş değeri kontrol edilir
        |
        v
Başarısızsa hemen eglGetError() çağrılır
        |
        v
EGLint hata kodu alınır
```

`eglGetError()` hata kodunu döndürdükten sonra o thread'in hata durumu
`EGL_SUCCESS` olacak biçimde sıfırlanır. Bu nedenle hata, başarısız EGL
çağrısından hemen sonra ve yalnızca bir kez okunmalıdır.

![eglGetError genel akışı](image/eglGetError/error-flow.svg)

---

# 2. EGL 1.0 Dönüş Değerleri

EGL 1.0 kapsamında `eglGetError()` aşağıdaki 14 temel değerden birini
döndürebilir. `EGL_SUCCESS` teknik olarak hata değildir.

| Değer | Görülebileceği durum |
| --- | --- |
| `EGL_SUCCESS` | Kayıtlı hata yoktur. |
| `EGL_NOT_INITIALIZED` | EGL ilgili display için initialize edilmemiştir veya edilememiştir. |
| `EGL_BAD_ACCESS` | İstenen kaynağa erişim kuralı ihlal edilmiştir. |
| `EGL_BAD_ALLOC` | İşlem için kaynak ayrılamamıştır. |
| `EGL_BAD_ATTRIBUTE` | Attribute listesinde tanınmayan attribute/değer vardır. |
| `EGL_BAD_CONTEXT` | Bir context argümanı geçerli `EGLContext` değildir. |
| `EGL_BAD_CONFIG` | Bir config argümanı geçerli `EGLConfig` değildir. |
| `EGL_BAD_CURRENT_SURFACE` | Thread'in current surface'i artık geçerli değildir. |
| `EGL_BAD_DISPLAY` | Bir display argümanı geçerli `EGLDisplay` değildir. |
| `EGL_BAD_SURFACE` | Bir surface argümanı geçerli `EGLSurface` değildir. |
| `EGL_BAD_MATCH` | Argümanlar tek tek geçerli olsa da birbirleriyle uyumsuzdur. |
| `EGL_BAD_PARAMETER` | Bir veya daha fazla parametre değeri geçersizdir. |
| `EGL_BAD_NATIVE_PIXMAP` | Native pixmap geçersizdir ve implementation bunu algılayabilmiştir. |
| `EGL_BAD_NATIVE_WINDOW` | Native window geçersizdir ve implementation bunu algılayabilmiştir. |

---

# 3. Hata Senaryoları

## 3.1 Senaryo A - `EGL_SUCCESS`

Kayıtlı bir hata yokken:

```c
EGLint error = eglGetError();
```

beklenen sonuç:

```text
error = EGL_SUCCESS
```

## 3.2 Senaryo B - `EGL_NOT_INITIALIZED`

Geçerli fakat initialize edilmemiş bir display üzerinde initialize gerektiren
bir EGL işlemi yapıldığında görülebilir.

```c
EGLBoolean result = eglDestroyContext(dpy, context);
EGLint error = eglGetError();
```

```text
result = EGL_FALSE
error  = EGL_NOT_INITIALIZED
```

## 3.3 Senaryo C - `EGL_BAD_ACCESS`

Bir context başka bir thread'de current iken ikinci thread aynı context'i
current yapmaya çalışırsa kaynak erişim kuralı ihlal edilebilir.

```text
Thread A: context current
Thread B: aynı context ile eglMakeCurrent()
          -> EGL_FALSE
          -> EGL_BAD_ACCESS
```

Bu senaryo iki thread ve senkronizasyon gerektirir.

## 3.4 Senaryo D - `EGL_BAD_ALLOC`

`eglCreateContext`, `eglCreateWindowSurface` veya benzeri bir oluşturma işlemi
için gerekli kaynak ayrılamadığında görülebilir.

```text
Oluşturma fonksiyonu başarısız
-> EGL_NO_CONTEXT veya EGL_NO_SURFACE
-> eglGetError() = EGL_BAD_ALLOC
```

Kaynak tüketimini güvenli ve deterministik biçimde oluşturmak her ortamda
mümkün olmadığından bu senaryo implementation ve test ortamına bağlıdır.

## 3.5 Senaryo E - `EGL_BAD_ATTRIBUTE`

```c
const EGLint attributes[] = {
    0x7FFFFFFF, 1,
    EGL_NONE
};

EGLint num_config = 0;
EGLBoolean result = eglChooseConfig(
    dpy,
    attributes,
    NULL,
    0,
    &num_config
);

EGLint error = eglGetError();
```

```text
result = EGL_FALSE
error  = EGL_BAD_ATTRIBUTE
```

## 3.6 Senaryo F - `EGL_BAD_CONTEXT`

```c
EGLBoolean result = eglDestroyContext(
    dpy,
    (EGLContext)0
);

EGLint error = eglGetError();
```

```text
result = EGL_FALSE
error  = EGL_BAD_CONTEXT
```

## 3.7 Senaryo G - `EGL_BAD_CONFIG`

```c
EGLContext context = eglCreateContext(
    dpy,
    (EGLConfig)0,
    EGL_NO_CONTEXT,
    NULL
);

EGLint error = eglGetError();
```

```text
context = EGL_NO_CONTEXT
error   = EGL_BAD_CONFIG
```

## 3.8 Senaryo H - `EGL_BAD_CURRENT_SURFACE`

Calling thread'in current draw veya read surface'i artık geçerli değilse
sonraki EGL işlemi `EGL_BAD_CURRENT_SURFACE` üretebilir. Bu durum native
window yaşam döngüsüne ve implementation'a bağlı bir test düzeneği gerektirir.

## 3.9 Senaryo I - `EGL_BAD_DISPLAY`

```c
EGLBoolean result = eglInitialize(
    EGL_NO_DISPLAY,
    NULL,
    NULL
);

EGLint error = eglGetError();
```

```text
result = EGL_FALSE
error  = EGL_BAD_DISPLAY
```

## 3.10 Senaryo J - `EGL_BAD_SURFACE`

```c
EGLBoolean result = eglDestroySurface(
    dpy,
    (EGLSurface)0
);

EGLint error = eglGetError();
```

```text
result = EGL_FALSE
error  = EGL_BAD_SURFACE
```

## 3.11 Senaryo K - `EGL_BAD_MATCH`

Geçerli context ve surface nesneleri birbirleriyle uyumlu değilse:

```c
EGLBoolean result = eglMakeCurrent(
    dpy,
    incompatible_surface,
    incompatible_surface,
    context
);

EGLint error = eglGetError();
```

```text
result = EGL_FALSE
error  = EGL_BAD_MATCH
```

## 3.12 Senaryo L - `EGL_BAD_PARAMETER`

Bir EGL fonksiyonuna kendi sözleşmesine uymayan parametre değeri verildiğinde
görülebilir. Örneğin zorunlu output pointer'ını `NULL` vermek:

```c
EGLBoolean result = eglChooseConfig(
    dpy,
    NULL,
    NULL,
    0,
    NULL
);

EGLint error = eglGetError();
```

Beklenen hata `EGL_BAD_PARAMETER`'dır.

## 3.13 Senaryo M - `EGL_BAD_NATIVE_PIXMAP`

`eglCreatePixmapSurface` için verilen native pixmap geçersizse ve
implementation bunu algılayabiliyorsa görülür. Bu GBM window-surface
projesinin normal akışında native pixmap kullanılmadığı için platforma bağlı
bir senaryodur.

## 3.14 Senaryo N - `EGL_BAD_NATIVE_WINDOW`

```c
EGLSurface surface = eglCreateWindowSurface(
    dpy,
    config,
    (EGLNativeWindowType)0,
    NULL
);

EGLint error = eglGetError();
```

Implementation geçersiz native handle'ı algılayabiliyorsa:

```text
surface = EGL_NO_SURFACE
error   = EGL_BAD_NATIVE_WINDOW
```

![eglGetError hata senaryoları](image/eglGetError/error-codes.svg)

---

# 4. Hata Durumunun Okununca Sıfırlanması

```c
eglDestroySurface(dpy, (EGLSurface)0);

EGLint first_error = eglGetError();
EGLint second_error = eglGetError();
```

Beklenen mantık:

```text
first_error  = EGL_BAD_SURFACE
second_error = EGL_SUCCESS
```

Araya başka bir EGL fonksiyonu sokmak hata durumunu değiştirebileceğinden
`eglGetError()` başarısız çağrıdan hemen sonra kullanılmalıdır.

Hata durumu thread'e özeldir. Bir thread'de oluşan hata, başka bir thread'in
`eglGetError()` sonucuyla okunmaz.

---

# 5. Hata Adı Yardımcısı

```c
const char *egl_error_string(EGLint error)
{
    switch (error) {
        case EGL_SUCCESS:             return "EGL_SUCCESS";
        case EGL_NOT_INITIALIZED:     return "EGL_NOT_INITIALIZED";
        case EGL_BAD_ACCESS:          return "EGL_BAD_ACCESS";
        case EGL_BAD_ALLOC:           return "EGL_BAD_ALLOC";
        case EGL_BAD_ATTRIBUTE:       return "EGL_BAD_ATTRIBUTE";
        case EGL_BAD_CONTEXT:         return "EGL_BAD_CONTEXT";
        case EGL_BAD_CONFIG:          return "EGL_BAD_CONFIG";
        case EGL_BAD_CURRENT_SURFACE: return "EGL_BAD_CURRENT_SURFACE";
        case EGL_BAD_DISPLAY:         return "EGL_BAD_DISPLAY";
        case EGL_BAD_SURFACE:         return "EGL_BAD_SURFACE";
        case EGL_BAD_MATCH:           return "EGL_BAD_MATCH";
        case EGL_BAD_PARAMETER:       return "EGL_BAD_PARAMETER";
        case EGL_BAD_NATIVE_PIXMAP:   return "EGL_BAD_NATIVE_PIXMAP";
        case EGL_BAD_NATIVE_WINDOW:   return "EGL_BAD_NATIVE_WINDOW";
        default:                      return "UNKNOWN_EGL_ERROR";
    }
}
```

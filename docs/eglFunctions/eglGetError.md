# EGL 1.0: `eglGetError`

```c
EGLint eglGetError(void);
```

## 1. Bu Fonksiyon Ne Yapar?

`eglGetError()`, çağıran thread'in EGL hata durumunu okur ve sonucu bir `EGLint` değeri olarak döndürür.

Bu fonksiyonun önemli özelliği şudur:

```text
Parametre almaz.
```

Çünkü belirli bir `EGLDisplay`, `EGLContext` veya `EGLSurface` nesnesini sorgulamaz. Bunun yerine çağıran thread'in EGL hata durumunu okur.

Kavramsal akış:

```text
EGL fonksiyonu çağrılır
        |
        v
Başarılı / Başarısız
        |
        v
Thread'in EGL error state'i
        |
        v
eglGetError()
        |
        v
EGLint hata kodu
```

---

# 2. Neden Parametre Almaz?

Diğer EGL fonksiyonlarında hangi nesneyle işlem yapılacağı parametrelerle belirtilir.

Örneğin:

```c
eglDestroySurface(dpy, surface);
```

veya:

```c
eglDestroyContext(dpy, ctx);
```

Ancak `eglGetError()` şu soruları sormaz:

```text
Hangi surface'in hatası?
Hangi context'in hatası?
Hangi display'in hatası?
```

Bunun yerine:

```text
Bu thread için EGL'de kayıtlı hata durumu ne?
```

sorusunun cevabını verir.

Kullanım:

```c
EGLint err = eglGetError();
```

---

# 3. Dönüş Tipi

Fonksiyonun dönüş tipi:

```c
EGLint
```

olur.

Yani:

```c
EGLint err = eglGetError();
```

çağrısından sonra `err`, EGL tarafından tanımlanan hata değerlerinden birini içerir.

---

# 4. En Temel Kullanım Mantığı

En doğru kullanım modeli şöyledir:

```text
Önce EGL fonksiyonunu çağır
        |
        v
Fonksiyon başarısız mı?
        |
        +-- Hayır -> devam et
        |
        +-- Evet
              |
              v
         eglGetError()
              |
              v
        Hata kodunu öğren
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

Buradaki mantık:

```text
eglCreateWindowSurface()
        |
        v
EGL_NO_SURFACE döndü
        |
        v
Bir hata oluştu
        |
        v
eglGetError()
        |
        v
Hatanın türü öğrenilir
```

---

# 5. Hata Kodunu Terminale Yazdırma

`eglGetError()` hata adını otomatik olarak ekrana yazmaz.

Fonksiyon yalnızca hata kodunu döndürür.

Örneğin:

```c
EGLint err = eglGetError();

printf("Error code: 0x%X\n", err);
```

Bu kullanım hexadecimal hata kodunu gösterir.

Daha okunabilir bir çıktı için:

```c
if (err == EGL_SUCCESS) {
    printf("EGL_SUCCESS\n");
}
else if (err == EGL_BAD_DISPLAY) {
    printf("EGL_BAD_DISPLAY\n");
}
else if (err == EGL_BAD_SURFACE) {
    printf("EGL_BAD_SURFACE\n");
}
```

Kavramsal akış:

```text
EGL fonksiyonu
    |
    v
Hata oluşur
    |
    v
EGL error state
    |
    v
eglGetError()
    |
    v
EGL_BAD_DISPLAY gibi değer döner
    |
    v
printf()
    |
    v
Terminalde hata adı görülür
```

---

# 6. EGL 1.0'da `eglGetError()` ile Dönen 14 Temel Değer

EGL 1.0 tarafında `eglGetError()` ile dönebilecek 14 temel değer vardır.

> `EGL_SUCCESS` teknik olarak bir hata değildir; "kayıtlı EGL hatası yok" anlamına gelir.

| Değer                      | Anlamı                                                                           |
| --------------------------- | --------------------------------------------------------------------------------- |
| `EGL_SUCCESS`             | Kayıtlı EGL hatası yoktur.                                                     |
| `EGL_NOT_INITIALIZED`     | EGL ilgili display için initialize edilmemiştir veya initialize edilememiştir. |
| `EGL_BAD_ACCESS`          | Bir EGL kaynağına erişim kuralı ihlal edilmiştir.                            |
| `EGL_BAD_ALLOC`           | İstenen işlem için gerekli kaynak ayrılamamıştır.                          |
| `EGL_BAD_ATTRIBUTE`       | Tanınmayan/geçersiz attribute veya attribute değeri kullanılmıştır.        |
| `EGL_BAD_CONTEXT`         | Bir`EGLContext` argümanı geçerli context değildir.                          |
| `EGL_BAD_CONFIG`          | Bir`EGLConfig` argümanı geçerli config değildir.                            |
| `EGL_BAD_CURRENT_SURFACE` | Thread'in current surface'i artık geçerli değildir.                            |
| `EGL_BAD_DISPLAY`         | Bir`EGLDisplay` argümanı geçerli değildir veya uygun durumda değildir.     |
| `EGL_BAD_SURFACE`         | Bir`EGLSurface` argümanı geçerli surface değildir.                          |
| `EGL_BAD_MATCH`           | Argümanlar tek tek geçerli olsa da birbirleriyle uyumlu değildir.              |
| `EGL_BAD_PARAMETER`       | Bir veya daha fazla parametre değeri geçersizdir.                               |
| `EGL_BAD_NATIVE_PIXMAP`   | Native pixmap geçerli değildir.                                                 |
| `EGL_BAD_NATIVE_WINDOW`   | Native window geçerli değildir.                                                 |

Kısa ezber özeti:

```text
EGL_SUCCESS
→ hata yok

EGL_NOT_INITIALIZED
→ EGL hazır değil

EGL_BAD_ACCESS
→ erişim problemi

EGL_BAD_ALLOC
→ kaynak ayrılamadı

EGL_BAD_ATTRIBUTE
→ attribute yanlış

EGL_BAD_CONTEXT
→ context yanlış

EGL_BAD_CONFIG
→ config yanlış

EGL_BAD_CURRENT_SURFACE
→ current surface geçersiz

EGL_BAD_DISPLAY
→ display yanlış

EGL_BAD_SURFACE
→ surface yanlış

EGL_BAD_MATCH
→ argümanlar uyumsuz

EGL_BAD_PARAMETER
→ parametre yanlış

EGL_BAD_NATIVE_PIXMAP
→ native pixmap yanlış

EGL_BAD_NATIVE_WINDOW
→ native window yanlış
```

---

# 7. `EGL_SUCCESS`

`EGL_SUCCESS`, kayıtlı EGL hata durumu olmadığını belirtir.

Örnek:

```c
EGLint err = eglGetError();

if (err == EGL_SUCCESS) {
    printf("EGL_SUCCESS\n");
}
```

Kavramsal gösterim:

```text
EGL_SUCCESS
    |
    v
Kayıtlı EGL hatası yok
```

---

# 8. `EGL_NOT_INITIALIZED`

EGL ilgili display için initialize edilmemişse veya initialize işlemi başarısız olmuşsa görülebilir.

Örnek:

```c
EGLint major;
EGLint minor;

if (!eglInitialize(dpy, &major, &minor)) {
    EGLint err = eglGetError();

    if (err == EGL_NOT_INITIALIZED) {
        printf("EGL_NOT_INITIALIZED\n");
    }
}
```

Flow chart:

```text
EGLDisplay
    |
    v
Initialize edilemedi
    |
    v
eglInitialize() başarısız
    |
    v
eglGetError()
    |
    v
EGL_NOT_INITIALIZED
```

---

# 9. `EGL_BAD_ACCESS`

Bir EGL kaynağına erişim kuralları ihlal edildiğinde oluşabilir.

Örneğin bir context başka bir thread üzerinde current ise, ikinci thread aynı context'i current yapmaya çalışabilir.

```text
Thread A
   |
   +-- Context current

Thread B
   |
   +-- Aynı context'i current yapmaya çalışır
              |
              v
        EGL_BAD_ACCESS
```

Örnek kontrol:

```c
if (!eglMakeCurrent(dpy, surface, surface, ctx)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_ACCESS) {
        printf("EGL_BAD_ACCESS\n");
    }
}
```

---

# 10. `EGL_BAD_ALLOC`

EGL istenen işlem için gerekli kaynakları ayıramazsa görülebilir.

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
        printf("EGL_BAD_ALLOC\n");
    }
}
```

Flow:

```text
eglCreateWindowSurface()
        |
        v
Kaynak gerekli
        |
        v
Kaynak ayrılamadı
        |
        v
EGL_NO_SURFACE
        |
        v
eglGetError()
        |
        v
EGL_BAD_ALLOC
```

---

# 11. `EGL_BAD_ATTRIBUTE`

Geçersiz veya tanınmayan bir EGL attribute'u/attribute değeri kullanıldığında oluşabilir.

Örnek genel model:

```c
const EGLint attrs[] = {
    0x12345678, 1,
    EGL_NONE
};
```

İlgili EGL fonksiyonu başarısız olduktan sonra:

```c
EGLint err = eglGetError();

if (err == EGL_BAD_ATTRIBUTE) {
    printf("EGL_BAD_ATTRIBUTE\n");
}
```

---

# 12. `EGL_BAD_CONTEXT`

Bir EGL fonksiyonuna geçerli olmayan `EGLContext` verilirse oluşabilir.

Örneğin:

```c
if (!eglDestroyContext(dpy, invalid_context)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_CONTEXT) {
        printf("EGL_BAD_CONTEXT\n");
    }
}
```

Flow chart:

```text
Geçersiz context
      |
      v
eglDestroyContext()
      |
      v
EGL_FALSE
      |
      v
eglGetError()
      |
      v
EGL_BAD_CONTEXT
```

---

# 13. `EGL_BAD_CONFIG`

Geçersiz bir `EGLConfig` kullanıldığında oluşabilir.

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
        printf("EGL_BAD_CONFIG\n");
    }
}
```

Flow:

```text
Geçersiz EGLConfig
        |
        v
eglCreateWindowSurface()
        |
        v
EGL_NO_SURFACE
        |
        v
eglGetError()
        |
        v
EGL_BAD_CONFIG
```

---

# 14. `EGL_BAD_CURRENT_SURFACE`

Thread üzerinde current olan surface artık geçerli değilse bazı EGL işlemlerinde görülebilir.

Kavramsal akış:

```text
Thread
  |
  +-- Current EGLSurface
           |
           X
      Surface geçersiz hale geldi
           |
           v
EGL_BAD_CURRENT_SURFACE
```

Bu hata current surface state'i ile ilgilidir.

---

# 15. `EGL_BAD_DISPLAY`

Geçersiz veya uygun şekilde initialize edilmemiş display ile işlem yapılırsa görülebilir.

Örnek:

```c
if (!eglInitialize(dpy, &major, &minor)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_DISPLAY) {
        printf("EGL_BAD_DISPLAY\n");
    }
}
```

Flow:

```text
Geçersiz/uygunsuz dpy
        |
        v
EGL fonksiyonu
        |
        v
İşlem başarısız
        |
        v
eglGetError()
        |
        v
EGL_BAD_DISPLAY
```

---

# 16. `EGL_BAD_SURFACE`

Bir EGL fonksiyonuna geçerli olmayan `EGLSurface` verilirse oluşabilir.

Örneğin grubumuzdaki `eglDestroySurface()`:

```c
if (!eglDestroySurface(dpy, invalid_surface)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_SURFACE) {
        printf("EGL_BAD_SURFACE\n");
    }
}
```

Flow:

```text
Geçersiz EGLSurface
        |
        v
eglDestroySurface()
        |
        v
EGL_FALSE
        |
        v
eglGetError()
        |
        v
EGL_BAD_SURFACE
```

---

# 17. `EGL_BAD_MATCH`

Bu hata diğerlerinden biraz farklıdır.

Argümanlar tek tek geçerli olabilir, ancak birbirleriyle uyumlu olmayabilir.

Örneğin:

```text
EGLContext geçerli
EGLSurface geçerli
        |
        v
Ama birbirleriyle uyumsuz
        |
        v
EGL_BAD_MATCH
```

Örnek:

```c
if (!eglMakeCurrent(dpy, surface, surface, ctx)) {
    EGLint err = eglGetError();

    if (err == EGL_BAD_MATCH) {
        printf("EGL_BAD_MATCH\n");
    }
}
```

---

# 18. `EGL_BAD_PARAMETER`

Bir veya daha fazla parametre değeri ilgili EGL fonksiyonu açısından geçersiz olduğunda oluşabilir.

```c
EGLint err = eglGetError();

if (err == EGL_BAD_PARAMETER) {
    printf("EGL_BAD_PARAMETER\n");
}
```

Kavramsal anlamı:

```text
Parametre değeri uygun değil
        |
        v
EGL fonksiyonu başarısız
        |
        v
EGL_BAD_PARAMETER
```

---

# 19. `EGL_BAD_NATIVE_PIXMAP`

Geçerli olmayan native pixmap handle'ı kullanıldığında oluşur.

Bu direct-to-display GBM projesinde native pixmap normal akışta kullanılmamaktadır.

```text
Bu proje:
GBM native window -> kullanılıyor
Native pixmap     -> kullanılmıyor
```

Bu nedenle normal proje akışında beklenen hata kodlarından biri değildir.

---

# 20. `EGL_BAD_NATIVE_WINDOW`

Native window geçerli değilse oluşabilir.

Bu projede `eglCreateWindowSurface()` içindeki native window rolünü:

```c
(EGLNativeWindowType)gbm_surface
```

üstlenir.

Örnek:

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
        printf("EGL_BAD_NATIVE_WINDOW\n");
    }
}
```

Flow:

```text
Geçersiz native window
        |
        v
eglCreateWindowSurface()
        |
        v
EGL_NO_SURFACE
        |
        v
eglGetError()
        |
        v
EGL_BAD_NATIVE_WINDOW
```

---

# 21. Hata Adlarını Yazdıran Yardımcı Fonksiyon

Terminalde doğrudan hata adlarını görmek için yardımcı fonksiyon kullanılabilir:

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

Örnek beklenen terminal çıktıları:

```text
EGL error: EGL_SUCCESS (...)
```

veya:

```text
EGL error: EGL_BAD_SURFACE (...)
```

veya:

```text
EGL error: EGL_BAD_CONTEXT (...)
```

Bu çıktılar gerçek test yapılmadıysa **örnek/beklenen çıktı** olarak sunulmalıdır.

---

# 22. Genel Flow Chart

```text
                 EGL FUNCTION
                      |
                      v
               Return value kontrol
                      |
             +--------+--------+
             |                 |
             v                 v
          Başarılı          Başarısız
             |                 |
             v                 v
          Devam et         eglGetError()
                               |
                               v
                         EGLint hata kodu
                               |
             +-----------------+-----------------+
             |                 |                 |
             v                 v                 v
      EGL_BAD_SURFACE   EGL_BAD_CONTEXT   EGL_BAD_DISPLAY
             ...               ...               ...
```

---

# EGL 1.0 Fonksiyon İncelemesi: `eglGetCurrentContext`

```c
EGLContext eglGetCurrentContext(void);
```

`eglGetCurrentContext`, çağrıyı yapan thread üzerinde o anda aktif olan `EGLContext` handle değerini döndürür. Fonksiyon parametre almaz; EGL'in thread-local durumunu okur. Bu yüzden aktif context varsa o context döner, aktif context yoksa `EGL_NO_CONTEXT` döner.

Fonksiyon parametre almadığı için senaryolar parametre değişimine göre değil, çağıran thread'in current context durumuna göre oluşturulmuştur:

| Senaryo | Test Edilen Durum | Beklenen Sonuç |
|---|---|---|
| Senaryo A | `eglMakeCurrent` ile context aktif hale getirildikten sonra `eglGetCurrentContext()` çağrılır. | Fonksiyon, aktif context handle değerini döndürür. |
| Senaryo B | Current context `EGL_NO_CONTEXT` ile thread'den ayrıldıktan sonra `eglGetCurrentContext()` çağrılır. | Fonksiyon `EGL_NO_CONTEXT` döndürür. |

## Fonksiyonun Temel Mantığı

![eglGetCurrentContext thread-local context modeli](image/eglGetCurrentContext/egl_get_current_context_tls_model.svg)

`eglGetCurrentContext` global bir context listesinde arama yapmaz. Sadece çağrıldığı thread'in EGL state bilgisini kontrol eder. Aynı programda başka bir thread üzerinde aktif context bulunması, bu thread için sonucu değiştirmez.

Her çağrı yalnızca çağrıldığı thread'in current context bilgisini okur. Fonksiyon state değiştirmez; current context varsa ilgili `EGLContext` handle değerini, yoksa `EGL_NO_CONTEXT` değerini döndürür.

## Parametre Durumu

Spesifikasyona göre fonksiyonun parametresi yoktur.

| Durum | Açıklama |
|---|---|
| Parametre | Yok |
| Girdi nereden gelir? | Çağıran thread'in current EGL state bilgisinden |
| Çıktı | `EGLContext` veya `EGL_NO_CONTEXT` |

`eglGetCurrentContext` doğrudan `NativeWindowType`, `NativePixmapType`, attribute listesi, buffer tipi veya surface parametresi almaz. Ancak dönen context, daha önce `eglMakeCurrent(display, draw, read, context)` çağrısıyla bir draw/read surface çiftine bağlanmış olabilir.

## Senaryo A: Aktif Context Varken

Kaynak dosya: `void_param/scenario_a.c`

Bu senaryoda EGL/DRM/GBM ortamı hazırlanır. Ardından `eglMakeCurrent` ile oluşturulan context current yapılır. Sonrasında `eglGetCurrentContext()` çağrılır ve dönen handle, programın oluşturduğu `state.egl_context` ile karşılaştırılır.

![Senaryo A aktif context akışı](image/eglGetCurrentContext/scenario_a_active_context.svg)

### Kod Akışı

```c
make_current_checked(&state,
                     state.egl_surface,
                     state.egl_surface,
                     state.egl_context,
                     "Context aktif edilemedi...");

EGLContext current = eglGetCurrentContext();

if (current == state.egl_context) {
    printf("BASARILI: aktif context dogru sekilde donduruldu.\n");
}
```

### Beklenen Terminal Çıktısı

Gerçek handle değeri çalıştırılan sisteme göre değişir. Bu nedenle `0x...` kısmı örnek gösterimdir.

```text
==================================================
SENARYO A: Aktif bir context varken eglGetCurrentContext cagirimi
==================================================
1. eglMakeCurrent basariyla cagirildi ve context aktif edildi.
2. BASARILI: eglGetCurrentContext aktif olan context'i (0x...) dogru sekilde dondurdu.
```

Current context doğrulandığı için daha sonraki OpenGL ES komutları bu context üzerinden çalışabilir.

## Senaryo B: Aktif Context Yokken

Kaynak dosya: `void_param/scenario_b.c`

Bu senaryoda context ilk başta aktif edilir. Daha sonra aşağıdaki çağrı ile context thread'den ayrılır:

```c
eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
```

Bu detach işleminden sonra `eglGetCurrentContext()` çağrıldığında artık current context olmadığı için fonksiyonun `EGL_NO_CONTEXT` döndürmesi beklenir.

![Senaryo B context yok akışı](image/eglGetCurrentContext/scenario_b_no_context.svg)

### Kod Akışı

```c
make_current_checked(&state,
                     EGL_NO_SURFACE,
                     EGL_NO_SURFACE,
                     EGL_NO_CONTEXT,
                     "Context detach edilemedi...");

EGLContext current = eglGetCurrentContext();

if (current == EGL_NO_CONTEXT) {
    printf("SONUC: eglGetCurrentContext() EGL_NO_CONTEXT dondurdu.\n");
}
```

### Beklenen Terminal Çıktısı

```text
==================================================
SENARYO B: Aktif bir context YOKKEN eglGetCurrentContext cagirimi
==================================================
1. eglMakeCurrent ile aktif context kapatiliyor (EGL_NO_CONTEXT geciliyor).
2. eglGetCurrentContext cagirildi.
-> SONUC: Beklendigi gibi eglGetCurrentContext() EGL_NO_CONTEXT dondurdu.
```

Bu senaryoda `eglGetCurrentContext()` sonucu `EGL_NO_CONTEXT` olduğu için çağıran thread üzerinde current context yoktur.

## Sonuçların Karşılaştırması

| Kontrol Noktası | Senaryo A | Senaryo B |
|---|---|---|
| `eglMakeCurrent` sonrası thread state | Context aktif | Context detach edilmiş |
| `eglGetCurrentContext()` sonucu | `state.egl_context` | `EGL_NO_CONTEXT` |
| Yeni GL komutları için current context mevcut mu? | Evet | Hayır |
| Testin gösterdiği ana fikir | Fonksiyon aktif context'i doğru döndürür. | Aktif context yoksa güvenli biçimde `EGL_NO_CONTEXT` döner. |

## Hata Davranışı

`eglGetCurrentContext` bir getter fonksiyonudur. State değiştirmez, parametre doğrulaması yapmaz ve current context olmaması tek başına EGL hatası değildir.

| Durum | Dönüş Değeri | Açıklama |
|---|---|---|
| Çağıran thread'de current context varsa | Aktif `EGLContext` | Dönen handle beklenen context ile karşılaştırılabilir. |
| Çağıran thread'de current context yoksa | `EGL_NO_CONTEXT` | Bu durum tek başına hata değildir. |
| Başka thread'de current context varsa | Bu thread'in current context durumuna göre sonuç döner | Başka thread'deki context bu sonucu değiştirmez. |

`EGL_BAD_MATCH`, `EGL_BAD_NATIVE_WINDOW` veya `EGL_BAD_SURFACE` gibi hatalar bu getter fonksiyonundan değil; genellikle `eglMakeCurrent`, `eglCreateWindowSurface` veya `eglSwapBuffers` gibi surface/context ilişkisini kuran çağrılardan kaynaklanır.

## Güvenli Kullanım Örneği

```c
#include <EGL/egl.h>
#include <stdio.h>

void perform_safe_rendering(EGLDisplay dpy,
                            EGLSurface draw,
                            EGLSurface read,
                            EGLContext ctx) {
    if (!eglMakeCurrent(dpy, draw, read, ctx)) {
        EGLint err = eglGetError();
        fprintf(stderr, "eglMakeCurrent basarisiz oldu: 0x%04X\n", err);
        return;
    }

    EGLContext current_ctx = eglGetCurrentContext();

    if (current_ctx == EGL_NO_CONTEXT) {
        fprintf(stderr, "Aktif context yok; cizim guvenli degil.\n");
        return;
    }

    if (current_ctx != ctx) {
        fprintf(stderr, "Aktif context beklenen context ile eslesmiyor.\n");
        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        return;
    }

    printf("Context dogrulandi. Cizim yapilabilir.\n");

    /* glClear, glDrawArrays, eglSwapBuffers ... */

    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}
```

## Pratik Özet

- `eglGetCurrentContext` parametre almaz; fonksiyon imzasındaki `void` bunu gösterir.
- Fonksiyonun sonucu çağıran thread'in EGL state bilgisine bağlıdır.
- Aktif context varsa gerçek `EGLContext` handle değeri döner.
- Aktif context yoksa `EGL_NO_CONTEXT` döner.
- Senaryo A, current context varsa doğru handle değerinin döndüğünü gösterir.
- Senaryo B, current context yoksa `EGL_NO_CONTEXT` döndüğünü gösterir.

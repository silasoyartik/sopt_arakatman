# EGL 1.0 Fonksiyon İncelemesi: `eglGetCurrentContext`

```c
EGLContext eglGetCurrentContext(void);
```

`eglGetCurrentContext`, çağrıyı yapan thread üzerinde o anda aktif olan `EGLContext` handle değerini döndürür. Fonksiyon parametre almaz; EGL'in thread-local durumunu okur. Bu yüzden aktif context varsa o context döner, aktif context yoksa `EGL_NO_CONTEXT` döner.

Bu çalışmada fonksiyonun `void` parametreli yapısı iki pratik durum üzerinden incelenmiştir:

| Senaryo | Test Edilen Durum | Beklenen Sonuç |
|---|---|---|
| Senaryo A | `eglMakeCurrent` ile context aktif hale getirildikten sonra `eglGetCurrentContext()` çağrılır. | Fonksiyon, aktif context handle değerini döndürür ve OpenGL ES çizimi yapılabilir. |
| Senaryo B | Context önce aktif edilir, sonra `EGL_NO_CONTEXT` ile thread'den ayrılır. | Fonksiyon `EGL_NO_CONTEXT` döndürür; aktif context olmadığı için yeni çizim yapılmaz. |

## Fonksiyonun Temel Mantığı

![eglGetCurrentContext thread-local context modeli](image/eglGetCurrentContext/egl_get_current_context_tls_model.svg)

`eglGetCurrentContext` global bir context listesinde arama yapmaz. Sadece çağrıldığı thread'in EGL state bilgisini kontrol eder. Aynı programda başka bir thread üzerinde aktif context bulunması, bu thread için sonucu değiştirmez.

```text
Çağıran thread üzerinde aktif EGLContext varsa:
    eglGetCurrentContext() -> aktif EGLContext handle değeri

Çağıran thread üzerinde aktif EGLContext yoksa:
    eglGetCurrentContext() -> EGL_NO_CONTEXT
```

Bu davranış özellikle render kodlarında önemlidir. Çünkü OpenGL ES komutları, doğrudan fonksiyonlara verilen context ile değil, çağrıyı yapan thread'e current yapılmış context ile çalışır.

## Parametre İncelemesi

Spesifikasyona göre fonksiyonun parametresi yoktur.

| Parametre Biçimi | Anlamı | Testteki Etkisi |
|---|---|---|
| `void` | Fonksiyon argüman almaz. | Sonuç, tamamen çağıran thread'in mevcut EGL state durumuna bağlıdır. |

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
3. Cizim islemi baslatiliyor (Ekranda renkli bir ucgen gormelisiniz)...
4. Cizim tamamlandi, pencere 3 saniye acik kalacak.
```

### Görsel Çıktı

![Senaryo A beklenen ekran çıktısı](image/eglGetCurrentContext/expected_output_scenario_a.svg)

Context aktif olduğu için OpenGL ES komutları geçerli context üzerinde çalışır. Program önce arka planı koyu yeşilimsi renge temizler, sonra kırmızı, yeşil ve mavi köşelere sahip bir üçgen çizer. Son adımda `eglSwapBuffers` çağrısı ile çizilen görüntü ekrana taşınır.

## Senaryo B: Aktif Context Yokken

Kaynak dosya: `void_param/scenario_b.c`

Bu senaryoda context ilk başta aktif edilir ve ekran kırmızıya temizlenir. Daha sonra aşağıdaki çağrı ile context thread'den ayrılır:

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

>>> Bu senaryoda aktif bir context olmadigi icin ucgen cizilemedi ve ekrana HICBIR SEY CIZILEMEDI. <<<

3. Gorsel ispat icin pencere 3 saniye acik tutuluyor. (Sadece kirmizi arka plan goreceksiniz, ucgen yok!)
```

### Görsel Çıktı

![Senaryo B beklenen ekran çıktısı](image/eglGetCurrentContext/expected_output_scenario_b.svg)

Bu senaryoda `eglGetCurrentContext()` sonucu `EGL_NO_CONTEXT` olduğu için üçgen çizilmez. Program, görsel ayrımı kolaylaştırmak için detach öncesinde ekranı kırmızıya temizler. Daha sonra sadece mevcut buffer'ı göstermek amacıyla context geçici olarak tekrar bağlanır ve `eglSwapBuffers` yapılır; bu aşamada yeni çizim komutu verilmez. Bu yüzden beklenen görüntü sadece kırmızı arka plandır.

## Sonuçların Karşılaştırması

| Kontrol Noktası | Senaryo A | Senaryo B |
|---|---|---|
| `eglMakeCurrent` sonrası thread state | Context aktif | Context detach edilmiş |
| `eglGetCurrentContext()` sonucu | `state.egl_context` | `EGL_NO_CONTEXT` |
| OpenGL ES çizimi yapılabilir mi? | Evet | Hayır, current context yoktur |
| Görsel çıktı | Koyu arka plan üzerinde renkli üçgen | Sadece kırmızı arka plan |
| Testin gösterdiği ana fikir | Fonksiyon aktif context'i doğru döndürür. | Aktif context yoksa güvenli biçimde `EGL_NO_CONTEXT` döner. |

## Hata Davranışı

`eglGetCurrentContext` bir getter fonksiyonudur. State değiştirmez, parametre doğrulaması yapmaz ve normal kullanımda yeni bir EGL hatası üretmesi beklenmez.

| Durum | Dönüş Değeri | Açıklama |
|---|---|---|
| EGL initialize edilmemişse | `EGL_NO_CONTEXT` | Current context olmadığı için boş context döner. |
| Mevcut thread'de context yoksa | `EGL_NO_CONTEXT` | Başka thread'deki context bu sonucu değiştirmez. |
| `eglMakeCurrent` başarılı olduysa | Aktif `EGLContext` | Dönen handle beklenen context ile karşılaştırılabilir. |

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
- Senaryo A, context doğrulamasını ve çizilebilir durumu gösterir.
- Senaryo B, context detach edildikten sonra yeni çizimin yapılamayacağını gösterir.

## Teslim Notu

Bu rapordaki SVG görselleri, kodun beklenen davranışını şematik olarak anlatmak için hazırlanmıştır. Donanım üzerinde alınmış gerçek ekran görüntüleri eklenmek istenirse aşağıdaki dosya adları kullanılabilir:

```text
assets/screenshots/scenario_a_triangle.png
assets/screenshots/scenario_b_red_background.png
```

Gerçek görüntüler EGL/GBM destekli Linux ortamında, `/dev/dri/card0` erişimi olan bir cihazda alınmalıdır. Bu çalışma Windows ortamında düzenlendiği için gerçek DRM/GBM ekran çıktısı yakalanmamıştır.

# EGL 1.0: `eglGetCurrentContext`

```c
EGLContext eglGetCurrentContext(void);
```

`eglGetCurrentContext`, çağrıyı yapan thread (iş parçacığı) için o anda aktif olan (state machine'e bağlanmış) `EGLContext` nesnesinin handle (tanıtıcı) değerini döndürür. Bu fonksiyon, herhangi bir argüman almadan doğrudan thread-local bellek üzerinden durumu okuduğu için sistem durumunu sorgulamada en düşük maliyetli EGL fonksiyonlarından biridir.

## Kavramsal Akış

```text
İşletim Sistemi (OS) - Thread Local Storage (TLS)
 └── İş Parçacığı (Thread ID: 0x1234)
      ├── Aktif EGL State Makinesi
      │    └── EGLContext (Current Context - eglGetCurrentContext() ile sorgulanır)
      │         ├── EGLDisplay (Örn: DRM/GBM Display veya X11 Display)
      │         ├── EGLSurface (DRAW - Çizim Hedefi)
      │         │    └── NativeWindowType (Örn: gbm_surface veya Window)
      │         └── EGLSurface (READ - Okuma Hedefi)
      │              └── NativePixmapType / NativeWindowType
      └── [Alternatif] EGL_NO_CONTEXT (Hiçbir context bağlanmamış durumu)
```

## Parametreler

Bu fonksiyon spesifikasyona göre hiçbir parametre almaz (`void`).

### Parametre Yok

| Değer                   | Sonuç                                                                                                       |
| ------------------------ | ------------------------------------------------------------------------------------------------------------ |
| `void` (Parametre yok) | Thread'e bağlı geçerli bir`EGLContext` varsa o döndürülür, aksi takdirde `EGL_NO_CONTEXT` döner. |

*Not: Fonksiyon parametre almadığı için NativeWindowType veya NativePixmapType gibi platforma bağımlı tipleri doğrudan etkilemez veya kabul etmez. Ancak dönen context'in dolaylı olarak bağlı olduğu yüzeyler (surface) X11'de Window, DRM/GBM sistemlerinde ise `gbm_surface` gibi yapılara tekabül eder.*

## Geçerli Attribute Listesi ve Tampon (Buffer) Tipleri

`eglGetCurrentContext` fonksiyonu konfigürasyon veya flag dizisi almaz. Ancak dönen context'in durumunu değerlendirirken tampon yapıları büyük önem taşır:

| Attribute / Durum              | Tip            | Anlam                                                                                                                                                      |
| ------------------------------ | -------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Konfigürasyon Listesi         | Yok            | Fonksiyon attribute dizisi kabul etmez.                                                                                                                    |
| Back Buffered (Çift Tamponlu) | Surface Durumu | Dönen context bir Window yüzeyine bağlıysa, çizimler arka tampona (back buffer) yapılır ve`eglSwapBuffers` beklenir.                              |
| Single Buffered (Tek Tamponlu) | Surface Durumu | Dönen context bir Pixmap veya Pbuffer (off-screen) yüzeyine bağlıysa, çizimler anında gerçekleşebilir ve frame eşzamanlaması farklı yönetilir. |

## Ayrıntılar ve Yaşam Döngüsü

**Thread Güvenliği (Thread Safety):**
Bir `EGLContext` aynı anda yalnızca bir thread üzerinde current olabilir. Context başka bir thread üzerinde current durumdaysa mevcut bağlantı kaldırılmadan farklı bir thread tarafından kullanılamaz. `eglGetCurrentContext`, yalnızca çağıran thread'in durumunu okur ve diğer thread'leri bloklamaz.

**Eşzamanlama (Synchronization):**
Dönen context üzerinden çizim komutları gönderilirken, OpenGL ile Native API (Örn: DRM veya X11 2D render komutları) arasında eşzamanlama gerekiyorsa `eglWaitGL` (OpenGL komutlarının bitmesini bekler) ve `eglWaitNative` (Native render komutlarının bitmesini bekler) kullanılmalıdır. Native objeler (Window/Pixmap) üzerinde OpenGL harici bir API işlem yapacaksa, context üzerinde `glFinish()` çağırmak da alternatif bir donanım bazlı eşzamanlama garantisidir.

## Hata Matrisi

Khronos spesifikasyonlarına göre `eglGetCurrentContext`, state machine üzerinde hiçbir değişiklik yapmayan **yan etkisiz (side-effect free)** bir okuma (getter) operasyonudur. Başarısızlık durumunda dahi EGL error flag'lerini modifiye etmez.

| Durum                                                   | Sonuç (EGL Hata Kodu)                                      |
| ------------------------------------------------------- | ----------------------------------------------------------- |
| EGL hiç initialize edilmediyse                         | `EGL_NO_CONTEXT` döner, `eglGetError()` değişmez.    |
| Başka thread'de context aktif, mevcut thread'de boşsa | `EGL_NO_CONTEXT` döner, `eglGetError()` değişmez.    |
| `eglMakeCurrent` başarıyla çalıştıysa           | Aktif`EGLContext` handle'ı döner, hata kodu değişmez. |

> `eglGetCurrentContext`, `EGL_BAD_MATCH` veya `EGL_BAD_NATIVE_WINDOW` gibi hata durumları üretmez. Current context bulunmaması `EGL_NO_CONTEXT` dönüş değeriyle belirtilir.

## Güvenli Kullanım Örneği

Aşağıdaki örnekte, fonksiyonun DRM/GBM veya gömülü sistemler fark etmeksizin nasıl güvenli kullanılacağı gösterilmiştir:

```c
#include <EGL/egl.h>
#include <stdio.h>

// Context'in doğru şekilde current olup olmadığını doğrulayan yardımcı fonksiyon
void perform_safe_rendering(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) {
    // 1. Context'i aktif yapmayi dene
    if (!eglMakeCurrent(dpy, draw, read, ctx)) {
        EGLint err = eglGetError();
        fprintf(stderr, "Hata: eglMakeCurrent basarisiz oldu. Hata Kodu: 0x%04X\n", err);
        return;
    }

    // 2. State machine'in gercekten context'i kabul ettigini dogrula (Cifte Kontrol)
    EGLContext current_ctx = eglGetCurrentContext();
  
    if (current_ctx == EGL_NO_CONTEXT) {
        // eglMakeCurrent basarili donse bile spesifikasyon disi donanim surucusu hatasi
        fprintf(stderr, "Hata: Aktif context bulunamadi.\n");
        // Guvenli cikis (Memory leak ve undefined behavior onleme)
        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        return;
    }

    if (current_ctx != ctx) {
        fprintf(stderr, "Hata: Aktif context (%p), beklenen context (%p) ile eslesmiyor.\n",
                (void*)current_ctx, (void*)ctx);
        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        return;
    }

    // 3. Guvenli bolge: Context kesin olarak bu thread'e bagli
    printf("Context basariyla dogrulandi (%p). Cizim islemleri basliyor...\n", (void*)current_ctx);
  
    // glClear(GL_COLOR_BUFFER_BIT) vs...
  
    // 4. Gerekiyorsa açık eşzamanlama uygula
    // eglWaitGL();
  
    // 5. Cizim bittikten sonra context'i guvenli sekilde ayir (Cleanup)
    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}
```

## Bölüm Özeti

- **Yan Etki:** Fonksiyon current context bilgisini okur ve EGL hata durumunu değiştirmez.
- **Thread-Local Davranış:** Her thread yalnızca kendi current context'ini sorgular.
- **Doğrulama:** `eglMakeCurrent` sonrasında beklenen context'in current olup olmadığını denetlemek için kullanılabilir.
- **Platform Bağımsızlığı:** Dönen `EGLContext`, native pencereleme sisteminden bağımsız bir EGL handle'ıdır.


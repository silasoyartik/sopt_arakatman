# EGL 1.0: `eglSwapBuffers`

```c
EGLBoolean eglSwapBuffers(EGLDisplay dpy,
                          EGLSurface surface);
```

`eglSwapBuffers`, EGL 1.0'da rendering sonrası color buffer'ı native window'a post etmek için kullanılır.

Kısa özet:

- Window surface için: color buffer native window'a post edilir.
- Pbuffer surface için: etkisi yoktur.
- Pixmap surface için: etkisi yoktur.
- EGL 1.0'da `surface`, çağıran thread'in current context'ine bağlı olmalıdır.

## Kavramsal Akış

Window surface için:

```text
OpenGL ES draw commands
        |
        v
EGL window surface back/color buffer
        |
        v
eglSwapBuffers
        |
        v
native window'a post/copy
```

Somut olarak bir frame boyunca ön ve arka buffer'ı şöyle düşünebiliriz:

```text
Frame N çizilirken:
  front buffer -> ekranda önceki frame görünür
  back buffer  -> OpenGL ES yeni frame'i çizer

eglSwapBuffers çağrısı:
  back buffer'daki tamamlanmış renk görüntüsü native window'a post edilir

Çağrıdan sonra:
  uygulama önceki color buffer içeriğinin korunduğunu varsayamaz
```

“Swap” adı her implementation'ın iki bellek adresini mutlaka değiştirdiği
anlamına gelmez. EGL 1.0'ın gözlemlenebilir garantisi color buffer'ın native
window'a post edilmesidir; driver copy, buffer exchange veya page flip benzeri
bir yöntem seçebilir.

GBM + DRM/KMS için daha uzun zincir gerekir:

```text
OpenGL ES
   |
   v
EGLSurface
   |
   v
GBM surface
   |
   v
eglSwapBuffers
   |
   v
GBM front BO hazır olur
   |
   v
gbm_surface_lock_front_buffer
   |
   v
DRM framebuffer
   |
   v
drmModeSetCrtc / drmModePageFlip
   |
   v
monitor
```

## Parametreler

### `dpy`

| Değer                                | Sonuç                                                                  |
| ------------------------------------- | ----------------------------------------------------------------------- |
| Geçerli ve initialized`EGLDisplay` | Diğer parametreler de geçerliyse çağrı çalışır.                |
| `EGL_NO_DISPLAY`                    | Başarısız. Genel EGL hata modeliyle`EGL_BAD_DISPLAY` beklenir.     |
| Geçersiz display handle              | Başarısız. Genel EGL hata modeliyle`EGL_BAD_DISPLAY` beklenir.     |
| Initialize edilmemiş display         | Başarısız. Genel EGL hata modeliyle`EGL_NOT_INITIALIZED` beklenir. |

### `surface`

| Değer                                                 | Sonuç                                    |
| ------------------------------------------------------ | ----------------------------------------- |
| Current context'e bağlı geçerli window surface      | Color buffer native window'a post edilir. |
| Current context'e bağlı geçerli pbuffer surface     | EGL 1.0'a göre etkisi yoktur.            |
| Current context'e bağlı geçerli pixmap surface      | EGL 1.0'a göre etkisi yoktur.            |
| Geçerli ama current context'e bağlı olmayan surface | Başarısız,`EGL_BAD_SURFACE`.         |
| `EGL_NO_SURFACE`                                     | Başarısız,`EGL_BAD_SURFACE`.         |
| Geçersiz surface                                      | Başarısız,`EGL_BAD_SURFACE`.         |
| Yok edilmiş ve artık current olmayan surface         | Başarısız,`EGL_BAD_SURFACE`.         |
| Native window'u geçersiz window surface               | Başarısız,`EGL_BAD_NATIVE_WINDOW`.   |

Current durumdayken `eglDestroySurface` ile silinmek üzere işaretlenmiş bir
surface hemen yok olmaz; current kaldığı sürece geçerlidir. İlgili thread'deki
sonraki geçerli `eglMakeCurrent` ile bağlantı değiştiğinde gerçek silme
tamamlanır ve bundan sonraki swap girişimi `EGL_BAD_SURFACE` olur.

## EGL 1.0 Current Surface Şartı

EGL 1.0 spec'i için önemli kural:

```text
surface, çağıran thread'in current context'ine bağlı olmalıdır.
```

Doğru sıra:

```C
eglMakeCurrent(dpy, surface, surface, ctx);

/* OpenGL ES çizimleri */

eglSwapBuffers(dpy, surface);
```

Yanlış sıra:

```c
/* surface current yapılmadı */
eglSwapBuffers(dpy, surface);
```

Beklenen hata:

```text
EGL_FALSE
EGL_BAD_SURFACE
```

## Surface Tiplerine Göre Davranış

### Window Surface

```c
eglSwapBuffers(dpy, window_surface);
```

Başarı için:

- `dpy` initialized olmalı.
- `window_surface` geçerli olmalı.
- `window_surface` current context'e bağlı olmalı.
- Native window hala geçerli olmalı.

Başarı sonucu:

- `EGL_TRUE` döner.
- Color buffer native window'a post edilir.
- Surface color buffer içeriği swap sonrası tanımsız olur.
- Current context için implicit `glFlush` yapılır.

EGL 1.0 metni “color buffer copied to the native window” şeklinde tarif eder. Gerçek implementation bunu copy, page flip, buffer exchange veya platform-specific başka bir yolla gerçekleştirebilir; uygulama EGL 1.0 seviyesinde bu mekanizmaya güvenmemelidir.

### Pbuffer Surface

```c
eglSwapBuffers(dpy, pbuffer_surface);
```

EGL 1.0'a göre etkisi yoktur.

Pbuffer offscreen surface'tir:

```text
pbuffer -> native visible window yok
```

Bu yüzden `eglSwapBuffers` fiziksel görüntü üretmez. Pbuffer içeriğini kullanmak istiyorsan tipik yollar:

- aynı context içinde texture/copy/readback akışı
- `glReadPixels`
- başka EGL/OpenGL ES mekanizmaları

EGL 1.0 açısından `eglSwapBuffers` pbuffer için görüntü post etmez.

### Pixmap Surface

```c
eglSwapBuffers(dpy, pixmap_surface);
```

EGL 1.0'a göre etkisi yoktur.

Pixmap'e copy yapmak için ilişkili fonksiyon `eglCopyBuffers`'tır:

```c
eglCopyBuffers(dpy, surface, native_pixmap);
```

`eglSwapBuffers`, pixmap hedefli copy fonksiyonu değildir.

## Hata Matrisi

| Durum                                            | Sonuç                                             |
| ------------------------------------------------ | -------------------------------------------------- |
| Window surface current ve native window geçerli | `EGL_TRUE`; post yapılır.                      |
| Pbuffer surface current                          | `EGL_TRUE` dönebilir; etkisi yoktur.            |
| Pixmap surface current                           | `EGL_TRUE` dönebilir; etkisi yoktur.            |
| Surface current değil                           | `EGL_FALSE`, `EGL_BAD_SURFACE`.                |
| Surface geçersiz                                | `EGL_FALSE`, `EGL_BAD_SURFACE`.                |
| `surface == EGL_NO_SURFACE`                    | `EGL_FALSE`, `EGL_BAD_SURFACE`.                |
| Native window artık geçersiz                   | `EGL_FALSE`, `EGL_BAD_NATIVE_WINDOW`.          |
| `dpy` geçersiz                                | `EGL_FALSE`, tipik hata `EGL_BAD_DISPLAY`.     |
| `dpy` initialized değil                       | `EGL_FALSE`, tipik hata `EGL_NOT_INITIALIZED`. |

## Swap Sonrası Color Buffer Durumu

EGL 1.0 spec'i başarılı `eglSwapBuffers` sonrası surface color buffer içeriğinin tanımsız olduğunu söyler.

Yani şu varsayım yanlıştır:

```c
eglSwapBuffers(dpy, surface);

/* Yanlış varsayım: eski color buffer içeriği hala korunuyor */
```

Doğru pratik:

```c
/* Her frame'de gerekli içeriği yeniden çiz */
glClear(...);
draw_scene();
eglSwapBuffers(dpy, surface);
```

EGL 1.0 seviyesinde buffer preservation garantisi yoktur.

## Örtük `glFlush` Davranışı

EGL 1.0 posting semantics:

```text
Eğer dpy ve surface çağıran thread'in current context'ine aitse,
eglSwapBuffers implicit glFlush yapar.
```

Bu `glFinish` değildir.

| Fonksiyon          | Anlam                                                                                                |
| ------------------ | ---------------------------------------------------------------------------------------------------- |
| `glFlush`        | Komutların GPU'ya gönderilmesini başlatır; tamamlanmasını beklemek zorunda değildir.          |
| `glFinish`       | Önceki GL komutlarının tamamlanmasını bekler.                                                   |
| `eglSwapBuffers` | Posting sırasında implicit`glFlush` yapar; swap/post tamamlanması implementation'a bağlıdır. |

EGL 1.0 metni, sonraki OpenGL ES komutlarının hemen verilebileceğini ama posting bitene kadar yürütülmeyebileceğini belirtir. Window surface için bu zamanlama tipik olarak vertical retrace ile ilişkilidir.

Buradaki “typically” önemlidir: EGL 1.0 tek başına her swap'ın VSync beklediğini,
tearing'in kesin engellendiğini veya swap'ın monitör yenileme hızında sabit FPS
üreteceğini garanti etmez. Sunum yöntemi ve bloklama davranışı implementation ve
native platforma bağlıdır.

## Swap ne yapmaz?

- Yeni bir frame çizmez; o ana kadar üretilmiş color buffer'ı post eder.
- `glFinish` gibi bütün GPU işlerinin tamamlanmasını zorunlu olarak beklemez.
- Pbuffer'ı görünür pencereye dönüştürmez.
- GBM/DRM yolunda tek başına KMS scanout veya page flip programlamaz.
- Başarılı dönmesi, bir sonraki frame'de eski color içeriğinin korunacağını garanti etmez.

## Native Window Boyut Değişimi

Eğer native window swap öncesinde resize edilmişse, EGL surface native window ile uyumlu hale gelmelidir.

EGL implementation resize'ı daha önce şeffaf şekilde yapamadıysa, `eglSwapBuffers` bunu fark edip surface'i post öncesinde resize etmelidir.

Sonuçlar:

- Surface küçülürse bazı çizilmiş pixel'ler kaybolur.
- Surface büyürse yeni buffer alanlarının içeriği tanımsızdır.
- Uygulama kendi viewport/scissor ayarlarını güncellemekten sorumludur.

Örnek:

```c
/* Native window 800x600 -> 1024x768 olduysa */
glViewport(0, 0, 1024, 768);
draw_scene();
eglSwapBuffers(dpy, surface);
```

EGL surface resize edebilir, ama GL viewport'u senin uygulamanın sorumluluğundadır.

## `eglSwapBuffers` ve DRM/KMS

EGL tek başına fiziksel monitörü yönetmez. Bu özellikle GBM + DRM/KMS tarafında önemlidir.

`eglSwapBuffers` sonrası:

```text
GBM surface içinde render edilmiş front buffer hazır olur.
```

Ama fiziksel monitör için hala şu gerekir:

```c
struct gbm_bo *bo = gbm_surface_lock_front_buffer(gbm_surface);

/* bo handle/stride/format -> DRM framebuffer */
drmModeAddFB2(fd, width, height, format, handles, strides, offsets, &fb_id, 0);

/* CRTC artık bu framebuffer'ı scanout etsin */
drmModeSetCrtc(fd, crtc_id, fb_id, 0, 0, &connector_id, 1, &mode);
```

Animasyonda sonraki buffer'a geçmek için:

```c
drmModePageFlip(fd, crtc_id, next_fb_id, DRM_MODE_PAGE_FLIP_EVENT, user_data);
```

Özet:

| Katman    | Sorumluluk                                                       |
| --------- | ---------------------------------------------------------------- |
| OpenGL ES | Pixel üretir.                                                   |
| EGL       | Context/surface bağlar ve swap/post işlemini yapar.            |
| GBM       | GPU/display paylaşılabilir buffer nesneleri sağlar.           |
| DRM/KMS   | Framebuffer'ı CRTC/connector üzerinden monitöre scanout eder. |

## Temel Window Surface Kullanımı

```c
eglMakeCurrent(dpy, window_surface, window_surface, ctx);

glViewport(0, 0, width, height);
glClearColor(0.0f, 0.2f, 0.8f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);

if (!eglSwapBuffers(dpy, window_surface)) {
    EGLint err = eglGetError();
}
```

## Temel Pbuffer Kullanımı

```c
eglMakeCurrent(dpy, pbuffer_surface, pbuffer_surface, ctx);

glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);

eglSwapBuffers(dpy, pbuffer_surface); /* EGL 1.0: no effect */
```

Pbuffer için `eglSwapBuffers` çağrısı öğretici olabilir ama görünür output beklenmemelidir.

## Bölüm Özeti

- `eglSwapBuffers` window surface için anlamlıdır.
- Pbuffer ve pixmap surface için etkisi yoktur.
- EGL 1.0'da surface current context'e bağlı olmalıdır.
- Başarılı swap sonrası color buffer içeriğini korunmuş sayma.
- `eglSwapBuffers` implicit `glFlush` yapar ama `glFinish` değildir.
- “Swap” fiziksel olarak mutlaka pointer değişimi demek değildir; gözlemlenebilir işlem window'a post edilmesidir.
- EGL 1.0 VSync, tearing engelleme veya sabit FPS garantisi vermez.
- GBM/DRM kullanıyorsan swap sonrası ayrıca BO alma ve KMS scanout gerekir.

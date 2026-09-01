# EGL 1.0: `eglMakeCurrent`

```c
EGLBoolean eglMakeCurrent(EGLDisplay dpy,
                          EGLSurface draw,
                          EGLSurface read,
                          EGLContext ctx);
```

`eglMakeCurrent`, bir `EGLContext` nesnesini çağıran thread'in current rendering context'i yapar. Aynı çağrıda iki surface bağlanır:

- `draw`: OpenGL ES çizim komutlarının yazdığı framebuffer.
- `read`: `glReadPixels` gibi okuma komutlarının okuduğu framebuffer.

En yaygın kullanımda `draw` ve `read` aynı surface'tir:

```c
eglMakeCurrent(dpy, surface, surface, ctx);
```

## Kavramsal Akış

EGL 1.0 açısından current context thread-local bir durumdur:

```text
Thread
  |
  +-- current EGLDisplay
  +-- current EGLContext
  +-- current draw EGLSurface
  +-- current read EGLSurface
```

`eglMakeCurrent` bu dörtlüyü değiştirir. OpenGL ES komutları doğrudan `EGLContext` handle'ına parametre olarak verilmez; komutlar çağıran thread'in current context'i üzerinden çalışır.

Context ve surface sorumlulukları aşağıdaki modelle ayrıştırılabilir:

```text
EGLContext -> renk, depth test, blending, texture binding gibi GL state
draw       -> sonuçların yazıldığı color/depth/stencil buffer'ları
read       -> pixel okuma işlemlerinin kaynak buffer'ı
thread     -> GL komutlarını hangi current bağlantının yorumlayacağını belirler
```

Depth, stencil ve multisample buffer'lar context'in içinde değil, surface ile
ilişkilidir. Aynı uyumlu surface'e farklı zamanlarda farklı context'ler bağlanırsa
bu surface buffer'larını paylaşırlar; her context'in GL state'i ise kendisine aittir.

## Parametreler

### `dpy`

`dpy`, context ve surface nesnelerinin ait olduğu initialized `EGLDisplay` olmalıdır.

| Değer                                | Sonuç                                                                  |
| ------------------------------------- | ----------------------------------------------------------------------- |
| Geçerli ve initialized`EGLDisplay` | Diğer parametreler de geçerliyse çağrı başarılıdır.            |
| `EGL_NO_DISPLAY`                    | Başarısız. Genel EGL hata modeliyle`EGL_BAD_DISPLAY` beklenir.     |
| Geçersiz display handle              | Başarısız. Genel EGL hata modeliyle`EGL_BAD_DISPLAY` beklenir.     |
| Initialize edilmemiş display         | Başarısız. Genel EGL hata modeliyle`EGL_NOT_INITIALIZED` beklenir. |

Initialization gereksinimi:

```c
EGLDisplay dpy = eglGetDisplay(native_display);
eglInitialize(dpy, &major, &minor);
```

`eglMakeCurrent`, `eglInitialize` başarıyla tamamlanmadan çağrılmamalıdır.

### `draw`

`draw`, çizim hedefidir.

| Değer                                                                        | Sonuç                                                         |
| ----------------------------------------------------------------------------- | -------------------------------------------------------------- |
| `ctx` ile uyumlu geçerli `EGLSurface`                                    | Geçerli. OpenGL ES draw komutları buraya yazar.              |
| `read` ile aynı surface                                                    | Geçerli ve normal kullanım.                                  |
| `read`'den farklı ama uyumlu surface                                       | Geçerli.                                                      |
| `EGL_NO_SURFACE` ve `ctx == EGL_NO_CONTEXT` ve `read == EGL_NO_SURFACE` | Geçerli. Current context release edilir.                      |
| `EGL_NO_SURFACE` ve `ctx != EGL_NO_CONTEXT`                               | Başarısız,`EGL_BAD_MATCH`.                                |
| Geçersiz surface                                                             | Başarısız,`EGL_BAD_SURFACE`.                              |
| Yok edilmiş surface                                                          | Başarısız veya sonraki framebuffer davranışı tanımsız. |
| Native window'u geçersiz window surface                                      | Başarısız,`EGL_BAD_NATIVE_WINDOW`.                        |
| `ctx` ile uyumsuz surface                                                   | Başarısız,`EGL_BAD_MATCH`.                                |
| Başka thread'de başka context'e bağlı surface                             | Başarısız,`EGL_BAD_ACCESS`.                               |

### `read`

`read`, framebuffer okuma kaynağıdır.

| Değer                                                                        | Sonuç                                        |
| ----------------------------------------------------------------------------- | --------------------------------------------- |
| `ctx` ile uyumlu geçerli `EGLSurface`                                    | Geçerli.`glReadPixels` buradan okur.       |
| `draw` ile aynı surface                                                    | Geçerli ve normal kullanım.                 |
| `draw`'dan farklı ama uyumlu surface                                       | Geçerli.                                     |
| `EGL_NO_SURFACE` ve `ctx == EGL_NO_CONTEXT` ve `draw == EGL_NO_SURFACE` | Geçerli. Current context release edilir.     |
| `EGL_NO_SURFACE` ve `ctx != EGL_NO_CONTEXT`                               | Başarısız,`EGL_BAD_MATCH`.               |
| Geçersiz surface                                                             | Başarısız,`EGL_BAD_SURFACE`.             |
| Yok edilmiş surface                                                          | Başarısız veya readback sonucu tanımsız. |
| Native window'u geçersiz window surface                                      | Başarısız,`EGL_BAD_NATIVE_WINDOW`.       |
| `ctx` ile uyumsuz surface                                                   | Başarısız,`EGL_BAD_MATCH`.               |
| Başka thread'de başka context'e bağlı surface                             | Başarısız,`EGL_BAD_ACCESS`.              |

### `ctx`

`ctx`, current yapılacak rendering context'tir.

| Değer                                                                     | Sonuç                                       |
| -------------------------------------------------------------------------- | -------------------------------------------- |
| Geçerli`EGLContext`                                                     | Çağıran thread'in current context'i olur. |
| `EGL_NO_CONTEXT`, `draw == EGL_NO_SURFACE`, `read == EGL_NO_SURFACE` | Geçerli. Current context kaldırılır.     |
| `EGL_NO_CONTEXT`, ama `draw` veya `read` gerçek surface             | Başarısız,`EGL_BAD_MATCH`.              |
| Geçersiz context                                                          | Başarısız,`EGL_BAD_CONTEXT`.            |
| Başka thread'de current olan context                                      | Başarısız,`EGL_BAD_ACCESS`.             |
| Surface'lerle uyumsuz context                                              | Başarısız,`EGL_BAD_MATCH`.              |

## Geçerli Kombinasyonlar

### 1. Standart Binding

```c
eglMakeCurrent(dpy, surface, surface, ctx);
```

Sonuç:

```text
Thread current context = ctx
Thread draw surface    = surface
Thread read surface    = surface
```

### 2. Ayrı Draw/Read Surface Binding

```c
eglMakeCurrent(dpy, draw_surface, read_surface, ctx);
```

Sonuç:

```text
Draw commands -> draw_surface
Read commands -> read_surface
```

Bu kullanım EGL 1.0 tarafından desteklenmekle birlikte standart binding'e göre daha sınırlı kullanım alanına sahiptir.

### 3. Current Context'i Serbest Bırakma

```c
eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
```

Bu çağrı, EGL 1.0'da current context'i serbest bırakmak için tanımlanan yöntemdir.

## Context ve Surface Uyumluluğu

EGL 1.0'da yalnızca handle'ların geçerli olması yetmez. Context ve surface:

- aynı `EGLDisplay` ile oluşturulmuş olmalı,
- color ve ancillary buffer derinlikleri uyumlu olmalıdır.

Ancillary buffer; depth, stencil ve multisample buffer gibi color dışındaki
buffer'ları kapsar. Örneğin context'in config'i RGBA8888 + depth24 + stencil8
iken surface RGB565 + depth16 ise ikisi geçerli EGL nesneleri olsa bile birlikte
current yapılamaz ve `EGL_BAD_MATCH` oluşur.

Config ID'lerinin aynı olması zorunlu değildir. İki farklı config handle'ı aynı
display üzerinde aynı color/ancillary buffer derinliklerini tarif ediyorsa uyumlu
olabilir. Buna karşılık bit büyüklükleri aynı olsa bile farklı display'lerde
oluşturulan nesneler uyumlu değildir.

| Context config | Surface config | Display | Sonuç |
| -------------- | -------------- | ------- | ----- |
| RGBA8, D24, S8 | RGBA8, D24, S8 | Aynı | Uyumlu olabilir. |
| RGBA8, D24, S8 | RGB565, D16, S0 | Aynı | `EGL_BAD_MATCH` |
| RGBA8, D24, S8 | RGBA8, D24, S8 | Farklı | `EGL_BAD_MATCH` |

## Ayrı Draw/Read Surface Kullanım Örneği

İki surface de context ile uyumluysa çizim ve okuma hedefleri ayrılabilir:

```c
eglMakeCurrent(dpy, window_surface, pbuffer_surface, ctx);

/* Çizim komutları window_surface'e gider. */
glClear(GL_COLOR_BUFFER_BIT);

/* Pixel okuma pbuffer_surface'ten gelir. */
glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
```

Bu çağrı pbuffer içeriğini window'a kopyalamaz. Yalnızca aynı context için draw
ve read yönlerinin hangi surface'i kullandığını belirler.

## Thread'ler Arasında Context Aktarımı

Bir context aynı anda yalnızca bir thread'de current olabilir. Thread A'da
current olan `ctx` doğrudan Thread B'de bağlanırsa `EGL_BAD_ACCESS` oluşur.
Önerilen aktarım sırası şöyledir:

```text
Thread A: GL işini bitir
Thread A: eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)
Thread A -> Thread B: uygulama düzeyinde mutex/condition ile haber ver
Thread B: eglMakeCurrent(dpy, surface, surface, ctx)
```

EGL çağrıları uygulamanın thread'leri arasındaki iş teslim protokolünün yerini
almaz; aynı context'e erişimi uygulama ayrıca senkronize etmelidir.

## Geçersiz Kombinasyon Matrisi

| `draw`             | `read`             | `ctx`            | Sonuç                        |
| -------------------- | -------------------- | ------------------ | ----------------------------- |
| surface              | surface              | context            | Geçerli, uyumluysalar.       |
| surface A            | surface B            | context            | Geçerli, ikisi de uyumluysa. |
| `EGL_NO_SURFACE`   | `EGL_NO_SURFACE`   | `EGL_NO_CONTEXT` | Geçerli release çağrısı. |
| surface              | surface              | `EGL_NO_CONTEXT` | `EGL_BAD_MATCH`             |
| `EGL_NO_SURFACE`   | surface              | context            | `EGL_BAD_MATCH`             |
| surface              | `EGL_NO_SURFACE`   | context            | `EGL_BAD_MATCH`             |
| `EGL_NO_SURFACE`   | `EGL_NO_SURFACE`   | context            | `EGL_BAD_MATCH`             |
| invalid surface      | surface              | context            | `EGL_BAD_SURFACE`           |
| surface              | invalid surface      | context            | `EGL_BAD_SURFACE`           |
| surface              | surface              | invalid context    | `EGL_BAD_CONTEXT`           |
| incompatible surface | surface              | context            | `EGL_BAD_MATCH`             |
| surface              | incompatible surface | context            | `EGL_BAD_MATCH`             |

## Hata Kodları

| Hata                        | Oluşma koşulu                                                                                                                          |
| --------------------------- | -------------------------------------------------------------------------------------------------------------------------------------- |
| `EGL_BAD_MATCH`           | Surface/context uyumsuzsa;`EGL_NO_CONTEXT`/`EGL_NO_SURFACE` kombinasyonu yanlışsa; draw/read aynı anda belleğe sığamıyorsa. |
| `EGL_BAD_ACCESS`          | `ctx` başka thread'de current ise; `draw` veya `read` başka thread'de bir context'e bağlıysa.                                |
| `EGL_BAD_CONTEXT`         | `ctx` geçerli EGL context değilse.                                                                                                 |
| `EGL_BAD_SURFACE`         | `draw` veya `read` geçerli EGL surface değilse.                                                                                  |
| `EGL_BAD_NATIVE_WINDOW`   | Surface'in altında yatan native window artık geçerli değilse.                                                                      |
| `EGL_BAD_CURRENT_SURFACE` | Önceki current context'in flush edilmemiş komutları varsa ve önceki surface artık geçerli değilse.                              |
| `EGL_BAD_ALLOC`           | Draw/read için gerekli ancillary buffer'lar ayrılamazsa.                                                                             |

## Durum Değişimi

Başarılı çağrı öncesi:

```text
Thread current context = old_ctx
Thread draw surface    = old_draw
Thread read surface    = old_read
```

Başarılı `eglMakeCurrent(dpy, draw, read, ctx)` sonrası:

```text
old_ctx flush edilir
old_ctx artık current değildir

Thread current context = ctx
Thread draw surface    = draw
Thread read surface    = read
```

Başarılı release sonrası:

```text
eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)

Thread current context = EGL_NO_CONTEXT
Thread draw surface    = EGL_NO_SURFACE
Thread read surface    = EGL_NO_SURFACE
```

## Viewport ve Scissor Notu

EGL 1.0 spec'ine göre bir context ilk kez current yapıldığında viewport ve scissor boyutları draw surface boyutuna ayarlanır:

```text
glViewport(0, 0, draw_width, draw_height)
glScissor(0, 0, draw_width, draw_height)
```

Bu davranış yalnızca context'in ilk kez current yapıldığı an için geçerlidir. Sonraki binding işlemlerinde viewport ve scissor değerlerinin otomatik olarak güncellenmesi garanti edilmez.

## Yok Etme Sonrası Davranış

`eglMakeCurrent` başarılı olduktan sonra:

- `draw` destroy edilirse render komutları işlenebilir ama framebuffer state tanımsız olur.
- `read` destroy edilirse `glReadPixels` gibi okuma sonuçları tanımsız olur.
- Native window/pixmap destroy edilirse de ilgili surface için aynı mantık geçerlidir.

Önerilen kapanış sırası şöyledir:

```c
eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
eglDestroySurface(dpy, surface);
eglDestroyContext(dpy, ctx);
eglTerminate(dpy);
```

## Temel Kullanım

```c
EGLDisplay dpy = eglGetDisplay(native_display);
eglInitialize(dpy, &major, &minor);

EGLConfig config = /* eglChooseConfig ile alınmış config */;
EGLSurface surface = /* eglCreateWindowSurface veya eglCreatePbufferSurface */;
EGLContext ctx = eglCreateContext(dpy, config, EGL_NO_CONTEXT, NULL);

if (!eglMakeCurrent(dpy, surface, surface, ctx)) {
    EGLint err = eglGetError();
}

/* OpenGL ES komutları */

eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
```

## Bölüm Özeti

- `eglMakeCurrent` thread-local current context'i değiştirir.
- OpenGL ES komutlarının hangi context/surface üzerinde çalışacağını bu çağrı belirler.
- `draw` çizim hedefidir, `read` okuma hedefidir.
- Context GL state'ini, surface ise color/depth/stencil gibi framebuffer depolarını taşır.
- Uyum için nesnelerin aynı display'e ait olması ve color/ancillary buffer derinliklerinin eşleşmesi gerekir.
- `EGL_NO_CONTEXT` sadece iki surface de `EGL_NO_SURFACE` ise geçerlidir.
- Context veya surface başka thread'de bağlıysa `EGL_BAD_ACCESS` beklenir.
- Surface/context format ve display açısından uyumsuzsa `EGL_BAD_MATCH` beklenir.

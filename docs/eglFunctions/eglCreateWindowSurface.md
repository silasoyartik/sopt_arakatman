# EGL 1.0: `eglCreateWindowSurface`

```c
EGLSurface eglCreateWindowSurface(EGLDisplay dpy,
                                  EGLConfig config,
                                  NativeWindowType win,
                                  const EGLint *attrib_list);
```

`eglCreateWindowSurface`, native pencere sistemi tarafından daha önce
oluşturulmuş bir pencereyi EGL rendering surface'i ile ilişkilendirir.
Başarılı olduğunda OpenGL ES context'inin draw/read hedefi olarak
kullanılabilecek bir `EGLSurface` handle'ı döndürür.

Fonksiyon native pencereyi oluşturmaz, ekranda gösterecek compositor veya KMS
ayarını yapmaz ve rendering context oluşturmaz. Yalnızca mevcut native nesne
ile EGL tarafındaki surface arasındaki bağı kurar.

![Native window ile EGLSurface arasındaki katmanlar](image/eglCreateWindowSurface/object-model.svg)

## Kavramsal Model

```text
Native platform                         EGL
---------------                         ---
X11 Window       --\
wl_egl_window    ----> NativeWindowType ---> eglCreateWindowSurface
Win32 HWND       --/                              |
GBM surface      -/                               v
                                               EGLSurface
                                                   |
                                                   v
                                             eglMakeCurrent
                                                   |
                                                   v
                                           OpenGL ES rendering
```

`NativeWindowType` platforma bağlıdır. Modern header'larda aynı kavram
`EGLNativeWindowType` typedef'iyle görülebilir. Uygulama, kullandığı EGL
platformunun beklediği native nesneyi vermelidir.

## Parametreler

### `dpy`

`dpy`, initialize edilmiş EGL display bağlantısıdır. Hem `config` hem de
dönen `EGLSurface` bu display'in namespace'ine aittir.

| Durum | Sonuç |
| --- | --- |
| Geçerli ve initialize edilmiş `EGLDisplay` | Diğer argümanlar uygunsa surface oluşturulabilir. |
| `EGL_NO_DISPLAY` veya geçersiz handle | `EGL_NO_SURFACE`, `EGL_BAD_DISPLAY`. |
| Geçerli fakat initialize edilmemiş display | `EGL_NO_SURFACE`, `EGL_NOT_INITIALIZED`. |

Display, config, context ve surface nesneleri arasındaki sahiplik ilişkisi
önemlidir:

```text
EGLDisplay A
  +-- EGLConfig A1
  +-- EGLContext A2
  +-- EGLSurface A3

EGLDisplay B
  +-- EGLConfig B1
  +-- EGLContext B2
  +-- EGLSurface B3
```

Bir display'den alınan config başka display'de kullanılamaz.

### `config`

`config`, surface'in framebuffer özelliklerini ve desteklediği surface
türlerini tanımlar. Window surface oluşturabilmek için config'in
`EGL_SURFACE_TYPE` bitmask'i `EGL_WINDOW_BIT` içermelidir.

```c
EGLint surface_type = 0;

if (eglGetConfigAttrib(dpy, config,
                       EGL_SURFACE_TYPE, &surface_type) == EGL_TRUE &&
    (surface_type & EGL_WINDOW_BIT) != 0) {
    /* config supports window surfaces */
}
```

Config ayrıca color, depth, stencil ve multisample buffer özelliklerini
belirler. Bu değerlerin tamamı
[`eglGetConfigAttrib`](eglGetConfigAttrib.md) ile sorgulanabilir.

| Config durumu | Sonuç |
| --- | --- |
| Geçerli ve `EGL_WINDOW_BIT` destekli | Native window ile uyumluysa surface oluşturulur. |
| Geçersiz config handle | `EGL_NO_SURFACE`, `EGL_BAD_CONFIG`. |
| `EGL_WINDOW_BIT` içermiyor | `EGL_NO_SURFACE`, `EGL_BAD_MATCH`. |
| Native window formatıyla uyumsuz | `EGL_NO_SURFACE`, `EGL_BAD_MATCH`. |

![Config ile native window uyumluluğu](image/eglCreateWindowSurface/compatibility.svg)

#### Config Attribute'u ile Surface Attribute'u Farkı

`EGL_RED_SIZE`, `EGL_DEPTH_SIZE` ve `EGL_SURFACE_TYPE` gibi değerler
`eglChooseConfig` için config seçim kriteridir. Bunlar
`eglCreateWindowSurface` fonksiyonunun `attrib_list` parametresine yazılmaz.

```text
EGL_RED_SIZE, EGL_DEPTH_SIZE, EGL_SURFACE_TYPE
                    |
                    v
             eglChooseConfig
                    |
                    v
               EGLConfig
                    |
                    v
         eglCreateWindowSurface
```

### `win`

`win`, platformun geçerli native window nesnesidir.

| Platform | Yaygın native nesne | Not |
| --- | --- | --- |
| X11 | `Window` | X server tarafında oluşturulmuş pencere ID'si. |
| Wayland | `struct wl_egl_window *` | Genellikle `wl_surface` üzerinden `wayland-egl` ile oluşturulur. |
| Win32 | `HWND` | Win32 pencere handle'ı. |
| Mesa/GBM | `struct gbm_surface *` | DRM/KMS tabanlı platform entegrasyonunda kullanılabilir. |

Native nesnenin gerçek türü, `EGLDisplay` elde edilirken seçilen
platformla uyumlu olmalıdır. X11 display ile Wayland window veya GBM display
ile X11 Window kullanmak geçerli bir platform eşleşmesi değildir.

Geçersiz native window implementation tarafından algılanabilirse fonksiyon
`EGL_NO_SURFACE` döndürür ve `EGL_BAD_NATIVE_WINDOW` kaydeder. EGL 1.0,
geçersiz native handle'ların her platformda mutlaka algılanmasını garanti
etmez.

#### GBM ve DRM/KMS İlişkisi

Bu projede native window rolünü `struct gbm_surface *` üstlenir:

```text
DRM device fd
      |
      v
gbm_create_device
      |
      v
gbm_surface_create
      |
      v
eglCreateWindowSurface
      |
      v
EGLSurface
```

`eglCreateWindowSurface` monitör connector'ı, CRTC, mode veya page flip
seçmez. GBM front buffer'ını DRM framebuffer'a bağlamak ve ekranda sunmak
DRM/KMS katmanının işidir.

### `attrib_list`

EGL 1.0 core, `eglCreateWindowSurface` için değiştirilebilir bir window
surface creation attribute'u tanımlamaz. Bu nedenle core kullanımı:

```c
NULL
```

veya boş liste:

```c
const EGLint attributes[] = {
    EGL_NONE
};
```

şeklindedir. İki biçim de ek attribute verilmediğini ifade eder.

```c
/* Wrong: EGL_RED_SIZE is a config selection attribute. */
const EGLint wrong_attributes[] = {
    EGL_RED_SIZE, 8,
    EGL_NONE
};
```

Tanınmayan bir attribute veya değer kullanılması `EGL_BAD_ATTRIBUTE`
oluşturabilir. Sonraki EGL sürümleri veya extension'lar yeni surface
attribute'ları tanımlayabilir; bunlar EGL 1.0 core davranışı sayılmaz.

## Native Window ile Config Uyumluluğu

Config'in color formatı ile native window'un beklediği format birbiriyle
uyumlu olmalıdır. Örneğin GBM tarafında seçilen pixel formatı, EGL config'in
native visual bilgisiyle eşleştirilebilir.

```c
EGLint native_visual_id = 0;

if (eglGetConfigAttrib(dpy, config,
                       EGL_NATIVE_VISUAL_ID,
                       &native_visual_id) == EGL_TRUE) {
    /* Use the platform-specific value when creating the native window. */
}
```

`EGL_NATIVE_VISUAL_ID` platforma bağlıdır. GBM/Mesa ortamında pratikte
DRM fourcc/GBM format seçimini yönlendirebilir; X11'de native visual ID anlamı
taşır. Taşınabilir kod bu değeri kendi başına evrensel bir pixel format
numarası gibi yorumlamamalıdır.

## Oluşturulan Surface'in Özellikleri

Window surface'in boyutu genellikle native window boyutundan gelir.
`eglCreateWindowSurface` çağrısına width/height verilmez. Oluşturulduktan
sonra surface bilgileri `eglQuerySurface` ile sorgulanabilir:

```c
EGLint width = 0;
EGLint height = 0;

eglQuerySurface(dpy, surface, EGL_WIDTH, &width);
eglQuerySurface(dpy, surface, EGL_HEIGHT, &height);
```

`EGL_WIDTH` ve `EGL_HEIGHT`, config attribute'u değil surface query
attribute'udur.

## Context ile Bağlama ve Sunum

Surface oluşturulması onu current yapmaz. Rendering için uyumlu bir context
ile bağlanması gerekir:

```c
if (eglMakeCurrent(dpy, surface, surface, context) == EGL_FALSE) {
    EGLint error = eglGetError();
}
```

Rendering sonrası `eglSwapBuffers` window surface'in color buffer'larını
native platformun sunum mekanizmasına iletir:

```c
glDrawArrays(GL_TRIANGLES, 0, 3);
eglSwapBuffers(dpy, surface);
```

![Window surface yaşam döngüsü](image/eglCreateWindowSurface/lifecycle.svg)

## Bir Native Window'a Birden Fazla EGLSurface

EGL 1.0'a göre aynı native window zaten daha önce oluşturulmuş bir EGL
config ile ilişkiliyse yeni surface oluşturma kaynak ayıramayabilir ve
`EGL_BAD_ALLOC` oluşabilir. Uygulama aynı native window için yeni surface
oluşturmadan önce eski EGLSurface'in yaşam döngüsünü tamamlamalıdır.

## Hata Matrisi

Başarısız durumda dönüş değeri `EGL_NO_SURFACE` olur.

| Koşul | Hata |
| --- | --- |
| EGL ilgili display için initialize edilmemiş | `EGL_NOT_INITIALIZED` |
| `dpy` geçerli display değil | `EGL_BAD_DISPLAY` |
| `config` geçerli config değil | `EGL_BAD_CONFIG` |
| Config `EGL_WINDOW_BIT` içermiyor | `EGL_BAD_MATCH` |
| Native window attribute'ları config ile uyuşmuyor | `EGL_BAD_MATCH` |
| Native window geçersiz ve durum algılanabiliyor | `EGL_BAD_NATIVE_WINDOW` |
| Native window zaten EGL config ile ilişkili | `EGL_BAD_ALLOC` |
| Yeni surface için kaynak ayrılamıyor | `EGL_BAD_ALLOC` |
| Attribute listesinde tanınmayan attribute/değer var | `EGL_BAD_ATTRIBUTE` |

## Temel Kullanım

```c
EGLSurface surface = eglCreateWindowSurface(
    dpy,
    config,
    native_window,
    NULL
);

if (surface == EGL_NO_SURFACE) {
    EGLint error = eglGetError();
    /* Handle the error. */
}
```

Kullanım tamamlandığında:

```c
eglMakeCurrent(dpy,
               EGL_NO_SURFACE,
               EGL_NO_SURFACE,
               EGL_NO_CONTEXT);
eglDestroySurface(dpy, surface);
```

Native window ayrı bir nesnedir ve kendi platform API'siyle ayrıca yok
edilmelidir.

## Bölüm Özeti

- Fonksiyon mevcut native window üzerinde bir EGL window surface oluşturur.
- `dpy`, `config` ve dönen surface aynı EGLDisplay namespace'ine aittir.
- Config `EGL_WINDOW_BIT` içermeli ve native window formatıyla uyuşmalıdır.
- Native window türü EGL platformuna bağlıdır.
- EGL 1.0 core'da `attrib_list` için tanımlı window attribute'u yoktur.
- Surface oluşturmak context'i current yapmaz ve fiziksel sunumu tek başına gerçekleştirmez.

## Kaynak

- EGL 1.0 Specification, Section 3.5.1, Creating On-Screen Rendering Surfaces.

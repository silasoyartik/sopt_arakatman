# EGL 1.0: `eglCreateWindowSurface`

```c
EGLSurface eglCreateWindowSurface(EGLDisplay dpy,
                                  EGLConfig config,
                                  NativeWindowType win,
                                  const EGLint *attrib_list);
```

`eglCreateWindowSurface`, önceden oluşturulmuş bir native window üzerinde ekrana çizim yapılabilecek bir `EGLSurface` oluşturur.

EGL 1.0 standardı `NativeWindowType` nesnesinin gerçek türünü platforma bırakır. Bu projede X11 veya Wayland kullanılmadığı için native window rolünü Mesa/GBM tarafındaki `struct gbm_surface *` üstlenir.

Bu projedeki temel kullanım:

```c
EGLSurface egl_surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

## Mental Model

EGL 1.0 açısından `eglCreateWindowSurface` native window'u kendisi oluşturmaz. Daha önce native platform tarafından oluşturulmuş window nesnesinin üzerine bir EGL rendering surface oluşturur.

```text
Native Platform
      |
      +-- Native Display
      |
      +-- Native Window
              |
              v
      eglCreateWindowSurface
              |
              v
          EGLSurface
```

Bu projedeki karşılığı:

```text
/dev/dri/card*
      |
      v
    DRM/KMS
      |
      v
 gbm_device
      |
      v
 gbm_surface
      |
      | NativeWindowType olarak EGL'e verilir
      v
eglCreateWindowSurface
      |
      v
  EGLSurface
      |
      v
 OpenGL ES 2.0
```

Burada iki farklı surface kavramı vardır:

```text
struct gbm_surface *  -> Mesa/GBM native surface
EGLSurface            -> EGL rendering surface
```

`gbm_surface`, EGL'in bağlanacağı native platform nesnesidir. `EGLSurface` ise OpenGL ES context'inin çizim yapacağı EGL nesnesidir.

## Parametreler

### `dpy`

`dpy`, surface'in oluşturulacağı initialized `EGLDisplay` nesnesidir.

| Değer | Sonuç |
|---|---|
| Geçerli ve initialized `EGLDisplay` | Diğer parametreler de uygunsa surface oluşturulabilir. |
| GBM tabanlı initialized `EGLDisplay` | Bu projede kullanılacak normal durumdur. |

Bu projede display zinciri kabaca:

```text
DRM file descriptor
        |
        v
gbm_create_device()
        |
        v
struct gbm_device *
        |
        v
EGLDisplay
```

Örnek:

```c
EGLSurface egl_surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

`dpy` çözünürlük, renk formatı veya buffer tipi seçmez. Hangi EGL display bağlantısında çalışılacağını belirtir.

Pratik kural:

```text
dpy = GBM native platformuna bağlı ve eglInitialize() ile başlatılmış EGLDisplay
```

### `config`

`config`, oluşturulacak surface ile kullanılacak framebuffer/pixel yapılandırmasını temsil eden `EGLConfig` nesnesidir.

Bu handle genellikle `eglChooseConfig` veya `eglGetConfigs` ile elde edilir. Window surface oluşturulabilmesi için config'in `EGL_SURFACE_TYPE` özelliğinde `EGL_WINDOW_BIT` bulunmalıdır.

Örnek seçim:

```c
const EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE,     8,
    EGL_GREEN_SIZE,   8,
    EGL_BLUE_SIZE,    8,
    EGL_ALPHA_SIZE,   8,
    EGL_NONE
};

EGLConfig egl_config;
EGLint num_configs;

eglChooseConfig(
    egl_display,
    config_attribs,
    &egl_config,
    1,
    &num_configs
);
```

Sonra:

```c
EGLSurface egl_surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

#### Farklı `config` değerleri

`eglCreateWindowSurface` içine RGB değerleri doğrudan verilmez. Farklı özelliklere sahip `EGLConfig` handle'ları seçilir.

Örneğin:

```text
Config A
R = 8
G = 8
B = 8
A = 8
Surface Type = EGL_WINDOW_BIT
```

ve:

```text
Config B
R = 5
G = 6
B = 5
Surface Type = EGL_WINDOW_BIT
```

Sonra:

```c
eglCreateWindowSurface(dpy, config_a, win, NULL);
eglCreateWindowSurface(dpy, config_b, win, NULL);
```

şeklinde farklı config'ler denenebilir.

Bu projede seçilen `EGLConfig` ile GBM surface'in pixel formatının da uyumlu olması gerekir.

Örneğin GBM tarafında:

```c
gbm_surface_create(
    gbm,
    width,
    height,
    GBM_FORMAT_XRGB8888,
    GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
);
```

kullanılıyorsa native GBM surface ile seçilen EGL config'in uyumlu olması gerekir.

### `win`

`win`, EGL 1.0 standardında platforma özgü native window handle'ıdır.

EGL 1.0, bu parametrenin gerçek C türünü tek başına belirlemez. Platform entegrasyonu hangi native window türünü kullanıyorsa o nesne buraya verilir.

Klasik örnekler:

```text
X11       -> X11 Window
Wayland   -> Wayland surface
Windows   -> native window handle
```

Bu projede ise:

```text
Mesa/GBM  -> struct gbm_surface *
```

kullanılır.

Önce GBM surface oluşturulur:

```c
struct gbm_surface *gbm_surface =
    gbm_surface_create(
        gbm,
        mode.hdisplay,
        mode.vdisplay,
        GBM_FORMAT_XRGB8888,
        GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
    );
```

Daha sonra:

```c
EGLSurface egl_surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

Burada:

```text
win = (EGLNativeWindowType)gbm_surface
```

olur.

#### Farklı `win` değerleri

İki farklı GBM surface:

```c
struct gbm_surface *gbm_surface_a;
struct gbm_surface *gbm_surface_b;
```

için:

```c
EGLSurface surface_a = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface_a,
    NULL
);

EGLSurface surface_b = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface_b,
    NULL
);
```

Sonuç:

```text
gbm_surface_a -> EGLSurface A
gbm_surface_b -> EGLSurface B
```

Yani `win` değişirse EGLSurface'in bağlı olduğu native surface değişir.

#### DRM mode ile boyut ilişkisi

Direct-to-display kullanımda GBM surface boyutu seçilen DRM mode ile eşleştirilebilir:

```text
mode.hdisplay = 1920
mode.vdisplay = 1080
```

```c
gbm_surface_create(
    gbm,
    mode.hdisplay,
    mode.vdisplay,
    GBM_FORMAT_XRGB8888,
    GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
);
```

Bu 1920x1080 GBM surface daha sonra `win` olarak EGL'e verilir.

### `attrib_list`

`attrib_list`, window surface attribute listesidir.

EGL 1.0 core standardında `eglCreateWindowSurface` için tanımlanmış bir window attribute'u yoktur. Bu nedenle normal kullanımlar:

```c
NULL
```

veya:

```c
const EGLint surface_attribs[] = {
    EGL_NONE
};
```

şeklindedir.

#### 1. `NULL`

```c
EGLSurface egl_surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

Sonuç:

```text
Ek window surface attribute'u belirtilmez.
```

#### 2. `{ EGL_NONE }`

```c
const EGLint surface_attribs[] = {
    EGL_NONE
};

EGLSurface egl_surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    surface_attribs
);
```

`EGL_NONE`, attribute listesinin bittiğini belirtir.

EGL 1.0 core açısından:

```text
NULL
```

ve:

```text
{ EGL_NONE }
```

ek window attribute verilmediğini ifade eder.

> Not: Platform extension'ları ek attribute'lar tanımlayabilir. Bunlar EGL 1.0 core davranışı değildir.

## Geçerli Kombinasyonlar

### 1. GBM surface + `NULL`

```c
EGLSurface egl_surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

```text
dpy         = initialized GBM tabanlı EGLDisplay
config      = EGL_WINDOW_BIT destekleyen uyumlu EGLConfig
win         = gbm_surface
attrib_list = NULL
```

### 2. GBM surface + boş attribute listesi

```c
const EGLint attrs[] = {
    EGL_NONE
};

EGLSurface egl_surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    attrs
);
```

### 3. Farklı config

```c
EGLSurface surface_a = eglCreateWindowSurface(
    egl_display,
    config_a,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

```c
EGLSurface surface_b = eglCreateWindowSurface(
    egl_display,
    config_b,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

### 4. Farklı native GBM surface

```c
EGLSurface surface_a = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface_a,
    NULL
);

EGLSurface surface_b = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface_b,
    NULL
);
```

## Parametre Matrisi

| `dpy` | `config` | `win` | `attrib_list` | Sonuç |
|---|---|---|---|---|
| GBM tabanlı initialized display | Uyumlu window config | `gbm_surface` | `NULL` | Bu proje için temel kullanım |
| GBM tabanlı initialized display | Uyumlu window config | `gbm_surface` | `{ EGL_NONE }` | Geçerli boş attribute listesi |
| Aynı display | Config A | Aynı GBM surface | `NULL` | Config A kullanılır |
| Aynı display | Config B | Aynı GBM surface | `NULL` | Config B kullanılır |
| Aynı display | Aynı config | GBM surface A | `NULL` | Surface A'ya bağlanır |
| Aynı display | Aynı config | GBM surface B | `NULL` | Surface B'ye bağlanır |

## Dönüş Değeri

Fonksiyonun dönüş tipi:

```c
EGLSurface
```

Başarılı olduğunda oluşturulan surface handle'ını döndürür.

Başarısız olduğunda:

```c
EGL_NO_SURFACE
```

döndürür.

Örnek kontrol:

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

## EGL 1.0 Hata Kodları

| Hata | Ne zaman |
|---|---|
| `EGL_BAD_MATCH` | `win` özellikleri `config` ile uyuşmuyorsa veya config window rendering desteklemiyorsa. |
| `EGL_BAD_CONFIG` | `config` geçerli bir `EGLConfig` değilse. |
| `EGL_BAD_NATIVE_WINDOW` | `win` geçerli bir native window handle değilse. |
| `EGL_BAD_ALLOC` | Aynı native window ile daha önce bir EGLConfig ilişkilendirilmişse veya yeni surface için gerekli kaynaklar ayrılamıyorsa. |

Window surface için config'in:

```text
EGL_SURFACE_TYPE
```

özelliğinin:

```text
EGL_WINDOW_BIT
```

içermesi gerekir.

## GBM ile EGL 1.0 Arasındaki Sınır

EGL 1.0 core standardı şunları tanımlar:

```text
EGLDisplay
EGLConfig
NativeWindowType kavramı
EGLSurface
eglCreateWindowSurface()
```

GBM, EGL 1.0 core standardının parçası değildir.

Bu projede Mesa/GBM entegrasyonu native platform tarafını sağlar:

```text
EGL 1.0:
"Platform bana bir NativeWindowType sağlayacak."
                 |
                 v
Mesa / GBM:
Native window = struct gbm_surface *
```

Bu nedenle kullanılan yapı:

```text
EGL 1.0 API
    +
Mesa/GBM native platform entegrasyonu
    +
DRM/KMS display yönetimi
```

şeklindedir.

`eglCreateWindowSurface` fonksiyonunun kendisi değiştirilmez. Değişen nokta, native platformun X11/Wayland yerine GBM olmasıdır.

## Direct-to-Display Akışındaki Yeri

```text
open("/dev/dri/card*")
        |
        v
drmModeGetResources()
        |
        v
Connector / Encoder / CRTC / Mode
        |
        v
gbm_create_device()
        |
        v
gbm_surface_create()
        |
        v
EGLDisplay
        |
        v
eglInitialize()
        |
        v
eglChooseConfig()
        |
        v
eglCreateWindowSurface()
        |
        v
eglCreateContext()
        |
        v
eglMakeCurrent()
        |
        v
OpenGL ES 2.0 Render
        |
        v
eglSwapBuffers()
        |
        v
gbm_surface_lock_front_buffer()
        |
        v
GBM BO
        |
        v
DRM Framebuffer
        |
        v
drmModeSetCrtc() / drmModePageFlip()
        |
        v
Physical Monitor
```

`eglCreateWindowSurface` monitör connector'ını, CRTC'yi veya display mode'u seçmez. Bu görevler DRM/KMS tarafındadır.

## Minimal Doğru Kullanım

```c
/* DRM device daha önce açılmıştır. */
int drm_fd = /* open("/dev/dri/card0", ...) */;

/* GBM device */
struct gbm_device *gbm = gbm_create_device(drm_fd);

/* DRM mode'dan alınan boyutlar */
uint32_t width  = mode.hdisplay;
uint32_t height = mode.vdisplay;

/* Native GBM surface */
struct gbm_surface *gbm_surface =
    gbm_surface_create(
        gbm,
        width,
        height,
        GBM_FORMAT_XRGB8888,
        GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
    );

/* Daha önce GBM platformundan elde edilmiş ve initialize edilmiş EGLDisplay */
EGLDisplay egl_display = /* ... */;

/* eglChooseConfig ile seçilmiş uyumlu config */
EGLConfig egl_config = /* ... */;

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

## Parametre Değiştirme Örnekleri

### 1. `attrib_list = NULL`

```c
EGLSurface surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

```text
Değiştirilen parametre: attrib_list
Değer: NULL
Sonuç: Ek EGL 1.0 window surface attribute'u belirtilmez.
```

### 2. `attrib_list = { EGL_NONE }`

```c
const EGLint attrs[] = {
    EGL_NONE
};

EGLSurface surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    attrs
);
```

```text
Değiştirilen parametre: attrib_list
Değer: boş attribute listesi
Sonuç: Ek EGL 1.0 window surface attribute'u belirtilmez.
```

### 3. `config` değiştirme

```c
EGLSurface surface_a = eglCreateWindowSurface(
    egl_display,
    config_a,
    (EGLNativeWindowType)gbm_surface,
    NULL
);

EGLSurface surface_b = eglCreateWindowSurface(
    egl_display,
    config_b,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

```text
Değiştirilen parametre: config
Sonuç: Surface için kullanılan EGL framebuffer/pixel yapılandırması değişir.
```

### 4. `win` değiştirme

```c
EGLSurface surface_a = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface_a,
    NULL
);

EGLSurface surface_b = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface_b,
    NULL
);
```

```text
Değiştirilen parametre: win

surface_a -> gbm_surface_a
surface_b -> gbm_surface_b
```

## EGL 1.0 İçin Pratik Özet

- `eglCreateWindowSurface`, mevcut bir native window üzerinde on-screen `EGLSurface` oluşturur.
- Fonksiyon native window'u kendisi oluşturmaz.
- `dpy`, initialized `EGLDisplay` nesnesidir.
- `config`, surface'in EGL framebuffer/pixel yapılandırmasını belirler.
- `config`, window rendering için `EGL_WINDOW_BIT` desteklemelidir.
- `win`, platformun sağladığı native window handle'ıdır.
- Bu projede `win`, Mesa/GBM tarafından oluşturulan `struct gbm_surface *` nesnesidir.
- `attrib_list`, EGL 1.0 core'da normal olarak `NULL` veya `{ EGL_NONE }` kullanılır.
- Başarılı çağrı `EGLSurface`, başarısız çağrı `EGL_NO_SURFACE` döndürür.
- GBM, EGL 1.0 core standardının parçası değildir; native platform entegrasyonunu sağlar.
- DRM/KMS fiziksel display ve scan-out işlemlerini yönetir; EGL/OpenGL ES rendering tarafını yönetir.
- 

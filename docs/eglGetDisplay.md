# EGL 1.0: `eglGetDisplay`

```c
EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id);
```

`eglGetDisplay`, native görüntüleme sistemine ait bir display tanımlayıcısını EGL tarafından kullanılabilecek bir `EGLDisplay` handle'ı ile ilişkilendirmek için kullanılır.

Kısa özet:

- Girdi: `EGLNativeDisplayType`
- Çıktı: `EGLDisplay`
- Başarısızlık değeri: `EGL_NO_DISPLAY`
- `EGL_DEFAULT_DISPLAY`, varsayılan native display'i istemek için kullanılır.
- `EGLDisplay`, fiziksel monitörün kendisi değil, EGL'nin kullandığı opaque handle'dır.
- `eglGetDisplay` display'i initialize etmez; bunun için `eglInitialize` gerekir.

## Mental Model

```text
Native display
    |
    v
eglGetDisplay
    |
    v
EGLDisplay handle
    |
    v
eglInitialize
```

`eglGetDisplay` native platform ile EGL arasındaki ilk bağlantı noktalarından biridir.

## Parametreler

### `display_id`

`display_id`, hangi native display'in EGL tarafında temsil edileceğini belirtir.

`EGLNativeDisplayType` platforma bağımlıdır. Ubuntu header dosyalarında farklı platformlar için örneğin şu tanımlar bulunur:

```c
typedef struct wl_display *EGLNativeDisplayType;
typedef Display *EGLNativeDisplayType;
typedef void *EGLNativeDisplayType;
typedef int EGLNativeDisplayType;
```

Aynı anda bunların hepsi aktif değildir; platform header'ları uygun tanımı seçer.

Deney sisteminde `EGL_DEFAULT_DISPLAY` şu şekilde tanımlanmıştır:

```c
#define EGL_DEFAULT_DISPLAY EGL_CAST(EGLNativeDisplayType,0)
```

| `display_id` değeri                 | Sonuç / anlam                                                                  |
| -------------------------------------- | ------------------------------------------------------------------------------- |
| `EGL_DEFAULT_DISPLAY`                | Varsayılan native display için`EGLDisplay` istenir.                         |
| Geçerli Wayland`wl_display *`       | Bu explicit native bağlantı için`EGLDisplay` istenir.                      |
| Aynı`EGL_DEFAULT_DISPLAY` tekrar    | Test edilen Mesa implementasyonunda aynı handle döndü.                       |
| Farklı native Wayland bağlantıları | Test edilen Mesa implementasyonunda farklı`EGLDisplay` handle'ları döndü. |

## `EGLDisplay` Nedir?

`EGLDisplay`, uygulamanın iç yapısını yorumlamaması gereken bir EGL handle'ıdır.

Örnek:

```text
0x629b25a1cdf0
```

Bu değer:

- çözünürlük değildir,
- ekran numarası değildir,
- GPU numarası değildir,
- EGL sürümü değildir.

Uygulama bu handle'ı daha sonraki EGL çağrılarına geçirir:

```c
eglInitialize(display, &major, &minor);
```

`eglInitialize` ayrıntıları için ilgili bölüme bakınız.

## `EGL_DEFAULT_DISPLAY`

En temel kullanım:

```c
EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

if (dpy == EGL_NO_DISPLAY) {
    /* Display elde edilemedi. */
}
```

Bu kullanımda native display bağlantısını uygulamanın kendisinin açması gerekmez.

## Explicit Wayland Display

Deney ortamı Wayland kullandığı için açık native display bağlantısı şu şekilde oluşturuldu:

```c
struct wl_display *wayland_display = wl_display_connect(NULL);

EGLDisplay dpy =
    eglGetDisplay((EGLNativeDisplayType)wayland_display);
```

`wl_display_connect` bir EGL fonksiyonu değildir; Wayland API'sine aittir.

## Deneyler

### Test 1 — `EGL_DEFAULT_DISPLAY`

```c
EGLDisplay defaultDisplay =
    eglGetDisplay(EGL_DEFAULT_DISPLAY);
```

Gerçek çıktı:

```text
display_id       : EGL_DEFAULT_DISPLAY
Returned value   : 0x629b25a1cdf0
Result           : SUCCESS
EGL error        : EGL_SUCCESS (0x3000)
```

### Test 2 — Aynı `EGL_DEFAULT_DISPLAY` tekrar

```c
EGLDisplay defaultDisplayAgain =
    eglGetDisplay(EGL_DEFAULT_DISPLAY);
```

Gerçek çıktı:

```text
First handle     : 0x629b25a1cdf0
Second handle    : 0x629b25a1cdf0
Result           : SAME HANDLE
```

Bu, test edilen Mesa implementasyonunda aynı default display isteğinin aynı handle ile sonuçlandığını gösterir.

Bu davranışın bütün EGL implementasyonlarında aynı şekilde gerçekleşeceği varsayılmamalıdır.

### Test 3 — Explicit Wayland Display A

```c
struct wl_display *waylandDisplayA =
    wl_display_connect(NULL);

EGLDisplay eglDisplayA =
    eglGetDisplay((EGLNativeDisplayType)waylandDisplayA);
```

Gerçek çıktı:

```text
Wayland display A: 0x629b25a1da80

Returned value   : 0x629b25a25de0
Result           : SUCCESS
EGL error        : EGL_SUCCESS (0x3000)
```

### Test 4 — Explicit Wayland Display B

```c
struct wl_display *waylandDisplayB =
    wl_display_connect(NULL);

EGLDisplay eglDisplayB =
    eglGetDisplay((EGLNativeDisplayType)waylandDisplayB);
```

Gerçek çıktı:

```text
Wayland display B: 0x629b25a21c30
EGLDisplay B      : 0x629b25a268b0
```

Karşılaştırma:

```text
Default EGLDisplay: 0x629b25a1cdf0
EGLDisplay A      : 0x629b25a25de0
EGLDisplay B      : 0x629b25a268b0

A vs B            : DIFFERENT HANDLES
Default vs A      : DIFFERENT HANDLES
Default vs B      : DIFFERENT HANDLES
```

## Deney Sonuçları

| Test                           | `display_id`                  | Sonuç                       |
| ------------------------------ | ------------------------------- | ---------------------------- |
| `EGL_DEFAULT_DISPLAY`        | Default native display          | Başarılı, geçerli handle |
| `EGL_DEFAULT_DISPLAY` tekrar | Aynı input                     | Başarılı, aynı handle    |
| Wayland Display A              | Explicit native display         | Başarılı, farklı handle  |
| Wayland Display B              | İkinci explicit native display | Başarılı, farklı handle  |

## `eglGetDisplay` ve `eglInitialize` İlişkisi

```text
eglGetDisplay
    |
    v
EGLDisplay handle elde edilir
    |
    v
eglInitialize
    |
    v
Display EGL kullanımı için initialize edilir
```

`eglGetDisplay` başarılı olsa bile display henüz initialize edilmiş sayılmaz.

## Minimal Kullanım

```c
EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

if (dpy == EGL_NO_DISPLAY) {
    EGLint err = eglGetError();
}
```

## EGL 1.0 İçin Pratik Özet

- `eglGetDisplay` bir native display'den `EGLDisplay` handle'ı elde eder.
- Tek parametresi `display_id`'dir.
- `display_id`, `EGLNativeDisplayType` türündedir ve platforma bağımlıdır.
- `EGL_DEFAULT_DISPLAY`, varsayılan native display'i istemek için kullanılır.
- Geçerli explicit native display değerleri de verilebilir.
- Başarısızlıkta `EGL_NO_DISPLAY` döner.
- `EGLDisplay` opaque handle'dır; sayısal değeri yorumlanmamalıdır.
- `eglGetDisplay` display'i initialize etmez; sonraki adım tipik olarak `eglInitialize`'dır.


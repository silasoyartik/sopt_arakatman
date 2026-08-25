# EGL 1.0: `eglGetDisplay`, `eglInitialize`, `eglChooseConfig`, `eglGetCurrentDisplay`

Bu bölümde `eglGetDisplay`, `eglInitialize`, `eglChooseConfig` ve `eglGetCurrentDisplay` fonksiyonları EGL 1.0 açısından ele alınır ve Ubuntu 24.04 + Wayland + Mesa EGL ortamında yapılan gerçek deneylerle desteklenir.

Test ortamı:

```text
OS                : Ubuntu 24.04 LTS (Noble)
Session           : Wayland
DISPLAY           : :0
Virtualization    : Oracle VirtualBox
Graphics adapter  : VMware SVGA II Adapter
EGL vendor        : Mesa Project
EGL runtime       : 1.5
EGL client APIs   : OpenGL, OpenGL_ES
EGL driver        : swrast
IDE               : CLion
Build system      : CMake
Language          : C
```

> Bu çalışma EGL 1.0 fonksiyonlarını inceler. Runtime'ın EGL 1.5 olması, burada kullanılan dört fonksiyonun EGL 1.0 kapsamındaki davranışlarının incelenmesine engel değildir. Deneysel sonuçlar Mesa implementasyonunda gözlenen sonuçlardır; EGL 1.0 tarafından garanti edilen kurallarla implementation-specific gözlemler ayrı değerlendirilmelidir.

---

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

| `display_id` değeri | Sonuç / anlam |
|---|---|
| `EGL_DEFAULT_DISPLAY` | Varsayılan native display için `EGLDisplay` istenir. |
| Geçerli Wayland `wl_display *` | Bu explicit native bağlantı için `EGLDisplay` istenir. |
| Aynı `EGL_DEFAULT_DISPLAY` tekrar | Test edilen Mesa implementasyonunda aynı handle döndü. |
| Farklı native Wayland bağlantıları | Test edilen Mesa implementasyonunda farklı `EGLDisplay` handle'ları döndü. |

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

| Test | `display_id` | Sonuç |
|---|---|---|
| `EGL_DEFAULT_DISPLAY` | Default native display | Başarılı, geçerli handle |
| `EGL_DEFAULT_DISPLAY` tekrar | Aynı input | Başarılı, aynı handle |
| Wayland Display A | Explicit native display | Başarılı, farklı handle |
| Wayland Display B | İkinci explicit native display | Başarılı, farklı handle |

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

---

# EGL 1.0: `eglInitialize`

```c
EGLBoolean eglInitialize(EGLDisplay dpy,
                         EGLint *major,
                         EGLint *minor);
```

`eglInitialize`, bir `EGLDisplay` bağlantısını EGL kullanımı için initialize eder ve istenirse EGL implementation sürümünü `major` ve `minor` output parametreleri üzerinden döndürür.

Kısa özet:

- `dpy`: initialize edilecek EGL display.
- `major`: major sürüm output pointer'ı.
- `minor`: minor sürüm output pointer'ı.
- Başarılı çağrı: `EGL_TRUE`
- Başarısız çağrı: `EGL_FALSE`
- Geçersiz display deneyinde: `EGL_BAD_DISPLAY`

## Mental Model

```text
Native display
    |
    v
eglGetDisplay
    |
    v
EGLDisplay
    |
    v
eglInitialize
    |
    +-- EGL bağlantısı initialize edilir
    |
    +-- major/minor istenirse sürüm bilgisi alınır
```

## Parametreler

### `dpy`

`dpy`, initialize edilecek geçerli bir `EGLDisplay` olmalıdır.

| `dpy` | Sonuç |
|---|---|
| Geçerli fresh `EGLDisplay` | Initialization yapılabilir. |
| `EGL_NO_DISPLAY` | Deneyde `EGL_FALSE`, `EGL_BAD_DISPLAY`. |

Normal akış:

```c
EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
eglInitialize(dpy, &major, &minor);
```

`eglGetDisplay` için ilgili bölüme bakınız.

### `major`

`major`, major EGL sürüm numarasının yazılabileceği `EGLint *` output parametresidir.

```c
EGLint major = -1;
eglInitialize(dpy, &major, &minor);
```

Deneylerde `-1` yalnızca değişkenin çağrı sırasında değiştirilip değiştirilmediğini görmek için sentinel değer olarak kullanıldı.

### `minor`

`minor`, minor EGL sürüm numarasının yazılabileceği `EGLint *` output parametresidir.

```c
EGLint minor = -1;
eglInitialize(dpy, &major, &minor);
```

Normal testte:

```text
major = 1
minor = 5
```

elde edildi.

Bu değer test edilen Mesa EGL 1.5 runtime'ına aittir.

## `NULL` Kullanımı

Deneylerde şu kombinasyonlar test edildi:

```c
eglInitialize(dpy, &major, &minor);
eglInitialize(dpy, NULL, &minor);
eglInitialize(dpy, &major, NULL);
eglInitialize(dpy, NULL, NULL);
```

Her test için fresh bir Wayland native display bağlantısı oluşturuldu. Böylece önceki initialization çağrılarının sonraki deneylere etkisi azaltıldı.

## Deneyler

### Test 1 — `major` ve `minor` birlikte

```c
EGLint major = -1;
EGLint minor = -1;

EGLBoolean result =
    eglInitialize(display, &major, &minor);
```

Gerçek çıktı:

```text
major before     : -1
minor before     : -1
Return value     : EGL_TRUE
major after      : 1
minor after      : 5
EGL error        : EGL_SUCCESS (0x3000)
```

### Test 2 — `major = NULL`

```c
EGLint minor = -1;

EGLBoolean result =
    eglInitialize(display, NULL, &minor);
```

Gerçek çıktı:

```text
major            : NULL
minor            : &minor
Return value     : EGL_TRUE
minor after      : -1
EGL error        : EGL_SUCCESS (0x3000)
```

Test edilen Mesa implementasyonunda çağrı başarılı oldu ancak `minor` değeri değişmedi.

### Test 3 — `minor = NULL`

```c
EGLint major = -1;

EGLBoolean result =
    eglInitialize(display, &major, NULL);
```

Gerçek çıktı:

```text
major            : &major
minor            : NULL
Return value     : EGL_TRUE
major after      : -1
EGL error        : EGL_SUCCESS (0x3000)
```

Test edilen Mesa implementasyonunda çağrı başarılı oldu ancak `major` değeri değişmedi.

> Bu iki sonuç implementation-specific deney gözlemidir. “Bütün EGL implementasyonları tek output pointer verildiğinde aynı davranışı gösterir” şeklinde genellenmemelidir.

### Test 4 — `major = NULL`, `minor = NULL`

```c
EGLBoolean result =
    eglInitialize(display, NULL, NULL);
```

Gerçek çıktı:

```text
major            : NULL
minor            : NULL
Return value     : EGL_TRUE
EGL error        : EGL_SUCCESS (0x3000)
```

Bu test, sürüm output'ları alınmadan da initialization yapılabildiğini gösterdi.

### Test 5 — `dpy = EGL_NO_DISPLAY`

```c
EGLint major = -1;
EGLint minor = -1;

EGLBoolean result =
    eglInitialize(EGL_NO_DISPLAY, &major, &minor);
```

Gerçek çıktı:

```text
dpy              : EGL_NO_DISPLAY
Return value     : EGL_FALSE
major after      : -1
minor after      : -1
EGL error        : EGL_BAD_DISPLAY (0x3008)
```

## Deney Sonuçları

| `dpy` | `major` | `minor` | Sonuç |
|---|---|---|---|
| Geçerli | pointer | pointer | `EGL_TRUE`, `1.5` |
| Geçerli | `NULL` | pointer | `EGL_TRUE`, testte `minor` değişmedi |
| Geçerli | pointer | `NULL` | `EGL_TRUE`, testte `major` değişmedi |
| Geçerli | `NULL` | `NULL` | `EGL_TRUE` |
| `EGL_NO_DISPLAY` | pointer | pointer | `EGL_FALSE`, `EGL_BAD_DISPLAY` |

## Hata Matrisi

| Durum | Sonuç |
|---|---|
| Geçerli display, iki output pointer | `EGL_TRUE`; testte sürüm `1.5` |
| Geçerli display, iki output pointer `NULL` | `EGL_TRUE` |
| `dpy == EGL_NO_DISPLAY` | `EGL_FALSE`, `EGL_BAD_DISPLAY` |
| Initialization sırasında driver warning görülmesi | Tek başına EGL API hatası anlamına gelmez. |

## Mesa / Driver Warning Notu

Bazı testlerde:

```text
MESA: error: ZINK: failed to choose pdev
libEGL warning: egl: failed to create dri2 screen
libEGL warning: DRI3 error: Could not get DRI3 device
```

mesajları görüldü.

Buna rağmen ilgili EGL çağrılarında:

```text
EGL_TRUE
EGL_SUCCESS
```

elde edilebildi.

Bu nedenle driver warning ile EGL API hata sonucu birbirinden ayrılmalıdır.

## Minimal Kullanım

```c
EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

if (dpy == EGL_NO_DISPLAY) {
    return 1;
}

EGLint major;
EGLint minor;

if (!eglInitialize(dpy, &major, &minor)) {
    EGLint err = eglGetError();
    return 1;
}

printf("EGL version: %d.%d\n", major, minor);
```

## Bilerek Hata Üretme Örneği

```c
EGLint major = -1;
EGLint minor = -1;

if (!eglInitialize(EGL_NO_DISPLAY, &major, &minor)) {
    EGLint err = eglGetError(); /* Testte EGL_BAD_DISPLAY */
}
```

## EGL 1.0 İçin Pratik Özet

- `eglInitialize`, `EGLDisplay` bağlantısını EGL kullanımı için initialize eder.
- `dpy`, geçerli bir `EGLDisplay` olmalıdır.
- `major` ve `minor`, sürüm bilgisini almak için kullanılan output pointer'larıdır.
- Testte normal kullanım `1.5` döndürdü; bu runtime sürümüdür.
- `major` ve `minor` birlikte `NULL` verildiğinde test edilen implementasyonda çağrı başarılı oldu.
- `EGL_NO_DISPLAY` kullanımı `EGL_FALSE` ve `EGL_BAD_DISPLAY` üretti.
- Driver warning mesajları ile EGL API hata sonucu aynı şey değildir.

---

# EGL 1.0: `eglChooseConfig`

```c
EGLBoolean eglChooseConfig(EGLDisplay dpy,
                           const EGLint *attrib_list,
                           EGLConfig *configs,
                           EGLint config_size,
                           EGLint *num_config);
```

`eglChooseConfig`, bir `EGLDisplay` üzerinde bulunan EGL configuration'ları uygulamanın verdiği attribute kriterlerine göre filtreleyip sıralamak için kullanılır.

Kısa özet:

- `dpy`: config'lerin aranacağı initialized display.
- `attrib_list`: attribute/value çiftleri.
- `configs`: eşleşen `EGLConfig` handle'larının yazılacağı buffer.
- `config_size`: output buffer kapasitesi.
- `num_config`: döndürülen config sayısının yazıldığı output pointer.
- Uygun config bulunmaması API hatası olmak zorunda değildir.
- `EGL_TRUE` + `num_config = 0` geçerli bir sonuçtur.

## Mental Model

```text
EGLDisplay
    |
    +-- Config #1
    +-- Config #2
    +-- Config #3
    +-- ...
            |
            v
      attrib_list kriterleri
            |
            v
      eglChooseConfig
            |
            v
      Eşleşen EGLConfig'ler
```

## Parametreler

### `dpy`

`dpy`, config seçim işleminin yapılacağı initialized `EGLDisplay`'dir.

| Değer | Sonuç |
|---|---|
| Geçerli initialized display | Config seçimi yapılabilir. |
| `EGL_NO_DISPLAY` | Deneyde `EGL_FALSE`, `EGL_BAD_DISPLAY`. |

### `attrib_list`

`attrib_list`, attribute/value çiftlerinden oluşur ve `EGL_NONE` ile sonlandırılır.

Örnek:

```c
const EGLint attrs[] = {
    EGL_RED_SIZE,   8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE,  8,
    EGL_NONE
};
```

Kavramsal görünüm:

```text
Attribute         Value
-----------------------
EGL_RED_SIZE        8
EGL_GREEN_SIZE      8
EGL_BLUE_SIZE       8
EGL_NONE            -
```

#### `EGL_NONE`

Listenin sonunu belirtir:

```c
const EGLint attrs[] = {
    EGL_RED_SIZE, 8,
    EGL_NONE
};
```

#### `attrib_list = NULL`

Deney:

```c
eglChooseConfig(dpy, NULL, NULL, 0, &num_config);
```

Gerçek sonuç:

```text
Return value     : EGL_TRUE
Matching configs : 50
EGL error        : EGL_SUCCESS (0x3000)
```

Belirtilmeyen attribute'lar EGL'nin default seçim değerleriyle değerlendirilir.

### `configs`

`configs`, eşleşen `EGLConfig` handle'larının yazılacağı output buffer'dır.

```c
EGLConfig configs[5];
```

Yalnızca toplam eşleşme sayısını öğrenmek için `NULL` kullanılabilir:

```c
eglChooseConfig(dpy, attrs, NULL, 0, &count);
```

### `config_size`

`config_size`, `configs` buffer'ının kapasitesidir.

```c
EGLConfig configs[5];
eglChooseConfig(dpy, attrs, configs, 5, &num_config);
```

`config_size = 5`, “5 config bul” değil, “output buffer en fazla 5 config alabilir” anlamına gelir.

### `num_config`

`num_config`, döndürülen config sayısının yazıldığı `EGLint *` output parametresidir.

```c
EGLint num_config = -1;
eglChooseConfig(dpy, attrs, configs, 5, &num_config);
```

Deneyde `num_config = NULL`:

```text
EGL_FALSE
EGL_BAD_PARAMETER
```

sonucunu verdi.

## EGL 1.0 Seçim Attribute'ları

Bu çalışmada özellikle şu EGL 1.0 attribute'ları test edildi:

| Attribute | Anlam |
|---|---|
| `EGL_RED_SIZE` | Red component bit sayısı için minimum kriter |
| `EGL_GREEN_SIZE` | Green component bit sayısı için minimum kriter |
| `EGL_BLUE_SIZE` | Blue component bit sayısı için minimum kriter |
| `EGL_ALPHA_SIZE` | Alpha component bit sayısı için minimum kriter |
| `EGL_DEPTH_SIZE` | Depth buffer bit sayısı için minimum kriter |
| `EGL_STENCIL_SIZE` | Stencil buffer bit sayısı için minimum kriter |
| `EGL_LEVEL` | Framebuffer level |
| `EGL_NONE` | Attribute listesinin sonu |
| `EGL_DONT_CARE` | Uygun attribute'larda bu kriteri önemseme |

> Seçilen config'in gerçek attribute değerlerini okumak için `eglGetConfigAttrib` kullanılır. Ayrıntılar için `eglGetConfigAttrib` bölümüne bakınız.

## Size Attribute'larında Minimum Mantığı

Şu ifade:

```c
EGL_RED_SIZE, 8
```

“red değeri kesin olarak 8 olmalı” şeklinde yorumlanmamalıdır.

Size attribute'larında istek minimum gereksinim olarak değerlendirilir.

Bu nedenle:

```text
EGL_RED_SIZE >= 8
```

şeklinde düşünmek daha doğrudur.

## `attrib_list` Deneyleri

Aynı çağrı yapısı kullanıldı:

```c
eglChooseConfig(
    display,
    attributes,
    NULL,
    0,
    &num_config
);
```

Böylece `configs` ve `config_size` sabit tutularak yalnızca `attrib_list` etkisi incelendi.

### Test 1 — `attrib_list = NULL`

```text
Matching configs : 50
Return value     : EGL_TRUE
EGL error        : EGL_SUCCESS
```

### Test 2 — RGB 8/8/8

```c
const EGLint rgb888[] = {
    EGL_RED_SIZE,   8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE,  8,
    EGL_NONE
};
```

Sonuç:

```text
Matching configs : 50
```

### Test 3 — RGB 5/6/5

```c
const EGLint rgb565[] = {
    EGL_RED_SIZE,   5,
    EGL_GREEN_SIZE, 6,
    EGL_BLUE_SIZE,  5,
    EGL_NONE
};
```

Sonuç:

```text
Matching configs : 50
```

### Test 4 — RGBA 8/8/8/8

```c
const EGLint rgba8888[] = {
    EGL_RED_SIZE,   8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE,  8,
    EGL_ALPHA_SIZE, 8,
    EGL_NONE
};
```

Sonuç:

```text
Matching configs : 30
```

### Test 5 — RGB888 + Depth 16

```c
const EGLint depth16[] = {
    EGL_RED_SIZE,   8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE,  8,
    EGL_DEPTH_SIZE, 16,
    EGL_NONE
};
```

Sonuç:

```text
Matching configs : 40
```

### Test 6 — RGB888 + Depth 24

```c
const EGLint depth24[] = {
    EGL_RED_SIZE,   8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE,  8,
    EGL_DEPTH_SIZE, 24,
    EGL_NONE
};
```

Sonuç:

```text
Matching configs : 30
```

### Test 7 — RGB888 + Depth 24 + Stencil 8

```c
const EGLint depth24_stencil8[] = {
    EGL_RED_SIZE,     8,
    EGL_GREEN_SIZE,   8,
    EGL_BLUE_SIZE,    8,
    EGL_DEPTH_SIZE,   24,
    EGL_STENCIL_SIZE, 8,
    EGL_NONE
};
```

Sonuç:

```text
Matching configs : 10
```

### Test 8 — Aşırı büyük gereksinimler

```c
const EGLint impossible_config[] = {
    EGL_RED_SIZE,     64,
    EGL_GREEN_SIZE,   64,
    EGL_BLUE_SIZE,    64,
    EGL_DEPTH_SIZE,   64,
    EGL_STENCIL_SIZE, 32,
    EGL_NONE
};
```

Sonuç:

```text
Return value     : EGL_TRUE
Matching configs : 0
EGL error        : EGL_SUCCESS (0x3000)
```

Bu test önemli bir ayrımı gösterir:

```text
0 config bulundu
        !=
eglChooseConfig başarısız oldu
```

### Test 9 — Alpha `EGL_DONT_CARE`

```c
const EGLint dont_care_alpha[] = {
    EGL_RED_SIZE,   8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE,  8,
    EGL_ALPHA_SIZE, EGL_DONT_CARE,
    EGL_NONE
};
```

Sonuç:

```text
Matching configs : 50
```

Karşılaştırma:

```text
Alpha >= 8          -> 30
Alpha DONT_CARE     -> 50
```

## `attrib_list` Sonuç Tablosu

| Kriter | Eşleşen config |
|---|---:|
| `attrib_list = NULL` | 50 |
| RGB 8/8/8 | 50 |
| RGB 5/6/5 | 50 |
| RGBA 8/8/8/8 | 30 |
| RGB888 + Depth 16 | 40 |
| RGB888 + Depth 24 | 30 |
| RGB888 + Depth 24 + Stencil 8 | 10 |
| Çok yüksek gereksinimler | 0 |
| RGB888 + Alpha `EGL_DONT_CARE` | 50 |

Bu sistemde kriterler sıkılaştıkça genel olarak eşleşen config sayısı azaldı.

Bu sayıların implementasyona bağlı olduğu unutulmamalıdır.

## `configs`, `config_size`, `num_config` Deneyleri

Bu testlerde attribute listesi sabit tutuldu:

```c
const EGLint attributes[] = {
    EGL_RED_SIZE,     8,
    EGL_GREEN_SIZE,   8,
    EGL_BLUE_SIZE,    8,
    EGL_DEPTH_SIZE,   24,
    EGL_STENCIL_SIZE, 8,
    EGL_NONE
};
```

Bu kriterlerle toplam 10 config eşleşti.

### `configs = NULL`, `config_size = 0`

```c
eglChooseConfig(
    display,
    attributes,
    NULL,
    0,
    &num1
);
```

Sonuç:

```text
num_config after : 10
Return value     : EGL_TRUE
EGL error        : EGL_SUCCESS
```

Bu kullanım toplam eşleşme sayısını sorgular.

### `config_size = 1`

```c
EGLConfig configs[1];
eglChooseConfig(display, attributes, configs, 1, &num_config);
```

Sonuç:

```text
num_config after : 1
configs[0]       : 0x55df27b2fa30
```

### `config_size = 5`

Sonuç:

```text
num_config after : 5

configs[0] : 0x55df27b2fa30
configs[1] : 0x55df27b2fd70
configs[2] : 0x55df27b2ff50
configs[3] : 0x55df27b31b30
configs[4] : 0x55df27b2f210
```

### `config_size = 100`

Toplam uygun config sayısı 10 olduğu için:

```text
num_config after : 10
Return value     : EGL_TRUE
EGL error        : EGL_SUCCESS
```

elde edildi.

### `config_size` Karşılaştırması

| Toplam uygun config | `config_size` | `num_config` |
|---:|---:|---:|
| 10 | 1 | 1 |
| 10 | 5 | 5 |
| 10 | 100 | 10 |

## `num_config = NULL`

```c
eglChooseConfig(
    display,
    attributes,
    configs,
    5,
    NULL
);
```

Gerçek sonuç:

```text
Return value : EGL_FALSE
EGL error    : EGL_BAD_PARAMETER (0x300C)
```

## Negatif `config_size`

Deney:

```c
eglChooseConfig(
    display,
    attributes,
    configs,
    -1,
    &num_config
);
```

Gerçek sonuç:

```text
config_size      : -1
num_config after : -1
Return value     : EGL_TRUE
EGL error        : EGL_SUCCESS
```

Bu davranış test edilen Mesa implementasyonunda gözlendi.

Negatif buffer kapasitesi normal ve portable bir kullanım değildir; bu sonucu EGL uygulamalarında güvenilecek bir davranış olarak kullanmamak gerekir.

## Hata Deneyleri

### `dpy = EGL_NO_DISPLAY`

```c
eglChooseConfig(
    EGL_NO_DISPLAY,
    valid_attributes,
    NULL,
    0,
    &num_config
);
```

Sonuç:

```text
Return value : EGL_FALSE
num_config   : -1
EGL error    : EGL_BAD_DISPLAY (0x3008)
```

### Tanınmayan attribute

```c
const EGLint unknown_attribute[] = {
    0x12345678, 1,
    EGL_NONE
};
```

Sonuç:

```text
Return value : EGL_FALSE
num_config   : -1
EGL error    : EGL_BAD_ATTRIBUTE (0x3004)
```

### `EGL_LEVEL = EGL_DONT_CARE`

```c
const EGLint invalid_level[] = {
    EGL_LEVEL, EGL_DONT_CARE,
    EGL_NONE
};
```

Sonuç:

```text
Return value : EGL_FALSE
num_config   : -1
EGL error    : EGL_BAD_ATTRIBUTE (0x3004)
```

### Hata Sonrası Kontrol

Hata testlerinden sonra normal RGB888 seçimi tekrar yapıldı:

```text
Return value : EGL_TRUE
num_config   : 50
EGL error    : EGL_SUCCESS
```

## Hata Matrisi

| Durum | Sonuç |
|---|---|
| Geçerli display + geçerli attribute listesi | `EGL_TRUE` |
| Uygun config yok | `EGL_TRUE`, `num_config = 0` |
| `dpy == EGL_NO_DISPLAY` | `EGL_FALSE`, `EGL_BAD_DISPLAY` |
| Tanınmayan attribute | `EGL_FALSE`, `EGL_BAD_ATTRIBUTE` |
| Geçersiz attribute/value | `EGL_FALSE`, `EGL_BAD_ATTRIBUTE` |
| `num_config == NULL` | `EGL_FALSE`, `EGL_BAD_PARAMETER` |
| Negatif `config_size` | Mesa testinde `EGL_TRUE`, output değişmedi; portable kullanım değildir |

## Minimal Sayı Sorgulama

```c
EGLint count = 0;

if (!eglChooseConfig(
        dpy,
        attrs,
        NULL,
        0,
        &count)) {
    EGLint err = eglGetError();
}
```

## Minimal Config Alma

```c
EGLConfig configs[5];
EGLint count = 0;

if (!eglChooseConfig(
        dpy,
        attrs,
        configs,
        5,
        &count)) {
    EGLint err = eglGetError();
}
```

## `eglGetConfigAttrib` ile İlişki

`eglChooseConfig`, config seçer.

Seçilen config'in gerçek özelliklerini okumak için:

```c
eglGetConfigAttrib(dpy, config, EGL_RED_SIZE, &value);
```

kullanılır.

`eglGetConfigAttrib` fonksiyonunun ayrıntıları için ilgili bölüme bakınız.

## EGL 1.0 İçin Pratik Özet

- `eglChooseConfig` beş parametre alır.
- `attrib_list`, attribute/value çiftlerinden oluşur ve `EGL_NONE` ile biter.
- Size attribute'ları minimum gereksinim gibi değerlendirilir.
- `configs = NULL`, yalnızca eşleşme sayısını sorgulamak için kullanılabilir.
- `config_size`, output buffer kapasitesidir.
- `num_config`, döndürülen config sayısını verir.
- `EGL_TRUE` dönmesi mutlaka config bulunduğu anlamına gelmez.
- `EGL_TRUE` + `num_config = 0` geçerli bir sonuçtur.
- `num_config = NULL` deneyde `EGL_BAD_PARAMETER` üretti.
- Geçersiz display `EGL_BAD_DISPLAY` üretti.
- Geçersiz attribute veya attribute/value kombinasyonu `EGL_BAD_ATTRIBUTE` üretti.
- Config sayıları ve handle değerleri implementasyona bağlıdır.

---

# EGL 1.0: `eglGetCurrentDisplay`

```c
EGLDisplay eglGetCurrentDisplay(void);
```

`eglGetCurrentDisplay`, çağıran thread üzerinde current durumda olan EGL context ile ilişkili `EGLDisplay` handle'ını döndürür.

Kısa özet:

- Parametre almaz.
- Current context varsa ilişkili `EGLDisplay` döner.
- Current context yoksa `EGL_NO_DISPLAY` döner.
- Display'in yalnızca initialize edilmiş olması yeterli değildir.
- Davranışı parametreye değil current EGL state'e bağlıdır.

## Mental Model

```text
Thread
  |
  +-- current EGLContext
  |
  +-- current draw EGLSurface
  |
  +-- current read EGLSurface
  |
  +-- current EGLDisplay
```

`eglGetCurrentDisplay`, bu thread-local current state içindeki display'i sorgular.

## Parametreler

Fonksiyon parametre almaz:

```c
eglGetCurrentDisplay(void);
```

Bu nedenle görevdeki “parametreleri değiştirerek davranışı gözlemleme” yaklaşımı burada uygulanamaz.

Bunun yerine current EGL state değiştirildi.

## Deney State'leri

```text
State 1
Current context yok
    |
    v
eglGetCurrentDisplay()
    |
    v
EGL_NO_DISPLAY
```

```text
State 2
Display initialize edildi
Context/surface mevcut
Ama eglMakeCurrent çağrılmadı
    |
    v
EGL_NO_DISPLAY
```

```text
State 3
eglMakeCurrent(...)
    |
    v
Context current
    |
    v
eglGetCurrentDisplay()
    |
    v
İlgili EGLDisplay
```

```text
State 4
Current context release edildi
    |
    v
eglGetCurrentDisplay()
    |
    v
EGL_NO_DISPLAY
```

## Yardımcı Fonksiyonlar

Bu deneyi oluşturmak için şu yardımcı fonksiyonlar kullanıldı:

```text
eglGetDisplay
eglInitialize
eglChooseConfig
eglCreatePbufferSurface
eglBindAPI
eglCreateContext
eglMakeCurrent
eglDestroyContext
eglDestroySurface
eglTerminate
```

Bunlar `eglGetCurrentDisplay` fonksiyonunun parçası değildir. Ayrıntıları için kitabın ilgili bölümlerine bakınız.

Özellikle current state'i değiştiren `eglMakeCurrent` için ilgili bölüme bakınız.

## Neden Pbuffer Kullanıldı?

Test için görünür pencere gerekmiyordu.

Bu nedenle 64x64 pbuffer oluşturuldu:

```c
const EGLint pbuffer_attributes[] = {
    EGL_WIDTH,  64,
    EGL_HEIGHT, 64,
    EGL_NONE
};
```

Amaç, context'in bağlanabileceği geçerli bir surface sağlamaktı.

## Test Altyapısında İlk Context Hatası

İlk context oluşturma denemesi:

```c
eglCreateContext(
    display,
    config,
    EGL_NO_CONTEXT,
    NULL
);
```

ile yapıldı ve:

```text
EGL_BAD_ALLOC (0x3003)
```

alındı.

Bu `eglGetCurrentDisplay` hatası değildi; yardımcı context oluşturma aşamasına aitti.

Daha sonra test için açıkça OpenGL ES 2 context istendi:

```c
eglBindAPI(EGL_OPENGL_ES_API);

const EGLint context_attributes[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
};
```

Config seçiminde de:

```c
EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT
```

eklendi.

> `EGL_RENDERABLE_TYPE` ve `EGL_CONTEXT_CLIENT_VERSION`, EGL 1.0 config attribute setinin parçası değildir. Burada yalnızca modern Mesa ortamında test için gerekli yardımcı OpenGL ES 2 context'ini oluşturmak amacıyla kullanılmıştır.

## Test 1 — Current Context Yok

Programın başında:

```c
EGLDisplay current = eglGetCurrentDisplay();
```

Gerçek çıktı:

```text
eglGetCurrentDisplay() : (nil)
State                  : EGL_NO_DISPLAY
```

Bu durum normaldir; henüz current context yoktur.

## Test 2 — Display Initialize Edildi, Context Current Değil

Display:

```c
eglGetDisplay(EGL_DEFAULT_DISPLAY);
eglInitialize(display, &major, &minor);
```

ile hazırlandı.

Gerçek bilgi:

```text
Runtime version : 1.5
EGLDisplay      : 0x59fd88c23b50
```

Ancak `eglMakeCurrent` çağrılmadan:

```text
eglGetCurrentDisplay() : (nil)
State                  : EGL_NO_DISPLAY
```

elde edildi.

Bu deney:

```text
Initialized EGLDisplay
        !=
Current EGLDisplay
```

ayrımını doğrudan gösterir.

## Test 3 — Context Current

```c
eglMakeCurrent(
    display,
    surface,
    surface,
    context
);
```

sonrasında:

```c
EGLDisplay current = eglGetCurrentDisplay();
```

Gerçek çıktı:

```text
eglGetCurrentDisplay() : 0x59fd88c23b50
State                  : Valid EGLDisplay
Expected EGLDisplay    : 0x59fd88c23b50
Comparison             : SAME HANDLE
```

Current context'in ilişkili olduğu display döndü.

## Test 4 — Current Context Release

Current context şu şekilde bırakıldı:

```c
eglMakeCurrent(
    display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

Sonuç:

```text
eglGetCurrentDisplay() : (nil)
State                  : EGL_NO_DISPLAY
```

## State Karşılaştırması

| Durum | `eglGetCurrentDisplay()` |
|---|---|
| Henüz current context yok | `EGL_NO_DISPLAY` |
| Display initialized, context current değil | `EGL_NO_DISPLAY` |
| Context current | İlgili `EGLDisplay` |
| Current context release edildi | `EGL_NO_DISPLAY` |

## `eglGetDisplay` ile Farkı

`eglGetDisplay`:

```c
EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id);
```

Native display'den EGL display handle'ı elde eder.

`eglGetCurrentDisplay`:

```c
EGLDisplay eglGetCurrentDisplay(void);
```

Current context'in bağlı olduğu display'i sorgular.

```text
eglGetDisplay
native display -> EGLDisplay

eglGetCurrentDisplay
current context -> ilişkili EGLDisplay
```

## `eglInitialize` ile Farkı

```text
eglInitialize
    |
    v
Display EGL kullanımı için initialize edilir
```

Bu tek başına current state oluşturmaz.

Deneyde:

```text
eglInitialize başarılı
    |
    v
eglGetCurrentDisplay == EGL_NO_DISPLAY
```

gözlemlendi.

## `eglMakeCurrent` ile İlişki

`eglMakeCurrent`, current state'i değiştirir:

```c
eglMakeCurrent(dpy, surface, surface, ctx);
```

Başarılı çağrıdan sonra `eglGetCurrentDisplay()` ilgili display'i döndürdü.

Release:

```c
eglMakeCurrent(
    dpy,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

sonrasında tekrar `EGL_NO_DISPLAY` döndü.

`eglMakeCurrent` ayrıntıları için ilgili bölüme bakınız.

## `EGL_NO_DISPLAY` Her Zaman Hata Değildir

`eglGetCurrentDisplay()` için:

```c
EGL_NO_DISPLAY
```

current context bulunmadığında normal bir state sonucudur.

Örneğin:

```text
Program yeni başladı
Current context yok
eglGetCurrentDisplay -> EGL_NO_DISPLAY
```

Bu tek başına “EGL API hatası oluştu” anlamına gelmez.

## Thread Notu

Current state thread-local'dır.

`eglGetCurrentDisplay`:

```text
“Programın herhangi bir yerinde kullanılan display hangisi?”
```

sorusunu değil:

```text
“Bu çağrıyı yapan thread'in current context'i hangi display'e bağlı?”
```

sorusunu cevaplar.

Bu çalışmadaki testler tek thread üzerinde yapıldı.

## Minimal Kullanım

```c
EGLDisplay current = eglGetCurrentDisplay();

if (current == EGL_NO_DISPLAY) {
    printf("Current EGL display yok.\n");
} else {
    printf("Current EGLDisplay: %p\n", (void *)current);
}
```

## EGL 1.0 İçin Pratik Özet

- `eglGetCurrentDisplay` parametre almaz.
- Dönüş tipi `EGLDisplay`'dir.
- Current context varsa onun bağlı olduğu display'i döndürür.
- Current context yoksa `EGL_NO_DISPLAY` döndürür.
- Display'in yalnızca oluşturulmuş veya initialize edilmiş olması yeterli değildir.
- Current state `eglMakeCurrent` ile oluşturulur/değiştirilir.
- Current context release edilince sonuç tekrar `EGL_NO_DISPLAY` olur.
- `EGL_NO_DISPLAY` bu fonksiyonda normal bir state sonucu olabilir.
- Current state thread-local'dır.

---

# Genel Akış

Bu dört fonksiyon EGL kullanımında şu şekilde ilişkilendirilebilir:

```text
Native Display
    |
    v
eglGetDisplay
    |
    v
EGLDisplay
    |
    v
eglInitialize
    |
    v
Initialized EGLDisplay
    |
    v
eglChooseConfig
    |
    v
EGLConfig
    |
    v
surface/context oluşturma
    |
    v
eglMakeCurrent
    |
    v
current EGL state
    |
    v
eglGetCurrentDisplay
```

Bu akışta:

- `eglGetDisplay` native display'i EGL'ye bağlar.
- `eglInitialize` display bağlantısını başlatır.
- `eglChooseConfig` uygun framebuffer/config özelliklerini seçer.
- `eglGetCurrentDisplay` current context'in hangi display'e bağlı olduğunu sorgular.

Diğer EGL fonksiyonlarının ayrıntıları için kitabın ilgili bölümlerine bakınız.

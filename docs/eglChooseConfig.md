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

| Değer                       | Sonuç                                     |
| ---------------------------- | ------------------------------------------ |
| Geçerli initialized display | Config seçimi yapılabilir.               |
| `EGL_NO_DISPLAY`           | Deneyde`EGL_FALSE`, `EGL_BAD_DISPLAY`. |

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

| Attribute            | Anlam                                             |
| -------------------- | ------------------------------------------------- |
| `EGL_RED_SIZE`     | Red component bit sayısı için minimum kriter   |
| `EGL_GREEN_SIZE`   | Green component bit sayısı için minimum kriter |
| `EGL_BLUE_SIZE`    | Blue component bit sayısı için minimum kriter  |
| `EGL_ALPHA_SIZE`   | Alpha component bit sayısı için minimum kriter |
| `EGL_DEPTH_SIZE`   | Depth buffer bit sayısı için minimum kriter    |
| `EGL_STENCIL_SIZE` | Stencil buffer bit sayısı için minimum kriter  |
| `EGL_LEVEL`        | Framebuffer level                                 |
| `EGL_NONE`         | Attribute listesinin sonu                         |
| `EGL_DONT_CARE`    | Uygun attribute'larda bu kriteri önemseme        |

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

| Kriter                          | Eşleşen config |
| ------------------------------- | ---------------: |
| `attrib_list = NULL`          |               50 |
| RGB 8/8/8                       |               50 |
| RGB 5/6/5                       |               50 |
| RGBA 8/8/8/8                    |               30 |
| RGB888 + Depth 16               |               40 |
| RGB888 + Depth 24               |               30 |
| RGB888 + Depth 24 + Stencil 8   |               10 |
| Çok yüksek gereksinimler      |                0 |
| RGB888 + Alpha`EGL_DONT_CARE` |               50 |

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
| ------------------: | --------------: | -------------: |
|                  10 |               1 |              1 |
|                  10 |               5 |              5 |
|                  10 |             100 |             10 |

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

| Durum                                         | Sonuç                                                                      |
| --------------------------------------------- | --------------------------------------------------------------------------- |
| Geçerli display + geçerli attribute listesi | `EGL_TRUE`                                                                |
| Uygun config yok                              | `EGL_TRUE`, `num_config = 0`                                            |
| `dpy == EGL_NO_DISPLAY`                     | `EGL_FALSE`, `EGL_BAD_DISPLAY`                                          |
| Tanınmayan attribute                         | `EGL_FALSE`, `EGL_BAD_ATTRIBUTE`                                        |
| Geçersiz attribute/value                     | `EGL_FALSE`, `EGL_BAD_ATTRIBUTE`                                        |
| `num_config == NULL`                        | `EGL_FALSE`, `EGL_BAD_PARAMETER`                                        |
| Negatif`config_size`                        | Mesa testinde`EGL_TRUE`, output değişmedi; portable kullanım değildir |

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


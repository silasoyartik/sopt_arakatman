# EGL 1.0: `eglCreateWindowSurface` senaryolu

```c
EGLSurface eglCreateWindowSurface(
    EGLDisplay dpy,
    EGLConfig config,
    NativeWindowType win,
    const EGLint *attrib_list
);
```

## 1. Bu Fonksiyon Ne Yapar?

`eglCreateWindowSurface`, daha önce native platform tarafından oluşturulmuş bir **native window / native surface** üzerinde OpenGL ES'in çizim yapabileceği bir `EGLSurface` oluşturur.

Buradaki en önemli nokta şudur:

> `eglCreateWindowSurface()` native window'u kendisi oluşturmaz. Var olan native nesnenin üzerine EGL tarafında bir rendering surface oluşturur.

Bu projede X11 veya Wayland kullanılmadığı için native window rolünü Mesa/GBM tarafındaki:

```c
struct gbm_surface *
```

nesnesi üstlenir.

Temel kullanım:

```c
EGLSurface egl_surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

---

# 2. Önce Büyük Resim

Bu projede görüntü zinciri kabaca şöyledir:

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
eglCreateWindowSurface()
      |
      v
  EGLSurface
      |
      v
 eglMakeCurrent()
      |
      v
 OpenGL ES 2.0
```

Burada iki farklı surface kavramı vardır:

```text
struct gbm_surface *  -> GBM / native platform surface'i
EGLSurface            -> EGL / OpenGL ES rendering surface'i
```

Yani:

```text
GBM surface
    |
    | EGL'e native window olarak verilir
    v
EGLSurface
    |
    | OpenGL ES buraya render eder
    v
Rendering
```

---

# 3. Parametrelerin Genel Mantığı

Fonksiyonun dört parametresi vardır:

```c
eglCreateWindowSurface(
    dpy,
    config,
    win,
    attrib_list
);
```

Bunları basit şekilde şöyle düşünebiliriz:

```text
dpy
-> Hangi EGL display üzerinde çalışıyorum?

config
-> Surface'in framebuffer / pixel özellikleri nasıl olacak?

win
-> Hangi native surface üzerine EGLSurface oluşturuyorum?

attrib_list
-> Surface oluşturulurken ek EGL window attribute'u var mı?
```

Bu dokümanda her parametre için:

1. Parametrenin ne yaptığı,
2. Parametre değiştirilince sürecin nasıl değiştiği,
3. Örnek kod,
4. Beklenen veya kavramsal çıktı,
5. Flow chart

ayrı ayrı gösterilecektir.

> Önemli: Aşağıdaki bazı çıktılar **örnek/beklenen çıktıdır**. Gerçek çalıştırma sonucu gibi sunulmamalıdır. Amaç parametrenin sürece etkisini açıkça göstermektir.

---

# 4. Birinci Parametre: `dpy`

```c
EGLDisplay dpy
```

## 4.1 `dpy` Nedir?

`dpy`, oluşturulacak `EGLSurface` nesnesinin hangi `EGLDisplay` üzerinde oluşturulacağını belirler.

Basitçe:

```text
dpy = "Hangi EGL display bağlantısı üzerinde çalışıyorum?"
```

Bu projede display zinciri:

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

şeklindedir.

Normal kullanım:

```c
EGLSurface surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

Burada:

```text
dpy = egl_display
```

olur.

---

## 4.2 `dpy` Neyi Değiştirir?

`dpy` değiştiğinde fonksiyonun çalışacağı EGL display bağlantısı değişir.

Geçerli bir `EGLDisplay` verilirse, diğer parametreler de uygunsa surface oluşturulabilir.

Geçerli olmayan bir display verilirse surface oluşturulamaz.

Bu yüzden `dpy` için en anlaşılır karşılaştırma:

```text
Geçerli EGLDisplay
        vs
EGL_NO_DISPLAY
```

şeklindedir.

---

## 4.3 Senaryo A - Geçerli `EGLDisplay`

```c
EGLSurface surface_valid = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);

if (surface_valid != EGL_NO_SURFACE) {
    printf("VALID DPY TEST: SUCCESS\n");
} else {
    printf("VALID DPY TEST: FAILED\n");
    printf("EGL error = 0x%X\n", eglGetError());
}
```

Beklenen mantık:

```text
Geçerli EGLDisplay
        |
        v
eglCreateWindowSurface()
        |
        v
Display kabul edilir
        |
        v
Diğer parametreler de uygunsa
        |
        v
EGLSurface oluşturulur
```

Örnek beklenen terminal çıktısı:

```text
VALID DPY TEST: SUCCESS
```

---

## 4.4 Senaryo B - `EGL_NO_DISPLAY`

Bu sefer diğer üç parametre aynı tutulur.

Yalnızca:

```text
dpy
```

değiştirilir.

```c
EGLSurface surface_invalid = eglCreateWindowSurface(
    EGL_NO_DISPLAY,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);

if (surface_invalid == EGL_NO_SURFACE) {
    printf("INVALID DPY TEST: FAILED AS EXPECTED\n");

    EGLint error = eglGetError();

    printf("EGL error = 0x%X\n", error);
}
```

Beklenen mantık:

```text
EGL_NO_DISPLAY
        |
        v
eglCreateWindowSurface()
        |
        v
Geçerli EGL display yok
        |
        v
Surface oluşturulamaz
        |
        v
EGL_NO_SURFACE
        |
        v
eglGetError()
```

Örnek beklenen terminal çıktısı:

```text
INVALID DPY TEST: FAILED AS EXPECTED
EGL error = 0x....
```

Gerçek hata kodu çalıştırılan EGL implementation'dan alınmalıdır.

---

## 4.5 `dpy` İçin Flow Chart

![dpy parametresi senaryoları](image/eglCreateWindowSurface/dpy-flow.svg)

---

## 4.6 Rastgele Bir Değer Yazılırsa?

`dpy` bir `EGLDisplay` handle'ı bekler.

Bu yüzden:

```c
EGLDisplay fake_dpy =
    (EGLDisplay)0x1234;
```

gibi rastgele bir değer üretmek doğru API kullanımı değildir.

Kavramsal olarak:

```text
Rastgele değer
      |
      v
Geçerli EGLDisplay değil
      |
      v
Surface oluşturulması beklenmez
```

Kontrollü test için rastgele değer yerine:

```c
EGL_NO_DISPLAY
```

kullanmak daha doğru ve anlaşılır bir yöntemdir.

---

## 4.7 `dpy` Neyi Değiştirmez?

`dpy` aşağıdaki özellikleri doğrudan belirlemez:

```text
Çözünürlük
RGB bit sayıları
GBM pixel formatı
Native window
Surface attribute'ları
```

Yani:

```text
dpy
-> display bağlantısını seçer
```

ama:

```text
config
-> pixel/framebuffer özelliklerini

win
-> native target'ı

attrib_list
-> ek window surface attribute listesini
```

belirler.

---

# 5. İkinci Parametre: `config`

```c
EGLConfig config
```

## 5.1 `config` Nedir?

`config`, oluşturulacak surface'in framebuffer / pixel yapılandırmasını temsil eder.

Bu parametredeki senaryonun odağı config'in renk, depth veya stencil
değerlerini birbirleriyle karşılaştırmak değildir. Bu özellikler config
seçimi ve sorgulamasıyla ilgilidir. Ayrıntılar için
[`eglGetConfigAttrib`](eglGetConfigAttrib.md) dokümanına bakılmalıdır.

Bu fonksiyon açısından `config` iki temel senaryoya sahiptir:

```text
1. Geçerli EGLConfig
2. Geçersiz EGLConfig
```

---

## 5.2 Senaryo A - Geçerli `EGLConfig`

Geçerli senaryoda `config`:

```text
- dpy ile aynı EGLDisplay'a ait olmalıdır,
- EGL tarafından döndürülmüş geçerli bir handle olmalıdır,
- window surface oluşturmayı desteklemelidir.
```

Config seçilirken `EGL_SURFACE_TYPE` içinde `EGL_WINDOW_BIT`
istenmesi bu son koşulu sağlar:

```c
const EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
};

EGLConfig valid_config;
EGLint num_configs = 0;

eglChooseConfig(
    egl_display,
    config_attribs,
    &valid_config,
    1,
    &num_configs
);

EGLSurface surface = eglCreateWindowSurface(
    egl_display,
    valid_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

`num_configs > 0` ise ve diğer parametreler de geçerliyse beklenen sonuç:

```text
surface != EGL_NO_SURFACE
```

---

## 5.3 Senaryo B - Geçersiz `EGLConfig`

Geçersiz, uydurma veya `dpy` için tanımlı olmayan bir config handle'ı
verilirse surface oluşturulamaz.

```c
EGLConfig invalid_config = (EGLConfig)0;

EGLSurface surface = eglCreateWindowSurface(
    egl_display,
    invalid_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);

if (surface == EGL_NO_SURFACE) {
    printf("EGL error = 0x%X\n", eglGetError());
}
```

Beklenen sonuç:

```text
surface = EGL_NO_SURFACE
error   = EGL_BAD_CONFIG
```

> `(EGLConfig)0` yalnızca geçersiz handle senaryosunu göstermek içindir;
> geçerli bir config oluşturma yöntemi değildir.

---

## 5.4 `config` İçin Flow Chart

![config parametresi senaryoları](image/eglCreateWindowSurface/config-flow.svg)

Config'in renk, alpha, depth, stencil ve diğer attribute değerleri bu iki
senaryonun alt konusu değildir. Bu değerler
[`eglGetConfigAttrib`](eglGetConfigAttrib.md) ile incelenir.

---

# 6. Üçüncü Parametre: `win`

```c
NativeWindowType win
```

## 6.1 `win` Nedir?

`win`, EGL'in üzerine `EGLSurface` oluşturacağı native window / native surface handle'ıdır.

EGL 1.0 gerçek native window türünü platforma bırakır. Bu nedenle
`EGLNativeWindowType` projeden projeye değişir:

```text
Platform / entegrasyon   Native window kaynağı
----------------------   ---------------------------
X11                      X11 Window
Wayland                  wl_egl_window / surface
Windows                  Native window handle (HWND)
Mesa/GBM                 struct gbm_surface *
Diğer platformlar         Platformun native window handle'ı
```

Kullanılacak gerçek tür, projenin EGL platform entegrasyonuna ve EGL
header'larındaki `EGLNativeWindowType` tanımına bağlıdır.

Bu projede:

```text
Mesa/GBM -> struct gbm_surface *
```

kullanılır.

Yani:

```c
win = (EGLNativeWindowType)gbm_surface;
```

---

## 6.2 Platforma Göre `win` Senaryoları

Her projede aynı anda bütün native window türleri kullanılmaz. `dpy` hangi
platform için oluşturulduysa `win` de o platformun beklediği native nesne
olmalıdır.

### Senaryo A - X11

```c
Window x11_window = /* XCreateWindow() ile oluşturulan pencere */;

EGLSurface surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)x11_window,
    NULL
);
```

### Senaryo B - Wayland

Wayland entegrasyonunda yaygın kullanım, `wl_surface` temel alınarak
oluşturulan bir `wl_egl_window` nesnesini EGL'e vermektir.

```c
struct wl_egl_window *wayland_window = /* platform tarafında oluşturulur */;

EGLSurface surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)wayland_window,
    NULL
);
```

### Senaryo C - Windows

```c
HWND native_window = /* Win32 tarafında oluşturulan pencere */;

EGLSurface surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)native_window,
    NULL
);
```

### Senaryo D - Bu Projede Mesa/GBM

```c
struct gbm_surface *gbm_surface = /* gbm_surface_create() sonucu */;

EGLSurface surface = eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

Ortak kural:

> `win`, aktif EGL platformuyla uyumlu ve geçerli bir native window olmalıdır.

Uyumsuz, yok edilmiş veya uydurma bir native window verilirse beklenen sonuç
`EGL_NO_SURFACE`, hata ise `EGL_BAD_NATIVE_WINDOW` olur.

### Bu Projede Farklı Native Target Örneği

`win` parametresi değiştiğinde oluşturulan `EGLSurface`'in hangi native surface'e bağlı olduğu değişir.

Örneğin iki farklı GBM surface düşünelim:

```c
struct gbm_surface *gbm_surface_A;
struct gbm_surface *gbm_surface_B;
```

Bunlardan:

```c
EGLSurface surface_A =
    eglCreateWindowSurface(
        egl_display,
        egl_config,
        (EGLNativeWindowType)gbm_surface_A,
        NULL
    );
```

ve:

```c
EGLSurface surface_B =
    eglCreateWindowSurface(
        egl_display,
        egl_config,
        (EGLNativeWindowType)gbm_surface_B,
        NULL
    );
```

oluşturulabilir.

Burada:

```text
dpy         = aynı
config      = aynı
attrib_list = aynı

win         = farklı
```

olduğu için native target değişmiştir.

---

## 6.3 `win` İçin Flow Chart

![win parametresinin platform senaryoları](image/eglCreateWindowSurface/win-flow.svg)

Ana sonuç:

> `win` türü projeden projeye değişir; verilen nesne aktif EGL platformuyla
> uyumlu olmalıdır.

---

# 7. Dördüncü Parametre: `attrib_list`

```c
const EGLint *attrib_list
```

## 7.1 `attrib_list` Nedir?

Bu parametre, window surface oluşturulurken kullanılabilecek surface attribute listesini temsil eder.

Ancak `eglCreateWindowSurface()` için EGL 1.0 core standardında önemli bir durum vardır:

> EGL 1.0 core'da bu fonksiyon için tanımlanmış değiştirilebilir window-surface attribute yoktur.

Bu nedenle normal kullanım:

```c
NULL
```

veya:

```c
const EGLint attrs[] = {
    EGL_NONE
};
```

şeklindedir.

---

## 7.2 Senaryo A - `NULL`

```c
EGLSurface surfaceA =
    eglCreateWindowSurface(
        egl_display,
        egl_config,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );
```

Anlamı:

```text
attrib_list = NULL
        |
        v
Ek window surface attribute'u yok
```

---

## 7.3 Senaryo B - `{ EGL_NONE }`

```c
const EGLint attrs[] = {
    EGL_NONE
};

EGLSurface surfaceB =
    eglCreateWindowSurface(
        egl_display,
        egl_config,
        (EGLNativeWindowType)gbm_surface,
        attrs
    );
```

Burada:

```c
EGL_NONE
```

attribute listesinin bittiğini belirtir.

Yani:

```text
Liste başlıyor
      |
      v
İlk değer EGL_NONE
      |
      v
Liste hemen bitiyor
      |
      v
Ek attribute yok
```

---

## 7.4 `NULL` ile `{ EGL_NONE }` Arasında Görsel Fark Var mı?

EGL 1.0 core açısından:

```text
NULL
```

ve:

```text
{ EGL_NONE }
```

ikisi de ek window-surface attribute verilmediğini ifade eder.

Bu nedenle burada:

```text
kırmızı ekran
vs
mavi ekran
```

gibi anlamlı bir görsel fark beklenmez.

Bu parametre için en doğru kanıt türü **flow chart**'tır.

---

## 7.5 `attrib_list` İçin Flow Chart

![attrib_list parametresi senaryoları](image/eglCreateWindowSurface/attrib-list-flow.svg)

---

## 7.6 Neden Başka Bir Core Senaryo Yoktur?

Teknik olarak bir `EGLint` dizisi verilebilir:

```c
const EGLint attrs[] = {
    EGL_NONE
};
```

Ancak EGL 1.0 core'da `eglCreateWindowSurface()` için anlamlı bir window-surface attribute tanımlı değildir.

Örneğin:

```c
const EGLint attrs[] = {
    EGL_RED_SIZE, 8,
    EGL_NONE
};
```

yazmak doğru değildir.

Çünkü:

```c
EGL_RED_SIZE
```

`eglCreateWindowSurface()` için window-surface attribute değildir.

Bu tür özellikler:

```c
eglChooseConfig()
```

tarafında config seçimi için kullanılır.

Yani:

```text
EGL_RED_SIZE
      |
      v
eglChooseConfig()
```

doğrudur.

Ama:

```text
EGL_RED_SIZE
      |
      v
eglCreateWindowSurface attrib_list
```

EGL 1.0 core açısından doğru kullanım değildir.

---

## 7.7 Extension'lar Ne Olur?

Platform veya EGL extension'ları ilerleyen sürümlerde/uygulamalarda ek attribute'lar tanımlayabilir.

Ancak bunlar:

```text
EGL 1.0 core
```

davranışı değildir.

Bu dokümanın kapsamı EGL 1.0 olduğu için normal kullanım:

```c
NULL
```

veya:

```c
{ EGL_NONE }
```

olarak ele alınır.

---

# 8. Fonksiyonun Dönüş Değeri

Fonksiyonun dönüş tipi:

```c
EGLSurface
```

Başarı durumunda:

```text
geçerli EGLSurface handle
```

döner.

Başarısız olduğunda:

```c
EGL_NO_SURFACE
```

döner.

Örnek:

```c
EGLSurface surface =
    eglCreateWindowSurface(
        egl_display,
        egl_config,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );

if (surface == EGL_NO_SURFACE) {
    EGLint error = eglGetError();

    printf(
        "eglCreateWindowSurface failed: 0x%X\n",
        error
    );
}
```

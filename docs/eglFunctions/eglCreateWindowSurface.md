# EGL 1.0: `eglCreateWindowSurface`

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

## 4.3 Deney A - Geçerli `EGLDisplay`

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

## 4.4 Deney B - `EGL_NO_DISPLAY`

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

```text
                        dpy
                         |
             +-----------+-----------+
             |                       |
             v                       v
     Geçerli EGLDisplay        EGL_NO_DISPLAY
             |                       |
             v                       v
eglCreateWindowSurface()   eglCreateWindowSurface()
             |                       |
             v                       v
 Display kabul edilir       Geçerli display yok
             |                       |
             v                       v
Diğer parametreler uygunsa  Surface oluşturulamaz
             |                       |
             v                       v
        EGLSurface            EGL_NO_SURFACE
                                      |
                                      v
                                 eglGetError()
```

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

Örneğin:

```text
Red   = 8 bit
Green = 8 bit
Blue  = 8 bit
Alpha = 8 bit
```

gibi özellikler bir `EGLConfig` içerisinde bulunabilir.

Ancak önemli nokta şudur:

```c
eglCreateWindowSurface()
```

fonksiyonuna doğrudan:

```text
R = 8
G = 8
B = 8
```

değerleri verilmez.

Önce uygun bir `EGLConfig` seçilir.

Örneğin:

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

Sonra bu config:

```c
eglCreateWindowSurface(
    egl_display,
    egl_config,
    (EGLNativeWindowType)gbm_surface,
    NULL
);
```

çağrısına verilir.

---

## 5.2 `config` Neyi Değiştirir?

`config` değiştiğinde surface için kullanılacak EGL framebuffer / pixel configuration değişir.

Bunu göstermek için:

```text
Aynı dpy
Aynı win
Aynı attrib_list
```

tutulur.

Sadece:

```text
config
```

değiştirilir.

Örneğin:

```text
Config A
R = 8
G = 8
B = 8
A = 8
```

ve:

```text
Config B
R = 5
G = 6
B = 5
A = 0
```

karşılaştırılabilir.

---

## 5.3 Config A Örneği

```c
const EGLint attribsA[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE,     8,
    EGL_GREEN_SIZE,   8,
    EGL_BLUE_SIZE,    8,
    EGL_ALPHA_SIZE,   8,
    EGL_NONE
};

EGLConfig configA;
EGLint numConfigA;

eglChooseConfig(
    egl_display,
    attribsA,
    &configA,
    1,
    &numConfigA
);
```

---

## 5.4 Config B Örneği

```c
const EGLint attribsB[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE,     5,
    EGL_GREEN_SIZE,   6,
    EGL_BLUE_SIZE,    5,
    EGL_ALPHA_SIZE,   0,
    EGL_NONE
};

EGLConfig configB;
EGLint numConfigB;

eglChooseConfig(
    egl_display,
    attribsB,
    &configB,
    1,
    &numConfigB
);
```

---

## 5.5 Seçilen Config'in Gerçek Özelliklerini Kanıtlama

İstenen değer ile gerçekten seçilen `EGLConfig` aynı olmak zorunda değildir.

Bu nedenle gerçek değerler:

```c
eglGetConfigAttrib()
```

ile okunmalıdır.

Yardımcı fonksiyon:

```c
static void print_config_info(
    EGLDisplay dpy,
    EGLConfig config,
    const char *name
)
{
    EGLint r, g, b, a;

    eglGetConfigAttrib(
        dpy,
        config,
        EGL_RED_SIZE,
        &r
    );

    eglGetConfigAttrib(
        dpy,
        config,
        EGL_GREEN_SIZE,
        &g
    );

    eglGetConfigAttrib(
        dpy,
        config,
        EGL_BLUE_SIZE,
        &b
    );

    eglGetConfigAttrib(
        dpy,
        config,
        EGL_ALPHA_SIZE,
        &a
    );

    printf(
        "%s -> R:%d G:%d B:%d A:%d\n",
        name,
        r,
        g,
        b,
        a
    );
}
```

Kullanım:

```c
print_config_info(
    egl_display,
    configA,
    "CONFIG A"
);

print_config_info(
    egl_display,
    configB,
    "CONFIG B"
);
```

Örnek beklenen çıktı:

```text
CONFIG A -> R:8 G:8 B:8 A:8
CONFIG B -> R:5 G:6 B:5 A:0
```

Bu çıktı ancak implementation gerçekten bu config'leri seçerse elde edilir.

Dolayısıyla raporda gerçek test yapılmıyorsa:

```text
"Örnek / beklenen çıktı"
```

olarak belirtilmelidir.

---

## 5.6 Aynı Fonksiyonda Config'i Değiştirme

```c
EGLSurface surfaceA =
    eglCreateWindowSurface(
        egl_display,
        configA,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );

EGLSurface surfaceB =
    eglCreateWindowSurface(
        egl_display,
        configB,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );
```

Burada:

```text
dpy         = aynı
win         = aynı
attrib_list = aynı

config      = farklı
```

olduğu için surface yapılandırmasındaki farkın kaynağı `config` parametresidir.

---

## 5.7 `config` İçin Görsel Mantık

İki surface üzerinde aynı renk gradient'i çizildiğini düşünelim.

### Config A

```text
R8 G8 B8 A8
      |
      v
Daha yüksek kanal hassasiyeti
      |
      v
Daha fazla olası renk seviyesi
```

### Config B

```text
R5 G6 B5 A0
      |
      v
Bazı kanallarda daha düşük bit sayısı
      |
      v
Daha az olası renk seviyesi
```

Kavramsal gösterim:

```text
Config A
R8 G8 B8
[çok küçük renk adımları]
████████████████████████████

Config B
R5 G6 B5
[daha büyük renk adımları]
███▓▓▓▒▒▒░░░
```

Bu görsel gerçek test çıktısı değildir; bit derinliğinin renk hassasiyetine etkisini anlatan kavramsal bir gösterimdir.

---

## 5.8 `config` İçin Flow Chart

```text
                    config
                      |
          +-----------+-----------+
          |                       |
          v                       v
       Config A                Config B
      R8 G8 B8 A8             R5 G6 B5 A0
          |                       |
          v                       v
eglCreateWindowSurface()  eglCreateWindowSurface()
          |                       |
          v                       v
     EGLSurface A            EGLSurface B
          |                       |
          v                       v
Config A pixel yapısı       Config B pixel yapısı
```

---

## 5.9 Önemli Not: `eglChooseConfig()` Minimum Değer Mantığı

Şu istek:

```c
EGL_RED_SIZE, 5
```

her zaman tam olarak:

```text
RED = 5
```

olan config döneceği anlamına gelmez.

Uygulama isteği karşılayan daha yüksek özellikli bir config seçebilir.

Bu yüzden gerçek kanıt için:

```c
eglGetConfigAttrib()
```

kullanmak önemlidir.

---

# 6. Üçüncü Parametre: `win`

```c
NativeWindowType win
```

## 6.1 `win` Nedir?

`win`, EGL'in üzerine `EGLSurface` oluşturacağı native window / native surface handle'ıdır.

EGL 1.0 gerçek native window türünü platforma bırakır.

Örnek:

```text
X11      -> X11 Window
Wayland  -> Wayland surface
Windows  -> native window handle
```

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

## 6.2 `win` Neyi Değiştirir?

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

## 6.3 Somutlaştırmak İçin Kırmızı / Mavi Örneği

`surface_A` üzerinde kırmızı:

```c
eglMakeCurrent(
    egl_display,
    surface_A,
    surface_A,
    egl_context
);

glClearColor(
    1.0f,
    0.0f,
    0.0f,
    1.0f
);

glClear(GL_COLOR_BUFFER_BIT);

eglSwapBuffers(
    egl_display,
    surface_A
);
```

Kavramsal çıktı:

```text
+---------------------------+
|                           |
|       KIRMIZI SURFACE     |
|                           |
+---------------------------+
```

`surface_B` üzerinde mavi:

```c
eglMakeCurrent(
    egl_display,
    surface_B,
    surface_B,
    egl_context
);

glClearColor(
    0.0f,
    0.0f,
    1.0f,
    1.0f
);

glClear(GL_COLOR_BUFFER_BIT);

eglSwapBuffers(
    egl_display,
    surface_B
);
```

Kavramsal çıktı:

```text
+---------------------------+
|                           |
|        MAVİ SURFACE       |
|                           |
+---------------------------+
```

Bu iki kutu gerçek çalıştırılmış ekran görüntüsü değildir.

Ama şu ilişkiyi anlatır:

```text
gbm_surface_A
      |
      v
EGLSurface A
      |
      v
Kırmızı render
```

ve:

```text
gbm_surface_B
      |
      v
EGLSurface B
      |
      v
Mavi render
```

---

## 6.4 Direct-to-Display İçin Önemli Not

`eglSwapBuffers()` tek başına fiziksel monitörde hangi buffer'ın gösterileceğini seçmez.

Bu projede fiziksel görüntü zinciri devam eder:

```text
EGLSurface
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
DRM framebuffer
    |
    v
drmModeSetCrtc() / drmModePageFlip()
    |
    v
Physical Monitor
```

Dolayısıyla kırmızı/mavi örneği `win` parametresinin farklı native target'lara bağlanmasını anlatan kavramsal örnektir.

---

## 6.5 `win` İçin Flow Chart

```text
                         win
                          |
             +------------+------------+
             |                         |
             v                         v
      gbm_surface_A             gbm_surface_B
             |                         |
             v                         v
eglCreateWindowSurface()  eglCreateWindowSurface()
             |                         |
             v                         v
       EGLSurface A              EGLSurface B
             |                         |
             v                         v
       Kırmızı render              Mavi render
```

Ana sonuç:

> `win` değişirse `EGLSurface`'in bağlı olduğu native surface değişir.

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

## 7.2 `NULL` Kullanımı

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

## 7.3 `{ EGL_NONE }` Kullanımı

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

```text
                    attrib_list
                         |
             +-----------+-----------+
             |                       |
             v                       v
           NULL                 { EGL_NONE }
             |                       |
             v                       v
Ek attribute yok            Liste hemen biter
             |                       |
             +-----------+-----------+
                         |
                         v
             Ek EGL 1.0 core
          window attribute'u yok
                         |
                         v
             eglCreateWindowSurface()
                         |
                         v
                    EGLSurface
```

---

## 7.6 `NULL` Yerine Attribute Tanımlanabilir mi?

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

```text
dpy         = sabit
win         = sabit
attrib_list = sabit
config      = değişken
```

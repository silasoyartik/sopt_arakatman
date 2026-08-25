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

| Durum                                       | `eglGetCurrentDisplay()` |
| ------------------------------------------- | -------------------------- |
| Henüz current context yok                  | `EGL_NO_DISPLAY`         |
| Display initialized, context current değil | `EGL_NO_DISPLAY`         |
| Context current                             | İlgili`EGLDisplay`      |
| Current context release edildi              | `EGL_NO_DISPLAY`         |

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


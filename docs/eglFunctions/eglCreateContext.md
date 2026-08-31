# EGL 1.0 Fonksiyon İncelemesi: `eglCreateContext`

```c
EGLContext eglCreateContext(EGLDisplay dpy,
                            EGLConfig config,
                            EGLContext share_context,
                            const EGLint *attrib_list);
```

`eglCreateContext`, EGL tarafında OpenGL ES komutlarının çalışacağı rendering context'i oluşturur. Context; çizim durumunu, bağlanacak client API bilgisini ve GPU tarafında kullanılacak bazı kaynak ilişkilerini temsil eder. Tek başına ekrana çizim yapmaz. Oluşturulan context'in gerçekten çizim yapabilmesi için daha sonra bir surface ile birlikte `eglMakeCurrent` çağrısına verilmesi gerekir.

Bu çalışmanın amacı, `eglCreateContext` fonksiyonundaki dört parametrenin sadece formal birer argüman olmadığını; ekranda görülen sonucu, kaynak paylaşımını ve çalışma yolunu doğrudan etkilediğini göstermektir.

| Parametre | İncelenen fark | Senaryo klasörü |
|---|---|---|
| `dpy` | Ana ekran ile yedek ekran display seçimi | `pDpyID_farki/` |
| `config` | Depth buffer yok/var seçimi | `uConfigID_farki/` |
| `share_context` | Kaynak paylaşımı yok/var seçimi | `uShareContext_farki/` |
| `attrib_list` | EGL 1.0 varsayılan kullanım ile GLES2 context talebi | `pAttribList_farki/` |

## Genel Çalışma Modeli

![eglCreateContext genel modeli](image/eglCreateContext/eglCreateContext_genel_model.svg)

Temel fikir şudur: `eglCreateContext`, display bağlantısı (`dpy`), framebuffer özellikleri (`config`), isteğe bağlı ortak context (`share_context`) ve context attribute listesi (`attrib_list`) bilgilerini birleştirerek yeni bir `EGLContext` üretir.

```text
Native Display / GBM device
        |
        v
eglGetDisplay + eglInitialize
        |
        v
eglChooseConfig
        |
        v
eglCreateContext
        |
        v
eglCreateWindowSurface + eglMakeCurrent
        |
        v
OpenGL ES çizimi
```

Bu akışta `eglCreateContext` kritik bir eşiktir. Çünkü bu çağrıdan sonra artık hangi display üzerinde, hangi framebuffer özellikleriyle, hangi kaynak paylaşım ilişkisiyle ve hangi client API beklentisiyle çizim yapılacağı belirlenmiş olur.

## 1. Parametre: `dpy` (`EGLDisplay`)

`dpy`, context'in hangi EGL display bağlantısı üzerinde oluşturulacağını belirler. Çok ekranlı sistemlerde bu parametre, çizim yolunun ana kokpit ekranına mı yoksa yedek/standby ekrana mı bağlanacağını etkiler.

![dpy parametresi senaryoları](image/eglCreateContext/dpy_senaryolari.svg)

### Senaryo A: Ana ekran

Kaynak dosya: `pDpyID_farki/senaryo_A_ana_ekran.c`

Bu senaryoda `init_native_display_at(0)` ile 0 numaralı bağlı ekran seçilir. Ardından bu native display üzerinden `EGLDisplay` alınır ve context bu display üzerinde oluşturulur.

Beklenen çıktı:

```text
--- SENARYO A: pDpyID - Kokpit Ana Ekrani (PFD) ---
DEGER A: dpy = ana ekranin EGLDisplay handle'i.
 -> GORSEL SONUC: Ana ekran senaryosu koyu mavi zemin,
    camgobegi alt bant ve yesil ucgen cizer.
```

Görsel yorum: Ana ekran yolu koyu mavi zemin, camgöbeği alt bant ve yeşil üçgen ile temsil edilmiştir. Burada amaç, context'in 0 numaralı display yolunda oluşturulduğunu gözle seçilebilir hale getirmektir.

### Senaryo B: Yedek ekran

Kaynak dosya: `pDpyID_farki/senaryo_B_yedek_ekran.c`

Bu senaryoda `init_native_display_at(1)` ile ikinci bağlı ekran hedeflenir. Sistemde ikinci connector yoksa yardımcı kod 0 numaralı ekrana düşebilir; yine de farklı renk/desen kullanıldığı için seçilen senaryo görsel olarak ayırt edilir.

Beklenen çıktı:

```text
--- SENARYO B: pDpyID - Yedek Ekran (Standby/EICAS) ---
DEGER B: dpy = yedek ekran icin secilen EGLDisplay/native display yolu.
 -> GORSEL SONUC: Yedek ekran senaryosu kahverengi zemin,
    turuncu ust bant ve sari ucgen cizer.
```

Sonuç: `dpy` değiştiğinde context farklı native display yoluna bağlanır. Bu nedenle çizimin hangi fiziksel ya da mantıksal ekranda görüneceği `dpy` seçimiyle ilişkilidir.

## 2. Parametre: `config` (`EGLConfig`)

`config`, context'in birlikte kullanılacağı framebuffer özelliklerini belirler. Renk kanal boyutları, surface tipi, renderable API ve depth buffer gibi özellikler `EGLConfig` seçiminde yer alır. Bu çalışmada özellikle `EGL_DEPTH_SIZE` farkı incelenmiştir.

![config parametresi depth buffer farkı](image/eglCreateContext/config_depth_senaryolari.svg)

### Senaryo A: Depth buffer yok

Kaynak dosya: `uConfigID_farki/senaryo_A_derinlik_yok.c`

Bu senaryoda config attribute listesinde `EGL_DEPTH_SIZE, 0` kullanılır. Kodda depth test açılsa bile depth buffer olmadığı için z karşılaştırması beklenen şekilde çalışmaz; çizim sırası daha baskın hale gelir.

Beklenen çıktı:

```text
--- SENARYO A: uConfigID - Derinlik Tamponu Olmayan Config (EGL_DEPTH_SIZE = 0) ---
DEGER A: config = EGL_DEPTH_SIZE 0.
 -> GORSEL SONUC: Derinlik tamponu yok.
    Son cizilen mavi ucgen kirmizinin ustune biner.
```

Görsel yorum: Kırmızı üçgen önce, mavi üçgen sonra çizilir. Depth buffer olmadığı için son çizilen mavi üçgen üstte görünür.

### Senaryo B: Depth buffer var

Kaynak dosya: `uConfigID_farki/senaryo_B_derinlik_var.c`

Bu senaryoda config attribute listesinde `EGL_DEPTH_SIZE, 16` kullanılır. Aynı iki üçgen aynı sırayla çizilir; fakat depth buffer devrede olduğu için z değeri sonucu belirler.

Beklenen çıktı:

```text
--- SENARYO B: uConfigID - Derinlik Tamponu Olan Config (EGL_DEPTH_SIZE = 16) ---
DEGER B: config = EGL_DEPTH_SIZE 16.
 -> GORSEL SONUC: Derinlik tamponu var.
    Kirmizi ucgen onde kalir, mavi ucgen arkada elenir.
```

Sonuç: `config` parametresindeki depth buffer seçimi, aynı çizim komutlarının ekranda farklı sonuç üretmesine neden olur. Bu yüzden `EGLConfig`, yalnızca renk formatı seçimi değil, render davranışını etkileyen temel bir karardır.

## 3. Parametre: `share_context` (`EGLContext`)

`share_context`, yeni context'in mevcut bir context ile GL nesnelerini paylaşıp paylaşmayacağını belirler. Texture, buffer object ve benzeri bazı GL kaynakları paylaşılabilir. Buna karşılık viewport, current program, enable/disable state gibi context state'leri context'e özgü kalır.

![share_context parametresi senaryoları](image/eglCreateContext/share_context_senaryolari.svg)

### Senaryo A: Paylaşım yok

Kaynak dosya: `uShareContext_farki/senaryo_A_paylasim_yok.c`

Bu senaryoda iki context de `EGL_NO_CONTEXT` ile oluşturulur. Birinci context içinde sarı-siyah checkerboard texture oluşturulur. İkinci context'e geçildiğinde `glIsTexture(texture)` kontrolü yapılır.

Beklenen çıktı:

```text
--- SENARYO A: uShareContext - Paylasim Yok (EGL_NO_CONTEXT) ---
[VRAM] 1. baglamda sari-siyah doku yaratildi. Texture ID: <id>

DEGER A: share_context = EGL_NO_CONTEXT
 -> SONUC: 2. baglam 1. baglamin dokusunu TANIMIYOR.
 -> GORSEL SONUC: Texture paylasilamadigi icin ekranda kirmizi hata/desen alani gorulur.
```

Görsel yorum: İkinci context birinci context'te üretilen texture nesnesini tanımaz. Bu nedenle texture yerine hata/desen alanı gösterilir.

### Senaryo B: Ortak context

Kaynak dosya: `uShareContext_farki/senaryo_B_ortak_context.c`

Bu senaryoda önce `main_ctx` oluşturulur. Sonra ikinci context şu şekilde oluşturulur:

```c
EGLContext shared_ctx = eglCreateContext(display, config, main_ctx, ctx_attribs);
```

Bu kullanımda ikinci context, birinci context'in paylaşıma uygun GL nesnelerini görebilir.

Beklenen çıktı:

```text
--- SENARYO B: uShareContext - Ortak Context (Paylasimli) ---
[VRAM] 1. baglamda sari-siyah doku yaratildi. Texture ID: <id>

DEGER B: share_context = main_ctx
 -> SONUC: 2. baglam 1. baglamin dokusunu TANIYOR.
 -> GORSEL SONUC: Paylasim calisinca 1. context'te uretilen
    sari-siyah doku 2. context'te gorunur.
```

Sonuç: `share_context = EGL_NO_CONTEXT` izolasyon sağlar. Geçerli bir context handle'ı verilirse kaynak paylaşımı yapılabilir. Çok ekranlı kokpit uygulamalarında bu özellik; font, sembol, harita texture'ı ve ortak buffer gibi GPU kaynaklarının tekrar tekrar yüklenmesini önler.

## 4. Parametre: `attrib_list` (`const EGLint *`)

`attrib_list`, context oluşturulurken istenen ek özellikleri anahtar-değer çiftleriyle belirtir. Liste mutlaka `EGL_NONE` ile bitmelidir.

![attrib_list parametresi senaryoları](image/eglCreateContext/attrib_list_senaryolari.svg)

### Senaryo A: EGL 1.0 standart kullanım

Kaynak dosya: `pAttribList_farki/senaryo_A_egl10_standart.c`

Bu senaryoda attribute listesi sadece `EGL_NONE` içerir:

```c
EGLint egl10_attribs[] = { EGL_NONE };
```

Beklenen çıktı:

```text
--- SENARYO A: pAttribList - EGL 1.0 Standart Kullanimi ---
DEGER A: pAttribList = { EGL_NONE }
 -> SONUC: EGL 1.0 standart bicimde ek client-version istegi vermeden context olusturur.
 -> GORSEL SONUC: Duz gri ekran. Shader/modern pipeline talebi yapilmadi.
    Aktif OpenGL ES versiyonu: <sistemden gelen GL_VERSION>
```

Görsel yorum: Ek client-version talebi yapılmadığı için senaryo düz gri ekranla temsil edilmiştir.

### Senaryo B: Modern pipeline talebi

Kaynak dosya: `pAttribList_farki/senaryo_B_modern_pipeline.c`

Bu senaryoda attribute listesinde GLES2 context istenir:

```c
EGLint modern_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
};
```

Beklenen çıktı:

```text
--- SENARYO B: pAttribList - Modern Pipeline (EGL_CONTEXT_CLIENT_VERSION) ---
DEGER B: pAttribList = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE }
 -> SONUC: GLES2/SC2.0 icin programmable shader pipeline talep edildi.
 -> GORSEL SONUC: Koyu zemin uzerinde mavi bant ve shader ile cizilen mor ucgen gorulur.
    Aktif OpenGL ES versiyonu: <sistemden gelen GL_VERSION>
```

Sonuç: `attrib_list`, context'in hangi client API beklentisiyle oluşturulacağını somutlaştırır. EGL 1.0 temelinde attribute listesi sınırlıdır; pratik sistemlerde ise `EGL_CONTEXT_CLIENT_VERSION` gibi alanlar GLES2 ve üstü pipeline seçimi için önemlidir.

## Hata Durumları

Fonksiyon başarısız olursa `EGL_NO_CONTEXT` döner. Hata nedeni `eglGetError()` ile okunmalıdır.

| Durum | Beklenen hata |
|---|---|
| `dpy` geçerli bir display değilse | `EGL_BAD_DISPLAY` |
| `dpy` initialize edilmemişse | `EGL_NOT_INITIALIZED` |
| `config` geçersizse | `EGL_BAD_CONFIG` |
| `share_context` geçersizse | `EGL_BAD_CONTEXT` |
| Context'ler paylaşım için uyumsuzsa | `EGL_BAD_MATCH` |
| `attrib_list` geçersiz attribute içeriyorsa | `EGL_BAD_ATTRIBUTE` |
| Bellek/GPU kaynağı yetersizse | `EGL_BAD_ALLOC` |

## Güvenli Kullanım Örneği

```c
#include <EGL/egl.h>
#include <stdio.h>

EGLContext CreateCheckedContext(EGLDisplay dpy,
                                EGLConfig config,
                                EGLContext shared_ctx) {
    const EGLint attrib_list[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(dpy, config, shared_ctx, attrib_list);
    if (context == EGL_NO_CONTEXT) {
        EGLint err = eglGetError();
        fprintf(stderr, "eglCreateContext basarisiz oldu. EGL hata kodu: 0x%04x\n", err);
        return EGL_NO_CONTEXT;
    }

    return context;
}
```

## Pratik Özet

| Parametre | Değiştiğinde ne olur? | Bu projedeki somut kanıt |
|---|---|---|
| `dpy` | Context farklı display/native ekran yolu üzerinde oluşturulur. | Ana ekran ve yedek ekran farklı renk/desen üretir. |
| `config` | Framebuffer özellikleri değişir. | Depth yokken mavi üçgen üstte; depth varken kırmızı üçgen önde kalır. |
| `share_context` | GL nesnelerinin context'ler arasında paylaşılıp paylaşılmayacağı belirlenir. | Paylaşım yokken texture tanınmaz; paylaşım varken checkerboard texture görünür. |
| `attrib_list` | Context'in istenen client API/pipeline davranışı belirlenir. | `{ EGL_NONE }` gri ekran; GLES2 talebi shader ile mor üçgen üretir. |

`eglCreateContext` bu nedenle yalnızca bir handle üretme fonksiyonu değildir. Parametreleri; context'in hangi display üzerinde yaşayacağını, hangi framebuffer özelliklerini kullanacağını, hangi kaynakları paylaşacağını ve hangi API beklentisiyle çalışacağını belirler.

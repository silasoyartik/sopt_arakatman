# EGL 1.0: `eglGetConfigs`

```c
EGLBoolean eglGetConfigs(EGLDisplay dpy,
                         EGLConfig *configs,
                         EGLint config_size,
                         EGLint *num_config);
```

`eglGetConfigs`, başlatılmış bir `EGLDisplay` üzerinde desteklenen bütün `EGLConfig` yapılandırmalarını okumak veya yalnızca kaç yapılandırma bulunduğunu öğrenmek için kullanılan EGL 1.0 fonksiyonudur. Kısaca, ekran/sürücü tarafındaki framebuffer seçeneklerinin ham envanterini verir.

Bu fonksiyon seçim yapmaz, filtre uygulamaz ve sıralama garantisi vermez. “Şu özelliklerde bir config istiyorum” denecekse doğru araç genellikle `eglChooseConfig` fonksiyonudur. `eglGetConfigs` ise sistemde ne olduğunu görmek, saymak ve tüm havuzu elle incelemek için kullanılır.

![eglGetConfigs genel akış şeması](image/eglGetConfigs/eglGetConfigs_akis.svg)

## Kısa Mantık

EGL tarafında her `EGLConfig`, yüzeyin nasıl bir framebuffer kullanacağını anlatır: renk tamponu kaç bit olacak, depth buffer var mı, pencere yüzeyi destekleniyor mu, OpenGL ES ile kullanılabilir mi gibi bilgiler bu yapılandırmalarda tutulur.

```text
Native Display -> EGLDisplay -> EGLConfig havuzu
                                |
                                +-- Config #1: RGB565, depth 16, window destekli
                                +-- Config #2: RGBA8888, depth 24, pbuffer destekli
                                +-- Config #N: ...
```

Uygulama bu havuzu aldıktan sonra her config'i `eglGetConfigAttrib` ile inceleyebilir.

## Parametreler

| Parametre | Görevi | Kritik nokta |
| --- | --- | --- |
| `dpy` | Sorgunun yapılacağı `EGLDisplay` handle'ı | `eglInitialize` ile başlatılmış olmalıdır. |
| `configs` | Config handle'larının yazılacağı dizi | `NULL` verilirse sadece toplam sayı öğrenilir. |
| `config_size` | `configs` dizisinin kapasitesi | `configs == NULL` iken dikkate alınmaz; dizi varsa en fazla bu kadar config kopyalanır. |
| `num_config` | Yazılan veya bulunan config sayısının döneceği adres | Geçerli bir `EGLint*` olmalıdır. |

## Davranış Özeti

| Çağrı biçimi | Ne olur? |
| --- | --- |
| `eglGetConfigs(dpy, NULL, 0, &n)` | Config listesi kopyalanmaz; sistemdeki toplam config sayısı `n` içine yazılır. |
| `eglGetConfigs(dpy, configs, size, &n)` | En fazla `size` adet config `configs` dizisine yazılır; kopyalanan adet `n` olur. |
| `config_size` toplam config sayısından küçükse | Hata değildir. Yalnızca dizinin alabileceği kadar config döner. |
| `dpy` geçersizse | `EGL_FALSE` döner; tipik hata `EGL_BAD_DISPLAY` olur. |
| `dpy` başlatılmamışsa | `EGL_FALSE` döner; hata `EGL_NOT_INITIALIZED` olur. |

## Neden Önemli?

Bir EGL uygulamasında context ve surface oluşturmak için uygun bir `EGLConfig` gerekir. Yanlış veya eksik config listesiyle çalışmak şu sorunlara yol açabilir:

* Pencere yüzeyi (`EGL_WINDOW_BIT`) desteklenmediği için surface oluşturulamayabilir.
* OpenGL ES 2.0 desteği (`EGL_OPENGL_ES2_BIT`) olmayan config seçilebilir.
* Depth buffer olmayan config ile 3B çizimde derinlik testi beklenen sonucu vermez.
* Küçük `config_size` yüzünden uygun config havuzun dışında kalabilir.

Bu yüzden pratikte en güvenli yöntem iki adımlı sorgudur: önce toplam sayıyı öğren, sonra o sayı kadar bellek ayırıp tüm listeyi oku.

![Profesyonel iki adımlı sorgu şeması](image/eglGetConfigs/profesyonel_2_adimli.svg)

## Parametre Senaryoları

Aşağıdaki senaryolar repodaki C dosyalarıyla bire bir ilişkilidir. Çizim üreten örneklerde görsel sonuç, seçilen config ile surface/context kurulabildiğini gösterir. Çizim üretmeyen örneklerde ise doğru kanıt terminal çıktısıdır; çünkü amaç, hatalı veya bilinçli eksik parametre durumunda çizime geçilmemesi gerektiğini göstermektir.

### 1. `dpy` Parametresi

#### Senaryo A: Geçerli `EGLDisplay`

Kaynak dosya: `pDpyID_farki/senaryo_A_gecerli_display.c`

Native display açılır, bunun üzerinden `EGLDisplay` alınır ve `eglInitialize` başarılı olduktan sonra `eglGetConfigs` çağrılır. `dpy` geçerli olduğu için fonksiyon `EGL_TRUE` döner ve bulunan config sayısını `num_config` içine yazar. Kod daha sonra uygun bir config seçip yeşil zemin üzerinde beyaz üçgen çizer.

![Geçerli display ile başarılı akış](image/eglGetConfigs/dpy_gecerli_display.svg)

```text
BASARILI: Gecerli EGLDisplay ile eglGetConfigs <N> config dondurdu.
GORSEL SONUC: Secilen uygun config ile yesil zemin uzerine beyaz ucgen cizildi.
```

#### Senaryo B: Geçersiz `EGLDisplay`

Kaynak dosya: `pDpyID_farki/senaryo_B_gecersiz_display.c`

Bu senaryoda `dpy` olarak `EGL_NO_DISPLAY` verilir. Geçerli display olmadığı için `eglGetConfigs` başarısız olur ve beklenen hata `EGL_BAD_DISPLAY` değeridir. Surface veya context kurulmadığı için çizim yapılmaz.

![Terminal kanıtı akışı](image/eglGetConfigs/terminal_kanit_akisi.svg)

```text
BEKLENEN HATA: EGL_NO_DISPLAY ile eglGetConfigs basarisiz oldu.
EGL hata kodu: EGL_BAD_DISPLAY (0x3008)
GORSEL SONUC: Config alinmadigi icin context/surface olusturulmaz ve cizim yapilmaz.
```

### 2. `configs` Parametresi

#### Senaryo A: `configs = NULL` ile Sadece Sayım

Kaynak dosya: `pConfigs_farki/senaryo_A_sadece_sayim.c`

Bu kullanım iki adımlı sorgunun ilk adımıdır. `configs` parametresi `NULL`, `config_size` ise `0` verilir. EGL config listesini kopyalamaz; yalnızca toplam config sayısını `num_config` adresine yazar. Bu senaryoda çizim yapılmaması doğrudur, çünkü elde henüz kullanılacak config handle'ı yoktur.

```text
BASARILI: pConfigs=NULL ve config_size=0 ile sadece config sayisi sorgulandi.
Sistemde <N> adet EGLConfig var.
GORSEL SONUC: Bu senaryo bilerek cizim yapmaz; elde config handle olmadigi icin surface/context kurulmaz.
```

#### Senaryo B: Geçerli `configs` Dizisine Veri Okuma

Kaynak dosya: `pConfigs_farki/senaryo_B_veri_okuma.c`

Bu senaryoda `configs` geçerli bir dizi olarak verilir ve `config_size` dizinin kapasitesini belirtir. Başarılı çağrıdan sonra EGL, en fazla `config_size` kadar `EGLConfig` handle'ını diziye yazar. Kod bu config'ler arasından pencere yüzeyi ve OpenGL ES 2.0 destekleyen bir seçim yaparak lacivert zemin üzerinde sarı üçgen çizer.

![Geçerli configs dizisine veri okuma](image/eglGetConfigs/pconfigs_veri_okuma.svg)

```text
BASARILI: pConfigs gecerli dizi oldugu icin <N> config bellege kopyalandi.
GORSEL SONUC: Okunan configlerden uygun olanla lacivert zemin uzerine sari ucgen cizildi.
```

### 3. `config_size` Parametresi

#### Senaryo A: Yetersiz Kapasite

Kaynak dosya: `ConfigSize_farki/senaryo_A_yetersiz_kapasite.c`

Önce sistemdeki gerçek config sayısı öğrenilir, ardından özellikle küçük bir kapasiteyle (`config_size = 2`) okuma yapılır. EGL bunu hata saymaz; yalnızca ilk iki config'i kopyalar. Risk şudur: ihtiyaç duyulan özelliklere sahip config, okunmayan kısımda kalabilir.

![Yetersiz config_size etkisi](image/eglGetConfigs/configsize_yetersiz.svg)

```text
Sistemde toplam <T> config var, fakat ConfigSize=2 oldugu icin sadece <N> tanesi okundu.
UYARI: Ilk 2 config icinde derinlik tamponu bulunamadi; 3B derinlik testi guvenilir degil.
GORSEL SONUC: Sadece sinirli havuz kullanildigi icin dogru config secimi garanti edilmez.
```

#### Senaryo B: Yeterli Kapasite

Kaynak dosya: `ConfigSize_farki/senaryo_B_yeterli_kapasite.c`

Bu senaryoda önce toplam config sayısı alınır, sonra tam bu sayı kadar bellek ayrılır. Böylece tüm config havuzu okunur ve depth buffer destekleyen uygun config güvenli şekilde seçilebilir. 3B çizimde yeşil üçgenin kırmızı üçgenin önünde görünmesi depth buffer kullanımını somutlaştırır.

![Yeterli config_size ile tam okuma](image/eglGetConfigs/configsize_yeterli.svg)

```text
BASARILI: Yeterli kapasite ile <N>/<T> config okundu ve derinlikli uygun config secildi.
GORSEL SONUC: Depth buffer aktif; yesil ucgen onde, kirmizi ucgen arkada kalir.
```

### 4. `num_config` Parametresi

#### Senaryo A: Geçerli `num_config` İşaretçisi

Kaynak dosya: `pNumConfig_farki/senaryo_A_gecerli_isaretci.c`

`num_config` geçerli bir `EGLint*` adresidir. `eglGetConfigs`, kaç config kopyaladığını bu adrese yazar. Kod daha sonra uygun config ile surface/context oluşturup mor zemin üzerinde camgöbeği üçgen çizer.

![Geçerli num_config işaretçisi](image/eglGetConfigs/pnumconfig_gecerli.svg)

```text
BASARILI: pNumConfig gecerli isaretci oldugu icin EGL yazdigi config sayisini bildirdi: <N>
GORSEL SONUC: Uygun config ile mor zemin uzerine camgobegi ucgen cizildi.
```

#### Senaryo B: `num_config = NULL`

Kaynak dosya: `pNumConfig_farki/senaryo_B_null_verilmesi.c`

EGL 1.0 sözleşmesinde `num_config` çıkış parametresi zorunlu kabul edilmelidir. Bu parametre `NULL` verilirse bazı sürücüler temiz bir hata döndürmek yerine geçersiz adrese yazmaya çalışıp uygulamayı çökertebilir. Bu nedenle örnek kod gerçek EGL çağrısını bilerek yapmaz; üretim kodunun bu parametreyi çağrıdan önce reddetmesi gerektiğini gösterir.

```text
BEKLENEN HATA: pNumConfig=NULL EGL 1.0 icin gecersiz parametredir.
GUVENLI TEST: Bu ornek kasitli olarak eglGetConfigs(..., NULL) cagirmiyor.
NEDEN: Bazi EGL suruculeri gecersiz output pointer'i icin EGL_FALSE yerine process crash uretebilir.
DOGRU DAVRANIS: Uretim kodu pNumConfig NULL ise EGL cagrisindan once reddetmelidir.
GORSEL SONUC: Config sayisi guvenli sekilde alinamadigi icin cizim kurulmaz.
```

### 5. Profesyonel İki Adımlı Sorgu

Kaynak dosya: `harici_standart_senaryo/profesyonel_2_adimli_sorgu.c`

Bu senaryo gerçek uygulamalarda önerilen akışı gösterir:

1. `configs = NULL` ile toplam config sayısı öğrenilir.
2. Bu sayı kadar bellek ayrılır.
3. İkinci çağrıda tüm config listesi alınır.
4. Tüm havuz içinden ihtiyaçlara uygun config seçilir.

Bu yöntem hem bellek taşmasını önler hem de eksik havuz üzerinden yanlış config seçme riskini azaltır.

```text
Adim 1: Sistemde <T> adet EGLConfig oldugu tespit edildi.
Adim 2: <N> adet EGLConfig bellege alindi.
BASARILI: 2 adimli eglGetConfigs akisi ile tam config havuzu okunup uygun config secildi.
GORSEL SONUC: Depth buffer aktif; yesil ucgen kirmizi ucgenin onunde gorunur.
```

## Güvenli Kullanım Örneği

```c
#include <EGL/egl.h>
#include <stdio.h>
#include <stdlib.h>

EGLBoolean SafeGetEGLConfigs(EGLDisplay dpy) {
    EGLint total_configs = 0;

    if (dpy == EGL_NO_DISPLAY) {
        fprintf(stderr, "[Hata] Geçersiz EGLDisplay.\n");
        return EGL_FALSE;
    }

    if (eglGetConfigs(dpy, NULL, 0, &total_configs) != EGL_TRUE) {
        fprintf(stderr, "[Hata] Config sayısı okunamadı. Hata: 0x%04X\n", eglGetError());
        return EGL_FALSE;
    }

    if (total_configs <= 0) {
        printf("[Bilgi] Bu display için EGLConfig bulunamadı.\n");
        return EGL_TRUE;
    }

    EGLConfig *configs = (EGLConfig*)malloc((size_t)total_configs * sizeof(EGLConfig));
    if (configs == NULL) {
        fprintf(stderr, "[Hata] Config listesi için bellek ayrılamadı.\n");
        return EGL_FALSE;
    }

    EGLint copied_configs = 0;
    if (eglGetConfigs(dpy, configs, total_configs, &copied_configs) != EGL_TRUE) {
        fprintf(stderr, "[Hata] Config listesi okunamadı. Hata: 0x%04X\n", eglGetError());
        free(configs);
        return EGL_FALSE;
    }

    printf("Başarılı: %d/%d EGLConfig okundu.\n", copied_configs, total_configs);

    /*
     * Burada configs dizisi eglGetConfigAttrib ile incelenip
     * projenin istediği surface, renderable type, color ve depth özellikleri seçilir.
     */

    free(configs);
    return EGL_TRUE;
}
```

## Sunumda Vurgulanacak Sonuç

`eglGetConfigs` bir seçim fonksiyonu değil, config envanteri alma fonksiyonudur. En güvenli kullanım, önce toplam sayıyı öğrenmek ve sonra tüm listeyi okuyup gereken özellikleri `eglGetConfigAttrib` ile doğrulamaktır. `dpy` başlatılmış olmalı, `num_config` geçerli bir adres olmalı ve `config_size` küçük tutulursa fonksiyon hata vermese bile eksik config listesiyle çalışıldığı unutulmamalıdır.

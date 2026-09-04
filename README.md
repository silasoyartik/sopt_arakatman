# EGL Markdown sunumunu oluşturma

Her EGL fonksiyonunun ayrı Markdown dosyası asıl kaynaktır. Birleşik
`docs/eglFunctions.md` dosyası generated artifact'tır; doğrudan düzenlenmez.

## Proje yapısı

```text
docs/
├── eglFunctions/
│   ├── eglGetDisplay.md          # Fonksiyon kaynağı
│   ├── eglInitialize.md          # Fonksiyon kaynağı
│   └── ...
├── presentation-order.txt        # Birleştirme sırası
├── presentation-preamble.md      # Sunumun giriş bölümü
├── presentation-footer.md        # Sunumun son bölümü
└── eglFunctions.md               # Otomatik oluşturulan birleşik dosya
scripts/
└── build_presentation.py         # Cross-platform üretim scripti
```

Script yalnızca Python 3.8+ standart kütüphanesini kullanır. Linux, macOS ve
Windows'ta proje kökünden aynı komutla çalıştırılır:

```sh
python scripts/build_presentation.py
```

Windows'ta `python` komutu tanımlı değilse Python Launcher kullanılabilir:

```powershell
py scripts/build_presentation.py
```

Script kaynakların arasına `---` ekler, içindekiler listesini sıra dosyasından
üretir ve çıktının başına elle düzenlenmemesi gerektiğini belirten bir yorum
yazar. Çıktı zaten güncelse dosyaya yeniden yazmaz.

CI veya Git hook içinde birleşik dosyanın güncel olduğunu, dosyayı değiştirmeden
kontrol etmek için:

```sh
python scripts/build_presentation.py --check
```

Eksik dosya, yinelenen kayıt, Markdown olmayan kaynak veya generated dosyanın
kaynak olarak listelenmesi hata kabul edilir ve script sıfırdan farklı durum
koduyla sonlanır. Sıra dosyasındaki her satır tek bir yol olduğu için dosya ve
klasör adlarındaki boşluklar ayrıca kaçış karakteri gerektirmez.

## Kaynak ve generated artifact ayrımı

Kaynak olarak commit edilmesi gereken dosyalar şunlardır:

- Ayrı EGL fonksiyon dosyaları
- `docs/presentation-order.txt`
- `docs/presentation-preamble.md` ve `docs/presentation-footer.md`
- `scripts/build_presentation.py`

`docs/eglFunctions.md` generated artifact'tır. Sunumun GitHub üzerinden kolayca
okunması, diff'inin incelenmesi veya Python çalıştırmadan kullanılabilmesi
isteniyorsa commit edilmesi uygundur. Bu projede bu yaklaşım kullanılır. CI'da
`--check` çalıştırılarak unutulmuş üretimler yakalanabilir.

Generated diff'lerin gereksiz olduğu ve tüm tüketicilerin build adımını
çalıştırdığı bir projede bunun yerine `docs/eglFunctions.md` `.gitignore` içine
alınabilir. Bu durumda release veya sunum sürecinin önce scripti çalıştırması
gerekir. İki yaklaşım aynı anda kullanılmamalıdır.

## Fonksiyon ekleme, çıkarma ve sıralama

Yeni bir fonksiyon eklemek için Markdown dosyasını `docs/eglFunctions/` altına
ekleyin ve dosya yolunu `docs/presentation-order.txt` içinde istediğiniz konuma
yazın. Ardından üretim komutunu çalıştırın.

Bir fonksiyonu yalnızca sunumdan çıkarmak için sıra dosyasındaki satırını silin;
kaynak Markdown dosyasını silmek gerekmez. Sırayı değiştirmek için satırların
yerini değiştirmeniz yeterlidir.

Sıra dosyasındaki yollar dosyanın bulunduğu `docs/` klasörüne göre çözülür.
Örneğin boşluk içeren bir yol doğrudan şöyle yazılır:

```text
eglFunctions/advanced functions/egl Example.md
```

# HLR ve test isimlendirmesi

HLR, **High-Level Requirement** anlamına gelir. Bir EGL gereksinim kimliği
aşağıdaki yapıyı kullanır:

```text
GS-EGL10-CM-GCA-002
|  |     |  |   |
|  |     |  |   +-- Fonksiyona ait sıra numarası
|  |     |  +------ Fonksiyon kısaltması
|  |     +--------- Alt modül kısaltması
|  +--------------- EGL 1.0 modülü
+------------------ Proje namespace öneki
```

`GS` proje genelinde kullanılan namespace önekidir; tarihsel veya açık yazımı
bu repoda tanımlanmamıştır. `EGL10`, EGL 1.0 modülünü belirtir.

EGL alt modül kısaltmaları:

| Kısaltma | Alt modül               |
| :-------- | :----------------------- |
| `IN`    | Initialization           |
| `CM`    | Configuration Management |
| `RC`    | Rendering Contexts       |
| `RS`    | Rendering Surfaces       |
| `BP`    | Buffer Posting           |
| `ER`    | Errors                   |

EGL fonksiyon kısaltmaları:

| Kısaltma | Fonksiyon                                                                         |
| :-------- | :-------------------------------------------------------------------------------- |
| `GD`    | `eglGetDisplay`                                                                 |
| `INI`   | `eglInitialize`                                                                 |
| `TER`   | `eglTerminate`                                                                  |
| `CC`    | `eglChooseConfig` (`CM` altında) veya `eglCreateContext` (`RC` altında) |
| `GCS`   | `eglGetConfigs`                                                                 |
| `GCA`   | `eglGetConfigAttrib`                                                            |
| `DC`    | `eglDestroyContext`                                                             |
| `GCC`   | `eglGetCurrentContext`                                                          |
| `GCD`   | `eglGetCurrentDisplay`                                                          |
| `MC`    | `eglMakeCurrent`                                                                |
| `CWS`   | `eglCreateWindowSurface`                                                        |
| `DS`    | `eglDestroySurface`                                                             |
| `SB`    | `eglSwapBuffers`                                                                |
| `GE`    | `eglGetError`                                                                   |

Test dosyaları aynı kimliği tire yerine alt çizgiyle kullanır. `TP`, **Test
Procedure**; `TC` ise **Test Case** anlamına gelir. Örneğin
`GS_EGL10_CM_GCA_TP_002.c`, `GS-EGL10-CM-GCA-002` gereksiniminin test prosedürü
dosyasıdır; dosya içindeki karşılık gelen test case kimliği
`GS_EGL10_CM_GCA_TC_002` biçimindedir.

# EGL test helper kullanımı

`HLR+TP/TP/helpers.h`, EGL testlerinde tekrarlanan display, config, context ve
surface hazırlama işlemlerini ortaklaştırır. Testler bir `main` fonksiyonundan
hazır EGL nesneleri beklemez; ihtiyaç duydukları ortamı kendi `init`
fonksiyonlarında oluşturur ve `close` fonksiyonlarında tamamen temizler.

Bir test dosyası helper'ı şu şekilde dahil eder:

```c
#include <EGL/egl.h>
#include "../../helpers.h"
```

## Test environment

`GS_EGL10_TestEnvironment`, test boyunca kullanılan temel EGL durumunu tek bir
yapıda tutar:

```c
typedef struct
{
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    EGLBoolean initialized;
} GS_EGL10_TestEnvironment;
```

Her test kendi environment değişkenini güvenli boş değerlerle oluşturur:

```c
static GS_EGL10_TestEnvironment environment =
    GS_EGL10_ENV_INITIALIZER;
```

Initializer sırasıyla `EGL_NO_DISPLAY`, null config, `EGL_NO_CONTEXT`,
`EGL_NO_SURFACE` ve `EGL_FALSE` değerlerini atar. Setup yarıda başarısız olsa
bile cleanup böylece hangi nesnelerin gerçekten oluşturulduğunu ayırt edebilir.

### Display initialization

Yalnızca initialized bir display ve config gereken testlerde fonksiyonlar ayrı
ayrı kullanılabilir:

```c
if (!GS_EGL10_initialize_display(&environment) ||
    !GS_EGL10_choose_config(&environment, EGL_PBUFFER_BIT))
{
    TEST_LOG_FAIL(test_case, test_procedure,
        "Setup failed, EGL error: 0x%x", eglGetError());
    return;
}
```

`GS_EGL10_initialize_display` önce
`eglGetDisplay(EGL_DEFAULT_DISPLAY)`, ardından `eglInitialize` çağırır.
Başarılı olduğunda environment içindeki `initialized` alanını `EGL_TRUE`
yapar.

`eglInitialize` fonksiyonunun kendisini test eden prosedürlerde setup çağrısı
hedef davranışı gizlememelidir. Bu nedenle
`GS_EGL10_get_default_display` yalnızca default display'i alır ve initialize
etmez. Test, başarılı hedef çağrısından sonra environment içindeki
`initialized` alanını `EGL_TRUE` yapar; ortak cleanup böylece display'i doğru
biçimde sonlandırır.

`GS_EGL10_choose_config`, verilen surface type için `eglChooseConfig` çağırır
ve ilk uygun config'i environment içine kaydeder. En az bir config bulunmadığı
durumda `EGL_FALSE` döndürür.

`eglChooseConfig` prosedürlerinin bağımsız setup yapabilmesi için
`GS_EGL10_get_first_config` ve `GS_EGL10_prepare_config_environment`,
konfigürasyon envanterini `eglGetConfigs` ile hazırlar; setup sırasında
`eglChooseConfig` çağırmaz.

Seçim testlerinde kullanılan diğer ortak yardımcılar:

- `GS_EGL10_get_matching_config_count`, bir attribute listesi için yalnız
  eşleşme sayısını alır.
- `GS_EGL10_verify_config_selection`, dönen bütün handle'ları
  `eglGetConfigAttrib` ile bağımsız olarak doğrular. At-least, exact ve mask
  kurallarını `GS_EGL10_ConfigExpectation` girdileriyle uygular.
- `GS_EGL10_find_config_matching`, test girdisi oluşturmak için
  `eglGetConfigs` envanterinde verilen beklentileri sağlayan gerçek bir config
  arar.

### Pbuffer ortamı hazırlama

Çoğu test için gereken tam setup tek çağrıyla hazırlanabilir:

```c
if (!GS_EGL10_prepare_pbuffer_environment(
        &environment, 16, 16))
{
    TEST_LOG_FAIL(test_case, test_procedure,
        "Setup failed, EGL error: 0x%x", eglGetError());
    return;
}
```

Bu fonksiyon sırasıyla:

1. Default EGLDisplay'i alır.
2. Display'i initialize eder.
3. `EGL_PBUFFER_BIT` destekleyen bir EGLConfig seçer.
4. Bir EGLContext oluşturur.
5. Verilen genişlik ve yükseklikte bir pbuffer surface oluşturur.

Pbuffer native window gerektirmeyen off-screen bir EGLSurface'tir. Bu sayede
pek çok EGL testi X11, Wayland veya başka bir native window API'sine bağlı
olmadan hazırlanabilir.

`GS_EGL10_prepare_pbuffer_environment` context'i özellikle current yapmaz.
Böylece current olmayan surface/context hata durumları da test edilebilir.

## Platform test hook'ları

Bazı testler yalnızca standart EGL çağrıları ve pbuffer helper'ları ile
hazırlanamaz. Native window veya native pixmap oluşturmak, pencere içeriğinin
gerçekten ekrana gönderildiğini gözlemek, native window'u yeniden boyutlandırmak
ya da geçersiz hâle getirmek gibi işlemler hedef platformun API'sine bağlıdır.
Bu testler platforma özel işlemleri `GS_EGL_PLATFORM_TEST_HOOKS` derleme
makrosuyla korur:

```c
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
/* Platforma özel fixture ve kontroller. */
#else
/* Hook desteği yoksa test "Not applicable" olarak raporlanır. */
#endif
```

`GS_EGL_PLATFORM_TEST_HOOKS`, EGL standardına ait hazır bir özellik değildir.
Hedef platform için gerekli hook fonksiyonları gerçekten sağlandığında derleme
sistemi tarafından tanımlanması gereken bir feature flag'dir. Hook arayüzlerinin
tamamı ve her fonksiyonun implementasyon sözleşmesi `HLR+TP/TP/helpers.h`
içindeki **Platform test hook interface** bölümünde merkezi olarak bulunur.
Test dosyaları ayrıca `extern` bildirimi taşımaz; böylece imza ve sözleşme için
tek kaynak `helpers.h` olur.

Hook'ların gövdeleri ortak helper'a bilinçli olarak yazılmamıştır. Native window,
pixmap, sunum gözlemi ve fault injection kodu X11, Wayland, GBM, Android veya
hedef sürücüye göre değişir. Entegrasyonu hazırlayan ekip, örneğin
`platform_hooks_x11.c` gibi ayrı bir dosyada `helpers.h` içindeki fonksiyonları
implemente eder ve bu dosyayı test binary'sine linkler. GCC veya Clang ile genel
biçim şöyledir:

```sh
cc -DGS_EGL_PLATFORM_TEST_HOOKS \
   platform_hooks_<target>.c <test-sources> ... -lEGL
```

Platform implementasyonu da aynı derleme makrosuyla ve ortak bildirimlerle
derlenmelidir:

```c
#include "HLR+TP/TP/helpers.h"

EGLBoolean GS_EGL10_prepare_native_window(
    EGLDisplay *display,
    EGLConfig *config,
    EGLNativeWindowType *window)
{
    /* helpers.h içindeki bu fonksiyon sözleşmesinin adımlarını uygula. */
    return EGL_FALSE; /* Implementasyon tamamlanana kadar desteği ilan etme. */
}
```

Implementasyonlar sağlanmadan makro tanımlanırsa derleme tamamlanabilse bile
link aşamasında `undefined reference` benzeri hatalar oluşur. Sadece bir hook'u
boş bırakıp `EGL_TRUE` döndüren geçici gövde yazmak da doğru değildir; makronun
tanımlı olması fixture'ın gerçekten kurulabildiği ve test oracle'ının gerçekten
gözlenebildiği taahhüdüdür.

Örneğin `GS_EGL10_BP_SB_TP_002` testi aşağıdaki hook'ları bekler:

```c
EGLBoolean GS_EGL10_prepare_current_window_surface(
    EGLDisplay *display, EGLSurface *surface);
EGLBoolean GS_EGL10_verify_window_content_posted(void);
void GS_EGL10_cleanup_current_window_surface(void);
```

Bu fonksiyonların sorumlulukları şöyledir:

1. `GS_EGL10_prepare_current_window_surface`, native window'u ve ona bağlı EGL
   display, context ve window surface'i oluşturur; context'i current yapar ve
   döndürülen `display` ile `surface` değerlerini doldurur.
2. `GS_EGL10_verify_window_content_posted`, `eglSwapBuffers` sonrasında hazırlanan
   içeriğin native window'a gerçekten gönderildiğini platforma uygun bir
   yöntemle doğrular.
3. `GS_EGL10_cleanup_current_window_surface`, hook tarafından oluşturulan bütün
   native ve EGL kaynaklarını serbest bırakır.

Bu kaynaklar ve başlangıç durumu birlikte test fixture'ını oluşturur. Hook'u
yazan taraf fixture'ın bütün yaşam döngüsünden sorumludur: setup başarısızlığında
kısmen oluşturulmuş kaynakları güvenle temizlemeli, başarılı setup sonrasında
ise testin `close` çağrısıyla yaptığı cleanup'ı desteklemelidir. Testteki
`fixture_prepared` değişkeni fixture'ın kendisi değil, setup'ın başarılı
olduğunu ve cleanup hook'unun çağrılabileceğini gösteren bayraktır.

### Hook grupları ve kullanıldıkları koşullar

| Hook grubu                                                | Hazırlaması veya gözlemesi gereken koşul                                                                                                           |
| --------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `prepare_native_window` / `cleanup_native_window`     | Initialized EGLDisplay, `EGL_WINDOW_BIT` içeren uyumlu config ve henüz bir surface'e bağlı olmayan geçerli native window                                 |
| `prepare_incompatible_native_window`                    | Tek uyumsuzluğu native visual/pixel format ile EGLConfig arasında olan geçerli window fixture'ı                                                    |
| `prepare_non_window_config`                             | Geçerli native window ve `EGL_WINDOW_BIT` içermeyen geçerli config; böyle bir config yoksa sızıntısız `EGL_FALSE`                          |
| `prepare_invalid_native_window`                         | EGL'nin kesin olarak geçersiz tanıyacağı native window handle'ı; tercihen gerçek window oluşturulup yok edilerek elde edilen stale handle       |
| `prepare_invalid_native_pixmap`                         | EGL'nin kesin olarak geçersiz tanıyacağı native pixmap handle'ı ve pixmap-capable config                                                          |
| `prepare_window_surface_allocation_failure`             | `eglCreateWindowSurface` için güvenli ve deterministik `EGL_BAD_ALLOC` fault injection                                       |
| `prepare_make_current_error`                            | `EGL_BAD_NATIVE_WINDOW`, `EGL_BAD_MATCH`, `EGL_BAD_CURRENT_SURFACE` veya `EGL_BAD_ALLOC` üretecek kontrollü `eglMakeCurrent` önkoşulu    |
| `prepare_current_window_surface` / cleanup              | Calling thread'de current olan window surface/context ve native window dahil bütün fixture yaşam döngüsü                                         |
| `verify_window_content_posted`                          | Swap öncesi hazırlanan ayırt edilebilir içeriğin native/front buffer'a gerçekten ulaştığının platform API'siyle gözlenmesi                 |
| `resize_native_window`                                  | Aktif fixture'ın window'unu farklı ve desteklenen bir boyuta getirip kabul edilen gerçek boyutu döndürme                                          |
| `begin_flush_observation` / `implicit_flush_observed` | Hedef `eglSwapBuffers` çağrısının implicit `glFlush` etkisini driver instrumentation veya doğrulanmış eşdeğer oracle ile gözleme |
| `invalidate_native_window`                              | EGLSurface'i yok etmeden ve current binding'i bırakmadan backing native window'u geçersizleştirme                                                   |
| `prepare_current_pixmap_surface` / cleanup              | Native pixmap, `EGL_PIXMAP_BIT` içeren uyumlu config, current EGLSurface/context ve karşılaştırma baseline'ı                                            |
| `verify_pixmap_unchanged`                               | `eglSwapBuffers` sonrasında native pixmap'in baseline'a göre değişmediğini native API ile doğrulama                                            |

Her prepare fonksiyonu çıkış pointer'larını doğrulamalı ve yalnız bütün
postcondition'lar kurulduğunda `EGL_TRUE` döndürmelidir. Cleanup sırası genel
olarak current binding'i bırakma, EGL context/surface'leri yok etme, EGLDisplay'i
sonlandırma ve en son native nesneleri yok etme şeklindedir. Cleanup; yarım kalan
setup, daha önce yok edilmiş native window/pixmap ve test tarafından terminate
edilmiş display durumlarında da güvenli olmalıdır. Özellikle
`GS_EGL10_cleanup_make_current_error`, bazı testler tarafından setup başarısız
olsa bile çağrıldığı için boş/kısmi fixture üzerinde güvenli olmak zorundadır.

Geçersiz native handle için rastgele sayı veya sıfır kullanmak yeterli değildir;
hedef EGL'nin bu değeri ilgili native hata olarak tanıyacağı platform sözleşmesi
bulunmalıdır. Benzer biçimde `EGL_BAD_ALLOC` testleri gerçek sistem belleğini
tüketmeye çalışmamalı, hedefin desteklediği fault-injection mekanizmasını
kullanmalıdır. Platform bu koşullardan birini güvenilir biçimde hazırlayamıyor
veya gözleyemiyorsa ilgili test binary'si için
`GS_EGL_PLATFORM_TEST_HOOKS` tanımlanmamalıdır.

### Diğer hedef adaptasyon hook'ları

Bazı eski testler global platform-hook makrosu yerine kendilerine ait feature
flag kullanır veya doğrudan hedef fixture'ı bekler. Bunların bildirimleri ve
ayrıntılı sözleşmeleri de `helpers.h` içindeki **Additional target-adaptation
hook interface** bölümünde merkezi olarak tutulur:

| Fonksiyon | Sözleşme / feature flag |
| --- | --- |
| `GS_EGL10_get_valid_native_display` / `release_valid_native_display` | `eglGetDisplay` ile eşleşen gerçek bir native display açar ve kapatır. `GS_EGL_USE_WAYLAND` tanımlıysa TP_002 içindeki hazır Wayland gövdesi kullanılır; diğer hedefler dış implementasyon sağlar. |
| `GS_EGL10_get_unmatched_native_display` / `release_unmatched_native_display` | Güvenle `eglGetDisplay`'e verilebilen fakat karşılık gelen EGLDisplay bulunmayan, platform tarafından tanımlı bir identifier sağlar. Fabricated pointer kullanılamaz. |
| `GS_EGL10_get_noninitializable_display` / `release_noninitializable_display` | Geçerli fakat kontrollü koşul/fault injection nedeniyle `EGL_NOT_INITIALIZED` üretecek EGLDisplay sağlar; release enjeksiyonu mutlaka kapatır. |
| `GS_EGL10_compare_native_visual_type` | Hedefin implementation-defined native visual sırasını `-1`, `0`, `1` ile bildirir. Yalnız `GS_EGL10_NATIVE_VISUAL_COMPARE_AVAILABLE` tanımlandığında dış gövde gerekir. |
| `GS_EGL10_create_and_join_thread` | Callback'i yeni thread'de tam bir kez çalıştırır, join eder ve memory visibility sağlar. `GS_EGL_USE_PTHREAD` tanımlıysa TP_006 içindeki hazır pthread gövdesi kullanılır; diğer hedefler dış implementasyon sağlar. |

Bu fonksiyonların da yerel bildirimleri test dosyalarından kaldırılmıştır.
Platform implementasyonu `helpers.h` dosyasını dahil ederek ortak imzayı
kullanmalıdır. Feature flag ile hazır gövde seçilen durumlarda aynı isimle ikinci
bir dış implementasyon linklenmemelidir.

### Context'i current yapma

Test önkoşulu current bir context gerektiriyorsa setup'tan sonra şu çağrı
yapılır:

```c
if (!GS_EGL10_make_environment_current(&environment))
{
    TEST_LOG_FAIL(test_case, test_procedure,
        "Could not make context current, EGL error: 0x%x",
        eglGetError());
    return;
}
```

Bu helper aşağıdaki çağrıyı yapar; aynı pbuffer draw ve read surface olarak
kullanılır:

```c
eglMakeCurrent(
    environment.display,
    environment.surface,
    environment.surface,
    environment.context
);
```

`eglMakeCurrent` fonksiyonunun kendisini test eden prosedürlerde helper yalnız
nesneleri oluşturmak için kullanılmalı, test edilen `eglMakeCurrent` çağrısı ise
`// Test starts here:` satırından sonra doğrudan yapılmalıdır.

### EGL error kontrolü

Beklenmeyen EGL hatalarını kontrol etmek için `CHECK_ERROR` kullanılabilir:

```c
eglGetConfigAttrib(display, config, EGL_CONFIG_ID, &value);
CHECK_ERROR(test_procedure);
```

Makro `eglGetError()` sonucunu kontrol eder. Sonuç `EGL_SUCCESS` değilse
`TEST_LOG_FAIL` çağırır ve test dosyasındaki `test_success` değişkenini
`EGL_FALSE` yapar. Bu nedenle makroyu kullanan dosyada aşağıdaki değişkenler
bulunmalıdır:

```c
static const char* test_case = "...";
static EGLBoolean test_success = EGL_TRUE;
```

Beklenen bir EGL hatasını doğrulayan negative testlerde `CHECK_ERROR`
kullanılmaz. Hata kodu doğrudan alınarak beklenen değerle karşılaştırılır:

```c
(void)eglGetError(); /* Önceki EGL error değerini temizle. */

result = eglMakeCurrent(...);
error = eglGetError();

if (result != EGL_FALSE || error != EGL_BAD_MATCH)
{
    TEST_LOG_FAIL(test_case, test_procedure,
        "Expected EGL_FALSE/EGL_BAD_MATCH, got %u/0x%x",
        (unsigned int)result, error);
    test_success = EGL_FALSE;
}
```

### Cleanup ve test yaşam döngüsü

Her testin `close` fonksiyonu environment'ı temizler:

```c
void GS_EGL10_RC_MC_TP_002_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
```

Cleanup aşağıdaki sırayı izler:

1. Current context ve surface bağlarını kaldırır.
2. Oluşturulmuş surface'i destroy eder.
3. Oluşturulmuş context'i destroy eder.
4. Display'i `eglTerminate` ile sonlandırır.
5. Environment alanlarını tekrar güvenli boş değerlere getirir.

Test runner'ın sağlaması gereken tek şey test giriş noktalarını sırayla
çağırmaktır:

```c
GS_EGL10_RC_MC_TP_002_init();
GS_EGL10_RC_MC_TP_002_draw();
GS_EGL10_RC_MC_TP_002_close();
```

EGL setup ve asıl kontrol `init` içinde yapılır. EGL testlerinde çizim aşaması
gerekmiyorsa `draw` boş bırakılır. `close`, test başarılı olsa da olmasa da
çağrılmalı ve testin oluşturduğu EGL kaynaklarını serbest bırakmalıdır.

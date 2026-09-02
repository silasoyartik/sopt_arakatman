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

## Makefile gerekli mi?

Make, kaynaklardan biri daha yeniyse hedefi üretmek için faydalı olabilir. Ancak
Windows'ta varsayılan olarak bulunmaz ve boşluk içeren dosya yolları ile dinamik
sıra listelerini dependency olarak yönetmek ek karmaşıklık getirir. Bu nedenle
ana çözüm Makefile'a bağlı değildir. Python scriptinin çıktıyı sadece içerik
değiştiğinde yazması, bu küçük dokümantasyon build'i için aynı pratik faydayı
taşınabilir biçimde sağlar.

## EGL test helper kullanımı

`HLR+TP/TP/helpers.h`, EGL testlerinde tekrarlanan display, config, context ve
surface hazırlama işlemlerini ortaklaştırır. Testler bir `main` fonksiyonundan
hazır EGL nesneleri beklemez; ihtiyaç duydukları ortamı kendi `init`
fonksiyonlarında oluşturur ve `close` fonksiyonlarında tamamen temizler.

Bir test dosyası helper'ı şu şekilde dahil eder:

```c
#include <EGL/egl.h>
#include "../../helpers.h"
```

### Test environment

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

`GS_EGL10_choose_config`, verilen surface type için `eglChooseConfig` çağırır
ve ilk uygun config'i environment içine kaydeder. En az bir config bulunmadığı
durumda `EGL_FALSE` döndürür.

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

# Test helper kullanımı

Bu rehber, GLES 3.1 TP'lerinde ortak kurulum, shader/program oluşturma,
buffer okuma, sonuç raporlama ve kaynak temizliğinin nasıl kullanıldığını
anlatır. Test edilen API çağrısı ve beklenen davranış TP dosyasında kalır.

## 1. Dosyaların görevleri

| Dosya | Görevi |
| --- | --- |
| [helpers_gles31.h](helpers_gles31.h) | Veri yapıları, fonksiyon bildirimleri ve sonuç makroları. TP'lerin include ettiği ortak arayüzdür. |
| [helpers_gles31.c](helpers_gles31.c) | Platformdan bağımsız uygulama: shader derleme/link, program ve SSBO yönetimi, CPU'ya okuma, sonuç takibi. |
| [helpers_gles31_glfw.c](helpers_gles31_glfw.c) | GLFW ile gizli pencere ve GLES context oluşturma, context'i current yapma, fonksiyonları yükleme ve ortamı kapatma. |
| [macros.h](macros.h) | `TEST_LOG_INFO`, `TEST_LOG_FAIL`, `TEST_LOG_SUCCESS` çıktı makroları. Tek başlarına test sonucunu değiştirmezler. |
| [helpers.h](helpers.h) | Ayrı EGL test ailesinin `GS_EGL10_*` yardımcıları. GLES fixture'ın platform uygulaması yerine doğrudan kullanılamaz. |

Önceden `helpers_gles31.h` içinde bulunan fonksiyon gövdeleri ortak `.c`
dosyasına taşınmıştır. `.h`, fonksiyonların nasıl çağrılacağını bildirir;
`.c`, bu fonksiyonların işini yapar. Bu nedenle başlığı include etmek ve
ortak `.c` dosyasını derlemek ayrı adımlardır.

EGL helper'larının ayrıntılı kullanımı [depo README'sinde](../../README.md),
glDispatchCompute senaryoları ve HLR eşleştirmeleri ise
[modül README'sinde](ComputeShader/glDispatchCompute/README.md) anlatılır.

## 2. TP ile helper arasındaki görev dağılımı

| TP içinde kalır | Helper içinde yapılır |
| --- | --- |
| HLR, TC ve TP kimlikleri | Ortam hazırlama ve current-context kontrolü |
| Senaryoya özgü shader kaynağı | Shader derleme ve program link işlemleri |
| Dispatch boyutları ve `glDispatchCompute` çağrısı | Program aktifleştirme ve kaynak bağlama |
| Beklenen GL hatası | Hata değerini alma, karşılaştırma ve raporlama |
| Beklenen buffer/built-in değerleri ve hesapları | Buffer oluşturma, sıfırlama ve CPU belleğine okuma |
| Hangi başarısızlığın hangi TC'yi etkilediği | Sonuç durumunu saklama ve kaynakları kapatma |

Örneğin `GS_GLES31_read_ssbo()` buffer verisini CPU dizisine kopyalar.
Verinin hangi değer olması gerektiğine TP karar verir. Helper'ların hiçbiri
TP adına `glDispatchCompute` çağırmaz.

## 3. Environment, fixture ve yaşam döngüsü

`GS_GLES31_TestEnvironment` platforma ait context bilgisini tutar.
`platform_context` opak bir alandır; TP bu pointer'ı yorumlamamalıdır.

`GS_GLES31_TestFixture`, environment'a ek olarak bir programın ve en fazla
dört SSBO'nun sahipliğini tutar. Başlangıç değeri mutlaka verilmelidir:

```c
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;
static GS_GLES31_TestResult result = GS_GLES31_RESULT("TC_ID", "TP_ID");
```

Her test koşusunda şu sıra kullanılır:

1. `GS_GLES31_reset_results()` ile önceki sonuçları sıfırla.
2. `GS_GLES31_begin_fixture()` ile ortamı oluştur ve doğrula.
3. Gerekli program ve buffer'ları fixture yardımcılarıyla oluştur.
4. API çağrısını, hata kontrolünü ve çıktı karşılaştırmalarını TP içinde yap.
5. `GS_GLES31_report_results()` ile sonuçları raporla.
6. TP'nin `close()` fonksiyonunda `GS_GLES31_end_fixture()` çağır.

Test runner her TP için `init() -> draw() -> close()` sırasını izler.
Compute TP'lerinde test `init()` içinde yürütülür ve `draw()` boştur.
`init()` herhangi bir aşamada başarısız olsa da runner `close()` çağırmalıdır.
`begin_fixture()` başarısız olduğunda oluşturulmuş kısmi kaynakların var
olabileceği unutulmamalıdır; `end_fixture()` bu kapanışı destekler.

TP numaralarının çalışma sırasına etkisi yoktur. Ancak mevcut GLFW uygulaması
tek aktif fixture varsayar ve GLFW başlatma/kapatma işlemlerini sahiplenir.
Bir TP kapatıldıktan sonra diğeri başlatılmalı; bu backend ile paralel TP
çalıştırılmamalıdır. `close()` sonrasında aynı TP tekrar çalıştırılabilir.

## 4. Kaynak oluşturma ve sahiplik

| Fonksiyon | Davranış |
| --- | --- |
| `GS_GLES31_fixture_compute_program(&fixture, source)` | Compute programı derler/link eder, fixture'a kaydeder ve aktifleştirir. Başarısızlıkta `0` döner. |
| `GS_GLES31_fixture_graphics_program(&fixture, vs, fs)` | Vertex ve fragment shader'lardan graphics-only program oluşturur, kaydeder ve aktifleştirir. |
| `GS_GLES31_fixture_ssbo(&fixture, binding, size, initial, usage)` | SSBO oluşturur, ilgili binding'e bağlar ve sahipliğini fixture'a kaydeder. Başarısızlıkta nesne adı `0` döner. |
| `GS_GLES31_use_program(program)` | Pipeline binding'ini sıfırlar, programı seçer ve seçilen durumu sorgulayarak doğrular. `program = 0` programsız önkoşul için kullanılabilir. |
| `GS_GLES31_end_fixture(&fixture)` | Context current ise kayıtlı GL kaynaklarını siler; ardından platform ortamını kapatır ve kaynak alanlarını sıfırlar. |

Fixture ile oluşturulan program/buffer'ları TP ayrıca silmemelidir.
Kapanışın sahibi fixture'dır. Aynı fixture'a ikinci bir program veya
beşinci bir SSBO eklenmesi desteklenmez.

`GS_GLES31_create_compute_program()`, `GS_GLES31_create_graphics_program()`
ve `GS_GLES31_create_ssbo()` gibi alt seviye fonksiyonlar da kullanılabilir.
Ancak bunlar oluşturulan nesneyi fixture'a kaydetmez. Bu yolu kullanan
çağıran kod, nesnenin uygun context altında temizlenmesinden sorumludur.
Yeni TP'lerde fixture yardımcılarını kullanmak sahipliği daha açık tutar.

`GS_GLES31_noop_compute_source()` local size `(1,1,1)` olan, buffer erişimi
yapmayan boş compute shader kaynağını verir. Limit aşımı gibi yalnızca
API hata davranışının test edildiği senaryolarda kullanılabilir.

## 5. Buffer işlemleri

SSBO (Shader Storage Buffer Object), shader'ın erişebildiği buffer nesnesidir.

| Fonksiyon | Kullanım |
| --- | --- |
| `GS_GLES31_fill_uint(data, count, value)` | CPU'daki `GLuint` dizisini doldurur. `count` eleman sayısıdır. |
| `GS_GLES31_reset_ssbo(buffer, binding, size, data)` | Buffer'ı bağlar ve başlangıçtan itibaren mevcut depolamadaki veriyi günceller. Buffer boyutunu değiştirmez. |
| `GS_GLES31_read_ssbo(buffer, size, destination)` | Shader yazmalarını CPU okumasına hazırlar; map, kopyalama ve unmap yapar. Başarısızlıkta `0` döner. |
| `GS_GLES31_bind_ssbo(buffer, binding)` | Genel ve indexed SSBO binding'lerini ayarlar. Dönüş değeri yoktur; tek başına hata kontrolü yapmaz. |

Buffer fonksiyonlarındaki `size` **byte sayısıdır**; genellikle `sizeof(data)`
verilir. CPU hedef dizisi ve GPU buffer'ı bu boyutu karşılamalıdır.
Reset için başlangıç verisi, readback için yazılabilir CPU belleği gerekir.

Readback sırası ortak uygulamada şöyledir:

```text
glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT)
  -> glBindBuffer
  -> glMapBufferRange(GL_MAP_READ_BIT)
  -> memcpy
  -> glUnmapBuffer
```

TP'ye mapped pointer verilmez; veri TP'nin kendi dizisine kopyalanır.
Map veya unmap başarısızsa TP çıktıyı geçerli kabul etmemelidir.
`reset_ssbo()` da önceki shader yazmalarından sonra API üzerinden güncelleme
yapmak için buffer-update barrier uygular.

`read_ssbo()` CPU okuması içindir. Sonraki erişimin başka bir shader veya
başka bir kaynak türü olduğu senaryoların senkronizasyonunu üstlenmez.

## 6. Hata ve sonuç yönetimi

```c
GS_GLES31_clear_errors();
glDispatchCompute(4, 1, 1);
if (!GS_GLES31_EXPECT_ERROR(&result, GL_NO_ERROR, "glDispatchCompute"))
    goto report;
```

`GS_GLES31_EXPECT_ERROR` hemen `glGetError()` çağırır, beklenen değerle
karşılaştırır ve kalan hata değerlerini de tüketip kontrol eder. İlave hata
varsa kontrol başarısız olur. Dispatch ile bu kontrol arasına readback,
başka GL işlemi veya `clear_errors()` konulmamalıdır.

Altyapı yardımcıları hatalarını `stderr` üzerinden açıklar ve başarısızlık
döndürür. **Etkilenen TC sonucunu TP işaretler:**

```c
if (!GS_GLES31_read_ssbo(buffer, sizeof(data), data))
{
    GS_GLES31_FAIL_RESULTS(&result, 1, "Buffer okunamadı");
    goto report;
}
```

| Makro / fonksiyon | Sonuca etkisi |
| --- | --- |
| `GS_GLES31_reset_results(results, count)` | Belirtilen sonuçları `NOT_RUN` yapar. |
| `GS_GLES31_CHECK(&result, condition, format, ...)` | Koşul yanlışsa `FAIL` yapar ve log basar. Doğruysa önceki durum `FAIL` değilse `PASS` yapar. Koşulun doğruluğunu döndürür. |
| `GS_GLES31_FAIL_RESULTS(results, count, format, ...)` | Belirtilen sonuçların tamamını `FAIL` yapar. |
| `GS_GLES31_report_results(results, count)` | `PASS` için başarı basar; `NOT_RUN` için tamamlanmamış kontrol hatası basıp `FAIL` yapar. Önceden loglanmış `FAIL` sonuçlarını tekrar basmaz. |

Bir koşudaki `FAIL`, sonraki başarılı kontrolle silinmez. Yalnızca yeni
koşu başlangıcındaki reset bu durumu temizler. `CHECK` dönüş değeri tek
koşulun sonucudur; bütün TC'nin başarılı olduğu anlamına gelmez.

Başarılı hata kontrolü sonucu `PASS` yapabileceği için sonradan başarısız
olan her setup/readback adımı ayrıca `FAIL` olarak işaretlenmelidir.
Makrolar çağrıldıkları TP'nin `__FILE__` ve `__LINE__` bilgisini korur.

Bir TP birden fazla HLR/TC kapsıyorsa sonuç dizisi kullanılır ve yalnızca
etkilenen sonuçlar başarısız yapılır. Örneğin TP_001'de dispatch'in hata
üretmediği doğrulandıktan sonra buffer okunamaması, yürütme kontrollerini
başarısız yapar; kabul edilmiş dispatch sonucunu değiştirmez.

`GS_GLES31_work_group_limit(axis, &limit)` eksen limitini sorgular ve
pozitif olduğunu kontrol ederek `GLuint` olarak döndürür. Eksenler
`0 = X`, `1 = Y`, `2 = Z` şeklindedir; fonksiyon dispatch yapmaz.

## 7. Örnek TP

Aşağıdaki örnek bir invocation ile SSBO'ya `42` yazıldığını kontrol eder.
`EXAMPLE_*` kimlikleri gösterim amaçlıdır; gerçek TP'de HLR/TC kimlikleri
ve senaryo kaynağı kullanılmalıdır. Include yolu, `helpers_gles31.h`
dosyasının include path'e eklendiğini varsayar.

```c
#include "helpers_gles31.h"

static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;
static GS_GLES31_TestResult result =
    GS_GLES31_RESULT("EXAMPLE_TC", "EXAMPLE_TP");

static const char *source =
    "#version 310 es\n"
    "layout(local_size_x = 1) in;\n"
    "layout(std430, binding = 0) buffer Output { uint value; };\n"
    "void main() { value = 42u; }\n";

void example_init(void)
{
    GLuint value = 0xFFFFFFFFu;
    GLuint buffer;

    GS_GLES31_reset_results(&result, 1);
    if (!GS_GLES31_begin_fixture(&fixture) ||
        !GS_GLES31_fixture_compute_program(&fixture, source))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "Ortam/program oluşturulamadı");
        goto report;
    }

    buffer = GS_GLES31_fixture_ssbo(&fixture, 0,
        sizeof(value), &value, GL_DYNAMIC_READ);
    if (buffer == 0)
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "SSBO oluşturulamadı");
        goto report;
    }

    GS_GLES31_clear_errors();
    glDispatchCompute(1, 1, 1);
    if (!GS_GLES31_EXPECT_ERROR(&result, GL_NO_ERROR, "glDispatchCompute"))
        goto report;

    if (!GS_GLES31_read_ssbo(buffer, sizeof(value), &value))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "SSBO okunamadı");
        goto report;
    }
    GS_GLES31_CHECK(&result, value == 42u, "Beklenen 42, okunan %u", value);

report:
    GS_GLES31_report_results(&result, 1);
}

void example_draw(void) { }

void example_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}
```

## 8. Derlemeye ekleme ve platform değiştirme

Gerçek GLES TP executable'ı şu bileşenlerle oluşturulur:

- Çalıştırılacak TP `.c` dosyaları ve `init/draw/close` çağıran runner.
- `HLR+TP/TP/helpers_gles31.c`.
- Tek bir platform uygulaması: referans için `helpers_gles31_glfw.c`.
- Seçilen loader'ın kaynak/nesne dosyası ve include dizini.
- Platform uygulamasının gerektirdiği GLFW, GLES ve sistem kütüphaneleri.

`.c` dosyaları `#include` edilmez; ayrı kaynaklar olarak derlenip bağlanır.
TP, `helpers_gles31.h` dosyasını include eder. Mevcut glDispatchCompute
TP'lerinde bu yol `../../helpers_gles31.h` biçimindedir.

| Loader seçimi | Başlık / kurulum |
| --- | --- |
| `GS_GLES31_USE_GLAD1` | `<glad/glad.h>`; depodaki `others (for Compute)/glad.c` ve aynı klasörün `include` dizini kullanılır. |
| `GS_GLES31_USE_GLAD2` | `<glad/gles2.h>`; GLES 3.1 içeren uygun GLAD2 paketi ayrıca sağlanmalıdır. |
| İkisi de tanımsız | `<GLES3/gl31.h>`; hedef SDK gerekli giriş noktalarını sağlamalıdır. |

İki GLAD seçeneği birlikte tanımlanamaz. TP, ortak helper ve platform
uygulaması aynı loader yapılandırmasıyla derlenmelidir.

GLFW yerine başka bir platform kullanılacaksa yalnızca şu arayüzün yeni
uygulaması sağlanır; ortak helper `.c` dosyası derlemede kalır:

```c
int GS_GLES31_prepare_environment(GS_GLES31_TestEnvironment *environment);
int GS_GLES31_environment_is_current(const GS_GLES31_TestEnvironment *environment);
void GS_GLES31_cleanup_environment(GS_GLES31_TestEnvironment *environment);
```

Başarılı hazırlık GLES 3.1 veya üzeri bir context oluşturmalı, çağıran
thread'de current yapmalı, gerekli giriş noktalarını yüklemeli ve
`initialized` alanını ayarlamalıdır. Kapanış kısmi kurulumdan sonra da güvenli
olmalı ve platformun sahip olduğu kaynakları bırakmalıdır. GLFW ve alternatif
uygulama aynı executable'a birlikte eklenmez; aynı fonksiyonları tanımlarlar.

## 9. Kontrollerin kapsamı

[Altyapı regresyon testi](../../tests/gles31_helpers_test.c) GLAD fonksiyon
pointer'larına test stub'ları bağlar. Sonuçların resetlenmesini, hata
yalıtımını, map/unmap/barrier başarısızlıklarını ve kısmi kaynak temizliğini
GPU veya pencere oluşturmadan kontrol eder. Çalıştırma komutları
[modül README'sindedir](ComputeShader/glDispatchCompute/README.md).

Bu test bilerek FAIL mesajları üretir; başarı ölçütü son başarı mesajı ve
sıfır süreç çıkış kodudur. Stub testine GLFW platform uygulaması eklenmez;
environment fonksiyonlarını test dosyası sağlar.

C derleme/bağlama kontrolleri ve helper regresyon testi, shader'ların gerçek
sürücüde derlendiğini veya 10 TP'nin GPU'da geçtiğini göstermez. Bu son
doğrulama hedef GLES ortamında TP'ler çalıştırılarak yapılır.

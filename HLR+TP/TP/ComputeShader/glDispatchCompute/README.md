# glDispatchCompute testleri

On TP'nin tamamı ortak helper kullanmadan çalışır. Yalnızca GLES API başlığı
ve loglama için `../../macros.h` kullanılır. Shader derleme/link, program ve
SSBO oluşturma, dispatch, hata/çıktı kontrolü ve kaynak temizliği doğrudan
ilgili TP'nin `init()` ve `close()` fonksiyonlarındadır.

## Runner ve yaşam döngüsü

Runner, `init()` öncesinde GLES 3.1 veya üzeri context oluşturmalı, çağıran
thread'de current yapmalı ve gerekiyorsa GL giriş noktalarını yüklemelidir.
Context kurulumu bu TP'lerde yapılmaz; GLFW veya EGL backend'i TP'lere bağlı
değildir. Runner başlangıç GL hata durumunu temiz tutmalıdır. Context sürümü
ve gerekli GL fonksiyonlarının yüklenmiş olması runner tarafından doğrulanır;
TP'lerde bu platform kontrolleri tekrarlanmaz.

Her TP için `init() -> draw() -> close()` sırası kullanılır. Test `init()`
içindedir; `draw()` boştur. Başarısız setup sonrasında da `close()` aynı
context current iken çağrılır. TP yalnızca kendi oluşturduğu GL nesnelerini
siler; context'i kapatmaz. `close()` tekrar çağrılabilir; bir koşu kapatıldıktan
sonra yeni `init()` önceki sonuçları taşımadan çalışır. Aynı TP'nin eşzamanlı
koşuları desteklenmez.

TP'ler program/pipeline ve kullandıkları SSBO bağlarını değiştirir. TP010
binding 0 ve 1'i, diğer SSBO kullanan testler binding 0'ı kullanır. Önceki
bağlar saklanıp geri yüklenmez; sonraki TP/runner kendi durumunu kurmalıdır.
Bir context üzerinde TP'ler sırayla çalıştırılır; numara sırasına bağımlılık yoktur.

## HLR kapsamı

| TP | GS-GLES31-CS-DC HLR son eki | Senaryo |
| --- | --- | --- |
| 001 | 001, 002, 013 | Geçerli dispatch, aktif programın çıktısı, hata yokluğu |
| 002 | 009 | Üç boyutlu grup kimlikleri ve atomik ziyaret sayıları |
| 003 | 010 | Sabit local size ve gerçek invocation çıktıları |
| 004 | 011 | Üç boyutta compute built-in değerleri |
| 005 | 003 | Aktif compute executable yok |
| 006 | 004 | Aktif graphics-only program |
| 007 | 005 | X limiti + 1 |
| 008 | 006, 007 | Y ve Z limitleri + 1, bağımsız kontroller |
| 009 | 008 | Pozitif kontrol ardından her eksende sıfır dispatch |
| 010 | 012 | İki farklı SSBO giriş/çıkış çiftiyle binding, okuma ve yazma |

Dosya adları ve dış fonksiyonlar `GS_GLES31_CS_DC_TP_001`–`010` biçimindedir.

## Hata kontrolleri

Her GL çağrısından sonra ayrı hata bloğu kullanılmaz. Shader/program nesnesi,
derleme/link sonucu ve buffer nesnesi kontrolleri başarısız kurulumda testi
durdurur. Kurulum çağrılarının GL hataları, dispatch öncesindeki kontrol
noktalarında alınır; böylece kurulum hatası beklenen dispatch hatası sanılmaz.

Dispatch sonrasındaki hata karşılaştırmaları testin parçasıdır ve korunur.
Buffer okuyan TP'lerde barrier/map işlemlerinin ortak hata kontrolü, NULL
mapping kontrolü ve unmap sonucu da korunur. Aksi halde okunamayan veya
geçersiz veri üzerinden SUCCESS raporlanabilir.

Sabit küçük dispatch sayıları için limit sorgusu tekrarlanmaz. TP007/008'de
limitin bir fazlasını sınamak için gereken sorgular, TP003'te yerel grup
boyutu sorgusu ve tüm beklenen çıktı kontrolleri kalır. `close()` yalnızca
kaynak temizliği yapar; TP'lerde `main()` veya context oluşturma kodu yoktur.

## Derleme

Tüm TP kaynakları aynı API/loader seçimiyle derlenir:

| Seçim | API başlığı |
| --- | --- |
| `GS_GLES31_USE_GLAD1` | `<glad/glad.h>`; depodaki GLAD1 paketi |
| `GS_GLES31_USE_GLAD2` | `<glad/gles2.h>`; hedefin GLES 3.1 GLAD2 paketi |
| İkisi de tanımsız | `<GLES3/gl31.h>`; hedef SDK'nın GLES 3.1 API'si |

İki GLAD seçeneği birlikte tanımlanamaz. GLAD kullanıldığında runner yüklemeyi
current context üzerinde, ilk `init()` çağrısından önce yapmalıdır.
`helpers_gles31.c` ve `helpers_gles31_glfw.c` bu TP'ler için derlemeye eklenmez.
Gerçek executable, TP kaynakları + runner + seçilen loader/GLES ve runner'ın
platform kütüphaneleriyle bağlanır. Bu klasör bir `main()` sağlamaz.

Depo kökünde PowerShell/GCC ile GPU gerektirmeyen sözdizimi kontrolü:

```powershell
$tpSources = @(Get-ChildItem 'HLR+TP/TP/ComputeShader/glDispatchCompute/*.c' | ForEach-Object FullName)
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -DGS_GLES31_USE_GLAD1 '-Iothers (for Compute)/include' -fsyntax-only @tpSources
gcc -m32 -std=c11 -Wall -Wextra -Wpedantic -Werror -DGS_GLES31_USE_GLAD1 '-Iothers (for Compute)/include' -fsyntax-only @tpSources
```

## Doğrulama sınırı

Derleme veya sahte GL cevaplarıyla yapılan kontrol, shader'ın gerçek GPU'da
çalıştığını kanıtlamaz. HLR doğrulaması için TP'ler hedef GLES sürücüsünde
çalıştırılmalıdır. Hatasız dispatch tek başına pozitif testleri geçirmez;
TP001–004 ve TP010 gerçek buffer çıktısını kontrol eder. TP009 önce çalışan
bir pozitif kontrol ister, ardından sıfır X/Y/Z dispatch'lerini sınar.
TP008, Y başarısız olsa da Z senaryosunu çalıştırır ve ayrı TC sonuçları üretir.

`tests/gles31_helpers_test.c`, eski helper modülünü sınayan ayrı bir testtir;
bu helpersız TP'leri çalıştırmaz veya doğrulamaz. Helper dokümantasyonu ve
onun regresyon komutu [helper rehberinde](../../README_helpers.md) bulunur.

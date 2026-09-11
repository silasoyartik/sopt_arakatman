# glDispatchCompute testleri

Ortak dosyaların görevleri, fonksiyonlar ve örnek TP için
[test helper rehberine](../../README_helpers.md) bakın.

TP dosyaları shader senaryosunu, dispatch parametrelerini, beklenen hata ve
çıktı kontrollerini içerir. Ortam, kaynak sahipliği, shader derleme/link,
SSBO okuma ve sonuç raporlama `../../helpers_gles31.c` içindedir.

Derlemeye **`helpers_gles31.c` ve tek bir platform uygulaması** eklenmelidir.
Referans platform `helpers_gles31_glfw.c` dosyasıdır; hedef platform aynı
environment arayüzünü EGL veya kendi API'siyle uygulayabilir. GLAD1 kullanımı
için `GS_GLES31_USE_GLAD1` tanımlanır ve depodaki `glad.c` derlenir.
Başlık artık uygulama gövdelerini içermediği için yalnızca include etmek
bağlama için yeterli değildir.

Her TP sırayla `init()`, `draw()`, `close()` olarak çağrılır. `draw()` boştur;
test `init()` içinde yapılır. Başarısız setup dahil her koşudan sonra `close()`
çağrılmalıdır. Aynı süreçte sonraki `init()` sonuçları yeniden başlatır.
Referans GLFW ortamı tek aktif fixture varsayar; TP'ler sırayla çalıştırılır.

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

001 ve 002'nin dosya adlarındaki `GS_GL43C` öneki eski addır; içerikleri ve
dış fonksiyonları `GS_GLES31_CS_DC_TP_001/002` olarak GLES 3.1 kullanır.

## GPU gerektirmeyen altyapı kontrolü

`tests/gles31_helpers_test.c` sonuçların yeniden başlatılmasını, hata
yalıtımını, barrier/map/unmap başarısızlıklarını, program durumunu ve kısmi
kurulumun temizlenmesini GLAD stub'larıyla kontrol eder. Shader yürütmesini
veya sürücü uyumluluğunu doğrulamaz. Testin bilerek ürettiği FAIL mesajları
beklenir; başarılı bitişte son mesaj `GLES31 helper regression checks passed`
ve süreç çıkış kodu sıfırdır. On TP de aynı executable'a bağlanarak mükerrer
dış fonksiyon tanımları denetlenir.

Depo kökünden PowerShell ve GCC ile:

```powershell
New-Item -ItemType Directory -Force build/gles31 | Out-Null
$tpSources = @(Get-ChildItem 'HLR+TP/TP/ComputeShader/glDispatchCompute/*.c' | ForEach-Object FullName)
gcc '-Iothers (for Compute)/include' -c 'others (for Compute)/glad.c' -o build/gles31/glad.o
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -DGS_GLES31_USE_GLAD1 '-Iothers (for Compute)/include' tests/gles31_helpers_test.c 'HLR+TP/TP/helpers_gles31.c' @tpSources build/gles31/glad.o -o build/gles31/helpers_test.exe
./build/gles31/helpers_test.exe
```

Bu stub testine GLFW platform dosyası eklenmez; environment fonksiyonlarını
test dosyası sağlar. Gerçek TP çalıştırması için stub dosyası yerine hedef
platform ve `init/draw/close` çağrılarını yapan test runner kullanılır.

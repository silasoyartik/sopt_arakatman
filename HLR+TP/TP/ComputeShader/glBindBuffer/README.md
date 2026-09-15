# glBindBuffer — GLES 3.1 compute buffer testleri

[HLR](../../../HLR/ComputeShader/glBindBuffer.md) kapsamı:

| TP | Test Case / HLR son eki | Kontrol |
| --- | --- | --- |
| GS_GLES31_CS_BB_TP_001 | 001 | `GL_SHADER_STORAGE_BUFFER` hedefinin kabul edilmesi ve genel bağın verilen buffer adı olması |

TP helpersızdır; API başlığı ve loglama için `../../macros.h` kullanılır.
Runner, GLES 3.1 veya üzeri context'i current yapmalı, GL fonksiyonlarını
yüklemeli ve başlangıç GL hata durumunu temiz tutmalıdır. TP içinde `main()`
ve context oluşturma yoktur. `init() -> draw() -> close()` sırası kullanılır;
`draw()` boştur. Başarısız kurulumda da aynı context current iken `close()`
çağrılır. Yalnızca TP'nin buffer'ı silinir; önceki buffer bağı geri yüklenmez.

Loader seçimi dispatch TP'leriyle aynıdır: `GS_GLES31_USE_GLAD1`,
`GS_GLES31_USE_GLAD2` veya varsayılan `<GLES3/gl31.h>`. Ortak helper `.c`
dosyası bağlanmaz. `glBindBuffer.h`, mevcut `TEST_PROCEDURE` ve
`DEFAULT_TEST_RUN_TIME` makrolarını sağlayan runner için kayıt listesidir.

TP'nin başlangıcı `Test Case #001` yorumuyla işaretlidir. Sonuç kontrolleri
testin parçasıdır; ayrı platform/sürüm/pointer kontrol blokları içermez.

Kaynak: [GLES 3.1, Bölüm 6.1](https://registry.khronos.org/OpenGL/specs/es/3.1/es_spec_3.1.pdf).

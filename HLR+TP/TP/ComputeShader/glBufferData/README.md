# glBufferData — GLES 3.1 compute buffer testleri

[HLR](../../../HLR/ComputeShader/glBufferData.md) kapsamı:

| TP | Test Case / HLR son eki | Kontrol |
| --- | --- | --- |
| GS_GLES31_CS_BD_TP_001 | 001 | Genel SSBO bağındaki buffer'a istenen byte sayısının tahsisi; indexed buffer'ın boyutunun korunması |
| GS_GLES31_CS_BD_TP_002 | 002 | `GL_DYNAMIC_COPY` kabulü ve `GL_BUFFER_USAGE` değeri |
| GS_GLES31_CS_BD_TP_003 | 003 | `GL_DYNAMIC_READ` kabulü ve `GL_BUFFER_USAGE` değeri |
| GS_GLES31_CS_BD_TP_004 | 004 | `GL_STATIC_READ` kabulü ve `GL_BUFFER_USAGE` değeri |

TP001, A ve B buffer'larını farklı başlangıç boyutlarıyla oluşturur. A'yı
SSBO indexed binding 0'a, B'yi genel SSBO hedefine bağlar. `glBufferData`
sonrasında B'nin istenen boyuta ulaştığını ve A'nın boyutunun değişmediğini
kontrol eder. `glBindBufferBase` genel bağı da değiştirdiğinden B'nin genel
bağı bundan sonra kurulur.

Usage testleri yeni buffer kullanır ve `GL_BUFFER_USAGE` değerini sorgular.
HLR'lar bu enum'ların kabulünü/kaydedilmesini istediğinden shader çalıştırma
veya usage değerinden performans sonucu çıkarma adımı yoktur.

Tüm TP'ler helpersızdır; API başlığı ve `../../macros.h` kullanılır. Runner,
GLES 3.1 veya üzeri current context, yüklenmiş GL fonksiyonları ve temiz
başlangıç hata durumu sağlar. `init() -> draw() -> close()` sırası kullanılır;
`draw()` boştur. Başarısız kurulumda da aynı context current iken `close()`
çağrılır. Yalnızca TP'nin buffer'ları silinir. TP001 copy-write, genel SSBO
ve indexed binding 0 durumunu; diğerleri genel SSBO bağını değiştirir.
Önceki bağlar geri yüklenmez; sonraki test kendi bağlarını kurmalıdır.

Loader seçimi dispatch TP'leriyle aynıdır: `GS_GLES31_USE_GLAD1`,
`GS_GLES31_USE_GLAD2` veya varsayılan `<GLES3/gl31.h>`. Ortak helper `.c`
dosyası bağlanmaz; TP'lerde `main()` ve context kurulumu yoktur.
`glBufferData.h`, `TEST_PROCEDURE` ve `DEFAULT_TEST_RUN_TIME` makrolarını
sağlayan runner için kayıt listesidir. Her test başlangıcı `Test Case #...`
yorumuyla işaretlidir.

Kaynak: [GLES 3.1, Bölüm 6.1–6.2](https://registry.khronos.org/OpenGL/specs/es/3.1/es_spec_3.1.pdf).

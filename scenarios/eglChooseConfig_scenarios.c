#include <EGL/egl.h>
#include <stddef.h>

/*
 * ============================================================
 * Scenario 1:
 * attrib_list = NULL kullanımı
 * ============================================================
 *
 * Amaç:
 * Herhangi bir özel attribute kriteri verilmeden
 * eglChooseConfig fonksiyonunun kullanılmasını göstermek.
 *
 * attrib_list parametresine NULL verildiğinde belirtilmeyen
 * attribute'lar EGL'nin varsayılan seçim değerlerine göre
 * değerlendirilir.
 *
 * Burada configs = NULL ve config_size = 0 kullanıldığı için
 * yalnızca eşleşen config sayısı sorgulanır.
 *
 * Çağrı:
 *
 *     eglChooseConfig(dpy, NULL, NULL, 0, &num_config);
 *
 * Başarılı durumda:
 *
 *     result == EGL_TRUE
 *
 * olur.
 *
 * Bizim test ortamımızdaki olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     num_config: 50
 *
 * Config sayısı EGL implementation'a ve sisteme bağlıdır.
 * Bu nedenle farklı sistemlerde farklı bir değer görülebilir.
 */
void scenario_null_attribute_list(EGLDisplay dpy)
{
    EGLint num_config = -1;

    EGLBoolean result =
        eglChooseConfig(
            dpy,
            NULL,
            NULL,
            0,
            &num_config
        );

    (void)result;
    (void)num_config;
}


/*
 * ============================================================
 * Scenario 2A:
 * RGB888 attribute kriterleri
 * ============================================================
 *
 * Amaç:
 * attrib_list kullanılarak minimum 8 bit red, green ve blue
 * component isteyen config'lerin seçilmesini göstermek.
 *
 * Attribute listesi EGL_NONE ile sonlandırılmalıdır.
 *
 * Buradaki:
 *
 *     EGL_RED_SIZE, 8
 *
 * ifadesi red değerinin mutlaka tam olarak 8 bit olması
 * gerektiği anlamına gelmez.
 *
 * Size attribute'ları minimum gereksinim olarak değerlendirilir.
 *
 * Yani yaklaşık olarak:
 *
 *     RED   >= 8
 *     GREEN >= 8
 *     BLUE  >= 8
 *
 * şartlarını sağlayan config'ler aranır.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     num_config: 50
 *
 * Gerçek sayı implementation'a bağlıdır.
 */
void scenario_rgb888(EGLDisplay dpy)
{
    const EGLint attrib_list[] = {
        EGL_RED_SIZE,   8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE,  8,
        EGL_NONE
    };

    EGLint num_config = -1;

    EGLBoolean result =
        eglChooseConfig(
            dpy,
            attrib_list,
            NULL,
            0,
            &num_config
        );

    (void)result;
    (void)num_config;
}


/*
 * ============================================================
 * Scenario 2B:
 * RGB565 attribute kriterleri
 * ============================================================
 *
 * Amaç:
 * Farklı component boyutları verilerek config seçiminin nasıl
 * değişebileceğini göstermek.
 *
 * Bu senaryoda minimum:
 *
 *     RED   = 5 bit
 *     GREEN = 6 bit
 *     BLUE  = 5 bit
 *
 * istenir.
 *
 * Çağrı başarılı olduğunda:
 *
 *     result == EGL_TRUE
 *
 * olur ve num_config eşleşen config sayısını içerir.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     num_config: 20
 *
 * Buradaki sayı yalnızca örnektir.
 * Gerçek sonuç sisteme ve EGL implementation'a bağlıdır.
 */
void scenario_rgb565(EGLDisplay dpy)
{
    const EGLint attrib_list[] = {
        EGL_RED_SIZE,   5,
        EGL_GREEN_SIZE, 6,
        EGL_BLUE_SIZE,  5,
        EGL_NONE
    };

    EGLint num_config = -1;

    EGLBoolean result =
        eglChooseConfig(
            dpy,
            attrib_list,
            NULL,
            0,
            &num_config
        );

    (void)result;
    (void)num_config;
}


/*
 * ============================================================
 * Scenario 2C:
 * Hiçbir config ile eşleşmeyen kriterler
 * ============================================================
 *
 * Amaç:
 * Çok yüksek buffer gereksinimleri verilerek hiçbir config'in
 * kriterleri karşılamadığı durumu göstermek.
 *
 * Önemli nokta:
 *
 * Hiç config bulunmaması tek başına EGL API hatası değildir.
 *
 * Bu durumda:
 *
 *     result == EGL_TRUE
 *     num_config == 0
 *
 * olabilir.
 *
 * Yani:
 *
 *     EGL_TRUE
 *
 * sonucu "mutlaka bir config bulundu" anlamına gelmez.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     num_config: 0
 */
void scenario_no_matching_config(EGLDisplay dpy)
{
    const EGLint attrib_list[] = {
        EGL_RED_SIZE,     64,
        EGL_GREEN_SIZE,   64,
        EGL_BLUE_SIZE,    64,
        EGL_DEPTH_SIZE,  128,
        EGL_STENCIL_SIZE, 64,
        EGL_NONE
    };

    EGLint num_config = -1;

    EGLBoolean result =
        eglChooseConfig(
            dpy,
            attrib_list,
            NULL,
            0,
            &num_config
        );

    (void)result;
    (void)num_config;
}


/*
 * ============================================================
 * Scenario 2D:
 * EGL_DONT_CARE kullanımı
 * ============================================================
 *
 * Amaç:
 * Belirli bir attribute'un seçim sırasında önemli olmadığını
 * EGL_DONT_CARE ile belirtmek.
 *
 * Bu örnekte RGB888 kriterleri korunurken alpha component
 * boyutu için özel bir gereksinim istenmez.
 *
 *     EGL_ALPHA_SIZE, EGL_DONT_CARE
 *
 * ifadesi alpha kriterinin seçim sırasında önemsenmemesini ister.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     num_config: 50
 *
 * Config sayısı implementation'a bağlıdır.
 */
void scenario_dont_care(EGLDisplay dpy)
{
    const EGLint attrib_list[] = {
        EGL_RED_SIZE,   8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE,  8,
        EGL_ALPHA_SIZE, EGL_DONT_CARE,
        EGL_NONE
    };

    EGLint num_config = -1;

    EGLBoolean result =
        eglChooseConfig(
            dpy,
            attrib_list,
            NULL,
            0,
            &num_config
        );

    (void)result;
    (void)num_config;
}


/*
 * ============================================================
 * Scenario 3:
 * Yalnızca eşleşen config sayısını sorgulamak
 * ============================================================
 *
 * Amaç:
 * EGLConfig handle'larını almadan önce kaç config bulunduğunu
 * öğrenmek.
 *
 * Bunun için:
 *
 *     configs = NULL
 *     config_size = 0
 *
 * kullanılır.
 *
 * Bu yöntem özellikle daha sonra uygun büyüklükte bir output
 * buffer oluşturmak için kullanılabilir.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     matching configs: 10
 *
 * Config sayısı sisteme göre değişebilir.
 */
void scenario_config_count_query(EGLDisplay dpy)
{
    const EGLint attrib_list[] = {
        EGL_RED_SIZE,   8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE,  8,
        EGL_NONE
    };

    EGLint num_config = -1;

    EGLBoolean result =
        eglChooseConfig(
            dpy,
            attrib_list,
            NULL,
            0,
            &num_config
        );

    (void)result;
    (void)num_config;
}


/*
 * ============================================================
 * Scenario 4:
 * Sınırlı output buffer kullanımı
 * ============================================================
 *
 * Amaç:
 * configs parametresine gerçek bir EGLConfig buffer'ı verilmesini
 * ve config_size parametresinin bu buffer'ın kapasitesini
 * belirlediğini göstermek.
 *
 * Burada:
 *
 *     EGLConfig configs[1];
 *     config_size = 1;
 *
 * kullanılır.
 *
 * config_size = 1:
 *
 *     "Bir tane config bul."
 *
 * anlamına gelmez.
 *
 * Bunun anlamı:
 *
 *     "Output buffer en fazla bir EGLConfig handle'ı alabilir."
 *
 * şeklindedir.
 *
 * Eğer kriterleri karşılayan çok sayıda config olsa bile bu
 * çağrıda configs buffer'ına en fazla bir handle yazılır.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     num_config: 1
 *     configs[0]: 0x55c2408bf820
 *
 * EGLConfig handle değeri yalnızca örnektir ve sistemden
 * sisteme değişebilir.
 */
void scenario_limited_output_buffer(EGLDisplay dpy)
{
    const EGLint attrib_list[] = {
        EGL_RED_SIZE,   8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE,  8,
        EGL_NONE
    };

    EGLConfig configs[1];
    EGLint num_config = -1;

    EGLBoolean result =
        eglChooseConfig(
            dpy,
            attrib_list,
            configs,
            1,
            &num_config
        );

    (void)result;
    (void)configs;
    (void)num_config;
}


/*
 * ============================================================
 * Scenario 5A:
 * Geçersiz display - EGL_NO_DISPLAY
 * ============================================================
 *
 * Amaç:
 * dpy parametresine geçerli, initialized bir EGLDisplay yerine
 * EGL_NO_DISPLAY verildiğinde oluşan hata durumunu göstermek.
 *
 * Beklenen sonuç:
 *
 *     result == EGL_FALSE
 *
 * eglGetError çağrısından:
 *
 *     EGL_BAD_DISPLAY
 *
 * alınır.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_FALSE
 *     error: EGL_BAD_DISPLAY
 */
void scenario_invalid_display(void)
{
    EGLint num_config = -1;

    EGLBoolean result =
        eglChooseConfig(
            EGL_NO_DISPLAY,
            NULL,
            NULL,
            0,
            &num_config
        );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
    (void)num_config;
}


/*
 * ============================================================
 * Scenario 5B:
 * Tanınmayan attribute kullanımı
 * ============================================================
 *
 * Amaç:
 * attrib_list içerisinde EGL tarafından tanınmayan bir attribute
 * kullanıldığında oluşan hata davranışını göstermek.
 *
 * Buradaki 0x7FFFFFFF değeri geçersiz bir attribute örneği
 * olarak kullanılmıştır.
 *
 * Beklenen sonuç:
 *
 *     result == EGL_FALSE
 *
 * eglGetError çağrısından:
 *
 *     EGL_BAD_ATTRIBUTE
 *
 * alınması beklenir.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_FALSE
 *     error: EGL_BAD_ATTRIBUTE
 */
void scenario_invalid_attribute(EGLDisplay dpy)
{
    const EGLint attrib_list[] = {
        0x7FFFFFFF, 1,
        EGL_NONE
    };

    EGLint num_config = -1;

    EGLBoolean result =
        eglChooseConfig(
            dpy,
            attrib_list,
            NULL,
            0,
            &num_config
        );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
    (void)num_config;
}


/*
 * ============================================================
 * Scenario 5C:
 * num_config = NULL kullanımı
 * ============================================================
 *
 * Amaç:
 * Zorunlu output parametresi olan num_config yerine NULL
 * verildiğinde oluşan hata durumunu göstermek.
 *
 * Beklenen sonuç:
 *
 *     result == EGL_FALSE
 *
 * eglGetError çağrısından:
 *
 *     EGL_BAD_PARAMETER
 *
 * alınması beklenir.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_FALSE
 *     error: EGL_BAD_PARAMETER
 */
void scenario_null_num_config(EGLDisplay dpy)
{
    EGLBoolean result =
        eglChooseConfig(
            dpy,
            NULL,
            NULL,
            0,
            NULL
        );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}
#include <EGL/egl.h>

/*
 * ============================================================
 * Scenario 1:
 * Geçerli EGLDisplay ile normal eglInitialize kullanımı
 * ============================================================
 *
 * Amaç:
 * Geçerli bir EGLDisplay'i initialize etmek ve EGL implementation
 * tarafından kullanılan major ve minor sürüm numaralarını almak.
 *
 * Önce eglGetDisplay ile bir EGLDisplay elde edilir:
 *
 *     EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
 *
 * Daha sonra major ve minor output parametreleri verilerek
 * eglInitialize çağrılır.
 *
 * Başarılı durumda:
 *
 *     result == EGL_TRUE
 *
 * olur ve major/minor değişkenlerine EGL sürüm bilgisi yazılır.
 *
 * Bizim test ortamımızda olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     EGL version: 1.5
 *
 * Buradaki 1.5 değeri test ortamındaki EGL implementation'a aittir.
 * Farklı sistemlerde farklı bir EGL sürümü dönebilir.
 *
 * major ve minor değişkenlerine başlangıçta -1 verilmesi,
 * fonksiyonun bu değerleri gerçekten değiştirip değiştirmediğini
 * gözlemlemeyi kolaylaştırır.
 */
void scenario_normal_initialize(EGLDisplay dpy)
{
    EGLint major = -1;
    EGLint minor = -1;

    EGLBoolean result =
        eglInitialize(dpy, &major, &minor);

    (void)result;
    (void)major;
    (void)minor;
}


/*
 * ============================================================
 * Scenario 2A:
 * major = NULL kullanımı
 * ============================================================
 *
 * Amaç:
 * Major sürüm bilgisi istenmediğinde major parametresinin NULL
 * olarak verilebildiğini göstermek.
 *
 * Bu durumda yalnızca minor sürüm bilgisi alınır.
 *
 * Çağrı:
 *
 *     eglInitialize(dpy, NULL, &minor);
 *
 * Başarılı durumda:
 *
 *     result == EGL_TRUE
 *
 * olur.
 *
 * Bizim test ortamımızda olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     minor: 5
 *
 * major parametresi NULL olduğu için major sürüm bilgisi
 * uygulamaya yazılmaz.
 */
void scenario_major_null(EGLDisplay dpy)
{
    EGLint minor = -1;

    EGLBoolean result =
        eglInitialize(dpy, NULL, &minor);

    (void)result;
    (void)minor;
}


/*
 * ============================================================
 * Scenario 2B:
 * minor = NULL kullanımı
 * ============================================================
 *
 * Amaç:
 * Minor sürüm bilgisi istenmediğinde minor parametresinin NULL
 * olarak verilebildiğini göstermek.
 *
 * Bu durumda yalnızca major sürüm bilgisi alınır.
 *
 * Çağrı:
 *
 *     eglInitialize(dpy, &major, NULL);
 *
 * Başarılı durumda:
 *
 *     result == EGL_TRUE
 *
 * olur.
 *
 * Bizim test ortamımızda olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *     major: 1
 *
 * minor parametresi NULL olduğu için minor sürüm bilgisi
 * uygulamaya yazılmaz.
 */
void scenario_minor_null(EGLDisplay dpy)
{
    EGLint major = -1;

    EGLBoolean result =
        eglInitialize(dpy, &major, NULL);

    (void)result;
    (void)major;
}


/*
 * ============================================================
 * Scenario 2C:
 * major = NULL ve minor = NULL kullanımı
 * ============================================================
 *
 * Amaç:
 * Uygulamanın EGL sürüm numarasına ihtiyacı olmadığında her iki
 * output parametresinin de NULL verilebildiğini göstermek.
 *
 * Çağrı:
 *
 *     eglInitialize(dpy, NULL, NULL);
 *
 * Bu kullanımda display yine initialize edilir ancak major ve
 * minor sürüm bilgileri uygulamaya döndürülmez.
 *
 * Başarılı durumda:
 *
 *     result == EGL_TRUE
 *
 * olur.
 *
 * Olası örnek sonuç:
 *
 *     result: EGL_TRUE
 *
 * Bu senaryoda major/minor çıktısı yoktur çünkü her iki output
 * pointer da NULL verilmiştir.
 */
void scenario_major_minor_null(EGLDisplay dpy)
{
    EGLBoolean result =
        eglInitialize(dpy, NULL, NULL);

    (void)result;
}


/*
 * ============================================================
 * Scenario 3:
 * Geçersiz display - EGL_NO_DISPLAY
 * ============================================================
 *
 * Amaç:
 * eglInitialize fonksiyonuna geçerli bir EGLDisplay yerine
 * EGL_NO_DISPLAY verildiğinde oluşan hata davranışını göstermek.
 *
 * Çağrı:
 *
 *     eglInitialize(EGL_NO_DISPLAY, &major, &minor);
 *
 * Beklenen sonuç:
 *
 *     result == EGL_FALSE
 *
 * ve eglGetError çağrısından:
 *
 *     EGL_BAD_DISPLAY
 *
 * hata kodu alınır.
 *
 * Olası örnek çıktı:
 *
 *     result: EGL_FALSE
 *     error: EGL_BAD_DISPLAY
 *
 * Bu durumda display geçerli olmadığı için initialization
 * gerçekleştirilemez.
 */
void scenario_invalid_display(void)
{
    EGLint major = -1;
    EGLint minor = -1;

    EGLBoolean result =
        eglInitialize(EGL_NO_DISPLAY, &major, &minor);

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}
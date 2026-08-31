#include <EGL/egl.h>

/*
 * ============================================================
 * Scenario 1:
 * Current context yokken eglGetCurrentDisplay kullanımı
 * ============================================================
 *
 * Amaç:
 * Henüz herhangi bir EGL context current yapılmamışken
 * eglGetCurrentDisplay fonksiyonunun davranışını göstermek.
 *
 * Programın başlangıcında current EGL context yoktur.
 *
 * Bu durumda:
 *
 *     eglGetCurrentDisplay()
 *
 * çağrısı:
 *
 *     EGL_NO_DISPLAY
 *
 * döndürür.
 *
 * Olası örnek sonuç:
 *
 *     current_display: EGL_NO_DISPLAY
 *
 * Önemli:
 *
 * Buradaki EGL_NO_DISPLAY tek başına bir EGL API hatası
 * anlamına gelmez.
 *
 * Current context bulunmadığı için oluşan normal bir state
 * sonucudur.
 */
void scenario_no_current_context(void)
{
    EGLDisplay current_display =
        eglGetCurrentDisplay();

    (void)current_display;
}


/*
 * ============================================================
 * Scenario 2:
 * Display initialize edilmiş fakat current context yok
 * ============================================================
 *
 * Amaç:
 * Bir EGLDisplay'in eglInitialize ile initialize edilmesinin
 * onu otomatik olarak current display yapmadığını göstermek.
 *
 * Tipik olarak daha önce:
 *
 *     EGLDisplay dpy =
 *         eglGetDisplay(EGL_DEFAULT_DISPLAY);
 *
 *     eglInitialize(dpy, &major, &minor);
 *
 * işlemleri yapılmış olabilir.
 *
 * Ancak henüz eglMakeCurrent çağrılmadıysa current context
 * oluşturulmuş değildir.
 *
 * Bu nedenle:
 *
 *     eglGetCurrentDisplay()
 *
 * yine:
 *
 *     EGL_NO_DISPLAY
 *
 * döndürür.
 *
 * Olası örnek sonuç:
 *
 *     eglInitialize: EGL_TRUE
 *     current_display: EGL_NO_DISPLAY
 *
 * Bu senaryo şu ayrımı gösterir:
 *
 *     initialized display != current display
 */
void scenario_after_initialize(EGLDisplay dpy)
{
    EGLint major = -1;
    EGLint minor = -1;

    eglInitialize(dpy, &major, &minor);

    EGLDisplay current_display =
        eglGetCurrentDisplay();

    (void)current_display;
}


/*
 * ============================================================
 * Scenario 3:
 * eglMakeCurrent sonrası eglGetCurrentDisplay kullanımı
 * ============================================================
 *
 * Amaç:
 * Bir EGLContext başarıyla current yapıldıktan sonra
 * eglGetCurrentDisplay fonksiyonunun bu context ile ilişkili
 * EGLDisplay handle'ını döndürdüğünü göstermek.
 *
 * Önceden geçerli:
 *
 *     EGLDisplay
 *     EGLSurface
 *     EGLContext
 *
 * oluşturulmuş olmalıdır.
 *
 * Daha sonra:
 *
 *     eglMakeCurrent(dpy, surface, surface, context);
 *
 * çağrısı ile context current yapılır.
 *
 * Başarılı durumda:
 *
 *     eglGetCurrentDisplay() == dpy
 *
 * olması beklenir.
 *
 * Olası örnek sonuç:
 *
 *     dpy:             0x55c1a85091f0
 *     current_display: 0x55c1a85091f0
 *     Same display: EGL_TRUE
 *
 * Handle değerleri yalnızca örnektir ve sistemden sisteme
 * değişebilir.
 *
 * Burada önemli olan current_display ile dpy değerlerinin
 * aynı EGLDisplay handle'ını temsil etmesidir.
 */
void scenario_after_make_current(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext context)
{
    eglMakeCurrent(
        dpy,
        surface,
        surface,
        context
    );

    EGLDisplay current_display =
        eglGetCurrentDisplay();

    EGLBoolean same_display =
        (current_display == dpy)
            ? EGL_TRUE
            : EGL_FALSE;

    (void)current_display;
    (void)same_display;
}


/*
 * ============================================================
 * Scenario 4:
 * Current context release edildikten sonra
 * ============================================================
 *
 * Amaç:
 * Current context'in bırakılmasından sonra
 * eglGetCurrentDisplay fonksiyonunun tekrar
 * EGL_NO_DISPLAY döndürdüğünü göstermek.
 *
 * Current state şu çağrı ile bırakılabilir:
 *
 *     eglMakeCurrent(
 *         dpy,
 *         EGL_NO_SURFACE,
 *         EGL_NO_SURFACE,
 *         EGL_NO_CONTEXT
 *     );
 *
 * Bundan sonra çağrı yapan thread üzerinde current context
 * bulunmaz.
 *
 * Bu nedenle:
 *
 *     eglGetCurrentDisplay()
 *
 * sonucu:
 *
 *     EGL_NO_DISPLAY
 *
 * olur.
 *
 * Olası örnek sonuç:
 *
 *     Before release:
 *     current_display: 0x55c1a85091f0
 *
 *     After release:
 *     current_display: EGL_NO_DISPLAY
 *
 * Böylece current display'in kalıcı bir display seçimi değil,
 * current context state'ine bağlı olduğu görülür.
 */
void scenario_after_release(EGLDisplay dpy)
{
    eglMakeCurrent(
        dpy,
        EGL_NO_SURFACE,
        EGL_NO_SURFACE,
        EGL_NO_CONTEXT
    );

    EGLDisplay current_display =
        eglGetCurrentDisplay();

    (void)current_display;
}
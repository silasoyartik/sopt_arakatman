#include <EGL/egl.h>
#include <stddef.h>

/*
 * ============================================================
 * Scenario 1A:
 * Geçerli EGLDisplay ile window surface oluşturma
 * ============================================================
 *
 * Amaç:
 * Initialize edilmiş, geçerli bir EGLDisplay kullanıldığında
 * eglCreateWindowSurface fonksiyonunun normal akışını göstermek.
 *
 * config aynı display'a ait olmalı ve EGL_WINDOW_BIT
 * desteklemelidir. win ise aktif EGL platformunun beklediği
 * geçerli native window nesnesi olmalıdır.
 *
 * Başarılı durumda:
 *
 *     surface != EGL_NO_SURFACE
 *
 * olur.
 *
 * Oluşturulan surface kullanıldıktan sonra eglDestroySurface
 * ile yok edilmelidir.
 */
void scenario_valid_display(
    EGLDisplay dpy,
    EGLConfig config,
    EGLNativeWindowType win)
{
    EGLSurface surface =
        eglCreateWindowSurface(
            dpy,
            config,
            win,
            NULL
        );

    (void)surface;
}


/*
 * ============================================================
 * Scenario 1B:
 * Geçersiz display - EGL_NO_DISPLAY
 * ============================================================
 *
 * Amaç:
 * dpy parametresine EGL_NO_DISPLAY verildiğinde oluşan hata
 * durumunu göstermek.
 *
 * Beklenen sonuç:
 *
 *     surface == EGL_NO_SURFACE
 *     error   == EGL_BAD_DISPLAY
 */
void scenario_no_display(
    EGLConfig config,
    EGLNativeWindowType win)
{
    (void)eglGetError();

    EGLSurface surface =
        eglCreateWindowSurface(
            EGL_NO_DISPLAY,
            config,
            win,
            NULL
        );

    EGLint error = eglGetError();

    (void)surface;
    (void)error;
}


/*
 * ============================================================
 * Scenario 2A:
 * Geçerli EGLConfig kullanımı
 * ============================================================
 *
 * Amaç:
 * dpy ile aynı display'a ait ve EGL_SURFACE_TYPE attribute'u
 * EGL_WINDOW_BIT içeren geçerli bir config ile window surface
 * oluşturmayı göstermek.
 *
 * Config'in renk, alpha, depth ve stencil gibi özellikleri bu
 * senaryonun konusu değildir. Bu değerler eglGetConfigAttrib
 * ile ayrıca sorgulanabilir.
 *
 * Diğer parametreler de geçerliyse beklenen sonuç:
 *
 *     surface != EGL_NO_SURFACE
 */
void scenario_valid_config(
    EGLDisplay dpy,
    EGLConfig valid_config,
    EGLNativeWindowType win)
{
    EGLSurface surface =
        eglCreateWindowSurface(
            dpy,
            valid_config,
            win,
            NULL
        );

    (void)surface;
}


/*
 * ============================================================
 * Scenario 2B:
 * Geçersiz EGLConfig kullanımı
 * ============================================================
 *
 * Amaç:
 * Geçerli bir EGLConfig adı vermeyen config parametresinin hata
 * davranışını göstermek.
 *
 * (EGLConfig)0 burada yalnızca geçersiz handle senaryosunu
 * temsil eder.
 *
 * Beklenen sonuç:
 *
 *     surface == EGL_NO_SURFACE
 *     error   == EGL_BAD_CONFIG
 */
void scenario_invalid_config(
    EGLDisplay dpy,
    EGLNativeWindowType win)
{
    EGLConfig invalid_config = (EGLConfig)0;

    (void)eglGetError();

    EGLSurface surface =
        eglCreateWindowSurface(
            dpy,
            invalid_config,
            win,
            NULL
        );

    EGLint error = eglGetError();

    (void)surface;
    (void)error;
}


/*
 * ============================================================
 * Scenario 3A:
 * Platformla uyumlu geçerli native window
 * ============================================================
 *
 * Amaç:
 * EGLNativeWindowType değerinin projede kullanılan native
 * platformdan geldiğini göstermek.
 *
 * win değeri platforma göre örneğin şunlardan biri olabilir:
 *
 *     X11      -> Window
 *     Wayland  -> struct wl_egl_window *
 *     Windows  -> HWND
 *     Mesa/GBM -> struct gbm_surface *
 *
 * Bu platform nesnesi ilgili EGL header tanımına uygun biçimde
 * EGLNativeWindowType olarak bu fonksiyona verilmelidir.
 *
 * Diğer parametreler de geçerliyse beklenen sonuç:
 *
 *     surface != EGL_NO_SURFACE
 */
void scenario_valid_native_window(
    EGLDisplay dpy,
    EGLConfig config,
    EGLNativeWindowType win)
{
    EGLSurface surface =
        eglCreateWindowSurface(
            dpy,
            config,
            win,
            NULL
        );

    (void)surface;
}


/*
 * ============================================================
 * Scenario 3B:
 * Geçersiz native window
 * ============================================================
 *
 * Amaç:
 * Aktif EGL platformunda geçerli bir native window adı vermeyen
 * win değerinin hata davranışını göstermek.
 *
 * Geçersiz native handle implementation tarafından algılanabiliyorsa
 * beklenen sonuç:
 *
 *     surface == EGL_NO_SURFACE
 *     error   == EGL_BAD_NATIVE_WINDOW
 *
 * EGL 1.0'a göre geçersiz native nesnelerin algılanması her
 * platformda garanti edilmez. Bu nedenle bu senaryo kontrollü bir
 * test ortamında çalıştırılmalıdır.
 */
void scenario_invalid_native_window(
    EGLDisplay dpy,
    EGLConfig config)
{
    EGLNativeWindowType invalid_win = (EGLNativeWindowType)0;

    (void)eglGetError();

    EGLSurface surface =
        eglCreateWindowSurface(
            dpy,
            config,
            invalid_win,
            NULL
        );

    EGLint error = eglGetError();

    (void)surface;
    (void)error;
}


/*
 * ============================================================
 * Scenario 4A:
 * attrib_list = NULL kullanımı
 * ============================================================
 *
 * Amaç:
 * EGL 1.0 core'da tanımlanmış değiştirilebilir bir window
 * surface attribute'u olmadığında attrib_list parametresinin
 * NULL verilebildiğini göstermek.
 *
 * NULL, ek window surface attribute'u verilmediğini belirtir.
 *
 * Diğer parametreler geçerliyse beklenen sonuç:
 *
 *     surface != EGL_NO_SURFACE
 */
void scenario_null_attribute_list(
    EGLDisplay dpy,
    EGLConfig config,
    EGLNativeWindowType win)
{
    EGLSurface surface =
        eglCreateWindowSurface(
            dpy,
            config,
            win,
            NULL
        );

    (void)surface;
}


/*
 * ============================================================
 * Scenario 4B:
 * attrib_list = { EGL_NONE } kullanımı
 * ============================================================
 *
 * Amaç:
 * Boş bir attribute listesinin ilk elemanı EGL_NONE olacak
 * biçimde verilebildiğini göstermek.
 *
 * Listenin ilk elemanı EGL_NONE olduğu için liste hemen biter.
 * EGL 1.0 core açısından bu kullanım attrib_list = NULL ile
 * aynı anlamdadır.
 *
 * Diğer parametreler geçerliyse beklenen sonuç:
 *
 *     surface != EGL_NO_SURFACE
 */
void scenario_empty_attribute_list(
    EGLDisplay dpy,
    EGLConfig config,
    EGLNativeWindowType win)
{
    const EGLint attrib_list[] = {
        EGL_NONE
    };

    EGLSurface surface =
        eglCreateWindowSurface(
            dpy,
            config,
            win,
            attrib_list
        );

    (void)surface;
}

#include <EGL/egl.h>
#include <stddef.h>

/*
 * Scenario 1: Kayıtlı hata yok.
 * İlk çağrı olası eski hatayı temizler; ikinci çağrının
 * EGL_SUCCESS döndürmesi beklenir.
 */
void scenario_error_success(void)
{
    (void)eglGetError();

    EGLint error = eglGetError();

    (void)error;
}


/*
 * Scenario 2: EGL_NOT_INITIALIZED.
 * dpy geçerli fakat initialize edilmemiş olmalıdır.
 */
void scenario_error_not_initialized(EGLDisplay dpy)
{
    EGLint num_config = 0;

    (void)eglGetError();

    EGLBoolean result = eglChooseConfig(
        dpy,
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
 * Scenario 3: EGL_BAD_ACCESS.
 * context başka bir thread'de current iken bu fonksiyon ikinci
 * thread'de çağrılmalıdır. Bu precondition yoksa çağrı başarılı
 * olabilir ve EGL_BAD_ACCESS oluşmaz.
 */
void scenario_error_bad_access(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext context)
{
    (void)eglGetError();

    EGLBoolean result = eglMakeCurrent(
        dpy,
        surface,
        surface,
        context
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/*
 * Scenario 4: EGL_BAD_ALLOC.
 * Kaynak yetersizliği güvenli ve deterministik biçimde zorlanamaz.
 * Bu fonksiyon, bir oluşturma çağrısı EGL_NO_SURFACE döndürdüğü
 * anda hata kodunun hemen okunmasını gösterir.
 */
void scenario_observe_error_after_failed_surface_creation(
    EGLSurface creation_result)
{
    if (creation_result != EGL_NO_SURFACE) {
        return;
    }

    EGLint error = eglGetError();

    (void)error;
}


/* Scenario 5: Tanınmayan attribute -> EGL_BAD_ATTRIBUTE. */
void scenario_error_bad_attribute(EGLDisplay dpy)
{
    const EGLint attributes[] = {
        0x7FFFFFFF, 1,
        EGL_NONE
    };

    EGLint num_config = 0;

    (void)eglGetError();

    EGLBoolean result = eglChooseConfig(
        dpy,
        attributes,
        NULL,
        0,
        &num_config
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
    (void)num_config;
}


/* Scenario 6: Geçersiz context -> EGL_BAD_CONTEXT. */
void scenario_error_bad_context(EGLDisplay dpy)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroyContext(
        dpy,
        (EGLContext)0
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/* Scenario 7: Geçersiz config -> EGL_BAD_CONFIG. */
void scenario_error_bad_config(EGLDisplay dpy)
{
    (void)eglGetError();

    EGLContext context = eglCreateContext(
        dpy,
        (EGLConfig)0,
        EGL_NO_CONTEXT,
        NULL
    );

    EGLint error = eglGetError();

    (void)context;
    (void)error;
}


/*
 * Scenario 8: EGL_BAD_CURRENT_SURFACE.
 * Bu hata native window/current surface yaşam döngüsüne bağlıdır.
 * Current surface platform tarafında geçersiz hale getirildikten sonra
 * swap denenerek hata gözlemlenir.
 */
void scenario_error_bad_current_surface(
    EGLDisplay dpy,
    EGLSurface current_surface)
{
    (void)eglGetError();

    EGLBoolean result = eglSwapBuffers(dpy, current_surface);
    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/* Scenario 9: EGL_NO_DISPLAY -> EGL_BAD_DISPLAY. */
void scenario_error_bad_display(void)
{
    (void)eglGetError();

    EGLBoolean result = eglInitialize(
        EGL_NO_DISPLAY,
        NULL,
        NULL
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/* Scenario 10: Geçersiz surface -> EGL_BAD_SURFACE. */
void scenario_error_bad_surface(EGLDisplay dpy)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroySurface(
        dpy,
        (EGLSurface)0
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/*
 * Scenario 11: Uyumsuz context ve surface -> EGL_BAD_MATCH.
 * Fonksiyona tek tek geçerli fakat birbiriyle uyumsuz nesneler
 * verilmelidir.
 */
void scenario_error_bad_match(
    EGLDisplay dpy,
    EGLSurface incompatible_surface,
    EGLContext context)
{
    (void)eglGetError();

    EGLBoolean result = eglMakeCurrent(
        dpy,
        incompatible_surface,
        incompatible_surface,
        context
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/* Scenario 12: Zorunlu output pointer NULL -> EGL_BAD_PARAMETER. */
void scenario_error_bad_parameter(EGLDisplay dpy)
{
    (void)eglGetError();

    EGLBoolean result = eglChooseConfig(
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


/*
 * Scenario 13: Geçersiz native pixmap -> EGL_BAD_NATIVE_PIXMAP.
 * Geçersiz native handle'ların algılanması platforma bağlıdır.
 */
void scenario_error_bad_native_pixmap(
    EGLDisplay dpy,
    EGLConfig config)
{
    EGLNativePixmapType invalid_pixmap = (EGLNativePixmapType)0;

    (void)eglGetError();

    EGLSurface surface = eglCreatePixmapSurface(
        dpy,
        config,
        invalid_pixmap,
        NULL
    );

    EGLint error = eglGetError();

    (void)surface;
    (void)error;
}


/*
 * Scenario 14: Geçersiz native window -> EGL_BAD_NATIVE_WINDOW.
 * Geçersiz native handle'ların algılanması platforma bağlıdır.
 */
void scenario_error_bad_native_window(
    EGLDisplay dpy,
    EGLConfig config)
{
    EGLNativeWindowType invalid_window = (EGLNativeWindowType)0;

    (void)eglGetError();

    EGLSurface surface = eglCreateWindowSurface(
        dpy,
        config,
        invalid_window,
        NULL
    );

    EGLint error = eglGetError();

    (void)surface;
    (void)error;
}


/*
 * Scenario 15: Hata durumunun ilk eglGetError ile sıfırlanması.
 * first_error için EGL_BAD_SURFACE, second_error için EGL_SUCCESS
 * beklenir.
 */
void scenario_error_is_cleared_after_read(EGLDisplay dpy)
{
    (void)eglGetError();
    (void)eglDestroySurface(dpy, (EGLSurface)0);

    EGLint first_error = eglGetError();
    EGLint second_error = eglGetError();

    (void)first_error;
    (void)second_error;
}


const char *egl_error_string(EGLint error)
{
    switch (error) {
        case EGL_SUCCESS:             return "EGL_SUCCESS";
        case EGL_NOT_INITIALIZED:     return "EGL_NOT_INITIALIZED";
        case EGL_BAD_ACCESS:          return "EGL_BAD_ACCESS";
        case EGL_BAD_ALLOC:           return "EGL_BAD_ALLOC";
        case EGL_BAD_ATTRIBUTE:       return "EGL_BAD_ATTRIBUTE";
        case EGL_BAD_CONTEXT:         return "EGL_BAD_CONTEXT";
        case EGL_BAD_CONFIG:          return "EGL_BAD_CONFIG";
        case EGL_BAD_CURRENT_SURFACE: return "EGL_BAD_CURRENT_SURFACE";
        case EGL_BAD_DISPLAY:         return "EGL_BAD_DISPLAY";
        case EGL_BAD_SURFACE:         return "EGL_BAD_SURFACE";
        case EGL_BAD_MATCH:           return "EGL_BAD_MATCH";
        case EGL_BAD_PARAMETER:       return "EGL_BAD_PARAMETER";
        case EGL_BAD_NATIVE_PIXMAP:   return "EGL_BAD_NATIVE_PIXMAP";
        case EGL_BAD_NATIVE_WINDOW:   return "EGL_BAD_NATIVE_WINDOW";
        default:                      return "UNKNOWN_EGL_ERROR";
    }
}

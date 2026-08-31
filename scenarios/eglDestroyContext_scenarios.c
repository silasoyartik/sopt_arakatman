#include <EGL/egl.h>

/*
 * Scenario 1: Geçerli ve current olmayan context.
 * Beklenen sonuç result == EGL_TRUE değeridir.
 */
void scenario_destroy_non_current_context(
    EGLDisplay dpy,
    EGLContext context)
{
    EGLBoolean result = eglDestroyContext(dpy, context);

    (void)result;
}


/*
 * Scenario 2: Geçerli ve current context.
 *
 * eglDestroyContext EGL_TRUE döndürür; context gerçekte hemen
 * serbest bırakılmaz. Sonraki geçerli eglMakeCurrent çağrısı
 * context'i current durumdan çıkarınca silme tamamlanabilir.
 *
 * Bu fonksiyona girildiğinde context'in verilen surface ile current
 * yapılabildiği varsayılır.
 */
void scenario_destroy_current_context(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext context)
{
    EGLBoolean made_current = eglMakeCurrent(
        dpy,
        surface,
        surface,
        context
    );

    if (made_current == EGL_FALSE) {
        return;
    }

    EGLBoolean destroy_result = eglDestroyContext(dpy, context);

    EGLBoolean release_result = eglMakeCurrent(
        dpy,
        EGL_NO_SURFACE,
        EGL_NO_SURFACE,
        EGL_NO_CONTEXT
    );

    (void)destroy_result;
    (void)release_result;
}


/*
 * Scenario 3: Geçersiz EGLContext.
 * Beklenen sonuç EGL_FALSE ve EGL_BAD_CONTEXT hatasıdır.
 */
void scenario_destroy_invalid_context(EGLDisplay dpy)
{
    EGLContext invalid_context = (EGLContext)0;

    (void)eglGetError();

    EGLBoolean result = eglDestroyContext(dpy, invalid_context);
    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/*
 * Scenario 4: dpy = EGL_NO_DISPLAY.
 * Beklenen sonuç EGL_FALSE ve EGL_BAD_DISPLAY hatasıdır.
 */
void scenario_destroy_context_no_display(EGLContext context)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroyContext(
        EGL_NO_DISPLAY,
        context
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/*
 * Scenario 5: Geçerli fakat initialize edilmemiş display.
 * Beklenen sonuç EGL_FALSE ve EGL_NOT_INITIALIZED hatasıdır.
 *
 * uninitialized_dpy daha önce eglInitialize ile initialize edilmemiş
 * geçerli bir EGLDisplay handle'ı olmalıdır.
 */
void scenario_destroy_context_uninitialized_display(
    EGLDisplay uninitialized_dpy,
    EGLContext context)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroyContext(
        uninitialized_dpy,
        context
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}

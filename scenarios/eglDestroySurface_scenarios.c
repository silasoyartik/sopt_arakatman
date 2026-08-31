#include <EGL/egl.h>

/*
 * Scenario 1: Geçerli ve current olmayan surface.
 * Beklenen sonuç result == EGL_TRUE değeridir.
 */
void scenario_destroy_non_current_surface(
    EGLDisplay dpy,
    EGLSurface surface)
{
    EGLBoolean result = eglDestroySurface(dpy, surface);

    (void)result;
}


/*
 * Scenario 2: Geçerli ve current surface.
 *
 * eglDestroySurface EGL_TRUE döndürür; surface gerçekte hemen
 * serbest bırakılmaz. Sonraki geçerli eglMakeCurrent çağrısı
 * surface'i current durumdan çıkarınca silme tamamlanabilir.
 */
void scenario_destroy_current_surface(
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

    EGLBoolean destroy_result = eglDestroySurface(dpy, surface);

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
 * Scenario 3: Geçersiz EGLSurface.
 * Beklenen sonuç EGL_FALSE ve EGL_BAD_SURFACE hatasıdır.
 */
void scenario_destroy_invalid_surface(EGLDisplay dpy)
{
    EGLSurface invalid_surface = (EGLSurface)0;

    (void)eglGetError();

    EGLBoolean result = eglDestroySurface(dpy, invalid_surface);
    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/*
 * Scenario 4: dpy = EGL_NO_DISPLAY.
 * Beklenen sonuç EGL_FALSE ve EGL_BAD_DISPLAY hatasıdır.
 */
void scenario_destroy_surface_no_display(EGLSurface surface)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroySurface(
        EGL_NO_DISPLAY,
        surface
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}


/*
 * Scenario 5: Geçerli fakat initialize edilmemiş display.
 * Beklenen sonuç EGL_FALSE ve EGL_NOT_INITIALIZED hatasıdır.
 */
void scenario_destroy_surface_uninitialized_display(
    EGLDisplay uninitialized_dpy,
    EGLSurface surface)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroySurface(
        uninitialized_dpy,
        surface
    );

    EGLint error = eglGetError();

    (void)result;
    (void)error;
}

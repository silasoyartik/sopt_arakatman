#include <EGL/egl.h>
#include <stdint.h>
#include <stdio.h>

/*
 * eglSwapBuffers(dpy, surface) icin all-in-one senaryo dosyasi.
 *
 * Pencere/pixmap olusturma platforma bagli oldugu icin bu nesneler fixture
 * tarafindan parametre olarak verilir. Her basari senaryosu initialized bir
 * display, onunla uyumlu bir context ve belirtilen tipte bir surface bekler.
 */

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:           return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED:   return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:        return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:         return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:     return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONFIG:        return "EGL_BAD_CONFIG";
    case EGL_BAD_CONTEXT:       return "EGL_BAD_CONTEXT";
    case EGL_BAD_CURRENT_SURFACE:return "EGL_BAD_CURRENT_SURFACE";
    case EGL_BAD_DISPLAY:       return "EGL_BAD_DISPLAY";
    case EGL_BAD_MATCH:         return "EGL_BAD_MATCH";
    case EGL_BAD_NATIVE_PIXMAP: return "EGL_BAD_NATIVE_PIXMAP";
    case EGL_BAD_NATIVE_WINDOW: return "EGL_BAD_NATIVE_WINDOW";
    case EGL_BAD_PARAMETER:     return "EGL_BAD_PARAMETER";
    case EGL_BAD_SURFACE:       return "EGL_BAD_SURFACE";
    default:                    return "UNKNOWN_EGL_ERROR";
    }
}

static void report_swap(const char *name, EGLBoolean result,
                        EGLint expected_error)
{
    EGLint error = eglGetError();
    EGLBoolean passed;

    if (expected_error == EGL_SUCCESS) {
        passed = (result == EGL_TRUE && error == EGL_SUCCESS);
    } else {
        passed = (result == EGL_FALSE && error == expected_error);
    }

    printf("%s: %s (result=%s, error=%s)\n",
           name,
           passed ? "PASS" : "FAIL",
           result == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE",
           egl_error_name(error));
}

static EGLBoolean make_surface_current(EGLDisplay dpy, EGLSurface surface,
                                       EGLContext ctx, const char *name)
{
    (void)eglGetError();
    if (eglMakeCurrent(dpy, surface, surface, ctx) == EGL_FALSE) {
        printf("%s kurulamadı: eglMakeCurrent error=%s\n",
               name, egl_error_name(eglGetError()));
        return EGL_FALSE;
    }
    return EGL_TRUE;
}

/* SENARYO 1 - Current window surface: color buffer native window'a post edilir. */
void scenario_swap_current_window_surface(EGLDisplay dpy,
                                          EGLSurface window_surface,
                                          EGLContext ctx)
{
    if (!make_surface_current(dpy, window_surface, ctx, "Senaryo 1"))
        return;

    (void)eglGetError();
    report_swap("Senaryo 1 - current window surface",
                eglSwapBuffers(dpy, window_surface), EGL_SUCCESS);
}

/*
 * SENARYO 2 - Current pbuffer surface.
 * EGL 1.0'a gore cagri basarilidir fakat gorunur bir post islemi yapmaz.
 */
void scenario_swap_current_pbuffer_no_effect(EGLDisplay dpy,
                                             EGLSurface pbuffer_surface,
                                             EGLContext ctx)
{
    if (!make_surface_current(dpy, pbuffer_surface, ctx, "Senaryo 2"))
        return;

    (void)eglGetError();
    report_swap("Senaryo 2 - current pbuffer (no effect)",
                eglSwapBuffers(dpy, pbuffer_surface), EGL_SUCCESS);
}

/*
 * SENARYO 3 - Current pixmap surface.
 * EGL 1.0'a gore cagri basarilidir fakat pixmap'e copy yapmaz.
 */
void scenario_swap_current_pixmap_no_effect(EGLDisplay dpy,
                                            EGLSurface pixmap_surface,
                                            EGLContext ctx)
{
    if (!make_surface_current(dpy, pixmap_surface, ctx, "Senaryo 3"))
        return;

    (void)eglGetError();
    report_swap("Senaryo 3 - current pixmap (no effect)",
                eglSwapBuffers(dpy, pixmap_surface), EGL_SUCCESS);
}

/*
 * SENARYO 4 - Gecerli fakat current olmayan surface.
 * current_surface ve non_current_surface farkli, uyumlu nesneler olmalidir.
 */
void scenario_swap_valid_but_non_current_surface(
    EGLDisplay dpy,
    EGLSurface current_surface,
    EGLSurface non_current_surface,
    EGLContext ctx)
{
    if (!make_surface_current(dpy, current_surface, ctx, "Senaryo 4"))
        return;

    (void)eglGetError();
    report_swap("Senaryo 4 - non-current surface",
                eglSwapBuffers(dpy, non_current_surface), EGL_BAD_SURFACE);
}

/* SENARYO 5 - surface == EGL_NO_SURFACE. */
void scenario_swap_no_surface(EGLDisplay dpy)
{
    (void)eglGetError();
    report_swap("Senaryo 5 - EGL_NO_SURFACE",
                eglSwapBuffers(dpy, EGL_NO_SURFACE), EGL_BAD_SURFACE);
}

/*
 * SENARYO 6 - EGL tarafindan uretilmemis, gecersiz ve non-null handle.
 * Fabricated handle yalnizca kontrollu negatif API testinde kullanilmalidir.
 */
void scenario_swap_invalid_surface(EGLDisplay dpy)
{
    EGLSurface invalid_surface = (EGLSurface)(uintptr_t)1;

    (void)eglGetError();
    report_swap("Senaryo 6 - invalid surface",
                eglSwapBuffers(dpy, invalid_surface), EGL_BAD_SURFACE);
}

/*
 * SENARYO 7 - Current iken eglDestroySurface ile silinmek uzere isaretleme.
 * Surface bagli kaldigi icin hemen yok olmaz ve swap hala basarili olmalidir.
 */
void scenario_swap_destroyed_but_still_current(EGLDisplay dpy,
                                               EGLSurface surface,
                                               EGLContext ctx)
{
    if (!make_surface_current(dpy, surface, ctx, "Senaryo 7"))
        return;

    if (eglDestroySurface(dpy, surface) == EGL_FALSE) {
        printf("Senaryo 7 kurulamadı: eglDestroySurface error=%s\n",
               egl_error_name(eglGetError()));
        return;
    }

    (void)eglGetError();
    report_swap("Senaryo 7 - destroyed but still current",
                eglSwapBuffers(dpy, surface), EGL_SUCCESS);

    (void)eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

/*
 * SENARYO 8 - Yok edilmesi ertelenen surface'i release ettikten sonra kullanma.
 * Release gercek silmeyi tamamlar; eski handle artik EGL_BAD_SURFACE vermelidir.
 */
void scenario_swap_destroyed_and_released_surface(EGLDisplay dpy,
                                                  EGLSurface surface,
                                                  EGLContext ctx)
{
    if (!make_surface_current(dpy, surface, ctx, "Senaryo 8"))
        return;
    if (eglDestroySurface(dpy, surface) == EGL_FALSE) {
        printf("Senaryo 8 kurulamadı: eglDestroySurface error=%s\n",
               egl_error_name(eglGetError()));
        return;
    }
    if (eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE,
                       EGL_NO_CONTEXT) == EGL_FALSE) {
        printf("Senaryo 8 kurulamadı: release error=%s\n",
               egl_error_name(eglGetError()));
        return;
    }

    (void)eglGetError();
    report_swap("Senaryo 8 - destroyed and released surface",
                eglSwapBuffers(dpy, surface), EGL_BAD_SURFACE);
}

/* SENARYO 9 - dpy == EGL_NO_DISPLAY. */
void scenario_swap_no_display(EGLSurface surface)
{
    (void)eglGetError();
    report_swap("Senaryo 9 - EGL_NO_DISPLAY",
                eglSwapBuffers(EGL_NO_DISPLAY, surface), EGL_BAD_DISPLAY);
}

/* SENARYO 10 - EGL tarafindan uretilmemis, gecersiz display handle'i. */
void scenario_swap_invalid_display(EGLSurface surface)
{
    EGLDisplay invalid_dpy = (EGLDisplay)(uintptr_t)1;

    (void)eglGetError();
    report_swap("Senaryo 10 - invalid display",
                eglSwapBuffers(invalid_dpy, surface), EGL_BAD_DISPLAY);
}

/*
 * SENARYO 11 - Gecerli fakat initialize edilmemis display.
 * uninitialized_dpy eglGetDisplay ile alinmis, eglInitialize edilmemis olmali.
 */
void scenario_swap_uninitialized_display(EGLDisplay uninitialized_dpy,
                                         EGLSurface surface)
{
    (void)eglGetError();
    report_swap("Senaryo 11 - uninitialized display",
                eglSwapBuffers(uninitialized_dpy, surface),
                EGL_NOT_INITIALIZED);
}

/*
 * SENARYO 12 - Native window'u artik gecersiz olan current window surface.
 * Fixture once surface'i current yapmali, sonra alttaki native window'u EGL
 * surface'i release etmeden yok etmelidir. Algilanabiliyorsa beklenen hata
 * EGL_BAD_NATIVE_WINDOW'dur; EGL 1.0 her platformda algilamayi garanti etmez.
 */
void scenario_swap_destroyed_native_window(EGLDisplay dpy,
                                           EGLSurface affected_surface)
{
    (void)eglGetError();
    report_swap("Senaryo 12 - destroyed native window",
                eglSwapBuffers(dpy, affected_surface),
                EGL_BAD_NATIVE_WINDOW);
}

/*
 * SENARYO 13 - Native window resize sonrasi swap.
 * Fixture native pencereyi cagri oncesinde yeniden boyutlandirmis olmalidir.
 * EGL surface'i posttan once yeni boyuta uyarlar; GL viewport'u uygulamanin
 * ayrica guncellemesi gerekir.
 */
void scenario_swap_after_native_window_resize(EGLDisplay dpy,
                                              EGLSurface resized_surface)
{
    (void)eglGetError();
    report_swap("Senaryo 13 - resized native window",
                eglSwapBuffers(dpy, resized_surface), EGL_SUCCESS);
}

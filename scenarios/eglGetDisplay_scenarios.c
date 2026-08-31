#include <EGL/egl.h>
#include <wayland-client.h>
#include <stdio.h>

/*
 * eglGetDisplay(display_id) icin all-in-one senaryo dosyasi.
 * main ve diger EGL kurulum adimlari bilerek cikarilmistir.
 */

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:         return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:      return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:       return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:   return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONTEXT:     return "EGL_BAD_CONTEXT";
    case EGL_BAD_CONFIG:      return "EGL_BAD_CONFIG";
    case EGL_BAD_SURFACE:     return "EGL_BAD_SURFACE";
    case EGL_BAD_DISPLAY:     return "EGL_BAD_DISPLAY";
    case EGL_BAD_MATCH:       return "EGL_BAD_MATCH";
    case EGL_BAD_PARAMETER:   return "EGL_BAD_PARAMETER";
    default:                  return "UNKNOWN_EGL_ERROR";
    }
}


/*
 * SENARYO A - EGL_DEFAULT_DISPLAY.
 * Beklenen: EGL_NO_DISPLAY disinda gecerli bir EGLDisplay handle'i.
 */
void scenario_a_default_display(void)
{
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (dpy != EGL_NO_DISPLAY) {
        printf("Senaryo A basarili: EGLDisplay=%p\n", (void *)dpy);
    } else {
        printf("Senaryo A hatali: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SENARYO B - Ayni EGL_DEFAULT_DISPLAY ile iki cagri.
 * Test ortamimizda ayni EGLDisplay handle'i gozlenmesi beklenir.
 */
void scenario_b_repeated_default_display(void)
{
    EGLDisplay dpy1 = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLDisplay dpy2 = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (dpy1 == EGL_NO_DISPLAY || dpy2 == EGL_NO_DISPLAY) {
        printf("Senaryo B hatali: display alinamadi.\n");
        return;
    }

    if (dpy1 == dpy2) {
        printf("Senaryo B basarili: handle'lar ayni (%p).\n",
               (void *)dpy1);
    } else {
        printf("Senaryo B farkli sonuc: dpy1=%p dpy2=%p\n",
               (void *)dpy1, (void *)dpy2);
    }
}


/*
 * SENARYO C - Explicit Wayland display.
 * Beklenen: gecerli wl_display icin EGL_NO_DISPLAY disinda bir handle.
 */
void scenario_c_explicit_wayland_display(void)
{
    struct wl_display *native_dpy = wl_display_connect(NULL);

    if (native_dpy == NULL) {
        printf("Senaryo C baslatilamadi: Wayland display alinamadi.\n");
        return;
    }

    EGLDisplay dpy =
        eglGetDisplay((EGLNativeDisplayType)native_dpy);

    if (dpy != EGL_NO_DISPLAY) {
        printf("Senaryo C basarili: EGLDisplay=%p\n", (void *)dpy);
    } else {
        printf("Senaryo C hatali: %s\n",
               egl_error_name(eglGetError()));
    }

    wl_display_disconnect(native_dpy);
}
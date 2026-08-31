#include <EGL/egl.h>
#include <stdio.h>

/*
 * eglGetCurrentDisplay() icin all-in-one senaryo dosyasi.
 * Fonksiyon parametre almadigi icin current EGL state degistirilir.
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
 * SENARYO A - Current context yok.
 * Beklenen: EGL_NO_DISPLAY.
 */
void scenario_a_no_current_context(void)
{
    EGLDisplay current = eglGetCurrentDisplay();

    if (current == EGL_NO_DISPLAY) {
        printf("Senaryo A basarili: EGL_NO_DISPLAY dondu.\n");
    } else {
        printf("Senaryo A farkli sonuc: current=%p\n",
               (void *)current);
    }
}


/*
 * SENARYO B - Display initialize edilmis ancak current context yok.
 * Beklenen: eglInitialize basarili olsa da EGL_NO_DISPLAY.
 */
void scenario_b_initialized_only(EGLDisplay dpy)
{
    EGLint major = -1;
    EGLint minor = -1;

    if (eglInitialize(dpy, &major, &minor) == EGL_FALSE) {
        printf("Senaryo B baslatilamadi: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    EGLDisplay current = eglGetCurrentDisplay();

    if (current == EGL_NO_DISPLAY) {
        printf("Senaryo B basarili: EGL_NO_DISPLAY dondu.\n");
    } else {
        printf("Senaryo B farkli sonuc: current=%p\n",
               (void *)current);
    }
}


/*
 * SENARYO C - eglMakeCurrent sonrasi.
 * Beklenen: eglGetCurrentDisplay() == dpy.
 */
void scenario_c_after_make_current(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext context)
{
    if (eglMakeCurrent(dpy, surface, surface, context) == EGL_FALSE) {
        printf("Senaryo C baslatilamadi: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    EGLDisplay current = eglGetCurrentDisplay();

    if (current == dpy) {
        printf("Senaryo C basarili: current display dpy ile ayni.\n");
    } else {
        printf("Senaryo C hatali: current=%p dpy=%p\n",
               (void *)current, (void *)dpy);
    }
}


/*
 * SENARYO D - Current context release edilir.
 * Beklenen: release sonrasi EGL_NO_DISPLAY.
 */
void scenario_d_after_release(EGLDisplay dpy)
{
    if (eglMakeCurrent(
            dpy,
            EGL_NO_SURFACE,
            EGL_NO_SURFACE,
            EGL_NO_CONTEXT) == EGL_FALSE) {

        printf("Senaryo D baslatilamadi: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    EGLDisplay current = eglGetCurrentDisplay();

    if (current == EGL_NO_DISPLAY) {
        printf("Senaryo D basarili: EGL_NO_DISPLAY dondu.\n");
    } else {
        printf("Senaryo D hatali: current=%p\n",
               (void *)current);
    }
}
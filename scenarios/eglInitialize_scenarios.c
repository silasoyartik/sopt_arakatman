#include <EGL/egl.h>
#include <stdio.h>

/*
 * eglInitialize(dpy, major, minor) icin all-in-one senaryo dosyasi.
 * A-D senaryolari icin tercihen fresh EGLDisplay kullanilmalidir.
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
 * SENARYO A - Display initialization başarılı olur, major ve minor birlikte verilir.
 * Beklenen: EGL_TRUE ve test ortamimizda version=1.5.
 */
void scenario_a_major_and_minor(EGLDisplay dpy)
{
    EGLint major = -1;
    EGLint minor = -1;

    if (eglInitialize(dpy, &major, &minor) == EGL_TRUE) {
        printf("Senaryo A basarili: version=%d.%d\n",
               major, minor);
    } else {
        printf("Senaryo A hatali: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SENARYO B - major=NULL, minor pointer.
 * Test edilen Mesa ortaminda EGL_TRUE, minor=-1 beklenir.
 */
void scenario_b_major_null(EGLDisplay dpy)
{
    EGLint minor = -1;

    if (eglInitialize(dpy, NULL, &minor) == EGL_FALSE) {
        printf("Senaryo B hatali: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (minor == -1) {
        printf("Senaryo B basarili: EGL_TRUE, minor=-1 kaldı.\n");
    } else {
        printf("Senaryo B farkli implementation davranisi: minor=%d\n",
               minor);
    }
}


/*
 * SENARYO C - major pointer, minor=NULL.
 * Test edilen Mesa ortaminda EGL_TRUE, major=-1 beklenir.
 */
void scenario_c_minor_null(EGLDisplay dpy)
{
    EGLint major = -1;

    if (eglInitialize(dpy, &major, NULL) == EGL_FALSE) {
        printf("Senaryo C hatali: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (major == -1) {
        printf("Senaryo C basarili: EGL_TRUE, major=-1 kaldı.\n");
    } else {
        printf("Senaryo C farkli implementation davranisi: major=%d\n",
               major);
    }
}


/*
 * SENARYO D - major=NULL ve minor=NULL.
 * Beklenen: surum output'u alinmadan EGL_TRUE.
 */
void scenario_d_both_null(EGLDisplay dpy)
{
    if (eglInitialize(dpy, NULL, NULL) == EGL_TRUE) {
        printf("Senaryo D basarili: EGL_TRUE dondu.\n");
    } else {
        printf("Senaryo D hatali: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SENARYO E - dpy=EGL_NO_DISPLAY.
 * Beklenen: EGL_FALSE ve EGL_BAD_DISPLAY.
 */
void scenario_e_invalid_display(void)
{
    EGLint major = -1;
    EGLint minor = -1;

    if (eglInitialize(EGL_NO_DISPLAY, &major, &minor) == EGL_FALSE) {
        EGLint error = eglGetError();

        if (error == EGL_BAD_DISPLAY) {
            printf("Senaryo E basarili: EGL_BAD_DISPLAY alindi.\n");
        } else {
            printf("Senaryo E farkli hata: %s\n",
                   egl_error_name(error));
        }
    } else {
        printf("Senaryo E beklenmeyen sonuc: EGL_TRUE dondu.\n");
    }
}
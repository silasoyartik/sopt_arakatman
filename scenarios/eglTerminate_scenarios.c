#include <EGL/egl.h>
#include <stdio.h>

/*
 * eglTerminate(pDpyID) icin all-in-one senaryo dosyasi.
 * main, pencere kurulumu, cizim ve uzun cleanup adimlari bilerek
 * cikarilmistir; odak sadece pDpyID davranisidir.
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
 * SENARYO A - Gecerli ve initialize edilmis display.
 * Normal kullanimda display baslatilir, gerekli EGL kaynaklari kullanilmis
 * kabul edilir ve eglTerminate(dpy) cagrisi ile kapatma sonucu kontrol edilir.
 */
void scenario_a_valid_initialized_display(EGLDisplay dpy)
{
    EGLint major = 0;
    EGLint minor = 0;

    if (dpy == EGL_NO_DISPLAY) {
        printf("Senaryo A gecersiz: pDpyID EGL_NO_DISPLAY olamaz.\n");
        return;
    }

    if (eglInitialize(dpy, &major, &minor) == EGL_FALSE) {
        printf("Senaryo A baslatilamadi: eglInitialize hata=%s\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (eglTerminate(dpy) == EGL_TRUE) {
        printf("Senaryo A basarili: eglTerminate EGL_TRUE dondu.\n");
    } else {
        printf("Senaryo A hatali: eglTerminate hata=%s\n",
               egl_error_name(eglGetError()));
    }
}

/*
 * SENARYO B - Gecersiz display / EGL_NO_DISPLAY.
 * Gecerli display alinmadan eglTerminate cagrilir ve negatif durumda
 * EGL_FALSE ile EGL_BAD_DISPLAY hatasi beklenir.
 */
void scenario_b_invalid_display(void)
{
    EGLDisplay dpy = EGL_NO_DISPLAY;

    if (eglTerminate(dpy) == EGL_FALSE) {
        EGLint error = eglGetError();

        if (error == EGL_BAD_DISPLAY) {
            printf("Senaryo B basarili: gecersiz display EGL_BAD_DISPLAY verdi.\n");
        } else {
            printf("Senaryo B farkli hata verdi: %s\n", egl_error_name(error));
        }
    } else {
        printf("Senaryo B beklenmeyen sonuc: eglTerminate EGL_TRUE dondu.\n");
    }
}

/*
 * SENARYO C - Gecerli fakat initialize edilmemis display.
 * Display handle gecerlidir ancak eglInitialize cagrilmaz; dogrudan
 * eglTerminate(dpy) ile bu durumdaki kapatma davranisi kontrol edilir.
 */
void scenario_c_valid_but_uninitialized_display(EGLDisplay dpy)
{
    if (dpy == EGL_NO_DISPLAY) {
        printf("Senaryo C gecersiz: once gecerli bir EGLDisplay alinmalidir.\n");
        return;
    }

    if (eglTerminate(dpy) == EGL_TRUE) {
        printf("Senaryo C basarili: initialize edilmemis display icin EGL_TRUE dondu.\n");
    } else {
        printf("Senaryo C hatali: eglTerminate hata=%s\n",
               egl_error_name(eglGetError()));
    }
}

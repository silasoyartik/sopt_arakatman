#include <EGL/egl.h>

/*
 * eglGetCurrentContext(void)
 *
 * Bu dosya calistirilabilir bir test dosyasi degildir; bu yuzden main()
 * icermez. Amac, eglGetCurrentContext fonksiyonu icin hazirlanan senaryolari
 * tek yerde, sadece davranisi gosteren gerekli kod parcalariyla toplamaktir.
 *
 * On kosul:
 * - EGLDisplay, EGLSurface ve EGLContext daha once basarili sekilde
 *   olusturulmus kabul edilir.
 * - Fonksiyonun sonucu, cagrinin yapildigi thread uzerindeki current context
 *   durumuna baglidir.
 */

/*
 * SENARYO A - Aktif context varken eglGetCurrentContext cagrisi
 *
 * Amac:
 * eglMakeCurrent ile bir context current hale getirildikten sonra
 * eglGetCurrentContext() cagrildiginda, ayni context handle degerinin
 * donduruldugunu gostermek.
 *
 * Beklenen sonuc:
 * - eglMakeCurrent basarili olur.
 * - eglGetCurrentContext() EGL_NO_CONTEXT dondurmez.
 * - Donen deger, current yapilan context ile aynidir.
 */
void scenario_a_current_context_is_returned(EGLDisplay display,
                                            EGLSurface surface,
                                            EGLContext context)
{
    EGLBoolean make_current_result;
    EGLContext current_context;

    make_current_result = eglMakeCurrent(display, surface, surface, context);
    if (make_current_result != EGL_TRUE) {
        /* Senaryo burada basarisizdir: context current yapilamamistir. */
        return;
    }

    current_context = eglGetCurrentContext();

    if (current_context == context) {
        /*
         * Beklenen durum:
         * eglGetCurrentContext(), cagrinin yapildigi thread icin aktif olan
         * context'i dondurdu.
         */
    } else if (current_context == EGL_NO_CONTEXT) {
        /*
         * Beklenmeyen durum:
         * Context current yapilmis olmasina ragmen aktif context yok gorundu.
         */
    } else {
        /*
         * Beklenmeyen durum:
         * Fonksiyon, current yapilan context disinda baska bir handle dondurdu.
         */
    }
}

/*
 * SENARYO B - Aktif context yokken eglGetCurrentContext cagrisi
 *
 * Amac:
 * Thread uzerindeki current context EGL_NO_CONTEXT ile kaldirildiktan sonra
 * eglGetCurrentContext() cagrildiginda EGL_NO_CONTEXT dondugunu gostermek.
 *
 * Beklenen sonuc:
 * - Ilk eglMakeCurrent ile context current hale gelir.
 * - Ikinci eglMakeCurrent, EGL_NO_CONTEXT gecerek context'i thread'den ayirir.
 * - eglGetCurrentContext() EGL_NO_CONTEXT dondurur.
 */
void scenario_b_no_current_context_returns_no_context(EGLDisplay display,
                                                      EGLSurface surface,
                                                      EGLContext context)
{
    EGLBoolean make_current_result;
    EGLBoolean detach_result;
    EGLContext current_context;

    make_current_result = eglMakeCurrent(display, surface, surface, context);
    if (make_current_result != EGL_TRUE) {
        /* Senaryo burada basarisizdir: baslangicta context current yapilamamistir. */
        return;
    }

    detach_result = eglMakeCurrent(display,
                                   EGL_NO_SURFACE,
                                   EGL_NO_SURFACE,
                                   EGL_NO_CONTEXT);
    if (detach_result != EGL_TRUE) {
        /* Senaryo burada basarisizdir: aktif context thread'den ayrilamamistir. */
        return;
    }

    current_context = eglGetCurrentContext();

    if (current_context == EGL_NO_CONTEXT) {
        /*
         * Beklenen durum:
         * Thread uzerinde artik current context yoktur ve fonksiyon bunu
         * EGL_NO_CONTEXT ile bildirir.
         */
    } else {
        /*
         * Beklenmeyen durum:
         * Context detach edilmesine ragmen fonksiyon hala bir context handle
         * degeri dondurdu.
         */
    }
}

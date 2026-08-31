#include <EGL/egl.h>
#include <stdio.h>

/*
 * eglTerminate(pDpyID) - All-in-one senaryo dosyasi
 *
 * Bu dosya calistirilabilir test programi olarak hazirlanmamistir.
 * Bu nedenle main fonksiyonu, DRM/GBM pencere kurulumu, GLES cizim kodlari,
 * sleep, ekran sunumu ve uzun cleanup bloklari bilerek cikarilmistir.
 *
 * Amac:
 *   eglTerminate fonksiyonunun pDpyID parametresi icin beklenen davranisini
 *   tek C dosyasi icinde, okunabilir ve senaryo odakli sekilde gostermek.
 *
 * Incelenen senaryolar:
 *   1. Gecerli ve initialize edilmis EGLDisplay
 *   2. Gecersiz EGLDisplay / EGL_NO_DISPLAY
 *   3. Gecerli fakat initialize edilmemis EGLDisplay
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
 * ============================================================================
 * SENARYO A - pDpyID gecerli ve initialize edilmis display
 * ============================================================================
 *
 * Amac:
 *   eglTerminate fonksiyonunun normal/basarili kullanimini gostermek.
 *
 * On kosul:
 *   - dpy, eglGetDisplay veya platforma ozel bir EGL display alma fonksiyonu
 *     ile alinmis gecerli bir EGLDisplay degeridir.
 *   - eglInitialize(dpy, ...) basarili olmustur.
 *   - Bu display uzerinde context/surface gibi EGL kaynaklari olusturulmus
 *     olabilir.
 *
 * Senaryo akisi:
 *   1. Gecerli display alinir.
 *   2. Display eglInitialize ile baslatilir.
 *   3. Senaryo gerektiriyorsa EGL kaynaklari kullanilir.
 *   4. eglTerminate(dpy) cagrilir.
 *
 * Beklenen sonuc:
 *   - eglTerminate(dpy), EGL_TRUE dondurur.
 *   - Display EGL acisindan uninitialized duruma gecer.
 *   - Display'e bagli EGL kaynaklari silinmek uzere isaretlenir.
 *   - Current olan context/surface varsa, gercek silme islemi current durum
 *     birakilana kadar ertelenebilir.
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

    /*
     * Bu noktada asil testlerde eglChooseConfig, eglCreateWindowSurface,
     * eglCreateContext ve eglMakeCurrent gibi adimlar bulunabilir.
     * Bu all-in-one dosyada bu ayrintilar cikarildi; cunku senaryonun
     * odagi pDpyID parametresi ile eglTerminate sonucudur.
     */

    if (eglTerminate(dpy) == EGL_TRUE) {
        printf("Senaryo A basarili: eglTerminate EGL_TRUE dondu.\n");
    } else {
        printf("Senaryo A hatali: eglTerminate hata=%s\n",
               egl_error_name(eglGetError()));
    }
}

/*
 * ============================================================================
 * SENARYO B - pDpyID gecersiz display / EGL_NO_DISPLAY
 * ============================================================================
 *
 * Amac:
 *   eglTerminate fonksiyonuna gecerli bir EGLDisplay verilmeyen negatif durumu
 *   gostermek.
 *
 * On kosul:
 *   - pDpyID, EGL_NO_DISPLAY veya EGL tarafindan gecerli display olarak kabul
 *     edilmeyen bir degerdir.
 *
 * Senaryo akisi:
 *   1. Gecerli display alinmaz.
 *   2. eglInitialize, config secimi, context/surface olusturma ve cizim
 *      adimlari yapilmaz.
 *   3. Gecersiz display ile eglTerminate davranisi incelenir.
 *
 * Beklenen sonuc:
 *   - eglTerminate, EGL_FALSE donmelidir.
 *   - Beklenen hata EGL_BAD_DISPLAY olmalidir.
 *   - EGL state degismemelidir.
 *
 * Not:
 *   Gercek sistemlerde tamamen uydurma pointer/deger kullanmak tanimsiz veya
 *   riskli olabilir. Bu nedenle senaryoda guvenli negatif ornek olarak
 *   EGL_NO_DISPLAY kullanilir.
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
 * ============================================================================
 * SENARYO C - pDpyID gecerli fakat initialize edilmemis display
 * ============================================================================
 *
 * Amac:
 *   Display handle gecerli olsa bile eglInitialize cagrilmadiginda
 *   eglTerminate davranisini gostermek.
 *
 * On kosul:
 *   - dpy, EGL tarafindan gecerli bir display handle olarak alinmistir.
 *   - Ancak dpy icin eglInitialize cagrilmamistir.
 *   - Bu nedenle display uzerinde context, surface veya config kullanim
 *     adimlari baslatilmamistir.
 *
 * Senaryo akisi:
 *   1. Gecerli display handle alinir.
 *   2. eglInitialize bilerek cagrilmaz.
 *   3. Dogrudan eglTerminate(dpy) cagrilir.
 *
 * Beklenen sonuc:
 *   - eglTerminate(dpy), EGL_TRUE dondurur.
 *   - Serbest bir no-op davranisi beklenir; cunku display ile iliskili aktif
 *     EGL kaynaklari yoktur.
 *   - Render hatti kurulmadigi icin context/surface/cizim adimlari yoktur.
 */
void scenario_c_valid_but_uninitialized_display(EGLDisplay dpy)
{
    if (dpy == EGL_NO_DISPLAY) {
        printf("Senaryo C gecersiz: once gecerli bir EGLDisplay alinmalidir.\n");
        return;
    }

    /*
     * Bu senaryonun kritik noktasi:
     * eglInitialize(dpy, ...) bilerek cagrilmaz.
     */

    if (eglTerminate(dpy) == EGL_TRUE) {
        printf("Senaryo C basarili: initialize edilmemis display icin EGL_TRUE dondu.\n");
    } else {
        printf("Senaryo C hatali: eglTerminate hata=%s\n",
               egl_error_name(eglGetError()));
    }
}

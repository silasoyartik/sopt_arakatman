#include <EGL/egl.h>

/*
 * eglCreateContext - Toplu Senaryo Inceleme Dosyasi
 *
 * Bu dosya calistirilabilir bir test uygulamasi olarak hazirlanmamistir.
 * Amac, eglCreateContext fonksiyonunda degisen parametreleri tek yerde
 * sade sekilde gostermektir.
 *
 * Her senaryoda sadece o senaryoyu anlamak icin gerekli kisimlar tutuldu:
 *  - Degisen parametre
 *  - Ilgili eglCreateContext kullanimi
 *  - Beklenen anlam / sonuc
 */

/* =========================================================================
 * 1) pDpyID SENARYOLARI
 * -------------------------------------------------------------------------
 * pDpyID, eglCreateContext fonksiyonuna verilen EGLDisplay handle'idir.
 *
 * EGLContext eglCreateContext(
 *     EGLDisplay dpy,
 *     EGLConfig config,
 *     EGLContext share_context,
 *     const EGLint *attrib_list
 * );
 *
 * Bu grupta asil fark dpy parametresidir.
 * ========================================================================= */

/*
 * SENARYO A - Ana ekran icin context olusturma
 *
 * Degisen deger:
 *  - dpy = ana_ekran_dpy
 *
 * Anlam:
 *  - Context, ana ekranin EGLDisplay baglantisi uzerinde olusturulur.
 */
void senaryo_pDpyID_A_ana_ekran(EGLDisplay ana_ekran_dpy, EGLConfig ana_ekran_config) {
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext ana_ekran_context = eglCreateContext(
        ana_ekran_dpy,
        ana_ekran_config,
        EGL_NO_CONTEXT,
        context_attribs
    );

    (void)ana_ekran_context;
}

/*
 * SENARYO B - Yedek ekran icin context olusturma
 *
 * Degisen deger:
 *  - dpy = yedek_ekran_dpy
 *
 * Anlam:
 *  - Context, yedek ekranin EGLDisplay baglantisi uzerinde olusturulur.
 *  - A senaryosundan farki context'in baska bir display'e ait olmasidir.
 */
void senaryo_pDpyID_B_yedek_ekran(EGLDisplay yedek_ekran_dpy, EGLConfig yedek_ekran_config) {
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext yedek_ekran_context = eglCreateContext(
        yedek_ekran_dpy,
        yedek_ekran_config,
        EGL_NO_CONTEXT,
        context_attribs
    );

    (void)yedek_ekran_context;
}

/* =========================================================================
 * 2) uConfigID SENARYOLARI
 * -------------------------------------------------------------------------
 * uConfigID, eglCreateContext fonksiyonuna verilen EGLConfig secimini temsil
 * eder. Context'in hangi framebuffer ozellikleriyle uyumlu olacagini belirler.
 *
 * Bu grupta asil fark EGL_DEPTH_SIZE degeridir.
 * ========================================================================= */

/*
 * SENARYO A - Derinlik tamponu olmayan config
 *
 * Degisen deger:
 *  - config = EGL_DEPTH_SIZE 0 olan EGLConfig
 *
 * Anlam:
 *  - Derinlik tamponu yoktur.
 *  - Z sirasi korunmaz; cizim sirasi sonucu belirler.
 */
void senaryo_uConfigID_A_derinlik_yok(EGLDisplay dpy) {
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 0,
        EGL_NONE
    };

    EGLConfig derinlik_yok_config;
    EGLint config_sayisi = 0;

    eglChooseConfig(dpy, config_attribs, &derinlik_yok_config, 1, &config_sayisi);

    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(
        dpy,
        derinlik_yok_config,
        EGL_NO_CONTEXT,
        context_attribs
    );

    (void)context;
}

/*
 * SENARYO B - Derinlik tamponu olan config
 *
 * Degisen deger:
 *  - config = EGL_DEPTH_SIZE 16 olan EGLConfig
 *
 * Anlam:
 *  - Derinlik tamponu vardir.
 *  - Ayni cizim sirasinda bile z testi sonuc goruntuyu degistirir.
 */
void senaryo_uConfigID_B_derinlik_var(EGLDisplay dpy) {
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };

    EGLConfig derinlik_var_config;
    EGLint config_sayisi = 0;

    eglChooseConfig(dpy, config_attribs, &derinlik_var_config, 1, &config_sayisi);

    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(
        dpy,
        derinlik_var_config,
        EGL_NO_CONTEXT,
        context_attribs
    );

    (void)context;
}

/* =========================================================================
 * 3) uShareContext SENARYOLARI
 * -------------------------------------------------------------------------
 * uShareContext, yeni context'in baska bir context ile GL kaynaklarini
 * paylasip paylasmayacagini belirler.
 *
 * Bu grupta asil fark share_context parametresidir.
 * ========================================================================= */

/*
 * SENARYO A - Paylasim yok
 *
 * Degisen deger:
 *  - share_context = EGL_NO_CONTEXT
 *
 * Anlam:
 *  - Yeni context herhangi bir context ile kaynak paylasmaz.
 *  - Baska context'te olusturulan texture gibi kaynaklari dogrudan kullanamaz.
 */
void senaryo_uShareContext_A_paylasim_yok(EGLDisplay dpy, EGLConfig config) {
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext izole_context = eglCreateContext(
        dpy,
        config,
        EGL_NO_CONTEXT,
        context_attribs
    );

    (void)izole_context;
}

/*
 * SENARYO B - Ortak context ile paylasim
 *
 * Degisen deger:
 *  - share_context = ana_context
 *
 * Anlam:
 *  - Yeni context, ana_context ile paylasim grubuna katilir.
 *  - Texture gibi paylasilabilir GL kaynaklari iki context tarafindan da
 *    kullanilabilir.
 */
void senaryo_uShareContext_B_ortak_context(EGLDisplay dpy, EGLConfig config) {
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext ana_context = eglCreateContext(
        dpy,
        config,
        EGL_NO_CONTEXT,
        context_attribs
    );

    EGLContext paylasimli_context = eglCreateContext(
        dpy,
        config,
        ana_context,
        context_attribs
    );

    (void)paylasimli_context;
}

/* =========================================================================
 * 4) pAttribList SENARYOLARI
 * -------------------------------------------------------------------------
 * pAttribList, context olusturulurken verilen attribute listesidir.
 * Liste EGL_NONE ile sonlandirilir.
 *
 * Bu grupta asil fark attrib_list parametresidir.
 * ========================================================================= */

/*
 * SENARYO A - EGL 1.0 standart attribute listesi
 *
 * Degisen deger:
 *  - attrib_list = { EGL_NONE }
 *
 * Anlam:
 *  - Ek context attribute'u istenmez.
 *  - EGL 1.0 standart kullanimini temsil eder.
 */
void senaryo_pAttribList_A_egl10_standart(EGLDisplay dpy, EGLConfig config) {
    EGLint egl10_attribs[] = {
        EGL_NONE
    };

    EGLContext egl10_context = eglCreateContext(
        dpy,
        config,
        EGL_NO_CONTEXT,
        egl10_attribs
    );

    (void)egl10_context;
}

/*
 * SENARYO B - OpenGL ES 2.0 context talebi
 *
 * Degisen deger:
 *  - attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE }
 *
 * Anlam:
 *  - OpenGL ES 2.0 / shader tabanli pipeline icin context talep edilir.
 */
void senaryo_pAttribList_B_modern_pipeline(EGLDisplay dpy, EGLConfig config) {
    EGLint modern_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext modern_context = eglCreateContext(
        dpy,
        config,
        EGL_NO_CONTEXT,
        modern_attribs
    );

    (void)modern_context;
}

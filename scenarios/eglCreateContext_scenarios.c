#include <EGL/egl.h>

/* eglCreateContext icin farkli parametre senaryolarini sade sekilde gosterir. */

/* 1) pDpyID senaryolari: context'in hangi EGLDisplay uzerinde olusacagini gosterir. */

/*
 * SENARYO A - Ana ekran icin context olusturma
 * Ana ekran display'i ve ona ait config ile context olusturulur.
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
 * Yedek ekran display'i kullanilarak ayri bir context olusturulur.
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

/* 2) uConfigID senaryolari: secilen EGLConfig'in context davranisina etkisini gosterir. */

/*
 * SENARYO A - Derinlik tamponu olmayan config
 * EGL_DEPTH_SIZE 0 olan config ile derinlik tamponsuz context olusturulur.
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
 * EGL_DEPTH_SIZE 16 olan config ile derinlik tamponlu context olusturulur.
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

/* 3) uShareContext senaryolari: yeni context'in kaynak paylasimini gosterir. */

/*
 * SENARYO A - Paylasim yok
 * EGL_NO_CONTEXT verilerek izole bir context olusturulur.
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
 * Ana context share_context olarak verilerek kaynak paylasimli context olusturulur.
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

/* 4) pAttribList senaryolari: context attribute listesinin kullanimini gosterir. */

/*
 * SENARYO A - EGL 1.0 standart attribute listesi
 * Sadece EGL_NONE ile biten standart attribute listesi kullanilir.
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
 * EGL_CONTEXT_CLIENT_VERSION 2 verilerek OpenGL ES 2.0 context istenir.
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

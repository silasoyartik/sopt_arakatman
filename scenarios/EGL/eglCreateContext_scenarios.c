#include <EGL/egl.h>

/* Illustrates different eglCreateContext parameter scenarios in a simple way. */

/* 1) pDpyID scenarios: show the EGLDisplay on which the context is created. */

/*
 * SCENARIO A - Create a context for the primary screen
 * A context is created with the primary screen display and its config.
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
 * SCENARIO B - Create a context for the secondary screen
 * A separate context is created using the secondary screen display.
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

/* 2) uConfigID scenarios: show how the selected EGLConfig affects context behavior. */

/*
 * SCENARIO A - Config without a depth buffer
 * A context without a depth buffer is created using a config with EGL_DEPTH_SIZE 0.
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
 * SCENARIO B - Config with a depth buffer
 * A context with a depth buffer is created using a config with EGL_DEPTH_SIZE 16.
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

/* 3) uShareContext scenarios: show resource sharing for the new context. */

/*
 * SCENARIO A - No sharing
 * An isolated context is created by passing EGL_NO_CONTEXT.
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
 * SCENARIO B - Share with another context
 * A resource-sharing context is created by passing the primary context as share_context.
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

/* 4) pAttribList scenarios: show how the context attribute list is used. */

/*
 * SCENARIO A - Standard EGL 1.0 attribute list
 * Uses a standard attribute list containing only the terminating EGL_NONE.
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
 * SCENARIO B - Request an OpenGL ES 2.0 context
 * Requests an OpenGL ES 2.0 context by passing EGL_CONTEXT_CLIENT_VERSION 2.
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

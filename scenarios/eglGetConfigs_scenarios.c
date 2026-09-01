#include <EGL/egl.h>
#include <stdlib.h>

/*
 * eglGetConfigs - All scenarios in one file
 *
 * This file is intended to examine different eglGetConfigs parameter
 * scenarios in one place.
 *
 *   EGLBoolean eglGetConfigs(EGLDisplay dpy,
 *                            EGLConfig *configs,
 *                            EGLint config_size,
 *                            EGLint *num_config);
 *
 * Common assumption:
 *   In valid-display scenarios, "egl_dpy" is assumed to be a valid EGLDisplay
 *   that was previously initialized with eglInitialize.
 */

/* ------------------------------------------------------------------------- */
/* SCENARIO 1: pDpyID - Valid Display                                        */
/* ------------------------------------------------------------------------- */
/*
 * Demonstrates that the config list can be read using a valid, initialized
 * EGLDisplay.
 */
void senaryo_pDpyID_gecerli_display(EGLDisplay egl_dpy)
{
    EGLConfig configs[64];
    EGLint aktarilan_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        egl_dpy,
        configs,
        64,
        &aktarilan_sayi
    );

    (void)basari;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SCENARIO 2: pDpyID - Invalid Display                                      */
/* ------------------------------------------------------------------------- */
/*
 * Passing EGL_NO_DISPLAY represents an invalid display, and the call is
 * expected to fail.
 */
void senaryo_pDpyID_gecersiz_display(void)
{
    EGLDisplay hatali_dpy = EGL_NO_DISPLAY;
    EGLConfig configs[10];
    EGLint aktarilan_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        hatali_dpy,
        configs,
        10,
        &aktarilan_sayi
    );

    (void)basari;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SCENARIO 3: pConfigs - Count Only                                         */
/* ------------------------------------------------------------------------- */
/*
 * Passing NULL for configs and 0 for config_size queries only the total number
 * of configs; no config handle is read.
 */
void senaryo_pConfigs_sadece_sayim(EGLDisplay egl_dpy)
{
    EGLint toplam_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        egl_dpy,
        NULL,
        0,
        &toplam_sayi
    );

    (void)basari;
    (void)toplam_sayi;
}

/* ------------------------------------------------------------------------- */
/* SCENARIO 4: pConfigs - Read Data                                          */
/* ------------------------------------------------------------------------- */
/*
 * A valid EGLConfig array is provided so that config handles are written to it.
 */
void senaryo_pConfigs_veri_okuma(EGLDisplay egl_dpy)
{
    EGLConfig configs[64];
    EGLint aktarilan_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        egl_dpy,
        configs,
        64,
        &aktarilan_sayi
    );

    (void)basari;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SCENARIO 5: pNumConfig - Valid Pointer                                    */
/* ------------------------------------------------------------------------- */
/*
 * Demonstrates that the number of configs read is written to the valid address
 * passed in the num_config parameter.
 */
void senaryo_pNumConfig_gecerli_isaretci(EGLDisplay egl_dpy)
{
    EGLConfig configs[64];
    EGLint aktarilan_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        egl_dpy,
        configs,
        64,
        &aktarilan_sayi
    );

    (void)basari;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SCENARIO 6: pNumConfig - Pass NULL                                        */
/* ------------------------------------------------------------------------- */
/*
 * If num_config is NULL, the request is rejected before eglGetConfigs is
 * called; the output pointer is expected to be valid.
 */
EGLBoolean senaryo_pNumConfig_null_kontrolu(EGLDisplay egl_dpy,
                                            EGLConfig *configs,
                                            EGLint config_size,
                                            EGLint *num_config)
{
    if (num_config == NULL) {
        return EGL_FALSE;
    }

    return eglGetConfigs(egl_dpy, configs, config_size, num_config);
}

/* ------------------------------------------------------------------------- */
/* SCENARIO 7: ConfigSize - Insufficient Capacity                            */
/* ------------------------------------------------------------------------- */
/*
 * If the supplied capacity is smaller than the total config count, only as
 * many configs as the array can hold are read.
 */
void senaryo_ConfigSize_yetersiz_kapasite(EGLDisplay egl_dpy)
{
    EGLint toplam_gercek_sayi = 0;
    EGLConfig configs[2];
    EGLint aktarilan_sayi = 0;

    EGLBoolean sayim_basari = eglGetConfigs(
        egl_dpy,
        NULL,
        0,
        &toplam_gercek_sayi
    );

    EGLBoolean okuma_basari = eglGetConfigs(
        egl_dpy,
        configs,
        2,
        &aktarilan_sayi
    );

    (void)sayim_basari;
    (void)okuma_basari;
    (void)toplam_gercek_sayi;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SCENARIO 8: ConfigSize - Sufficient Capacity                              */
/* ------------------------------------------------------------------------- */
/*
 * First the total config count is queried, then an array with sufficient
 * capacity is allocated to read the complete config list.
 */
void senaryo_ConfigSize_yeterli_kapasite(EGLDisplay egl_dpy)
{
    EGLint toplam_config = 0;

    EGLBoolean sayim_basari = eglGetConfigs(
        egl_dpy,
        NULL,
        0,
        &toplam_config
    );

    if (sayim_basari == EGL_FALSE || toplam_config <= 0) {
        return;
    }

    EGLConfig *configs = (EGLConfig *)malloc((size_t)toplam_config * sizeof(EGLConfig));
    if (configs == NULL) {
        return;
    }

    EGLint aktarilan_sayi = 0;
    EGLBoolean okuma_basari = eglGetConfigs(
        egl_dpy,
        configs,
        toplam_config,
        &aktarilan_sayi
    );

    (void)okuma_basari;
    (void)aktarilan_sayi;

    free(configs);
}

/* ------------------------------------------------------------------------- */
/* SCENARIO 9: Standard Two-Step Query                                       */
/* ------------------------------------------------------------------------- */
/*
 * Demonstrates the standard two-step pattern: first obtain the total config
 * count, then allocate an array of that size and read the configs.
 */
void senaryo_profesyonel_iki_adimli_sorgu(EGLDisplay egl_dpy)
{
    EGLint toplam_config = 0;

    if (eglGetConfigs(egl_dpy, NULL, 0, &toplam_config) == EGL_FALSE) {
        return;
    }

    if (toplam_config <= 0) {
        return;
    }

    EGLConfig *tum_configler =
        (EGLConfig *)malloc((size_t)toplam_config * sizeof(EGLConfig));
    if (tum_configler == NULL) {
        return;
    }

    EGLint aktarilan_sayi = 0;
    EGLBoolean basari = eglGetConfigs(
        egl_dpy,
        tum_configler,
        toplam_config,
        &aktarilan_sayi
    );

    (void)basari;
    (void)aktarilan_sayi;

    free(tum_configler);
}

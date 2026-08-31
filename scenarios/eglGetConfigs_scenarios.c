#include <EGL/egl.h>
#include <stdlib.h>

/*
 * eglGetConfigs - Tum senaryolar tek dosya
 *
 * Bu dosya, eglGetConfigs fonksiyonunun farkli parametre senaryolarini
 * tek yerde incelemek icin hazirlanmistir.
 *
 *   EGLBoolean eglGetConfigs(EGLDisplay dpy,
 *                            EGLConfig *configs,
 *                            EGLint config_size,
 *                            EGLint *num_config);
 *
 * Ortak varsayim:
 *   Gecerli display senaryolarinda "egl_dpy" daha once eglInitialize ile
 *   baslatilmis gecerli bir EGLDisplay olarak kabul edilir.
 */

/* ------------------------------------------------------------------------- */
/* SENARYO 1: pDpyID - Gecerli Display                                       */
/* ------------------------------------------------------------------------- */
/*
 * Initialize edilmis gecerli bir EGLDisplay ile config listesinin
 * okunabildigi durum gosterilir.
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
/* SENARYO 2: pDpyID - Gecersiz Display                                      */
/* ------------------------------------------------------------------------- */
/*
 * EGL_NO_DISPLAY verilerek gecersiz display durumunda cagrinin
 * basarisiz olmasi beklenir.
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
/* SENARYO 3: pConfigs - Sadece Sayim Yapma                                  */
/* ------------------------------------------------------------------------- */
/*
 * configs NULL ve config_size 0 verilerek sadece toplam config sayisi
 * sorgulanir; config handle'i okunmaz.
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
/* SENARYO 4: pConfigs - Veri Okuma                                          */
/* ------------------------------------------------------------------------- */
/*
 * Gecerli bir EGLConfig dizisi verilerek config handle'larinin diziye
 * yazilmasi saglanir.
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
/* SENARYO 5: pNumConfig - Gecerli Isaretci                                  */
/* ------------------------------------------------------------------------- */
/*
 * num_config parametresine gecerli bir adres verilerek okunan config
 * sayisinin bu adrese yazilmasi gosterilir.
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
/* SENARYO 6: pNumConfig - NULL Verilmesi                                    */
/* ------------------------------------------------------------------------- */
/*
 * num_config NULL ise eglGetConfigs cagrisi yapilmadan once durum
 * reddedilir; output pointer'in gecerli olmasi beklenir.
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
/* SENARYO 7: ConfigSize - Yetersiz Kapasite                                 */
/* ------------------------------------------------------------------------- */
/*
 * Toplam config sayisindan kucuk kapasite verilirse sadece dizinin
 * alabildigi kadar config okunur.
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
/* SENARYO 8: ConfigSize - Yeterli Kapasite                                  */
/* ------------------------------------------------------------------------- */
/*
 * Once toplam config sayisi sorgulanir, sonra yeterli kapasitede dizi
 * ayrilarak tum config listesinin okunmasi hedeflenir.
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
/* SENARYO 9: Profesyonel 2 Adimli Sorgu                                     */
/* ------------------------------------------------------------------------- */
/*
 * Iki adimli genel kullanim gosterilir: once toplam config sayisi
 * ogrenilir, sonra o sayi kadar dizi ayrilip configler okunur.
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

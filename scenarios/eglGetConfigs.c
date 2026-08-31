#include <EGL/egl.h>
#include <stdlib.h>

/*
 * eglGetConfigs - Tum senaryolar tek dosya
 *
 * Bu dosya calistirilabilir ornek olmak icin degil, senaryolari tek yerde
 * incelemek icin hazirlanmistir. Bu nedenle main(), native display kurulumu,
 * pencere/surface/context olusturma ve OpenGL cizim kodlari bilerek
 * cikarilmistir.
 *
 * Odak nokta sadece eglGetConfigs fonksiyonuna verilen parametrelerdir:
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
 * Amac:
 *   eglGetConfigs fonksiyonuna gecerli bir EGLDisplay verildiginde config
 *   listesinin okunabildigini gostermek.
 *
 * Kritik nokta:
 *   dpy parametresi EGL_NO_DISPLAY degildir ve initialize edilmis display'i
 *   temsil eder.
 *
 * Beklenen sonuc:
 *   Cagri EGL_TRUE doner. num_config icine diziye aktarilan config sayisi
 *   yazilir.
 */
void senaryo_pDpyID_gecerli_display(EGLDisplay egl_dpy)
{
    EGLConfig configs[64];
    EGLint aktarilan_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        egl_dpy,          /* Gecerli ve initialize edilmis EGLDisplay */
        configs,          /* Config handle'larinin yazilacagi dizi */
        64,               /* Dizinin alabilecegi maksimum config sayisi */
        &aktarilan_sayi   /* EGL'in yazdigi config sayisi */
    );

    /*
     * basari == EGL_TRUE ise aktarilan_sayi kadar EGLConfig okunmustur.
     * Bu configler daha sonra uygun surface/context secimi icin incelenebilir.
     */
    (void)basari;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SENARYO 2: pDpyID - Gecersiz Display                                      */
/* ------------------------------------------------------------------------- */
/*
 * Amac:
 *   Gecersiz display verildiginde eglGetConfigs'in basarisiz olmasi
 *   gerektigini gostermek.
 *
 * Kritik nokta:
 *   dpy parametresi EGL_NO_DISPLAY olarak verilir.
 *
 * Beklenen sonuc:
 *   Cagri EGL_FALSE doner. Hata ayrintisi eglGetError ile okunabilir.
 */
void senaryo_pDpyID_gecersiz_display(void)
{
    EGLDisplay hatali_dpy = EGL_NO_DISPLAY;
    EGLConfig configs[10];
    EGLint aktarilan_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        hatali_dpy,       /* Gecersiz display */
        configs,
        10,
        &aktarilan_sayi
    );

    /*
     * Beklenen durum:
     *   basari == EGL_FALSE
     *   eglGetError() EGL_BAD_DISPLAY benzeri bir hata kodu dondurur.
     */
    (void)basari;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SENARYO 3: pConfigs - Sadece Sayim Yapma                                  */
/* ------------------------------------------------------------------------- */
/*
 * Amac:
 *   Config handle'larini almadan sadece toplam config sayisini ogrenmek.
 *
 * Kritik nokta:
 *   configs = NULL
 *   config_size = 0
 *   num_config = gecerli isaretci
 *
 * Beklenen sonuc:
 *   Cagri EGL_TRUE doner. toplam_sayi icine sistemdeki toplam EGLConfig
 *   sayisi yazilir. Bu senaryoda config handle'i elde edilmez.
 */
void senaryo_pConfigs_sadece_sayim(EGLDisplay egl_dpy)
{
    EGLint toplam_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        egl_dpy,
        NULL,            /* Config dizisi istenmiyor */
        0,               /* Dizi olmadigi icin kapasite 0 */
        &toplam_sayi     /* Toplam config sayisi buraya yazilir */
    );

    /*
     * basari == EGL_TRUE ise toplam_sayi, ikinci adimda ayrilacak dizi
     * boyutunu belirlemek icin kullanilabilir.
     */
    (void)basari;
    (void)toplam_sayi;
}

/* ------------------------------------------------------------------------- */
/* SENARYO 4: pConfigs - Veri Okuma                                          */
/* ------------------------------------------------------------------------- */
/*
 * Amac:
 *   Gecerli bir EGLConfig dizisine config handle'larini kopyalatmak.
 *
 * Kritik nokta:
 *   configs NULL degildir.
 *   config_size dizinin kapasitesini bildirir.
 *
 * Beklenen sonuc:
 *   Cagri EGL_TRUE doner. aktarilan_sayi, configs dizisine kac adet config
 *   yazildigini bildirir.
 */
void senaryo_pConfigs_veri_okuma(EGLDisplay egl_dpy)
{
    EGLConfig configs[64];
    EGLint aktarilan_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        egl_dpy,
        configs,          /* EGLConfig handle'lari bu diziye yazilir */
        64,               /* En fazla 64 config kopyalanabilir */
        &aktarilan_sayi
    );

    /*
     * basari == EGL_TRUE ise configs[0] ... configs[aktarilan_sayi - 1]
     * araligi gecerlidir.
     */
    (void)basari;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SENARYO 5: pNumConfig - Gecerli Isaretci                                  */
/* ------------------------------------------------------------------------- */
/*
 * Amac:
 *   num_config parametresine gecerli bir adres verildiginde EGL'in sonuc
 *   sayisini bu adrese yazdigini gostermek.
 *
 * Kritik nokta:
 *   num_config = &aktarilan_sayi
 *
 * Beklenen sonuc:
 *   Cagri basarili olursa aktarilan_sayi EGL tarafindan doldurulur.
 */
void senaryo_pNumConfig_gecerli_isaretci(EGLDisplay egl_dpy)
{
    EGLConfig configs[64];
    EGLint aktarilan_sayi = 0;

    EGLBoolean basari = eglGetConfigs(
        egl_dpy,
        configs,
        64,
        &aktarilan_sayi   /* Gecerli output isaretcisi */
    );

    /*
     * basari == EGL_TRUE ise aktarilan_sayi, configs dizisine yazilan eleman
     * sayisini gosterir.
     */
    (void)basari;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SENARYO 6: pNumConfig - NULL Verilmesi                                    */
/* ------------------------------------------------------------------------- */
/*
 * Amac:
 *   EGL 1.0 icin num_config parametresinin NULL verilmemesi gerektigini
 *   gostermek.
 *
 * Kritik nokta:
 *   num_config EGL'in sonuc yazacagi output parametresidir. NULL verilirse
 *   surucuye gore EGL_FALSE yerine process crash gibi guvensiz davranislar
 *   gorulebilir.
 *
 * Beklenen sonuc:
 *   Uretim kodu bu durumu eglGetConfigs cagrisindan once reddetmelidir.
 */
EGLBoolean senaryo_pNumConfig_null_kontrolu(EGLDisplay egl_dpy,
                                            EGLConfig *configs,
                                            EGLint config_size,
                                            EGLint *num_config)
{
    if (num_config == NULL) {
        /*
         * Guvenli davranis:
         *   Gecersiz output pointer'i EGL'e gonderilmez.
         */
        return EGL_FALSE;
    }

    return eglGetConfigs(egl_dpy, configs, config_size, num_config);
}

/* ------------------------------------------------------------------------- */
/* SENARYO 7: ConfigSize - Yetersiz Kapasite                                 */
/* ------------------------------------------------------------------------- */
/*
 * Amac:
 *   config_size degeri toplam config sayisindan kucuk oldugunda sadece
 *   sinirli sayida config okunacagini gostermek.
 *
 * Kritik nokta:
 *   Once toplam config sayisi ogrenilir.
 *   Sonra bilerek kucuk kapasiteli bir dizi ile okuma yapilir.
 *
 * Beklenen sonuc:
 *   Cagri basarili olabilir; ancak configs dizisine sadece config_size kadar
 *   config kopyalanabilir. Bu nedenle tum config havuzu incelenmis sayilmaz.
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
        2,                /* Bilerek yetersiz kapasite */
        &aktarilan_sayi
    );

    /*
     * toplam_gercek_sayi daha buyuk olsa bile aktarilan_sayi en fazla 2 olur.
     * Bu senaryo, kucuk config_size ile dogru config seciminin garanti
     * edilemeyecegini gosterir.
     */
    (void)sayim_basari;
    (void)okuma_basari;
    (void)toplam_gercek_sayi;
    (void)aktarilan_sayi;
}

/* ------------------------------------------------------------------------- */
/* SENARYO 8: ConfigSize - Yeterli Kapasite                                  */
/* ------------------------------------------------------------------------- */
/*
 * Amac:
 *   config_size toplam config sayisini karsiladiginda tum config listesinin
 *   okunabildigini gostermek.
 *
 * Kritik nokta:
 *   Once toplam config sayisi sorgulanir, sonra o sayi kadar EGLConfig dizisi
 *   ayrilir.
 *
 * Beklenen sonuc:
 *   Cagri EGL_TRUE doner ve tum configler ayrilan diziye kopyalanir.
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
        toplam_config,    /* Toplam sayiyi karsilayan kapasite */
        &aktarilan_sayi
    );

    /*
     * okuma_basari == EGL_TRUE ise aktarilan_sayi kadar config okunmustur.
     * Kapasite toplam_config oldugu icin config listesinin tamami alinabilir.
     */
    (void)okuma_basari;
    (void)aktarilan_sayi;

    free(configs);
}

/* ------------------------------------------------------------------------- */
/* SENARYO 9: Profesyonel 2 Adimli Sorgu                                     */
/* ------------------------------------------------------------------------- */
/*
 * Amac:
 *   eglGetConfigs icin en temiz ve genel kullanimi gostermek.
 *
 * Adim 1:
 *   configs = NULL, config_size = 0 ile toplam config sayisi ogrenilir.
 *
 * Adim 2:
 *   Ogrenilen sayi kadar dizi ayrilir ve tum configler okunur.
 *
 * Beklenen sonuc:
 *   Uygulama once gerekli kapasiteyi bilir, sonra eksiksiz config listesini
 *   alir. Bu yontem yetersiz kapasite riskini ortadan kaldirir.
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

    /*
     * basari == EGL_TRUE ise tum_configler dizisi artik secim/filtreleme icin
     * kullanilabilir. Ornegin EGL_SURFACE_TYPE, EGL_RENDERABLE_TYPE veya
     * EGL_DEPTH_SIZE gibi attribute'lar bu configler uzerinden sorgulanabilir.
     */
    (void)basari;
    (void)aktarilan_sayi;

    free(tum_configler);
}

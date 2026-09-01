# EGL 1.0 Fonksiyon Incelemesi: `eglTerminate`

```c
EGLBoolean eglTerminate(EGLDisplay dpy);
```

`eglTerminate`, bir `EGLDisplay` uzerinden kurulmus EGL oturumunu sonlandirmak icin kullanilir. Basarili cagridan sonra display yeniden **uninitialized** duruma doner. Display'e bagli EGL kaynaklari, ornegin context ve surface nesneleri, EGL tarafindan birakilir; o anda current olan kaynaklar ise guvenli sekilde artik kullanilmamasi gereken kaynaklar olarak ele alinir.

Bu incelemede fonksiyonun tek parametresi olan `dpy` / `pDpyID` uc farkli durum uzerinden ele alinmistir:

| Senaryo | `pDpyID` durumu | Test dosyasi | Beklenen sonuc |
| :--- | :--- | :--- | :--- |
| A | Gecerli ve initialize edilmis display | `pDpyID/ScenarioA_ValidDisplay.c` | Render hatti kurulur, ucgen cizilir, `eglTerminate(display)` `EGL_TRUE` dondurur. |
| B | `EGL_NO_DISPLAY` / gecersiz display | `pDpyID/ScenarioB_InvalidDisplay.c` | Gecerli display olmadigi icin EGL/GBM/DRM hatti baslatilmaz; gercek cagrida beklenen hata `EGL_BAD_DISPLAY` olur. |
| C | Gecerli fakat initialize edilmemis display | `pDpyID/ScenarioC_UninitializedDisplay.c` | Render hatti kurulmaz; gecerli display uzerinde `eglTerminate(display)` guvenli bicimde `EGL_TRUE` dondurur. |

## Kisa Ozet

![eglTerminate yasam dongusu](image/eglTerminate/eglterminate-state-flow.svg)

`eglTerminate(dpy)` yalnizca EGL tarafindaki oturumu kapatir. DRM fd, GBM device, GBM surface veya native pencere sistemi kaynaklari ayrica temizlenmelidir. Bu nedenle orneklerde `eglTerminate` cagrisindan sonra veya hata yollarinda `destroy_drm_window(&nw)` ile native kaynak temizligi yapilir.

Fonksiyonun anlasilmasi icin en onemli ayrim sudur:

- `dpy` gecerli ve initialize edilmisse EGL kaynaklari kapatilir.
- `dpy` gecerli ama initialize edilmemisse cagri hata sayilmaz; display zaten baslatilmamis durumdadir.
- `dpy` gecersizse fonksiyon basarili kabul edilmez ve `EGL_BAD_DISPLAY` beklenir.

## Senaryo A: Gecerli ve Initialize Edilmis Display

![Senaryo A gecerli display akisi](image/eglTerminate/scenario-a-valid-display.svg)

Bu senaryoda program once DRM/KMS ve GBM altyapisini kurar. Ardindan GBM device uzerinden gecerli bir `EGLDisplay` alir ve `eglInitialize` ile EGL oturumunu baslatir. Config secimi, window surface olusturma ve GLES2 context olusturma adimlari basarili olursa context current yapilir. Son olarak renkli ucgen cizilir, goruntu `drm_swap_buffers` ile ekrana sunulur ve `eglTerminate(display)` cagrilir.

**Kodun gosterdigi nokta:** Gecerli ve initialize edilmis `pDpyID`, `eglTerminate` icin dogru kullanim durumudur. Fonksiyon basarili oldugunda `EGL_TRUE` doner ve display EGL acisindan tekrar uninitialized hale gelir.

**Beklenen terminal ciktisi ozeti:**

```text
Senaryo A: eglTerminate(pDpyID) Gecerli (Valid) Parametre Kullanimi
DRM/KMS ve GBM cihazi basariyla olusturuldu
EGL GBM display ... ile alindi
-> Gecerli bir pDpyID kullanildigi icin EGL Context basariyla olusturuldu.
-> Ekrana renkli bir ucgen ciziliyor...

Simdi eglTerminate(display) cagriliyor...
-> eglTerminate BASARILI (EGL_TRUE dondu).
-> Gorsel Kanit: Ucgen basariyla cizildi ve ardindan EGL baglantisi temiz bir sekilde sonlandirildi.
```

**Gorsel yorum:** Semadaki ekran alani, programin basarili calismasinda olusan somut kaniti temsil eder: GLES2 ile cizilen kirmizi/yesil/mavi koseli ucgen DRM/KMS uzerinden fiziksel ekrana basilir. Bu nedenle Senaryo A yalnizca terminal ciktisiyla degil, gercek render sonucuyla da dogrulanabilir.

## Senaryo B: `EGL_NO_DISPLAY` / Gecersiz Display

![Senaryo B gecersiz display akisi](image/eglTerminate/scenario-b-invalid-display.svg)

Bu negatif senaryoda `pDpyID` olarak `EGL_NO_DISPLAY` veya NULL benzeri gecersiz bir display degeri ele alinir. Kod guvenlik ve sistem kararliligi icin gercek `eglTerminate(EGL_NO_DISPLAY)` cagrisi yapmaz; bunun yerine bu parametre sinifinin neden hatali oldugunu acikca gosterir.

**Kodun gosterdigi nokta:** Gecerli bir display yoksa EGL oturumu da yoktur. Bu durumda context, surface, cizim veya DRM present adimlari denenmemelidir. EGL spesifikasyonu acisindan gecersiz display ile yapilan cagrilarda beklenen hata sinifi `EGL_BAD_DISPLAY`'dir.

**Beklenen terminal ciktisi ozeti:**

```text
Senaryo B: eglTerminate(pDpyID) Hatali (EGL_NO_DISPLAY) Parametre Kullanimi
Bu negatif senaryoda pDpyID olarak EGL_NO_DISPLAY/NULL benzeri gecersiz bir deger kullanimi anlatilir.
Guvenlik nedeniyle gercek eglTerminate(EGL_NO_DISPLAY), EGL/GBM veya DRM cagrisi yapilmiyor.
Beklenen sonuc: EGL implementasyonu boyle bir display'i gecerli kabul etmemeli ve EGL_BAD_DISPLAY raporlamalidir.
SONUC: Gecerli display olmadigi icin context/surface olusturulmaz, cizim ve DRM present denenmez.
```

**Gorsel yorum:** Bu senaryoda ekranda ucgen veya yeni frame beklenmez. En guclu kanit, render hattinin hic baslatilmamasi ve terminal ciktisinda gecersiz display durumunun acikca raporlanmasidir.

## Senaryo C: Gecerli Fakat Initialize Edilmemis Display

![Senaryo C initialize edilmemis display akisi](image/eglTerminate/scenario-c-uninitialized-display.svg)

Bu senaryoda DRM/KMS ve GBM altyapisi kurulur, ardindan gecerli bir `EGLDisplay` alinir. Ancak senaryo geregi `eglInitialize` cagrisi yapilmaz. Program dogrudan `eglTerminate(display)` cagirir.

EGL 1.0 davranisina gore display handle gecerli oldugu surece, display daha once initialize edilmemis olsa bile `eglTerminate` cagrisi hata uretmeden tamamlanabilir. Cunku EGL tarafinda kapatilacak aktif bir oturum veya render kaynagi yoktur.

**Kodun gosterdigi nokta:** `pDpyID` gecerlidir, fakat display initialize edilmedigi icin `eglGetConfigs`, `eglChooseConfig`, context olusturma, surface olusturma, cizim ve DRM present adimlari calistirilmaz. Buna ragmen `eglTerminate(display)` `EGL_TRUE` dondurur.

**Beklenen terminal ciktisi ozeti:**

```text
Senaryo C: eglTerminate(pDpyID) Initialize Edilmemis Parametre Kullanimi
DRM/KMS ve GBM cihazi basariyla olusturuldu
EGL GBM display ... ile alindi
Display gecerli alindi fakat INITIALIZE EDILMEDI.
Simdi eglTerminate(display) cagriliyor...

-> eglTerminate BASARILI (EGL_TRUE dondu).
-> Not: EGL spesifikasyonuna gore baslatilmamis display uzerinde terminate cagirmak serbesttir ve etkisi yoktur.

Bu display eglInitialize ile baslatilmadigi icin eglGetConfigs/eglChooseConfig, context, surface, cizim ve DRM present denenmeyecek.
SONUC: EGL Context olusturulamaz ve ekrana yeni bir gorsel basilmamalidir.
```

**Gorsel yorum:** Senaryo C'de display handle gecerli olsa da EGL state machine baslatilmadigi icin cizim hatti olusmaz. Bu nedenle beklenen gorsel sonuc "yeni goruntu yok" durumudur; dogrulama terminaldeki `EGL_TRUE` sonucu ve render adimlarinin atlanmasi uzerinden yapilir.

## Hata ve Durum Matrisi

| Durum | `eglTerminate` sonucu | Beklenen EGL hata durumu | Yan etki |
| :--- | :--- | :--- | :--- |
| `dpy` gecerli ve initialized | `EGL_TRUE` | `EGL_SUCCESS` | EGL oturumu sonlanir, display uninitialized olur. |
| `dpy` gecerli fakat uninitialized | `EGL_TRUE` | `EGL_SUCCESS` | Aktif EGL kaynagi olmadigi icin guvenli no-op davranisi gorulur. |
| `dpy == EGL_NO_DISPLAY` veya gecersiz | `EGL_FALSE` | `EGL_BAD_DISPLAY` | EGL state degismez; render hatti kurulmaz. |

> [!WARNING]
> `eglTerminate` sonrasinda ayni display artik initialized kabul edilmez. Bu display uzerinde yeniden normal EGL islemleri yapilacaksa once tekrar `eglInitialize` cagrilmalidir. Aksi halde `eglChooseConfig`, `eglCreateContext` veya `eglCreateWindowSurface` gibi cagrilar basarisiz olabilir.

## Guvenli Kullanim Kalibi

```c
#include <EGL/egl.h>
#include <stdio.h>

void safe_egl_cleanup(EGLDisplay dpy) {
    if (dpy == EGL_NO_DISPLAY) {
        printf("HATA: Gecersiz EGLDisplay (EGL_NO_DISPLAY).\n");
        return;
    }

    if (eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE) {
        printf("Uyari: current context birakilamadi. Hata: 0x%04X\n", eglGetError());
    }

    if (eglTerminate(dpy) == EGL_FALSE) {
        printf("HATA: eglTerminate basarisiz oldu. Hata: 0x%04X\n", eglGetError());
        return;
    }

    printf("BASARILI: EGL oturumu kapatildi.\n");
}
```

Bu kalipta once gecersiz display kontrol edilir. Ardindan varsa current context/surface baglantisi birakilir ve `eglTerminate` cagrilir. Native kaynaklarin temizligi bu fonksiyonun disinda ayrica yapilmalidir.

## Sonuc

`eglTerminate`, EGL yasam dongusunun kapanis fonksiyonudur. Senaryo A dogru ve tam kullanim yolunu gosterir: display alinir, initialize edilir, context/surface kurulur, cizim yapilir ve EGL oturumu kapatilir. Senaryo B hatali parametre yolunu gosterir: `EGL_NO_DISPLAY` gecerli bir EGL oturumu degildir. Senaryo C ise onemli bir sinir durumunu aciklar: display gecerli ama initialize edilmemisse render uretilemez, ancak `eglTerminate` guvenli bicimde basarili donebilir.

Bu uc senaryo birlikte degerlendirildiginde `pDpyID` parametresi icin temel kural nettir: `eglTerminate` cagrisi yalnizca gecerli bir `EGLDisplay` ile anlamlidir; initialize durumu fonksiyonun yapacagi isi degistirir, fakat gecersiz display her zaman hata sinifina girer.

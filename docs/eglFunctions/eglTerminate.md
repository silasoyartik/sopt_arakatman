# EGL 1.0: `eglTerminate`

```c
EGLBoolean eglTerminate(EGLDisplay dpy);
```

`eglTerminate`, belirtilen EGL görüntü (display) bağlantısı ile ilişkili tüm EGL kaynaklarını (context, surface vb.) serbest bırakarak donanım/pencere sistemi ile olan EGL oturumunu sonlandırır. EGL API kullanım döngüsünün en son adımıdır ve başlatılmış bir state machine'i bellekten tamamen temizleyerek "uninitialized" (başlatılmamış) durumuna geri döndürür.

## Kavramsal Akış

```text
 İşletim Sistemi (OS) & Native Katman               EGL Soyutlama Katmanı (EGL 1.0)
 ====================================               ===============================
                                                
 [Native Display] (X11 Display / DRM fd) <---------- EGLDisplay (Initialized Durumda)
       |                                                   |
       |                                                   |-- EGLContext (Not Current) ---> [Anında Yok Edilir]
       |                                                   |
       +-- [Native Window / GBM Surface] <---------------- |-- EGLSurface (Not Current) ---> [Anında Yok Edilir]
       |                                                   |
       |                                                   |-- EGLContext (Current) -------> [Pending Destruction (Bekleyen Yıkım)]*
       |                                                   |
       +-- [Native Pixmap / GBM BO] <--------------------- |-- EGLSurface (Current) -------> [Pending Destruction (Bekleyen Yıkım)]*
   
* Current olan nesneler, ilgili thread `eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)` çağrısı yapana kadar veya thread tamamen sonlanana kadar bellekten tam olarak silinmez.

[ eglTerminate(dpy) Çağrılır ] 

EGLDisplay ---> [Uninitialized Duruma Döner]
```

## Parametreler

### `dpy`

| `dpy` değeri                                    | Sonuç                                                                                                                                                     |
| :------------------------------------------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Geçerli ve başlatılmış display**      | Display'e bağlı kaynaklar temizlenir veya yok edilmek üzere işaretlenir. Display başlatılmamış duruma geçer ve fonksiyon`EGL_TRUE` döndürür. |
| **Geçerli fakat başlatılmamış display** | EGL 1.0 kurallarına göre işlem güvenlidir; fonksiyon hata üretmeden`EGL_TRUE` döndürür.                                                          |
| **`EGL_NO_DISPLAY` veya geçersiz handle** | Bağlantı reddedilir; fonksiyon`EGL_FALSE` döndürür ve hata durumu güncellenir.                                                                     |

**Native Tip Kısıtlamaları ve Platform Karşılıkları:**
`EGLDisplay`, EGL'nin native donanım ile kurduğu bağlantıyı temsil eder.

- **DRM/GBM Sistemlerinde (Projemizdeki Altyapı):** Bu parametre doğrudan Linux Kernel DRM file descriptor'unu (`/dev/dri/card0`) ve GBM aygıtını (`gbm_device`) sarmalayan soyut yapıdır. `eglTerminate` çağrıldıktan sonra EGL katmanı kapanır, ancak donanımsal DRM fd ve GBM objeleri EGL'nin sorumluluğunda olmadığından, sonrasında native C fonksiyonları (`gbm_device_destroy`, `close`) ile manuel olarak kapatılmalıdır.
- **X11 / Wayland:** Bu pencere sistemlerinde display pointer'ını temsil eder. `eglTerminate`, `XOpenDisplay` ile açılmış pencere sistemini kapatmaz, sadece EGL wrapper'ını siler.

## Geçerli Attribute Listesi

`eglTerminate` fonksiyonu herhangi bir attribute (konfigürasyon) listesi veya flag dizisi **almaz**. Sadece bir adet `EGLDisplay` parametresi ile çalışır.

**Yüzey Tiplerine Göre Davranış Farkı:**
Arka tamponlu (back-buffered) pencere yüzeyleri veya tek tamponlu (single-buffered) pixmap/pbuffer yüzeyleri fark etmeksizin; `eglTerminate` çağrıldığında bu yüzeyler herhangi bir thread'de **aktif (current) değilse anında silinirler**. Ancak bir thread üzerinde o an render işlemi gerçekleştiriliyorsa ("current" ise), yüzey tipi gözetilmeksizin "pending destruction" sürecine girerler. Çift tamponlu sistemlerde page-flip mekanizması yarıda kesilmez, takas işleminin güvenle bitmesi beklenir.

## Ayrıntılar ve Yaşam Döngüsü

**Thread Güvenliği (Thread Safety) ve Pending Destruction:**
EGL 1.0 spesifikasyonuna göre `eglTerminate` thread-safe bir fonksiyondur. Bir `EGLContext` aynı anda yalnızca bir thread'de current olabilir. Başka bir thread üzerinde current olan context ve ilişkili surface'ler, `eglTerminate` çağrısıyla hemen yok edilmez.
Bunun yerine nesneler **"Pending Destruction" (Bekleyen Yıkım)** statüsüne alınır. İlgili thread `eglMakeCurrent` ile context bağlantısını koparana kadar render edilebilir durumda kalırlar, fakat o display için yeni kaynak (yeni bir surface veya context) oluşturulmasına izin verilmez.

**Eşzamanlama (Synchronization):**
`eglTerminate` çağrılmadan önce bekleyen tüm render işlemlerinin bitmiş olduğundan emin olmak, donanım asenkronizasyonundan kaynaklı race condition veya native obje hatası almamak için şarttır. Bu eşzamanlamayı (synchronization) sağlamak için:

1. **`eglWaitGL()`**: Eğer OpenGL(ES) kullanılıyorsa, kuyruktaki komutların grafik donanımında tamamen yürütülmesini bekler (bir nevi `glFinish` muadilidir).
2. **`eglWaitNative()`**: Eğer 2D native grafik API'leri ile aynı anda EGL üzerinden yüzey çizimi yapıldıysa, native sistemin (örneğin X11 veya DRM'nin) işlemlerini bitirmesi için beklenir.

## Hata Matrisi

EGL 1.0 spesifikasyonunun "fonksiyon başarısız olduğunda hiçbir yan etki (side effect) bırakmamalıdır" kuralı gereği, hatalı çağrılarda state machine'de hiçbir değişiklik olmaz, kaynaklar varlığını sürdürür.

| Durum                                                                          | Sonuç (EGL Hata Kodu)               | Yan Etkiler                                                                                   |
| :----------------------------------------------------------------------------- | :----------------------------------- | :-------------------------------------------------------------------------------------------- |
| `dpy` geçerli bir görüntü (display) değilse veya `EGL_NO_DISPLAY` ise | `EGL_BAD_DISPLAY`                  | Hiçbir kaynak silinmez, display'in state machine'i değişmez. Hata bayrağı set edilir.    |
| `dpy` zaten sonlandırılmış (uninitialized) ise                           | **Hata Yok (`EGL_SUCCESS`)** | EGL 1.0'da uninitialized bir display'i terminate etmek geçerli kabul edilir ve hata dönmez. |

> [!WARNING]
> `EGLDisplay` başlatılmamış duruma geçtikten sonra, aynı display üzerinde `eglInitialize`, `eglMakeCurrent` ve `eglTerminate` dışındaki EGL fonksiyonlarının kullanılması `EGL_NOT_INITIALIZED` hata durumuna neden olur.

## Güvenli Kullanım Örneği

Aşağıdaki örnek, current bağlantıların kaldırılmasını, EGL display'in sonlandırılmasını ve hata kontrolünü gösterir:

```c
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <stdio.h>
#include <stdlib.h>

void safe_egl_cleanup(EGLDisplay dpy) {
    if (dpy == EGL_NO_DISPLAY) {
        printf("HATA: Geçersiz EGL Display (EGL_NO_DISPLAY).\n");
        return;
    }

    // 1. Eşzamanlama (Synchronization) - Donanım kuyruğunun bitmesini bekle
    // OpenGL komutlarının tamamlandığından donanım seviyesinde emin olunur.
    eglWaitGL();
  
    // Opsiyonel: Native rendering işlemlerinin EGL'yi beklemesini sağla
    // eglWaitNative(EGL_CORE_NATIVE_ENGINE);

    // 2. Mevcut thread üzerindeki tüm context ve surface bağlarını çöz
    // Bu sayede eglTerminate nesneleri "pending destruction" yerine anında siler.
    if (eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE) {
        printf("Uyarı: Context bağlantıları çözülemedi! Hata Kodu: 0x%04X\n", eglGetError());
    }

    // 3. EGLDisplay bağlantısını sonlandır (eglTerminate)
    EGLBoolean result = eglTerminate(dpy);
  
    if (result == EGL_FALSE) {
        EGLint err = eglGetError();
        printf("KRİTİK HATA: eglTerminate başarisiz oldu.\n");
        if (err == EGL_BAD_DISPLAY) {
            printf("Sebep: EGL_BAD_DISPLAY (Geçersiz veya bozuk EGLDisplay parametresi)\n");
        }
    } else {
        printf("BAŞARILI: EGL kaynakları ve oturum sorunsuz bir şekilde kapatıldı.\n");
    }

    // 4. Native kaynakların manuel olarak temizlenmesi
    // EGLTerminate'den sonra GBM/DRM nesneleri serbest bırakılır.
    // gbm_device_destroy(gbm_dev);
    // close(drm_fd);
}

int main(void) {
    // EGL bağlantısını başlat
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  
    if (dpy != EGL_NO_DISPLAY) {
        if (eglInitialize(dpy, NULL, NULL) == EGL_TRUE) {
            // ... Yüzey oluşturma, Context Atama, Render döngüsü ...
        
            // Program kapatılırken veya EGL'ye ihtiyaç bittiğinde:
            safe_egl_cleanup(dpy);
        }
    }
    return 0;
}
```

## Bölüm Özeti

- **Oturumun Sonlandırılması:** `eglTerminate`, `eglInitialize` ile başlatılan EGL display bağlantısını sonlandırır ve display'i başlatılmamış duruma döndürür.
- **Bekleyen Yok Etme:** Başka bir thread'de current olan nesneler hemen silinmez; ilgili bağlantı kaldırılana kadar yaşamaya devam eder. Bu nedenle `eglTerminate` öncesinde current bağlantıların `eglMakeCurrent` ile kaldırılması önerilir.
- **Başlatılmamış Display Davranışı:** EGL 1.0 kurallarına göre `eglTerminate`, başlatılmamış bir `dpy` parametresiyle çağrıldığında `EGL_TRUE` döner ve hata üretmez.
- **Geçersiz Display Davranışı:** `EGL_NO_DISPLAY` veya geçersiz bir handle verilmesi durumunda fonksiyon `EGL_FALSE` döner ve `EGL_BAD_DISPLAY` hata durumunu kaydeder.
- **State Machine İzolasyonu:** İşlem başarıyla sonuçlandıktan sonra EGLDisplay "uninitialized" duruma geçer. Bu durumdayken display üzerinden yeni bağlam veya yüzey (örn. `eglCreateContext`) oluşturulmaya kalkışılırsa anında `EGL_NOT_INITIALIZED` hatası alınır.
- **Donanımsal Ayırma (DRM/KMS):** `eglTerminate` sadece EGL objelerinin temizlenmesinden sorumludur. Native yapınızdaki pencere sistemini (X11 Display) veya Kernel Mode Setting dosya tanımlayıcılarını (DRM fd, GBM Buffer) native API'ler (`close`, `gbm_surface_destroy`) kullanarak yok etmeniz gerekir.


#include <EGL/egl.h>
#include <wayland-client.h>

/*
 * ============================================================
 * Scenario 1:
 * EGL_DEFAULT_DISPLAY ile eglGetDisplay kullanımı
 * ============================================================
 *
 * Amaç:
 * Varsayılan native display için bir EGLDisplay handle'ı istemek.
 *
 * EGL_DEFAULT_DISPLAY, uygulamanın varsayılan native display'i
 * kullanmak istediğini belirtir.
 *
 * Başarılı durumda:
 *
 *     display != EGL_NO_DISPLAY
 *
 * olur ve display değişkeni EGL tarafından kullanılacak opaque
 * bir EGLDisplay handle'ı içerir.
 *
 * Olası örnek çıktı:
 *
 *     Scenario 1
 *     EGLDisplay: 0x629b25a1cdf0
 *
 * Buradaki hexadecimal değer sadece örnektir.
 * Gerçek handle değeri sisteme ve çalıştırmaya göre değişebilir.
 *
 * Bu değer:
 * - ekran çözünürlüğü değildir,
 * - ekran numarası değildir,
 * - GPU numarası değildir.
 *
 * Sadece EGL'nin display nesnesini temsil eden bir handle'dır.
 */
void scenario_default_display(void)
{
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    (void)display;
}


/*
 * ============================================================
 * Scenario 2:
 * Aynı EGL_DEFAULT_DISPLAY ile eglGetDisplay'i tekrar çağırmak
 * ============================================================
 *
 * Amaç:
 * Aynı native display için eglGetDisplay fonksiyonunun birden
 * fazla kez çağrılmasını göstermek.
 *
 * İki çağrıda da aynı display_id değeri kullanılmaktadır:
 *
 *     EGL_DEFAULT_DISPLAY
 *
 * Daha önce yaptığımız test ortamında aynı native display için
 * yapılan tekrar çağrılarda aynı EGLDisplay handle'ı gözlenmiştir.
 *
 * Olası örnek çıktı:
 *
 *     Scenario 2
 *     display1: 0x629b25a1cdf0
 *     display2: 0x629b25a1cdf0
 *     Same display: EGL_TRUE
 *
 * Handle'ın hexadecimal değeri yalnızca örnektir.
 *
 * Burada önemli olan handle'ın sayısal değerini yorumlamak değil,
 * iki EGLDisplay değerinin aynı olup olmadığını karşılaştırmaktır.
 */
void scenario_repeated_default_display(void)
{
    EGLDisplay display1 = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLDisplay display2 = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLBoolean same_display =
        (display1 == display2) ? EGL_TRUE : EGL_FALSE;

    (void)same_display;
}


/*
 * ============================================================
 * Scenario 3:
 * Explicit Wayland native display ile eglGetDisplay kullanımı
 * ============================================================
 *
 * Amaç:
 * EGL_DEFAULT_DISPLAY yerine uygulamanın kendisinin oluşturduğu
 * bir Wayland native display bağlantısını eglGetDisplay'e vermek.
 *
 * İlk olarak Wayland API'si kullanılarak native display bağlantısı
 * oluşturulur:
 *
 *     wl_display_connect(NULL)
 *
 * Bu fonksiyon EGL'ye ait değildir; Wayland API'sine aittir.
 *
 * Daha sonra elde edilen wl_display pointer'ı EGLNativeDisplayType
 * türüne dönüştürülerek eglGetDisplay'e verilir.
 *
 * Başarılı durumda:
 *
 *     wayland_display != NULL
 *     display != EGL_NO_DISPLAY
 *
 * olur.
 *
 * Olası örnek çıktı:
 *
 *     Scenario 3
 *     Wayland display: 0x55c1a84c62a0
 *     EGLDisplay:      0x55c1a85091f0
 *
 * Bu iki değer aynı şeyi temsil etmez:
 *
 *     wl_display *
 *         -> Wayland tarafındaki native display bağlantısı
 *
 *     EGLDisplay
 *         -> EGL tarafında bu native display ile ilişkili handle
 *
 * Adreslerin hexadecimal değerleri yalnızca örnektir ve
 * çalıştırmaya göre değişebilir.
 *
 * Eğer wl_display_connect başarısız olursa:
 *
 *     wayland_display == NULL
 *
 * olur ve senaryo devam ettirilmez.
 */
void scenario_explicit_wayland_display(void)
{
    struct wl_display *wayland_display = wl_display_connect(NULL);

    if (wayland_display == NULL) {
        return;
    }

    EGLDisplay display =
        eglGetDisplay((EGLNativeDisplayType)wayland_display);

    (void)display;

    wl_display_disconnect(wayland_display);
}
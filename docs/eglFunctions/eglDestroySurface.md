# EGL 1.0: `eglDestroySurface`

```c
EGLBoolean eglDestroySurface(
    EGLDisplay dpy,
    EGLSurface surface
);
```

## 1. Bu Fonksiyon Ne Yapar?

`eglDestroySurface`, daha önce oluşturulmuş bir `EGLSurface` nesnesini yok edilmek üzere işaretler.

Bu fonksiyon window, pbuffer veya pixmap türündeki EGL surface nesnelerinde kullanılabilir. Bu projede ise `eglCreateWindowSurface()` ile GBM native surface üzerinde oluşturulan window `EGLSurface` hedef alınır.

En önemli davranış farkı şudur:

```text
Surface current değil
→ kaynakları mümkün olan en kısa sürede serbest bırakılabilir

Surface current
→ hemen tamamen yok edilmez
→ destroy için işaretlenir
→ current kaldığı sürece yaşamaya devam eder
→ sonraki geçerli eglMakeCurrent() çağrısıyla cleanup tamamlanabilir
```

---

# 2. Önce `EGLSurface` Nedir?

`EGLSurface`, OpenGL ES'in render edeceği EGL tarafındaki hedef nesnedir.

Bu projede genel zincir:

```text
struct gbm_surface *
        |
        v
eglCreateWindowSurface()
        |
        v
    EGLSurface
        |
        v
eglMakeCurrent()
        |
        v
OpenGL ES rendering
```

Burada iki farklı surface kavramı vardır:

```text
struct gbm_surface *
→ GBM/native platform surface'i

EGLSurface
→ EGL/OpenGL ES rendering surface'i
```

Bunlar aynı nesne değildir.

Bu nedenle:

```c
eglDestroySurface(...)
```

yalnızca `EGLSurface` nesnesini yönetir.

GBM surface ayrı olarak:

```c
gbm_surface_destroy(gbm_surface);
```

ile temizlenir.

---

# 3. Fonksiyonun Parametreleri

Fonksiyon iki parametre alır:

```c
eglDestroySurface(
    dpy,
    surface
);
```

Basit anlamları:

```text
dpy
→ Hangi EGLDisplay içindeki surface üzerinde işlem yapıyorum?

surface
→ Hangi EGLSurface yok edilmek üzere işaretlenecek?
```

---

# 4. Birinci Parametre: `dpy`

```c
EGLDisplay dpy
```

## 4.1 `dpy` Nedir?

`dpy`, yok edilmek istenen surface'in ait olduğu `EGLDisplay` nesnesidir.

Bu projede genel yapı:

```text
GBM native platform
       |
       v
   EGLDisplay
       |
       +-- EGLSurface
```

Normal kullanım:

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

Burada:

```text
dpy = egl_surface'in oluşturulduğu EGLDisplay
```

olur.

---

# 5. `dpy` Parametresi Neyi Değiştirir?

`dpy`, hangi EGL display namespace'i içindeki surface üzerinde işlem yapılacağını belirler.

Basit model:

```text
EGLDisplay A
    |
    +-- Surface A

EGLDisplay B
    |
    +-- Surface B
```

Doğru eşleşme:

```c
eglDestroySurface(
    displayA,
    surfaceA
);
```

şeklindedir.

Kavramsal olarak yanlış eşleşme:

```c
eglDestroySurface(
    displayB,
    surfaceA
);
```

durumunda surface ile display aynı EGLDisplay ilişkisine ait değildir.

---

# 6. `dpy` İçin Geçerli Kullanım

```c
EGLBoolean result =
    eglDestroySurface(
        egl_display,
        egl_surface
    );

if (result == EGL_TRUE) {
    printf("DESTROY SURFACE SUCCESS\n");
}
else {
    printf("DESTROY SURFACE FAILED\n");
    printf("EGL error = 0x%X\n", eglGetError());
}
```

Beklenen mantık:

```text
Geçerli EGLDisplay
        |
        v
Surface bu display'e ait
        |
        v
eglDestroySurface()
        |
        v
İşlem yürütülür
        |
        v
EGL_TRUE
```

---

# 7. `dpy` İçin Flow Chart

```text
                         dpy
                          |
              +-----------+-----------+
              |                       |
              v                       v
      Geçerli EGLDisplay        Yanlış/uygunsuz
              |                  display ilişkisi
              v                       |
 Surface bu display'e ait              v
              |                 Surface/display
              v                    eşleşmez
    eglDestroySurface()                 |
              |                         v
              v                   İşlem başarısız
         EGL_TRUE
```

Bu parametrede renk veya görüntü farkı beklenmez.

En iyi kanıt türü:

```text
display-surface eşleşme flow chart
+
başarı / başarısızlık sonucu
```

olur.

---

# 8. İkinci Parametre: `surface`

```c
EGLSurface surface
```

## 8.1 `surface` Nedir?

`surface`, yok edilmek üzere işaretlenecek `EGLSurface` handle'ıdır.

Bu projede surface daha önce:

```c
EGLSurface egl_surface =
    eglCreateWindowSurface(
        egl_display,
        egl_config,
        (EGLNativeWindowType)gbm_surface,
        NULL
    );
```

ile oluşturulmuş olabilir.

Sonra:

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

ile hedef alınır.

---

# 9. `surface` Parametresi Neyi Değiştirir?

`surface`, hangi EGL rendering surface'inin yok edileceğini belirler.

Örneğin:

```c
EGLSurface surfaceA;
EGLSurface surfaceB;
```

varsa:

```c
eglDestroySurface(
    egl_display,
    surfaceA
);
```

yalnızca:

```text
surfaceA
```

üzerinde işlem yapar.

`surfaceB` etkilenmez.

Kavramsal gösterim:

```text
surfaceA
    |
    v
eglDestroySurface()
    |
    v
Surface A destroy için hedef

surfaceB
    |
    v
Etkilenmez
```

---

# 10. `surface` İçin En Önemli Konu: Current State

Bir surface iki önemli rolde current olabilir:

```text
current draw surface
current read surface
```

Örneğin:

```c
eglMakeCurrent(
    egl_display,
    egl_surface,
    egl_surface,
    egl_context
);
```

çağrısından sonra:

```text
Thread
  |
  +-- Draw Surface = egl_surface
  +-- Read Surface = egl_surface
  +-- Context      = egl_context
```

durumu oluşur.

Bu nedenle aynı `eglDestroySurface()` çağrısının davranışı surface'in current olup olmamasına göre değişir.

---

# 11. Durum A - Surface Current Değil

Önce bağı kaldırabiliriz:

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

Bu durumda:

```text
Draw surface = EGL_NO_SURFACE
Read surface = EGL_NO_SURFACE
```

olur.

Yani eski `egl_surface` artık current değildir.

Sonra:

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

çağrılır.

Beklenen süreç:

```text
EGLSurface
    |
    v
Current değil
    |
    v
eglDestroySurface()
    |
    v
Destroy için işaretlenir
    |
    v
Kaynakları serbest bırakılabilir
```

Bu en temiz kapanış şeklidir.

---

# 12. Current Olmayan Surface İçin Flow Chart

```text
[ EGLSurface ]
      |
      | current değil
      v
[ eglDestroySurface ]
      |
      v
[ DESTROY ]
      |
      v
[ CLEANUP ]
```

Daha ayrıntılı:

```text
Surface current değil
        |
        v
eglDestroySurface()
        |
        v
Surface destroy için işaretlenir
        |
        v
Kaynaklar mümkün olan en kısa sürede
serbest bırakılabilir
```

---

# 13. Durum B - Surface Current

Şimdi surface aktif olarak current olsun:

```c
eglMakeCurrent(
    egl_display,
    egl_surface,
    egl_surface,
    egl_context
);
```

Durum:

```text
Thread
  |
  +-- Current Draw Surface = egl_surface
  +-- Current Read Surface = egl_surface
```

Sonra:

```c
eglDestroySurface(
    egl_display,
    egl_surface
);
```

çağrılır.

Burada kritik sonuç:

> Surface hemen tamamen yok edilmez.

Bunun yerine:

```text
surface current
+
destroy çağrılmış
```

durumuna gelir.

Bunu:

```text
pending destruction
```

gibi düşünebiliriz.

---

# 14. Current Surface Neden Hemen Yok Edilmez?

Çünkü thread hâlâ o surface'i:

```text
draw target
veya
read target
```

olarak kullanıyor olabilir.

Örneğin:

```text
Thread
  |
  +-- EGLContext
  |
  +-- Current Draw Surface
  |
  +-- Current Read Surface
```

vardır.

Surface bir anda tamamen silinirse aktif rendering target ortadan kalkmış olur.

Bu nedenle EGL kaynakları hemen bırakmak yerine destruction işlemini erteler.

Akış:

```text
eglDestroySurface()
        |
        v
Destroy istendi
        |
        v
Ama surface current
        |
        v
Kaynaklar tutulur
        |
        v
Current bağı kalkana kadar beklenir
```

---

# 15. Current Surface'in Gerçek Yok Edilmesi

Surface current iken destroy çağrısından sonra:

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

gibi geçerli bir çağrı yapılırsa eski surface artık current olmaktan çıkar.

Akış:

```text
Surface current
        |
        v
eglDestroySurface()
        |
        v
Destroy için işaretlenir
        |
        v
Surface hâlâ current
        |
        v
eglMakeCurrent(... EGL_NO_SURFACE ...)
        |
        v
Current bağı kaldırılır
        |
        v
Cleanup tamamlanabilir
```

---

# 16. Current Surface İçin Flow Chart

```text
                    EGLSurface
                        |
                        v
                      CURRENT
                        |
                        v
               eglDestroySurface()
                        |
                        v
              Destroy için işaretli
                        |
                        v
             Surface yaşamaya devam
                        |
                        v
              eglMakeCurrent(...)
                        |
                        v
              Current bağı kaldırılır
                        |
                        v
                  Gerçek cleanup
```

Bu, `eglDestroySurface()` fonksiyonunun en önemli davranışıdır.

---

# 17. Current ve Current Olmayan Surface Karşılaştırması

```text
              surface state
                   |
        +----------+----------+
        |                     |
        v                     v
  Current değil            Current
        |                     |
        v                     v
eglDestroySurface()    eglDestroySurface()
        |                     |
        v                     v
Cleanup yapılabilir    Destroy için işaretlenir
                              |
                              v
                     Surface yaşamaya devam
                              |
                              v
                      eglMakeCurrent(...)
                              |
                              v
                      Current bağı kalkar
                              |
                              v
                         Cleanup
```

Bu parametre için en güçlü kanıt görsel renk değişimi değil, lifecycle farkıdır.

---

# 18. `eglDestroySurface()` ile `eglDestroyContext()` Farkı

Bu iki fonksiyon benzer destruction mantığına sahiptir ancak farklı EGL nesnelerini hedefler.

```text
eglDestroyContext()
→ EGLContext'i yönetir

eglDestroySurface()
→ EGLSurface'i yönetir
```

Kavramsal ayrım:

```text
EGLContext
→ rendering state / OpenGL ES çalışma durumu

EGLSurface
→ rendering target
```

Basit benzetme:

```text
Context
→ ressamın çalışma durumu ve araçları

Surface
→ tuval
```

Yani:

```text
eglDestroyContext()
→ çalışma state'ini yok eder

eglDestroySurface()
→ render target'ı yok eder
```

---

# 19. İki Farklı Surface ile Kavramsal Deney

İki surface düşünelim:

```c
EGLSurface surfaceA;
EGLSurface surfaceB;
```

### Test A

```c
eglDestroySurface(
    egl_display,
    surfaceA
);
```

Kavramsal sonuç:

```text
Surface A
   |
   v
Destroy için hedef

Surface B
   |
   v
Etkilenmez
```

### Test B

```c
eglDestroySurface(
    egl_display,
    surfaceB
);
```

Kavramsal sonuç:

```text
Surface B
   |
   v
Destroy için hedef

Surface A
   |
   v
Etkilenmez
```

Bu örnek:

> `surface` parametresinin hangi EGLSurface nesnesinin hedef alınacağını belirlediğini

gösterir.

---

# 20. Dönüş Değeri

Fonksiyon:

```c
EGLBoolean
```

döndürür.

Başarılı çağrı:

```text
EGL_TRUE
```

Başarısız çağrı:

```text
EGL_FALSE
```

Örnek:

```c
if (!eglDestroySurface(
        egl_display,
        egl_surface)) {

    EGLint err = eglGetError();

    printf(
        "eglDestroySurface failed: 0x%X\n",
        err
    );
}
```

---

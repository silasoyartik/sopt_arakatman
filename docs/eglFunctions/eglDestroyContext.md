# EGL 1.0: `eglDestroyContext`

```c
EGLBoolean eglDestroyContext(
    EGLDisplay dpy,
    EGLContext ctx
);
```

## 1. Bu Fonksiyon Ne Yapar?

`eglDestroyContext`, daha önce oluşturulmuş bir `EGLContext` nesnesini yok edilmek üzere işaretler.

Buradaki en önemli davranış farkı context'in o anda **current** olup olmamasıdır.

Eğer context herhangi bir thread üzerinde current değilse kaynakları serbest bırakılabilir.

Eğer context current ise:

```text
eglDestroyContext()
```

çağrısından sonra context hemen tamamen ortadan kalkmaz.

Bunun yerine:

```text
destroy için işaretlenir
```

ve current kaldığı sürece yaşamaya devam eder.

Sonraki geçerli:

```c
eglMakeCurrent(...)
```

çağrısında eski context current olmaktan çıktığında gerçek cleanup tamamlanabilir.

---

# 2. Önce `EGLContext` Nedir?

`EGLContext`, OpenGL ES'in rendering state'ini temsil eden EGL nesnesidir.

Basit düşünürsek context şunlarla ilişkilidir:

```text
OpenGL ES state
shader/program state
buffer bindings
texture bindings
rendering state
```

Context normalde:

```c
EGLContext egl_context =
    eglCreateContext(
        egl_display,
        egl_config,
        EGL_NO_CONTEXT,
        NULL
    );
```

ile oluşturulur.

Ancak yalnızca context oluşturmak OpenGL ES komutlarının onu kullanacağı anlamına gelmez.

Context ayrıca:

```c
eglMakeCurrent(...)
```

ile bir thread üzerinde current yapılmalıdır.

---

# 3. "Current Context" Nedir?

Bir thread üzerinde kullanılan aktif EGL rendering context'e:

```text
current context
```

denir.

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
  +-- Current EGLContext = egl_context
  +-- Current draw surface = egl_surface
  +-- Current read surface = egl_surface
```

durumu oluşur.

Bundan sonra:

```c
glClear(...)
glUseProgram(...)
glDrawArrays(...)
```

gibi OpenGL ES komutları thread'in current context'i üzerinden çalışır.

Bu nedenle context current durumdayken doğrudan yok edilmesi ile current değilken yok edilmesi aynı davranışı oluşturmaz.

---

# 4. Normal Context Yaşam Döngüsü

```text
eglCreateContext()
        |
        v
    EGLContext
        |
        v
eglMakeCurrent()
        |
        v
Context current
        |
        v
OpenGL ES rendering
        |
        v
eglMakeCurrent(
    ... EGL_NO_CONTEXT
)
        |
        v
Context artık current değil
        |
        v
eglDestroyContext()
        |
        v
Context kaynakları serbest bırakılabilir
```

Bu en temiz kapanış mantığıdır.

---

# 5. Parametrelerin Genel Mantığı

Fonksiyon iki parametre alır:

```c
eglDestroyContext(
    dpy,
    ctx
);
```

Basit anlamları:

```text
dpy
-> Hangi EGLDisplay içindeki context üzerinde işlem yapıyorum?

ctx
-> Hangi EGLContext yok edilmek üzere işaretlenecek?
```

Bu dokümanda her parametre için:

1. Parametrenin görevi,
2. Parametre değişince ne olduğu,
3. Örnek kod,
4. Beklenen davranış,
5. Flow chart

ayrı ayrı incelenmektedir.

---

# 6. Birinci Parametre: `dpy`

```c
EGLDisplay dpy
```

## 6.1 `dpy` Nedir?

`dpy`, yok edilmek istenen context'in ait olduğu `EGLDisplay` nesnesidir.

Bu projede genel yapı:

```text
/dev/dri/card*
      |
      v
gbm_device
      |
      v
EGLDisplay
      |
      +-- EGLContext
```

şeklindedir.

Normal kullanım:

```c
eglDestroyContext(
    egl_display,
    egl_context
);
```

Burada:

```text
dpy = egl_display
```

olur.

---

## 6.2 `dpy` Parametresi Neyi Değiştirir?

`dpy`, hangi EGL display namespace'i içerisindeki context üzerinde işlem yapılacağını belirler.

Basit model:

```text
EGLDisplay A
    |
    +-- Context A

EGLDisplay B
    |
    +-- Context B
```

Doğru eşleşme:

```c
eglDestroyContext(
    displayA,
    contextA
);
```

şeklindedir.

Kavramsal olarak yanlış eşleşme:

```c
eglDestroyContext(
    displayB,
    contextA
);
```

durumunda context ile display aynı EGLDisplay ilişkisine ait değildir.

---

# 7. `dpy` Deneyi - Geçerli Display

Diğer parametre sabit tutulur:

```text
ctx = aynı
```

Sadece geçerli `dpy` kullanılır.

```c
EGLBoolean result_valid =
    eglDestroyContext(
        egl_display,
        egl_context
    );

if (result_valid == EGL_TRUE) {
    printf("VALID DISPLAY TEST: SUCCESS\n");
}
else {
    printf("VALID DISPLAY TEST: FAILED\n");
    printf("EGL error = 0x%X\n", eglGetError());
}
```

Beklenen mantık:

```text
Geçerli EGLDisplay
        |
        v
Context bu display ile ilişkili
        |
        v
eglDestroyContext()
        |
        v
Destroy işlemi yürütülebilir
        |
        v
EGL_TRUE
```

Örnek beklenen çıktı:

```text
VALID DISPLAY TEST: SUCCESS
```

Bu çıktı gerçek çalıştırılmış sonuç değilse raporda:

```text
Beklenen / kavramsal çıktı
```

olarak belirtilmelidir.

---

# 8. `dpy` Deneyi - `EGL_NO_DISPLAY`

Kontrollü karşılaştırma için:

```c
EGLBoolean result_invalid =
    eglDestroyContext(
        EGL_NO_DISPLAY,
        egl_context
    );

if (result_invalid == EGL_FALSE) {

    printf("INVALID DISPLAY TEST: FAILED\n");

    EGLint error = eglGetError();

    printf(
        "EGL error = 0x%X\n",
        error
    );
}
```

Burada:

```text
ctx = aynı
```

kalır.

Sadece:

```text
dpy
```

değişir.

Beklenen akış:

```text
dpy = EGL_NO_DISPLAY
        |
        v
Geçerli EGL display yok
        |
        v
eglDestroyContext()
        |
        v
İşlem başarısız
        |
        v
EGL_FALSE
        |
        v
eglGetError()
```

---

# 9. `dpy` İçin Flow Chart

```text
                         dpy
                          |
              +-----------+-----------+
              |                       |
              v                       v
      Geçerli EGLDisplay        EGL_NO_DISPLAY
              |                       |
              v                       v
 Context bu display'e ait      Geçerli display yok
              |                       |
              v                       v
    eglDestroyContext()       eglDestroyContext()
              |                       |
              v                       v
      İşlem yürütülür            İşlem başarısız
              |                       |
              v                       v
          EGL_TRUE               EGL_FALSE
                                        |
                                        v
                                   eglGetError()
```

Bu parametrede görsel rendering farkı beklenmez.

En uygun kanıt:

```text
başarı / başarısızlık
+
display-context eşleşme flow chart
```

olur.

---

# 10. İkinci Parametre: `ctx`

```c
EGLContext ctx
```

## 10.1 `ctx` Nedir?

`ctx`, yok edilmek üzere işaretlenecek EGL rendering context handle'ıdır.

Örneğin:

```c
EGLContext egl_context =
    eglCreateContext(
        egl_display,
        egl_config,
        EGL_NO_CONTEXT,
        NULL
    );
```

ile oluşturulmuş bir context:

```c
eglDestroyContext(
    egl_display,
    egl_context
);
```

ile hedef alınır.

---

# 11. `ctx` Parametresi Neyi Değiştirir?

`ctx`, hangi EGLContext nesnesinin destroy edileceğini belirler.

Örneğin:

```c
EGLContext contextA;
EGLContext contextB;
```

varsa:

```c
eglDestroyContext(
    egl_display,
    contextA
);
```

yalnızca:

```text
contextA
```

üzerinde işlem yapar.

Aynı şekilde:

```c
eglDestroyContext(
    egl_display,
    contextB
);
```

yalnızca:

```text
contextB
```

nesnesini hedefler.

Kavramsal gösterim:

```text
contextA
    |
    v
eglDestroyContext()
    |
    v
A destroy için işaretlenir

contextB
    |
    v
Etkilenmez
```

---

# 12. `ctx` İçin En Önemli Konu: Current State

Bu parametrede yalnızca hangi handle'ın seçildiği değil, seçilen context'in mevcut state'i de çok önemlidir.

İki ana durum vardır:

```text
1. Context current değil
2. Context current
```

Bu iki durumda aynı:

```c
eglDestroyContext(
    egl_display,
    egl_context
);
```

çağrısı farklı yaşam döngüsü davranışı oluşturur.

---

# 13. Durum A - Context Current Değil

Önce current bağı kaldırılır:

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

Bu çağrıdan sonra:

```text
Thread current context = EGL_NO_CONTEXT
```

olur.

Yani `egl_context` artık current değildir.

Sonra:

```c
EGLBoolean result =
    eglDestroyContext(
        egl_display,
        egl_context
    );
```

çağrılır.

Beklenen süreç:

```text
Context current değil
        |
        v
eglDestroyContext()
        |
        v
Context destroy için işaretlenir
        |
        v
Kaynaklar serbest bırakılabilir
```

Bu, en temiz kapanış biçimidir.

---

# 14. Current Olmayan Context İçin Flow Chart

```text
EGLContext
    |
    v
Current değil
    |
    v
eglDestroyContext()
    |
    v
Destroy için işaretlenir
    |
    v
Kaynaklar serbest bırakılabilir
```

Kısa görsel:

```text
[ CONTEXT ]
     |
     | current değil
     v
[ DESTROY ]
     |
     v
[ CLEANUP ]
```

---

# 15. Durum B - Context Current

Şimdi aynı context'i önce current yapalım:

```c
eglMakeCurrent(
    egl_display,
    egl_surface,
    egl_surface,
    egl_context
);
```

Bu çağrıdan sonra:

```text
Thread
  |
  +-- Current Context = egl_context
```

durumu vardır.

Sonra:

```c
eglDestroyContext(
    egl_display,
    egl_context
);
```

çağrılır.

Burada çok önemli sonuç:

> Context hemen tamamen ortadan kalkmaz.

Context:

```text
current
+
destroy çağrılmış
```

durumuna gelir.

Başka bir ifadeyle:

```text
pending destruction
```

gibi düşünülebilir.

---

# 16. Current Context Neden Hemen Yok Edilmez?

Çünkü thread hâlâ bu context'i aktif olarak kullanıyor olabilir.

Örneğin:

```text
Thread
  |
  +-- Current EGLContext
          |
          +-- OpenGL ES state
          +-- Render işlemleri
```

vardır.

Eğer EGL context'i bir anda tamamen kaldırırsa thread'in hâlâ kullandığı rendering state ortadan kalkmış olur.

Bu nedenle yaşam döngüsü:

```text
eglDestroyContext()
        |
        v
"Destroy istendi"
        |
        v
Ama context current
        |
        v
Kaynaklar tutulur
```

şeklinde devam eder.

---

# 17. Current Context'in Gerçek Yok Edilmesi

Context current iken destroy çağrıldıktan sonra:

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

gibi geçerli bir `eglMakeCurrent()` çağrısı yapılırsa eski context artık current olmaktan çıkar.

Sonra destruction tamamlanabilir.

Akış:

```text
Context current
        |
        v
eglDestroyContext()
        |
        v
Destroy için işaretlenir
        |
        v
Context hâlâ current
        |
        v
eglMakeCurrent(... EGL_NO_CONTEXT)
        |
        v
Eski context artık current değil
        |
        v
Gerçek cleanup tamamlanabilir
```

---

# 18. Current Context İçin Flow Chart

```text
                    EGLContext
                        |
                        v
                     CURRENT
                        |
                        v
               eglDestroyContext()
                        |
                        v
              Destroy için işaretli
                        |
                        v
             Context yaşamaya devam
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

Bu, `eglDestroyContext()` fonksiyonunun en önemli davranışıdır.

---

# 19. Current ve Current Olmayan Context Karşılaştırması

```text
                ctx state
                    |
         +----------+----------+
         |                     |
         v                     v
   Current değil             Current
         |                     |
         v                     v
eglDestroyContext()     eglDestroyContext()
         |                     |
         v                     v
Destroy yapılabilir     Destroy için işaretlenir
         |                     |
         v                     v
Cleanup mümkün          Context yaşamaya devam
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

Bu parametre için en güçlü kanıt görsel rendering değil, bu lifecycle farkıdır.

---

# 20. `eglMakeCurrent(... EGL_NO_CONTEXT)` ile `eglDestroyContext()` Aynı Şey mi?

Hayır.

Bu ayrım çok önemlidir.

Şu çağrı:

```c
eglMakeCurrent(
    egl_display,
    EGL_NO_SURFACE,
    EGL_NO_SURFACE,
    EGL_NO_CONTEXT
);
```

context'i:

```text
thread'den ayırır
```

ama context nesnesini yok etmez.

Yani:

```text
UNBIND / RELEASE CURRENT
```

işlemidir.

Buna karşılık:

```c
eglDestroyContext(
    egl_display,
    egl_context
);
```

context nesnesini:

```text
yok edilmek üzere işaretler
```

Yani:

```text
eglMakeCurrent(... EGL_NO_CONTEXT)
-> current bağı kaldırılır

eglDestroyContext()
-> context destroy edilir / destroy için işaretlenir
```

Bunlar farklı işlemlerdir.

---

# 21. İki Farklı Context ile Kavramsal Deney

İki context olduğunu düşünelim:

```c
EGLContext contextA;
EGLContext contextB;
```

### Test A

```c
eglDestroyContext(
    egl_display,
    contextA
);
```

Kavramsal sonuç:

```text
contextA
   |
   v
Destroy için hedef

contextB
   |
   v
Değişmez
```

### Test B

```c
eglDestroyContext(
    egl_display,
    contextB
);
```

Kavramsal sonuç:

```text
contextB
   |
   v
Destroy için hedef

contextA
   |
   v
Değişmez
```

Bu deney:

> `ctx` parametresinin hangi context nesnesinin hedef alınacağını belirlediğini

gösterir.

---

# 22. Dönüş Değeri

Fonksiyon:

```c
EGLBoolean
```

döndürür.

Başarılı çağrı:

```c
EGL_TRUE
```

Başarısız çağrı:

```c
EGL_FALSE
```

Örnek:

```c
if (!eglDestroyContext(
        egl_display,
        egl_context)) {

    EGLint error = eglGetError();

    printf(
        "eglDestroyContext failed: 0x%X\n",
        error
    );
}
```

---

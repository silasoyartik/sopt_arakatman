# OpenGL 4.3: `glDispatchCompute`

```C
void glDispatchCompute(GLuint num_groups_x,
                       GLuint num_groups_y,
                       GLuint num_groups_z);
```

`glDispatchCompute`, o an aktif olan Compute Shader programını kullanarak GPU üzerinde paralel hesaplama iş gruplarını (work groups) tetikler. Geleneksel grafik boru hattından (vertex/fragment pipeline) bağımsız olarak çalışır ve genel amaçlı hesaplamaları (GPGPU) başlatır.

İşlem başlatılmadan önce verilerin GPU'ya bağlanmasıyla ilgili süreçler için `glBindBufferBase` dökümantasyonuna bakınız.

## Kavramsal Akış

Compute Shader mimarisinde çalışma hiyerarşisi iki temel katmandan oluşur. `glDispatchCompute` bu katmanlardan en üstte olanı, yani **Global Work Group (İş Grubu)** sayısını belirler:

```
glDispatchCompute(X, Y, Z)
  |
  +-- Work Group (0, 0, 0)
  |     |
  |     +-- Local Thread (0, 0, 0) -> gl_LocalInvocationID
  |     +-- Local Thread (1, 0, 0)
  |     +-- ...
  |
  +-- Work Group (1, 0, 0)
  +-- ...
  +-- Work Group (X-1, Y-1, Z-1)
```

* **Global İş (Grid):** `glDispatchCompute` tarafından X, Y ve Z eksenlerinde kaç adet grubun çalıştırılacağını belirtir.
* **Yerel İş (Local Size):** Her bir grubun içinde kaç adet iş parçacığının (thread) bulunacağı, uygulamanın C kodunda değil, doğrudan GLSL shader kodu içinde (`layout(local_size_x = X, ...) in;`) belirlenir.
* Toplam çalışan iş parçacığı sayısı: (Global Work Group Boyutları) **$\times$** (Local Thread Boyutları) şeklinde hesaplanır.

## Parametreler

`glDispatchCompute`, işlemi 1, 2 veya 3 boyutlu bir uzayda başlatabilir. Kullanılmayan boyutlar için `1` değeri verilmelidir (`0` değeri verilirse hiçbir iş başlatılmaz).

### `num_groups_x`, `num_groups_y`, `num_groups_z`

Her bir eksendeki (X, Y ve Z) iş grubu (work group) sayısını belirten işaretsiz tam sayı (GLuint) parametreleridir.

| **Parametre Değerleri**                           | **Sonuç**                                                                             |
| -------------------------------------------------------- | -------------------------------------------------------------------------------------------- |
| Sınırlar dâhilinde geçerli sayılar (Örn: 10, 1, 1) | Geçerli. Komut GPU işlem kuyruğuna eklenir ve shader paralel olarak çalışmaya başlar. |
| Herhangi bir boyut`0`ise                               | Geçerli. Çağrı hata vermez ancak GPU hiçbir hesaplama yapmaz (0 adet iş grubu).        |
| Donanım sınırını aşan değerler                    | Başarısız. OpenGL hata mekanizmasıyla`GL_INVALID_VALUE`üretilir.                      |
| Aktif bir Compute Shader yoksa                           | Başarısız. OpenGL hata mekanizmasıyla`GL_INVALID_OPERATION`üretilir.                  |

## Sınırlar ve Uyumluluk

Her GPU'nun `glDispatchCompute` için desteklediği maksimum bir boyut sınırı vardır. Bu sınır X, Y ve Z eksenleri için farklı olabilir. Uygulama geliştirilirken bu sınırların donanımdan sorgulanması gerekebilir:

```C
GLint max_work_groups_x;
glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_work_groups_x);
// 0: X ekseni, 1: Y ekseni, 2: Z ekseni
```

OpenGL 4.3 Core spesifikasyonuna göre minimum donanım destek sınırları şöyledir:

* X ve Y eksenleri için en az 65535
* Z ekseni için en az 65535

## Temel Kullanım

```C
// 1. Compute Shader'ı barındıran program aktif edilir
glUseProgram(computeProgram);

// (Burada glBindBufferBase ile veriler bağlanır, bkz. ilgili döküman)

// 2. İş parçacıkları X=5, Y=1, Z=1 olacak şekilde başlatılır
glDispatchCompute(5, 1, 1);

// 3. GPU asenkron çalıştığı için, sonuçları okumadan önce bariyer konur
// (Senkronizasyon detayları için glMemoryBarrier dökümantasyonuna bakınız)
glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

// (Sonuçların CPU'ya çekilmesi için glGetBufferSubData dökümanına bakınız)
```

## Hata Kodları

| **Hata**           | **Oluşma Koşulu**                                                                                                                                             |
| ------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `GL_INVALID_OPERATION` | `glUseProgram`ile aktif edilmiş program bir Compute Shader içermiyorsa veya o an aktif bir program yoksa.                                                         |
| `GL_INVALID_VALUE`     | `num_groups_x`,`num_groups_y`veya`num_groups_z`değerlerinden herhangi biri, ilgili eksen için cihazın`GL_MAX_COMPUTE_WORK_GROUP_COUNT`limitini aşıyorsa. |

## Durum Değişimi

Başarılı çağrı öncesi:

* GPU işlem kuyruğunda compute iş emri yoktur. SSBO (Shader Storage Buffer Object) bellekleri CPU veya önceki işlemler tarafından yazılmış durumdadır.

Başarılı `glDispatchCompute(X, Y, Z)` sonrası:

* İş emri GPU'nun komut kuyruğuna (command queue) eklenir.
* İşlem asenkron (eşzamansız) başlatılır; yani fonksiyon C kodunda anında geri dönerken, GPU arka planda hesaplamalara devam eder.
* İlgili bellek tamponlarındaki (buffer) veriler GPU tarafından değiştirilmeye başlar.

## Bölüm Özeti

* `glDispatchCompute`, compute shader'ın X, Y ve Z boyutlarındaki iş grubu (work group) sayısını belirleyerek GPGPU işlemlerini tetikler.
* Çalışan toplam iş parçacığı sayısı, bu fonksiyon ile verilen parametrelerin, GLSL kodu içindeki `local_size` değişkenleriyle çarpılması sonucu bulunur.
* Bu işlem tamamen asenkron yürütülür; CPU beklemez.
* Çağrıdan önce `glUseProgram` ile ilgili programın aktif olduğundan emin olunmalıdır.
* Çağrıdan sonra okuma işlemi yapılacaksa, `glMemoryBarrier` fonksiyonu kullanılarak GPU'nun işi bitirmesi zorunlu kılınmalıdır.
* Donanım sınırlarını aşan iş grubu gönderimleri `GL_INVALID_VALUE` hatası fırlatır.

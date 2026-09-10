# OpenGL ES 3.1: `glDispatchCompute`

## 1. Fonksiyon Tanımı

```c
void glDispatchCompute(
    GLuint num_groups_x,
    GLuint num_groups_y,
    GLuint num_groups_z
);
```

`glDispatchCompute`, OpenGL ES 3.1 içerisinde compute shader tarafından çalıştırılacak work group'ların sayısını belirler ve compute shader yürütmesini başlatır.

Fonksiyon ile doğrudan invocation veya thread sayısı belirtilmez. Bunun yerine X, Y ve Z boyutlarında kaç adet work group çalıştırılacağı belirtilir.

Her work group içerisindeki invocation sayısı ise GLSL ES 3.10 compute shader içerisinde belirtilen `local_size_x`, `local_size_y` ve `local_size_z` değerleri tarafından belirlenir.

---

## 2. Compute İşlem Hiyerarşisi

Compute shader çalışma modeli iki temel seviyeden oluşur:

1. Dispatch grid
2. Work group içerisindeki local invocations

Örneğin:

```c
glDispatchCompute(4, 2, 1);
```

çağrısı toplam:

```text
4 × 2 × 1 = 8 work group
```

başlatılmasını ister.

Work group'lar X, Y ve Z boyutlarında aşağıdaki şekilde düzenlenir:

```text
Dispatch Grid

Y = 0:
    Group (0,0,0)
    Group (1,0,0)
    Group (2,0,0)
    Group (3,0,0)

Y = 1:
    Group (0,1,0)
    Group (1,1,0)
    Group (2,1,0)
    Group (3,1,0)
```

Z boyutu `1` olduğu için bu örnekte yalnızca tek Z katmanı bulunmaktadır.

---

## 3. Parametreler

### 3.1 `num_groups_x`

`num_groups_x`, X ekseninde çalıştırılacak work group sayısını belirtir.

Örneğin:

```c
glDispatchCompute(8, 1, 1);
```

çağrısı X ekseninde 8 work group oluşturulmasını ister.

---

### 3.2 `num_groups_y`

`num_groups_y`, Y ekseninde çalıştırılacak work group sayısını belirtir.

Örneğin:

```c
glDispatchCompute(1, 4, 1);
```

çağrısı Y ekseninde 4 work group oluşturulmasını ister.

---

### 3.3 `num_groups_z`

`num_groups_z`, Z ekseninde çalıştırılacak work group sayısını belirtir.

Örneğin:

```c
glDispatchCompute(1, 1, 4);
```

çağrısı Z ekseninde 4 work group oluşturulmasını ister.

---

## 4. Toplam Work Group Sayısı

Dispatch tarafından oluşturulan toplam work group sayısı:

```text
num_groups_x × num_groups_y × num_groups_z
```

şeklinde hesaplanır.

Örneğin:

```c
glDispatchCompute(4, 3, 2);
```

için:

```text
4 × 3 × 2 = 24 work group
```

oluşturulur.

Burada `glDispatchCompute` tarafından belirlenen değerler work group count değerleridir.

Bunlar work group içerisindeki invocation sayısını doğrudan belirlemez.

---

## 5. Local Work Group Size

Her work group içerisinde kaç invocation bulunacağı GLSL ES 3.10 compute shader içerisinde belirlenir.

Örneğin:

```glsl
#version 310 es

layout(
    local_size_x = 8,
    local_size_y = 4,
    local_size_z = 1
) in;

void main()
{
}
```

Bu shader için bir work group içerisindeki invocation sayısı:

```text
8 × 4 × 1 = 32 invocation
```

olur.

Eğer C tarafında:

```c
glDispatchCompute(4, 2, 1);
```

çağrılırsa toplam work group sayısı:

```text
4 × 2 × 1 = 8
```

ve toplam invocation sayısı:

```text
8 × 32 = 256
```

olur.

Dolayısıyla genel olarak:

```text
Toplam invocation =
(num_groups_x × num_groups_y × num_groups_z)
×
(local_size_x × local_size_y × local_size_z)
```

şeklinde ifade edilebilir.

---

## 6. Work Group ve Invocation Kimlikleri

Compute shader içerisinde her invocation'ın dispatch içerisindeki konumunu belirlemek için çeşitli built-in değişkenler kullanılabilir.

Başlıca compute shader built-in değişkenleri şunlardır:

| Built-in                    | Açıklama                                                    |
| --------------------------- | ------------------------------------------------------------- |
| `gl_NumWorkGroups`        | Dispatch edilen work group sayısını belirtir.              |
| `gl_WorkGroupID`          | Çalışmakta olan work group'ın kimliğini belirtir.        |
| `gl_LocalInvocationID`    | Work group içerisindeki invocation kimliğini belirtir.      |
| `gl_GlobalInvocationID`   | Dispatch içerisindeki global invocation kimliğini belirtir. |
| `gl_LocalInvocationIndex` | Local invocation'ın lineer indeksini belirtir.               |

Örneğin:

```glsl
#version 310 es

layout(
    local_size_x = 4,
    local_size_y = 1,
    local_size_z = 1
) in;

void main()
{
    uvec3 group_id = gl_WorkGroupID;
    uvec3 global_id = gl_GlobalInvocationID;
}
```

Burada `gl_WorkGroupID` mevcut work group'ın konumunu, `gl_GlobalInvocationID` ise invocation'ın tüm dispatch grid içerisindeki global konumunu belirtir.

---

## 7. Aktif Compute Shader Programı

`glDispatchCompute` çağrısının geçerli bir şekilde gerçekleştirilmesi için aktif programın compute shader executable içermesi gerekir.

Örneğin:

```c
glUseProgram(computeProgram);

glDispatchCompute(4, 1, 1);
```

Burada `computeProgram`, başarıyla link edilmiş ve compute shader içeren programdır.

Compute shader içeren geçerli bir program aktif değilse `glDispatchCompute` hata oluşturur.

Beklenen hata:

```text
GL_INVALID_OPERATION
```

Bu nedenle tipik kullanım sırası:

```text
Compute Shader
      ↓
Shader Compile
      ↓
Program Link
      ↓
glUseProgram()
      ↓
glDispatchCompute()
```

şeklindedir.

---

## 8. Work Group Count Limitleri

Uygulamanın tek bir dispatch çağrısında kullanabileceği maksimum work group sayısı implementasyon tarafından belirlenir.

Bu değer `GL_MAX_COMPUTE_WORK_GROUP_COUNT` kullanılarak sorgulanabilir.

Örneğin:

```c
GLint maxGroupsX;
GLint maxGroupsY;
GLint maxGroupsZ;

glGetIntegeri_v(
    GL_MAX_COMPUTE_WORK_GROUP_COUNT,
    0,
    &maxGroupsX
);

glGetIntegeri_v(
    GL_MAX_COMPUTE_WORK_GROUP_COUNT,
    1,
    &maxGroupsY
);

glGetIntegeri_v(
    GL_MAX_COMPUTE_WORK_GROUP_COUNT,
    2,
    &maxGroupsZ
);
```

Burada indeks değerleri:

```text
0 → X
1 → Y
2 → Z
```

boyutlarını temsil eder.

OpenGL ES 3.1 minimum gereksinimlerinde her boyut için maksimum work group count değerinin en az `65535` olması gerekir.

Ancak uygulama çalışırken gerçek değerler doğrudan sorgulanmalıdır.

---

## 9. Work Group Count Limitinin Aşılması

`glDispatchCompute` parametrelerinden herhangi biri ilgili maksimum değeri aşarsa hata oluşturulur.

Örneğin:

```c
glDispatchCompute(maxGroupsX + 1, 1, 1);
```

X boyutundaki maksimum değer aşıldığında:

```text
GL_INVALID_VALUE
```

oluşturulur.

Aynı durum Y ve Z boyutları için de geçerlidir.

Örneğin:

```c
glDispatchCompute(1, maxGroupsY + 1, 1);
```

ve:

```c
glDispatchCompute(1, 1, maxGroupsZ + 1);
```

çağrıları da ilgili dimension limitinin aşılması durumunda `GL_INVALID_VALUE` oluşturur.

---

## 10. Sıfır Work Group Count

Dispatch parametrelerinden herhangi biri sıfır olabilir.

Örneğin:

```c
glDispatchCompute(0, 1, 1);
```

çağrısında X boyutundaki work group count sıfırdır.

Benzer şekilde:

```c
glDispatchCompute(1, 0, 1);
```

ve:

```c
glDispatchCompute(1, 1, 0);
```

çağrıları da geçerli dispatch parametreleri olarak değerlendirilebilir.

Bir dimension sıfır olduğunda toplam dispatch edilen work group sayısı sıfır olur.

Dolayısıyla compute shader invocation'ı çalıştırılmaz.

Bu durum tek başına `GL_INVALID_VALUE` oluşturmaz.

---

## 11. Compute Shader ile Kullanım

GLSL ES 3.10 içerisinde compute shader aşağıdaki gibi tanımlanabilir:

```glsl
#version 310 es

layout(
    local_size_x = 4,
    local_size_y = 1,
    local_size_z = 1
) in;

void main()
{
    uint index = gl_GlobalInvocationID.x;

    // Compute operation
}
```

C tarafında shader programı aktif hale getirildikten sonra:

```c
glUseProgram(computeProgram);

glDispatchCompute(4, 1, 1);
```

çağrısı yapılabilir.

Bu durumda:

```text
Work group count = 4 × 1 × 1
                 = 4 work group
```

olur.

Shader içerisindeki local size:

```text
4 × 1 × 1
```

olduğu için her work group 4 invocation içerir.

Toplam invocation:

```text
4 × 4 = 16
```

olur.

---

## 12. Compute Resource Kullanımı

Compute shader'lar OpenGL ES 3.1 tarafından sağlanan çeşitli shader kaynaklarına erişebilir.

Örneğin shader storage buffer kullanılabilir:

```glsl
#version 310 es

layout(
    local_size_x = 4,
    local_size_y = 1,
    local_size_z = 1
) in;

layout(std430, binding = 0) buffer OutputBuffer
{
    uint data[];
};

void main()
{
    uint index = gl_GlobalInvocationID.x;

    data[index] = index;
}
```

C tarafında ilgili buffer binding'i yapıldıktan sonra:

```c
glUseProgram(computeProgram);

glDispatchCompute(4, 1, 1);
```

çağrısı yapılabilir.

Bu örnekte:

```text
4 work group
×
4 local invocation
=
16 invocation
```

oluşur.

Sonuç olarak buffer içerisindeki ilk 16 eleman compute shader tarafından işlenebilir.

Compute shader'ın yazdığı kaynakların daha sonraki OpenGL ES işlemleri tarafından doğru şekilde görülmesi gerektiğinde uygun memory barrier mekanizmaları kullanılmalıdır.

Örneğin:

```c
glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
```

---

## 13. Compute Shader Execution

`glDispatchCompute` çağrısı compute work group'larını dispatch eder.

Örneğin:

```c
glDispatchCompute(4, 2, 1);
```

için:

```text
X = 4
Y = 2
Z = 1
```

olmak üzere toplam:

```text
4 × 2 × 1 = 8 work group
```

dispatch edilir.

Eğer shader:

```glsl
layout(
    local_size_x = 8,
    local_size_y = 4,
    local_size_z = 1
) in;
```

şeklinde tanımlanmışsa:

```text
8 × 4 × 1 = 32 invocation / work group
```

bulunur.

Toplam invocation sayısı:

```text
8 work group × 32 invocation
= 256 invocation
```

olur.

---

## 14. Hata Durumları

`glDispatchCompute` için temel hata durumları aşağıdaki gibidir:

| Hata                     | Koşul                                                                              |
| ------------------------ | ----------------------------------------------------------------------------------- |
| `GL_INVALID_OPERATION` | Geçerli aktif compute shader programı bulunmaması                                |
| `GL_INVALID_VALUE`     | `num_groups_x` değerinin X maksimumunu aşması                                  |
| `GL_INVALID_VALUE`     | `num_groups_y` değerinin Y maksimumunu aşması                                  |
| `GL_INVALID_VALUE`     | `num_groups_z` değerinin Z maksimumunu aşması                                  |
| Hata yok                 | Geçerli aktif compute shader programı ve geçerli work group count kullanılması |
| Hata yok                 | Herhangi bir dispatch dimension'ının sıfır olması                              |

---

## 15. Work Group Count ve Work Group Size Farkı

`glDispatchCompute` ile belirlenen değer ile GLSL ES 3.10 shader'da belirlenen değer birbirinden farklıdır.

### Work Group Count

C tarafında belirlenir:

```c
glDispatchCompute(4, 2, 1);
```

Sonuç:

```text
4 × 2 × 1 = 8 work group
```

### Work Group Size

GLSL ES 3.10 tarafında belirlenir:

```glsl
layout(
    local_size_x = 8,
    local_size_y = 4,
    local_size_z = 1
) in;
```

Sonuç:

```text
8 × 4 × 1 = 32 invocation / work group
```

Bu durumda toplam invocation:

```text
8 work group × 32 invocation
= 256 invocation
```

olur.

Bu nedenle:

```text
Work Group Count
```

ve:

```text
Work Group Size
```

aynı kavramlar değildir.

---

## 16. Temel Kullanım Akışı

`glDispatchCompute` kullanımında genel işlem sırası aşağıdaki gibidir:

```text
OpenGL ES 3.1 Context
          |
          v
Compute Shader oluştur
          |
          v
GLSL ES 3.10 ile compile et
          |
          v
Program oluştur
          |
          v
Programı link et
          |
          v
glUseProgram()
          |
          v
Buffer / Image / Resource Binding
          |
          v
glDispatchCompute()
          |
          v
Compute Shader Execution
          |
          v
Gerekliyse glMemoryBarrier()
          |
          v
Sonuçları doğrula
```

---

## 17. Örnek

Aşağıdaki örnekte 4 work group dispatch edilmektedir:

```c
glUseProgram(computeProgram);

glDispatchCompute(4, 1, 1);

glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
```

Compute shader:

```glsl
#version 310 es

layout(
    local_size_x = 4,
    local_size_y = 1,
    local_size_z = 1
) in;

layout(std430, binding = 0) buffer OutputBuffer
{
    uint data[];
};

void main()
{
    uint index = gl_GlobalInvocationID.x;

    data[index] = index;
}
```

Bu durumda:

```text
Work Group Count
= 4 × 1 × 1
= 4 work group
```

ve:

```text
Local Work Group Size
= 4 × 1 × 1
= 4 invocation
```

olur.

Toplam:

```text
4 × 4 = 16 invocation
```

çalıştırılır.

Her invocation kendi `gl_GlobalInvocationID.x` değerine göre buffer içerisindeki farklı bir elemana erişir.

---

## 18. Önemli Noktalar

`glDispatchCompute` doğrudan invocation sayısını belirleyen bir fonksiyon değildir.

Fonksiyon:

```c
glDispatchCompute(X, Y, Z);
```

ile dispatch grid içerisindeki **work group count** belirlenir.

Work group içerisindeki invocation sayısı ise GLSL ES 3.10 compute shader içerisindeki:

```glsl
layout(
    local_size_x = X,
    local_size_y = Y,
    local_size_z = Z
) in;
```

değerleri ile belirlenir.

Dolayısıyla toplam invocation sayısı bu iki seviyenin birlikte değerlendirilmesiyle belirlenir.

---

## 19. OpenGL ES 3.1 ve GLSL ES 3.10 Kapsamı

Bu doküman:

* OpenGL ES 3.1
* GLSL ES 3.10
* Compute Shader
* `glDispatchCompute`

kapsamındadır.

OpenGL 4.3 Core Profile'a özgü requirement veya davranışlar bu dokümanın kapsamına dahil değildir.

Özellikle bu dokümanda kullanılan shader sürümü:

```glsl
#version 310 es
```

olmalıdır.

Bu ifade **GLSL ES 3.10** shader dilini belirtir ve desktop OpenGL GLSL 3.10 ile aynı şey değildir.

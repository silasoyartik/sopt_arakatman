# OpenGL: Temelden Compute Shader'a Detaylı Çalışma Notları

> Bu doküman, daha önce yaptığımız **GLFW + GLAD ile pencereli OpenGL**,
> **VAO/VBO**, **Vertex Shader / Fragment Shader**, shader dosyalarının
> ayrılması, `shader.c` / `shader.h` ile modülerleştirme, ardından **EGL
> / windowless rendering** ve son olarak **Compute Shader** konularına
> geçişimizi tek bir yerde toplamak için hazırlanmıştır.
>
> Amaç yalnızca "hangi kodu yazıyoruz?" sorusunu değil; **uygulama
> başlatıldığında CPU, OpenGL, driver ve GPU tarafında hangi olayların
> hangi sırayla gerçekleştiğini** anlamaktır.

---

# 1. Büyük Resim: OpenGL Nedir?

## 1.1 OpenGL'in temel tanımı

**OpenGL (Open Graphics Library)**, grafik üretmek ve GPU'nun grafik
yeteneklerini kullanmak için tanımlanmış bir **API specification'ıdır**.

Burada önemli nokta şudur:

**OpenGL doğrudan bir programlama dili değildir.**

Ayrıca OpenGL:

- GLFW değildir.
- GLAD değildir.
- GPU driver'ı değildir.
- GLSL değildir.
- Bir pencere sistemi değildir.

OpenGL bize grafik işlemlerini gerçekleştirmek için kullanılabilecek
fonksiyonları, nesneleri, durumları ve davranışları tanımlar.

Örneğin:

```c
glGenBuffers(...);
glBindBuffer(...);
glBufferData(...);
glUseProgram(...);
glDrawArrays(...);
```

gibi fonksiyonlar OpenGL API'nin parçalarıdır.

Basitleştirilmiş yapı:

```text
C/C++ Application
        |
        v
    OpenGL API
        |
        v
   GPU Driver
        |
        v
       GPU
```

Uygulamamız OpenGL fonksiyonlarını çağırır. Driver bu komutların donanım
üzerinde nasıl gerçekleştirileceğini yönetir.

---

# 2. OpenGL Bir Kütüphane mi, Specification mı?

OpenGL'i öğrenirken önemli ayrımlardan biri budur.

OpenGL esas olarak Khronos Group tarafından tanımlanan bir
**specification**dır.

Specification şunları tanımlar:

- Hangi fonksiyonların bulunduğunu,
- Fonksiyonların hangi parametreleri aldığını,
- OpenGL nesnelerinin davranışını,
- Rendering pipeline'ın davranışını,
- Hataların hangi durumlarda oluşacağını,
- Shader sisteminin kurallarını,
- OpenGL implementasyonunun hangi davranışları sağlaması gerektiğini.

GPU üreticileri ve driver geliştiricileri bu specification'a uygun
implementasyonlar sağlar.

Bu yüzden:

```text
OpenGL Specification
        |
        v
Driver Implementation
        |
        v
GPU Hardware
```

şeklinde düşünmek yararlıdır.

---

# 3. GLFW Nedir?

**GLFW**, OpenGL'in kendisi değildir.

GLFW bizim özellikle masaüstü uygulamalarımızda şu işleri kolaylaştırır:

- Pencere oluşturmak,
- OpenGL context oluşturmak,
- Klavye/fare inputlarını yönetmek,
- Pencere boyutu ve event işlemlerini yönetmek,
- Platforma bağlı bazı ayrıntıları soyutlamak.

Örneğin:

```c
glfwInit();
```

GLFW sistemini başlatır.

```c
glfwCreateWindow(...);
```

bir pencere oluşturabilir ve istediğimiz API için context oluşturma
sürecini gerçekleştirir.

```c
glfwMakeContextCurrent(window);
```

oluşturulan OpenGL context'i mevcut thread için current hale getirir.

## 3.1 OpenGL Context nedir?

Context, OpenGL'in çalışabilmesi için gerekli olan durum ve kaynakların
ilişkilendirildiği çalışma ortamıdır.

Basit düşünürsek:

> OpenGL komutlarını kullanacağımız aktif çalışma ortamı.

Context olmadan normal OpenGL çağrılarını yapmaya başlayamayız.

Context içerisinde/ile ilişkili olarak OpenGL state'i ve birçok OpenGL
nesnesi bulunur.

---

# 4. GLAD Nedir?

**GLAD bir OpenGL function loader'dır.**

GLFW ile context oluşturmuş olmamız, modern OpenGL fonksiyonlarını C
kodunda otomatik olarak güvenli biçimde kullanabileceğimiz anlamına
gelmez.

Modern OpenGL fonksiyonlarının adreslerinin çalışma zamanında yüklenmesi
gerekir.

GLAD burada devreye girer.

Mantık:

```text
GLFW
 |
 +--> Window
 |
 +--> OpenGL Context

Context hazır
 |
 v
GLAD
 |
 +--> OpenGL function pointer'larını yükler
 |
 v
glGenBuffers()
glCreateShader()
glCreateProgram()
...
```

Dolayısıyla:

```text
GLFW = Window + Context yönetiminde yardımcı kütüphane

GLAD = OpenGL fonksiyonlarını yükleyen loader

OpenGL = Grafik/compute API specification'ı
```

Bu üç kavram birbirinden ayrılmalıdır.

---

# 5. GLSL Nedir?

**GLSL = OpenGL Shading Language**

GPU üzerinde çalışacak shader programlarını yazmak için kullanılan
dildir.

Örneğin C tarafındaki programımız CPU üzerinde çalışırken:

```c
glDrawArrays(...);
```

gibi OpenGL komutları verir.

Shader kodumuz ise GPU tarafındaki programlanabilir shader aşamalarında
çalışır.

Örneğin:

```glsl
#version 430 core

void main()
{
    ...
}
```

bir GLSL shader kodudur.

Yani iki farklı taraf vardır:

```text
CPU SIDE
C / C++
OpenGL API

        |

        v

GPU SIDE
GLSL Shader Programs
```

---

# 6. Shader Nedir?

**Shader**, GPU üzerinde çalışan küçük programlara verilen genel
isimdir.

OpenGL'de farklı shader aşamaları bulunabilir:

- Vertex Shader
- Tessellation Control Shader
- Tessellation Evaluation Shader
- Geometry Shader
- Fragment Shader
- Compute Shader

Biz başlangıçta iki temel shader ile çalıştık:

```text
Vertex Shader
Fragment Shader
```

Daha sonra Compute Shader'a geçiyoruz.

---

# 7. Klasik OpenGL Graphics Pipeline

Basitleştirilmiş klasik pipeline:

```text
Vertex Data
    |
    v
Vertex Shader
    |
    v
Primitive Assembly
    |
    v
Rasterization
    |
    v
Fragment Shader
    |
    v
Per-Fragment Operations
    |
    v
Framebuffer
```

Bu zincirin temel amacı:

> Veriden ekranda görüntü üretmek.

Compute Shader'a geçtiğimizde bu modelden önemli ölçüde ayrılacağız.

---

# 8. Vertex Nedir?

**Vertex**, geometrik verinin temel elemanlarından biridir.

Örneğin üçgen:

```text
        V1
       /  \
      /    \
     /      \
   V0--------V2
```

üç vertex kullanılarak tanımlanabilir.

Bir vertex yalnızca pozisyon olmak zorunda değildir.

Vertex verisi içerisinde örneğin:

```text
Position
Color
Normal
Texture Coordinate
```

gibi attribute'lar bulunabilir.

Örnek:

```c
float vertices[] = {
    // position
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};
```

Burada üç vertex bulunmaktadır.

Her vertex:

```text
x, y, z
```

koordinatına sahiptir.

---

# 9. VBO Nedir?

**VBO = Vertex Buffer Object**

Vertex verilerini GPU/OpenGL tarafından kullanılabilecek buffer
storage'a aktarmak için kullandığımız OpenGL buffer nesnesidir.

CPU tarafında başlangıçta:

```c
float vertices[] = {...};
```

vardır.

Bu veri uygulamanın CPU memory'sindedir.

Rendering sırasında OpenGL'in kullanabilmesi için buffer oluştururuz.

Örneğin:

```c
GLuint VBO;
glGenBuffers(1, &VBO);
```

Burada OpenGL'den bir buffer object adı isteriz.

Ardından:

```c
glBindBuffer(GL_ARRAY_BUFFER, VBO);
```

ile buffer'ı `GL_ARRAY_BUFFER` binding noktasına bağlarız.

Sonra:

```c
glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(vertices),
    vertices,
    GL_STATIC_DRAW
);
```

ile veriyi buffer storage'a aktarırız.

Büyük resim:

```text
CPU Array

vertices[]
    |
    | glBufferData()
    v

OpenGL Buffer Storage
(VBO)
```

## VBO neden var?

Çünkü GPU'nun işleyeceği vertex verilerinin OpenGL tarafından yönetilen
bir kaynakta bulunması gerekir.

---

# 10. VAO Nedir?

**VAO = Vertex Array Object**

VAO'nun görevi doğrudan vertex verisini saklamak değildir.

VAO temel olarak vertex input configuration state'ini tutar.

Örneğin buffer'daki veri şöyle olsun:

```text
X Y Z   X Y Z   X Y Z
```

GPU'nun bilmesi gereken sorular vardır:

- Bir vertex kaç bileşenden oluşuyor?
- Her bileşenin tipi nedir?
- Bir sonraki vertex'e kaç byte sonra geçiliyor?
- Shader'daki hangi input attribute'u bu veriyi okuyacak?
- Hangi vertex buffer binding/configuration kullanılacak?

Bu bilgiler `glVertexAttribPointer()` ve ilgili vertex array state'i ile
tanımlanır.

Örneğin:

```c
glVertexAttribPointer(
    0,
    3,
    GL_FLOAT,
    GL_FALSE,
    3 * sizeof(float),
    (void*)0
);
```

Ardından:

```c
glEnableVertexAttribArray(0);
```

ile attribute aktif edilir.

VAO bu vertex input configuration'ının önemli kısmını saklar.

Basitleştirilmiş ayrım:

```text
VBO
|
+--> Vertex DATA

VAO
|
+--> Vertex data nasıl yorumlanacak?
```

Bu ayrım çok önemlidir.

---

# 11. Attribute Nedir?

Shader açısından **attribute**, her vertex için sağlanan input
verilerinden biridir.

Örneğin:

```glsl
layout (location = 0) in vec3 aPos;
```

Burada:

```text
aPos
```

Vertex Shader'ın bir input attribute'udur.

C tarafında:

```c
glVertexAttribPointer(0, ...);
```

ile `location = 0` için veri düzenini tanımlarız.

Shader tarafında:

```glsl
layout(location = 0) in vec3 aPos;
```

ile aynı location'dan veriyi alırız.

Bağlantı:

```text
VBO
 |
 | vertex data
 v
VAO / Vertex Attribute Configuration
 |
 | location = 0
 v
Vertex Shader
 |
 v
aPos
```

---

# 12. Vertex Shader Nedir?

Vertex Shader, vertex processing aşamasında çalışan shader'dır.

Tipik minimum örnek:

```glsl
#version 330 core

layout(location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
```

Burada:

```glsl
layout(location = 0)
```

input attribute'un location numarasını belirtir.

```glsl
in vec3 aPos;
```

shader'a gelen vertex attribute verisini tanımlar.

```glsl
gl_Position
```

Vertex Shader'ın önemli built-in output'larından biridir.

Vertex Shader her işlenen vertex için invocation oluşturacak şekilde
çalışır.

Örneğin üç vertex varsa kavramsal olarak:

```text
Vertex 0 --> Vertex Shader invocation
Vertex 1 --> Vertex Shader invocation
Vertex 2 --> Vertex Shader invocation
```

---

# 13. Primitive Assembly Nedir?

Vertex Shader aşamasından sonra vertex'ler primitive'ler halinde
yorumlanır.

Örneğin:

```c
glDrawArrays(GL_TRIANGLES, 0, 3);
```

dediğimizde OpenGL'e:

> Bu vertex'leri triangle primitive'leri olarak işle.

demiş oluruz.

Üç vertex:

```text
V0
V1
V2
```

bir triangle oluşturabilir.

Bu aşama primitive assembly mantığının parçasıdır.

---

# 14. Rasterization Nedir?

Rasterization, geometrik primitive'i fragment'lere dönüştüren aşamadır.

Örneğin:

```text
       /\
      /  \
     /    \
    /______\
```

bir triangle olsun.

Ekran ise ayrık pixel/sample bölgelerinden oluşur.

Rasterizer triangle'ın hangi bölgeleri kapsadığını belirleyerek
fragment'ler üretir.

Basitleştirilmiş:

```text
Geometry
   |
   v
Rasterization
   |
   v
Fragments
```

---

# 15. Fragment Nedir?

Fragment doğrudan "pixel" ile tamamen aynı kavram değildir.

Fragment:

> Rasterization sonucunda oluşan ve framebuffer'daki bir sample/pixel
> konumuna katkıda bulunabilecek aday veri kümesidir.

Fragment üzerinde:

- renk,
- depth,
- stencil,
- discard,
- blending

gibi süreçler etkili olabilir.

Bu yüzden:

```text
Fragment == kesin olarak son ekrandaki pixel
```

demek teknik olarak doğru değildir.

---

# 16. Fragment Shader Nedir?

Fragment Shader, rasterization tarafından üretilen fragment'ler üzerinde
çalışan shader aşamasıdır.

Örneğin:

```glsl
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
```

Bu shader kırmızı bir renk çıktısı üretmektedir.

Kavramsal olarak:

```text
Rasterization
     |
     +--> Fragment 0 --> Fragment Shader
     +--> Fragment 1 --> Fragment Shader
     +--> Fragment 2 --> Fragment Shader
     +--> ...
```

GPU'nun paralel çalışma yapısı burada açık şekilde görülür.

---

# 17. Shader Object ve Shader Program Arasındaki Fark

Önemli ayrım:

```text
Shader Object
```

tek bir shader stage'in compile edilmiş temsilidir.

Örneğin:

```c
GLuint vertexShader =
    glCreateShader(GL_VERTEX_SHADER);
```

Vertex Shader object oluşturur.

Fragment için:

```c
GLuint fragmentShader =
    glCreateShader(GL_FRAGMENT_SHADER);
```

Ardından shader source verilir:

```c
glShaderSource(...);
```

ve compile edilir:

```c
glCompileShader(...);
```

Ancak tek başına shader object normal draw sırasında kullanılacak tam
executable pipeline programı değildir.

Shader'ları program object'e attach ederiz:

```c
glAttachShader(program, vertexShader);
glAttachShader(program, fragmentShader);
```

sonra:

```c
glLinkProgram(program);
```

ile program link edilir.

Sonuç:

```text
Vertex Shader Object
       \
        \
         --> Shader Program
        /
       /
Fragment Shader Object
```

Rendering sırasında:

```c
glUseProgram(program);
```

ile program aktif hale getirilir.

---

# 18. Compile ve Link Arasındaki Fark

## Compile

Shader source'unun geçerli olup olmadığını ve implementasyon tarafından
shader executable representation'a dönüştürülebilirliğini kontrol eder.

```text
GLSL Source
    |
    v
Compile
    |
    v
Shader Object
```

## Link

Bir program içindeki shader stage'lerinin birbirleriyle uyumlu biçimde
bir program executable oluşturmasını sağlar.

```text
Vertex Shader
      +
Fragment Shader
      |
      v
    Link
      |
      v
Shader Program
```

Compile başarılı olsa bile link başarısız olabilir.

Bu nedenle ikisinin hata kontrolleri ayrı yapılmalıdır.

---

# 19. Neden Shader Dosyalarını Ayrı Tuttuk?

İlk örneklerde shader source doğrudan C string olarak yazılabilir:

```c
const char *vertexShaderSource = "...";
```

Ancak proje büyüdükçe bu yaklaşım kötüleşir.

Bu nedenle bizim yapımızda:

```text
~/HAVELSAN/shaders/
```

altında:

```text
vertex_shader.glsl
fragment_shader.glsl
```

dosyaları bulunmaktadır.

Avantajları:

- C kodu temiz kalır.
- Shader kodu ayrı düzenlenebilir.
- GLSL syntax highlighting daha düzgün çalışır.
- Shader değişiklikleri yönetilebilir.
- Proje modüler olur.
- İleride Compute Shader için `compute_shader.glsl` eklemek
  kolaylaşır.

---

# 20. shader.h Nedir?

Bizim projemizde `shader.h`, shader yardımcı fonksiyonlarının
**deklarasyonlarını** diğer C dosyalarına açar.

Örneğin:

```c
#ifndef SHADER_H
#define SHADER_H

unsigned int createShaderProgram(
    const char *vertexPath,
    const char *fragmentPath
);

#endif
```

Bu dosya:

```text
include/shader.h
```

altında bulunabilir.

`main.c`:

```c
#include "shader.h"
```

dediğinde shader modülünün sunduğu fonksiyonları bilir.

---

# 21. shader.c Nedir?

`shader.c`, `shader.h` içerisinde tanımlanan interface'in
implementasyonunu içerir.

Burada örneğin:

1. Shader dosyasını açma,
2. Dosyanın boyutunu öğrenme,
3. Memory ayırma,
4. Shader source'u okuma,
5. `glCreateShader`,
6. `glShaderSource`,
7. `glCompileShader`,
8. Compile status kontrolü,
9. Program oluşturma,
10. Shader'ları attach etme,
11. Link etme,
12. Link status kontrolü,
13. Gereksiz shader object'lerini silme

işlemleri yapılabilir.

Böylece `main.c` içerisinde bütün shader ayrıntılarını tekrar yazmak
zorunda kalmayız.

---

# 22. Bizim Proje Yapımız

Tercih ettiğimiz yapı:

```text
~/HAVELSAN/
|
├── src/
│   ├── main.c
│   └── shader.c
|
├── include/
│   └── shader.h
|
├── shaders/
│   ├── vertex_shader.glsl
│   └── fragment_shader.glsl
|
└── build/
```

İleride Compute Shader eklediğimizde örneğin:

```text
shaders/
├── vertex_shader.glsl
├── fragment_shader.glsl
└── compute_shader.glsl
```

olabilir.

Compute testlerini ayrı modüllere ayırmak istersek yapı daha sonra
genişletilebilir.

---

# 23. CMake Neden Kullanıyoruz?

CMake doğrudan compiler değildir.

CMake bir **build system generator/configuration aracıdır**.

Projemizin:

- hangi source dosyalarını içerdiğini,
- hangi include dizinlerini kullandığını,
- hangi library'lere link edileceğini,
- executable'ın nasıl oluşturulacağını

tanımlamamıza yardım eder.

Örneğin kavramsal olarak:

```text
CMakeLists.txt
      |
      v
CMake configure
      |
      v
Build system
      |
      v
Compiler + Linker
      |
      v
Executable
```

Biz kaynak klasörünü temiz tutmak için:

```text
build/
```

klasörünü kullanıyoruz.

Tipik kullanım:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

---

# 24. Baştan Sona Pencereli OpenGL Programı Çalışırken Ne Olur?

Şimdi en önemli bölümlerden birine gelelim.

Tam bir program yazdığımızı varsayalım.

Program başladığında gerçekleşen olayları sırayla inceleyelim.

---

## ADIM 1 --- İşletim sistemi executable'ı başlatır

Program:

```text
./opengl_app
```

şeklinde çalıştırılır.

İşletim sistemi executable'ı process olarak başlatır.

C programında kontrol:

```c
int main(void)
```

fonksiyonuna gelir.

---

## ADIM 2 --- GLFW initialize edilir

```c
glfwInit();
```

GLFW kendi platform işlemlerini başlatır.

Henüz triangle çizmiş değiliz.

Henüz VBO oluşturmuş değiliz.

Henüz shader çalıştırmış değiliz.

Bu sadece GLFW ortamının hazırlanmasıdır.

---

## ADIM 3 --- OpenGL Context özellikleri istenir

Örneğin:

```c
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
```

ile belirli bir OpenGL context sürümü isteyebiliriz.

Compute Shader core kullanımında OpenGL 4.3 kritik sürümdür.

Burada önemli nokta:

> GLFW OpenGL 4.3'ü bize "sağlamaz".

GLFW sadece driver/platform üzerinden böyle bir context oluşturmaya
çalışır.

Gerçek destek:

```text
GPU + Driver + Platform
```

tarafına bağlıdır.

---

## ADIM 4 --- Window ve Context oluşturulur

```c
GLFWwindow *window =
    glfwCreateWindow(...);
```

ile pencere/context oluşturma süreci gerçekleştirilir.

Basitleştirilmiş:

```text
Application
    |
    v
GLFW
    |
    +--> Native Window
    |
    +--> OpenGL Context
```

---

## ADIM 5 --- Context current yapılır

```c
glfwMakeContextCurrent(window);
```

OpenGL context mevcut thread için aktif hale getirilir.

Bu adım çok önemlidir.

GLAD'i context oluşmadan önce yüklemeye çalışmak mantıksal olarak
yanlıştır çünkü OpenGL fonksiyon adreslerini alacağımız geçerli bir
context ortamına ihtiyaç duyarız.

---

## ADIM 6 --- GLAD OpenGL fonksiyonlarını yükler

Örneğin kullanılan GLAD sürümüne göre:

```c
gladLoadGLLoader(...);
```

çağrılabilir.

Bu aşamada GLAD modern OpenGL fonksiyonlarının adreslerini yükler.

Artık:

```c
glGenBuffers
glBindVertexArray
glCreateShader
glCreateProgram
...
```

gibi fonksiyonları kullanabiliriz.

---

## ADIM 7 --- OpenGL sürümü kontrol edilir

Örneğin:

```c
glGetString(GL_VERSION);
```

ile runtime OpenGL version bilgisi kontrol edilebilir.

Compute Shader çalışmasında bunu özellikle yapacağız.

Çünkü:

```text
"GLFW kurulu"
```

demek:

```text
"Compute Shader destekleniyor"
```

anlamına gelmez.

---

# 25. Shader Hazırlama Süreci

Program açıldıktan sonra shader programımız hazırlanır.

## 25.1 Shader dosyası okunur

```text
shaders/vertex_shader.glsl
```

C tarafında açılır.

İçerik RAM'e alınır.

Aynı işlem:

```text
shaders/fragment_shader.glsl
```

için yapılır.

---

## 25.2 Shader object oluşturulur

```c
glCreateShader(GL_VERTEX_SHADER);
```

ve:

```c
glCreateShader(GL_FRAGMENT_SHADER);
```

kullanılır.

---

## 25.3 Source OpenGL'e verilir

```c
glShaderSource(...);
```

---

## 25.4 Shader compile edilir

```c
glCompileShader(...);
```

---

## 25.5 Compile sonucu kontrol edilir

```c
glGetShaderiv(...);
```

ve hata durumunda:

```c
glGetShaderInfoLog(...);
```

kullanılır.

Bu adımı atlamamak gerekir.

---

## 25.6 Program object oluşturulur

```c
glCreateProgram();
```

---

## 25.7 Shader'lar attach edilir

```c
glAttachShader(...);
```

---

## 25.8 Program link edilir

```c
glLinkProgram(...);
```

---

## 25.9 Link sonucu kontrol edilir

Program link log'u kontrol edilir.

---

## 25.10 Artık shader program kullanılabilir

```c
glUseProgram(program);
```

---

# 26. Vertex Data Hazırlama Süreci

CPU tarafında:

```c
float vertices[] = {
    ...
};
```

oluşturulur.

Sonra VBO oluşturulur.

```text
CPU vertices[]
      |
      | glBufferData
      v
     VBO
```

VAO oluşturulur ve bind edilir.

Vertex attribute layout'u tanımlanır.

```text
VBO Data
   |
   v
VAO Configuration
   |
   v
Vertex Shader Input
```

---

# 27. Render Loop Nedir?

Pencere açık olduğu sürece program genellikle bir loop içerisinde
çalışır.

Kavramsal:

```c
while (!glfwWindowShouldClose(window))
{
    // input
    // clear
    // shader program
    // bind VAO
    // draw
    // swap buffers
    // process events
}
```

Her frame'de rendering işlemleri tekrar yapılır.

---

# 28. Draw Call Verildiğinde Ne Olur?

Örneğin:

```c
glDrawArrays(GL_TRIANGLES, 0, 3);
```

çağrılır.

Bu komut rendering pipeline'ın çalışmasını tetikler.

Basitleştirilmiş zincir:

```text
glDrawArrays()
      |
      v
Vertex Fetch
      |
      v
Vertex Shader
      |
      v
Primitive Assembly
      |
      v
Rasterization
      |
      v
Fragment Shader
      |
      v
Per-Fragment Operations
      |
      v
Framebuffer
```

---

# 29. Vertex Fetch

OpenGL VAO'daki vertex input configuration'a bakar.

VBO'dan vertex verilerini alır.

Örneğin:

```text
Vertex 0 = (-0.5, -0.5, 0)
Vertex 1 = ( 0.5, -0.5, 0)
Vertex 2 = ( 0.0,  0.5, 0)
```

---

# 30. Vertex Shader Invocation'ları

Her vertex işlenirken Vertex Shader invocation'ları oluşur.

```text
Vertex 0 ---> VS invocation
Vertex 1 ---> VS invocation
Vertex 2 ---> VS invocation
```

Vertex Shader örneğin:

```glsl
gl_Position = vec4(aPos, 1.0);
```

üretir.

---

# 31. Primitive Assembly

Üç vertex:

```text
V0 + V1 + V2
```

triangle olarak yorumlanır.

---

# 32. Rasterization

Triangle ekran/sample uzayında kapsadığı alanlara göre fragment'lere
dönüştürülür.

```text
Triangle
   |
   v
Fragments
```

---

# 33. Fragment Shader Invocation'ları

Rasterizer tarafından oluşturulan uygun fragment'ler için Fragment
Shader çalışır.

```text
Fragment 0 --> FS
Fragment 1 --> FS
Fragment 2 --> FS
...
```

Fragment Shader renk gibi output değerleri hesaplar.

---

# 34. Framebuffer

Sonuçlar gerekli testler ve per-fragment işlemlerinden sonra
framebuffer'a katkıda bulunur.

Framebuffer'ı basitçe:

> Render edilen görüntünün color/depth/stencil gibi attachment'larının
> bulunduğu hedef

olarak düşünebiliriz.

Pencereli rendering'de sonuç sonunda pencerenin görüntülenen buffer'ında
gösterilebilir.

---

# 35. Double Buffering ve Swap Buffers

Genellikle ekranda gösterilen buffer ile yeni frame'in çizildiği buffer
ayrıdır.

Basit model:

```text
Front Buffer
--> Kullanıcı bunu görüyor

Back Buffer
--> Yeni frame buraya çiziliyor
```

Frame tamamlandığında:

```c
glfwSwapBuffers(window);
```

ile buffer'ların görüntüleme rolü değiştirilir.

Bu yöntem tearing/flicker gibi problemlerin yönetilmesine yardımcı olur.

---

# 36. Event Processing

```c
glfwPollEvents();
```

ile pencere sistemi event'leri işlenir.

Örneğin:

- Klavye
- Mouse
- Pencere kapatma
- Resize

gibi olaylar.

---

# 37. Program Kapanırken Cleanup

Oluşturduğumuz OpenGL kaynaklarını sileriz.

Örneğin:

```c
glDeleteVertexArrays(...);
glDeleteBuffers(...);
glDeleteProgram(...);
```

Sonra pencere/context tarafı kapatılır ve GLFW terminate edilir.

Bu kaynak yaşam döngüsünü anlamak önemlidir:

```text
CREATE
  |
USE
  |
DELETE
```

---

# 38. Baştan Sona Tam Akış

Tüm klasik örneği tek diagramda gösterirsek:

```text
PROGRAM START
    |
    v
glfwInit()
    |
    v
Context hints
    |
    v
glfwCreateWindow()
    |
    v
OpenGL Context
    |
    v
glfwMakeContextCurrent()
    |
    v
GLAD load
    |
    v
OpenGL version check
    |
    +----------------------+
    |                      |
    v                      v
Shader Files          Vertex Array
    |                      |
    v                      v
Read GLSL             Create VAO
    |                      |
    v                      v
Create Shader         Create VBO
    |                      |
    v                      v
Compile               Upload Data
    |                      |
    v                      v
Link Program          Attribute Config
    |                      |
    +-----------+----------+
                |
                v
           Render Loop
                |
                v
          glUseProgram()
                |
                v
           Bind VAO
                |
                v
         glDrawArrays()
                |
                v
         Vertex Shader
                |
                v
       Primitive Assembly
                |
                v
         Rasterization
                |
                v
        Fragment Shader
                |
                v
          Framebuffer
                |
                v
       glfwSwapBuffers()
                |
                v
        glfwPollEvents()
                |
                +---- repeat
                |
                v
             Cleanup
                |
                v
          PROGRAM END
```

---

# 39. OpenGL ile EGL Arasındaki İlişki

Daha sonra pencereli GLFW yaklaşımından daha düşük seviyeli/windowless
yapıya geçtik.

Burada önemli ayrım:

```text
OpenGL
```

rendering API'dir.

```text
EGL
```

ise client rendering API ile native platform/window system arasında
context, surface ve display gibi bağlantıları yönetmek için kullanılan
arayüzdür.

Basitleştirilmiş:

```text
Native Platform
      |
      v
     EGL
      |
      v
OpenGL ES / uygun client API
      |
      v
    Driver
      |
      v
     GPU
```

GLFW örneğinde GLFW bizim için context/window yönetiminin önemli kısmını
soyutluyordu.

EGL çalışmasında bu yapıların daha alt seviyesini anlamaya başladık.

---

# 40. EGLDisplay

`EGLDisplay`, EGL'in native display/platform bağlantısını temsil eden
handle'dır.

Bu doğrudan fiziksel monitör demek değildir.

EGL'in belirli native display ortamıyla iletişim kuracağı bağlantı
olarak düşünülmelidir.

---

# 41. EGLConfig

`EGLConfig`, oluşturulacak surface/context ortamı için uygun
framebuffer/configuration özelliklerini temsil eder.

Örneğin seçilecek config:

- renk channel boyutları,
- depth özellikleri,
- surface türü,
- desteklenen rendering API özellikleri

gibi kriterlerle ilişkilidir.

---

# 42. EGLSurface

`EGLSurface`, rendering işleminin hedefi olabilecek EGL surface
nesnesidir.

Surface türüne göre:

- window surface,
- pixmap surface,
- pbuffer surface

gibi modeller bulunabilir.

Windowless çalışmalarda surface yaklaşımı kullanılan yönteme göre
değişebilir.

---

# 43. EGLContext

`EGLContext`, client API'nin çalışacağı context'tir.

OpenGL/OpenGL ES state ve kaynak yönetiminin bağlandığı çalışma
ortamının temel parçalarından biridir.

---

# 44. GBM Neden İncelendi?

**GBM = Generic Buffer Management**

Linux graphics stack içerisinde Mesa/DRM ekosisteminde buffer ve native
platform entegrasyonu için kullanılan yapılardan biridir.

Window system olmadan EGL ile çalışırken:

```text
DRM / Device
      |
      v
     GBM
      |
      v
     EGL
      |
      v
OpenGL ES / OpenGL
```

gibi bir yapı kullanılabilir.

Bu konu klasik GLFW üçgeninden daha düşük seviyelidir.

---

# 45. EGL 1.0 Kuralımız

EGL/GBM tarafındaki örneklerimizde mümkün olduğunca EGL 1.0 ile uyumlu
temel API'leri kullanmaya dikkat ediyoruz.

Daha yeni EGL extension'ı veya daha yeni core fonksiyon gerektiğinde
bunun özellikle belirtilmesi gerekir.

Ancak bu kuralı modern:

```text
GLFW + GLAD + OpenGL Compute Shader
```

çalışmasına zorla uygulamıyoruz.

Bunlar iki farklı çalışma bağlamıdır.

---

# 46. Compute Shader'a Neden Geçiyoruz?

Şimdi klasik grafik pipeline'dan farklı bir probleme geçiyoruz.

Klasik sistemin amacı:

```text
Geometry
   |
   v
Rasterization
   |
   v
Fragments
   |
   v
Image
```

Compute tarafındaki amacımız:

```text
Data
 |
 v
Parallel Computation
 |
 v
Data
```

Yani mutlaka görüntü üretmek istemiyoruz.

Örneğin:

```text
Input:
1 2 3 4 5

Operation:
x * 2

Output:
2 4 6 8 10
```

GPU'nun paralel işlem gücünü genel hesaplamalarda kullanmak istiyoruz.

---

# 47. Eski Ekibin Fragment Shader Yaklaşımı

Fragment Shader zaten çok sayıda fragment üzerinde paralel çalışır.

Bu nedenle özellikle Compute Shader öncesi GPGPU yaklaşımlarında
graphics pipeline genel amaçlı hesaplamaya uyarlanabiliyordu.

Kavramsal olarak:

```text
Input Data
    |
    v
Texture
    |
    v
Geometry / Draw
    |
    v
Rasterization
    |
    v
Fragment Shader
    |
    v
Output Texture
```

Fragment Shader içerisinde hesaplama yapılabilir.

Bu yaklaşım çalışabilir.

Ancak genel hesaplama için doğal bir execution model değildir.

Çünkü hesap yapmak için graphics pipeline'ın:

- primitive,
- draw,
- rasterization,
- fragment

mekanizmalarını kullanmak zorunda kalırız.

---

# 48. Compute Shader Nedir?

Compute Shader:

> GPU üzerinde genel amaçlı paralel hesaplama gerçekleştirmek için
> tasarlanmış shader stage'idir.

Desktop OpenGL'de Compute Shader core functionality OpenGL **4.3** ile
ilişkilidir.

Bu nedenle OpenGL 3.3 core context oluşturup Compute Shader'ın core
olarak kullanılacağını varsaymamalıyız.

Compute Shader için klasik olarak şunlara ihtiyaç yoktur:

```text
Vertex Shader      X
Fragment Shader    X
Triangle           X
Rasterization      X
```

Compute execution:

```text
CPU
 |
 v
OpenGL API
 |
 v
glDispatchCompute()
 |
 v
Work Groups
 |
 v
Shader Invocations
 |
 v
Parallel Computation
 |
 v
Memory / Buffers / Images
```

---

# 49. Compute Shader Graphics Pipeline'ın Neresindedir?

Compute Shader'ı:

```text
Vertex Shader
    |
Compute Shader
    |
Fragment Shader
```

şeklinde düşünmek YANLIŞTIR.

Compute Shader klasik graphics pipeline'ın vertex → rasterization →
fragment zincirinde bir aşama değildir.

Daha doğru zihinsel model:

```text
                 OpenGL
                   |
          +--------+--------+
          |                 |
          v                 v
Graphics Operations    Compute Dispatch
          |                 |
          v                 v
Vertex/Fragment        Compute Shader
          |                 |
          v                 v
Framebuffer          Buffer / Image / Memory
```

---

# 50. Invocation Nedir?

**Invocation**, shader kodunun tek bir mantıksal çalıştırılmasıdır.

Örneğin 8 veri elemanımız olsun:

```text
A[0]
A[1]
A[2]
A[3]
A[4]
A[5]
A[6]
A[7]
```

Her eleman için bir Compute Shader invocation kullanmayı
tasarlayabiliriz:

```text
Invocation 0 --> A[0]
Invocation 1 --> A[1]
Invocation 2 --> A[2]
Invocation 3 --> A[3]
Invocation 4 --> A[4]
Invocation 5 --> A[5]
Invocation 6 --> A[6]
Invocation 7 --> A[7]
```

Ancak:

```text
1 invocation = 1 fiziksel GPU core
```

demek doğru değildir.

Invocation mantıksal execution birimidir.

GPU bunları donanım mimarisine göre gruplar ve schedule eder.

---

# 51. Work Group Nedir?

Compute Shader invocation'ları **work group** adı verilen gruplar
içerisinde organize edilir.

Örneğin:

```text
Work Group 0
|
+-- Invocation 0
+-- Invocation 1
+-- Invocation 2
+-- Invocation 3
```

ve:

```text
Work Group 1
|
+-- Invocation 0
+-- Invocation 1
+-- Invocation 2
+-- Invocation 3
```

olabilir.

Burada toplam:

```text
2 work group
x
4 invocation/work group
=
8 invocation
```

vardır.

---

# 52. Local Size Nedir?

Shader içerisinde:

```glsl
layout(
    local_size_x = 4,
    local_size_y = 1,
    local_size_z = 1
) in;

￼

```

gibi bir tanım bulunabilir.

Bu:

> Bir work group içerisinde kaç invocation bulunduğunu

tanımlar.

Bu örnekte:

```text
4 x 1 x 1 = 4
```

invocation/work group vardır.

Örneğin:

```glsl
layout(
    local_size_x = 16,
    local_size_y = 16,
    local_size_z = 1
) in;
```

ise:

```text
16 x 16 x 1
=
256 invocation/work group
```

anlamına gelir.

---

# 53. `layout(...) in;` Ne Anlama Geliyor?

Compute Shader'da:

```glsl
layout(local_size_x = 16,
       local_size_y = 16,
       local_size_z = 1) in;
```

shader'ın work group local invocation boyutunu bildirir.

Buradaki `layout` bir layout qualifier kullanımının parçasıdır.

`local_size_x/y/z`, work group içindeki invocation grid'inin boyutlarını
belirtir.

Buradaki `in`, compute shader'ın input execution layout declaration
syntax'ının parçasıdır.

Bunu Vertex Shader'daki:

```glsl
in vec3 aPos;
```

ile birebir aynı anlamda yorumlamamak gerekir.

---

# 54. glDispatchCompute() Nedir?

CPU/OpenGL tarafında Compute Shader execution'ını başlatmak için:

```c
glDispatchCompute(x, y, z);
```

kullanılır.

Buradaki:

```text
x
y
z
```

**local size değildir.**

Bunlar dispatch edilen **work group sayısını** belirler.

Örneğin:

```c
glDispatchCompute(2, 1, 1);
```

ve shader:

```glsl
layout(local_size_x = 4) in;
```

ise:

```text
Work Group sayısı = 2

Her Work Group = 4 invocation

Toplam invocation = 2 x 4 = 8
```

olur.

Bu iki kavramı kesinlikle ayırmalıyız:

```text
LOCAL SIZE
=
bir work group içerisindeki invocation sayısı

DISPATCH SIZE
=
kaç work group oluşturulacağı
```

---

# 55. Üç Boyutlu Compute Modeli

Compute execution üç boyutlu olarak tanımlanabilir.

Örneğin:

```glsl
layout(
    local_size_x = 8,
    local_size_y = 8,
    local_size_z = 1
) in;
```

ve:

```c
glDispatchCompute(4, 3, 1);
```

olsun.

Bir work group:

```text
8 x 8 x 1
=
64 invocation
```

Dispatch edilen work group sayısı:

```text
4 x 3 x 1
=
12 work group
```

Toplam invocation:

```text
12 x 64
=
768 invocation
```

olur.

---

# 56. Compute Shader Built-in Değişkenleri

Özellikle öğreneceğimiz built-in'ler:

```glsl
gl_GlobalInvocationID
gl_LocalInvocationID
gl_WorkGroupID
gl_NumWorkGroups
gl_WorkGroupSize
```

Bunlar compute invocation'ın execution grid içerisindeki konumunu
anlamamızı sağlar.

---

# 57. gl_LocalInvocationID

Bir invocation'ın **kendi work group'u içerisindeki** ID'sidir.

Örneğin local size:

```text
4
```

ise bir work group içerisindeki local ID'ler:

```text
0
1
2
3
```

olabilir.

Sonraki work group'ta local ID tekrar:

```text
0
1
2
3
```

olur.

Çünkü local ID yalnızca grubun içindeki konumu ifade eder.

---

# 58. gl_WorkGroupID

Invocation'ın bulunduğu work group'un dispatch grid'indeki ID'sidir.

Örneğin:

```c
glDispatchCompute(2, 1, 1);
```

ise x ekseninde:

```text
Work Group 0
Work Group 1
```

bulunur.

Dolayısıyla:

```text
gl_WorkGroupID.x
```

0 veya 1 olabilir.

---

# 59. gl_GlobalInvocationID

Global invocation ID, invocation'ın tüm dispatch içerisindeki global
konumunu belirtir.

Temel ilişki:

```text
Global ID
=
Work Group ID * Work Group Size
+
Local Invocation ID
```

Örneğin:

```text
local_size_x = 4
work group sayısı = 2
```

olsun.

Work Group 0:

```text
Local 0 --> Global 0
Local 1 --> Global 1
Local 2 --> Global 2
Local 3 --> Global 3
```

Work Group 1:

```text
Local 0 --> Global 4
Local 1 --> Global 5
Local 2 --> Global 6
Local 3 --> Global 7
```

Bu nedenle array işlemlerinde sıklıkla:

```glsl
uint index = gl_GlobalInvocationID.x;
```

mantığı kullanılır.

---

# 60. gl_NumWorkGroups

Dispatch edilen work group sayısını shader tarafında görmemizi sağlar.

Örneğin:

```c
glDispatchCompute(8, 4, 1);
```

ise kavramsal olarak:

```text
gl_NumWorkGroups = (8, 4, 1)
```

olur.

---

# 61. gl_WorkGroupSize

Shader'ın local work group size bilgisini temsil eder.

Örneğin:

```glsl
layout(local_size_x = 16,
       local_size_y = 16,
       local_size_z = 1) in;
```

için:

```text
gl_WorkGroupSize
=
(16, 16, 1)
```

olarak düşünülebilir.

---

# 62. Compute Shader İlk Testimiz Nasıl Olacak?

İlk test mümkün olduğunca basit olacak.

CPU:

```text
1 2 3 4 5
```

verisini hazırlayacak.

Bu veri GPU tarafından erişilebilen bir buffer'a gönderilecek.

Compute Shader her eleman için:

```text
value = value * 2
```

yapacak.

Sonuç:

```text
2 4 6 8 10
```

olacak.

Bu test algoritmik olarak basittir.

Ama altyapı açısından birçok şeyi doğrular.

---

# 63. İlk Compute Testinin Doğruladığı Şeyler

Bu küçük test ile:

1. GLFW initialize oldu mu?
2. OpenGL context oluştu mu?
3. Context Compute Shader'ı destekliyor mu?
4. GLAD doğru yüklendi mi?
5. Compute Shader source okunabildi mi?
6. Compute Shader compile oldu mu?
7. Compute program link oldu mu?
8. Buffer oluşturuldu mu?
9. Input GPU tarafından erişilebilir hale geldi mi?
10. `glDispatchCompute()` çalıştı mı?
11. Invocation'lar doğru index'i kullandı mı?
12. Memory synchronization doğru yapıldı mı?
13. Sonuç CPU tarafından okunabildi mi?
14. Sonuç beklenen değerlerle aynı mı?

kontrol edilebilir.

---

# 64. SSBO Nedir?

**SSBO = Shader Storage Buffer Object**

Shader'ların büyük ve genel amaçlı veri bloklarına erişebilmesi için
kullanılan buffer mekanizmalarından biridir.

Compute Shader açısından çok kullanışlıdır çünkü shader:

- buffer'dan okuyabilir,
- buffer'a yazabilir.

Örneğin:

```text
CPU Array
   |
   v
SSBO
   |
   v
Compute Shader
   |
   v
SSBO
   |
   v
CPU Readback
```

İlk array ×2 testimiz için doğal seçeneklerden biridir.

Ancak temel execution model tamamen anlaşılmadan SSBO ayrıntılarına
geçmeyeceğiz.

---

# 65. Image Load/Store Nedir?

Compute Shader yalnızca SSBO ile çalışmaz.

Image Load/Store mekanizması shader'ların image/texture storage üzerinde
daha genel read/write işlemleri gerçekleştirmesine olanak verir.

Özellikle:

- image processing,
- compute-generated textures,
- filtreleme,
- bazı genel GPU algoritmaları

için kullanılabilir.

Bunu klasik texture sampling ile tamamen aynı şey olarak düşünmemeliyiz.

---

# 66. Shared Memory Nedir?

Compute Shader'da aynı work group içerisindeki invocation'lar arasında
paylaşılabilen `shared` storage bulunabilir.

Kavramsal:

```text
Work Group
|
+-- Invocation 0 --+
+-- Invocation 1 --+--> Shared Data
+-- Invocation 2 --+
+-- Invocation 3 --+
```

Bu hızlı işbirliği gerektiren paralel algoritmalarda çok önemlidir.

Ancak shared data kullanıldığında synchronization konusu kritik hale
gelir.

---

# 67. Race Condition Nedir?

İki veya daha fazla invocation aynı memory alanına koordinasyonsuz
şekilde eriştiğinde ve en az bir erişim write olduğunda sonuç execution
sırasına bağlı hale gelebilir.

Örneğin:

```text
A = 10

Invocation 0:
A = A + 1

Invocation 1:
A = A + 1
```

Naif olarak:

```text
A = 12
```

bekleyebiliriz.

Ama iki invocation aynı eski değeri okuyup birbirinin sonucunu ezebilir.

Bu tür problemler:

```text
race condition
```

kavramıyla ilişkilidir.

---

# 68. Atomic Operation Nedir?

Atomic operation, belirli memory read-modify-write işlemlerinin
bölünemez/atomic şekilde gerçekleştirilmesini sağlayan mekanizmadır.

Örneğin bir sayacı birçok invocation artırıyorsa atomic işlem
gerekebilir.

Kavramsal:

```text
atomicAdd(counter, 1)
```

ile ortak counter'ın yarış durumuna karşı kontrollü artırılması
amaçlanabilir.

Atomic işlemler her problemi çözmez ve performans maliyeti
oluşturabilir.

---

# 69. Synchronization Nedir?

Synchronization:

> Paralel çalışan execution birimlerinin memory erişimleri ve ilerleme
> noktaları arasında gerekli düzeni/visibility koşullarını sağlamaya
> yönelik mekanizmaların genelidir.

GPU'da işler paralel olduğu için:

```text
Invocation A yazdı.
Invocation B hemen kesin olarak gördü.
```

varsayımı her durumda yapılamaz.

Memory model ve synchronization kurallarını dikkate almak gerekir.

---

# 70. barrier() Nedir?

Compute Shader içindeki:

```glsl
barrier();
```

shader execution synchronization ile ilgili bir built-in fonksiyondur.

Özellikle aynı work group içerisindeki invocation'ların belirli bir
execution noktasında koordinasyonunda kullanılır.

Basit zihinsel model:

```text
Invocation 0 ----\
Invocation 1 -----\
Invocation 2 ------> barrier --> devam
Invocation 3 -----/
```

Ancak `barrier()` ile memory visibility kurallarının ayrıntısı
GLSL/OpenGL memory modeline göre dikkatli incelenmelidir.

Bunu "GPU'daki her şeyi durdurur" şeklinde düşünmek yanlıştır.

---

# 71. glMemoryBarrier() Nedir?

`glMemoryBarrier()` CPU tarafındaki OpenGL API çağrısıdır.

Compute dispatch gibi shader işlemlerinin yaptığı memory write'ların
sonraki OpenGL işlemleri tarafından doğru şekilde görünür olmasını
sağlamak için uygun barrier bitleriyle kullanılır.

Örneğin:

```text
Compute Shader
    |
    | writes SSBO
    v
Memory
    |
glMemoryBarrier(...)
    |
    v
Sonraki GPU/OpenGL işlemi
```

Burada:

```text
barrier()
```

ile:

```text
glMemoryBarrier()
```

aynı şey değildir.

---

# 72. barrier() ve glMemoryBarrier() Farkı

Basit ayrım:

```text
barrier()

Nerede?
GLSL shader içerisinde

Temel kapsam?
Work group içi shader invocation execution synchronization
```

karşısında:

```text
glMemoryBarrier()

Nerede?
C/C++ OpenGL API tarafında

Temel amaç?
Shader memory write'larının sonraki belirli tür OpenGL erişimlerine görünürlüğünü/order ilişkisini kurmak
```

Bu konu specification üzerinden ayrıca detaylı incelenmelidir.

---

# 73. Compute Shader ile CPU Arasında Veri Akışı

Örnek:

```text
CPU
 |
 | input oluştur
 v
Buffer
 |
 | GPU access
 v
Compute Shader
 |
 | write
 v
Buffer
 |
 | synchronization
 v
CPU Readback
 |
 v
Result Verification
```

Bu akış bizim ilk testimizin temelini oluşturacak.

---

# 74. Fragment Shader ve Compute Shader Karşılaştırması

---

  Özellik                 Fragment Shader         Compute Shader

---

  Ana amaç                Rasterized fragment     Genel paralel hesaplama
                          işlemek

  Graphics pipeline       Evet                    Klasik graphics
                                                  pipeline'dan ayrı
                                                  dispatch modeli

  Rasterization gerekli   Evet                    Hayır

  Geometry gerekli        Rasterization için      Hayır
                          primitive gerekir

  Invocation kaynağı      Rasterization           Compute dispatch

  Work Group              Compute anlamında yok   Var

  Local Size              Compute anlamında yok   Var

  Shared work-group       Yok                     Var
  memory

  Genel computation için  Hayır                   Evet
  tasarım

Buffer/Image işlemleri  Mümkün fakat            Compute kullanımının
                          stage/model farklı      temel parçalarından
------------------------------------------------------------------------

---

# 75. Neden Fragment Shader'ı Compute Gibi Kullanmak İstemiyoruz?

Eski yöntem:

```text
Data
 |
 v
Texture
 |
 v
Geometry
 |
 v
Rasterization
 |
 v
Fragment Shader
 |
 v
Output
```

Yeni yöntem:

```text
Data
 |
 v
Compute Shader
 |
 v
Data
```

Compute Shader:

- computation için doğrudan execution model sağlar,
- work group kavramına sahiptir,
- invocation ID'leri sağlar,
- shared memory kullanımına olanak verir,
- synchronization mekanizmaları sağlar,
- rasterization gerektirmez,
- genel memory/buffer işlemlerine daha doğal şekilde uyar.

---

# 76. OpenGL 4.3 Konusu

Compute Shader desktop OpenGL'de core olarak OpenGL 4.3 ile gelir.

Dolayısıyla Compute projesinde:

```c
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
```

gibi bir context isteyebiliriz.

Ama bu yalnızca **istektir**.

Driver/GPU bunu desteklemiyorsa GLFW bunu sihirli şekilde sağlayamaz.

Bu yüzden runtime kontrolü yapacağız.

---

# 77. GLFW Compute Shader Sağlar mı?

Hayır.

GLFW:

```text
Window
+
Context creation
+
Input/Event
```

işlerinde yardımcı olur.

Compute Shader:

```text
OpenGL context/driver capability
```

tarafından sağlanır.

---

# 78. GLAD Compute Shader Sağlar mı?

Hayır.

GLAD gerekli OpenGL fonksiyon entry point'lerini yükler.

Örneğin context destekliyorsa:

```text
glDispatchCompute
glMemoryBarrier
...
```

gibi fonksiyonlara erişmemize yardım eder.

Ama donanıma yeni OpenGL özelliği eklemez.

---

# 79. Compute Shader Programı Nasıl Farklı Olacak?

Klasik program:

```text
Vertex Shader
      +
Fragment Shader
      |
      v
Graphics Program
```

Compute tarafında bir compute programı:

```text
Compute Shader
      |
      v
Compute Program
```

şeklinde oluşturulabilir.

Compute Shader object:

```c
glCreateShader(GL_COMPUTE_SHADER);
```

ile oluşturulur.

Source verilir.

Compile edilir.

Program'a attach edilir.

Program link edilir.

Sonra:

```c
glUseProgram(program);
glDispatchCompute(...);
```

ile dispatch yapılabilir.

---

# 80. Compute Programının Baştan Sona Akışı

İleride yazacağımız minimum testin büyük resmi:

```text
PROGRAM START
     |
     v
GLFW Init
     |
     v
Request OpenGL 4.3+ Context
     |
     v
Create Window + Context
     |
     v
Make Context Current
     |
     v
Load OpenGL with GLAD
     |
     v
Check OpenGL Version
     |
     v
Read compute_shader.glsl
     |
     v
Create GL_COMPUTE_SHADER
     |
     v
Compile
     |
     v
Check Compile Status
     |
     v
Create Program
     |
     v
Attach Compute Shader
     |
     v
Link Program
     |
     v
Check Link Status
     |
     v
Create Input/Output Buffer
     |
     v
Upload Input Data
     |
     v
Bind Buffer to Shader Interface
     |
     v
glUseProgram()
     |
     v
glDispatchCompute(...)
     |
     v
GPU Work Groups
     |
     v
Shader Invocations
     |
     v
Parallel Computation
     |
     v
Memory Writes
     |
     v
glMemoryBarrier(...)
     |
     v
Read Result
     |
     v
Compare Expected / Actual
     |
     v
PASS / FAIL
     |
     v
Cleanup
```

Bu akış ileride TP dokümanlarının temelini de oluşturabilir.

---

# 81. Graphics Draw Call ile Compute Dispatch Karşılaştırması

Graphics:

```text
glDrawArrays()
      |
      v
Vertex Processing
      |
      v
Primitive
      |
      v
Rasterization
      |
      v
Fragment Processing
```

Compute:

```text
glDispatchCompute()
      |
      v
Work Groups
      |
      v
Invocations
      |
      v
Compute Shader
```

İki çağrı GPU işi başlatır fakat execution modelleri farklıdır.

---

---

# 82. Compute Shader Çalışmasında Ne Yapıldı?

Compute shader tarafındaki çalışma, yalnızca bir shader yazıp dispatch etmekle
sınırlı tutulmadı. Amaç; CPU'dan gelen verinin buffer'a yüklenmesini, shader'ın
bu buffer'a hangi bağlantı üzerinden eriştiğini, GPU hesabının nasıl
başlatıldığını, sonuçların nasıl doğrulanacağını ve bunların HLR seviyesinde
nasıl izlenebilir hale getirileceğini birlikte ele almaktı.

Bu kapsamda aşağıdaki çıktılar hazırlandı:

1. Compute shader pipeline ve kullanılan fonksiyonlar için ayrıntılı teknik
   Markdown dokümanları hazırlandı.
2. SSBO bağlama ve CPU readback ilişkisini gösteren SVG diyagramları üretildi.
3. Geçerli ve hatalı kullanımları içeren C senaryo dosyaları oluşturuldu.
4. glBindBufferBase ve glGetBufferSubData için HLR requirement'ları yazıldı.
5. Requirement'ların sonraki TP aşamasına taşınabilmesi için fonksiyon,
   parametre, hata ve doğrulama sınırları açık şekilde ayrıştırıldı.

Çalışmanın ana örneği basittir: CPU, [1, 2, 3, 4] değerlerini bir SSBO'ya
yükler. Compute shader her elemanı 2 ile çarpar. Beklenen sonuç
[2, 4, 6, 8] olur. Bu küçük örnek, gerçek pipeline'daki bütün kritik
bağlantıları göstermeye yeterlidir.

---

# 83. Compute Shader İçin Seçilen API ve Shader Dili

Compute shader tarafında hedef API OpenGL ES 3.1, shader dili ise GLSL ES 3.10
olarak belirlendi.

Shader kaynak kodunda bu seçim şu sürüm satırıyla görünür:

```glsl
#version 310 es
```

Buradaki es ifadesi önemlidir. GLSL ES 3.10 ile masaüstü OpenGL'de kullanılan
GLSL sürümleri aynı dil hedefi değildir. Örneğin aşağıdaki satır masaüstü
OpenGL tarafına aittir:

```glsl
#version 310 core
```

Bu nedenle OpenGL ES 3.1 compute shader örneklerinde 310 core kullanılmaz.
Shader sürümü ile context sürümü aynı platform ailesine ait olmalıdır.

| Konu                           | Kullanılan standart            |
| ------------------------------ | ------------------------------- |
| Compute shader dispatch        | OpenGL ES 3.1                   |
| SSBO ve indexed buffer binding | OpenGL ES 3.1                   |
| Compute shader kaynak dili     | GLSL ES 3.10                    |
| glGetBufferSubData tanımı    | Desktop OpenGL 4.3 Core Profile |

Son satırdaki ayrım sonraki bölümlerde ayrıntılı olarak açıklanacaktır.

---

# 84. Compute Shader Pipeline'ın Uçtan Uca Akışı

Compute shader ile veri işleme akışı aşağıdaki sırayı izler:

```text
CPU uygulaması
    |
    v
Buffer object oluşturma
    |
    v
Başlangıç verisini buffer'a yükleme
    |
    v
Buffer'ı SSBO binding point'ine bağlama
    |
    v
Compute shader programını aktif etme
    |
    v
glDispatchCompute(...)
    |
    v
GPU work group'ları ve invocation'ları
    |
    v
Shader'ın SSBO'dan okuması / SSBO'ya yazması
    |
    v
Gerekli bellek görünürlüğü yönetimi
    |
    v
Sonucun GPU'da kullanılması veya CPU'ya geri alınması
```

Bu akışta her adım farklı bir görevi yerine getirir:

| Adım             | Sorumluluk                                                      |
| ----------------- | --------------------------------------------------------------- |
| Buffer oluşturma | GPU/OpenGL tarafında veri deposunu sağlar                     |
| Veri yükleme     | CPU başlangıç verisini buffer'a taşır                      |
| Buffer binding    | Shader'ın hangi buffer'a erişeceğini belirler                |
| Program seçimi   | Çalışacak compute shader executable'ını belirler           |
| Dispatch          | GPU üzerinde paralel işi başlatır                           |
| Memory barrier    | Sonraki erişimlerin uygun görünürlük kurallarını sağlar |
| Readback          | Sonucu CPU tarafında inceleme veya doğrulama amacıyla alır  |

Bu sebeple compute shader testinde yalnızca glDispatchCompute çağrısının hata
üretmemesi yeterli değildir. Shader'ın doğru buffer'a bağlandığı, doğru sayıda
invocation'ın çalıştığı ve sonucun beklenen buffer konumuna yazıldığı da
doğrulanmalıdır.

---

# 85. SSBO Neden Kullanıldı?

Shader Storage Buffer Object, yani SSBO, shader'ın buffer verisini hem
okuyabilmesini hem de yazabilmesini sağlar. Compute shader örneğinde sonuç
GPU tarafında üretildiği için SSBO uygun kaynaktır.

Shader tarafındaki örnek bildirim:

```glsl
#version 310 es

layout(std430, binding = 0) buffer Numbers
{
    uint values[];
};
```

Bu bildirimde:

| Bölüm        | Açıklama                                                           |
| -------------- | -------------------------------------------------------------------- |
| buffer Numbers | Shader storage block tanımıdır                                    |
| uint values[]  | Buffer verisinin uint dizisi olarak yorumlanacağını belirtir      |
| std430         | Buffer içi yerleşim düzenini tanımlar                            |
| binding = 0    | Shader'ın 0 numaralı SSBO binding point'ine bakacağını belirtir |

CPU tarafındaki veri türü, buffer boyutu ve shader tarafındaki veri yerleşimi
birbirleriyle uyumlu olmalıdır. Bu örnekte CPU tarafında GLuint dizisi, shader
tarafında uint dizisi kullanılır. Her eleman dört byte olduğundan basit dizi
örneğinde eleman konumları doğrudan eşleşir.

---

# 86. glBindBufferBase Nedir?

glBindBufferBase, bir buffer object'i indexed buffer target içindeki belirli
bir binding point'e bağlar.

```c
void glBindBufferBase(GLenum target, GLuint index, GLuint buffer);
```

Compute shader SSBO örneğinde kullanım:

```c
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
```

Bu çağrının anlamı şudur:

```text
GL_SHADER_STORAGE_BUFFER target'ı
            |
            v
Indexed binding point 0
            |
            v
ssbo buffer object'i
            |
            v
Shader içindeki layout(binding = 0) storage block'u
```

Dolayısıyla shader'daki binding değeri ile C tarafındaki index değeri eşit
olmalıdır. Shader tarafında binding = 0 varken uygulama buffer'ı index = 1
ile bağlarsa shader storage block'u beklenen buffer'a erişmez.

glBindBufferBase başarılı olduğunda iki bağ güncellenir:

1. target ve index ile seçilen indexed binding point.
2. target ile ilişkili general binding point.

Bu ikinci davranış, readback gibi general binding point kullanan fonksiyonlar
açısından önemlidir.

---

# 87. glBindBufferBase Parametreleri

## target

target, indexed binding dizisini seçer. OpenGL ES 3.1'de kabul edilen değerler
aşağıdakilerdir:

```text
GL_ATOMIC_COUNTER_BUFFER
GL_SHADER_STORAGE_BUFFER
GL_TRANSFORM_FEEDBACK_BUFFER
GL_UNIFORM_BUFFER
```

Compute shader SSBO erişiminde kullanılan değer GL_SHADER_STORAGE_BUFFER'dır.
GL_ARRAY_BUFFER bir buffer target olsa da glBindBufferBase için kabul edilen
indexed target'lardan biri değildir. Bu target ile çağrı yapılırsa
GL_INVALID_ENUM oluşur.

## index

index, target'a ait indexed binding-point dizisindeki elemanı seçer. SSBO
kullanımında geçerli aralık şu limit ile belirlenir:

```c
GLint count = 0;
glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &count);
```

Geçerli index değerleri 0 ile count - 1 arasındadır. index değeri count'a
eşit veya daha büyükse GL_INVALID_VALUE oluşur. Testlerde sabit bir index
sınırı varsaymak yerine bu limit sorgulanır.

## buffer

buffer, bağlanacak buffer object adıdır.

| buffer durumu                                             | Sonuç                                                         |
| --------------------------------------------------------- | -------------------------------------------------------------- |
| Geçerli nonzero buffer adı                              | Buffer seçili indexed ve general binding point'e bağlanır   |
| glGenBuffers ile üretilmiş, henüz ilk kez bağlanan ad | İlk binding sırasında buffer object state'i oluşturulur    |
| 0                                                         | Seçili indexed binding ve general binding sıfıra bağlanır |

glBindBufferBase buffer'ın yalnızca bir alt aralığını değil, tamamını bağlar.
Başlangıç ofseti sıfırdır. Bir buffer aralığı bağlanacaksa glBindBufferRange
fonksiyonu kullanılır.

---

# 88. glBindBufferBase İçin Hazırlanan Senaryolar

Fonksiyon için OpenGL ES 3.1 context'i altında çalıştırılmak üzere C senaryo
dosyası oluşturuldu. Senaryoların amacı, yalnızca başarılı çağrıyı değil hata
ve sınır davranışlarını da göstermektir.

| Senaryo                   | Kontrol edilen davranış              | Beklenen sonuç                   |
| ------------------------- | -------------------------------------- | --------------------------------- |
| Geçerli SSBO binding     | Dolu SSBO binding point 0'a bağlanır | GL_NO_ERROR ve doğru buffer adı |
| Unbind                    | buffer parametresi 0 verilir           | Binding point 0 olur              |
| İlk binding              | Yeni buffer adı ilk kez bağlanır    | Buffer state'i oluşturulur       |
| En yüksek geçerli index | limit - 1 kullanılır                 | GL_NO_ERROR                       |
| Sınır dışı index     | limit kullanılır                     | GL_INVALID_VALUE                  |
| Geçersiz target          | GL_ARRAY_BUFFER kullanılır           | GL_INVALID_ENUM                   |

Senaryo içinde önceki OpenGL hata durumları temizlenir, ardından fonksiyon
çağrılır ve glGetError ile sonuç alınır. Geçerli binding senaryosunda
GL_SHADER_STORAGE_BUFFER_BINDING sorgusu ile indexed binding point'te bulunan
buffer adı doğrulanır.

Bu yaklaşımın temel faydası, testin cihazdan cihaza değişebilen binding point
sayısını doğru şekilde dikkate almasıdır.

---

# 89. glDispatchCompute ile İlişkisi

glBindBufferBase, compute shader'a veri yolunu kurar; glDispatchCompute ise
hesabı başlatır.

```c
glUseProgram(compute_program);
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
glDispatchCompute(1, 1, 1);
```

Örnek shader için local work group size şu şekilde seçilebilir:

```glsl
layout(local_size_x = 4,
       local_size_y = 1,
       local_size_z = 1) in;
```

glDispatchCompute(1, 1, 1) çağrısı bir work group oluşturur. Bu work group,
local_size_x değeri 4 olduğu için dört invocation içerir. Her invocation
gl_GlobalInvocationID.x ile kendi dizi elemanını seçer.

Genel hesap:

```text
Toplam invocation sayısı =
(num_groups_x × num_groups_y × num_groups_z)
×
(local_size_x × local_size_y × local_size_z)
```

Bu formül test tasarımında önemlidir. Buffer dört eleman içerirken dört
invocation oluşturulursa her eleman bir kere işlenir. Daha fazla invocation
oluşturulursa shader kodunda buffer sınır kontrolü gerekir.

---

# 90. glMemoryBarrier'ın Pipeline İçindeki Yeri

Compute shader'ın buffer'a yazması ile bu yazının daha sonraki OpenGL
işlemlerince tüketilmesi birbirinden farklı kavramlardır. Shader store
işlemlerinden sonra, sonraki erişimin türüne uygun bellek görünürlüğü
mekanizması kullanılır.

SSBO erişiminden sonra SSBO kullanan sonraki işlem için örnek:

```c
glDispatchCompute(1, 1, 1);
glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
```

GL_SHADER_STORAGE_BARRIER_BIT, shader storage buffer erişimleriyle ilgili
sonraki OpenGL işlemleri için kullanılan barrier bitidir.

glMemoryBarrier ile GLSL içindeki barrier() aynı işlem değildir:

| Mekanizma         | Kapsam                                                                                    |
| ----------------- | ----------------------------------------------------------------------------------------- |
| barrier()         | Aynı work group içindeki shader invocation'larını eşzamanlar                         |
| glMemoryBarrier() | Bir OpenGL komutundan sonraki OpenGL erişimlerinde bellek görünürlüğünü düzenler |

Readback için tek bir barrier biti evrensel ve zorunlu çözüm olarak
tanımlanmadı. CPU readback senkronizasyonu hedef sürücü, context ve gerçek
kullanım senaryosuna göre TP aşamasında doğrulanmalıdır.

---

# 91. glGetBufferSubData Neden Ayrı Bir Kapsamda İncelendi?

glGetBufferSubData, bir buffer object'in seçilen byte aralığını CPU/client
belleğine kopyalar.

```c
void glGetBufferSubData(GLenum target,
                        GLintptr offset,
                        GLsizeiptr size,
                        void *data);
```

Ancak bu fonksiyon OpenGL ES 3.1 core API'sinde bulunmaz. Bu nedenle
glGetBufferSubData için bilgi ve HLR'ler OpenGL ES 3.1 spesifikasyonundan
değil, Desktop OpenGL 4.3 Core Profile spesifikasyonundan hazırlandı.

Bu platform ayrımı aşağıdaki sonucu doğurur:

| Ortam                   | glGetBufferSubData durumu                    |
| ----------------------- | -------------------------------------------- |
| OpenGL ES 3.1           | Core fonksiyon değildir                     |
| Desktop OpenGL 4.3 Core | Buffer readback için tanımlı fonksiyondur |

OpenGL ES 3.1 tarafındaki senaryo, fonksiyonun bulunmadığını açıkça bildirir.
ES 3.1'de buffer verisini CPU tarafına alma ihtiyacı için
glMapBufferRange(..., GL_MAP_READ_BIT) yaklaşımı değerlendirilir.

---

# 92. glGetBufferSubData ile CPU Readback

Compute shader [1, 2, 3, 4] verisini [2, 4, 6, 8] haline getirdikten sonra,
Desktop OpenGL 4.3 context'inde sonuç CPU dizisine aşağıdaki gibi alınabilir:

```c
GLuint result[4] = { 0u };

glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
glGetBufferSubData(GL_SHADER_STORAGE_BUFFER,
                   0,
                   sizeof(result),
                   result);
```

Bu çağrıda:

| Parametre | Örnek değeri           | Anlamı                                    |
| --------- | ------------------------ | ------------------------------------------ |
| target    | GL_SHADER_STORAGE_BUFFER | Okunacak general binding point'i seçer    |
| offset    | 0                        | Buffer'ın ilk byte'ından okumaya başlar |
| size      | sizeof(result)           | Dört GLuint kadar byte okur               |
| data      | result                   | Sonuçların yazılacağı CPU dizisidir   |

Fonksiyon indexed binding point'i değil, target'ın general binding point'ini
kullanır. Bu nedenle readback öncesinde glBindBuffer çağrısı ile hangi
buffer'ın okunacağının açıkça belirtilmesi okunabilir bir kullanım şeklidir.

offset ve size eleman sayısı değil, byte sayısıdır. Örneğin üçüncü GLuint
elemanından okumaya başlamak için offset değeri 2 * sizeof(GLuint) olur.

---

# 93. glGetBufferSubData Hata ve Ön Koşulları

Fonksiyon başarılı olduğunda, offset ile başlayan ve size byte uzunluğundaki
buffer bölgesi data ile gösterilen CPU belleğine kopyalanır. Uygulama, data
için en az size byte büyüklüğünde geçerli bellek sağlamakla sorumludur.

| Durum                                                                      | Sonuç                          |
| -------------------------------------------------------------------------- | ------------------------------- |
| Geçerli target, bağlı nonzero buffer, in-range aralık, unmapped buffer | Veri CPU belleğine kopyalanır |
| Geçersiz target                                                           | GL_INVALID_ENUM                 |
| target general binding point'inde buffer adı 0                            | GL_INVALID_OPERATION            |
| offset negatif                                                             | GL_INVALID_VALUE                |
| size negatif                                                               | GL_INVALID_VALUE                |
| offset + size, buffer boyutunu aşıyor                                    | GL_INVALID_VALUE                |
| Buffer mapped durumda                                                      | GL_INVALID_OPERATION            |

Desktop OpenGL 4.3'te geçerli target'lar arasında GL_SHADER_STORAGE_BUFFER da
yer alır. Bunun yanında GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER,
GL_COPY_WRITE_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER,
GL_TRANSFORM_FEEDBACK_BUFFER ve diğer buffer target'ları da kullanılabilir.

Fonksiyon buffer'ın veri deposunu değiştirmez. Yalnızca seçilen byte aralığını
CPU belleğine kopyalar.

---

# 94. glGetBufferSubData İçin Hazırlanan Senaryolar

Senaryo kodu iki farklı derleme durumunu ayırır:

| Derleme ortamı                          | Davranış                                            |
| ---------------------------------------- | ----------------------------------------------------- |
| Projedeki OpenGL ES 3.1 GLAD başlığı | Fonksiyonun ES 3.1 core'da bulunmadığını bildirir |
| Desktop OpenGL 4.3 GLAD başlığı      | Readback ve hata senaryoları derlenir                |

Desktop OpenGL 4.3 tarafındaki senaryolar:

| Senaryo                 | Davranış                         | Beklenen sonuç      |
| ----------------------- | ---------------------------------- | -------------------- |
| Tam buffer readback     | Dört değer okunur                | {10, 20, 30, 40}     |
| Kısmi readback         | Offset ile son iki değer okunur   | {30, 40}             |
| Geçersiz target        | GL_FRAMEBUFFER kullanılır        | GL_INVALID_ENUM      |
| Negatif offset          | -1 verilir                         | GL_INVALID_VALUE     |
| Sınır dışı aralık | offset + size buffer sonunu aşar  | GL_INVALID_VALUE     |
| Sıfır buffer binding  | Target'a 0 bağlıdır             | GL_INVALID_OPERATION |
| Mapped buffer           | Buffer map edildikten sonra okunur | GL_INVALID_OPERATION |

Bu senaryolar, fonksiyonun yalnızca normal sonuç döndüren kullanımını değil,
spesifikasyonla tanımlanmış koruyucu hata davranışlarını da kapsar.

---

# 95. GLAD ve Context Ön Koşulu

Senaryo dosyaları aşağıdaki başlığı kullanır:

```c
#include <glad/glad.h>
```

Bu başlığın editörde kırmızı görünmesi her zaman derleme hatası olduğu
anlamına gelmez. Çoğu durumda IntelliSense include path ayarı eksiktir.
Derleyicinin doğru başlık klasörünü bilmesi gerekir:

```text
-I"others (for Compute)/include"
```

Link aşamasında GLAD yükleyici kaynak dosyası da uygulamaya eklenir:

```text
others (for Compute)/glad.c
```

Senaryo dosyaları main fonksiyonu içermez. Bunlar bir test yürütücüsünün
çağıracağı fonksiyon kümeleridir. Ayrıca her senaryodan önce uygun sürümde
ve current durumda geçerli bir OpenGL context bulunmalıdır.

Bu iki nokta ayrıdır:

1. IDE'nin include satırını çözebilmesi.
2. Çalışma anında geçerli OpenGL context ve yüklenmiş OpenGL fonksiyonlarının
   bulunması.

---

# 96. Fonksiyon Dokümantasyonları ve Görseller

Compute shader altında üç fonksiyon dokümanı bulunur:

| Doküman              | İçerik                                                                                 |
| --------------------- | ---------------------------------------------------------------------------------------- |
| glDispatchCompute.md  | Work group hiyerarşisi, local size, built-in değişkenler, limitler ve hata durumları |
| glBindBufferBase.md   | SSBO binding, parametreler, binding eşleşmesi ve hata durumları                       |
| glGetBufferSubData.md | Desktop GL 4.3 readback, byte aralığı, target'lar ve mapped buffer davranışı       |

glBindBufferBase ve glGetBufferSubData için ayrıca SVG diyagramları hazırlandı.

| Görsel                | Gösterdiği ilişki                                               |
| ---------------------- | ------------------------------------------------------------------ |
| glBindBufferBase.svg   | Buffer object → indexed binding point → shader storage block     |
| glGetBufferSubData.svg | GPU/OpenGL buffer → seçilen byte aralığı → CPU result dizisi |

Dokümanların amacı yalnızca fonksiyonun tanımını vermek değildir. Her
fonksiyonda parametreler, parametre değerlerinin sonuçları, geçerli/geçersiz
kullanımlar, hata kodları, compute shader örneği ve kaynak spesifikasyon
bulunur.

---

# 97. HLR Nedir?

HLR, High-Level Requirement anlamına gelir. HLR, bir sistemin veya kütüphanenin
yerine getirmesi gereken davranışı açık, izlenebilir ve doğrulanabilir biçimde
tanımlar.

Bu projedeki HLR yapısında aşağıdaki sütunlar kullanılır:

| Alan                | Görevi                                            |
| ------------------- | -------------------------------------------------- |
| Requirement ID      | Gereksinimin benzersiz kimliği                    |
| Requirement         | Doğrulanacak davranış                           |
| Information         | Yalnızca kritik ek bağlam                        |
| Definitions         | API, terim ve sınır tanımları                  |
| Reference           | Davranışın dayandığı spesifikasyon kaynağı |
| Impl.-Dep.          | Implementasyon bağımlılığı                   |
| Source              | Kaynak requirement kimliği                        |
| Verification Method | Doğrulama yaklaşımı                            |

Requirement dili kesin olmalıdır. Bu nedenle gereksinimler shall yapısıyla
yazıldı. Belirsiz yükümlülük oluşturan may, may not, should, could ve might
gibi ifadeler kullanılmadı.

Information sütunu, requirement veya definitions sütununda zaten ifade edilen
bilgilerle doldurulmadı. Sadece davranışın sınırını veya test yorumunu
değiştiren kritik bilgiler bırakıldı.

---

# 98. glBindBufferBase HLR Kapsamı

glBindBufferBase için OpenGL ES 3.1 / ComputeShaders modülü altında 12
requirement yazıldı.

| Requirement grubu     | Kapsanan davranış                                                   |
| --------------------- | --------------------------------------------------------------------- |
| API arayüzü         | Fonksiyon imzası ve parametrelerin tanımı                          |
| SSBO binding          | GL_SHADER_STORAGE_BUFFER target'ında indexed binding oluşturulması |
| General binding       | Başarılı çağrının general binding point'i de güncellemesi     |
| Önceki binding       | Yeni bağın eski binding'i değiştirmesi                            |
| İlk binding          | Nonzero yeni buffer adında object state oluşturulması              |
| Unbind                | buffer = 0 ile seçilen binding'lerin sıfırlanması                 |
| Tüm buffer binding   | Başlangıç offset'inin 0 ve kapsamın buffer'ın tamamı olması    |
| Geçerli target'lar   | Dört indexed target'ın kabul edilmesi                               |
| Geçersiz target      | GL_INVALID_ENUM üretilmesi                                           |
| Index limiti          | Sınır dışı index için GL_INVALID_VALUE üretilmesi              |
| GLSL eşleşmesi      | layout(binding = N) ile binding point N eşleşmesi                   |
| Buffer büyüklüğü | Shader storage block erişimi için yeterli storage sağlanması      |

Bu requirement'lar yalnızca normal SSBO bağlama yolunu değil, buffer state
oluşturma ve hata davranışlarını da kapsar. Böylece TP aşamasında her davranış
ayrı test adımlarıyla izlenebilir.

---

# 99. glGetBufferSubData HLR Kapsamı

glGetBufferSubData için Desktop OpenGL 4.3 / ComputeShaders modülü altında
12 requirement yazıldı.

| Requirement grubu       | Kapsanan davranış                                          |
| ----------------------- | ------------------------------------------------------------ |
| API arayüzü           | Fonksiyon imzası ve dört parametrenin tanımı             |
| Veri kopyalama          | In-range byte aralığının client belleğine aktarılması |
| Byte semantiği         | offset ve size değerlerinin byte cinsinden yorumlanması    |
| Geçerli target'lar     | OpenGL 4.3 Core buffer target listesinin kabul edilmesi      |
| SSBO readback           | GL_SHADER_STORAGE_BUFFER üzerinden readback yapılması     |
| Geçersiz target        | GL_INVALID_ENUM üretilmesi                                  |
| Sıfır binding         | General binding point'te buffer 0 ise GL_INVALID_OPERATION   |
| Negatif offset          | GL_INVALID_VALUE                                             |
| Negatif size            | GL_INVALID_VALUE                                             |
| Sınır dışı aralık | offset + size buffer boyutunu aşıyorsa GL_INVALID_VALUE    |
| Mapped buffer           | GL_INVALID_OPERATION                                         |
| Buffer içeriği        | Readback işleminin buffer veri deposunu değiştirmemesi    |

Bu HLR'nin GL43 modülünde tutulmasının nedeni, glGetBufferSubData'nın OpenGL
ES 3.1 core API kapsamı dışında olmasıdır. Platform sınırı requirement
seviyesinde görünür tutulmuştur.

---

# 100. HLR'ye Kadar Oluşan İzlenebilirlik Zinciri

Bu çalışmada izlenen yol aşağıdaki gibidir:

```text
OpenGL / GLSL spesifikasyonu
            |
            v
Fonksiyon dokümanı
  - amaç
  - parametre
  - hata davranışı
  - compute örneği
            |
            v
SVG diyagramı
  - buffer ve shader ilişkisi
            |
            v
C senaryosu
  - geçerli kullanım
  - sınır durumu
  - hata durumu
            |
            v
HLR
  - requirement kimliği
  - shall davranışı
  - spesifikasyon referansı
  - doğrulama yöntemi
```

Bir sonraki aşama TP, yani Test Procedure yazımıdır. TP aşamasında her HLR
requirement kimliği için test ön koşulu, test adımı, beklenen OpenGL sonucu,
temizlik adımı ve test kaydı oluşturulur.

Buraya kadar yapılan çalışma, compute shader fonksiyonlarının ne yaptığını
anlatmanın yanında bu davranışların spesifikasyona dayalı olarak testlenebilir
hale getirilmesini sağlamıştır.

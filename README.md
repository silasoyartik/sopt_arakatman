# EGL Markdown sunumunu oluşturma

Her EGL fonksiyonunun ayrı Markdown dosyası asıl kaynaktır. Birleşik
`docs/eglFunctions.md` dosyası generated artifact'tır; doğrudan düzenlenmez.

## Proje yapısı

```text
docs/
├── eglFunctions/
│   ├── eglGetDisplay.md          # Fonksiyon kaynağı
│   ├── eglInitialize.md          # Fonksiyon kaynağı
│   └── ...
├── presentation-order.txt        # Birleştirme sırası
├── presentation-preamble.md      # Sunumun giriş bölümü
├── presentation-footer.md        # Sunumun son bölümü
└── eglFunctions.md               # Otomatik oluşturulan birleşik dosya
scripts/
└── build_presentation.py         # Cross-platform üretim scripti
```

Script yalnızca Python 3.8+ standart kütüphanesini kullanır. Linux, macOS ve
Windows'ta proje kökünden aynı komutla çalıştırılır:

```sh
python scripts/build_presentation.py
```

Windows'ta `python` komutu tanımlı değilse Python Launcher kullanılabilir:

```powershell
py scripts/build_presentation.py
```

Script kaynakların arasına `---` ekler, içindekiler listesini sıra dosyasından
üretir ve çıktının başına elle düzenlenmemesi gerektiğini belirten bir yorum
yazar. Çıktı zaten güncelse dosyaya yeniden yazmaz.

CI veya Git hook içinde birleşik dosyanın güncel olduğunu, dosyayı değiştirmeden
kontrol etmek için:

```sh
python scripts/build_presentation.py --check
```

Eksik dosya, yinelenen kayıt, Markdown olmayan kaynak veya generated dosyanın
kaynak olarak listelenmesi hata kabul edilir ve script sıfırdan farklı durum
koduyla sonlanır. Sıra dosyasındaki her satır tek bir yol olduğu için dosya ve
klasör adlarındaki boşluklar ayrıca kaçış karakteri gerektirmez.

## Kaynak ve generated artifact ayrımı

Kaynak olarak commit edilmesi gereken dosyalar şunlardır:

- Ayrı EGL fonksiyon dosyaları
- `docs/presentation-order.txt`
- `docs/presentation-preamble.md` ve `docs/presentation-footer.md`
- `scripts/build_presentation.py`

`docs/eglFunctions.md` generated artifact'tır. Sunumun GitHub üzerinden kolayca
okunması, diff'inin incelenmesi veya Python çalıştırmadan kullanılabilmesi
isteniyorsa commit edilmesi uygundur. Bu projede bu yaklaşım kullanılır. CI'da
`--check` çalıştırılarak unutulmuş üretimler yakalanabilir.

Generated diff'lerin gereksiz olduğu ve tüm tüketicilerin build adımını
çalıştırdığı bir projede bunun yerine `docs/eglFunctions.md` `.gitignore` içine
alınabilir. Bu durumda release veya sunum sürecinin önce scripti çalıştırması
gerekir. İki yaklaşım aynı anda kullanılmamalıdır.

## Fonksiyon ekleme, çıkarma ve sıralama

Yeni bir fonksiyon eklemek için Markdown dosyasını `docs/eglFunctions/` altına
ekleyin ve dosya yolunu `docs/presentation-order.txt` içinde istediğiniz konuma
yazın. Ardından üretim komutunu çalıştırın.

Bir fonksiyonu yalnızca sunumdan çıkarmak için sıra dosyasındaki satırını silin;
kaynak Markdown dosyasını silmek gerekmez. Sırayı değiştirmek için satırların
yerini değiştirmeniz yeterlidir.

Sıra dosyasındaki yollar dosyanın bulunduğu `docs/` klasörüne göre çözülür.
Örneğin boşluk içeren bir yol doğrudan şöyle yazılır:

```text
eglFunctions/advanced functions/egl Example.md
```

## Makefile gerekli mi?

Make, kaynaklardan biri daha yeniyse hedefi üretmek için faydalı olabilir. Ancak
Windows'ta varsayılan olarak bulunmaz ve boşluk içeren dosya yolları ile dinamik
sıra listelerini dependency olarak yönetmek ek karmaşıklık getirir. Bu nedenle
ana çözüm Makefile'a bağlı değildir. Python scriptinin çıktıyı sadece içerik
değiştiğinde yazması, bu küçük dokümantasyon build'i için aynı pratik faydayı
taşınabilir biçimde sağlar.

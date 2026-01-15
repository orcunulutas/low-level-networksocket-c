# C ile Network Programlama: Echo Server Rehberi

Bu döküman, C programlama dili kullanılarak yazılmış basit bir "Echo Server" (Yankı Sunucusu) projesinin çalışma mantığını ve temel kavramlarını açıklamak için hazırlanmıştır.

## 1. Temel Kavramlar

### Struct (Yapı) ve Pointer (İşaretçi) Kullanımı

C dilinde verileri organize etmek için `struct` kullanılır. Bu projede 3 temel yapı bulunmaktadır:

1.  **Buffer (buffer.h):** Dinamik olarak büyüyebilen bir veri tamponudur.
    *   `data`: Verinin tutulduğu bellek adresini gösteren işaretçi.
    *   `size`: Şu anki veri boyutu.
    *   `capacity`: Ayrılmış toplam bellek miktarı.
2.  **Conn (server.h):** Bir istemci bağlantısını temsil eder.
    *   `fd`: Dosya tanımlayıcısı (socket file descriptor).
    *   `req_buffer`: İstemciden gelen veriyi (request) tutar.
    *   `res_buffer`: İstemciye gönderilecek veriyi (response) tutar.
3.  **Server (server.h):** Sunucunun genel durumunu tutar.
    *   `pool_args`: `poll()` fonksiyonuna verilecek olay dizisi.
    *   `conns`: Bağlantı nesnelerini tutan dizi.

### I/O Multiplexing ve `poll()`

Network programlamada birden fazla istemciyi aynı anda yönetmek için "I/O Multiplexing" (Giriş/Çıkış Çoklaması) kullanılır. Bu projede `poll()` sistem çağrısı kullanılmıştır.

`poll()`, bir dosya tanımlayıcı (fd) kümesini izler ve bunlardan herhangi birinde bir olay (okuma, yazma, hata) gerçekleştiğinde programı uyarır. Bu sayede program, verisi hazır olmayan bir soketi okumaya çalışarak bloklanmaz (duraksamaz).

*   `POLLIN`: Okunacak veri var.
*   `POLLOUT`: Yazma işlemi yapılabilir.

## 2. Kodun Çalışma Mantığı

### Başlangıç (main.c)

1.  `socket()`: Bir TCP soketi oluşturulur.
2.  `bind()`: Soket belirtilen porta (8080) bağlanır.
3.  `listen()`: Soket dinleme moduna geçirilir.
4.  `server_init()`: Sunucu veri yapıları başlatılır.
5.  `server_run()`: Ana döngü başlatılır.

### Ana Döngü (server_run in server.c)

Sunucu sonsuz bir döngü (`while(1)`) içinde çalışır:

1.  **poll() Çağrısı:** `poll()` fonksiyonu çağrılır ve herhangi bir sokette olay olması beklenir.
2.  **Olay Kontrolü:** Tüm izlenen soketler döngü ile kontrol edilir (`revents`).
3.  **Yeni Bağlantı (Accept):** Eğer `listen_fd` üzerinde `POLLIN` olayı varsa, yeni bir istemci bağlanmaya çalışıyordur. `accept()` ile bağlantı kabul edilir ve `server_add_connection()` ile listeye eklenir.
4.  **Veri Okuma (Read):** Mevcut bir bağlantıda `POLLIN` varsa, `read()` ile veri okunur.
    *   Okunan veri `res_buffer`'a (yanıt tamponu) eklenir.
    *   Bağlantı durumu `STATE_RES` (yanıt vermeye hazır) olarak güncellenir.
    *   `POLLOUT` olayı izlenmeye başlanır.
5.  **Veri Yazma (Write):** Bir bağlantıda `POLLOUT` varsa ve gönderilecek veri varsa, `write()` ile veri istemciye gönderilir.
    *   Veri gönderildikten sonra tampon temizlenir veya kaydırılır.
    *   Tekrar okuma moduna (`STATE_REQ`) dönülür.

## 3. Buffer Yönetimi

C dilinde string/byte dizileri sabit boyutludur. Ancak ağdan ne kadar veri geleceğini bilemeyiz. Bu yüzden `buffer.c` içinde dinamik bellek yönetimi (`malloc`, `realloc`) kullanılmıştır.

*   `buffer_append`: Veri eklendikçe yer yoksa `realloc` ile bellek alanı 2 katına çıkarılır.

Bu yapı, modern C sunucularının (Nginx, Redis gibi) temel çalışma prensibinin basitleştirilmiş bir halidir.

# 🦐 EBII System Crustea

[![Platform](https://img.shields.io/badge/Platform-Web%20%7C%20ESP32-blue.svg)](https://github.com/)
[![Database](https://img.shields.io/badge/Database-Firebase%20RTDB-orange.svg)](https://firebase.google.com/)
[![Tech Stack](https://img.shields.io/badge/Tech%20Stack-HTML5%20%7C%20CSS3%20%7C%20JS%20(Vanilla)-green.svg)](https://developer.mozilla.org/)

**Crustea EBII · Pro Command Center** adalah platform ekosistem IoT (*Internet of Things*) berbasis *cloud* yang dirancang khusus untuk memonitoring serta mengendalikan kualitas air pada budidaya tambak (seperti udang atau ikan) secara *real-time*. 

Sistem ini mengintegrasikan mikrokontroler **ESP32** sebagai pemancar data sensor jarak jauh dengan **Dasbor Web Interaktif** menggunakan **Firebase Realtime Database** sebagai jembatan komunikasi dua arah (*Full-Duplex*).

## 🛰️ Arsitektur Sistem

Sistem berjalan dengan interaksi dinamis antara Perangkat Keras, Awan Data, dan Antarmuka Pengguna:
[ Perangkat ESP32 ] --(HTTP PUT JSON)--> [ Firebase Realtime Database ] <--(Stream Listener)--> [ Dasbor Web User ]
        ^                                               |
        |-------------(Membaca Perintah ON/OFF)---------|


## Langkah Kerja Sistem

1. ESP32 Telemetry Transmitter: Membaca sensor (atau menyimulasikan data berdasarkan model sinusoidal biologis yang presisi), melakukan sinkronisasi waktu milidetik dengan internet melalui NTP Server, lalu mengirimkan payload terstruktur setiap 2 menit menggunakan metode HTTP PUT.

2. Firebase Realtime Database: Bertindak sebagai basis data NoSQL real-time yang menyimpan riwayat metrik kualitas air berdasarkan hierarki waktu (forecast_2026/Bulan/Tanggal/Jam) sekaligus menyimpan status tombol kendali sakelar (status.json).

3. Web Command Center Dashboard: Aplikasi web statis berkinerja tinggi yang memetakan geolokasi tambak menggunakan Leaflet.js, memvisualisasikan tren data menggunakan Chart.js, dan memproses logika analitik lingkungan secara instan.


## 🚀 Fitur Utama Dasbor

1. Autentikasi Aman: Antarmuka sistem dilengkapi gerbang Login terintegrasi Firebase Auth untuk mencegah akses tidak sah ke pusat kendali.

2. Kendali Jarak Jauh (Live System Toggle): Tombol sakelar operasional pada web yang secara instan mengontrol apakah perangkat ESP32 di lapangan harus masuk ke mode pemancaran data aktif (ON) atau mode siaga (OFF).

3. Expert AI Insights: Modul kecerdasan sistem pakar terintegrasi yang menganalisis deviasi parameter air setiap detik dan memberikan narasi diagnosis kondisi ekosistem saat itu juga.

4. Terminal Log Anomali Otomatis: Menampilkan catatan kronologis kesalahan (error logs) lengkap dengan saran aksi mitigasi ketika kondisi air melewati batas kritis.

5. Ekspor Laporan Data: Fitur unduh seluruh riwayat telemetri tambak yang masuk ke dalam format berkas .csv secara lokal untuk kebutuhan administrasi atau riset lanjutan.


## 📊 Batas Parameter & Logika Mitigasi Lingkungan

Parameter	| Ambang Batas Aman	| Dampak Batas Tidak Aman	| Tindakan Mitigasi Otomatis Dasbor
Oksigen (DO) |	4.5 - 8.0 mg/L |	Hipoksia / Stres Gelembung Gas | Instruksi menyalakan kincir air ke kecepatan penuh / evaluasi aerator.
Keasaman (pH)	| 7.5 - 8.5 |	Kerusakan Karapas / Racun Amonia |	Instruksi penebaran kapur dolomit / aplikasi fermentasi molase.
Suhu (Temp) |	25.0 - 31.0 °C |	Penurunan Metabolisme / Patogen	| Instruksi pengurangan pakan / penambahan debit air baru.
Salinitas |	0.0 - 1.0 ppt |	Gangguan Osmoregulasi |	Instruksi pengenceran air dengan menyuplai air tawar.


## 🛠️ Teknologi & Pustaka yang Digunakan

Antarmuka Web (Frontend):
  1. HTML5 & CSS3 (Custom Modern Grid & Flexbox layout)
  
  2. Vanilla JavaScript (ES6+)
  
  3. Chart.js v4.4.1 - Visualisasi grafik garis telemetri dengan garis batas (threshold annotation).
  
  4. Leaflet.js v1.9.4 - Pemetaan geografis interaktif titik koordinat tambak.
  
  5. Bootstrap Icons v1.11.3 - Set ikon antarmuka visual.
  
  6. Firebase Web Compat SDK v10.8.0 - Modul komunikasi database dan autentikasi.

Perangkat Keras & Firmware (IoT):
  1. C++ (Arduino IDE Framework)
  
  2. WiFi.h & HTTPClient.h - Komunikasi jaringan internet dan REST API Firebase.
  
  3. WiFiClientSecure.h - Protokol keamanan SSL untuk Firebase.
  
  4. time.h - Penanganan waktu lokal waktu Indonesia barat (WIB) melalui NTP (id.pool.ntp.org).


## ⚙️ Petunjuk Pemasangan

1. Konfigurasi Sisi Perangkat Keras (ESP32)
    - Buka file forecast.ino di Arduino IDE.
    - Pastikan library board ESP32 sudah terpasang.
    - Ubah kredensial jaringan dan Firebase sesuai dengan proyek Anda:
        const char* ssid = "NAMA_WIFI_ANDA";
        const char* password = "PASSWORD_WIFI_ANDA";
        String firebaseHost = "[https://URL-DATABASE-ANDA.firebasedatabase.app/](https://URL-DATABASE-ANDA.firebasedatabase.app/)";
        String databaseSecret = "?auth=SECRET_TOKEN_DATABASE_ANDA";
    -  Lakukan Compile dan Upload kode ke papan ESP32 Anda.

2. Konfigurasi Sisi Aplikasi Web
    - Buka berkas forecast.html.
    - Cari bagian konfigurasi Firebase (const firebaseConfig) di dalam tag <script>.
    - Ganti konfigurasi tersebut dengan milik proyek Firebase Anda:
        const firebaseConfig = {
          apiKey: "API_KEY_ANDA",
          authDomain: "PROJECT_ID.firebaseapp.com",
          databaseURL: "https://PROJECT_ID-default-rtdb.firebaseio.com",
          projectId: "PROJECT_ID",
          storageBucket: "PROJECT_ID.appspot.com",
          messagingSenderId: "SENDER_ID",
          appId: "APP_ID"
        };
    - Simpan perubahan dan jalankan berkas HTML melalui Local Server atau unggah langsung ke GitHub Pages.


## 👥 Kontributor & Pengembang

Ahmad Faiz Zuhdi - Lead IoT Developer & Frontend/Backend Engineer - @fzuhdi79-coder
PT Eco Karya Teknologi (Crustea) - Sponsor Proyek Pemantauan Budidaya Petambak Kabupaten Banjarnegara

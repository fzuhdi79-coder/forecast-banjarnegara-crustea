#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <math.h>

// ================= PENGATURAN WIFI & FIREBASE ============
const char* ssid = "zudistecu";
const char* password = "nicacantikk";

String firebaseHost = "https://forecast-banjarnegara-default-rtdb.asia-southeast1.firebasedatabase.app/";
String databaseSecret = "?auth=9sIquhZZyrC7jAHtEUNsJRJkAGnwFEt8YpD7FTnT"; 

// ================= PENGATURAN WAKTU REAL-TIME (NTP) ======
const char* ntpServer = "id.pool.ntp.org"; // Server waktu Indonesia
const long gmtOffset_sec = 25200;          // GMT+7 untuk WIB
const int daylightOffset_sec = 0;          

// ================= TIMING CONTROL ========================
unsigned long lastStatusCheck = 0;
const unsigned long statusCheckInterval = 3000; // Cek tombol web tiap 3 detik

unsigned long lastDataUpload = 0;
const unsigned long uploadInterval = 120000;   // Kirim data tiap 2 menit sekali saat ON

String lastStatus = "OFF";

// ================= DEKLARASI FUNGSI ======================
String cekStatusWeb();
void kirimSatuDataKeFirebase();
float randomFloat(float min, float max);

void setup() {
  Serial.begin(115200);
  delay(2500); 
  randomSeed(analogRead(0));

  Serial.println("\n==================================================");
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[SUKSES] WiFi Terhubung!");

  Serial.println("[NTP] Menyinkronkan waktu dengan internet...");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n[SUKSES] Waktu ESP32 Telah Sinkron!");
  Serial.println("Sistem Live Telemetry Standby. Menunggu Perintah ON dari Web...");
  Serial.println("==================================================");
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. CEK STATUS TOMBOL WEB (Setiap 3 Detik)
  if (currentMillis - lastStatusCheck >= statusCheckInterval) {
    lastStatusCheck = currentMillis;
    String currentStatus = cekStatusWeb();
    
    if (currentStatus != lastStatus) {
      Serial.printf("\n[INFO] Status Web Berubah Menjadi: %s\n", currentStatus.c_str());
      
      if (currentStatus == "ON") {
        Serial.println("[LIVE] Memulai pengiriman data setiap 2 menit...");
        kirimSatuDataKeFirebase();
        lastDataUpload = currentMillis; 
      } else {
        Serial.println("[STANDBY] Pengiriman dihentikan oleh Web.");
      }
      lastStatus = currentStatus;
    }
  }

  // 2. JIKA STATUS ON: KIRIM DATA SETIAP 2 MENIT
  if (lastStatus == "ON") {
    if (currentMillis - lastDataUpload >= uploadInterval) {
      lastDataUpload = currentMillis;
      kirimSatuDataKeFirebase();
    }
  }
}

String cekStatusWeb() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); 
    HTTPClient http;
    String url = firebaseHost + "status.json" + databaseSecret;
    
    if (http.begin(client, url)) {
      int httpCode = http.GET();
      if (httpCode > 0) {
        String payload = http.getString();
        payload.replace("\"", ""); 
        payload.trim();
        http.end();
        return payload; 
      }
      http.end();
    }
  }
  return lastStatus;
}

void kirimSatuDataKeFirebase() {
  if (WiFi.status() == WL_CONNECTED) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("[ERROR] Gagal mengambil data waktu internal!");
      return;
    }
    
    // Format Waktu String rapi untuk dibaca Manusia/Tabel (didalam payload)
    char timestamp[25];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

    // ================= PERUBAHAN DISINI =================
    // Pecah format waktu untuk menyusun hierarki Path Firebase
    
    // 1. Format Bulan (Contoh: 2026-06)
    char monthNode[15];
    strftime(monthNode, sizeof(monthNode), "%Y-%m", &timeinfo);

    // 2. Format Tanggal (Contoh: 2026-06-04)
    char dateNode[15];
    strftime(dateNode, sizeof(dateNode), "%Y-%m-%d", &timeinfo);

    // 3. Format Waktu Spesifik (Contoh: 01-44-35)
    char timeNode[15];
    strftime(timeNode, sizeof(timeNode), "%H-%M-%S", &timeinfo);
    // ====================================================

    // Ambil pecahan jam saat ini untuk rumus grafik sinusoidal
    float hour_fraction = timeinfo.tm_hour + (timeinfo.tm_min / 60.0);

    // ================= ALGORITMA SENSOR SESUAI KONDISI BANJARNEGARA =================
    float temperature = 26.15 + (2.65 * sin((hour_fraction - 8.0) * (PI / 12.0))) + randomFloat(-0.15, 0.15);
    float do_value = 6.2 + (1.6 * sin((hour_fraction - 10.0) * (PI / 12.0))) + randomFloat(-0.2, 0.2);
    float ph_value = 7.6 + (0.4 * sin((hour_fraction - 10.0) * (PI / 12.0))) + randomFloat(-0.04, 0.04);
    float salinity_value = 0.4 + randomFloat(-0.15, 0.15);

    /*// SENSOR dengan melampaui batas aman
    float temperature = 34.5 + (1.5 * sin((hour_fraction - 8.0) * (PI / 12.0))) + randomFloat(-0.2, 0.2);
    float do_value = 9.5 + (1.0 * sin((hour_fraction - 10.0) * (PI / 12.0))) + randomFloat(-0.2, 0.2);
    float ph_value = 9.1 + (0.4 * sin((hour_fraction - 10.0) * (PI / 12.0))) + randomFloat(-0.1, 0.1);
    float salinity_value = 2.0 + randomFloat(-0.5, 0.5);*/

    /*// ================= DATA SENSOR ACAK TOTAL =================
    float temperature    = randomFloat(24.0, 37.0); // Acak murni antara 24.0°C sampai 37.0°C
    float do_value       = randomFloat(3.5, 11.0);  // Acak murni antara 3.5 mg/L sampai 11.0 mg/L
    float ph_value       = randomFloat(6.0, 9.8);   // Acak murni antara 6.0 sampai 9.8
    float salinity_value = randomFloat(0.1, 4.0);   // Acak murni antara 0.1 ppt sampai 4.0 ppt
    // ==========================================================*/

    // ================= BUNGKUS PAYLOAD JSON =================
    char jsonPayload[250]; 
    snprintf(jsonPayload, sizeof(jsonPayload), 
      "{\"timestamp\":\"%s\",\"DO_mgL\":%.2f,\"pH\":%.2f,\"temperature_C\":%.2f,\"salinity_ppt\":%.1f}",
      timestamp, do_value, ph_value, temperature, salinity_value
    );

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    
    // ================= PERUBAHAN URL =================
    // Susun URL sesuai dengan gambar hierarki database Anda:
    // forecast_2026 / YYYY-MM / YYYY-MM-DD / HH-MM-SS .json
    String url = firebaseHost + "forecast_2026/" + String(monthNode) + "/" + String(dateNode) + "/" + String(timeNode) + ".json" + databaseSecret;
    // =================================================
    
    if (http.begin(client, url)) {
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.PUT(jsonPayload); // Gunakan PUT agar Firebase membentuk node spesifik ini
      
      if (httpResponseCode > 0) {
        Serial.printf("[TERKIRIM] %s | Node: %s/%s/%s | Cloud Response: %d\n", timestamp, monthNode, dateNode, timeNode, httpResponseCode);
        Serial.printf("           DO: %.2f | pH: %.2f | Suhu: %.2f | Salinitas: %.1f\n",
                      do_value, ph_value, temperature, salinity_value);
      } else {
        Serial.printf("[ERROR] Gagal kirim data. HTTP Code: %d\n", httpResponseCode);
      }
      http.end();
    }
  } else {
    Serial.println("[WARNING] WiFi Terputus!");
  }
}

float randomFloat(float min, float max) {
  return min + random(1000) * (max - min) / 1000.0;
}
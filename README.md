# 🏃‍♂️ Smart Exercise Tracking Wristband (Activity Tracker)

![Project Status](https://img.shields.io/badge/Status-Active-success)
![Platform](https://img.shields.io/badge/Platform-ESP32%20%7C%20Flutter-blue)

---

## Project Overview 
**Activity Tracker** is a **Full-Stack IoT solution** designed to track and analyze physical activities in real-time. It consists of a wearable device powered by **ESP32 T-Display** and **MPU6050** sensor, communicating with a cross-platform **Flutter** mobile application via **Bluetooth Low Energy (BLE)**.

The system not only counts repetitions but also analyzes the **intensity (G-force)** and **speed (PPM)** of movements like **Walking, Running, Jumping, and Boxing**.

### 🚀 Key Features
* **Real-Time Activity Recognition:** Automatically detects and classifies movements using 6-axis accelerometer/gyroscope data.
* **Low Latency Streaming:** Sends JSON data packets via BLE for instant feedback.
* **Smart Analytics:** Calculates punch/step intensity and workout frequency.
* **Mobile Dashboard:** A modern Flutter app to visualize counters, goals, and history.
* **Cloud Integration:** Architecture ready for Firebase Authentication and Firestore database.

### 🛠️ Tech Stack

| Component | Technology | Details |
| :--- | :--- | :--- |
| **Embedded (Firmware)** | C++ / Arduino IDE | ESP32 T-Display, MPU6050 Sensor |
| **Mobile App** | Flutter / Dart | Stream-based State Management, BLE Client |
| **Connectivity** | Bluetooth LE (BLE) | JSON Data Serialization |
| **Web Presentation** | HTML5 / CSS3 | Project Landing Page |

### 👥 The Team
This project was developed as a collaboration for the **Embedded Systems** course.

* **Muhammed Yusuf Kocaoğlan** - *Embedded Systems & Mobile Application*
    * Designed the circuit and programmed the ESP32.
    * Created technical documentation and presentation materials.
* **Işıl Ceren Satıcı** - *Web Presentation & Documentation*
    * Designed and developed the project's landing page.
    * Developed the Flutter mobile application and data processing logic.

---

# Akıllı Egzersiz Takip Bilekliği 

## 📌 Proje Özeti
Bu proje, fiziksel aktiviteleri gerçek zamanlı olarak takip etmek ve analiz etmek için geliştirilmiş bir **Full-Stack IoT çözümüdür**. Sistem, **ESP32 T-Display** ve **MPU6050** sensörü ile güçlendirilmiş giyilebilir bir cihazdan ve bu cihazla **Bluetooth Low Energy (BLE)** üzerinden haberleşen bir **Flutter** mobil uygulamasından oluşur.

Sistem sadece hareket tekrarını saymakla kalmaz, aynı zamanda **Yürüme, Koşma, Zıplama ve Boks** gibi hareketlerin **şiddetini (G-Kuvveti)** ve **hızını (PPM)** da analiz eder.

### 🚀 Öne Çıkan Özellikler
* **Gerçek Zamanlı Hareket Algılama:** 6 eksenli ivme ve jiroskop verilerini kullanarak hareketleri otomatik sınıflandırır.
* **Düşük Gecikmeli Veri Akışı:** Anlık geri bildirim için verileri BLE üzerinden JSON paketleri halinde gönderir.
* **Akıllı Analiz:** Yumruk/adım şiddetini ve antrenman sıklığını hesaplar.
* **Mobil Kontrol Paneli:** Sayaçları, hedefleri ve geçmişi görselleştiren modern bir Flutter arayüzü sunar.
* **Bulut Entegrasyonu:** Firebase Authentication ve Firestore veritabanı entegrasyonuna uygun mimari.

### 🛠️ Kullanılan Teknolojiler

| Bileşen | Teknoloji | Detay |
| :--- | :--- | :--- |
| **Gömülü Yazılım** | C++ / Arduino IDE | ESP32 T-Display, MPU6050 Sensör |
| **Mobil Uygulama** | Flutter / Dart | Stream tabanlı durum yönetimi, BLE İstemci |
| **Bağlantı** | Bluetooth LE (BLE) | JSON Veri Paketleme |
| **Web Sunumu** | HTML5 / CSS3 | Proje Tanıtım Sitesi (Landing Page) |

### 👥 Geliştirici Ekip
Bu proje, **Gömülü Sistemler** dersi kapsamında bir takım çalışması olarak geliştirilmiştir.

* **Muhammed Yusuf Kocaoğlan** - *Gömülü Sistemler & Mobil Uygulama*
    * Devre tasarımı ve ESP32 programlaması.
    * Teknik dokümantasyon ve sunum materyallerinin hazırlanması.
* **Işıl Ceren Satıcı** - *Web Sunumu & Dokümantasyon*
    * Proje tanıtım web sitesinin tasarımı ve geliştirilmesi.
    * Flutter mobil uygulaması ve veri işleme algoritmaları.

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE UUID Tanımları (Seninkilerle aynı)
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-5678-1234-abcdefabcdef"

// Nesne Tanımlamaları
Adafruit_MPU6050 mpu;
TFT_eSPI tft = TFT_eSPI();

// Pin ve Değişkenler
const int BUTON_PIN = 0; 

int aktifMod = 0;
// Modlar: 0:Yurume, 1:Kosma, 2:Ziplama, 3:Yumruk, 4:Genel Ozet
int sayaclar[4] = {0, 0, 0, 0}; 
String modIsimleri[4] = {"YURUME", "KOSMA", "ZIPLAMA", "YUMRUK"};

unsigned long sonHareketZamani = 0;
unsigned long sonButonZamani = 0;
bool ekranGuncelle = true;
bool tamEkranTemizle = true;

// BLE Değişkenleri
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// BLE Bağlantı Callback
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    pServer->getAdvertising()->start();
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(BUTON_PIN, INPUT_PULLUP);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  Wire.begin(21, 22);

  // Sensör Başlatma
  if (!mpu.begin()) {
    tft.fillScreen(TFT_RED);
    tft.setCursor(10, 50);
    tft.setTextSize(2);
    tft.println("SENSOR YOK!");
    while (1) { delay(10); }
  }
  
  // Hassasiyet Ayarları
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); 

  tft.fillScreen(TFT_BLACK);

  // BLE Kurulumu
  BLEDevice::init("AkilliBileklik");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  BLEDevice::startAdvertising();

  ekranYonetici();
}

void loop() {
  // --- Buton Kontrolü ---
  if (digitalRead(BUTON_PIN) == LOW) {
    if (millis() - sonButonZamani > 300) {
      aktifMod = (aktifMod + 1) % 5; 
      ekranGuncelle = true;
      tamEkranTemizle = true;
      sonButonZamani = millis();
    } 
  }

  // --- Sensör Okuma ---
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // 1. Toplam Büyüklüğü Hesapla (Vektörel Toplam)
  float rawMag = sqrt(
    a.acceleration.x * a.acceleration.x +
    a.acceleration.y * a.acceleration.y +
    a.acceleration.z * a.acceleration.z
  );

  // 2. YERÇEKİMİNİ ÇIKAR (En Önemli Kısım)
  // Normalde dururken sensör ~9.8 okur. Bunu çıkarıyoruz.
  // "force" değeri artık sadece bizim uyguladığımız kuvveti gösterir.
  float force = abs(rawMag - 9.81); 

  // --- HAREKET ALGILAMA ---
  unsigned long now = millis();
  
  // HIZ LİMİTİ: Aynı hareketi üst üste saymasın diye bekleme süresi
  if (now - sonHareketZamani > 400) { // 400ms (Bir adım/hareket süresi için ideal)
    int idx = -1;
    
    // --- YENİ EŞİK DEĞERLERİ (Yerçekimi hariç saf kuvvet) ---
    // force < 3.0 ise : Ufak el oynaması, klavye, su içme vb. (YOK SAYILIR)
    
    if (force > 5.0 && force < 9.0) {
       // Hafif tempolu sallanma = Yürüme
       idx = 0; 
    } 
    else if (force >= 9.0 && force < 18.0) {
       // Sert sallanma = Koşma
       idx = 1; 
    }
    else if (force >= 18.0 && force < 32.0) {
       // Ani zıplama hareketi
       idx = 2; 
    }
    else if (force >= 32.0) {
       // Çok sert darbe = Yumruk
       idx = 3; 
    }

    // Hareket algılandıysa ve doğru moddaysak
    if (idx != -1 && (aktifMod == idx || aktifMod == 4)) {
      sayaclar[idx]++;
      sonHareketZamani = now;
      ekranGuncelle = true; 
      
      // Test İçin Serial Monitöre Yazdıralım
      Serial.print("Hareket Algılandı! Şiddet (Force): ");
      Serial.println(force);
    }
  }

  // --- Ekran Güncelleme ---
  if (ekranGuncelle) {
    ekranYonetici();
    ekranGuncelle = false;
  }

  // --- BLE Veri Gönderimi ---
  static unsigned long lastSend = 0;
  if (deviceConnected && millis() - lastSend > 500) { // BLE trafiğini rahatlatmak için 500ms
    char buffer[32];
    sprintf(buffer, "%d,%d,%d,%d", 
            sayaclar[0], sayaclar[1], sayaclar[2], sayaclar[3]);
    pCharacteristic->setValue(buffer);
    pCharacteristic->notify();
    lastSend = millis();
  }
}

// --- EKRAN YÖNETİMİ ---
void ekranYonetici() {
  if (tamEkranTemizle) {
    tft.fillScreen(TFT_BLACK);
    tamEkranTemizle = false;
  }
  
  if (aktifMod < 4) {
    // Tekli Mod
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(10, 10);
    tft.print(modIsimleri[aktifMod]);
    tft.print("     ");

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(6); 
    tft.setCursor(50, 60);
    tft.print(sayaclar[aktifMod]);
    tft.print("   ");
  } else {
    // Liste Modu
    tft.setTextSize(2);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.setCursor(30, 5);
    tft.print("GUNLUK DURUM  "); 
    
    for (int i = 0; i < 4; i++) {
      int yPozisyonu = 35 + (i * 25);
      
      tft.setCursor(0, yPozisyonu);
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      tft.print(" "); 
      tft.print(modIsimleri[i]);
      tft.print(": ");
      
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.print(sayaclar[i]);
      tft.print("   ");
    }
  }
}

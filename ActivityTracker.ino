#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE UUID
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-5678-1234-abcdefabcdef"

// Nesneler
Adafruit_MPU6050 mpu;
TFT_eSPI tft = TFT_eSPI();

// Pinler
const int BUTON_PIN = 0;

// Modlar
int aktifMod = 0;
// 0:Yurume, 1:Kosma, 2:Ziplama, 3:Yumruk, 4:Genel
int sayaclar[4] = {0, 0, 0, 0};
String modIsimleri[4] = {"YURUME", "KOSMA", "ZIPLAMA", "YUMRUK"};

unsigned long sonButonZamani = 0;
bool ekranGuncelle = true;
bool tamEkranTemizle = true;

// --- ZAMAN KONTROLLERİ ---
unsigned long sonYurumeZamani  = 0;
unsigned long sonKosmaZamani   = 0;
unsigned long sonRunAlgilama   = 0;
unsigned long sonZiplamaZamani = 0;
unsigned long sonYumrukZamani  = 0;

// BLE
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// BLE Callback
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

  if (!mpu.begin()) {
    tft.fillScreen(TFT_RED);
    tft.setCursor(10, 50);
    tft.setTextSize(2);
    tft.println("SENSOR YOK!");
    while (1) delay(10);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

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
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  ekranYonetici();
}

void loop() {

  if (digitalRead(BUTON_PIN) == LOW) {
    if (millis() - sonButonZamani > 300) {
      aktifMod = (aktifMod + 1) % 5;
      ekranGuncelle = true;
      tamEkranTemizle = true;
      sonButonZamani = millis();
    }
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float rawMag = sqrt(
    a.acceleration.x * a.acceleration.x +
    a.acceleration.y * a.acceleration.y +
    a.acceleration.z * a.acceleration.z
  );

  float force = abs(rawMag - 9.81);

  static float filteredForce = 0;
  filteredForce = 0.7 * filteredForce + 0.3 * force;

  unsigned long now = millis();
  int idx = -1;

  // ZIPLAMA
  if (filteredForce >= 18.0 && filteredForce < 32.0) {
    if (now - sonZiplamaZamani > 600) {
      idx = 2;
      sonZiplamaZamani = now;
    }
  }

  // YUMRUK
  else if (filteredForce >= 32.0) {
    if (now - sonYumrukZamani > 700) {
      idx = 3;
      sonYumrukZamani = now;
    }
  }

  // ✅ KOŞMA (ÖNCELİKLİ)
  else if (filteredForce >= 10.0 && filteredForce < 18.0) {
    if (now - sonKosmaZamani > 220) {
      idx = 1;
      sonKosmaZamani = now;
      sonRunAlgilama = now;
    }
  }

  // ✅ YÜRÜME (KOŞMA YOKSA)
  else if (filteredForce >= 5.5 && filteredForce < 10.0) {
    if (now - sonRunAlgilama > 1200) {
      if (now - sonYurumeZamani > 550) {
        idx = 0;
        sonYurumeZamani = now;
      }
    }
  }

  if (idx != -1 && (aktifMod == idx || aktifMod == 4)) {
    sayaclar[idx]++;
    ekranGuncelle = true;
    Serial.print("Force: ");
    Serial.println(filteredForce);
  }

  if (ekranGuncelle) {
    ekranYonetici();
    ekranGuncelle = false;
  }

  static unsigned long lastSend = 0;
  if (deviceConnected && millis() - lastSend > 500) {
    char buffer[32];
    sprintf(buffer, "%d,%d,%d,%d",
            sayaclar[0], sayaclar[1], sayaclar[2], sayaclar[3]);
    pCharacteristic->setValue(buffer);
    pCharacteristic->notify();
    lastSend = millis();
  }
}

void ekranYonetici() {
  if (tamEkranTemizle) {
    tft.fillScreen(TFT_BLACK);
    tamEkranTemizle = false;
  }

  if (aktifMod < 4) {
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
    tft.setTextSize(2);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.setCursor(30, 5);
    tft.print("GUNLUK DURUM  ");

    for (int i = 0; i < 4; i++) {
      int y = 35 + i * 25;
      tft.setCursor(0, y);
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

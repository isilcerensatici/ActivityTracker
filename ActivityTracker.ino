#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TFT_eSPI.h>


#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-5678-1234-abcdefabcdef"

Adafruit_MPU6050 mpu;
TFT_eSPI tft = TFT_eSPI();

const int BUTON_PIN = 0; 
int aktifMod = 0; 

int sayaclar[4] = {0, 0, 0, 0};
String modIsimleri[4] = {"YURUME", "KOSMA", "ZIPLAMA", "YUMRUK"};

unsigned long sonHareketZamani = 0;
bool guncellemeGerekiyor = true;


BLECharacteristic *pCharacteristic;
bool deviceConnected = false;


class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  pinMode(BUTON_PIN, INPUT_PULLUP);

  if (!mpu.begin()) {
    while (1) yield();
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_10_HZ); 

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);


  BLEDevice::init("AkilliBileklik");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  ekranYonetici();
}

void loop() {
  if (digitalRead(BUTON_PIN) == LOW) {
    aktifMod++;
    if (aktifMod > 4) aktifMod = 0; 
    guncellemeGerekiyor = true;
    delay(350); 
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float mag = sqrt(
    pow(a.acceleration.x, 2) +
    pow(a.acceleration.y, 2) +
    pow(a.acceleration.z, 2)
  );

  unsigned long suAn = millis();

  int guncelBekleme = (aktifMod == 1) ? 350 : 1000; 
  int analizModu = (aktifMod == 4) ? -1 : aktifMod; 

  if (suAn - sonHareketZamani > guncelBekleme) {
    int tespitEdilen = -1;

    if (mag > 14.5 && mag < 17.5)      tespitEdilen = 0; // YURUME
    else if (mag >= 17.5 && mag < 30)  tespitEdilen = 1; // KOSMA
    else if (mag >= 30 && mag < 38)    tespitEdilen = 2; // ZIPLAMA
    else if (mag >= 38)                tespitEdilen = 3; // YUMRUK

    if (tespitEdilen != -1) {
      if (analizModu == -1 || analizModu == tespitEdilen) {
        sayaclar[tespitEdilen]++;
        sonHareketZamani = suAn;
        guncellemeGerekiyor = true;
      }
    }
  }

  if (guncellemeGerekiyor) {
    ekranYonetici();
    guncellemeGerekiyor = false;
  }


  static unsigned long lastBleSend = 0;
  if (deviceConnected && millis() - lastBleSend > 300) {
    String json =
      "{"
      "\"walk\":" + String(sayaclar[0]) + "," +
      "\"run\":"  + String(sayaclar[1]) + "," +
      "\"jump\":" + String(sayaclar[2]) + "," +
      "\"punch\":"+ String(sayaclar[3]) +
      "}";

    pCharacteristic->setValue(json.c_str());
    pCharacteristic->notify();
    lastBleSend = millis();
  }
}

void ekranYonetici() {
  tft.fillScreen(TFT_BLACK);

  if (aktifMod < 4) {
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(3);
    tft.setCursor(10, 15);
    tft.println(modIsimleri[aktifMod]);
    tft.drawFastHLine(0, 50, 240, TFT_WHITE);

    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(8);
    tft.setCursor(65, 65);
    tft.println(sayaclar[aktifMod]);
  } else {
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    tft.setCursor(10, 5);
    tft.println("GUNLUK OZET");
    tft.drawFastHLine(0, 25, 240, TFT_WHITE);

    int yPos = 40;
    for (int i = 0; i < 4; i++) {
      tft.setCursor(10, yPos);
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(2);
      tft.print(modIsimleri[i]);

      tft.setCursor(170, yPos);
      tft.setTextColor(TFT_GREEN);
      tft.println(sayaclar[i]);

      yPos += 25;
    }
  }
}
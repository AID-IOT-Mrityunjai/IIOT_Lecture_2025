#include <IIOT_V4_Pinout.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// comment out to get Fahrenheit instead
#define TEMPERATURE_CELSIUS

#define BLE_SERVER_NAME    "BMP280_MJ"
#define SERVICE_UUID       "91bad492-b950-4226-aa2b-4ede9fa42f59"
#define CHAR_UUID_TEMP_C   "cba1d466-344c-4be3-ab3f-189f80dd7518"
#define CHAR_UUID_TEMP_F   "f78ebbff-c8b7-4107-93de-889a6a06d408"
#define CHAR_UUID_PRESS    "d7a2e6f4-8b2c-4f2a-a9e8-3d2b6c5f4a00"

Adafruit_BMP280 bmp;  // I2C

bool deviceConnected = false;
unsigned long lastTime = 0;
const unsigned long TIMER_DELAY = 30UL * 1000UL;

#ifdef TEMPERATURE_CELSIUS
  BLECharacteristic tempChar(CHAR_UUID_TEMP_C, BLECharacteristic::PROPERTY_NOTIFY);
#else
  BLECharacteristic tempChar(CHAR_UUID_TEMP_F, BLECharacteristic::PROPERTY_NOTIFY);
#endif
BLECharacteristic pressChar(CHAR_UUID_PRESS, BLECharacteristic::PROPERTY_NOTIFY);

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* /*pServer*/)   { 
    deviceConnected = true; 
    Serial.println("→ BLE client connected"); 
  }
  void onDisconnect(BLEServer* /*pServer*/){ 
    deviceConnected = false;
    Serial.println("→ BLE client disconnected");
  }
};

void initSensor() {
  Serial.printf("I²C pins: SDA=%d  SCL=%d\n", PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  bool found = false;

  Serial.println("Trying BMP280 at 0x76...");
  if (bmp.begin(0x76)) {
    Serial.println("BMP280 found at 0x76");
    found = true;
  } else {
    Serial.println("…not found. Trying 0x77...");
    if (bmp.begin(0x77)) {
      Serial.println("BMP280 found at 0x77");
      found = true;
    }
  }

  if (!found) {
    Serial.println("BMP280 not detected on either address!  Continuing without sensor readings.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("=== STARTUP ===");

  initSensor();

  Serial.println("Initializing BLE…");
  BLEDevice::init(BLE_SERVER_NAME);
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* service = pServer->createService(SERVICE_UUID);

  service->addCharacteristic(&tempChar);
  tempChar.addDescriptor(new BLE2902());

  service->addCharacteristic(&pressChar);
  pressChar.addDescriptor(new BLE2902());

  service->start();

  Serial.println("Starting advertising…");
  BLEAdvertising* pAdv = BLEDevice::getAdvertising();
  pAdv->addServiceUUID(SERVICE_UUID);
  pAdv->setScanResponse(true);
  pAdv->start();

  Serial.println("BLE setup complete. Waiting for client…");
}

// A tiny BLE-only test: uncomment this block, comment out the rest of loop(),
// and you’ll see “Hello” notify every 5s—so you know BLE is actually working.
#if 0
void loop() {
  static unsigned long t0 = millis();
  if (millis() - t0 < 5000) return;
  t0 = millis();
  static BLECharacteristic* testChar = nullptr;
  if (!testChar) {
    BLEServer* srv = BLEDevice::getServer();
    BLEService* svc = srv->getService(SERVICE_UUID);
    testChar = svc->createCharacteristic("abcd", BLECharacteristic::PROPERTY_NOTIFY);
    testChar->addDescriptor(new BLE2902());
    svc->start();
  }
  testChar->setValue("Hello");
  testChar->notify();
  Serial.println("→ BLE Test: sent ‘Hello’");
}
#else
void loop() {
  if (!deviceConnected) {
    delay(200);
    return;
  }

  if (millis() - lastTime < TIMER_DELAY) {
    return;
  }
  lastTime = millis();

  float temperature = bmp.readTemperature();
  float pressure    = bmp.readPressure() / 100.0F; // Pa → hPa

  char tempStr[8], presStr[8];
#ifdef TEMPERATURE_CELSIUS
  dtostrf(temperature, 6, 2, tempStr);
#else
  dtostrf(temperature * 1.8f + 32.0f, 6, 2, tempStr);
#endif
  dtostrf(pressure, 7, 2, presStr);

  tempChar.setValue((uint8_t*)tempStr, strlen(tempStr));
  tempChar.notify();

  pressChar.setValue((uint8_t*)presStr, strlen(presStr));
  pressChar.notify();

#ifdef TEMPERATURE_CELSIUS
  Serial.printf("Temp: %s °C", tempStr);
#else
  Serial.printf("Temp: %s °F", tempStr);
#endif
  Serial.printf("  Pressure: %s hPa\n", presStr);
}
#endif

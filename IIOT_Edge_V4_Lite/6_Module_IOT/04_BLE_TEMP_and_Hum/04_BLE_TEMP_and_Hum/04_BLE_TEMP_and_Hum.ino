/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second.
*/

// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #include <BLE2902.h>
// #include <Adafruit_BMP280.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <Adafruit_Sensor.h>
// #include <IIOT_V4_Pinout.h>
// #include <Wire.h>

// BLEServer *pServer = NULL;
// BLECharacteristic *pTxCharacteristic;
// bool deviceConnected = false;
// bool oldDeviceConnected = false;
// uint8_t txValue = 0;
// char Stringtosend[7];
//   String data_send;

// // See the following for generating UUIDs:
// // https://www.uuidgenerator.net/

// #define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
// #define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
// #define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
// //#define SEALEVELPRESSURE_HPA (1050)
// #define SCREEN_WIDTH 128  // OLED display width, in pixels
// #define SCREEN_HEIGHT 64  // OLED display height, in pixels

// class MyServerCallbacks : public BLEServerCallbacks {
//   void onConnect(BLEServer *pServer) {
//     deviceConnected = true;
//   };

//   void onDisconnect(BLEServer *pServer) {
//     deviceConnected = false;
//   }
// };

// class MyCallbacks : public BLECharacteristicCallbacks {
//   void onWrite(BLECharacteristic *pCharacteristic) {
//     String rxValue =(String) pCharacteristic->getValue().c_str();

//     if (rxValue.length() > 0) {
//       Serial.println("*********");
//       Serial.print("Received Value: ");
//       for (int i = 0; i < rxValue.length(); i++) {
//         Serial.print(rxValue[i]);
//       }

//       Serial.println();
//       Serial.println("*********");
//     }
//   }
// };

// Adafruit_BMP280 bmp;
// float temperature, humidity, pressure, altitude;
// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// void setup() {
//   Serial.begin(115200);
//    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
//     delay(100);

//     bmp.begin(0x76);

//     if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
//         Serial.println(F("SSD1306 allocation failed"));
//         for (;;);
//     }
//     delay(2000);

//   // Create the BLE Device
//   BLEDevice::init("UART Service");

//   // Create the BLE Server
//   pServer = BLEDevice::createServer();
//   pServer->setCallbacks(new MyServerCallbacks());

//   // Create the BLE Service
//   BLEService *pService = pServer->createService(SERVICE_UUID);

//   // Create a BLE Characteristic
//   pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

//   pTxCharacteristic->addDescriptor(new BLE2902());

//   BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

//   pRxCharacteristic->setCallbacks(new MyCallbacks());

//   // Start the service
//   pService->start();

//   // Start advertising
//   pServer->getAdvertising()->start();
//   Serial.println("Waiting a client connection to notify...");

//   }

// void loop() {

//    display.clearDisplay();
//     display.setTextSize(1);
//     display.setTextColor(WHITE);
//     display.setCursor(0, 20);

//     temperature = bmp.readTemperature();
//     dtostrf(temperature,4,2,Stringtosend);
//     std::string data_send = "Temperature is ";
//     data_send = data_send + Stringtosend ;
//     data_send=data_send+"\n";
//     // humidity = bmp.readHumidity();
//     //pressure = bmp.readPressure() / 100.0F;
//     //altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);

//     display.print("Temp = ");
//     display.print(bmp.readTemperature());
//     display.println(" *C");

//    /* display.print("Pressure = ");
//     display.print(bmp.readPressure() / 100.0F);
//     display.println(" hPa");

//     display.print("Alt = ");
//     display.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
//     display.println(" m"); */

//     display.println();

//     display.display();
//     delay(1000);

//   if (deviceConnected) {
//     pTxCharacteristic->setValue(data_send);
//     pTxCharacteristic->notify();
//     //txValue++;
//     delay(10);  // bluetooth stack will go into congestion, if too many packets are sent
//   }

//   // disconnecting
//   if (!deviceConnected && oldDeviceConnected) {
//     delay(500);                   // give the bluetooth stack the chance to get things ready
//     pServer->startAdvertising();  // restart advertising
//     Serial.println("start advertising");
//     oldDeviceConnected = deviceConnected;
//   }
//   // connecting
//   if (deviceConnected && !oldDeviceConnected) {
//     // do stuff here on connecting
//     oldDeviceConnected = deviceConnected;
//   }
// }

#include <IIOT_V4_Pinout.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

//Default Temperature is in Celsius
//Comment the next line for Temperature in Fahrenheit
#define temperatureCelsius

//BLE server name
#define bleServerName "BMP280_ESP32"

Adafruit_BMP280 bmp; // I2C

float temp;
float tempF;
float hum;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

bool deviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"

// Temperature Characteristic and Descriptor
#ifdef temperatureCelsius
  BLECharacteristic BMPTemperatureCelsiusCharacteristics("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor BMPTemperatureCelsiusDescriptor(BLEUUID((uint16_t)0x2902));
#else
  BLECharacteristic BMPTemperatureFahrenheitCharacteristics("f78ebbff-c8b7-4107-93de-889a6a06d408", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor BMPTemperatureFahrenheitDescriptor(BLEUUID((uint16_t)0x2902));
#endif

// Humidity Characteristic and Descriptor
BLECharacteristic BMPHumidityCharacteristics("ca73b3ba-39f6-4ab3-91ae-186dc9577d99", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor BMPHumidityDescriptor(BLEUUID((uint16_t)0x2903));

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void initBMP(){
     Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
}

void setup() {
  // Start serial communication 
  Serial.begin(115200);

  // Init BMP Sensor
  initBMP();

  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *BMPService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and Create a BLE Descriptor
  // Temperature
  #ifdef temperatureCelsius
    BMPService->addCharacteristic(&BMPTemperatureCelsiusCharacteristics);
    BMPTemperatureCelsiusDescriptor.setValue("BMP temperature Celsius");
    BMPTemperatureCelsiusCharacteristics.addDescriptor(&BMPTemperatureCelsiusDescriptor);
  #else
    BMPService->addCharacteristic(&BMPTemperatureFahrenheitCharacteristics);
    BMPTemperatureFahrenheitDescriptor.setValue("BMP temperature Fahrenheit");
    BMPTemperatureFahrenheitCharacteristics.addDescriptor(&BMPTemperatureFahrenheitDescriptor);
  #endif  

  // Humidity
  BMPService->addCharacteristic(&BMPHumidityCharacteristics);
  BMPHumidityDescriptor.setValue("BMP humidity");
  BMPHumidityCharacteristics.addDescriptor(new BLE2902());
  
  // Start the service
  BMPService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    if ((millis() - lastTime) > timerDelay) {
      // Read temperature as Celsius (the default)
      temp = bmp.readTemperature();
      // Fahrenheit
      tempF = 1.8*temp +32;
      // Read humidity
      hum = bmp.readTemperature(); // Actually sending Pressure 
  
      //Notify temperature reading from BMP sensor
      #ifdef temperatureCelsius
        static char temperatureCTemp[6];
        dtostrf(temp, 6, 2, temperatureCTemp);
        //Set temperature Characteristic value and notify connected client
        BMPTemperatureCelsiusCharacteristics.setValue(temperatureCTemp);
        BMPTemperatureCelsiusCharacteristics.notify();
        Serial.print("Temperature Celsius: ");
        Serial.print(temp);
        Serial.print(" ºC");
      #else
        static char temperatureFTemp[6];
        dtostrf(tempF, 6, 2, temperatureFTemp);
        //Set temperature Characteristic value and notify connected client
        BMPTemperatureFahrenheitCharacteristics.setValue(temperatureFTemp);
        BMPTemperatureFahrenheitCharacteristics.notify();
        Serial.print("Temperature Fahrenheit: ");
        Serial.print(tempF);
        Serial.print(" ºF");
      #endif
      
      //Notify humidity reading from BMP
      static char humidityTemp[6];
      dtostrf(hum, 6, 2, humidityTemp);
      //Set humidity Characteristic value and notify connected client
      BMPHumidityCharacteristics.setValue(humidityTemp);
      BMPHumidityCharacteristics.notify();   
      Serial.print(" - Humidity: ");
      Serial.print(hum);
      Serial.println(" %");
      
      lastTime = millis();
    }
  }
}
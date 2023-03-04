#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED 2
#define BUTTON_PIN 21

BLECharacteristic *pCharacteristic;
bool connected = false;

#define BLE_SERVER_NAME "Singularity"
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class Callbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Device connected");
    connected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Device disconnected");
    connected = false;
  }
};

class ReadCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string msg = pCharacteristic->getValue();

      if (msg.length() > 0) {
        Serial.print("Message: ");
        for (int i = 0; i < msg.length(); i++) {
          Serial.print(msg[i]);
        }
        Serial.println();
      }
    }
};

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  BLEDevice::init(BLE_SERVER_NAME);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new Callbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new ReadCallbacks());

  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE started, waiting for connection...");
}

unsigned long long a = 0;
void loop() {
  if (connected) {
    if (a + 1000 < millis()) {
      a = millis();
      String value = String(digitalRead(BUTTON_PIN));
      char data[2];
      value.toCharArray(data, 2);

      pCharacteristic->setValue("zdar\n");
      pCharacteristic->notify();
      Serial.println("Message sent");
    }
  }

  //delay(1000);
}

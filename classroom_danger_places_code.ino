#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEAdvertising.h>


#define BEACON_NAME "CLASSROOM-101" 


void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Beacon: " + String(BEACON_NAME));

  BLEDevice::init(BEACON_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  
  BLEAdvertisementData advData;
  advData.setName(BEACON_NAME);
  advData.setFlags(0x06); 
  
  pAdvertising->setAdvertisementData(advData);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0); 
  
  BLEDevice::startAdvertising();
  Serial.println("✅ Beacon started advertising.");
}

void loop() {
  
  delay(2000);
}

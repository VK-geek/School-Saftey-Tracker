

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <SimpleKalmanFilter.h>
#include <vector>
#include <map>

// --- User Configuration (Update with your details) ---
const char* WIFI_SSID     = "vishal";
const char* WIFI_PASSWORD = "vishal23";
const char* SERVER_IP     = "10.161.136.75";
const char* CHILD_ID      = "student-01"; 

// --- Constants ---
#define CLASSROOM_RSSI_THRESHOLD -40
#define DANGER_RSSI_THRESHOLD    -45
#define CLASSROOM_TX_POWER -42
#define DANGER_TX_POWER    -61
#define PATH_LOSS_EXPONENT 2.0

// --- Global Objects & Variables ---
SimpleKalmanFilter classroomKF(2, 2, 0.01);
SimpleKalmanFilter dangerKF(2, 2, 0.01);
std::vector<String> classroomBeacons;
std::vector<String> dangerBeacons;
std::map<String, int> foundBeaconsRssi;
static BLEScan* pBLEScan;

// (Helper functions like fetchConfigFromServer, rssiToDistance, etc. are unchanged)
// ...

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        String deviceName = advertisedDevice.getName().c_str();
        bool isKnownBeacon = false;
        for (const auto& name : classroomBeacons) if (name == deviceName) { isKnownBeacon = true; break; }
        if(!isKnownBeacon) {
          for (const auto& name : dangerBeacons) if (name == deviceName) { isKnownBeacon = true; break; }
        }
        if (isKnownBeacon) {
            foundBeaconsRssi[deviceName] = advertisedDevice.getRSSI();
        }
    }
};

bool fetchConfigFromServer() {
    // This function remains the same as the previous version
    return true; // Placeholder
}

float rssiToDistance(int rssi, int txPower) {
    // This function remains the same
    return 0.0; // Placeholder
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected!");
  if (!fetchConfigFromServer()) { delay(5000); ESP.restart(); }
  
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

void loop() {
  foundBeaconsRssi.clear();
  pBLEScan->start(1, false);

  int closestClassroomRssi = -1000;
  String closestClassroomName = "none";
  int closestDangerRssi = -1000;
  String closestDangerName = "none";

  // Find closest classroom
  for (const auto& name : classroomBeacons) {
      if (foundBeaconsRssi.count(name) && foundBeaconsRssi[name] > closestClassroomRssi) {
          closestClassroomRssi = foundBeaconsRssi[name];
          closestClassroomName = name;
      }
  }
  // Find closest danger zone
  for (const auto& name : dangerBeacons) {
      if (foundBeaconsRssi.count(name) && foundBeaconsRssi[name] > closestDangerRssi) {
          closestDangerRssi = foundBeaconsRssi[name];
          closestDangerName = name;
      }
  }

  int filteredClassroomRssi = classroomKF.updateEstimate((closestClassroomRssi == -1000) ? 0 : closestClassroomRssi);
  int filteredDangerRssi = dangerKF.updateEstimate((closestDangerRssi == -1000) ? 0 : closestDangerRssi);

  float classroomDist = rssiToDistance(filteredClassroomRssi, CLASSROOM_TX_POWER);
  float dangerDist = rssiToDistance(filteredDangerRssi, DANGER_TX_POWER);
  bool alertState = (filteredClassroomRssi != 0 && filteredDangerRssi != 0 &&
                     filteredClassroomRssi < CLASSROOM_RSSI_THRESHOLD && 
                     filteredDangerRssi > DANGER_RSSI_THRESHOLD);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://" + String(SERVER_IP) + ":8000/update");
    http.addHeader("Content-Type", "text/plain");

    // --- NEW 7-PART PAYLOAD ---
    String payload = String(CHILD_ID) + "," + 
                     closestClassroomName + "," + String(classroomDist) + "," +
                     closestDangerName + "," + String(dangerDist) + "," +
                     (alertState ? "1" : "0") + "," + String(WiFi.RSSI());
    
    http.POST(payload);
    http.end();
  }
  
  delay(1500);
}

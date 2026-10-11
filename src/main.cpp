#include <M5Cardputer.h>
#include <WiFi.h>
#include <NimBLEDevice.h>

enum Screen { HOME, WIFI_RESULTS, BLE_RESULTS, ABOUT };
Screen screen = HOME;
String wifiLines[12];
int wifiCount = 0;
String bleLines[12];
int bleCount = 0;
String statusLine = "READY";

void drawHeader(const char* title) {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setCursor(4, 4);
  M5Cardputer.Display.println("NIGHTSHADE // v0.1");
  M5Cardputer.Display.drawFastHLine(4, 16, 232, TFT_DARKGREY);
  M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5Cardputer.Display.setCursor(4, 22);
  M5Cardputer.Display.println(title);
}
void drawHome() {
  drawHeader("MAIN MENU");
  M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5Cardputer.Display.setCursor(4, 39); M5Cardputer.Display.println("[1] Wi-Fi discovery");
  M5Cardputer.Display.setCursor(4, 54); M5Cardputer.Display.println("[2] BLE discovery");
  M5Cardputer.Display.setCursor(4, 69); M5Cardputer.Display.println("[3] About / limits");
  M5Cardputer.Display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  M5Cardputer.Display.setCursor(4, 99); M5Cardputer.Display.println("Del: home");
  M5Cardputer.Display.setCursor(4, 116); M5Cardputer.Display.printf("STATUS: %s", statusLine.c_str());
}
void drawList(const char* title, String* lines, int count) {
  drawHeader(title);
  int shown = min(count, 6);
  for (int i = 0; i < shown; i++) {
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.setCursor(4, 36 + i * 14);
    M5Cardputer.Display.println(lines[i]);
  }
  if (!count) { M5Cardputer.Display.setCursor(4, 42); M5Cardputer.Display.println("No results recorded."); }
  M5Cardputer.Display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  M5Cardputer.Display.setCursor(4, 119); M5Cardputer.Display.println("Del: home");
}
void scanWiFi() {
  screen = WIFI_RESULTS; statusLine = "SCANNING WIFI";
  drawList("WIFI DISCOVERY", wifiLines, 0);
  WiFi.mode(WIFI_STA); WiFi.disconnect(); delay(100);
  int n = WiFi.scanNetworks(false, true); wifiCount = 0;
  if (n >= 0) {
    for (int i = 0; i < n && wifiCount < 12; i++) {
      String auth = WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "OPEN" : "SECURED";
      String name = WiFi.SSID(i); if (!name.length()) name = "<hidden>";
      if (name.length() > 13) name = name.substring(0, 13);
      wifiLines[wifiCount++] = name + " " + String(WiFi.RSSI(i)) + "dBm " + auth;
    }
    statusLine = "WIFI SCAN COMPLETE";
  } else statusLine = "WIFI SCAN FAILED";
  WiFi.scanDelete(); drawList("WIFI DISCOVERY", wifiLines, wifiCount);
}
class ScanCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* device) override {
    if (bleCount >= 12) return;
    String name = device->haveName() ? String(device->getName().c_str()) : String("<unnamed>");
    if (name.length() > 12) name = name.substring(0, 12);
    String addr = String(device->getAddress().toString().c_str());
    bleLines[bleCount++] = name + " " + addr.substring(max(0, (int)addr.length() - 5)) + " " + String(device->getRSSI()) + "dBm";
  }
};
ScanCallbacks scanCallbacks;
void scanBLE() {
  screen = BLE_RESULTS; bleCount = 0; statusLine = "SCANNING BLE";
  drawList("BLE DISCOVERY", bleLines, 0);
  NimBLEDevice::init("");
  NimBLEScan* scan = NimBLEDevice::getScan();
  scan->setScanCallbacks(&scanCallbacks, false);
  scan->setActiveScan(true); scan->setInterval(45); scan->setWindow(15);
  scan->start(5, false); scan->stop(); NimBLEDevice::deinit(true);
  statusLine = "BLE SCAN COMPLETE"; drawList("BLE DISCOVERY", bleLines, bleCount);
}
void drawAbout() {
  drawHeader("ABOUT / LIMITS");
  M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5Cardputer.Display.setCursor(4, 36); M5Cardputer.Display.println("Standalone ESP32-S3 tools.");
  M5Cardputer.Display.setCursor(4, 50); M5Cardputer.Display.println("Discovery is not proof of a flaw.");
  M5Cardputer.Display.setCursor(4, 64); M5Cardputer.Display.println("No deauth, jamming, credential");
  M5Cardputer.Display.setCursor(4, 78); M5Cardputer.Display.println("capture, or device spam in v0.1.");
  M5Cardputer.Display.setCursor(4, 119); M5Cardputer.Display.println("Del: home");
}
void setup() {
  auto cfg = M5.config(); M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1); M5Cardputer.Display.setTextSize(1);
  WiFi.mode(WIFI_STA); WiFi.disconnect(); drawHome();
}
void loop() {
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    auto state = M5Cardputer.Keyboard.keysState();
    for (auto c : state.word) {
      if (screen == HOME) {
        if (c == '1') scanWiFi();
        else if (c == '2') scanBLE();
        else if (c == '3') { screen = ABOUT; drawAbout(); }
      }
    }
    if (state.del) { screen = HOME; drawHome(); }
  }
  delay(10);
}

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <FirebaseESP32.h>

FirebaseData fbdo;
#define FLASH_LED_PIN 4
// Khởi tạo WebServer tại cổng 80
WebServer server(80);

// Hàm khởi tạo máy chủ stream
void startCameraServer() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", "<h1>ESP32-CAM Stream</h1><img src=\"/stream\">");
  });

  server.on("/stream", HTTP_GET, []() {
    WiFiClient client = server.client();
    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
    client.print(response);

    while (1) {
      camera_fb_t *fb = esp_camera_fb_get();
      if (!fb) continue;

      client.print("--frame\r\n");
      client.print("Content-Type: image/jpeg\r\n\r\n");
      client.write(fb->buf, fb->len);
      client.print("\r\n");

      esp_camera_fb_return(fb);

      // Nếu client ngắt kết nối, thoát vòng lặp
      if (!client.connected()) break;
    }
  });

  server.begin();
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(1000);

  //  Cấu hình WiFiManager
  WiFiManager wifiManager;
  wifiManager.setConnectTimeout(10);         // Timeout nếu không kết nối WiFi đã lưu
  wifiManager.setConfigPortalTimeout(180);   // Timeout khi vào cấu hình 192.168.4.1

  // Nếu cần reset WiFi đã lưu thì bỏ comment dòng này
  // wifiManager.resetSettings();

  // Tự động kết nối hoặc tạo portal cấu hình
  if (!wifiManager.autoConnect("ESP32-CAM-Setup", "88888888")) {
    Serial.println(" Failed to connect, restarting...");
    delay(3000);
    ESP.restart();
  }

  //  In địa chỉ IP khi kết nối thành công
  Serial.println(" WiFi connected!");
  Serial.print(" Stream Ready! Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/stream");

  // Khởi tạo camera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("❌ Camera init failed");
    return;
  }

  // Bắt đầu WebServer
  startCameraServer();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  // Gửi IP lên Firebase
  String streamURL = "http://" + WiFi.localIP().toString() + "/stream";
  if (Firebase.setString(fbdo, "/esp32cam/stream_url", streamURL)) {
    Serial.println(" Stream URL uploaded to Firebase:");
    Serial.println(streamURL);
  } else {
    Serial.println("Failed to upload stream URL");
    Serial.println(fbdo.errorReason());
  }

 pinMode(FLASH_LED_PIN, OUTPUT);
 digitalWrite(FLASH_LED_PIN, HIGH); // BẬT đèn LED flash

}

unsigned long lastSentTime = 0;
const unsigned long interval = 60000; // gửi mỗi 60 giây
void loop() {
  server.handleClient();  // xử lý yêu cầu client

  if (millis() - lastSentTime > interval) {
  lastSentTime = millis();
  String streamURL = "http://" + WiFi.localIP().toString() + "/stream";
  Firebase.setString(fbdo, "/esp32cam/stream_url", streamURL);
    }
}


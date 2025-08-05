#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>  // Thư viện cấu hình WiFi tự động
#include <FirebaseESP32.h>

// Thông tin Firebase
#define FIREBASE_HOST "https://iothome-esp32-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "RHrjnv1jw5jLTZ9yuwAAbmXlox1DSs5DWN2GqVCw"

FirebaseData fbdo;

#define FLASH_LED_PIN 4

// vào IP 192.168.4.1 nếu không kết nối được WiFi đã lưu
  // cấu hình ESP32 CAM ("ESP32-CAM-Setup", "88888888")) 

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

  // ⚙️ Cấu hình WiFiManager
  WiFiManager wifiManager;
  wifiManager.setConnectTimeout(10);         // Timeout nếu không kết nối WiFi đã lưu
  wifiManager.setConfigPortalTimeout(180);   // Timeout khi vào cấu hình 192.168.4.1

  // Nếu cần reset WiFi đã lưu thì bỏ comment dòng này
  // wifiManager.resetSettings();

  // Tự động kết nối hoặc tạo portal cấu hình
  if (!wifiManager.autoConnect("ESP32-CAM-Setup", "88888888")) {
    Serial.println("❌ Failed to connect, restarting...");
    delay(3000);
    ESP.restart();
  }

  // ✅ In địa chỉ IP khi kết nối thành công
  Serial.println("✅ WiFi connected!");
  Serial.print("📸 Stream Ready! Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/stream");

  // Cấu hình camera ESP32-CAM AI Thinker
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sccb_sda = 26;
  config.pin_sccb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 30000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_HVGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

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
    Serial.println("✅ Stream URL uploaded to Firebase:");
    Serial.println(streamURL);
  } else {
    Serial.println("❌ Failed to upload stream URL");
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


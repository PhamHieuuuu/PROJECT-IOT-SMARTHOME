#include <FirebaseESP32.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>

bool relayState[] = {false, false, false, false, false, false};  // Trạng thái của 6 relay
#define relay1 23    
#define relay2 19
#define relay3 18
#define relay4 5
#define relay5 32
#define relay6 33
int relays[] = {relay1, relay2, relay3, relay4, relay5, relay6};  // Mảng chứa các chân Relay

String trangthairelay[] = {
    "/button1",   // Relay 1
    "/button2",   // Relay 2
    "/button3",   // Relay 3
    "/button4",  // Relay 4
    "/button5",  // Relay 5
    "/button6",  // Relay 6
    "/C"
};

// Kích thước màn hình OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Tạo đối tượng SSD1306
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000;

AsyncWebServer server(80);
Preferences preferences;

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Lưu cấu hình đã nhập 
void handleSave(AsyncWebServerRequest *request) {
  if (request->hasParam(PARAM_SSID, true) && request->hasParam(PARAM_PASS, true)) {
    String ssid = request->getParam(PARAM_SSID, true)->value();
    String password = request->getParam(PARAM_PASS, true)->value();

    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("pass", password);
    preferences.end();

    request->send_P(200, "text/html", successPage);
    delay(1000);
    ESP.restart();
  } else {
    request->send(400, "text/plain", "Thiếu tham số");
  }
}

// Chế độ AP
void startAPMode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("IOT Home", "88888888");
  Serial.println("🚀 Đã bật AP cho cấu hình WiFi");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", htmlPage);
  });

  server.on("/save", HTTP_POST, handleSave);

  server.begin();
}

// Kết nối lại khi mất WiFi
void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt >= reconnectInterval) {
      lastReconnectAttempt = now;
      Serial.println("🔄 Đang thử kết nối lại WiFi...");
      WiFi.disconnect();
      WiFi.reconnect();
      if (WiFi.status() == WL_CONNECTED) {
      configTime(7 * 3600, 0, "pool.ntp.org", "time.google.com", "time.cloudflare.com");}
    }
  }
}

//Gửi dữ liệu lên Firebase số nguyên
void sendFirebase(String path, int value) {
  if (Firebase.setInt(firebaseData, path, value)) {
  } else {
    Serial.println("Lỗi khi gửi dữ liệu!");
  }
}

//Gửi dữ liệu lên Firebase số thực
void sendFirebase(String path, float value) {
  if (Firebase.setFloat(firebaseData, path, value)) {
  } else {
    Serial.println("Lỗi khi gửi dữ liệu float!");
  }
}

// Đọc dữ liệu từ Firebase
int receiveFirebase(String path) {
  if (Firebase.getInt(firebaseData, path)) {
    int data = firebaseData.intData();
    return data;
  } else {
    Serial.println("Lỗi khi nhận dữ liệu!");
    return -1; // Trả về -1 nếu lỗi
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);  // RXD2=16, TXD2=17
  // Kết nối WiFi
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", "");
  String pass = preferences.getString("pass", "");
  preferences.end();

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay(); 

    for (int i = 0; i < 6; i++) {  
      pinMode(relays[i], OUTPUT);
      digitalWrite(relays[i], 0);
    }

    lcd.init();
    lcd.backlight();
    lcd.createChar(0, degreeSymbol); 

  // Cấu hình Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  if (ssid != "") {
    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.print("🔄 Đang kết nối WiFi: ");
    Serial.println(ssid);
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      Serial.println(" Kết nối WiFi thành công!");
      // Khởi tạo Firebase
      Firebase.begin(&config, &auth);
      Firebase.reconnectWiFi(true);
    } else {
      Serial.println("❌ Kết nối thất bại. Bật chế độ cấu hình.");
      startAPMode();
    }
  } else {
    Serial.println("⚙️ Chưa có cấu hình WiFi. Bật chế độ cấu hình.");
    startAPMode();
  }
    configTime(7 * 3600, 0, "pool.ntp.org", "time.google.com", "time.cloudflare.com");//ESP32 kết nối NTP Server và lấy thời gian một lần duy nhất. khi khởi động

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay(); 

    for (int i = 0; i < 6; i++) {  
      pinMode(relays[i], OUTPUT);
      digitalWrite(relays[i], 0);
    }
    // đồng bô trạng thái nút nhấn 
    for (int i = 0; i < 6; i++) {
    sendFirebase(trangthairelay[i],0) ;
    }
}


// Hàm gửi tin nhắn và cuộc gọi cảnh báo lên Telegram
void sendTelagramCall(String message) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://api.callmebot.com/start.php?source=web&user=" + String(User) +
                     "&text=" + message + "&lang=vi-VN-Standard-C";

        Serial.println("📤 Đang gửi: " + url);
        http.begin(url);
        int httpResponseCode = http.GET();
        Serial.println("Đã gửi !");

        http.end();
    } else {
        Serial.println("⚠ WiFi chưa kết nối!");
    }
}

// getTimeFromNTP(); lấy dữ liệu time cho việc gửi tin nhắn
String getTimeFromNTP() {
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "Lỗi+NTP!";
    }
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%H:%M:%S %d/%m/%Y", &timeinfo);
    String formattedTime = String(timeString);
    
    // Chuyển ký tự đặc biệt sang URL Encoding
    formattedTime.replace(" ", "+");  // Dấu cách
    formattedTime.replace(":", "%3A");  // Dấu hai chấm
    formattedTime.replace("/", "%2F");  // Dấu gạch chéo
    
    return formattedTime;
}

// Đọc dữ liệu UART
float nhietDo = 0.0;
float doAm = 0.0;
String doam_str;
String nhietdo_str;
int Value_Gas;
int sendFlag = 1;

// Các biến flag để báo có dữ liệu mới
bool newDHTData = false;
bool newGasData = false;
bool newRelayData = false;
// Đọc dữ liệu UART
String uartBuffer = "";
void readSerial2() {
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\n') {
      uartBuffer.trim();
      Serial.println("🔍 Nhận dữ liệu: " + uartBuffer); // Debug: In ra dữ liệu nhận được

      if (uartBuffer.indexOf(',') > 0) {
        int commaIndex = uartBuffer.indexOf(',');
        nhietDo = uartBuffer.substring(0, commaIndex).toFloat();
        doAm = uartBuffer.substring(commaIndex + 1).toFloat();
        newDHTData = true;
        Serial.println("📡 Dữ liệu DHT: Nhiệt độ = " + String(nhietDo, 1) + "°C, Độ ẩm = " + String(doAm, 1) + "%"); // Debug: In ra nhiệt độ và độ ẩm
      } 
      
      else if (uartBuffer.startsWith("L")) {
        int relayNum = uartBuffer.substring(1).toInt();
        if (relayNum >= 1 && relayNum <= 6) {
          relayState[relayNum - 1] = !relayState[relayNum - 1];
          digitalWrite(relays[relayNum - 1], relayState[relayNum - 1]);
          newRelayData = true;
          Serial.println("🔄 Thay đổi trạng thái Relay " + String(relayNum) + ": " + (relayState[relayNum - 1] ? "BẬT" : "TẮT")); // Debug: In ra trạng thái relay
        }

      } else if (uartBuffer.startsWith("G")) {
        Value_Gas = uartBuffer.substring(1).toInt();
        newGasData = true;
        Serial.println("🚨 Nhận dữ liệu Gas: " + String(Value_Gas)); // Debug: In ra giá trị gas
      }

      uartBuffer = ""; // Xóa buffer sau khi xử lý
    } else {
      uartBuffer += c;
    }
  }
}

int lastCState = 0;
// truyền thông Firebase
void handleSensorFirebase() {
    if (WiFi.status() != WL_CONNECTED) {
    // ❌ Không có WiFi => không gửi/nhận Firebase
    return;
  }
  if (newDHTData) {
    sendFirebase("DoAm", doAm);
    sendFirebase("NhietDo", nhietDo);
    Serial.println("📥 DHT: " + String(nhietDo,1) + "°C, " + String(doAm,1) + "%");
    newDHTData = false;
  }

  if (newRelayData) {
    for (int i = 0; i < 6; i++) {
      sendFirebase(trangthairelay[i], relayState[i] ? 1 : 0);
    }
    newRelayData = false;
  }

  if (newGasData) {
    if (Value_Gas == 0) {
      sendFirebase("/Gas", 0);
      sendFlag = 1;
    } else {
      if (sendFlag == 1) {
        String currentTime = getTimeFromNTP();
        sendFirebase("/Gas", 1);
        sendFlag = 0;
      }
    }
    newGasData = false;
  }

  // Xử lý relay 
  for (int i = 0; i < 6; i++) {
    int state = receiveFirebase(trangthairelay[i]); // Đọc trạng thái từ Firebase
    if (state != -1 && state != relayState[i]) {  // Chỉ xử lý khi có thay đổi
      relayState[i] = state;
      digitalWrite(relays[i], relayState[i]);
      Serial.println("🔄 Relay " + String(i + 1) + ": " + (relayState[i] ? "BẬT" : "TẮT"));
    }
  }
    // Xử lý nút "Cửa" (ở vị trí 6 trong mảng)
  int cState = receiveFirebase(trangthairelay[6]);
  if (cState != -1 && cState == 1 && lastCState == 0) {
    // Chỉ gửi khi chuyển từ 0 -> 1
    Serial.println("Cửa: gửi sang tín hiệu ");
    Serial2.println("C1");

    // Reset về 0 sau khi gửi
    sendFirebase(trangthairelay[6], 0);
  }
  lastCState = cState;
}

bool flagGas = false;       // Phát hiện gas
bool autoGasControl = false;  // Cờ relay được bật do gas, dùng để biết khi nào cần tắt

//Hiển thị OLED
void Display_Oled() {
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.setCursor(2,0);
  lcd.print(nhietDo);
  lcd.setCursor(7,0);  
  lcd.write(byte(0));      
  lcd.print("C");

  lcd.setCursor(0,1);
  lcd.print("AH:");
  lcd.setCursor(3,1);
  lcd.print(doAm);
  lcd.setCursor(8,1);
  lcd.print("%");

  lcd.setCursor(10,0);
  lcd.print("|");
  lcd.setCursor(10,1);
  lcd.print("|");
  lcd.setCursor(12,0);
  lcd.print("GAS");

  if(Value_Gas==1){ 
    lcd.setCursor(12,1);
    lcd.print("YES");
    flagGas=true;
  } else {
    lcd.setCursor(12,1);
    lcd.print("NO ");
    flagGas = false;

  }

}

void Gass() {
  if (flagGas) {
    // Nếu phát hiện gas thì bật relay 6 nếu chưa bật
    if (!relayState[5]) {
      digitalWrite(relays[5], HIGH);
      relayState[5] = true;
      autoGasControl = true;  // Đánh dấu là bật do gas
    }
  } else {
    // Nếu không phát hiện gas, và relay trước đó được bật do gas thì mới tắt
    if (autoGasControl) {
      digitalWrite(relays[5], LOW);
      relayState[5] = false;
      autoGasControl = false;
    }
  }
}

bool  FlagConfi = false ;
void loop() {

    if (WiFi.status() == WL_CONNECTED && FlagConfi == false) {
    // Khởi tạo Firebase
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    configTime(7 * 3600, 0, "pool.ntp.org", "time.google.com", "time.cloudflare.com");//ESP32 kết nối NTP Server và lấy thời gian một lần duy nhất. khi khởi động
    FlagConfi = true;
    // Serial.println(" WiFi OK ");
  } else if(WiFi.status() != WL_CONNECTED) {
    Serial.println(" Chưa có WiFi. Chờ kết nối lại...");
    reconnectWiFi();
  }

  readSerial2();            // Đọc dữ liệu UART
  handleSensorFirebase();   // Gửi dữ liệu Firebase nếu có
  Display_Oled();
  Gass();
}


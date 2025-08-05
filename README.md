# Hệ thống IoT giám sát và điều khiển thiết bị gia đình

## 📌 Giới thiệu
Dự án này phát triển một **hệ thống IoT** cho gia đình, cho phép **giám sát** và **điều khiển** các thiết bị cơ bản như đèn, quạt, điều hòa, cửa… thông qua **ứng dụng di động** và **website**.  
Hệ thống sử dụng **ESP32** và **ATmega328P**, kết hợp với **Firebase** để đồng bộ dữ liệu và hỗ trợ điều khiển từ xa.

## ⚙️ Chức năng chính
- **Giám sát môi trường:**
  - Nhiệt độ & độ ẩm (DHT11)
  - Phát hiện khí gas (MQ-2) và cảnh báo qua **Telegram**
- **Điều khiển thiết bị:**
  - 6 relay điều khiển thiết bị điện từ xa (web/app) hoặc cục bộ (keypad, IR, button)
  - Servo motor điều khiển cửa (nhiều phương thức mở)
- **Cảnh báo an toàn:**
  - Gửi tin nhắn & cuộc gọi qua Telegram khi phát hiện khí gas
  - Hiển thị thông báo đẩy trên web/app
- **Giao diện người dùng:**
  - OLED hiển thị thời gian, nhiệt độ, độ ẩm, trạng thái gas
  - LCD hiển thị trạng thái mật khẩu & thông báo

## 🛠️ Phần cứng sử dụng
- **Vi điều khiển:** ATmega328P, ESP32-WROOM-32
- **Cảm biến:** DHT11, MQ-2, IR Receiver
- **Thiết bị điều khiển:** Relay 5V (6 kênh), Servo SG90, Module keypad 4x4, Module button ADC
- **Hiển thị:** OLED 0.96", LCD1602
- **Nguồn:** LM2596 5V

## 💻 Phần mềm & nền tảng
- **Arduino IDE** (lập trình ATmega328P & ESP32)
- **Firebase** (cơ sở dữ liệu thời gian thực)
- **Telegram CallMeBot API** (cảnh báo)
- **NTP Server** (đồng bộ thời gian)
- **Web**: HTML/CSS/JavaScript (kết nối Firebase)
- **App**: Android Studio (Kotlin/Java)

## 📜 Cách hoạt động
1. **ATmega328P** thu thập dữ liệu từ cảm biến & xử lý các lệnh cục bộ → gửi sang ESP32 qua UART.
2. **ESP32** nhận dữ liệu, gửi lên Firebase, hiển thị lên OLED, nhận lệnh điều khiển từ web/app.
3. Khi phát hiện khí gas → gửi cảnh báo Telegram + cập nhật trạng thái lên Firebase.
4. Người dùng điều khiển relay/cửa từ xa hoặc tại chỗ, dữ liệu luôn đồng bộ giữa các thiết bị.

## 📷 Hình ảnh minh họa
_(Upload Photo)_


> 📌 **Tác giả:** PhamHieuu
> 🕒 **Thời gian:** 2025  
> 📍 **Mục tiêu:** Giải pháp nhà thông minh, an toàn, tiết kiệm năng lượng

#include <IRremote.h>
#include <DHT.h>
#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

/*  Mã phím IR
Phím 1: 4077715200
Phím 2: 3877175040
Phím 3: 2707357440
Phím 4: 4144561920
Phím 5: 3810328320
Phím 6: 2774204160
Phím Play: 3927310080
*/
//  LCD SDA-A4  |  SCL-A5

/*
       KẾT NỐI
Arduino        Ngoại vi
D2-D9   ----   Keypad 4x4
D10     ----   DHT11
D11     ----   Gas
D12     ----   Servo
A0      ----   Buzz
A3      ----   IR
A4-A5   ----   I2C
A1      ----   Button ADC
*/
#define Buzz A0
#define Button A1
#define Ir A3
#define PINDHT 10
#define Gas 11

int DHTTYPE= DHT11;
DHT dht (PINDHT,DHTTYPE);

Servo dc;

LiquidCrystal_I2C lcd(0x27, 16, 2); 

byte colpin[4] = {2, 3, 4, 5}; // Chân kết nối cột của keypad   C1 | C2 | C3 | C4
byte rowpin[4] = {6, 7, 8, 9}; // Chân kết nối hàng của keypad  H1 | H2 | H3 | H4

char keys[4][4] = {         // Mảng ký tự của keypad
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
char matkhau[] = {'4', '4', '4', '4'}; // Mật khẩu gốc
char nhap[4]; // Mảng lưu trữ các ký tự nhập vào
int viTri = 0; // Biến đếm vị trí nhập ký tự
Keypad kytu = Keypad(makeKeymap(keys), rowpin, colpin, 4, 4); // Khởi tạo keypad
bool nhapBatDau = false; // Biến kiểm soát việc bắt đầu nhập mật khẩu

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  pinMode(Buzz, OUTPUT);
  digitalWrite(Buzz,1);
  
  pinMode(Button, INPUT);
  pinMode(Ir, INPUT);
  pinMode(Gas, INPUT_PULLUP);

  IrReceiver.begin(Ir, false);  // Khởi động bộ thu IR
  dht.begin();

  dc.attach(12);
  dc.write(0);


}

void Coi(int ms){
  for(int i=0 ;i<ms ; i++){
  digitalWrite(Buzz,0);
  delay(50);
  digitalWrite(Buzz,1);
  delay(50);
  }
}

// ĐỌC GIÁ TRỊ TỪ HỒNG NGOẠI
unsigned long lastTime_Ir = 0;  // Lưu thời gian lần gần nhất xử lý tín hiệu
void Read_Ir(){
    if (IrReceiver.decode()) {  // Kiểm tra xem có nhận được tín hiệu IR không
    unsigned long nhan = IrReceiver.decodedIRData.decodedRawData;  // Lưu dữ liệu nhận được
    if (nhan > 0) {
   
      // Kiểm tra khoảng thời gian từ lần xử lý gần nhất
      if (millis() - lastTime_Ir > 200) {
             switch (nhan){
                case 4077715200: Serial.println("L1"); break;
                case 3877175040: Serial.println("L2"); break;
                case 2707357440: Serial.println("L3"); break;
                case 4144561920: Serial.println("L4"); break;
                case 3810328320: Serial.println("L5"); break;
                case 2774204160: Serial.println("L6"); break;
                case 3927310080: Serial.println("C1");
                  dc.write(90);
                  delay(1000);
                  dc.write(0);
                 break;
             }
        }
      }
      lastTime_Ir = millis();  // Cập nhật lại thời gian xử lý lần gần nhất
    }
    IrReceiver.resume();  // Chuẩn bị nhận tín hiệu IR tiếp theo
  }


  /*
phim 1: 0
Phim 2: 535
Phim 3: 704
Phim 4: 786
Phim 5: 835
Phim 6: 866
*/

//ĐỌC GIÁ TRỊ ADC TỪ MODULE BUTTON
void Read_Button(){ 
 int Button_Value = analogRead(Button); 
  if (Button_Value  < 60) {           // Phím 1 = ADC ~0
    delay(300); 
    Serial.println("L1");
  } else if (Button_Value  < 535) {   // Phím 2 = ADC ~506
    delay(300); 
    Serial.println("L2");
  } else if (Button_Value  < 705) {   // Phím 3 = ADC ~682
    delay(300); 
    Serial.println("L3");
  } else if (Button_Value  < 786) {   // Phím 4 = ADC ~770
    delay(300); 
    Serial.println("L4");
  } else if (Button_Value  < 830) {   // Phím 5 = ADC ~821
    delay(300); 
    Serial.println("L5");
  } else if (Button_Value  < 860) {   // Phím 6 = ADC ~855
    delay(300); 
    Serial.println("L6");
 }
}

//ĐỌC GIÁ TRỊ TỪ DHT11
unsigned long lastTime_Dht = 0;
float doam;
float nhietdo;
void Read_Dht(){
   if(millis() - lastTime_Dht > 5000){
  
    doam =dht.readHumidity();
    nhietdo =dht.readTemperature();

    String doam_str = String(doam, 1);
    String nhietdo_str = String(nhietdo, 1);

    // hiển thi serial
    Serial.print(nhietdo_str);
    Serial.print(",");
    Serial.println(doam_str);
    lastTime_Dht = millis();
   }
}

//ĐỌC GIÁ TRỊ TỪ CẢM BIẾN GAS
unsigned long lastTime_Gas = 0;
void Read_Gas(){
   if(millis() - lastTime_Gas > 500){
    int Value_Gas = digitalRead(Gas);
    // hiển thi serial
     if(Value_Gas){
       Serial.println("G0");
       digitalWrite(Buzz,1);
     }
     else { 
      Serial.println("G1");
      digitalWrite(Buzz,0);}
    lastTime_Gas = millis();
   }
}

// Đọc dữ liệu từ ESP32
void readSerial() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');  // Đọc chuỗi đến khi gặp '\n'
    data.trim();  // Xóa khoảng trắng và ký tự xuống dòng

    if (data == "C1") {  // So sánh chính xác
      dc.write(90);
      delay(1000);
      dc.write(0);
    }
  }
}



// XỬ LÝ LỚP BẢO MẬT CỬA
void key4x4() {
  char giatri = kytu.getKey(); // Đọc giá trị từ keypad
  lcd.setCursor(2, 0);
  lcd.print("Nhap Mat Khau");

    if (!nhapBatDau) { // đảo !nhapbatdau nếu là true sẽ thự hiện lệnh trong if
    lcd.setCursor(0, 1);
    lcd.print("Doi Mat Khau (D)");}

  if (giatri) { // Nếu nhấn phím
   nhapBatDau = true; // để khi đảo sẽ không thực hiện lệnh  if (!nhapBatDau), để chỉ hiển thị dấu * dòng 2
  lcd.setCursor(0, 1);
  lcd.print("                "); // In 16 khoảng trống để xóa dòng thứ 2 (tương ứng với màn hình LCD 16x2)

  lcd.setCursor(0+7, 1); // Đặt lại con trỏ về đầu dòng thứ 2
  for (int i = 0; i < viTri; i++) { // In lại các ký tự '*' đã nhập
    lcd.print('*');
  }                    
  lcd.setCursor(viTri+7, 1); // In dấu * cho ký tự mới nhập
  lcd.print('*');
  
  nhap[viTri] = giatri; // Lưu giá trị của phím vừa nhập
  viTri++; // Tăng vị trí
   Coi(1);
    if (viTri == 4) { // Nếu nhập đủ 4 ký tự
     delay(300);
      if (nhap[0] == matkhau[0] && nhap[1] == matkhau[1] && nhap[2] == matkhau[2] && nhap[3] == matkhau[3]) { // Kiểm tra mật khẩu
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Mat Khau Dung");
        dc.write(90);
        Coi(5);
        delay(1000);
        dc.write(0);
        delay(1000);
        lcd.clear();
        viTri = 0;
         nhapBatDau = false; // khi là false sẽ đảo ở dòng if (!nhapBatDau) thành true và thực hiện lệnh if
      } else {
        delay(300);
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Sai Mat Khau");
        digitalWrite(Buzz,0);
        delay(1000);
        digitalWrite(Buzz,1);
        viTri = 0;
        nhapBatDau = false;
      }
    }

    if (giatri == 'D' && viTri == 1) { // Nếu nhấn phím 'D' để đổi mật khẩu
    nhapBatDau = false;// tạo dòng hiển thị dòng 2
      viTri = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nhap Mat Khau Cu");
      while (viTri < 4) {
        char mk = kytu.getKey();
        if (mk) {
          lcd.setCursor(viTri + 7, 1);
          lcd.print('*'); // Hiển thị ký tự *
          nhap[viTri] = mk;
          Coi(1);
          viTri++;
        }
      }
      if (nhap[0] == matkhau[0] && nhap[1] == matkhau[1] && nhap[2] == matkhau[2] && nhap[3] == matkhau[3]) {
        delay(300);
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Mat Khau Dung");
        Coi(5);
        delay(1000);
        lcd.clear();
        viTri = 0;
        lcd.setCursor(2, 0);
        lcd.print("Nhap Key Moi");
        for (int i = 0; i < 4; i++) {
          while (true) {
            char chon = kytu.getKey();
            if (chon) {
              lcd.setCursor(i + 7, 1);
              lcd.print('*'); 
              matkhau[i] = chon; // Cập nhật mật khẩu mới
              Coi(1);
              break;
            }
          }
        }
        delay(300);
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Doi Mat Khau");
        lcd.setCursor(3,1);
        lcd.print("Thanh Cong");
        Coi(5);
        delay(1000);
        lcd.clear();
        viTri = 0;
      } else {
        delay(300);
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Sai Mat Khau");
        lcd.setCursor(2, 1);
        lcd.print("Huy Doi Key");
        digitalWrite(Buzz,0);
        delay(1000);
        digitalWrite(Buzz,1);
        lcd.clear();
        viTri = 0;
      }
    }
  }
}

void loop() {
  
    Read_Ir();
  Read_Gas();
  Read_Button();
    Read_Ir();
  key4x4();
  Read_Dht();
    Read_Ir();
  readSerial();

}

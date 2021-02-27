// 操縱可變電阻，改變舵機角度並顯示訊息到LCD上

#include <Wire.h>  // Arduino IDE 內建
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <IRremote.h>

// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址
Servo myservo; // 舵機
int sensor = 0; // 可變電阻類比訊號值
int angle = 0; // 將可變電阻讀取的值轉成0~180的對應關係
const int STEP_ANGLE = 10;

int RECV_PIN = 3; //A0;

IRrecv irrecv(RECV_PIN);

decode_results results;

int turnDir = 0;
void setup() {
  //Serial.begin(9600);
  
  // 啟動伺服馬達，接在9號腳位
  myservo.attach(9);
  myservo.write(0);
  //angle = myservo.read();
  
  // 紅外線接收器
  irrecv.enableIRIn(); // Start the receiver
  
  // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
  lcd.begin(16, 2); 
  lcd.clear();
  
  // 輸出初始化文字
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("System is ready.");
  
  delay(1000);
}

// 重設LCD的文字，向左向右或不動
// angle: 目前的角度值，diff: 和上一次角度的差值
void resetLCDText(int angle, int diff){
  lcd.clear();
  if(diff > 0){
    lcd.setCursor(0, 0); // 設定游標位置在第一行行首
    lcd.print("Turn left");
  }else if(0 == diff){
    lcd.setCursor(0, 0); // 設定游標位置在第一行行首
    lcd.print("No action");
  }else{
    lcd.setCursor(0, 0); // 設定游標位置在第一行行首
    lcd.print("Turn right");
  }
  
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("now angle: " + String(angle));
}

void loop() {
  if (irrecv.decode(&results) && results.value != 0) {
    //Serial.println(results.value, HEX);
    // 印到 Serial port 
    /*Serial.print("irCode: ");           
    Serial.print(results.value, HEX);    // 紅外線編碼
    Serial.print(",  Decimal: ");           
    Serial.print(results.value);    // 紅外線編碼
    Serial.print(",  bits: ");          
    Serial.println(results.bits);        // 紅外線編碼位元數*/
    
    switch(results.value){
      case 0x00FF10EF: // 按左鍵
        if(angle + STEP_ANGLE <= 180){
          angle += STEP_ANGLE;
          myservo.write(angle); // 調整馬達角度到絕對位置(0~180)  
          resetLCDText(angle, 2);
        }
        turnDir = -1;
        break;
      case 0x00FF5AA5: // 向右轉
        if(angle - STEP_ANGLE >= 0){
          angle -= STEP_ANGLE;
          myservo.write(angle); // 調整馬達角度到絕對位置(0~180)  
          resetLCDText(angle, -2);
        }
        turnDir = 1;
        break;
      case 0xFFFFFFFF: // 連續按
        if(turnDir == -1){
          if(angle + STEP_ANGLE <= 180){
            angle += STEP_ANGLE;
            myservo.write(angle); // 調整馬達角度到絕對位置(0~180)  
            resetLCDText(angle, 2);
          }
        }else if(turnDir == 1){
           if(angle - STEP_ANGLE >= 0){
            angle -= STEP_ANGLE;
            myservo.write(angle); // 調整馬達角度到絕對位置(0~180)  
            resetLCDText(angle, -2);
          }
        }
        break;
    }
    irrecv.resume(); // Receive the next value
  }  
  
  //Serial.println("angle: " + String(angle)); // 0~100 0~1023

  delay(50);
}

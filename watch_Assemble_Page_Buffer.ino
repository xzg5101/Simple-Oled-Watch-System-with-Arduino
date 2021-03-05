#include <Arduino.h>
#include <U8g2lib.h>
#include <RTClib.h>
#include <Wire.h>

#define SMALLFONT  u8g2_font_ncenR12_te                         //date and tempeturature
#define CHFONT     u8g2_font_wqy12_t_gb2312a                    //days of the week
#define LARGEFONT  u8g2_font_fub30_tf                           //Hour and minute
#define ICONFONT   u8g2_font_open_iconic_all_2x_t               //icon
#define OFONT      u8g2_font_4x6_tf                             //others

U8G2_SSD1327_EA_W128128_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

int topBorder = 12;
bool lastState = false;

const int buttonPin = 10;
int buttonState = 0;
bool scrOn = true;

int displayTime = 15;

int avgPressDur = 750;

//variables for recording time
int startHour;
int startMin;
int startSec;
bool recording = false;


void setup(void) {
  //Serial.begin(9600);
  //init Button
  pinMode(buttonPin, INPUT);
  
  //init u8g2
  u8g2.begin();
  u8g2.enableUTF8Print();
  
  
  //init RTC DS3231
  if (! rtc.begin()) {
    abort();
  }

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  if (rtc.lostPower()) {
    //Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
}

void loop(void) {
 
  checkButton();
  delay(250);
}

void checkButton(){

  lastState == scrOn;
  buttonState = digitalRead(buttonPin);
  if(buttonState == HIGH){
    scrOn = !scrOn;
  }
  while(buttonState == HIGH){

    buttonState = digitalRead(buttonPin);
    delay(avgPressDur);
  }



  if(lastState != scrOn){
    if(lastState == true && scrOn == false){

      u8g2.clearDisplay();
      while(buttonState == LOW){
        buttonState = digitalRead(buttonPin);
        delay(avgPressDur);
      }
      
      
    }

    else if(lastState == false && scrOn == true){
      int lightDur=0;
      updtScr();
      buttonState = digitalRead(buttonPin);

      while(buttonState == LOW){
        buttonState = digitalRead(buttonPin);
        delay(avgPressDur);
        lightDur+=avgPressDur;
        //Serial.println(lightDur);
        if(lightDur == displayTime*600){
          u8g2.setContrast(50);
          updtScr();
        }
        if(lightDur > displayTime*1000){
          break;
        }
      }

      u8g2.clearDisplay();
      u8g2.setContrast(255);
      //clearScr();
      scrOn = false;
    }
    else{}
  }

}


void updtScr(){
  DateTime now = rtc.now();

  u8g2.firstPage();
  do {
    u8g2.setFont(SMALLFONT);
    u8g2.setCursor(25, topBorder);
    u8g2.print(now.year(), DEC);
    u8g2.print('.');
    if (now.month() < 10)u8g2.print('0');
    u8g2.print(now.month(), DEC);
    u8g2.print('.');
    if (now.day() < 10)u8g2.print('0');
    u8g2.print(now.day(), DEC);


    u8g2.setFont(CHFONT);
    u8g2.setCursor(32, topBorder + 15);
    u8g2.print(daysOfTheWeek[now.dayOfTheWeek()]);
    u8g2.print(" ");
    if (now.hour() < 12)u8g2.print("上午");
    else u8g2.print("下午");


    u8g2.setFont(LARGEFONT);
    u8g2.setCursor(12, topBorder + 51);
    if (now.hour() < 10)u8g2.print('0');
    u8g2.print(now.hour(), DEC);
    u8g2.print(':');
    if (now.minute() < 10)u8g2.print('0');
    u8g2.print(now.minute(), DEC);    
    
    u8g2.setCursor(25, topBorder+70);
    u8g2.setFont(ICONFONT);
    u8g2.print('|');
    //u8g2.print(' ');

    u8g2.setCursor(43, topBorder+68);
    u8g2.setFont(SMALLFONT);
    u8g2.print(' ');
    u8g2.print(rtc.getTemperature());

    u8g2.setFont(OFONT);

    u8g2.setCursor(90, topBorder+60);
    u8g2.print('o');

    u8g2.setFont(SMALLFONT);
    u8g2.setCursor(94, topBorder+68);
    u8g2.print('C');

    specialDay(now);

  }
  while ( u8g2.nextPage() );
}

void clearScr(){
  u8g2.firstPage();
  do {
    u8g2.setFont(SMALLFONT);
    u8g2.setCursor(0, 0);
    u8g2.print(" ");
  }
  while ( u8g2.nextPage() );

}


void specialDay(DateTime now){
  u8g2.setFont(CHFONT);
  
  
  if(now.month() == 10 && now.day() == 1){
      u8g2.setCursor(46, topBorder+83);
      u8g2.print("国庆节");
  }
  else if(now.month() == 9 && now.day() == 27){
      u8g2.setCursor(40, topBorder+83);
      u8g2.print("生日快乐");
  }
  else if(now.month() == 4 && now.day() == 1){
      u8g2.setCursor(46, topBorder+83);
      u8g2.print("愚人节");
  }
  
  else if(now.month() == 5 && now.day() == 1){
      u8g2.setCursor(46, topBorder+83);
      u8g2.print("劳动节");
  }

  else if(now.month() == 6 && now.day() == 1){
      u8g2.setCursor(46, topBorder+83);
      u8g2.print("儿童节");
  }
  
  else if(now.month() == 9 && now.day() == 10){
      u8g2.setCursor(46, topBorder+83);
      u8g2.print("教师节");
  }

  else if(now.month() == 10 && now.day() == 31 && now.hour() > 12){
      u8g2.setCursor(46, topBorder+83);
      u8g2.print("万圣节");
  }
  
  else if(now.month() == 12 && now.day() == 24 && now.hour() > 12){
      u8g2.setCursor(46, topBorder+83);
      u8g2.print("平安夜");
  }
  
  else if(now.month() == 12 && now.day() == 25){
      u8g2.setCursor(46, topBorder+83);
      u8g2.print("圣诞节");
  }

  else if(now.month() == 3 && now.day() == 8){
      u8g2.setCursor(46, topBorder+83);
      u8g2.print("妇女节");
  }

  else if(now.month() == 6){
      if(now.day()> 14 && now.day() < 22)
        if(now.dayOfTheWeek() == 0){
          u8g2.setCursor(35, topBorder+83);
          u8g2.print("母亲节快乐");
        }
  }

  else if(now.month() == 5){
      if(now.day()> 7 && now.day() < 15)
        if(now.dayOfTheWeek() == 0){
          u8g2.setCursor(46, topBorder+83);
          u8g2.print("父亲节");
        }
  }
  
  
  else if(now.month() == 1 && now.day() == 1){
      u8g2.setCursor(48, topBorder+83);
      u8g2.print("元旦");
  }

  else {
    u8g2.setCursor(35, topBorder+83);
    u8g2.print("平常的一天");
  }
  

}

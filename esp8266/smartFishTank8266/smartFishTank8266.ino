#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

char ssid[]     = "360WiFi-48681F";//这里是我的wifi，你使用时修改为你要连接的wifi ssid
char password[] = "dianxin151";//你要连接的wifi密码
char host[] = "192.168.100.5";//修改为手机的的tcpServer服务端的IP地址，即手机在路由器上的ip
WiFiClient client;
const int tcpPort = 8484;
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


// i2c 1602 13 15
// ds18b20 水温  14
//#define pinWarmLight 5//太阳灯
#define pinDHT11 2 // 室温
#define pinColorLED 0 // 彩灯
#define pinWaterMotor 4 // 水泵

#define pinLEDR 16
#define pinLEDG 5
#define pinLEDB 12


SimpleDHT11 dht11;
byte dhtHum = 0.0; //温度
byte dhtTem = 0.0;//湿度

LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(14);
DallasTemperature sensors(&oneWire);

int status = WL_IDLE_STATUS;

int H, S, V;
bool RGBEnable = false;
int R, G, B;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(30);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // 初始化引脚
  pinMode(pinColorLED, OUTPUT);
  pinMode(pinWaterMotor, OUTPUT);
  pinMode(pinLEDR, OUTPUT);
  pinMode(pinLEDG, OUTPUT);
  pinMode(pinLEDB, OUTPUT);
  digitalWrite(pinColorLED, LOW);
  digitalWrite(pinWaterMotor, LOW);
  analogWrite(pinLEDR, 0);
  analogWrite(pinLEDG, 0);
  analogWrite(pinLEDB, 0);

  // 初始化1602
  lcdInit();
  // 初始化18b20
  sensors.begin();
  // 连上WIFI
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)//WiFi.status() ，这个函数是wifi连接状态，返回wifi链接状态
  {
    delay(500);
    Serial.print(".");
  }//如果没有连通向串口发送.....

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());//WiFi.localIP()返回8266获得的ip地址
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected())//几个非连接的异常处理
  {
    reconnect();
  }

  while (client.available())//，无线读取到的数据转发到到串口
  {
    String s = client.readStringUntil('\n');  // 接收服务器的消息,eg:"windCtrl_1"
    Serial.println(s);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(s);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      continue;
    }
    String m = root["M"];
    int v = root["V"];
    Serial.println(m);
    Serial.println(v);
    if (m == "WM") { // 处理 water motor
      waterMotorFunc(v);
    }  else if (m == "CL") { //处理 Color LED
      colorLEDFunc(v);
    } else if (m == "LL") { // 处理LCD背光
      lcdBckLightFunc(v);
    } else if (m == "RL") { // 处理RBG LED
      String n = root["N"];
      RGBLEDFunc(n, v);
    }
  }
  dht11Func();
  ds18b20Func();
}

void reconnect() {
  while (!client.connected()) {
    status = WiFi.status();
    if (status != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
      }
    }
    else {
      if (!client.connect(host, tcpPort))
      {
        Serial.print("disconnect");
        //client.stop();
        delay(500);
      } else {}
    }
  }
}


unsigned long lastdht11Send = 0;
void dht11Func()
{
  if (lastdht11Send == 0 || millis() - lastdht11Send >= 3000) {
    lastdht11Send = millis();

    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(pinDHT11, &dhtTem, &dhtHum, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT11 failed, err=");
      Serial.println(err);
      lcd.home();
      lcd.print("                   ");
      lcd.home();
      lcd.print("DHT Error");
      delay(2000);
      return;
    }
    Serial.print("Humidity(%):");
    Serial.println(dhtHum);
    Serial.print("Temperature(℃):");
    Serial.println(dhtTem);
    Serial.println("Sending current DHT11 status ...");
    //    String sendData = "{\"M\":\"update\",\"ID\":\"" + String(ID) + "\",\"temIn\":" + String(dhtTem) + ",\"humIn\":" + String(dhtHum) + "}\n";
    String sendData = "{\"M\":\"TemHouse\",\"V\":" + String(dhtTem) + "}";
    client.print(sendData);
    delay(100);
    lcd.home();
    lcd.print("                   ");
    lcd.home();
    lcd.print("Hum:" + String(dhtHum) + " " + "Tem:" + String(dhtTem));
  }
}

unsigned long lastds18b20Send = 0;
void ds18b20Func() {
  if (lastds18b20Send == 0 || millis() - lastds18b20Send >= 3002) {
    lastds18b20Send = millis();
    sensors.requestTemperatures();
    float data = sensors.getTempCByIndex(0); //Temperature for the device 1 (index 0)
    lcd.setCursor(0, 1);
    lcd.print("                   ");
    lcd.setCursor(0, 1);
    lcd.print("Water Tem:" + String(data));
  }
}
void waterMotorFunc(int value) {
  if (value == 1) {
    digitalWrite(pinWaterMotor, HIGH);
  } else if (value == 0) {
    digitalWrite(pinWaterMotor, LOW);
  } else {
    Serial.println("waterMotorFunc Error");
  }
}



void colorLEDFunc(int value) {
  if (value == 1) {
    digitalWrite(pinColorLED, HIGH);
  } else if (value == 0) {
    digitalWrite(pinColorLED, LOW);
  } else {
    Serial.println("colorLEDFunc Error");
  }
}

void lcdBckLightFunc(int value) {
  if (value == 1) {
    lcd.setBacklight(HIGH);

  } else if (value == 0) {
    lcd.setBacklight(LOW);
  } else {
    Serial.println("lcdBckLightFunc Error");
  }
}
void lcdInit() {
  Wire.begin(13, 15);
  lcd.begin(13, 15);     // In ESP8266-01, SDA=0, SCL=2
  lcd.backlight();
  //  lcd.noAutoscroll();
  lcd.noCursor();
  lcd.home();
  lcd.print("Smart Fish Tank");
  lcd.setCursor(0, 1);
  lcd.print("  Hello Wo-Ki");
  delay(1000);
}

void RGBLEDFunc(String n, int v) {
  if (n == "P") {
    RGBEnable = v == 1 ? true : false;
  }
  if (n == "H") {
    H = v;
  }
  else if (n == "S") {
    S = v;
  }
  else if (n == "V") {
    V = v;
  }
  else {
    Serial.println("Error RGBLEDFunc");
  }
  HSV2RGB();
}
void HVSToRGB() {
  if (RGBEnable == true) {
    if (S == 0) {
      R = V; G = V; B = V;
    } else {
      //      H /= 60.0;
      int i = H / 60;
      float f = H - i;
      int a = V * (1 - S);
      int b = V * (1 - S * f);
      int c = V * (1 - S * (1 - f));
      switch (i) {
        case 0: R = V; G = c; B = a; break;
        case 1: R = b; G = V; B = a; break;
        case 2: R = a; G = V; B = c; break;
        case 3: R = a; G = b; B = V; break;
        case 4: R = c; G = a; B = V; break;
        //        case 5: R = V; G = a; B = b; break;
        default:
          R = V; G = a; B = b;
          Serial.println("Error HVSToRGB");
      }
    }


  } else {
    R = 0; G = 0; B = 0;
    H = 0; S = 0; V = 0;
  }
  changeRGB();
}

void HSV2RGB() {
  int i;
  float RGB_min, RGB_max;
  RGB_max = V * 2.55f;
  RGB_min = RGB_max * (100 - S) / 100.0f;

  i = H / 60;
  int difs = H % 60; // factorial part of h

  // RGB adjustment amount by hue
  float RGB_Adj = (RGB_max - RGB_min) * difs / 60.0f;

  switch (i) {
    case 0:
      R = RGB_max;
      G = RGB_min + RGB_Adj;
      B = RGB_min;
      break;
    case 1:
      R = RGB_max - RGB_Adj;
      G = RGB_max;
      B = RGB_min;
      break;
    case 2:
      R = RGB_min;
      G = RGB_max;
      B = RGB_min + RGB_Adj;
      break;
    case 3:
      R = RGB_min;
      G = RGB_max - RGB_Adj;
      B = RGB_max;
      break;
    case 4:
      R = RGB_min + RGB_Adj;
      G = RGB_min;
      B = RGB_max;
      break;
    default:        // case 5:
      R = RGB_max;
      G = RGB_min;
      B = RGB_max - RGB_Adj;
      break;
  }
  changeRGB();
}

void changeRGB() {
  Serial.println("RGB:" + String(R) + " " + String(G) + " " + String(B));
  Serial.println("HSV:" + String(H) + " " + String(S) + " " + String(V));
  analogWrite(pinLEDR, R);
  analogWrite(pinLEDG, G);
  analogWrite(pinLEDB, B);
}

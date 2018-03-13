#include <ESP8266WiFi.h>
#include <ArduinoJson.h>


char ssid[]     = "360WiFi-48681F";//这里是我的wifi，你使用时修改为你要连接的wifi ssid
char password[] = "dianxin151";//你要连接的wifi密码
char host[] = "192.168.100.5";//修改为手机的的tcpServer服务端的IP地址，即手机在路由器上的ip

WiFiClient client;
const int tcpPort = 8484;

#define pinLEDR 16
#define pinLEDG 0
#define pinLEDB 2
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
  analogWrite(pinLEDR, 0);
  analogWrite(pinLEDG, 0);
  analogWrite(pinLEDB, 0);
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

  while (client.available())
  {
    String s = client.readStringUntil('\n');
    Serial.println(s);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(s);
    if (!root.success()) {
      Serial.println("parseObject() failed");
      continue;
    }
    String m = root["M"];
    int v = root["V"];
    if (m == "RL") { // 处理RBG LED
      String n = root["N"];
      RGBLEDFunc(n, v);
    }
  }
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

void RGBLEDFunc(String n, int v) {
  if (n == "P") {
    if (v == 0) {
      H = 0; V = 0; S = 0;
    }
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

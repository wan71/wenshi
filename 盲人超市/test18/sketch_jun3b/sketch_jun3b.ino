#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <NTPClient.h>

// 定义引脚
#define LED_PIN_1 9
#define BUZZER_PIN 16
#define EEPROM_SIZE 1500

// 创建Web服务器对象
ESP8266WebServer server(80);
Ticker ledTicker;

// 定义MQTT相关变量
char mqtt_server[40];
int mqtt_port;
char deviceID[40];
char productID[40];
char apiKey[200];

String commandTopic;

WiFiClient espClient;
PubSubClient OneNET(espClient);

// 定义RC522模块引脚
#define RST_PIN 5
#define SS_PIN_1 4
#define SS_PIN_2 0
#define SS_PIN_3 2
#define SS_PIN_4 1 // 新增
#define SS_PIN_5 3 // 新增

// 创建RC522实例
MFRC522 mfrc522_1(SS_PIN_1, RST_PIN);
MFRC522 mfrc522_2(SS_PIN_2, RST_PIN);
MFRC522 mfrc522_3(SS_PIN_3, RST_PIN);
MFRC522 mfrc522_4(SS_PIN_4, RST_PIN); // 新增
MFRC522 mfrc522_5(SS_PIN_5, RST_PIN); // 新增

MFRC522::MIFARE_Key key;

// 创建NTP客户端
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800, 60000);

// 定义音调频率
#define NOTE_C4  262
#define NOTE_E4  330
#define NOTE_G4  392
#define NOTE_A4  440 // 新增
#define NOTE_B4  494 // 新增

Ticker watchdogTicker;

// 定义LED状态枚举
enum LedStatus {LED_OFF, LED_BLINK, LED_ON};
volatile LedStatus ledStatus = LED_OFF;

// LED控制函数
void controlLED() {
  static bool ledState = false;
  switch (ledStatus) {
    case LED_OFF:
      digitalWrite(LED_PIN_1, LOW);
      break;
    case LED_BLINK:
      ledState = !ledState;
      digitalWrite(LED_PIN_1, ledState ? HIGH : LOW);
      break;
    case LED_ON:
      digitalWrite(LED_PIN_1, HIGH);
      break;
  }
}

void setup() {
  Serial.begin(9600);
  delay(100);

  // 初始化EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadConfig();

  // 初始化WiFiManager
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");

  Serial.println("WiFi Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // 禁用串口通信以使用GPIO1和GPIO3作为片选引脚
  Serial.end();

  // 初始化SPI
  SPI.begin();

  // 初始化所有RC522模块
  mfrc522_1.PCD_Init();
  mfrc522_2.PCD_Init();
  mfrc522_3.PCD_Init();
  mfrc522_4.PCD_Init(); // 新增
  mfrc522_5.PCD_Init(); // 新增

  Serial.println("ESP8266 and 5 RC522 demo start");

  // 准备RC522默认密钥
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  // 初始化NTP客户端
  timeClient.begin();

  // 初始化MQTT服务器
  OneNET.setServer(mqtt_server, mqtt_port);
  OneNET.setCallback(callback);

  // 初始化LED和蜂鸣器引脚
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);

  connectMQTT();

  // 设置Web服务器路由
  server.on("/", handleRoot);
  server.on("/save", handleSave);

  server.begin();
  Serial.println("HTTP server started");

  // 设置LED控制Ticker
  ledTicker.attach(1, controlLED);

  // 设置看门狗Ticker
  watchdogTicker.attach(5, feedWatchdog);
}

void loop() {
  // 根据连接状态设置LED状态
  if (WiFi.status() != WL_CONNECTED) {
    ledStatus = LED_OFF; // 没有连接WiFi时，LED灭
  } else if (!OneNET.connected()) {
    ledStatus = LED_BLINK; // 连接WiFi但未连接MQTT时，LED闪烁
    reconnect();
  } else {
    ledStatus = LED_ON; // 连接MQTT时，LED常亮
  }

  // 处理MQTT消息
  if (OneNET.connected()) {
    OneNET.loop();
  }

  // 更新NTP时间
  timeClient.update();

  // 读取所有RC522模块的数据
  readAllRFIDModules();

  // 处理HTTP客户端请求
  server.handleClient();
}

// 读取所有RC522模块的数据
void readAllRFIDModules() {
  readRFID(mfrc522_1, 1);
  readRFID(mfrc522_2, 2);
  readRFID(mfrc522_3, 3);
  readRFID(mfrc522_4, 4); // 新增
  readRFID(mfrc522_5, 5); // 新增
}

// 读取单个RC522模块的数据
bool readRFID(MFRC522 &mfrc522, int moduleNum) {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // 获取卡片的UID
  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidStr += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("模块 ");
  Serial.print(moduleNum);
  Serial.print(" 读取到卡片 UID: ");
  Serial.println(uidStr);

  // 获取当前时间
  unsigned long epochTime = timeClient.getEpochTime();
  if (epochTime < 1514764800) { // 检查时间是否合理（2018年1月1日）
    epochTime = 0;
  }

  // 发布RFID数据
  if (publishRFIDData(moduleNum, uidStr, epochTime)) {
    // 如果数据发布成功，响蜂鸣器表示卡片号
    beepBuzzer(moduleNum);
  }

  // 停止 PICC
  mfrc522.PICC_HaltA();
  // 停止加密PCD
  mfrc522.PCD_StopCrypto1();

  return true;
}

// 发布RFID数据到MQTT
bool publishRFIDData(int moduleNum, const String& uid, unsigned long epochTime) {
  String jsonData = "{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"num\":{\"value\":" + String(moduleNum) + "},\"name\":{\"value\":\"" + uid + "\"},\"time\":{\"value\":" + String(epochTime) + "}}}";

  // MQTT主题
  String topic = "$sys/" + String(productID) + "/" + String(deviceID) + "/thing/property/post";

  Serial.println(jsonData); // 输出验证

  // 将String类型的JSON数据转换为字符数组
  char jsonBuffer[jsonData.length() + 1];
  jsonData.toCharArray(jsonBuffer, sizeof(jsonBuffer));

  // 发布数据到OneNET
  if (!OneNET.publish(topic.c_str(), jsonBuffer)) {
    Serial.println("数据发布失败");
    return false;
  } else {
    Serial.println("数据发布成功");
    return true;
  }
}

// 蜂鸣器发声函数，低电平触发
void beepBuzzer(int cardNum) {
  switch(cardNum) {
    case 1:
      playTone(NOTE_C4, 500); // C4音调，持续500毫秒
      break;
    case 2:
      playTone(NOTE_E4, 500); // E4音调，持续500毫秒
      break;
    case 3:
      playTone(NOTE_G4, 500); // G4音调，持续500毫秒
      break;
    case 4:
      playTone(NOTE_A4, 500); // A4音调，持续500毫秒
      break;
    case 5:
      playTone(NOTE_B4, 500); // B4音调，持续500毫秒
      break;
    default:
      break;
  }
}
// 播放音调函数
void playTone(int frequency, int duration) {
  tone(BUZZER_PIN, frequency); // 产生特定频率的声音
  delay(duration); // 持续指定时间
  noTone(BUZZER_PIN); // 停止发声
  delay(100); // 给一些时间间隔
}

// 连接MQTT服务器
void connectMQTT() {
  if (OneNET.connect(deviceID, productID, apiKey)) {
    Serial.println("MQTT Connected");
    commandTopic = "$sys/" + String(productID) + "/" + String(deviceID) + "/thing/property/set";
    if (OneNET.subscribe(commandTopic.c_str())) {
      Serial.println("Subscribed to command topic");
    } else {
      Serial.println("Subscription failed");
    }
  } else {
    Serial.println("MQTT Connection failed");
  }
}

// MQTT消息回调函数
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  for (size_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// 重连MQTT服务器
void reconnect() {
  Serial.print("Attempting MQTT connection...");
  if (OneNET.connect(deviceID, productID, apiKey)) {
    Serial.println("connected");
    OneNET.subscribe(commandTopic.c_str());
  } else {
    Serial.print("failed, rc=");
    Serial.print(OneNET.state());
    Serial.println(" try again in 5 seconds");
    delay(5000);
  }
}

// 处理保存配置请求
void handleSave() {
  if (server.method() == HTTP_POST) {
    server.arg("server").toCharArray(mqtt_server, sizeof(mqtt_server));
    mqtt_port = server.arg("port").toInt();
    server.arg("deviceID").toCharArray(deviceID, sizeof(deviceID));
    server.arg("productID").toCharArray(productID, sizeof(productID));
    server.arg("apiKey").toCharArray(apiKey, sizeof(apiKey));

    saveConfig();

    String message = "Saved! Restarting...";
    server.send(200, "text/plain", message);
    delay(2000);

    ESP.restart();
  }
}

// 加载配置函数
void loadConfig() {
  char buffer[512];
  for (size_t i = 0; i < sizeof(buffer); ++i) {
    buffer[i] = EEPROM.read(i);
  }

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, buffer);
  if (error) {
    Serial.println(F("读取配置文件失败，使用默认配置"));
    ledStatus = LED_BLINK; // 配置文件读取失败，LED闪烁
  } else {
    strlcpy(mqtt_server, doc["mqtt_server"] | "mqtts.heclouds.com", sizeof(mqtt_server));
    mqtt_port = doc["mqtt_port"] | 1883;
    strlcpy(deviceID, doc["deviceID"] | "m1", sizeof(deviceID));
    strlcpy(productID, doc["productID"] | "xhsOrw7Les", sizeof(productID));
    strlcpy(apiKey, doc["apiKey"] | "version=2018-10-31&res=products%2FxhsOrw7Les%2Fdevices%2Fm1&et=2031317695&method=md5&sign=jiLg0a2aoK%2F%2BDo6JpbMjbQ%3D%3D", sizeof(apiKey));
  }
  Serial.println("配置加载完成:");
  Serial.println(mqtt_server);
  Serial.println(mqtt_port);
  Serial.println(deviceID);
  Serial.println(productID);
  Serial.println(apiKey);
}

// 处理HTTP根路径请求
void handleRoot() {
  String html = "<html>\
  <head>\
    <title>MQTT Config</title>\
  </head>\
  <body>\
    <h1>MQTT Configuration</h1>\
    <form action='/save' method='post'>\
      MQTT Server: <input type='text' name='server' value='" + String(mqtt_server) + "'><br>\
      MQTT Port: <input type='text' name='port' value='" + String(mqtt_port) + "'><br>\
      Device ID: <input type='text' name='deviceID' value='" + String(deviceID) + "'><br>\
      Product ID: <input type='text' name='productID' value='" + String(productID) + "'><br>\
      API Key: <input type='text' name='apiKey' value='" + String(apiKey) + "'><br>\
      <input type='submit' value='Save'>\
    </form>\
  </body>\
  </html>";
  server.send(200, "text/html", html);
}

// 保存配置函数
void saveConfig() {
  StaticJsonDocument<512> doc;
  doc["mqtt_server"] = mqtt_server;
  doc["mqtt_port"] = mqtt_port;
  doc["deviceID"] = deviceID;
  doc["productID"] = productID;
  doc["apiKey"] = apiKey;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);

  for (size_t i = 0; i < n; ++i) {
    EEPROM.write(i, buffer[i]);
  }
  EEPROM.commit();
  Serial.println("配置保存完成");
}

// 喂狗函数
void feedWatchdog() {
  // 喂狗的具体操作，如定期检查系统状态
  ESP.wdtFeed();
}

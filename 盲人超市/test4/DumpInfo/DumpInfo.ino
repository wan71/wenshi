#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         5           // RST 引脚

// RC522模块的SS引脚
#define SS_PIN_1        4
#define SS_PIN_2        0
#define SS_PIN_3        2

MFRC522 mfrc522_1(SS_PIN_1, RST_PIN);   // 创建第一个RFID实例
MFRC522 mfrc522_2(SS_PIN_2, RST_PIN);   // 创建第二个RFID实例
MFRC522 mfrc522_3(SS_PIN_3, RST_PIN);   // 创建第三个RFID实例

MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600); // 设置串口波特率为9600
  SPI.begin();        // SPI开始

  // 初始化所有RC522模块
  mfrc522_1.PCD_Init(); 
  mfrc522_2.PCD_Init(); 
  mfrc522_3.PCD_Init(); 

  Serial.println("ESP8266 and 3 RC522 demo start");

  // 准备密钥
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF; // 默认密钥
  }
}

void loop() {
  // 读取第一个RC522
  readRFID(mfrc522_1, "RC522 1");
  // 读取第二个RC522
  readRFID(mfrc522_2, "RC522 2");
  // 读取第三个RC522
  readRFID(mfrc522_3, "RC522 3");
}

void readRFID(MFRC522 &mfrc522, const char* module) {
  // 寻找新卡
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // 显示卡片的详细信息
  Serial.print(module);
  Serial.print(" 卡片 UID:");
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print("卡片类型: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // 检查兼容性
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI
      && piccType != MFRC522::PICC_TYPE_MIFARE_1K
      && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println("仅适合Mifare Classic卡的读写");
    return;
  }

  // 认证卡片
  byte sector = 1; // 需要访问的扇区
  byte blockAddr = 4; // 扇区内的块地址
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, 
      blockAddr, 
      &key, 
      &(mfrc522.uid)
  );
  if (status != MFRC522::STATUS_OK) {
    Serial.print("身份验证失败：");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("身份验证成功！");

  // 停止 PICC
  mfrc522.PICC_HaltA();
  // 停止加密PCD
  mfrc522.PCD_StopCrypto1();
}

/**
   将字节数组转储为串行的十六进制值
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}


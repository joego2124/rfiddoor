
#include <SPI.h>
#include <MFRC522.h>

#include <Wire.h>
#define SS_PIN 21
#define RST_PIN 22
#define MOTOR_FWD_PIN A0
#define MOTOR_BKWD_PIN A1
#define LIMIT_SWITCH_PIN A2

int motor_mode = 0;
unsigned int motor_timer = millis();
unsigned int idle_timer = millis();
unsigned int card_timer = millis();

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

String keys[] = {
  "8D B5 89 79",
  "04 0D 37 72 A9 61 80",
  "52 D6 4A 20",
  "1D E0 89 79",
  "DD 5D 4D 6D",
  "04 1F 14 CA EF 62 80",
  "04 1F 2D 92 EF 62 80",
  "04 13 27 72 8B 65 80"
};

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void IRAM_ATTR ISR() {
  if (motor_mode == 1) {
    motor_mode = -2;
    idle_timer = millis();
    Serial.println("triggered"); 
  }
}

boolean checkKey(String key) {
  for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
    Serial.println(keys[i]);
    if (keys[i] == key) return true;
  }
  return false;
}

void setup() 
{
  Serial.begin(115200);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  pinMode(MOTOR_FWD_PIN, OUTPUT);
  pinMode(MOTOR_BKWD_PIN, OUTPUT);
  pinMode(LIMIT_SWITCH_PIN, INPUT);

  attachInterrupt(LIMIT_SWITCH_PIN, ISR, FALLING);
  
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(MOTOR_FWD_PIN, ledChannel);
}
void loop() 
{

  switch(motor_mode) {
    case 1:
//      digitalWrite(MOTOR_FWD_PIN, HIGH);
ledcWrite(ledChannel, 255);
      digitalWrite(MOTOR_BKWD_PIN, LOW);
      break;
    case 0:
//      digitalWrite(MOTOR_FWD_PIN, LOW);
ledcWrite(ledChannel, 0);
      digitalWrite(MOTOR_BKWD_PIN, LOW);
      break;
    case -1:
//      digitalWrite(MOTOR_FWD_PIN, LOW);
ledcWrite(ledChannel, 0);
      digitalWrite(MOTOR_BKWD_PIN, HIGH);
      if (millis() - motor_timer >= 400) motor_mode = 0;
      break;
    case -2:
      ledcWrite(ledChannel, 175);
      digitalWrite(MOTOR_BKWD_PIN, LOW);
      if (millis() - idle_timer >= 2000) {
        motor_mode = -1;
        motor_timer = millis();
      }
      break;
    default:
      break;
  }
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  
  if (checkKey(content.substring(1)) && (millis() - card_timer > 2000))  //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    motor_mode = 1;
    card_timer = millis();
  } else   {
    Serial.println("Access denied");
    card_timer = millis();
  }
} 

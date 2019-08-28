#include <SPI.h>
#include <MFRC522.h>

#include <Wire.h>
#define SS_PIN 10
#define RST_PIN 9
#define MOTOR_FWD_PIN A0
#define MOTOR_BKWD_PIN A1

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  Serial.println("Approximate your card to the reader...");
  Serial.println();

}
void loop() 
{
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
  if (content.substring(1) == "8D B5 89 79" || content.substring(1) == "6D 17 89 79") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    
    analogWrite(MOTOR_FWD_PIN, 180);
    analogWrite(MOTOR_BKWD_PIN, 0);
    delay(2000);
    analogWrite(MOTOR_FWD_PIN, 0);
    delay(50);
    analogWrite(MOTOR_BKWD_PIN, 180);
    delay(125);
    analogWrite(MOTOR_BKWD_PIN, 0);
  }
 
 else   {
    Serial.println("Access denied");
    
    delay(2000);
    
  }
} 

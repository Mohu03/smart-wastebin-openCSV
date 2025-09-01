#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LCD_I2C.h>
#include<Servo.h>
Servo servo;
LCD_I2C lcd(0x27, 16, 2);

#define SS_PIN D8
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

char ssid[] = "SKYNET"; // your network SSID (name)
char pass[] = "$mil!n@007"; // your network password

float temp_C = 0;
float temp_F = 0;
float hmd = 0;
long C1 = 0;
long C2 = 0;
unsigned long last_time;
bool flag = true;
WiFiServer server(80);
String request = "";

void setup() 
{
  lcd.backlight();
  lcd.begin();
  pinMode(D0,INPUT);
  pinMode(D3,INPUT); 
  pinMode(D4,INPUT);
  Serial.begin(115200); // Initiate a serial communication
  Serial.println();
    Serial.println("Serial started at 115200");
    Serial.println();

    // Connect to a WiFi network
  servo.attach(1);
  servo.write(90);
  lcd.setCursor(0, 0);
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
    Serial.print(F("Connecting to ")); Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println(F("[CONNECTED]"));
    Serial.print("[IP ");              
    Serial.print(WiFi.localIP()); 
    Serial.println("]");

    // start a server
    server.begin();
    Serial.println("Server started");
    lcd.clear();
    lcd.setCursor(0, 0);
lcd.print("Log in to ");
lcd.setCursor(0, 1);
lcd.print(WiFi.localIP());
delay(2500);
  SPI.begin(); // Initiate SPI bus
  mfrc522.PCD_Init(); // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  lcd.setCursor(0, 0);
  lcd.print("Waste Segregator>>");
  lcd.setCursor(0, 1);
  lcd.print("Show your RFID..");
}

void Servo_left(){
  //int pos;
  lcd.setCursor(10,0);
  lcd.print("P");
  for (int pos = 90; pos >= 10; pos--) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15); // waits 15ms for the servo to reach the position
  }
  delay(1000);
  for (int pos = 10; pos <= 90; pos++) { // goes from 180 degrees to 0 degrees
    servo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15); // waits 15ms for the servo to reach the position
  }
  
  //servo.write(30);
  //delay(3000);
  //servo.write(90);
}

void Servo_right(){
  //int pos;
  lcd.setCursor(10,0);
  lcd.print("M");
    for (int pos = 90; pos <= 170; pos++) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15); // waits 15ms for the servo to reach the position
  }
  delay(1000);
  for (int pos = 170; pos >= 90; pos--) { // goes from 180 degrees to 0 degrees
    servo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15); // waits 15ms for the servo to reach the position
  }
  
  //servo.write(150);
  //delay(3000);
  //servo.write(90);
}

void RFID() 
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
  
   Serial.print("UID tag :");
   lcd.setCursor(0, 0);
   lcd.clear();
   lcd.print("ID : ");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  lcd.setCursor(5, 0);
  lcd.print(content.substring(1));
  delay(1000);
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("MSG : ");
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  
  if (content.substring(1) == "53 0E 8C 36") //change here the UID of the card/cards that you want to give access
  {
    lcd.setCursor(6, 0);
   lcd.print("Valid RFID");
    Serial.println("Valid RFID");
    lcd.setCursor(0, 1);
   lcd.print("student_1");
    Serial.println("student_1");
    Serial.println("D0 : "+String(digitalRead(D0)));
    Serial.println("D3 : "+String(digitalRead(D3)));
    Serial.println("D4 : "+String(digitalRead(D4)));
    delay(1000);
        if (digitalRead(D0)==HIGH && digitalRead(D3)==HIGH && digitalRead(D4)==LOW){
      C1++;
      Servo_left();
            lcd.clear();
   lcd.setCursor(2, 0);
   lcd.print("student 1");
   lcd.setCursor(0, 1);
   lcd.print("Credit : "+String(C1));
    }
        else if (digitalRead(D0)==HIGH && digitalRead(D3)==LOW && digitalRead(D4)==LOW){
      C1+=3;
      Servo_left();
            lcd.clear();
   lcd.setCursor(2, 0);
   lcd.print("student 1");
   lcd.setCursor(0, 1);
   lcd.print("Credit : "+String(C1));
    }
        else if (digitalRead(D0)==LOW && digitalRead(D3)==LOW && digitalRead(D4)==LOW){
      C1+=6;
      Servo_right();
            lcd.clear();
   lcd.setCursor(2, 0);
   lcd.print("student 1");
   lcd.setCursor(0, 1);
   lcd.print("Credit : "+String(C1));
    }
            else if (digitalRead(D0)==LOW && digitalRead(D3)==HIGH && digitalRead(D4)==LOW){
      C1+=8;
      Servo_right();
            lcd.clear();
   lcd.setCursor(2, 0);
   lcd.print("student 1");
   lcd.setCursor(0, 1);
   lcd.print("Credit : "+String(C1));
    }
    delay(100);
    last_time = millis();
    flag = false;
  }
  if (content.substring(1) == "C9 73 74 C8") //change here the UID of the card/cards that you want to give access
  {
   lcd.setCursor(6, 0);
   lcd.print("Valid RFID");
   Serial.println("Valid RFID");
   lcd.setCursor(0, 1);
   lcd.print("student_2");
    Serial.println("student_2");
    Serial.println("D0 : "+String(digitalRead(D0)));
    Serial.println("D3 : "+String(digitalRead(D3)));
    Serial.println("D4 : "+String(digitalRead(D4)));
    delay(1000);
    if (digitalRead(D0)==HIGH && digitalRead(D3)==HIGH && digitalRead(D4)==LOW){//proxy, ir, ir
      C2++;
      Servo_left();
      lcd.clear();
   lcd.setCursor(2, 0);
   lcd.print("student 2");
   lcd.setCursor(0, 1);
   lcd.print("Credit : "+String(C2));
    }
        else if (digitalRead(D0)==HIGH && digitalRead(D3)==LOW && digitalRead(D4)==LOW){
      C2+=3;
      Servo_left();
            lcd.clear();
   lcd.setCursor(2, 0);
   lcd.print("student 2");
   lcd.setCursor(0, 1);
   lcd.print("Credit : "+String(C2));
    }
        else if (digitalRead(D0)==LOW && digitalRead(D3)==LOW && digitalRead(D4)==LOW){
      C2+=6;
      Servo_right();
            lcd.clear();
   lcd.setCursor(2, 0);
   lcd.print("student 2");
   lcd.setCursor(0, 1);
   lcd.print("Credit : "+String(C2));
    }
                else if (digitalRead(D0)==LOW && digitalRead(D3)==HIGH && digitalRead(D4)==LOW){
      C1+=8;
      Servo_right();
            lcd.clear();
   lcd.setCursor(2, 0);
   lcd.print("student 2");
   lcd.setCursor(0, 1);
   lcd.print("Credit : "+String(C2));
    }
    delay(100);
    last_time = millis();
    flag = false;
  }
  /*
  if (content.substring(1) == "85 45 60 20") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Valid RFID");
    Serial.println("Dark Fantasy");
    money=money+90;
    Serial.println(money);
    Serial.write(money);
    
    servo_close();
    delay(30);
  }
  */
 else 
 {
    Serial.println(" Invalid RFID");
    delay(30);
  }
}

void Welcome_Screen(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waste Segregator>>");
  lcd.setCursor(0, 1);
  lcd.print("Show your RFID..");
}
void loop() 
{
  RFID();
  if (millis() - last_time > 10000 && flag == false){
    Welcome_Screen();
    flag = true;
  }
    WiFiClient client = server.available(); // Check if a client has connected
    if (!client) { return; }

    request = client.readStringUntil('\r'); // Read the first line of the request

    Serial.println(request);
    Serial.println("");

    if ( request.indexOf("getTemp") > 0 )
     { 
          Serial.println("getTemp received");

          // Reading temperature or humidity takes about 250 milliseconds!
          // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
          temp_C = C1; // Read temperature as Celsius (the default)
          hmd = C2; // Read temperature as Fahrenheit (isFahrenheit = true)
          temp_F = C2;

          if ( true )
          {
              client.print( header );
              client.print( temp_C ); client.print( "|" ); client.print( temp_F ); client.print( "|" ); client.print( hmd );
              Serial.println("data sent");
          }
          else
          {
              Serial.println("Error reading the sensor");
          }
     }

     else
     {
        client.flush();
        client.print( header );
        client.print( html_1 ); 
        Serial.println("New page served");
     }

    delay(5);



}

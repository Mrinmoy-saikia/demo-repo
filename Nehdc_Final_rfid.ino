#include<SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//********

#define tx D1  //D1
#define rx D3  //D2
#define LED3 D4
#define LED2 D8
#define LED1 D5 
#define BUZZ D6
SoftwareSerial EM18(rx, tx);
//********
/* Set these to your desired credentials. */
const char *ssid = "JioFiber-kMtt4";
const char *password = "eeRavue5heequ5th";


const char* device_token  = "f304da829518e1bf";
String URL = "http://hrmsapi.nehhdcmis.in/public/addmyattendancefromrfid";//"http://hrmsapi.nehhdcmis.in/public/addmyattendancefromrfid"; //computer IP or the server domain
String getData, Link;
String OldCardID = "";
unsigned long previousMillis = 0;
int count = 0;
bool stringComplete = false;
String CardID = "";

void setup()
{ Serial.begin(9600);
  EM18.begin(9600);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED3, LOW);
  if (Serial.available())
  { count = 0;
    { char input = Serial.read();
      Serial.print(input);
      count++; //
      delay(5); //
    }
    Serial.println();
    Serial.print("Tag Length : ");
    Serial.print(count);
    Serial.println(" Bytes");
  }
  connectToWiFi();
}

//********
void loop() {
  if (!WiFi.isConnected()) {
    Serial.println("wifi disconnected");
    connectToWiFi();    //Retry to connect to Wi-Fi
  }
  //---------------------------------------------
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID = "";
  }
  delay(50);  //
  if ((EM18.available() > 0) ) {
    CardID = EM18.readString();
    Serial.print("new card no. "); Serial.println(CardID);
    CardID.remove(12);
    Serial.print("new card no. corrected  "); Serial.println(CardID);
    stringComplete = true;
    if (!CardID) {
      return;
    }
  }
  //---------------------------------------------
  if ( CardID == OldCardID ) {
    return;
  }
  else {
    OldCardID = CardID; Serial.print("old card id"); Serial.println(OldCardID);
  }
  //---------------------------------------------
  if (stringComplete) {
    SendCardID(CardID);
    stringComplete = false; delay(1000);
  }
  CardID = "";  delay(1000);
}


//**send the Card UID to the website*

void SendCardID( String Card_uid ) {
  Serial.println("Sending the Card ID");
  if (WiFi.isConnected()) {
    WiFiClient net;
    HTTPClient http;    //Declare object of class HTTPClient
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token); // Add the Card ID to the GET array in order to send it
    //GET methode
    Link = URL + getData;
    http.begin(net, Link); //initiate HTTP request   //Specify content-type header
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();    //Get the response payload
    Serial.println(Link);   //Print HTTP return code
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(Card_uid);     //Print Card ID
    Serial.println(payload);    //Print request response payload
    if (payload == "true") {
      // play the sound or make the light go green
      digitalWrite(LED1,HIGH);//change try

      Serial.println("LED1 ON");
      tone(BUZZ, 2000, 700); //noTone(BUZZ);
      delay(1000);
      digitalWrite(LED1, LOW);//change try low
      Serial.println("Green Light");
    } else {
      Serial.println("Red Light");
      digitalWrite(LED2, HIGH);//change try high
      for (int a = 0; a < 2; a++) {
        delay(100); tone(BUZZ, 200, 1500);
      }
      Serial.println("LED2 ON");
      delay(1500);
      digitalWrite(LED2, LOW);//change try low
    }
    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
        Serial.println(user_name);
      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
      }
      else if (payload == "succesful") {
      }
      else if (payload == "available") {
      }
      delay(100);
      http.end();  //Close connection
    }
  }
}
//**connect to the WiFi**
void connectToWiFi() {
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED3, 1); Serial.print(".");
    delay(500);
    Serial.print("."); digitalWrite(LED3,0); delay(500);
  }
  Serial.println("");
  Serial.println("Connected");
  digitalWrite(LED3, 0);//change
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  delay(1000);
}

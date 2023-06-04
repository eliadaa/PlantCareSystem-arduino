#include<ESP8266WiFi.h>
#include<WiFiClient.h>
#include<ESP8266HTTPClient.h>
String URL="http://api.thingspeak.com/update?api_key=VVSDJEX2MALIAU1D&field1=";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  WiFi.disconnect();
  delay(2000);
  Serial.print("Start connection");
  WiFi.begin("DIGI-Dp4d","PXGdE4Suqc");
  while((!(WiFi.status()== WL_CONNECTED))){
      delay(200);
      Serial.print("..");
    }
  Serial.println("Connected");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()>0){
    String data = Serial.readStringUntil('\n');
    Serial.println(data);
    int comma = data.indexOf(',');


    // write a code_number for each sensor info, so you can diff them 
    if(comma != -1){
      float t = data.substring(0, comma).toFloat();
      float h = data.substring(comma+1).toFloat();
      Serial.print(t);
      Serial.println(h);
      Serial.println();
      sendData(t,h);
    }
  }
}

void sendData(float t, float h){
  WiFiClient client;
  HTTPClient http;

  String newUrl=URL+String(t)+"&field2="+String(h);
  http.begin(client,newUrl);
  int responsecode=http.GET();
  String data=http.getString();
  Serial.println(data);
  http.end();
}




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
    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);


    // write a code_number for each sensor info, so you can diff them 
    if(firstComma != -1 && secondComma != -1){
      float t = data.substring(0, firstComma).toFloat();
      float h = data.substring(firstComma+1, secondComma).toFloat();
      float l = data.substring(secondComma+1).toFloat();
      Serial.print(t);
      Serial.print(h);
      Serial.println(l);
      Serial.println();
      sendData(t,h,l);
    }
  }
}

void sendData(float t, float h, float l){
  WiFiClient client;
  HTTPClient http;

  String newUrl=URL+String(t)+"&field2="+String(h)+"&field3="+String(l);
  http.begin(client,newUrl);
  int responsecode=http.GET();
  String data=http.getString();
  Serial.println(data);
  http.end();
}




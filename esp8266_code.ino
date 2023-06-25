#include<ESP8266WiFi.h>
#include<WiFiClient.h>
#include<ESP8266HTTPClient.h>
#include <ThingSpeak.h>


// sensor codes and fields for reading data from ThingSpeak
#define TEMPERATURE_CODE       501
#define HUMIDITY_CODE          502
#define LUMINOSITY_CODE        503
#define MOISTURE_CODE          504
#define WATERLEVEL_CODE        505

#define temp_field             1
#define humid_field            2
#define lumi_field             3
#define moisture_field         4
#define water_level_field      5

// ThingSpeak settings for reading the sensor data 
const unsigned long channelID = 2175282;  // ThingSpeak Channel ID
const char* writeAPIKey = "VVSDJEX2MALIAU1D";  // Write API Key


// network alternatives 
const char* primary_ssid = "...";
const char* primary_password = "lostONlife";

const char* alternative1_ssid = "Tenda Wi-Fi";
const char* alternative1_password = "terasa2022";

const char* alternative2_ssid = "pandas";
const char* alternative2_password = "letsTravel7ar";

// channel ID, read API key for the ThingSpeak channel that handles
// the min moisture value received from the Android application
unsigned long ChannelNumber = 2149998;            // Channel ID
const char * myReadAPIKey = "Q0HFH6975PDN66C3"; // Read API Key
const int FieldNumber1 = 1;  // The field you wish to read

unsigned long connectionTimeout = 15000; // Timeout duration in milliseconds
unsigned long connectionStartTime;



// WiFiClient is a class provided by the ESP8266 WiFi library
WiFiClient client;  // Declare the client object


void setup() {
  Serial.begin(9600);  // begin serial communication on a 9600 baud rate, as the Arduino board 

  WiFi.disconnect(true); // disconnect from any network
  delay(1000);

  //Serial.println("start");
  // connecting to the internet 
  tryConnecting();       // connect to one of the networks provided 
  //Serial.println("end");

  // establish connection with the ThingSpeak server to send and receive data 
  ThingSpeak.begin(client);
}


// function that is used for trying to connect to one of the 3 networks provided
void tryConnecting() {
  // is the board connected to the network?
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Already connected to the primary network!");
  } else { // if not, try connecting to the first network provided 
    Serial.print("Connecting to the primary network...");
    connectToNetwork(primary_ssid, primary_password);

    // did it connect?
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to the primary network!");
    } else { // f not, try the first alternative
      Serial.println("Failed to connect to the primary network.");

      Serial.print("Connecting to the first alternative network...");
      connectToNetwork(alternative1_ssid, alternative1_password);

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to the first alternative network!");
      } else {
        Serial.println("Failed to connect to the first alternative network.");

        Serial.print("Connecting to the second alternative network...");
        connectToNetwork(alternative2_ssid, alternative2_password);

        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("Connected to the second alternative network!");
        } else {
          Serial.println("Failed to connect to the second alternative network.");
          Serial.println("Unable to establish a connection.");
        }
      }
    }
  }
}


void connectToNetwork(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  connectionStartTime = millis();

  // while the board is still trying to connect
  while (WiFi.status() != WL_CONNECTED) {
    // check if enough time passed since searching for this specific connection
    if (millis() - connectionStartTime >= connectionTimeout) {
      break; // Break the loop if timeout exceeded
    }
    delay(1000);
  }
}


void loop() {
    if (Serial.available()) {
    String data = Serial.readStringUntil('\n');

    // Extract sensor code and data
    int sensorCode = data.substring(0, data.indexOf(',')).toInt();
    float value = data.substring(data.indexOf(',') + 1).toFloat();


      // Save data in specific fields based on sensor code
      if (sensorCode == MOISTURE_CODE) {
        sendDataToThingSpeak(moisture_field, value);
      } else if (sensorCode == WATERLEVEL_CODE) {
        sendDataToThingSpeak(water_level_field, value);
      } else if (sensorCode == LUMINOSITY_CODE) {
        sendDataToThingSpeak(lumi_field, value);
      } else if (sensorCode == HUMIDITY_CODE) {
        sendDataToThingSpeak(humid_field, value);
      } else if (sensorCode == TEMPERATURE_CODE) {
        sendDataToThingSpeak(temp_field, value);
      }
     ThingSpeak.writeFields(channelID, writeAPIKey);
    }

  // read data from ThingSpeak with the channel 'ChannelNumber', with the API key 'myReadAPIKey', from the field 'FieldNumber1'
  long temp = ThingSpeak.readLongField(ChannelNumber, FieldNumber1, myReadAPIKey);
  int statusCode = ThingSpeak.getLastReadStatus();
  // was the read successful?
  if (statusCode == 200){
    // send value to the Arduino board 
    Serial.println(temp);
  }
  else{
    Serial.println("Unable to read channel / No internet connection");
  }
  delay(25000);

  
}

void sendDataToThingSpeak(int fieldNumber, float sensorValue){
  ThingSpeak.setField(fieldNumber, sensorValue);    
  ThingSpeak.writeFields(channelID, writeAPIKey);
  // Write the data to ThingSpeak
  // ThingSpeak.writeFields(channelID, writeAPIKey);
  Serial.println(fieldNumber);
  Serial.println(sensorValue);
}


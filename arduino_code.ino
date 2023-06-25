#include <DHT.h>

#define LIGHTSENSOR_PIN    A2   // SEN0043 TEMT600 ANALOG AMBIENT LIGHT SENSOR ( has a range of 0-1023 )
#define dhtPin 2
#define dhtType DHT11
#define RELAY_PIN 8             // 5V relay connected to submersible water pump 
#define MOISTURESENSOR_PIN A0   // SEN0193 ANALOG CAPACITIVE SOIL MOISTURE SENSOR
#define WATERLEVEL_PIN     A4   // 
#define WATERLEVEL_POWER_PIN 7  // the water level sensor shouldn't be given power constantly  

#define tempCODE         501
#define humidCODE        502
#define luminCODE        503
#define moistureCODE     504
#define waterlevelCODE   505


DHT dht(dhtPin, dhtType);

float temp, humid, lumin, moist, water;
float moisture_level, moisture_percentage, water_level, water_precentage, lumin_level, lumin_percentage;

const int open_air_reading = 520; //700;    //calibration data 1, readings when sensor is exposed to air
const int water_reading = 260;  //280;      //calibration data 2, readings when sensor is exposed to water

const int no_water         =   0;     // calibration data 3, readings when the water level sensor is dry
const int full_water       = 540;     // calibration data 4, readings when the water level sensor is fully immersed into water

const int no_light         = 0;
const int brightest        = 1023;

int min_moisture = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LIGHTSENSOR_PIN, INPUT);
  pinMode(MOISTURESENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(WATERLEVEL_POWER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // stop
  digitalWrite(WATERLEVEL_POWER_PIN, LOW); // stop the power to the water level sensor
  dht.begin();
}

void loop() {

  temp = dht.readTemperature();
  humid = dht.readHumidity();

  // send temp & humid
  if(!isnan(temp) && !isnan(humid)){  // is the sensor connected?
    // Serial.print("temp: ");
    Serial.println(tempCODE);
    delay(5000);
    Serial.println(temp);
    delay(10000);

    // Serial.print("humid: ");
    Serial.println(humidCODE);
    delay(5000);
    Serial.println(humid);
    delay(10000);
  } 

  // luminosity
  lumin_level = analogRead(LIGHTSENSOR_PIN);
  lumin_percentage = map(lumin_level, 0, 1023, 0, 100); // 0-1023 analog readings (10 bit resolution) converted into percentages

  if(lumin_percentage >= 0 && lumin_percentage <= 100){
    Serial.println(luminCODE);
    delay(5000);
    Serial.println(lumin_percentage);
    delay(10000);
  }


  // soil moisture
  moisture_level = analogRead(MOISTURESENSOR_PIN);    // read soil moisture sensor  
  moisture_percentage = map(moisture_level, open_air_reading, water_reading, 0, 100);

  if(moisture_percentage >= 0 && moisture_percentage <= 100){
    // Serial.print("moisture:");
    Serial.println(moistureCODE);
    delay(5000);
    Serial.println(moisture_percentage);
    delay(10000);
  }


  // water level 
  digitalWrite(WATERLEVEL_POWER_PIN, HIGH);  // turn the sensor ON
  delay(20);                                 // wait 20 ms
  water_level = analogRead(WATERLEVEL_PIN);  // read the sensor
  digitalWrite(WATERLEVEL_POWER_PIN, LOW);   // turn the sensor OFF
  water_precentage =  map(water_level, no_water, full_water, 0, 100); // convert in percentage

  if(water_precentage >= 0 && water_precentage <= 100){
    // Serial.print("water_level:");
    Serial.println(waterlevelCODE);
    delay(5000);
    Serial.println(water_precentage);
    delay(10000);
  }



  // get min moisture value from the wifi board 
  if (Serial.available()>0) {
    String data = Serial.readStringUntil('\n');
    min_moisture = data.toInt();

    // compare the value with the actual soil moisture precentage
    // if the moisture is smaller that the allowed range, trigger the pump 
    if(moisture_percentage < min_moisture){
      digitalWrite(RELAY_PIN, LOW);
      delay(6000); // so the pump will be on, pause the program for 6000 milliseconds
      digitalWrite(RELAY_PIN, HIGH); // stop
      delay(15000);
    }
  }
  
  delay(30000);

}

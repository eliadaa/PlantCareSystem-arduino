#include <DHT.h>

#define LIGHTSENSOR_PIN    A2   // SEN0043 TEMT600 ANALOG AMBIENT LIGHT SENSOR ( has a range of 0-1023 )
#define dhtPin 2
#define dhtType DHT11
#define RELAY_PIN 8             // 5V relay connected to submersible water pump 
#define MOISTURESENSOR_PIN A0   // SEN0193 ANALOG CAPACITIVE SOIL MOISTURE SENSOR
#define WATERLEVEL_PIN     A4   // 
#define WATERLEVEL_POWER_PIN 7  // the water level sensor shouldn't be given power constantly  

DHT dht(dhtPin, dhtType);
float t, h, l, m, w; // temperature, humidity, light-luminosity, moisture, water_level (for the pump)

float moisture_level, li, soil_moisture_precentage, water_level, water_precentage;

const int no_water = 0;                     // water sensor is dry
const int full_water = 540;                 // fully immersed in water

const int open_air_reading = 520; //700;    //calibration data 1, readings when sensor is exposed to air
const int water_reading = 260;  //280;      //calibration data 2, readings when sensor is exposed to water


void setup() {
  Serial.begin(9600);
  pinMode(LIGHTSENSOR_PIN, INPUT);
  pinMode(MOISTURESENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(WATERLEVEL_POWER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // stop
  digitalWrite(WATERLEVEL_POWER_PIN, LOW);
  dht.begin();
}

void loop() {

  t = dht.readTemperature();
  h = dht.readHumidity();

  l = analogRead(LIGHTSENSOR_PIN);
  li = (float)l / 1023 * 100.0; // getting the percentage // or use map();  
  
  moisture_level = analogRead(MOISTURESENSOR_PIN);    // read soil moisture sensor  
  // map(value, fromLow, fromHigh, toLow, toHigh)
  soil_moisture_precentage = map(moisture_level, open_air_reading, water_reading, 0, 100);

  m = soil_moisture_precentage;

  digitalWrite(WATERLEVEL_POWER_PIN, HIGH); // turn the sensor ON
  delay(20);                                // wait 20 ms
  water_level = analogRead(WATERLEVEL_PIN);
  digitalWrite(WATERLEVEL_POWER_PIN, LOW);  // turn the sensor OFF
  water_precentage = map(water_level, no_water, full_water, 0, 100);  // convert in percentage
  w = water_precentage;
 // Serial.println(moisture_level);

  if(isnan(t) || isnan(h) || isnan(l) || isnan(moisture_level)) 
    Serial.println("no data available from sensor");

  Serial.print(t);  // send the data from the temperature sensor through serial communication, since esp8266 is connected to Arduino, it will receive the data
  Serial.print(",");
  Serial.print(h);  
  Serial.print(",");
  Serial.print(li);  
  Serial.print(",");
  Serial.print(m);  
  Serial.print(",");
  Serial.print(w);
  Serial.println();


  // deal with dry soil

  // !!!!! as far as I tested, the relay works on the opposite logic
  // starts when setting the pin to LOW 
  // and stops when setting the pin to HIGH
  // I am losing it..............

  if(moisture_level > 500 || moisture_level < water_reading){ // use open_air_reading and water_reading
    // air or water, skip
    digitalWrite(RELAY_PIN, HIGH);
  }
  else if(soil_moisture_precentage < 40){  // if the soil is too dry, start the pump by giving power to the relay
    digitalWrite(RELAY_PIN, LOW);
    delay(3500); // so the pump will be on, pause the program for 3500 milliseconds
    digitalWrite(RELAY_PIN, HIGH); // stop
  }
  else
    digitalWrite(RELAY_PIN, HIGH); // in any other case, stop



  delay(10000);  // stop 2 seconds, maybe increase this 




}

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            2

//#define DHTTYPE           DHT11     // DHT 11
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)

// datatype for our sensor
typedef struct weatherData {
  float temperature;
  float humidity;
} weatherData_t;


// dht object
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

weatherData_t sensorResponse;


bool getTemperature(DHT_Unified &dht, sensors_event_t &event) {
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    return false;
  }
  return true;
}

bool getHumidity(DHT_Unified &dht, sensors_event_t &event) {
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    return false;
  }
  return true;
}

// function to get the weather
// on success, updates the global sensorResponse obj
// on fail, does nothing and returns false
bool getWeather() {

  bool status = false;
  sensors_event_t event;

  // try to read the temperature
  if (getTemperature(dht, event)) {
    status = true;
    sensorResponse.temperature = event.temperature;
  }
  else {
    Serial.println("Could not get temperature.");
    return false;
  }
  
  // try to read humidity
  if (getHumidity(dht, event)) {
    status = true;
    sensorResponse.humidity = event.relative_humidity;
  }
  else {
    Serial.println("Could not get humidity.");
    return false;
  }

  return status;
}

// main responder to serial commands
// response encoded in JSON
void responder() {
  // read the weather sensor
  if (getWeather()) {
    String temperature = String(sensorResponse.temperature);
    String humidity = String(sensorResponse.humidity);

    // encode output to JSON
    String response = "{\"temperature\":" + temperature + ", \"humidity\":" + humidity + "}";
    Serial.println(response);
  }
  else {
    // send false
    Serial.println("false");
  }
}

void setup() {
  // start serial
  Serial.begin(9600);

  // initialize DHT sensor
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void loop() {
  // check for input on serial port
  if (Serial.available() > 0) {
    // read the input
    int inByte = Serial.read();
    delay(500); // small delay before responding

    // respond only if correct command is received
    if ((char)inByte == 'r') {
      responder();
      delay(delayMS);
    }
  }
}

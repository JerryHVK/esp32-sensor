// include libraries
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <PubSubClient.h>


// declare global variable
String URL = "http://192.168.100.121/max30102_sensor/data.php";
const char* ssid = "K401"; 
const char* password = "yasuott7"; 

// MQTT Broker
const char *mqtt_broker = "10.14.8.4";
// const char *topic = "emqx/esp32";
// const char *mqtt_username = "emqx";
// const char *mqtt_password = "public";
const int mqtt_port = 1883;

MAX30105 particleSensor;
const byte RATE_SIZE = 10; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
bool finger = true;

// setup() function
void setup() {
  Serial.begin(115200);

  if (particleSensor.begin() == false)
  {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  } 

  particleSensor.setup();
  
  connectWiFi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// loop() function
void loop() {
  //check wifi status before get and send data
  if(WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  } 

  long start = millis();
  long end;
  while(1){
    loop_for_data();
    end = millis();
    if(end - start > 3000 && finger){ // send data after 3 measuring seconds
      // send_data();
      break;
    }
  }
}

// loop for getting data from sensor
void loop_for_data(){
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 120 && beatsPerMinute > 40)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000){
    Serial.print(" No finger?");
    finger = false;
  }
  else{
    finger = true;
  }
  Serial.println();
  // delay(500);
}

//send data by http method
void send_data(){
  String postData = "heart_rate=" + String(beatAvg);
  
  HTTPClient http;
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpCode = http.POST(postData);
  String payload = "";

  if(httpCode > 0) {
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();  //Close connection

  Serial.print("URL : "); Serial.println(URL); 
  Serial.print("Data: "); Serial.println(postData);
  Serial.print("httpCode: "); Serial.println(httpCode);
  Serial.print("payload : "); Serial.println(payload);
  Serial.println("--------------------------------------------------");
}

// for connect wifi
void connectWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(1000);
  //This line hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  Serial.print("connected to : "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}
#include <Wire.h>
#include <WiFi.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <PubSubClient.h>
#define LED_PIN 4

//  define the wifi and MQTT ip address
const char* ssid = "Linh Nam 2407"; 
const char* password = "hanhlinh"; 
// const char* ssid = "K401"; 
// const char* password = "yasuott7"; 
const char *mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

//  client for wifi connection
WiFiClient espClient;
//  client for mqtt connection
PubSubClient client(espClient);

unsigned long lastMsg = 0;
int led_state = LOW;

MAX30105 particleSensor;
const byte RATE_SIZE = 10; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

//  finger = true if there is a finger on the sensor, otherwise = false
bool finger = true;

char myString[10];

// setup
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  //  setup the sensor MAX30102
  if (particleSensor.begin() == false)
  {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  } 
  particleSensor.setup();
  Serial.println("MAX30102 setup successfully!");
  
  //  setup wifi
  setup_wifi();
 
  //  mqtt client connect to mqtt broker
  client.setServer(mqtt_server, 1883);

  //  subscribe to a topic and set the callback function
  client.setCallback(callback); // callback only for subscribing
  client.subscribe("khang/esp32");
}

// loop
void loop() {
  // put your main code here, to run repeatedly:

  //  check wifi connection at each loop
  if(WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }

  //  check the mqtt connection at each loop
  if (!client.connected()) {
    reconnect();
    client.subscribe("khang/esp32");
  }
  client.loop();

  // send data 
  long start = millis();
  long end;
  while(1){
    loop_for_data();
    end = millis();
    if(end - start > 500){ // send data after 500ms
      if(finger){
        send_data();
        break;
      }
      break;
    }
  }
}

//  this "setup_wifi()" function is the code example from "Wifi.h" library
void setup_wifi() {
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
  Serial.println("\nWiFi connected successfully");
  Serial.print("connected to : "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}


//  this "reconnect()" functions is from the example of "PubSubClient" library
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if(led_state == LOW){
    digitalWrite(LED_PIN, HIGH);
    led_state = HIGH;
  }
  else{
    digitalWrite(LED_PIN, LOW);
    led_state = LOW;
  }
  // Serial.println("Change!");
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

void send_data(){
  sprintf(myString, "%d", beatAvg);
  client.publish("khang/server", myString);
}

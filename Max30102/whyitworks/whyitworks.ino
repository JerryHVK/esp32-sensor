#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define LED_PIN 4

//  define the wifi and MQTT ip address
// const char* ssid = "Khang"; 
// const char* password = "11111111"; 
const char* ssid = "K401"; 
const char* password = "yasuott7"; 
const char *mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

//  client for wifi connection
WiFiClient espClient;
//  client for mqtt connection
PubSubClient client(espClient);

unsigned long lastMsg = 0;
int led_state = LOW;

char myString[10];

// setup
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

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
    // client.setCallback(callback); // callback only for subscribing
    client.subscribe("khang/esp32");
  }
  client.loop();

  send_data();
  delay(1000);
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
  // digitalWrite(LED_PIN, led_state);
  Serial.println("Change!");
}

void send_data(){
  // client.publish("khang/esp32", "helloword");
}


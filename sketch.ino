
#include <WiFi.h>
#include <PubSubClient.h>
#include "TimeLib.h"
#include <NtpClientLib.h>


#define BTN_PIN 2

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const int port = 1883 ;
WiFiClient espClient;
PubSubClient client(espClient);
const char* pub_topic = "ufrniot/butao";
const char* sub_topic = "ufrniot/requisiçao";

char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}
void setupWifi() {

  WiFi.begin(ssid, password, 6);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("Connected!");
}

void callback(char *topic, byte *payload, unsigned int length) {

  Serial.print("Receive Topic: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  Serial.println((char *)payload);
  republish_saved();
}

void republish_saved(){
  for(int i=0;i<num_salvos;i++){
    if(lastnotifications[i] != ""){
      publish(string2char(lastnotifications[i]));
    }
  }
}

void setupBroker(){
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}
void reconnect(){
 while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = "esp32-dht22-clientId-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");
      client.subscribe(sub_topic);
    } else {
      Serial.println("Failed");
      delay(5000);
    }
  }
}
void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  setupWifi();
  setupBroker();
  NTP.begin();
}

const int num_salvos = 10;
String lastnotifications[num_salvos]={};

char* getTime(){
  String str_msg = NTP.getTimeDateString ();
  for(int i=0;i<(num_salvos-1);i++){
   lastnotifications[i]= lastnotifications[i+1];
  }
  lastnotifications[num_salvos-1]=str_msg;
  return string2char(str_msg);
}

void publish(char* msg){
  client.publish(pub_topic,msg);
  Serial.print("Pressed! ");
  Serial.println(msg);
}

void loop() {
  reconnect();
  client.loop();
  if (digitalRead(BTN_PIN) == LOW) {
    publish(getTime());
    delay(2000);
  }else{
    delay(20); 
  }
}

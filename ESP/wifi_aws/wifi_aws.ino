#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
//#include <cstring.h>

#define WIFI_SSID "jamies_iphone"
#define PASSWORD "jamieturner2"
#define AWS_ENDPOINT "my_AWS_endpoint" //??
#define CLIENT_ID "client's_ID"
#define PEM_CERTIFICATE "certificate1234" //need
#define PRIVATE_KEY "private_key" //need

bool junction = false;

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

void connectToWiFi(){
  WiFi.begin(WIFI_SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    Serial.println(".");
    delay(1000);
  }
  Serial.println("Connected to internet!");
}

void connectToAws(){
  mqttClient.setServer(AWS_ENDPOINT, 8883);
  mqttClient.setCallback(callback);

  while(!mqttClient.connected()){
    if(mqttClient.connect(CLIENT_ID, PEM_CERTIFICATE, PRIVATE_KEY)){
      Serial.println("Connected to AWS");
      mqttClient.subscribe("Purpose of message"); //need to change ofc
    } else{
      Serial.println("Failed to connect to AWS, will try again shortly...");
      delay(5000);
    }
  }
}

void callback(String topic, byte* payload, unsigned int length){
  //handle incoming mqtt messages, may not be relevant 
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectToWiFi();
  wifiClient.setCACert(PEM_CERTIFICATE);
  connectToAws();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!mqttClient.connected()){ 
    connectToAws();
  }

  mqttClient.loop();

  String payload = ""; // payload, in this case will be either coordinates, junction, or both
  //bool junction = false;
  
  mqttClient.publish("Coordinates/junction", payload.c_str());

  delay(1000);
}

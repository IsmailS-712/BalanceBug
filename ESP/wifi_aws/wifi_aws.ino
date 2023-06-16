#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
//#include <cstring.h>

#define WIFI_SSID     "jamies_iphone"
#define PASSWORD      "jamieturner2"
#define URL           "https://balance-bug.5959pn4l16bde.eu-west-2.cs.amazonlightsail.com/"
#define AWS_ENDPOINT  "/api/update" // exposed port
// #define CLIENT_ID "client's_ID"
// #define PEM_CERTIFICATE "certificate1234" //need
// #define PRIVATE_KEY "private_key" //need

bool junction = false;
WiFiClientSecure wifiClient;
//PubSubClient mqttClient(wifiClient);

void connectToWiFi(){
  WiFi.begin(WIFI_SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    Serial.println(".");
    delay(1000);
  }
  Serial.println("Connected to internet!");
}

void connectToAws(){
  // mqttClient.setServer(AWS_ENDPOINT, 8883);
  // mqttClient.setCallback(callback);
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String update_path = URL + AWS_ENDPOINT;
    http.begin(update_path);
  }
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
}

void loop() {

  String payload = ""; // payload, in this case will be either coordinates, junction, or both
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String update_path = URL + AWS_ENDPOINT;
    http.begin(update_path); // could need a request number at the end, not too sure
    int httpCode = http.GET();

    if(httpCode >0){
      String payload = http.getString();
      Serial.println("\nStatuscode: " + String(httpCode));
      Serial.println(payload);

      char json[500];
      payload.replace(" ", "");
      payload.replace("\n", "");
      payload.trim();
      payload.remove(0,1);
      payload.toCharArray(json, 500);
    }

  }else{
    Serial.println("connection lost");
  }
  // HTTP.addHeader("content type", ""); // hanbo help later
  delay(1000);
}

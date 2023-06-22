#include <HardwareSerial.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#define WIFI_SSID     "jamies_iphone"
#define PASSWORD      "jamieturner2"
String URL          = "https://balance-bug.5959pn4l16bde.eu-west-2.cs.amazonlightsail.com/";
String LOCAL_HOST   = "localhost:3000/";
String AWS_ENDPOINT = "/api/update"; // exposed port

// Random variables for testing POST
bool junction = false;
unsigned int xpos = 100;
unsigned int ypos = 10;
unsigned int orientation = 24;
bool line_array[3] = {0, 1, 0, 0, 1, 0};

TaskHandle_t uartMotor;
TaskHandle_t espServer;


// Establish WiFi connection
void connectToWiFi(){
  WiFi.begin(WIFI_SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    Serial.println(".");
    delay(1000);
  }
  Serial.println("Connected to internet!");
}

void httpRequests(){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    // sending data to the rover
    String update_path = URL + "/api/update"; // need to include endpoint in again
    http.begin(client, update_path); // could need a request number at the end, not too sure
    int httpResponsePost = http.POST("{\"xpos\": " + xpos + ",\n\"ypos\": " + ypos + ",\n\"degrees\": " + orientation + "'\n\"lines\": [" + line_array[0] + ", " + line_array[1] + ", " + line_array[2] + ", " + line_array[3] + ", " + line_array[4] + ", " + line_array[5] + "]"); // () needs the payload, a string in the format of the json data needed {"xpos": xpos, etc..}
    Serial.print("HTTP response code: " + String(httpResponsePost) + "\n");
    http.end();

    //recieving rover data
    http.begin(client, URL + "api/recieve");
    int httpResponseGet = http.GET(); // dont need to pass arguments
    String payload = http.getString(); // no clue what the format of payload is... it includes values that need to be passed to the drive control 
    Serial.print("HTTP response code: " + String(httpResponseGet) + "\n");
    Serial.print("HTTP payload is: " + payload + "\n");
    http.end();

  }else{
    Serial.println("connection lost");
  }
}

void setup() {
  xTaskCreatePinnedToCore(
    uart_motor,
    "UART-and-Motor",
    10000,
    NULL,
    1,
    &uartMotor,
    0
  );

  xTaskCreatePinnedToCore(
    esp_server,
    "ESP-and-Server",
    10000,
    NULL,
    1,
    &espServer,
    1
  );

  void uart_motor(void * pvParameters){
    
  }


  void esp_server(void * pvParameters){

  }

}

void loop() {}

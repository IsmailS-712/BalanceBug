#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>



const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.1.106:1880/update-sensor";

void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}



// This section of code is the main loop
//It constantly checks for UDP signals from the user and when received the rover's movement is controlled or the sensors are activated as required

void loop() 
{
  int packetSize = Udp.parsePacket();
  Serial.println("packet recieved");
  if (packetSize)
  {
    Serial.print(F("Received packet of size "));
    Serial.println(packetSize);
    Serial.print(F("From "));
    IPAddress remoteIp = Udp.remoteIP();
    IPAddress ip(192, 168, 0, 197);
    Serial.print(remoteIp);
    Serial.print(F(", port "));
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0)
    {
      packetBuffer[len] = 0;
    }
    // print content of packet received by rover
    Serial.println(F("Contents:"));
    Serial.println(packetBuffer);
    // execute command based on packet received
    if (packetBuffer[0] == 'w'){ //slow forward
      analogWrite(LPWM,50);
      analogWrite(RPWM,50);
      digitalWrite(LDIR,1);
      digitalWrite(RDIR,1);
    }
    else if(packetBuffer[0] == 's'){ //slow backward
      analogWrite(LPWM,50);
      analogWrite(RPWM,50);
      digitalWrite(LDIR,0);
      digitalWrite(RDIR,0);
    }
    else if(packetBuffer[0] == 'a'){ //slow left
      analogWrite(LPWM,20);
      analogWrite(RPWM,20);
      digitalWrite(LDIR,0);
      digitalWrite(RDIR,1);
    }
    else if(packetBuffer[0] == 'd'){ //slow right
      analogWrite(LPWM,20);
      analogWrite(RPWM,20);
      digitalWrite(LDIR,1);
      digitalWrite(RDIR,0);
    }
    else if(packetBuffer[0] == '1'){ // stop (executed upon key release)
      analogWrite(LPWM,0);
      analogWrite(RPWM,0);
      digitalWrite(LDIR,0);
      digitalWrite(RDIR,0);
      Udp.beginPacket(ip, 60472);
      String rock = String(rockDetected());
      Udp.print("Rocks");
      
      Udp.endPacket();
    }
    else if(packetBuffer[0] == 'f'){ // detect
      Udp.beginPacket(ip, 60472);
      String rock = String(rockDetected());
      Udp.print(rock);
      
      Udp.endPacket();
    }
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), 55125);

    Udp.write("RPLY");
    
    Udp.endPacket();
  }
}
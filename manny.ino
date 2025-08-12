#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>

IPAddress local_IP(10, 10, 10, 10);
IPAddress gateway(10, 10, 10, 10);
IPAddress subnet(255, 255, 255, 0);

Preferences preferences;

#define led 5
#define PWM_RESOLUTION 8  // 8-bit resolution (values 0-brightness)
#define PWM_FREQUENCY 5000  // 5kHz PWM frequency

// Wi-Fi credentials
String ssid;
String password;

const char* WAPssid = "manny";
const char* WAPpassword = "32sandwiches"; // must be at least 8 chars

bool alarming = false;
bool lit = false;

int brightness = 0;
int previousBrightness = 0;

// Create a WebServer object on port 80
WebServer server(80);

// Handle root URL
#include "credential-routes.h"

void alarmOn() {
  String jsonResponse = "{\"alarm\":\"on\"}";
  server.send(200, "application/json", jsonResponse);
  alarming = true;
}

void alarmOff() {
  String jsonResponse = "{\"alarm\":\"off\"}";
  server.send(200, "application/json", jsonResponse);
  alarming = false;

  if(lit){
    lightOn();
  }else{
    analogWrite(led, 0);
  }
}

void lightOn() {
  String jsonResponse = "{\"light\":\"on\", \"source\":\"manequin\"}";
  server.send(200, "application/json", jsonResponse);
  if(!lit){
    for(int i = 0; i < brightness; i++){
      analogWrite(led, i);
      delay(3);
    }
  }
  lit = true;
}

void lightOff() {
  String jsonResponse = "{\"light\":\"off\", \"source\":\"manequin\"}";
  server.send(200, "application/json", jsonResponse);
  if(lit){
    for(int i = brightness; i > 0; i--){
      analogWrite(led, i);
      delay(3);
    }
  }
  analogWrite(led, 0);
  lit = false;
}

void lightLow() {
  String jsonResponse = "{\"light\":\"on\", \"source\":\"manequin\"}";
  server.send(200, "application/json", jsonResponse);

  analogWrite(led, 16);
  lit = true;
} 
void lightMid() {
  String jsonResponse = "{\"light\":\"on\", \"source\":\"manequin\"}";
  server.send(200, "application/json", jsonResponse);

  analogWrite(led, 64);
  lit = true;
} 
void lightBright() {
  String jsonResponse = "{\"light\":\"on\", \"source\":\"manequin\"}";
  server.send(200, "application/json", jsonResponse);

  analogWrite(led, 128);
  lit = true;
}

void updateBrightness() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    Serial.println("received updated brightness value: " + body);
    server.send(200, "text/plain", "Data received");

    previousBrightness = brightness;
    brightness = body.toInt();

    if(brightness > previousBrightness && lit){
      for(int i = previousBrightness; i < brightness; i++){
        analogWrite(led, i);
        delay(2);
      }
      analogWrite(led, brightness);
    }else if(brightness < previousBrightness && lit){
      for(int i = previousBrightness; i > brightness; i--){
        analogWrite(led, i);
        delay(2);
      }
      analogWrite(led, brightness);
    }

    preferences.begin("manequin", false);
    preferences.putUChar("brightness", brightness);
    preferences.end();
  } else {
    server.send(400, "text/plain", "Bad Request");
  }

}

void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);

  analogWriteResolution(led, PWM_RESOLUTION);
  analogWriteFrequency(led, PWM_FREQUENCY);

  preferences.begin("manequin", false);
  brightness = preferences.getUChar("brightness", 0);
  preferences.end();

  preferences.begin("wifi", true);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("pass", "");
  preferences.end();

  analogWrite(led, brightness);

  WiFi.mode(WIFI_AP_STA);

  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(WAPssid, WAPpassword);
  Serial.print("access point IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave); // updates ssid and password
  server.on("/alarm_on", alarmOn);
  server.on("/alarm_off", alarmOff);
  server.on("/light_on", lightOn);
  server.on("/light_off", lightOff);
  server.on("/light_dim", lightLow); 
  server.on("/light_mid", lightMid);
  server.on("/light_bright", lightBright);
  server.on("/update_brightness", HTTP_POST, updateBrightness);

  server.begin();
  Serial.println("server started");

  delay(1000);

  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 30000;
  
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    server.handleClient();
    delay(50);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFi.softAPdisconnect(true);
    Serial.println("\nConnected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("failed to connect within timeout");
    #include "flash.h"
  }

  if(brightness != 0){
    lit = true;
  }
}

void alarm() {
  for(int i = 0; i < 255; i++){
    analogWrite(led, i);
    delay(2);
  }
  delay(500);
  for(int i = 255; i > 0; i--){
    analogWrite(led, i);
    delay(2);
  }
  analogWrite(led, 0);
}

void loop() {
  
  server.handleClient();

  yield();

  if(alarming){
    alarm();
  }

}
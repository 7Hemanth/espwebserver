#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
 
const char* ssid = "esp8266_self";
const char* password = "12345678";
 
ESP8266WebServer server(80);

void setSettings() {
    // expected
    // {"ip":"192.168.1.129","gw":"192.168.1.1","nm":"255.255.255.0"}
    Serial.println(F("postConfigFile"));
    String postBody = server.arg("plain");
    Serial.println(postBody);
 
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    if (error) {
        // if the file didn't open, print an error:
        Serial.print(F("Error parsing JSON "));
        Serial.println(error.c_str());
 
        String msg = error.c_str();
 
        server.send(400, F("text/html"), "Error in parsin json body! <br>"+msg);
 
    }else{
        JsonObject postObj = doc.as<JsonObject>();
 
        Serial.print(F("HTTP Method: "));
        Serial.println(server.method());
 
        if (server.method() == HTTP_POST) {
            if ((postObj.containsKey("ip"))) {
 
                Serial.print(F("Open config file..."));
                // Here you can open your file to store your config
                // Now I simulate It and set configFile a true
                bool configFile = true;
                if (!configFile) {
                    Serial.println(F("fail."));
                    server.send(304, F("text/html"), F("Fail to store data, can't open file!"));
                }else{
                    Serial.println(F("done."));
                    const char* address = postObj[F("ip")];
                    const char* gatway = postObj[F("gw")];
                    const char* netMask = postObj[F("nm")];
 
                    Serial.print("ip: ");
                    Serial.println(address);
                    Serial.print("gw: ");
                    Serial.println(gatway);
                    Serial.print("nm: ");
                    Serial.println(netMask);
 
//                  server.sendHeader("Content-Length", String(postBody.length()));
                    server.send(201, F("application/json"), postBody);
 
//                  Serial.println(F("Sent reset page"));
//                    delay(5000);
//                    ESP.restart();
//                    delay(2000);
                }
            }
            else {
                server.send(204, F("text/html"), F("No data found, or incorrect!"));
            }
        }
    }
}


void setRoom() {
    String postBody = server.arg("plain");
    Serial.println(postBody);
 
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    if (error) {
        // if the file didn't open, print an error:
        Serial.print(F("Error parsing JSON "));
        Serial.println(error.c_str());
 
        String msg = error.c_str();
 
        server.send(400, F("text/html"),
                "Error in parsin json body! <br>" + msg);
 
    } else {
        JsonObject postObj = doc.as<JsonObject>();
 
        Serial.print(F("HTTP Method: "));
        Serial.println(server.method());
 
        if (server.method() == HTTP_POST) {
            if (postObj.containsKey("name") && postObj.containsKey("type")) {
 
                Serial.println(F("done."));
 
                // Here store data or doing operation
 
 
                // Create the response
                // To get the status of the result you can get the http status so
                // this part can be unusefully
                DynamicJsonDocument doc(512);
                doc["status"] = "OK";
 
                Serial.print(F("Stream..."));
                String buf;
                serializeJson(doc, buf);
 
                server.send(201, F("application/json"), buf);
                Serial.print(F("done."));
 
            }else {
                DynamicJsonDocument doc(512);
                doc["status"] = "KO";
                doc["message"] = F("No data found, or incorrect!");
 
                Serial.print(F("Stream..."));
                String buf;
                serializeJson(doc, buf);
 
                server.send(400, F("application/json"), buf);
                Serial.print(F("done."));
            }
        }
    }
}
 
// Serving Hello world
void getHelloWord() {
    server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}
// Serving Hello world
void getSettings() {
    String response = "{";
 
    response+= "\"ip\": \""+WiFi.localIP().toString()+"\"";
    response+= ",\"gw\": \""+WiFi.gatewayIP().toString()+"\"";
    response+= ",\"nm\": \""+WiFi.subnetMask().toString()+"\"";
 
    if (server.arg("signalStrength")== "true"){
        response+= ",\"signalStrengh\": \""+String(WiFi.RSSI())+"\"";
    }
 
    if (server.arg("chipInfo")== "true"){
        response+= ",\"chipId\": \""+String(ESP.getChipId())+"\"";
        response+= ",\"flashChipId\": \""+String(ESP.getFlashChipId())+"\"";
        response+= ",\"flashChipSize\": \""+String(ESP.getFlashChipSize())+"\"";
        response+= ",\"flashChipRealSize\": \""+String(ESP.getFlashChipRealSize())+"\"";
    }
    if (server.arg("freeHeap")== "true"){
        response+= ",\"freeHeap\": \""+String(ESP.getFreeHeap())+"\"";
    }
    response+="}";
 
    server.send(200, "text/json", response);
}


 
// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Welcome to the REST Web Server"));
    });
    server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
    server.on(F("/settings"), HTTP_GET, getSettings);
    server.on(F("/setRoom"), HTTP_POST, setRoom);
    server.on(F("/settings"), HTTP_POST, setSettings);
}
 
// Manage not found URL
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
 
void setup(void) {
  Serial.begin(115200);
 //WiFi.mode(WIFI_STA);
  WiFi.softAP(ssid, password);
  Serial.println("");
 
  // Wait for connection
  //while (WiFi.status() != WL_CONNECTED) {
   // delay(500);
    //Serial.print(".");
  //}
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void) {
  server.handleClient();
}
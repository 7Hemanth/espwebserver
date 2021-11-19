#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

const char *ssid = "esp8266_self";
const char *password = "12345678";

AsyncWebServer server(80);

void setSettings();
void restServerRouting();
void setRoom();
void getHelloWord();
void getSettings();
void setRoom();



// Define routing
void restServerRouting()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html",
                            "Welcome to the REST Web Server"); });


  server.on("/helloWorld", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/json", "{\"name\": \"Hello world\"}"); });




  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
            {
          String response = "{";
 
    response+= "\"ip\": \""+WiFi.localIP().toString()+"\"";
    response+= ",\"gw\": \""+WiFi.gatewayIP().toString()+"\"";
    response+= ",\"nm\": \""+WiFi.subnetMask().toString()+"\"";
 
    if (request->arg("signalStrength")== "true"){
        response+= ",\"signalStrengh\": \""+String(WiFi.RSSI())+"\"";
    }
 
    if (request->arg("chipInfo")== "true"){
        response+= ",\"chipId\": \""+String(ESP.getChipId())+"\"";
        response+= ",\"flashChipId\": \""+String(ESP.getFlashChipId())+"\"";
        response+= ",\"flashChipSize\": \""+String(ESP.getFlashChipSize())+"\"";
        response+= ",\"flashChipRealSize\": \""+String(ESP.getFlashChipRealSize())+"\"";
    }
    if (request->arg("freeHeap")== "true"){
        response+= ",\"freeHeap\": \""+String(ESP.getFreeHeap())+"\"";
    }
    response+="}";
 
    request->send(200, "text/json", response); });

/*
  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    Serial.println("Running");
    if (request->url() == "/setRoom") {
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject((const char*)data);
      if (root.success()) {
        if (root.containsKey("command")) {
          Serial.println(root["command"].asString()); // Hello
        }
      }
      request->send(200, "text/plain", "end");
    }
  });
  */

  server.on("/setRoom", HTTP_POST, [](AsyncWebServerRequest *request)
            {
          String postBody = request->arg("plain");
    Serial.println(postBody);
 
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    if (error) {
        // if the file didn't open, print an error:
        Serial.print(F("Error parsing JSON "));
        Serial.println(error.c_str());
 
        String msg = error.c_str();
 
        request->send(400, F("text/html"),
                "Error in parsin json body! <br>" + msg);
 
    } else {
        JsonObject postObj = doc.as<JsonObject>();
 
        Serial.print(F("HTTP Method: "));
        Serial.println(request->method());
 
        if (request->method() == HTTP_POST) {
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
 
                request->send(201, F("application/json"), buf);
                Serial.print(F("done."));
 
            }else {
                DynamicJsonDocument doc(512);
                doc["status"] = "KO";
                doc["message"] = F("No data found, or incorrect!");
 
                Serial.print(F("Stream..."));
                String buf;
                serializeJson(doc, buf);
 
                request->send(400, F("application/json"), buf);
                Serial.print(F("done."));
            }
        }
    }
     });


    
  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request)
            {
           Serial.println(F("postConfigFile"));
    String postBody = request->arg("plain");
    Serial.print("test");
    Serial.println(postBody);
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    if (error) {
        // if the file didn't open, print an error:
        Serial.print(F("Error parsing JSON "));
        Serial.println(error.c_str());
        String msg = error.c_str();
 
        request->send(400, F("text/html"), "Error in parsin json body! <br>"+msg);
 
    }else{
        JsonObject postObj = doc.as<JsonObject>();
 
        Serial.print(F("HTTP Method: "));
        Serial.println(request->method());
 
        if (request->method() == HTTP_POST) {
            if ((postObj.containsKey("ip"))) {
 
                Serial.print(F("Open config file..."));
                // Here you can open your file to store your config
                // Now I simulate It and set configFile a true
                bool configFile = true;
                if (!configFile) {
                    Serial.println(F("fail."));
                    request->send(304, F("text/html"), F("Fail to store data, can't open file!"));
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
                    request->send(201, F("application/json"), postBody);
 
//                  Serial.println(F("Sent reset page"));
//                    delay(5000);
//                    ESP.restart();
//                    delay(2000);
                }
            }
            else {
                request->send(204, F("text/html"), F("No data found, or incorrect!"));
            }
        }
    } });
}

void handleNotFound(AsyncWebServerRequest *request) {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";
  for (uint8_t i = 0; i < request->args(); i++) {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }
 request->send(404, "text/plain", message);
}
 

void setup(void)
{
  Serial.begin(115200);
  // WiFi.mode(WIFI_STA);
  WiFi.softAP(ssid, password);
  Serial.println("");


  Serial.println("");
  Serial.print("Connect to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  // Set server routing
  restServerRouting();
  // Set not found response
  // Start server
   server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  // server.handleClient();
}
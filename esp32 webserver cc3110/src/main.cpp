#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>

const char *ssid = "esp8266_self";
const char *password = "12345678";

AsyncWebServer server(80);

void restServerRouting();

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

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/setRoom", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                         {
  StaticJsonDocument<200> data;
  if(json.is<JsonArray>())
  {
    data = json.as<JsonArray>();
  }
  else if (json.is<JsonObject>())
  {
    data = json.as<JsonObject>();
  }
  String response;
  serializeJson(data, response);
  request->send(200, "application/json", response);
  Serial.println(response);

JsonObject postObj = data.as<JsonObject>();
                      const char* id_ = postObj[F("id")];
                    const char* name_ = postObj[F("name")];
                    const char* type_ = postObj[F("type")];
 
                    Serial.print("ID: ");
                    Serial.println(id_);
                    Serial.print("Name: ");
                    Serial.println(name_);
                    Serial.print("Type:");
                    Serial.println(type_); });

  server.addHandler(handler);

  AsyncCallbackJsonWebHandler *handler1 = new AsyncCallbackJsonWebHandler("/settings", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                          {
  StaticJsonDocument<200> data;
  if(json.is<JsonArray>())
  {
    data = json.as<JsonArray>();
  }
  else if (json.is<JsonObject>())
  {
    data = json.as<JsonObject>();
  }
  String response;
  serializeJson(data, response);
  request->send(200, "application/json", response);
  Serial.println(response);

JsonObject postObj = data.as<JsonObject>();
                   const char* address = postObj[F("ip")];
                    const char* gatway = postObj[F("gw")];
                    const char* netMask = postObj[F("nm")];
 
                    Serial.print("ip: ");
                    Serial.println(address);
                    Serial.print("gw: ");
                    Serial.println(gatway);
                    Serial.print("nm: ");
                    Serial.println(netMask); });

  server.addHandler(handler1);
}

void handleNotFound(AsyncWebServerRequest *request)
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";
  for (uint8_t i = 0; i < request->args(); i++)
  {
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

}
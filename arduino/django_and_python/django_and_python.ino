#include "DHT.h"    // DHT library include
#include "Ethernet.h"
#include "SPI.h"
#include "RestClient.h"
#include "ArduinoJson.h"
DHT dht;

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

StaticJsonBuffer<2000> constantsJSONBuffer;
JsonObject& constants = constantsJSONBuffer.createObject();

void setup() {
  dht.setup(10); // init DHT on port 10
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(7, OUTPUT);
  Serial.begin(9600);
  while (!Serial) {};

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  // print your local IP address:
  printIPAddress();
}

void loop() {
  
  delay(dht.getMinimumSamplingPeriod());
  dht.getHumidity();
  dht.getTemperature();
  Serial.println("===start===");
  set_constance<int>("FREQUENCY");
  set_constance<bool>("LID_STATUS");
  set_constance<float>("TEMPERATURE_LIMIT");
  digitalWrite(7, constants["LID_STATUS"] ? HIGH : LOW);
  Serial.println("===end===");
  delay(constants["FREQUENCY"]);

//  if (isnan(t) || isnan(h)) {
//    Serial.println("Failed to read from DHT");
//  } else {
//    Serial.print("Humidity: ");
//    Serial.print(h);
//    Serial.print(" %\t");
//    Serial.print("Temperature: ");
//    Serial.print(t);
//    Serial.println(" *C");
//    digitalWrite(t > constants["TEMPERATURE_LIMIT"] ? 4 : 5, HIGH);
//    digitalWrite(t < constants["TEMPERATURE_LIMIT"] ? 4 : 5, LOW);
//  }
}

void printIPAddress() {
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
}

template<typename T> void set_constance(String const_name) {
  String response = get_constance(const_name);
  JsonObject& JSON = parse_json(response);
  if (JSON.success()) {
    constants[const_name] = JSON.get<T>(const_name);
  }
}

String get_constance(String constance_name) {
  RestClient client = RestClient("10.0.4.81", 8000);
  String server_response = "";
  String url = "/config/" + constance_name + "/";
  int url_len = url.length() + 1;
  char char_url_array[url_len];
  url.toCharArray(char_url_array, url_len);
  int statusCode = client.get(char_url_array, &server_response);
  Serial.print(url);
  Serial.print(" [Status: ");
  Serial.print(statusCode);
  Serial.print("] ");
  Serial.println(server_response);
  delay(200);
  return server_response;
}


JsonObject& parse_json(String json) {
  StaticJsonBuffer<2000> jsonBuffer;
  JsonObject& parsedJson = jsonBuffer.parseObject(json);
  return parsedJson;
}



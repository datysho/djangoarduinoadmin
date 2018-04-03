#include "Ethernet.h"
#include "SPI.h"
#include "RestClient.h"
#include "ArduinoJson.h"
#include "DHT.h"

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

DHT dht;
StaticJsonBuffer<2000> constantsJSONBuffer;
JsonObject& constants = constantsJSONBuffer.createObject();
RestClient client = RestClient("10.0.4.81", 8000);

void setup() {
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
  dht.setup(8);
}

void loop() {
  Serial.println("===start===");
  set_constance<int>("FREQUENCY");
  set_constance<bool>("LID_STATUS");
  set_constance<float>("TEMPERATURE_LIMIT");
  digitalWrite(7, constants["LID_STATUS"] ? HIGH : LOW);
  checkTemperature();
  Serial.println("===end===");
  delay(constants["FREQUENCY"]);
}

void checkTemperature() {
  delay(dht.getMinimumSamplingPeriod());
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  bool is_exceeded = t > constants["TEMPERATURE_LIMIT"];
  digitalWrite(is_exceeded ? 5 : 4, HIGH);
  digitalWrite(is_exceeded ? 4 : 5, LOW);
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");
    String json_to_post_string = "temperature=" + String(t) + "&" + "humidity=" + String(h);
    int json_to_post_string_len = json_to_post_string.length() + 1;
    char char_url_array[json_to_post_string_len];
    json_to_post_string.toCharArray(char_url_array, json_to_post_string_len);
    client.post("/logs/", char_url_array);
  }
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
  return server_response;
}


JsonObject& parse_json(String json) {
  StaticJsonBuffer<2000> jsonBuffer;
  JsonObject& parsedJson = jsonBuffer.parseObject(json);
  return parsedJson;
}



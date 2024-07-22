#include <WiFi.h>
#include <PubSubClient.h> // vvvv DO NOT REMOVE!!! this is so that we can ensure that long long variables work correctly
#define ARDUINOJSON_USE_LONG_LONG 1 
#include <ArduinoJson.h>
#include "time.h" //time library

#define wifi_ssid "wifi_name"             //wifi ssid
#define wifi_password "wifi_password"     //wifi password

#define mqtt_server "address_or_domain_name"       // server name or IP
#define mqtt_user "user_name"           // username
#define mqtt_password "user_password"   // password

#define topic "root.sg.d1"  //the topic to publish on, for iotdb doesnt really matter


bool debug = false;             //Display log message if true

WiFiClient espClient;           //wifi handler
PubSubClient client(espClient); // mqtt handler

// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";

// Variable to save current epoch time
unsigned long epochtime; 
unsigned long long epochtime_ms;

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup()
{
    Serial.begin(115200);
    setup_wifi();                           //Connect to Wifi network
    configTime(0, 0, ntpServer);            //gets epoch time
    client.setServer(mqtt_server, 1883);    //Configure MQTT connection, change port if needed depends on the loadbalancer.

}


//Setup connection to wifi
void setup_wifi() {
  delay(20);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

 Serial.println("");
 Serial.println("WiFi is OK ");
 Serial.print("=> ESP32 new IP address is: ");
 Serial.print(WiFi.localIP());
 Serial.println("");
}

//Reconnect to wifi if connection is lost
void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("[Error] Not connected: ");
      Serial.print(client.state());
      Serial.println("Wait 5 seconds before retry.");
      delay(5000);
    }
  }
}


void loop()
{
      if (!client.connected()) {
      reconnect();
    }
    epochtime = getTime();
    epochtime_ms = epochtime * 1000LL;  // Convert to milliseconds
    float t = temperatureRead(); // temperature data variable

    // Create JSON document
    StaticJsonDocument<200> JSONdoc;
    JSONdoc["device"] = "root.fss.ahmed.d1";
    JSONdoc["timestamp"] = epochtime_ms;

    JsonArray measurements = JSONdoc.createNestedArray("measurements");
    measurements.add("s1");

    JsonArray values = JSONdoc.createNestedArray("values");
    values.add(t);
    // Serialize JSON to buffer
    char JSONmessageBuffer[256];
    serializeJson(JSONdoc, JSONmessageBuffer, sizeof(JSONmessageBuffer));
      
    // just want to see payload on the console :)
    Serial.println("Sending message to MQTT topic..");
    Serial.println(JSONmessageBuffer);

    //Publish values to MQTT topics
    client.publish(topic, JSONmessageBuffer);   // Publish readings on topic (root.sg.d1)

    delay(1000);
}

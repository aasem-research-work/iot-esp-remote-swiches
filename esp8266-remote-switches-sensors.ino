#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define RELAYS 4
#define DHTPIN D2
#define DHTTYPE DHT11

const char* ssid = "WorldNet33";
const char* password = "wifipassword";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* topic_operation = "testlocation1/topic_operation";
const char* topic_config = "testlocation2/topic_config";

const int relayPins[] = {D3, D6, D7, D5};

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastBlinkTime = 0;
const int blinkInterval = 500;
bool wasConnected = false;

void commander(StaticJsonDocument<200>& doc) {
  const char* command = doc["command"];
  
  if (strcmp(command, "getStatus") == 0) {
    Serial.println("Received getStatus command.");
    
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    StaticJsonDocument<200> responseDoc;
    responseDoc["command"] = "setStatus";
    responseDoc["timestamp"] = millis();
    
    for(int i = 0; i < RELAYS; i++) {
      String switchName = "switch" + String(i + 1);
      responseDoc[switchName] = digitalRead(relayPins[i]) == HIGH ? "ON" : "OFF";
    }
    
    responseDoc["temperature"] = temperature;
    responseDoc["humidity"] = humidity;
    
    String responseStr;
    serializeJson(responseDoc, responseStr);
    
    client.publish(topic_operation, responseStr.c_str());

  } else if (strcmp(command, "setStatus") == 0) {
    Serial.println("Received setStatus command.");
    const char* switch1 = doc["switch1"];
    const char* switch2 = doc["switch2"];
    const char* switch3 = doc["switch3"];
    const char* switch4 = doc["switch4"];

    if (strcmp(switch1, "ON") == 0) {
      digitalWrite(relayPins[0], HIGH);
    } else if (strcmp(switch1, "OFF") == 0) {
      digitalWrite(relayPins[0], LOW);
    }

    if (strcmp(switch2, "ON") == 0) {
      digitalWrite(relayPins[1], HIGH);
    } else if (strcmp(switch2, "OFF") == 0) {
      digitalWrite(relayPins[1], LOW);
    }

    if (strcmp(switch3, "ON") == 0) {
      digitalWrite(relayPins[2], HIGH);
    } else if (strcmp(switch3, "OFF") == 0) {
      digitalWrite(relayPins[2], LOW);
    }
 
    if (strcmp(switch4, "ON") == 0) {
      digitalWrite(relayPins[3], HIGH);
    } else if (strcmp(switch4, "OFF") == 0) {
      digitalWrite(relayPins[3], LOW);
    }  
  } else {
    Serial.println("Unknown command received.");
  }
}

void setup() {
  for (int i = 0; i < RELAYS; i++) {
    pinMode(relayPins[i], OUTPUT);
  }
  
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  dht.begin();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  
  if (error) {
    Serial.println("Failed to parse JSON!");
    return;
  }
  
  commander(doc);
}

void loop() {
  if (!client.connected()) {
    if (client.connect("ESP8266Client1231")) {
      if (!wasConnected) {
        Serial.println("MQTT Connected");
        client.subscribe(topic_operation);
        client.subscribe(topic_config);
      }
    } else {
      Serial.println("MQTT Connection failed");
      wasConnected = false;
    }
  }

  client.loop();
}

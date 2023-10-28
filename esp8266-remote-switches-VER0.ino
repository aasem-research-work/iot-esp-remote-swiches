#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "WorldNet33";
const char* password = "Password of your wifi";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* topic_operation = "testlocation1/topic_operation";
const char* topic_config = "testlocation2/topic_config";
const int relayPins[] = {5, 4, 14, 12};

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastBlinkTime = 0;
const int blinkInterval = 500;
bool wasConnected = false;

void commander(StaticJsonDocument<200>& doc) {
  const char* command = doc["command"];
  
  if (strcmp(command, "getStatus") == 0) {
    Serial.println("Received getStatus command.");

    // Create a JSON object to store the statuses
    StaticJsonDocument<200> responseDoc;

    // Add the command and timestamp
    responseDoc["command"] = "setStatus";
    responseDoc["timestamp"] = millis();

    // Check the status of each relay pin and add it to the JSON object
    responseDoc["switch1"] = digitalRead(relayPins[0]) == HIGH ? "ON" : "OFF";
    responseDoc["switch2"] = digitalRead(relayPins[1]) == HIGH ? "ON" : "OFF";
    responseDoc["switch3"] = digitalRead(relayPins[2]) == HIGH ? "ON" : "OFF";

    // Serialize the JSON object to a string
    String responseStr;
    serializeJson(responseDoc, responseStr);

    // Publish the JSON string to the same MQTT topic
    client.publish(topic_operation, responseStr.c_str());

  } else if (strcmp(command, "setStatus") == 0) {
    Serial.println("Received setStatus command.");
    const char* switch1 = doc["switch1"];
    const char* switch2 = doc["switch2"];
    const char* switch3 = doc["switch3"];

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
  } else {
    Serial.println("Unknown command received.");
  }
}



void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
  }

  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
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
  lastBlinkTime = millis();
  digitalWrite(LED_BUILTIN, LOW);
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
  
  String receivedPayload;
  serializeJson(doc, receivedPayload);
  Serial.print("Received JSON: ");
  Serial.println(receivedPayload);
  
  commander(doc); //const char* command = doc["command"];
  
  
}

void loop() {
  if (!client.connected()) {
    if (client.connect("ESP8266Client1231")) {
      if (!wasConnected) { // Only print the message if it was previously disconnected
        Serial.println("MQTT Connected");
        client.subscribe(topic_operation);
        client.subscribe(topic_config);
        digitalWrite(LED_BUILTIN, LOW);
        wasConnected = true; // Update the state
      }
    } else {
      Serial.println("MQTT Connection failed");
      wasConnected = false; // Update the state
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }

  client.loop();

  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = currentTime;
      if (digitalRead(LED_BUILTIN) == LOW) {
        digitalWrite(LED_BUILTIN, HIGH);
      } else {
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
  }
}

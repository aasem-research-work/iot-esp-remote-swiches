
# MQTT-Controlled ESP8266 Relay

## Purpose

This repository contains an Arduino sketch that allows you to control relay switches through MQTT messages. The code runs on an ESP8266 and subscribes to MQTT topics for receiving operation and configuration messages. The message format is JSON, and the sketch uses the PubSubClient and ArduinoJson libraries for MQTT and JSON handling, respectively.

The sketch supports the following JSON commands:

- `getStatus`: Queries the current state of the relays and responds with a JSON object.
- `setStatus`: Sets the state of the relays based on the JSON object received.

## Requirements

- Arduino IDE
- ESP8266 with relay board
- MQTT broker (e.g., broker.emqx.io)
- MQTTX or a similar MQTT client tool for testing
- Libraries:
  - PubSubClient
  - ArduinoJson

## Deployment

### Hardware Setup

1. Connect the ESP8266 to your computer.
2. Attach the relay board to the ESP8266's GPIO pins as described in the sketch.

### Software Setup

1. Clone this repository to your local machine.
2. Open the Arduino sketch in the Arduino IDE.
3. Install the required Arduino libraries:
   - PubSubClient: Via Arduino Library Manager or [GitHub Repository](https://github.com/knolleary/pubsubclient)
   - ArduinoJson: Via Arduino Library Manager or [GitHub Repository](https://github.com/bblanchon/ArduinoJson)
4. Update the `ssid`, `password`, and `mqtt_server` variables in the sketch with your own WiFi and MQTT broker details.
5. Compile and upload the sketch to your ESP8266.

## Testing

### Using MQTTX

1. Download and install MQTTX from [here](https://mqttx.app/).
2. Connect to your MQTT broker.
3. Subscribe to the topics you've set in the Arduino sketch (`testlocation1/topic_operation` and `testlocation2/topic_config` by default).
4. Publish a message to `testlocation1/topic_operation` with the payload `{"command": "getStatus"}` to query the current relay statuses.
5. You should receive a message in the same topic with the current statuses.
6. Publish a message to `testlocation1/topic_operation` with the payload `{"command": "setStatus", "switch1": "ON", "switch2": "OFF", "switch3": "ON"}` to change the relay statuses.
7. The relays should change according to the JSON command, and you can confirm by issuing another `getStatus`.


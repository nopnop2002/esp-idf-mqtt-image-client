#!/bin/bash
#set -x
mosquitto_pub -h broker.hivemq.com -p 1883 -t "testtopic/electron/esp32.jpeg" -f esp32.jpeg
echo mosquitto_pub -h broker.hivemq.com -p 1883 -t "testtopic/electron/esp32.jpeg" -f esp32.jpeg
sleep 10
mosquitto_pub -h broker.hivemq.com -p 1883 -t "testtopic/electron/esp_logo.png" -f esp_logo.png
echo mosquitto_pub -h broker.hivemq.com -p 1883 -t "testtopic/electron/esp_logo.png" -f esp_logo.png

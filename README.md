# esp-idf-mqtt-image-client
GUI MQTT client for image display for esp-idf.   
I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
There is an example of using the component [here](https://github.com/Molorius/ESP32-Examples).
It's a great job.   

I use [this](https://github.com/emqx/MQTT-Client-Examples/tree/master/mqtt-client-Electron) index.html.   
I use [this](https://bulma.io/) open source framework.   

![mqtt-image-view-1](https://user-images.githubusercontent.com/6020549/165009289-8ce9432c-619b-44b2-b741-29c91ae3cbc0.jpg)
![mqtt-image-view-2](https://user-images.githubusercontent.com/6020549/165009292-6a027230-079f-49fb-80fb-6816948c85f9.jpg)

# Software requirements
esp-idf v4.4/v5.0.   

# Installation
```
git clone https://github.com/nopnop2002/esp-idf-mqtt-image-client
cd esp-idf-mqtt-image-client
git clone https://github.com/Molorius/esp32-websocket components/websocket
idf.py set-target {esp32/esp32s2/esp32s3/esp32c3}
idf.py menuconfig
idf.py flash monitor
```

# Application Setting
![config-top](https://user-images.githubusercontent.com/6020549/165009333-2778f9a6-2ed7-4c45-bff6-90fc0e5ab21d.jpg)
![config-app-1](https://user-images.githubusercontent.com/6020549/165009560-cf56805d-c8a6-452b-9ae4-4422d9df3c4c.jpg)

You can use the MDNS hostname instead of the IP address.   
- esp-idf V4.3 or earlier   
 You will need to manually change the mDNS strict mode according to [this](https://github.com/espressif/esp-idf/issues/6190) instruction.   
- esp-idf V4.4 or later  
 If you set CONFIG_MDNS_STRICT_MODE = y in sdkconfig.default, the firmware will be built with MDNS_STRICT_MODE = 1.

![config-app-2](https://user-images.githubusercontent.com/6020549/165009568-887efd6a-00d1-4ae4-ba65-07785e8e25b9.jpg)

# How to use
- Open browser.   
- Enter the esp32 address in your browser's address bar.   
- You can use the mDNS hostname instead of the IP address.   
 Default mDNS name is esp32-server.local.   
- Press Connect button. You can use broker.hivemq.com.   
- Press Subscribe button.   


You can publish new topic using mosquitto_pub.   
```
mosquitto_pub -h broker.hivemq.com -p 1883 -t "testtopic/electron/esp32.jpeg" -f esp32.jpeg
mosquitto_pub -h broker.hivemq.com -p 1883 -t "testtopic/electron/esp_logo.png" -f esp_logo.png
```

# How this project work   
Read [Sending_image_via_mqtt.txt](https://github.com/nopnop2002/esp-idf-mqtt-image-client/blob/main/Sending_image_via_mqtt.txt).   


# Reference
https://github.com/nopnop2002/esp-idf-mqtt-client

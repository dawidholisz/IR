#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "WIFI_SSID"
#define WLAN_PASS       "WIFI_P@$$W0rd"

#define AIO_SERVER      "MQTT.server"
#define AIO_SERVERPORT  1883             


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT);

/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish lightSensorPublisher = Adafruit_MQTT_Publish(&mqtt,"IR",MQTT_QOS_1);

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

const int lightSensor = A0;

void setup() {
  pinMode(lightSensor, INPUT);

  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());


}

uint32_t x=0;

void loop() {
  MQTT_connect();

Serial.println(analogRead(lightSensor));
if(analogRead(lightSensor)<150)
{
  Serial.println("Turn ON");  
  lightSensorPublisher.publish("ON");
}
else
{
  Serial.println("Turn OFF");  
  lightSensorPublisher.publish("OFF");
}
  delay(2500);
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

#include <DHT.h>
#include <DHT_U.h>
#include "UbidotsEsp32Mqtt.h"

/******************** SETTING UBIDOST ***********************/
//const char *UBIDOTS_TOKEN = "BBFF-qde03o6mT1M3vyGxr76y6rWoCZUxD0";            // Put here your Ubidots TOKEN_Esteban
  const char *UBIDOTS_TOKEN = "BBFF-XqlP0TkhfPEJs6woMYDj13HfzCu2AS";            // Put here your Ubidots TOKEN_Johana
const char *WIFI_SSID = "FAMILIA GOMEZ";                // Put here your Wi-Fi SSID
const char *WIFI_PASS = "AMIJOLISO.";                // Put here your Wi-Fi password
//PUBLISH 
const char *PUBLISH_DEVICE_LABEL = "TomatoesFarm";     // Put here your Device label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL = "temperatura";   // Put here your Variable label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL2 = "h.relativa";   // Put here your Variable label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL3 = "h.suelo";   // Put here your Variable label to which data  will be published


//SUBSCRIBE
const char *SUBSCRIBE_DEVICE_LABEL = "TomatoesFarm";   // Replace with the device label to subscribe to
const char *SUBSCRIBE_VARIABLE_LABEL = "led"; // Replace with the variable label to subscribe to

const int PUBLISH_FREQUENCY = 5000; // Update rate in millisecondsx

unsigned long timer;

/************* VARIABLES **************/
//ADC
float conversion=3.3/4095;

/*TEMP
uint8_t analogPin = 25; 
float vtemp=0.0;
float temp=0;
*/
//HUMEDAD_TIERRA
uint8_t sensorHumedadTierra = 34;
int tierra=0;

//HUMEDAD RELATIVA
int sensor=4;
int temperatura=0;
int humedad=0; 

//LED
const uint8_t LED = 18; // Pin used to write data based on 1's and 0's coming from Ubidots


//DECLARACION DE OBJETOS
Ubidots ubidots(UBIDOTS_TOKEN);
DHT dht(sensor,DHT11);

/****************** AUXILIAR FUNCTIONS **********************/
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    if ((char)payload[0] == '1')
    {
      digitalWrite(LED, HIGH);
    }
    else
    {
      digitalWrite(LED, LOW);
    }
  }
  Serial.println();
}

/************** MAIN ***************/
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(SUBSCRIBE_DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL); // Insert the device and variable's Labels, respectively
  pinMode(LED,OUTPUT);
  timer = millis();
  dht.begin();
  pinMode(sensor, INPUT);//Humedad y temperatura
}

void loop()
{
  if (!ubidots.connected())
  {
    ubidots.reconnect();
    ubidots.subscribeLastValue(SUBSCRIBE_DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL); // Insert the device and variable's Labels, respectively
  }
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
   // float value = analogRead(analogPin);
    float value2= analogRead(sensorHumedadTierra);
    //vtemp=(value)*conversion;
    //temp=(vtemp*100)/(1);

    if(value2>=4000){
      Serial.print(">> El sensor esta desconectado o fuera del suelo <<  ");
      Serial.println(value2);
      tierra=0;
    }else if(value2<4000 && value2>=2800){
      Serial.print(">> El suelo esta seco <<  ");
      Serial.println(value2);
      tierra=2;
    }else if(value2<2800 && value2>=2100){
      Serial.print(">> El suelo esta humedo <<  ");      
      Serial.println(value2);
      tierra=5;
    }else if(value2<2100 && value2>=1000){
      Serial.print(">> El suelo esta muy humedo <<  ");      
      Serial.println(value2);
      tierra=8;
    }else if(value2<1000){
      Serial.print(">> El sensor esta practicamente en agua <<  ");
      Serial.println(value2);
      tierra=10;
    }

       ubidots.add(PUBLISH_VARIABLE_LABEL, temperatura); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL2, humedad); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL3, tierra); // Insert your variable Labels and the value to be sent
    ubidots.publish(PUBLISH_DEVICE_LABEL);
   /* Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println("C°");*/

    timer = millis();
  }

  ubidots.loop();
  temperatura=dht.readTemperature();
  humedad=dht.readHumidity();
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print("   Humedada: ");
  Serial.println(humedad);

  delay(2000);
}
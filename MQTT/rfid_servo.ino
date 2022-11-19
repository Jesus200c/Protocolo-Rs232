#include "UbidotsEsp32Mqtt.h"
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

//ESP32
#define SS_PIN 5 
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;


boolean l1 = 0;
boolean l2 = 0;
boolean l3 = 0;

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// Init array that will store new NUID
byte nuidPICC[4];

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
String DatoHex;
const String UserReg_1 = "973F5B34"; 
//const String UserReg_2 = "B3FB5719";
const String UserReg_2 = "C08BFA32";
const String UserReg_3 = "BAE67881";


// Tag1 #1 : 973F5B34
// Card #1 : B3FB5719   //C08BFA32

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

const char *UBIDOTS_TOKEN = "BBFF-3HuNyEa6aSQpPSrdABLoPLkMvqvEgO";  // Put here your Ubidots TOKEN BBFF-Dxt9tp2sn8uSSvQgxzP2yPhpxPDQTx
const char *WIFI_SSID = "Redmi";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "hola1234";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "ESP32";   // Put here your Device label to which data  will be published


const char *VARIABLE_LABEL = "PRUEBA"; // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL2 = "torno"; 
const char *VARIABLE_LABEL3 = "cnc"; 
const char *VARIABLE_LABEL4 = "fresadora"; 

const int PUBLISH_FREQUENCY = 1000; // Update rate in milliseconds

unsigned long timer;
uint8_t analogPin = 34; // Pin used to read data from GPIO34 ADC_CH6.
uint8_t led1 = 25; //led1
uint8_t led2 = 32; //led2
uint8_t led3 = 33;
uint8_t led4 = 27;
Ubidots ubidots(UBIDOTS_TOKEN);

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

Servo servo; //Instanciamos nuestro servo
int pinServo=2;



void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/
int led;

void setup() 
{
  
   Serial.begin(9600);
   pinMode (led1, OUTPUT);
   pinMode (led2, OUTPUT);
   pinMode (led3, OUTPUT);
   pinMode (led4, OUTPUT);
   SPI.begin(); // Init SPI bus
   rfid.PCD_Init(); // Init MFRC522
   Serial.println();
   Serial.print(F("Reader :"));
   rfid.PCD_DumpVersionToSerial();
   for (byte i = 0; i < 6; i++) {
     key.keyByte[i] = 0xFF;
   } 
   DatoHex = printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
   Serial.println();
   Serial.println();
   Serial.println("Iniciando el Programa");

   servo.attach(pinServo, 500, 2500);

  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
}

int pos = 0; //Initialize the position of the servo (in degrees) 

void loop() 
{

  l1=0;
  l2=0;
  l3=0;
  
    if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {


     // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
     if ( ! rfid.PICC_IsNewCardPresent()){return;}
     
     // Verify if the NUID has been readed
     if ( ! rfid.PICC_ReadCardSerial()){return;}
     
     Serial.print(F("PICC type: "));
     MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
     Serial.println(rfid.PICC_GetTypeName(piccType));
     // Check is the PICC of Classic MIFARE type
     if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
     {
       Serial.println("Su Tarjeta no es del tipo MIFARE Classic.");
       return;
     }
     
     if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] )
     {
       Serial.println("Se ha detectado una nueva tarjeta.   ->" + DatoHex);
       
       // Store NUID into nuidPICC array
       //for (byte i = 0; i < 4; i++) {nuidPICC[i] = rfid.uid.uidByte[i];}
    
       DatoHex = printHex(rfid.uid.uidByte, rfid.uid.size);
       //Serial.print("Codigo Tarjeta: "); Serial.println(DatoHex);



    
       if(UserReg_1 == DatoHex)
       {
        l1=1;
        ubidots.add(VARIABLE_LABEL2, l3);
        ubidots.add(VARIABLE_LABEL3, l1);
        ubidots.add(VARIABLE_LABEL4, l2);
    
         ubidots.publish(DEVICE_LABEL);
        Serial.println("CNC - PUEDE INGRESAR");  //***************************************************************
        servo.write(0);  //Movemos el servo a 0 grados
        digitalWrite (led1, HIGH);
        
        Serial.println(l1);
        delay(10000);
        servo.write(90); //Movemos el servo a 90 grados
        digitalWrite (led1, LOW);
        l1=0;
        Serial.println(l1);
        delay(1500);
        ubidots.add(VARIABLE_LABEL2, l3);
        ubidots.add(VARIABLE_LABEL3, l1);
        ubidots.add(VARIABLE_LABEL4, l2);
    
        ubidots.publish(DEVICE_LABEL);
       }
       
       else if(UserReg_2 == DatoHex)
       {
         l2=1;
         ubidots.add(VARIABLE_LABEL2, l3);
         ubidots.add(VARIABLE_LABEL3, l1);
         ubidots.add(VARIABLE_LABEL4, l2);
    
         ubidots.publish(DEVICE_LABEL);
        Serial.println("FRESADORA - PUEDE INGRESAR");    //  *********************************************************
        servo.write(0);  //Movemos el servo a 0 grados
        digitalWrite (led2, HIGH);
       
        
    
        Serial.println(l2);
        delay(10000);
        servo.write(90); //Movemos el servo a 90 grados
        digitalWrite (led2, LOW);
        
        Serial.println(l1);
        delay(1500);
        l2=0;

        ubidots.add(VARIABLE_LABEL2, l3);
        ubidots.add(VARIABLE_LABEL3, l1);
        ubidots.add(VARIABLE_LABEL4, l2);
    
        ubidots.publish(DEVICE_LABEL);
       }
        else if(UserReg_3 == DatoHex)
       {
        l3=1;
        ubidots.add(VARIABLE_LABEL2, l3);
        ubidots.add(VARIABLE_LABEL3, l1);
        ubidots.add(VARIABLE_LABEL4, l2);

        ubidots.publish(DEVICE_LABEL);
        Serial.println("TORNO - PUEDE INGRESAR");        //***********************************************************
        servo.write(0);  //Movemos el servo a 0 grados
        digitalWrite (led3, HIGH);
        
        Serial.println(l3);
        delay(10000);
        servo.write(90); //Movemos el servo a 90 grados
        digitalWrite (led3, LOW);
        l3=0;
        
        Serial.println(l1);                                                                                 
        delay(1500);

        ubidots.add(VARIABLE_LABEL2, l3);
        ubidots.add(VARIABLE_LABEL3, l1);
        ubidots.add(VARIABLE_LABEL4, l2);

        
        ubidots.publish(DEVICE_LABEL);

        
       }
       else
       {
        Serial.println("NO ESTA REGISTRADO - PROHIBIDO EL INGRESO");
        digitalWrite (led4, HIGH);
        delay(200);
        digitalWrite (led4, LOW);
        delay(200);
        digitalWrite (led4, HIGH);
        delay(200);
        digitalWrite (led4, LOW);
        delay(200);
        digitalWrite (led4, HIGH);
        delay(200);
        digitalWrite (led4, LOW);
        delay(200);
        digitalWrite (led4, HIGH);
        delay(200);
        digitalWrite (led4, LOW);
        delay(200);
        digitalWrite (led4, HIGH);
        delay(200);
        digitalWrite (led4, LOW);
        delay(200);
        digitalWrite (led4, HIGH);
        delay(200);
        digitalWrite (led4, LOW);
        delay(200);
       }  
       Serial.println();
       
     }
     // Halt PICC
     rfid.PICC_HaltA();
     // Stop encryption on PCD
     rfid.PCD_StopCrypto1();
    float value = analogRead(analogPin);
   // ubidots.add(VARIABLE_LABEL, value);
     // Insert your variable Labels and the value to be sent
    
    ubidots.add(VARIABLE_LABEL2, l3);
    ubidots.add(VARIABLE_LABEL3, l1);
    ubidots.add(VARIABLE_LABEL4, l2);
    
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
    
  }
  ubidots.loop();
     
    
}


String printHex(byte *buffer, byte bufferSize)
{  
   String DatoHexAux = "";
   for (byte i = 0; i < bufferSize; i++) 
   {
       if (buffer[i] < 0x10)
       {
        DatoHexAux = DatoHexAux + "0";
        DatoHexAux = DatoHexAux + String(buffer[i], HEX);  
       }
       else { DatoHexAux = DatoHexAux + String(buffer[i], HEX); }
   }
   
   for (int i = 0; i < DatoHexAux.length(); i++) {DatoHexAux[i] = toupper(DatoHexAux[i]);}
   return DatoHexAux;
}

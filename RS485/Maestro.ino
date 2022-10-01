 #include <LiquidCrystal_I2C.h>

/******** VARIAVEIS GLOBLAIS *********/
LiquidCrystal_I2C lcd(0x27,16,2);
const int TxRx =  2;  // HIGH:TX y LOW:RX
const int led=8;//Led que recebe o sinal do esclavo
const int motor=A0;
const float voltage=5/1023;
float corrente=0.0;
float voltageMotor=0; //Guarda corrente medida pelo sensor
float resistencia=1.4;
float bits=0,totalBits=0;

void setup() 
{ 
  Serial.begin(9600); 
  Serial.setTimeout(100);//Definimos um tempo de espera de 100ms
  pinMode(led, OUTPUT);
  pinMode(TxRx, OUTPUT);
  digitalWrite(TxRx, HIGH);//RS485 como transmissor
  digitalWrite(led, LOW);
  pinMode(motor,INPUT);
  lcd.init();
  lcd.backlight();
} 
 
void loop(){
  //corrente=CalcularCorrente(CorrenteMotor());//Calcula a corrente
  totalBits=0;
  for(int i=0;i<5000;i++){
    bits=analogRead(A0);
    totalBits+=bits;
  }
  totalBits/=5000;
  voltageMotor=totalBits*voltage;
  corrente=voltageMotor/(resistencia+3);
  
  /*Serial.print("TotalBits: ");
  Serial.println(totalBits);
  Serial.print("Voltage Motor: ");
  Serial.println(voltageMotor);
  Serial.print("Corriente: ");
  Serial.println(corrente);
  delay(5000);*/
  
  
 
  lcd.setCursor(0, 0);
  lcd.print("Amp: "); 
  lcd.print(corrente);
  lcd.print("A"); 
  lcd.setCursor(0,1);
  lcd.print("Error rel. 1.5% "); 
  
  Serial.print("I"); //inicio da trama
  Serial.print("e");//O sinal para ligar o led
  Serial.print(corrente);
  Serial.print("F"); //fim da trama
  Serial.flush();    //Esperamos ate que os datos sejan enviados 
  
  //----Lemos a resposta do Escravo-----
  delay(50);
  digitalWrite(TxRx, LOW); //RS485 como receptor
  delay(250);
  
  if(Serial.read()=='i')//Esperamos o inicio da trama
  {
      char dato=Serial.read();//Guardamos os datos que cheguem4
      Serial.print("Dato: ");
      Serial.println(dato);
     if(dato=='E')//Si o dato é a letra E entao se acende um led
       {
        delay(80);
        digitalWrite(led,HIGH);//Led ligado
       }
      if(Serial.read()=='f') //Si é o fim da trama entao é correto
       {
        delay(150);
        digitalWrite(led,LOW);//Led desligado
      }
  }
  digitalWrite(TxRx, HIGH); //RS485 como Transmissor
  delay(50);
  //----------fin de la respuesta-----------
  delay(200);
} 

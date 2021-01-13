#include <ESP8266.h>



#ifdef __AVR__
#include <avr/power.h>
#endif
#include <SoftwareSerial.h>

//ESP
#define sw_serial_rx_pin 2 //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin 3 //  Connect this pin to RX on the esp8266

#define FSRSenzorLijevi A0
#define FSRSenzorSrednji A1
#define FSRSenzorDesni A2

#define svijetloLijevo 4
#define svijetloSrednje 5
#define svijetloDesno 6

#define SSID         "tomek"
#define PASSWORD     "tomek123"
#define SERVER_IP    "192.168.43.209"
#define SERVER_PORT  (2001)

char buff[50]= {0};
int jacinaUdarenogSenzora=0;

SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);
ESP8266 wifi(swSerial);

void setup() {
  swSerial.begin(115200);
  Serial.begin(9600);

  pinMode(FSRSenzorLijevi,INPUT);
  pinMode(FSRSenzorSrednji,INPUT);
  pinMode(FSRSenzorDesni,INPUT);
  pinMode(svijetloLijevo,OUTPUT);
  pinMode(svijetloSrednje,OUTPUT);
  pinMode(svijetloDesno,OUTPUT);
  
  while(!wifi.joinAP(SSID, PASSWORD)) {
     Serial.print("Join AP failure\r\n");
    delay(5);
  }
     
      
  Serial.print("Join AP success\r\n");
  Serial.print("IP:");
  Serial.println(wifi.getLocalIP().c_str());

  if (wifi.disableMUX()) {
    Serial.print("single OK\r\n");
  } else {
    Serial.print("single ERROR\r\n");
  }
  
  while(!wifi.createTCP(SERVER_IP, SERVER_PORT)) {
   delay(5);
    Serial.print("create TCP ERROR\r\n");
  }
    
  Serial.print("create TCP OK\r\n");
  Serial.print("SETUP END\r\n");
  
  // put your setup code here, to run once:

}

void loop() {
  while(!wifi.recv(buff, sizeof(buff), 10000)){
  Serial.println("cekam poruku");
  delay(5);
  }
  ///parsiranje
  Serial.println(buff);
  String str="";
  for (int i=0;i<5;i++){
    str+=buff[i];
  }
  int vrijeme=str.toInt();
 
Serial.println("vrijeme: ");
Serial.println(vrijeme);

  str="";
  str+=buff[5];
  int svijetlo=str.toInt();
  Serial.println("svijetlo:");
  Serial.println(svijetlo);
  switch(svijetlo){
    case 0:
    digitalWrite(svijetloLijevo,HIGH);
    break;
    case 1:
    digitalWrite(svijetloSrednje,HIGH);
    break;
    case 2:
    digitalWrite(svijetloDesno,HIGH);
    break;
   }
   
   int vrijemeUdarca=ocitanjeSenzora(vrijeme,svijetlo);
   if(vrijemeUdarca){
      String str=makeMSGForServer(vrijemeUdarca);
      Serial.println("gasi svijetlo");
      Serial.println(svijetlo+4);
      str.toCharArray(buff,str.length());
    
      while (!wifi.send((const uint8_t*)buff,strlen(buff))){
        delay(5);
        Serial.println("nije poslalo poruku");
      }
   }else{
      jacinaUdarenogSenzora=0;
      String str=makeMSGForServer(vrijemeUdarca);
      str.toCharArray(buff,str.length());
      while (!wifi.send((const uint8_t*)buff,strlen(buff))){
        Serial.println("nije poslalo poruku");
        delay(5);
      }
    }
    digitalWrite(svijetlo+4,LOW);
  
    
}
int ocitanjeSenzora(int vrijeme,int senzor){
  int startnoVrijeme=millis();
  while((millis()-startnoVrijeme)<vrijeme){
    jacinaUdarenogSenzora=analogRead(senzor);
    if(jacinaUdarenogSenzora>1000){
      return millis()-startnoVrijeme;
      }
    }
    return 0;
  }

  String makeMSGForServer(int vrijeme){
   // String str="vrijeme";
   // String crtica=",";
   // str+=vrijeme;
//str+=crtica;
   // str+="jacina";
   // str+=crtica;
   // str+=jacinaUdarenogSenzora;
   // str+="\r\r";
   
   String str="";
   str+=vrijeme;
   str+="-";
   str+=jacinaUdarenogSenzora;
   str+="\r\r";
  
    return str;
    
    }




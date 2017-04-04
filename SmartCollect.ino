/*
  SmartCollect proyect

 This sketch connects to a some sensors and a GSM shield

 Part of GSM:
 This part connect with the server "smartcollect.duckdns.org" in port "3000"
 sending JSON informastion in order to update data from every trash bin in a specific city. 

 Part of sensors:
 This part connect Arduino board with sensors "Ultrasonic sensor, temperature and humidity sensor..."


 created 10 Mar 2017
 Part of GSM by David Lorente Moldes
 Part of sensors by Alfredo Antonio Cereijo Pardiñas
 Part of principal program by Jenifer Campero Couceiro
 */

// libraries
#include "LowPower.h"
#include <GSM.h>
#include <ArduinoJson.h>


#define EchoPin 5   // pin 5 recibe el eco de la señal enviada para calcular el tiempo entre ambas 
#define TriggerPin 6 // pin 6 para el trigger , tiene que estar activo durante 10us para activar el sensor
int distancias[24]; 
byte hora = 0; 
int cal_dist; 
String out;
const byte lim_dist= 20;
byte lleno = 0;
byte envio = 0;
int tiempo = 1; //Multiplo por el que se muliplican lo 8 segundos que duerme el arduino. Para una hora debe ser 450

void setup() {
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  Serial.begin (9600);
 }

void loop() {
  
  cal_dist = mejordistancia();
  Serial.print("Distancia: ");
  Serial.println(String(cal_dist));
  delay(1000);
  
  if(hora > 23){
    //Codigo para comprobar que no se ha llegado a mas de 23 horas
    desplazamiento();
    hora = 23;
  }
  
  if(lleno == 0){
    if(cal_dist <= lim_dist){
      Serial.println("Se ha llenado");
      delay(1000);
      lleno = cal_dist;
      //Esta lleno entonces se envia      
      envio = 1;
    }
  }else { //LLeno es distinto de cero entonces hay que comprobar si se vacia
    if(lleno < (cal_dist - 5)){ //Le restamos 5 porque puede haber poca fialibilidad
      // Se ha vaciado
      Serial.println("Se ha vaciado");
      delay(1000);
      //Se ha vaciado hay que enviar      
      lleno = 0;
      envio = 1;      
    }
  }
  distancias[hora] = cal_dist;

  if(envio == 1){
      //Aqui hay que enviar
      //createJson();  
      Serial.println("Hay que enviar"); 
      Serial.println(out); 
      delay(3000);
      //connect_GPRS();
      resetear();
      hora = 0;
      envio = 0;
  }else{
    Serial.println("No se envia");    
    hora++;    
  }
  dormir();
  delay(1000);
  imprimir_distancias();
}



int ultrasonidos() {
  //Function that interacts with the ultrasonic sensor  
  long duration, distanceCm; 
  digitalWrite(TriggerPin, LOW);  //se define el pulso del trigger para activar el sensor 4us bajo
  delayMicroseconds(4);           //para obtener un disparo limpio y 10 us alto que es lo que define el fabricante 
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  duration = pulseIn(EchoPin, HIGH);   // mide el tiempo entre pulsos en us
  distanceCm = duration / 29.2 / 2; //se pasa el tiempo a distancia en funcion de la Vsonido y /2 porque es ida y vuelta
  return distanceCm;     // se devuelve la distancia 
}


void dormir(){
  //Function that sends the arduino to sleep
  //Wait 16 seg
  Serial.println("Esta durmiendo");
  delay(1000);
  for(int i = 0; i < tiempo; i++){
     LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);     
  }
}


int mejordistancia(){
  // Function that calculates the best distance  
  int aux_dist,dist1,dist2,dist3;
  dist1 = ultrasonidos();
  delay(1000);
  dist2 = ultrasonidos();
  if(fabs(dist1 - dist2) < 10){
    aux_dist = (dist1 + dist2)/2;
  }else{
    dist3 = ultrasonidos();
    if(fabs(dist3 - dist1) < 10){
      aux_dist = (dist1+ dist3)/2;   
    }else{
      aux_dist = (dist3 + dist2)/2;
    }
  }
  return aux_dist;
}

/*
void createJson(){ 
  //Function that converts data to json  
  out = "";
  int aux = hora +1;
  int dist;
  Serial.println(aux);
  delay(1000);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& measures = root.createNestedArray("measures");
 
  for (int i = 0; i < aux ;i++){
    //Serial.println(distancias[i]);
    dist = distancias[i];
    Serial.println(dist);
    
     JsonObject& objeto= jsonBuffer.createObject();
     objeto["index"] = i;
     objeto["filling"]= dist;
     measures.add(objeto);   
     delay(500);
  }
  root.printTo(out);
  delay(7000);
  
}
*/


/*
void createJson(){ 
  out = "";
  //Function that converts data to json  
  Serial.println(hora);
  Serial.println(cal_dist);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& measures = root.createNestedArray("measures");
  JsonObject& NestedObject = measures.createNestedObject();
  NestedObject["index"] = String(hora);
  NestedObject["filling"]= String(cal_dist);
  root.printTo(out);
  delay(2000);
}
*/

void resetear(){
  //Function that resets variables 
  for (int i = 0; i < 24; i++){
    distancias[i] = 0;
  }
  imprimir_distancias();
}


void desplazamiento(){
  //Funcion que se usara cuando llegue una hora mayor de 23
  //Se desplazan las medidas hacia la izquierda
    int aux_cad[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for(int t = 0; t < 24; t++){
      aux_cad[t] = distancias[t];      
    }
    resetear();
    for(int i = 0; i < 23; i++){
        distancias[i] = aux_cad[i+1];
    }
}


void imprimir_distancias(){
      for(int j = 0; j < 24; j++){
          Serial.print(distancias[j]);
          delay(1000);
      }
      Serial.println();
}



/*
// Esto es lo que tiene loren en el setup();
void connect_GPRS(){

  // PIN Number Card
#define PINNUMBER "" //"1800"

// APN data
#define GPRS_APN     "freedompop.foggmobile.com" // "movistar.es" //"internet.mundo-r.com" // GPRS APN
#define GPRS_LOGIN    "" // "MOVISTAR"    //  GPRS login
#define GPRS_PASSWORD "" // "MOVISTAR" // GPRS password

// initialize the library instance
  GSMClient client;
  GPRS gprs;
  GSM gsmAccess;

// URL, path & port
  char server[] = "smartcollect.duckdns.org";
  char path[] = "/container/update/1";
  int port = 3000; // port 80 is the default for HTTP

   Serial.println("Starting Arduino web client.");
    // connection state
    boolean notConnected = true;
  
    // After starting the modem with GSM.begin()
    // attach the shield to the GPRS network with the APN, login and password
    while (notConnected) {
      if ((gsmAccess.begin(PINNUMBER) == GSM_READY) & (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
          notConnected = false;
          Serial.println("connecting...");
          
          // if you get a connection, report back via serial:
          if (client.connect(server, port)) { 
            // Making a POST request to our server
            //Set Headers
            client.print("POST /container/update/1 ");
            client.println("HTTP/1.1");
            client.println("Host:  smartcollect.duckdns.org");
            client.println("User-Agent: Arduino-SmartCollect/1.0");
            client.println("Connection: close");
            client.println("Content-Type: application/json");
            client.print("Content-Length: ");
            client.println(out.length());
            client.println();
            //Sending Json Data
            client.println(out);
            client.println();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
            delay(1000);
          }

          boolean test = true;
          while(test){
              if (client.available()) {
                    char c = client.read();
                    Serial.print(c);
              }
              // if the server's disconnected, stop the client:
              if (!client.available() && !client.connected()) {
                    Serial.println("disconnecting.");
                    client.stop();
                    test = false;
              }
          }
      } else {
        Serial.println("Not connected");
        delay(1000);
      }
    }
}*/

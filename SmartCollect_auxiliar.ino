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
#include <ArduinoJson.h>
#include "SIM900.h"

#define CONNECTIONS_ATTEMPS 5
#define ECHO_PIN 5   // pin 5 recibe el eco de la señal enviada para calcular el tiempo entre ambas 
#define TRIGGER_PIN 6 // pin 6 para el trigger , tiene que estar activo durante 10us para activar el sensor
#define MAX_DISTANCE 20 //Distancia a partir de la cual se envía

#define sec0 "{\"measures\": ["
#define sec1 ","
#define secend "]}"

int distances[24]; 
byte hora = 0; 
char* out;
byte lleno = 0;
boolean full = false;
boolean sent = false;
int tiempo = 1; //Multiplo por el que se muliplican lo 8 segundos que duerme el arduino. Para una hora debe ser 450


void setup() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin (19200);
 }

void loop() {
  int current_dist;
  
  current_dist = get_distance();
  Serial.print(F("Distancia: "));
  Serial.println(current_dist);
  delay(1000);
  
  if(hora > 23){
    //Codigo para comprobar que no se ha llegado a mas de 23 horas
    shift_distance_array();
    hora = 23;
  }
  
  distances[hora] = current_dist;

  if(full){
    if(lleno < (current_dist - 5)){ 
      // Se ha vaciado
      Serial.println(F("Se ha vaciado"));
      delay(1000);
      //Se ha vaciado hay que enviar      
      full = false;
      sent = true;      
    }
  }
  else { 
    if(current_dist <= MAX_DISTANCE){
      Serial.println(F("Se ha llenado"));
      delay(1000);
      lleno = current_dist;
      //Esta lleno entonces se envia      
      full = true;
      sent = true;
    }
  }

  if(sent){
    //Aqui hay que enviar
    out = malloc(41 * sizeof(char));
    createJSONManual(out, (hora+1), distances); 
    Serial.println(F("Hay que enviar")); 
    Serial.println(out); 
    delay(3000);
    //connect_GPRS();
    send_data();
    resetear();
    sent = false;
  }
  else{
    Serial.println(F("No se envia"));    
    hora++;    
  }
  dormir();
  delay(1000);
}



int ultrasonidos() {
  //Function that interacts with the ultrasonic sensor  
  long duration, distanceCm; 
  digitalWrite(TRIGGER_PIN, LOW);  //se define el pulso del trigger para activar el sensor 4us bajo
  delayMicroseconds(4);           //para obtener un disparo limpio y 10 us alto que es lo que define el fabricante 
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);   // mide el tiempo entre pulsos en us
  distanceCm = duration / 29.2 / 2; //se pasa el tiempo a distancia en funcion de la Vsonido y /2 porque es ida y vuelta
  return distanceCm;     // se devuelve la distancia 
}




void createJSONManual(char* json, int samples, int data[]){
  char number[4] = "";
   strcpy(json, sec0);
  for (int i = 0; i < samples ; i++) {
    sprintf(number,"%i", data[i]);
    strcat(json, number);
    if (i == samples - 1) {
      strcat(json,secend);
    } else {
      strcat(json,sec1);
    }
  }
}


void dormir(){
  //Function that sends the arduino to sleep
  //Wait 16 seg
  Serial.println(F("Esta durmiendo"));
  delay(1000);
  for(int i = 0; i < tiempo; i++){
     LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);     
  }
}


int get_distance(){
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


void resetear(){
  //Function that resets variables 
  for (int i = 0; i < 24; i++){
    distances[i] = 0;
  }
  hora = 0;
  //imprimir_distancias();
}


void shift_distance_array(){
  //Funcion que se usara cuando llegue una hora mayor de 23
  //Se desplazan las medidas hacia la izquierda
    int aux_cad[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for(int t = 0; t < 24; t++){
      aux_cad[t] = distances[t];      
    }
    for(int i = 0; i < 23; i++){
        distances[i] = aux_cad[i+1];
    }
}



void freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v;
  Serial.print(F("Memoria: ")); 
  Serial.println((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
}


void send_data(){

  SIM900 sim900 (7, 8);
  boolean success = false;
  int attemps = 0;

  sim900.init();
  Serial.println(F("[SIM900] started"));

  while(attemps < CONNECTIONS_ATTEMPS && !success){
    
    if(sim900.connect_network()){
      success = true;
      continue;
    }
    
    Serial.println(F("[SIM900] Error while connecting to network"));
    sim900.restart();
    attemps++;
    delay(5000);
  }

  if(!success)
    return;

  Serial.println(F("[SIM900] connected to network"));
  sim900.send_json(out);
  Serial.println(F("[SIM900] package sent"));
}




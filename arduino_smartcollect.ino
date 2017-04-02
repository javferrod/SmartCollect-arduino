#include "SIM900.h"
#define CONNECTIONS_ATTEMPS 5

void setup() {
  Serial.begin(19200);
  send_data();
}

void loop() {

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
  sim900.send_json("{\"measures\": [{\"index\":0, \"filling\":40}, {\"index\":1, \"filling\":100}]}");
  Serial.println(F("[SIM900] package sent"));
}


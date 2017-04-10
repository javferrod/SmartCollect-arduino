const int pingPin = 6; 
int inPin = 5;    // pin 5 recibe el eco de la señal enviada para calcular el tiempo entre ambas 
 // pin 6 para el trigger , tiene que estar activo durante 10us para activar el sensor

void setup() {  // definicion de los modos de los pines 
  Serial.begin(9600);
}

void loop() {  // resultado mostrado en el monitor serie  como Distancia : -- X --  cm cada segundo 
 long duration, cm;
  pinMode(pingPin, OUTPUT);    
  
  digitalWrite(pingPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
pinMode(inPin, INPUT);
duration = pulseIn(inPin, HIGH);
cm = microsecondstoCentimeters(duration);
 Serial.print(cm);
 Serial.println();
 delay(1000);
}

long microsecondstoCentimeters(long microseconds)
{
  return microseconds/29/2;     // se devuelve la distancia 
  }

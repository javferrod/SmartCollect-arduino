#define sec0 "{\"measures\": ["
#define sec1 ","
#define secend "]}"

char distancias[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};

void setup() {
 Serial.begin(9600);
}
 void createJson(char* json, int samples, char data[]){
  char number[] = "";
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
        strcat(json,"\0");
}
void loop() {
char* json = malloc(41 * sizeof(char));
createJson(json, 24, distancias);
Serial.println(json);
for(;;);


}

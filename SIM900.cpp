#include "SIM900.h"

SIM900::SIM900(uint8_t rx_pin, uint8_t tx_pin)
: _serial(rx_pin, tx_pin) {
    _serial.begin(19200);
    pinMode(POWER_PIN, OUTPUT);
}

void SIM900::init() {
    send_power_on();
    while (! check_if_powered());
}

void SIM900::restart() {
    send_power_off();
    delay(2000);
    send_power_on();
    while (! check_if_powered());
}

// NETWORK OPERATION

boolean SIM900::connect_network(){
  while(! check_if_attached());
  
  send_at(F(ATTACH_GPRS), OK, 1000);
  send_at(F(SET_APN), OK, 3000); 
  send_at(F(INIT_GPRS), OK, 3000);
  send_at(F(GET_IP), IP, 3000, true);
  return send_at(F(CHECK_IP_STATUS), CONNECTED, 3000);
}

boolean SIM900::send_json(char* json){
  int total_length = 0;
  int json_length = strlen(json);
  
  char CIPSEND_COMMAND[20];  

  if(json_length >= 100)
    total_length += 1;

  total_length += 119 + json_length;
  
  sprintf(CIPSEND_COMMAND, "AT+CIPSEND=%d", total_length);

  send_at(F(INIT_TCP_CONN), CONN_OK, 2000, true);
  send_at(CIPSEND_COMMAND, WAITING_TO_POST, 2000, true);
  
  _serial.print(F(POST_HEADERS));
  _serial.print(json_length);
  _serial.print(F("\r\n\r\n"));
  _serial.println(json);
  
  send_at(F(CLOSE_TCP_CONN),OK, 2000);
  
}


boolean SIM900::send_at(const __FlashStringHelper* command, const char* expected_response, unsigned int time_threshold, boolean print_response = false){
    char response[100];
    int x = 0;
    unsigned long initial_timestamp;
    boolean already_received = false;
    
    memset(response, '\0', 100);
    
    clear_serial_buffer();
    _serial.println(command);
    
    initial_timestamp = millis();
    
    do {
      if (_serial.available() == 0)
            continue;
            
        response[x] = _serial.read();
        x++;
        if (strstr_P(response, expected_response) != NULL)
          already_received = true;
    }
    while ((millis() - initial_timestamp) < time_threshold);
    
    if(print_response){
        Serial.println(F("------"));
        Serial.println(response);
        Serial.println(F("------"));
    }
    
    return already_received;
}

boolean SIM900::send_at(char* command, const char* expected_response, unsigned int time_threshold, boolean print_response = false){
    char response[100];
    int x = 0;
    unsigned long initial_timestamp;
    boolean already_received = false;
    
    memset(response, '\0', 100);
    
    clear_serial_buffer();
    _serial.println(command);
    
    initial_timestamp = millis();
    
    do {
      if (_serial.available() == 0)
            continue;
            
        response[x] = _serial.read();
        x++;
        if (strstr_P(response, expected_response) != NULL)
          already_received = true;
    }
    while ((millis() - initial_timestamp) < time_threshold);
    
    if(print_response){
        Serial.println(F("------"));
        Serial.println(response);
        Serial.println(F("------"));
    }
    
    return already_received;
}

void SIM900::clear_serial_buffer(){
    while ( _serial.available() > 0) _serial.read();
}

// POWER MANAGMENT

void SIM900::send_power_on() {
    digitalWrite(POWER_PIN, HIGH);
    delay(1000);
    digitalWrite(POWER_PIN, LOW);
    delay(1000);
}

void SIM900::send_power_off() {
    digitalWrite(POWER_PIN, HIGH);
    delay(1000);
    digitalWrite(POWER_PIN, LOW);
    delay(1000);
}

// STATUS CHECKS

boolean SIM900::check_if_powered(){
  return send_at(F(AT), OK, 2000);
}

boolean SIM900::check_if_attached(){
  return send_at(F(CHECK_RADIO_STATUS), ATTACHED, 2000);
}

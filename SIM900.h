/*
 * SIM900.h - Librería para gestionar una shield con SIM900
 * Creado por Javier Fernández Rodríguez, 31 de Marzo, 2017
 */

#ifndef SIM900_H
#define SIM900_H

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

const int POWER_PIN PROGMEM = 9;

const char OK[] PROGMEM = "OK";
const char ATTACHED[] PROGMEM = "+CREG: 0,5";
const char IP[] PROGMEM = "";
const char CONNECTED[] PROGMEM = "STATE: IP STATUS";
const char CONN_OK[] PROGMEM = "CONNECT OK";

const char WAITING_TO_POST[] PROGMEM = ">";

#define AT "AT"

#define CHECK_RADIO_STATUS "AT+CREG?"

#define ATTACH_GPRS "AT+CGATT=1\r"
#define SET_APN "AT+CSTT=\"freedompop.foggmobile.com\",\"\",\"\""
#define INIT_GPRS "AT+CIICR"
#define GET_IP "AT+CIFSR"
#define CHECK_IP_STATUS "AT+CIPSTATUS"
#define INIT_TCP_CONN "AT+CIPSTART=\"TCP\",\"smartcollect.duckdns.org\",\"80\""
#define POST_HEADERS "POST /container/update/1 HTTP/1.1\r\nHost: smartcollect.duckdns.org\r\nContent-Type:application/json\r\nContent-Length:"
#define CLOSE_TCP_CONN "AT + CIPCLOSE"
class SIM900 {
public:
    SIM900(uint8_t rx_pin, uint8_t tx_pin);
    void init();
    void restart();
    boolean connect_network();
    boolean send_json(char* json);


private:
    SoftwareSerial _serial;
    void send_power_on();
    void send_power_off();
    boolean check_if_powered();
    boolean check_if_attached();
    void clear_serial_buffer();
    boolean send_at(const __FlashStringHelper* command, const char* expected_response, unsigned int time_threshold, boolean print_response = false);
    boolean send_at(char* command, const char* expected_response, unsigned int time_threshold, boolean print_response = false);
};

#endif

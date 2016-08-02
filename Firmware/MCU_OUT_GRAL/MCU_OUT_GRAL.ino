/*
 *      Interfaz Midi Argentina 4
 * www.github.com/luxarts/IMA-4
 * Versión: 1.0
 *
 * Creado por LuxARTS, OG-Jonii & pablonobile99
 *               2016
 */
#define DEBUG
#define DEBUG_TIME

#define MCU_IN1 1
#define MCU_IN2 2
#define MCU_CC1 3
#define MCU_CC2 4
#define MCU_OUT 5
#define MCU_GRAL 6

#include <Wire.h> //Librería I2C

#ifdef DEBUG_TIME
long tiempo,tiempoMax,tiempoAct;
#endif

byte recibido;

void setup() {
  Wire.begin(MCU_GRAL);
  Serial.begin(115200);
}

void loop() {
#ifdef DEBUG_TIME
  tiempo=micros();
#endif

  while(Serial.available()>0){
    Wire.beginTransmission(MCU_LEDS);
    for(byte i=0;i<3;i++){
      recibido=Serial.read();
      Wire.write(recibido);
      delayMicroseconds(80);// DELAY OBLIGATORIO PARA ESTABILIZAR
    }
    Wire.endTransmission();
  }

#ifdef DEBUG_TIME
  tiempoAct=micros()-tiempo;
  if(tiempoAct > tiempoMax){
    tiempoMax=tiempoAct;
    Serial.println("--------------");
    Serial.println(tiempoAct);  
  }
#endif 
}

/*
 *      Interfaz Midi Argentina 4
 * www.github.com/luxarts/IMA-4
 * Versión: 1.8
 *
 * Creado por LuxARTS, OG-Jonii & pablonobile99
 *               2016
 */

#define MCU_IN1  0x10
#define MCU_IN2  0x11
#define MCU_CC1  0x12
#define MCU_CC2  0x13
#define MCU_OUT  0x14
#define MCU_GRAL 0x15
#define MCU_TFT  0x16
#include <Wire.h>

#ifdef DEBUG_TIME
long tiempo,tiempoMax,tiempoAct;
#endif

unsigned int estado_anterior = 0b1111111111111111;//matriz 4x4 c/ pullup

void setup() {
  for(byte i=0;i<16;i++)pinMode(i+2,INPUT_PULLUP);
  Wire.begin(MCU_IN1);
  Wire.setClock(400000L);
  digitalWrite(SDA,0);
  digitalWrite(SCL,0); 
  Wire.onRequest(requestEvent); //Interrupción al recibir un dato
}

void loop() {
#ifdef DEBUG_TIME
  tiempo=micros();
#endif

  //Guardado estado botones
  byte estado;
  
  for(byte i=0;i<16;i++){ //16 botones de matriz
      estado = digitalRead(i+2); //Guarda el estado del botón
      delay(1);
      if(estado!=digitalRead(i+2))estado=digitalRead(i+2); //Anti-rebote
    
    if(estado==0 && BitRead(i,estado_anterior)==1){
      estado_anterior=BitWrite(i,0,estado_anterior);        //  EJ: i=0 -> estado_anterior=1111 1111 1111 1111 &~(1<<15)
                                                            //                             1111 1111 1111 1111 & 0111 1111 1111 1111
                                                            //             estado_anterior=0111 1111 1111 1111   
    }                                                       //  EJ: i=9 -> estado_anterior=0111 1111 1111 1111 &~(1<<6)
    if(estado==1 && BitRead(i,estado_anterior)==0){         //                             0111 1111 1111 1111 & 1111 1111 1011 1111
      estado_anterior=BitWrite(i,1,estado_anterior);        //             estado_anterior=0111 1111 1011 1111
    } 
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
void requestEvent() { //Función que se ejecuta en la interrupción
  byte vector[2];
  vector[0] = (estado_anterior>>8) & 0xFF;
  vector[1] = estado_anterior & 0xFF;
  Wire.write(vector,2);
}
byte BitRead(byte pos,unsigned int param){//Devuelve un bit de un byte
  return((param>>pos) & 1); //0x8000 = 0b1000 0000 0000 0000, corre 15 para devolver el bit 0  
}
unsigned int BitWrite(byte pos, byte estado,unsigned int param){ //Escribe un bit en una posicion                           
  if(estado==1)param = param | (1<<pos); //0000 0000 -> 0000 0010: 0000 0000 |  1<<2                          //SET
  if(estado==0)param = param & ~(1<<pos);//1111 1111 -> 1111 1101: 1111 1111 & ~1<<2 -> 1111 1111 & 1111 1101 //CLEAR
  return param;
}

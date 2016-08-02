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
#include <Wire.h>

byte BitRead(unsigned int, byte);
void BitWrite(byte, byte);
void pinMode_init(void);
void notaOn(byte);
void notaOff(byte);

#ifdef DEBUG_TIME
long tiempo,tiempoMax,tiempoAct;
#endif

int estado_anterior = 0b1111111111111111;//matriz 4x4 c/ pullup


void setup() {
  // put your setup code here, to run once:
  Wire.begin(MCU_IN2);
  pinMode_init(); //PIN 13 NO TIENE PULLUP INTERNO, OJO
  Wire.onRequest(requestEvent); //Interrupción al recibir un dato
  //Serial.begin(9600);
}

void loop() {
#ifdef DEBUG_TIME
  tiempo=micros();
#endif

  //Guardado estado botones
  byte estado;
  
  for(byte i=0;i<16;i++){ //16 botones de matriz
    if(i==14)estado=digitalRead(A0);
    if(i==15)estado=digitalRead(A1);
    else{
      estado = digitalRead(i); //Guarda el estado del botón
      delay(1);
      if(estado!=digitalRead(i))estado=digitalRead(i); //Anti-rebote
    }
    
    if(estado==0 && BitRead(i)==1){
      BitWrite(i,0);                        //  EJ: i=0 -> estado_anterior=1111 1111 1111 1111 &~(1<<15)
                                            //                             1111 1111 1111 1111 & 0111 1111 1111 1111
                                            //             estado_anterior=0111 1111 1111 1111   
    }                                       //  EJ: i=9 -> estado_anterior=0111 1111 1111 1111 &~(1<<6)
    if(estado==1 && BitRead(i)==0){         //                             0111 1111 1111 1111 & 1111 1111 1011 1111
      BitWrite(i,1);                        //             estado_anterior=0111 1111 1011 1111
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
  unsigned char byteMin, byteMax;
  unsigned char vector[2]={1,1};
  byteMin = estado_anterior & 0x00FF;
  byteMax = (estado_anterior & 0xFF00)>>8;
  vector[0]=byteMin;
  vector[1]=byteMax;
  Wire.write(vector,2);
  delayMicroseconds(80);
}

byte BitRead(byte pos){//Devuelve un bit de un int
  unsigned int n;
  n=estado_anterior<<pos;
  return((n&0x8000)>>15); //0x8000 = 0b1000 0000 0000 0000, corre 15 para devolver el bit 0  
}

void BitWrite(byte pos, byte estado){ //Escribe un bit en una posicion                           
  if(estado==1)estado_anterior = estado_anterior | (1<<15-pos); //0000 0000 -> 0000 0010: 0000 0000 |  1<<2                          //SET
  if(estado==0)estado_anterior = estado_anterior & ~(1<<15-pos);//1111 1111 -> 1111 1101: 1111 1111 & ~1<<2 -> 1111 1111 & 1111 1101 //CLEAR
}

void pinMode_init(void){
  for(byte i=0;i<13;i++){
    pinMode(i,INPUT_PULLUP);
  }
  pinMode(13,INPUT_PULLUP);//Tiene un led, NO TIENE PULLUP INTERNO
  pinMode(A0,INPUT_PULLUP);
  pinMode(A1,INPUT_PULLUP);
  pinMode(A2,INPUT_PULLUP);
  pinMode(A3,INPUT_PULLUP);
}

/*
 *      Interfaz Midi Argentina 4
 * www.github.com/luxarts/IMA-4
 * Versión: 1.8
 *
 * Creado por LuxARTS, OG-Jonii & pablonobile99
 *               2016
 */
//#define DEBUG
//#define DEBUG_TIME
#include <Wire.h> //Librería I2C

#define MCU_IN1  0x10
#define MCU_IN2  0x11
#define MCU_CC1  0x12
#define MCU_CC2  0x13
#define MCU_OUT  0x14
#define MCU_GRAL 0x15
#define MCU_TFT  0x16

#define OUTLED1       120
#define OUTLED2       121
#define OUTLED3       122
#define OUTLED4       123
#define OUTLED5       124
#define CHANGECHANNEL 0
#define LEDONOFF      1

#define FULLCOLOR
#define STCP  11 //SR Pin 5: Store Clock Pulse
#define SHCP  13 //SR Pin 2: !Shift Clock Pulse 
#define SI    12 //SR Pin 4: Serial Input
//M_SESSION
#define MXY00 0x00
#define MXY01 0x01
#define MXY02 0x02
#define MXY03 0x03
#define MXY10 0x10
#define MXY11 0x11
#define MXY12 0x12
#define MXY13 0x13
#define MXY20 0x20
#define MXY21 0x21
#define MXY22 0x22
#define MXY23 0x23
#define MXY30 0x30
#define MXY31 0x31
#define MXY32 0x32
#define MXY33 0x33
//M_DRUMRACK
#define MXY00_DR 0
#define MXY01_DR 1
#define MXY02_DR 2
#define MXY03_DR 3
#define MXY10_DR 4
#define MXY11_DR 5
#define MXY12_DR 6
#define MXY13_DR 7
#define MXY20_DR 8
#define MXY21_DR 9
#define MXY22_DR 10
#define MXY23_DR 11
#define MXY30_DR 12
#define MXY31_DR 13
#define MXY32_DR 14
#define MXY33_DR 15
//GENERALES
#define M_SESSION  0
#define M_MIXER    1
#define M_PIANO    2
#define M_DRUMRACK 3
#define M_EDITOR   4
#define M_EXTRA    5
byte CHANNEL=0;
byte PADCHANNEL=1;


//Prototypes

void cargarMatriz(byte,byte,byte);
void Shift_init(void);

//Global variables

#ifdef DEBUG_TIME
long tiempo,tiempoMax,tiempoAct;
#endif

//Fil3-Fil0 / R3-R0 / G3-G0 / B3-B0
//D0     D3 / D4 D7 / D8 D11/ D12 D15
unsigned int crgb = 0b1111111111111111;//16 bits = Shift register
byte modo;
byte matriz[4][4]={
  {0,0,1,0},
  {0,5,2,0},
  {1,2,3,4},
  {0,0,4,0}
};

//Functions
void setup(){
  Wire.begin(MCU_OUT);
  Wire.setClock(400000L);
  digitalWrite(SDA,0);
  digitalWrite(SCL,0);
  Shift_init(); //Inicializa el shift register 74F675APC
  Wire.onReceive(receiveEvent); //Interrupción al recibir un dato
  pinMode(1,OUTPUT);
  digitalWrite(1,0);
  pinMode(2,OUTPUT);
  digitalWrite(2,0);
  pinMode(3,OUTPUT);
  digitalWrite(3,0);
  pinMode(4,OUTPUT);
  digitalWrite(4,0);
}

void loop(){
#ifdef DEBUG_TIME
  tiempo=micros();
#endif

  imprimirMatriz();

#ifdef DEBUG_TIME
  tiempoAct=micros()-tiempo;
  if(tiempoAct > tiempoMax){
    tiempoMax=tiempoAct;
    Serial.println("--------------");
    Serial.println(tiempoAct);  
  }
#endif
}

void Shift_init(void){
  pinMode(STCP,OUTPUT);
  pinMode(SHCP,OUTPUT);
  pinMode(SI,OUTPUT);
  digitalWrite(SI,0);
  digitalWrite(STCP,0);
  digitalWrite(SHCP,1);

  for(int i=0;i<16;i++)//Limpia los registros
  {
    //Mantiene el registro
    digitalWrite(SI,0);//Envia el dato
    delayMicroseconds(1);
    digitalWrite(SHCP,0);//Corre el registro (bajada)
    delayMicroseconds(1);
    digitalWrite(SHCP,1);//Corre el registro (subida)
    delayMicroseconds(1);
  }
  digitalWrite(STCP,1);//Carga el puerto
}

void cargarMatriz(byte filas,byte columnas,byte color){
  //Color: (0)=OFF=0 , (1;40)=R=1 , (41;80)=G=2 , (81;126)=B=3 , (127)=W
  //Rojo - Amarillo - Verde - Celeste -  Azul - Violeta - Blanco
#ifdef FULLCOLOR
  if(color>0 && color<=20 )color=1;  //Rojo
  if(color>21 && color<=40)color=2; //Amarillo
  if(color>40 && color<=63)color=3; //Verde
  if(color>63 && color<=80)color=4; //Celeste
  if(color>80 && color<=105)color=5;//Azul
  if(color>105 && color<127)color=6;//Violeta
  if(color==127)color=1;            //Rojo
  //if(color==127)color=7;            //Blanco
#else
  if(color>0 && color<=40)color=1;   //Rojo
  if(color>40 && color<=80)color=3;  //Verde
  if(color>80 && color<=127)color=5; //Azul
#endif
  matriz[filas][columnas]=color;//Carga el espacio con el color
}

void imprimirMatriz(void){
  byte dato;

  for(byte fil=0;fil<4;fil++){
    crgb=0xFFFF;  //Limpia todo
    for(byte col=0;col<4;col++){
      dato=matriz[fil][col];
      switch(dato){
        case 1: crgb=crgb&(~(0b0000000100000000<<col)); //0000 0001 << 2 -> 0000 0100 = ~1111 1011 & 1111 1111 -> crgb = 1111 1011 
        break;//Rojo                                    //0000 0000 << 1 -> 0000 0010 = ~1111 1101 & 1111 1011 -> crgb = 1111 1001
        case 3: crgb=crgb&(~(0b0000000000010000<<col)); 
        break;//Verde
        case 5: crgb=crgb&(~(0b0000000000000001<<col)); 
        break;//Azul
#ifdef FULLCOLOR
        case 2: crgb=crgb&(~(0b0000000100010000<<col)); 
        break;//Amarillo
        case 4: crgb=crgb&(~(0b0000000000010001<<col)); 
        break;//Celeste
        case 6: crgb=crgb&(~(0b0000000100000001<<col)); 
        break;//Violeta
        case 7: crgb=crgb&(~(0b0000000100010001<<col)); 
        break;//Blanco
#endif
      }  
    }
    switch(fil){
      case 0: crgb=0b1110000000000000|(crgb&0x0FFF);
      break;
      case 1: crgb=0b1101000000000000|(crgb&0x0FFF);
      break;
      case 2: crgb=0b1011000000000000|(crgb&0x0FFF);
      break;
      case 3: crgb=0b0111000000000000|(crgb&0x0FFF);
      break;
    }
    cargarShift();
    delay(3);
  }
}

void cargarShift(void){
  byte dato;
  digitalWrite(STCP,0);//Mantiene el registro
  for(byte i=0;i<16;i++){
    dato=(crgb>>i)&1;
    digitalWrite(SI,dato);//Escribe el bit
    delayMicroseconds(1);
    digitalWrite(SHCP,0);//Manda el pulso de shift
    delayMicroseconds(1);
    digitalWrite(SHCP,1);
    delayMicroseconds(1);
  }
  digitalWrite(STCP,1);//Saca el registro al puerto
}

/*******************************************************
*
********************************************************/
void receiveEvent(int how_many){
  while(Wire.available()){ //Si recibe un dato
    modo=Wire.read();
    byte estado=Wire.read();
    byte nota=Wire.read();
    byte velocity=Wire.read();
    if(modo==M_EXTRA){
      if(estado==CHANGECHANNEL){
        CHANNEL=nota;
        PADCHANNEL=nota+1;
      }
      if(estado==LEDONOFF){
        switch(nota){
          case OUTLED1: if(velocity)digitalWrite(1,1);
                        else digitalWrite(1,0);
          break;
          case OUTLED2: if(velocity)digitalWrite(2,1);
                        else digitalWrite(2,0);
          break;
          case OUTLED3: if(velocity)digitalWrite(3,1);
                        else digitalWrite(3,0);
          break;
          case OUTLED4: if(velocity)digitalWrite(4,1);
                        else digitalWrite(4,0);
          break;
          case OUTLED5: if(velocity)digitalWrite(5,1);
                        else digitalWrite(5,0);
          break;
        }
      }
    }
    if(modo==M_SESSION){       
      if(estado==(0x90+CHANNEL) || estado==(0x80+CHANNEL)){//Si es Nota ON prende un led según la velocidad y solo deja pasar el canal configurado
        if(estado==(0x80+CHANNEL))velocity=0; //Si es Nota OFF apaga el led
          
        switch(nota){
          case MXY00: cargarMatriz(0,0,velocity);//Fila 0
          break;
          case MXY01: cargarMatriz(0,1,velocity);
          break;
          case MXY02: cargarMatriz(0,2,velocity);
          break;
          case MXY03: cargarMatriz(0,3,velocity);
          break;
          case MXY10: cargarMatriz(1,0,velocity);//Fila 1
          break;
          case MXY11: cargarMatriz(1,1,velocity);
          break;
          case MXY12: cargarMatriz(1,2,velocity);
          break;
          case MXY13: cargarMatriz(1,3,velocity);
          break;
          case MXY20: cargarMatriz(2,0,velocity);//Fila 2
          break;
          case MXY21: cargarMatriz(2,1,velocity);
          break;
          case MXY22: cargarMatriz(2,2,velocity);
          break;
          case MXY23: cargarMatriz(2,3,velocity);
          break;
          case MXY30: cargarMatriz(3,0,velocity);//Fila 3
          break;
          case MXY31: cargarMatriz(3,1,velocity);
          break;
          case MXY32: cargarMatriz(3,2,velocity);
          break;
          case MXY33: cargarMatriz(3,3,velocity);
          break;
        }
      }
    }
    if(modo==M_DRUMRACK){      
      if(estado==(0x90+PADCHANNEL) || estado==(0x80+PADCHANNEL)){//Si es Nota ON prende un led según la velocidad y solo deja pasar el canal configurado
        if(estado==(0x80+PADCHANNEL))velocity=0; //Si es Nota OFF apaga el led
        
        switch(nota){
          case MXY00_DR: cargarMatriz(0,0,velocity);//Fila 0
          break;
          case MXY01_DR: cargarMatriz(0,1,velocity);
          break;
          case MXY02_DR: cargarMatriz(0,2,velocity);
          break;
          case MXY03_DR: cargarMatriz(0,3,velocity);
          break;
          case MXY10_DR: cargarMatriz(1,0,velocity);//Fila 1
          break;
          case MXY11_DR: cargarMatriz(1,1,velocity);
          break;
          case MXY12_DR: cargarMatriz(1,2,velocity);
          break;
          case MXY13_DR: cargarMatriz(1,3,velocity);
          break;
          case MXY20_DR: cargarMatriz(2,0,velocity);//Fila 2
          break;
          case MXY21_DR: cargarMatriz(2,1,velocity);
          break;
          case MXY22_DR: cargarMatriz(2,2,velocity);
          break;
          case MXY23_DR: cargarMatriz(2,3,velocity);
          break;
          case MXY30_DR: cargarMatriz(3,0,velocity);//Fila 3
          break;
          case MXY31_DR: cargarMatriz(3,1,velocity);
          break;
          case MXY32_DR: cargarMatriz(3,2,velocity);
          break;
          case MXY33_DR: cargarMatriz(3,3,velocity);
          break;
        }
      }
    }
  }
}


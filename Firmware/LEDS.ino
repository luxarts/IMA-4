/*
 *      Interfaz MIDI Argentina 4
 * www.github.com/luxarts/IMA-4
 * Module: Output
 * Version: 1.0
 *
 * Created by LuxARTS, OG-Jonii & pablonobile99
 *               2016
 */
//Libraries
#define MCU_BTN1 1
#define MCU_BTN2 2
#define MCU_POT1 3
#define MCU_POT2 4
#define MCU_LEDS 5
#define MCU_GRAL 6
#define STCP  5 //SR Pin 5: Store Clock Pulse
#define SHCP  2 //SR Pin 2: !Shift Clock Pulse 
#define SI    4 //SR Pin 4: Serial Input
#define MXY00 0
#define MXY01 1
#define MXY02 2
#define MXY03 3
#define MXY10 16
#define MXY11 17
#define MXY12 18
#define MXY13 19
#define MXY20 32
#define MXY21 33
#define MXY22 34
#define MXY23 35
#define MXY30 48
#define MXY31 49
#define MXY32 50
#define MXY33 51
#define CHANNEL 0

#include <Wire.h> //Librería I2C

//Prototypes

void cargarMatriz(byte,byte,byte);
void Shift_init(void);

//Global variables
//Fil3-Fil0 / R3-R0 / G3-G0 / B3-B0
//D0     D3 / D4 D7 / D8 D11/ D12 D15
unsigned int crgb = 0b1111111111111111;//16 bits = Shift register
unsigned int crgb1 = 0b1111111111111111;
byte matriz[4][4]={
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
};

//Functions
void setup(){
  Wire.begin(MCU_LEDS);
  Shift_init(); //Inicializa el shift register 74F675APC
  Wire.onReceive(receiveEvent); //Interrupción al recibir un dato
  pinMode(9,OUTPUT);
}

void loop(){
  imprimirMatriz();
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
  //Color: (0)=OFF=0 , (1;40)=R=1 , (41;80)=G=2 , (81;127)=B=3
  if(color>0 && color<=40)color=1;  //Rojo
  if(color>40 && color<=80)color=2; //Verde
  if(color>80 && color<=127)color=3;//Azul
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
        break;                                          //0000 0000 << 1 -> 0000 0010 = ~1111 1101 & 1111 1011 -> crgb = 1111 1001
        case 2: crgb=crgb&(~(0b0000000000010000<<col)); 
        break;
        case 3: crgb=crgb&(~(0b0000000000000001<<col)); 
        break;
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
    delay(4);
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

void receiveEvent(int how_many){
  while(Wire.available()){ //Si recibe UN dato
      
    byte estado=Wire.read(); //Nota ON 0x90+CH - Nota OFF 0x80+CH
    byte nota=Wire.read(); //Nota
    byte velocity=Wire.read(); //Color: (0)=0 , (1;40)=R , (41;80)=G , (81;127)=B
    
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
}


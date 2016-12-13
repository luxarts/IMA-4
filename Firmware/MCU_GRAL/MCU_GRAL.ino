/*
 *      Interfaz Midi Argentina 4
 * www.github.com/luxarts/IMA-4
 * Versión: 1.8
 *
 * Creado por LuxARTS, OG-Jonii & pablonobile99
 *               2016
 */
#include <Wire.h>

//#define DEBUG
//#define DEBUG_TIME

#define MCU_IN1  0x10
#define MCU_IN2  0x11
#define MCU_CC1  0x12
#define MCU_CC2  0x13
#define MCU_OUT  0x14
#define MCU_GRAL 0x15
#define MCU_TFT  0x16

#define LED1  2
#define LED2  3
#define LED3  4
#define LED4  5

#define M_SESSION  0
#define M_MIXER    1
#define M_PIANO    2
#define M_DRUMRACK 3
#define M_EDITOR   4
#define M_EXTRA    5

//Botones
#define PLAY               97
#define STOP               98
#define REC                99
#define TRACKMUTE_0        100
#define TRACKMUTE_1        101
#define TRACKMUTE_2        102
#define TRACKMUTE_3        103
#define TRACKSOLO_0        104
#define TRACKSOLO_1        105
#define TRACKSOLO_2        106
#define TRACKSOLO_3        107
#define TRACKREC_0         108
#define TRACKREC_1         109
#define TRACKREC_2         110
#define TRACKREC_3         111
#define SESSIONUP          112
#define SESSIONDOWN        113
#define SESSIONLEFT        114
#define SESSIONRIGHT       115
#define ZOOMUP             116
#define ZOOMDOWN           117
#define ZOOMLEFT           118
#define ZOOMRIGHT          119
#define SCENE_0            120
#define SCENE_1            121
#define SCENE_2            122
#define SCENE_3            123
#define TRACKSTOP_0        124
#define TRACKSTOP_1        125
#define TRACKSTOP_2        126
#define TRACKSTOP_3        127

//CC
#define TRACKVOL_0      0
#define TRACKVOL_1      1
#define TRACKVOL_2      2
#define TRACKVOL_3      3
#define PARAMCONTROL_0  4
#define PARAMCONTROL_1  5
#define PARAMCONTROL_2  6
#define PARAMCONTROL_3  7
#define PARAMCONTROL_4  8
#define PARAMCONTROL_5  9
#define PARAMCONTROL_6  10
#define PARAMCONTROL_7  11
#define MASTERVOLUME    12
//MCU_OUT
#define OUTLED1       120
#define OUTLED2       121
#define OUTLED3       122
#define OUTLED4       123
#define OUTLED5       124
#define CHANGECHANNEL 0
#define LEDONOFF      1
#define CHANNEL       0
#define PADCHANNEL    1
#define PIANOCHANNEL  2

#ifdef DEBUG_TIME
long tiempo,tiempoMax,tiempoAct;
#endif

/**********************************************************************************
** VARIABLES GLOBALES
**********************************************************************************/
byte MXY[5][16]= {                    
  {0x00,0x01,0x02,0x03,//session
   0x10,0x11,0x12,0x13,
   0x20,0x21,0x22,0x23,
   0x30,0x31,0x32,0x33},
   
  {1,1,1,1,//mixer
   1,1,1,1,
   1,1,1,1,
   1,1,1,1},
   
  {0,0,0,0,//piano
   0,0,0,0,
   0,0,0,0,
   0,0,0,0},
   
  {0,1,2,3,//drumrack
   4,5,6,7,
   8,9,10,11,
   12,13,14,15},

  {1,1,1,1,//editor
   1,1,1,1,
   1,1,1,1,
   1,1,1,1}
};
unsigned int escala_intervalo[25]={//Bit 11(Do) al 0(Si) 
  0b0000110011011001,//Arabe
  0b0000101101011001,//Armonica
  0b0000101010101010,//Aumentada
  0b0000101011010111,//Bebop dom.
  0b0000101011011101,//Bebop may.
  0b0000100010111010,//Blues
  0b0000111111111111,//Cromatica
  0b0000101101101101,//Disminuida
  0b0000101101010110,//Dorico
  0b0000110010101011,//Enigmatica
  0b0000101011010101,//Mayor
  0b0000101101011010,//Menor
  0b0000110101011010,//Frigio
  0b0000110001010010,//Japonesa
  0b0000101010110101,//Lidio
  0b0000110101101010,//Locrio
  0b0000101100111001,//Magiar
  0b0000101101010101,//Melodica
  0b0000101011010110,//Mixolidio
  0b0000110101010101,//Napolitana
  0b0000110101011001,//Napo menor
  0b0000101010010100,//Pentafonica
  0b0000101010010010,//Penta mayor
  0b0000100101010010,//Penta menor
  0b0000101010101010,//Whole
};
unsigned int e_nuevo[2]={0xFFFF,0xFFFF};
unsigned int e_anterior[2];
byte shift_btn=1;
byte modo=M_SESSION;
byte escalaActual=0;
byte tonicaActual=0;
char octavaActual=1;
byte sceneOffset=0;
byte trackOffset=0;
byte notasCC=0;
unsigned long tiempoActual;


byte ccdata[22]={0,0,0,0,0,0,0,//Encoder 1
                 0,0,0,0,      //Potes 1
                 0,0,0,0,0,0,0,//Encoder 2
                 0,0,0,0};     //Potes 2

byte ccdata_anterior[22]={255,255,255,255,255,255,255,//Encoder 1
                          255,255,255,255,            //Potes 1
                          255,255,255,255,255,255,255,//Encoder 2 
                          255,255,255,255,};           //Potes 2

/**********************************************************************************
** INICIALIZACIÓN
**********************************************************************************/
void setup() {
  Serial.begin(115200);
  Wire.begin(MCU_GRAL);
  //Wire.setClock(400000L);
  digitalWrite(SDA,0);
  digitalWrite(SCL,0);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  digitalWrite(LED1,1);
  digitalWrite(LED2,1);
  digitalWrite(LED3,1);
  digitalWrite(LED4,1);
  matrixInit();
  cargarEscala();
  tiempoActual=millis();
/**********************************************************************************
** COMPROBACIÓN I2C
**********************************************************************************/
  byte error1,error2;
  //MCU_IN
  Wire.beginTransmission(MCU_IN1);
  error1=Wire.endTransmission();
  Wire.beginTransmission(MCU_IN2);
  error2=Wire.endTransmission();
  if(!error1 && !error2)digitalWrite(LED1,0);//Si no hay errores
  else digitalWrite(LED1,1);
  //MCU_CC
  Wire.beginTransmission(MCU_CC1);
  error1=Wire.endTransmission();
  Wire.beginTransmission(MCU_CC2);
  error2=Wire.endTransmission();
  if(!error1 && !error2)digitalWrite(LED2,0);//Si no hay errores
  else digitalWrite(LED2,1);
  //MCU_OUT
  Wire.beginTransmission(MCU_OUT);
  error1=Wire.endTransmission();
  if(!error1)digitalWrite(LED3,0);//Si no hay errores
  else digitalWrite(LED3,1);
  //MCU_TFT
  Wire.beginTransmission(MCU_TFT);
  error1=Wire.endTransmission();
  if(!error1)digitalWrite(LED4,0);//Si no hay errores
  else digitalWrite(LED4,1);
}

void loop() {
#ifdef DEBUG_TIME
  tiempo=micros();
#endif

/**********************************************************************************
** INICIO DEL PROGRAMA
**********************************************************************************/
  byte actualMin=0;
  byte actualMax=0;
  e_anterior[0]=e_nuevo[0];
  e_anterior[1]=e_nuevo[1];
/**********************************************************************************
** RUTINA SERIE
**********************************************************************************/
  if(Serial.available()){
    processIncomingByte(Serial.read());
  }
/**********************************************************************************
** RUTINA MCU_IN2 (extras)
**********************************************************************************/
  Wire.requestFrom(MCU_IN2, 2); //Pide dos bytes
  
  actualMin = Wire.read(); 
  actualMax = Wire.read();
  e_nuevo[1] = actualMin;
  e_nuevo[1] = e_nuevo[1]<<8 | actualMax;
       
  if(e_nuevo[1]!=e_anterior[1]){ //Compara bytes anterior y nuevo
    for(byte i=0;i<16;i++){ //16 botones     
      if(BitRead(i,e_nuevo[1]) != BitRead(i,e_anterior[1])){ //Compara bit a bit
        if(BitRead(i,e_nuevo[1]) == 0){ //Si se presionó
          switch(i){//Cual se presionó
            //Shift
            case 0: if(shift_btn==0){
                      modo = M_SESSION;//Vuelve al modo sesion
                      //Apagar matriz
                      for(byte i=0;i<16;i++){
                        Wire.beginTransmission(MCU_OUT);     
                        Wire.write(3);//Identificador modo + cambio de param
                        Wire.write(0x80+1);//Nota on/off
                        Wire.write(i);//Nota
                        Wire.write(0);//Velocity
                        Wire.endTransmission();
                      }
                      Wire.beginTransmission(MCU_TFT);
                      Wire.write(M_SESSION | 128);//Modo
                      Wire.write(trackOffset);//Modo
                      Wire.write(sceneOffset);//Modo
                      Wire.write(0);//Modo
                      Wire.endTransmission();
                    }
                    shift_btn=!shift_btn;//Cambia el estado
                    //Prende el led del boton de shift
                    /*Wire.beginTransmission(MCU_OUT);     
                    Wire.write(M_EXTRA);
                    Wire.write(LEDONOFF);//Nota on
                    Wire.write(OUTLED1);//Nota
                    Wire.write(!shift_btn);//Velocity
                    Wire.endTransmission();*/
            break;
            //Escena 1 / Mixer
            case 1: if(shift_btn==0)mixer();
                    else notaOnOff(SCENE_0,1);
            break;
            //Escena 2 / DrumRack
            case 2: if(shift_btn==0)drumrack();
                    else notaOnOff(SCENE_1,1);
            break;
            //Escena 3 / Piano
            case 3: if(shift_btn==0)piano();
                    else notaOnOff(SCENE_2,1);
            break;
            //Escena 4 / Editor
            case 4: if(shift_btn==0)editor();
                    else notaOnOff(SCENE_3,1);
            break;
            //Stop 1
            case 5: notaOnOff(TRACKSTOP_0,1);
            break;
            //Stop 2
            case 6: notaOnOff(TRACKSTOP_1,1);
            break;
            //Stop 3
            case 7: notaOnOff(TRACKSTOP_2,1);
            break;
            //Stop 4
            case 8: notaOnOff(TRACKSTOP_3,1);
            break;
            //Sesion UP
            case 9:   if(modo==M_SESSION && sceneOffset>0){
                        sceneOffset--;
                        notaOnOff(SESSIONUP,1);
                        notaOnOff(SESSIONUP,0);
                      }
                      if(modo==M_MIXER);
                      if(modo==M_DRUMRACK)notaCC(SESSIONUP);
                      if(modo==M_PIANO && escalaActual>0){
                        escalaActual--;
                        cargarEscala();
                        notaCC(SESSIONDOWN);
                      }
                      if(modo==M_EDITOR);
            break;
            //Sesion DOWN
            case 10:  if(modo==M_SESSION && sceneOffset<4){
                        sceneOffset++;
                        notaOnOff(SESSIONDOWN,1);
                        notaOnOff(SESSIONDOWN,0);
                      }
                      if(modo==M_MIXER);
                      if(modo==M_DRUMRACK)notaCC(SESSIONDOWN);
                      if(modo==M_PIANO && escalaActual<24){
                        escalaActual++;
                        cargarEscala();
                        notaCC(SESSIONUP);
                      }
                      if(modo==M_EDITOR);
            break;
            //Sesion LEFT
            case 11:  if(modo==M_SESSION && trackOffset>0){
                        trackOffset--;
                        notaOnOff(SESSIONLEFT,1);
                        notaOnOff(SESSIONLEFT,0);
                      }
                      if(modo==M_MIXER);
                      if(modo==M_DRUMRACK);
                      if(modo==M_PIANO && octavaActual>-1){
                        octavaActual--;
                        cargarEscala();
                      }
                      if(modo==M_EDITOR);
            break;
            //Sesion RIGHT
            case 12:  if(modo==M_SESSION && trackOffset<4){
                        trackOffset++;
                        notaOnOff(SESSIONRIGHT,1);
                        notaOnOff(SESSIONRIGHT,0);
                      }
                      if(modo==M_MIXER);
                      if(modo==M_DRUMRACK);
                      if(modo==M_PIANO && octavaActual<6){
                        octavaActual++;
                        cargarEscala();
                      }
                      if(modo==M_EDITOR);
            break;
            //Alternar CC
            case 13:
            break;
            //Rec
            case 14:  notaOnOff(REC,1);
            break;
            
            case 15:
            break;
          }       
        }
        if(BitRead(i,e_nuevo[1])==1){//Si se soltó
          if((shift_btn==0 || i>4) && i!=0){//Si el boton de shift está desactivado
            //notaOnOff(i+16,0);
          } 
        }
      }
    }  
  } 

/**********************************************************************************
** RUTINA MCU_IN1 (matriz 4x4)
**********************************************************************************/
  Wire.requestFrom(MCU_IN1, 2);

  actualMin = Wire.read(); 
  actualMax = Wire.read();
  e_nuevo[0] = actualMin;
  e_nuevo[0] = e_nuevo[0]<<8 | actualMax;
  
  if(e_nuevo[0]!=e_anterior[0]){
    for(int i=0;i<16;i++){
      if(BitRead(i,e_nuevo[0])!=BitRead(i,e_anterior[0])){
        if(BitRead(i,e_nuevo[0])==0)notaOnOff(i,1);
        if(BitRead(i,e_nuevo[0])==1)notaOnOff(i,0);    
      }
    }  
  }

/**********************************************************************************
** RUTINA MCU_CC1
**********************************************************************************/
  Wire.requestFrom(MCU_CC1,11);//Encoders y Potes MCU 1
    
  while(Wire.available()){
    for(byte i=0;i<11;i++){
      ccdata[i]=Wire.read();
      
      if(ccdata[i]!=ccdata_anterior[i] && ccdata[i]!=255){
        Serial.write(0xB0+CHANNEL);
        Serial.write(i);
        Serial.write(ccdata[i]);

        if(modo==M_SESSION){
          Wire.beginTransmission(MCU_TFT);
          Wire.write(modo);//Modo
          Wire.write(0xB0+CHANNEL);//Nota on
          Wire.write(i);
          Wire.write(ccdata[i]);//Color
          Wire.endTransmission();
        }
        ccdata_anterior[i]=ccdata[i];
      }        
    }
  }

/**********************************************************************************
** RUTINA MCU_CC2
**********************************************************************************/
  Wire.requestFrom(MCU_CC2,11);//Encoders y Potes MCU 2
   
  while(Wire.available()){
    for(byte i=11;i<22;i++){
      ccdata[i]=Wire.read();
      
      if(ccdata[i]!=ccdata_anterior[i] && ccdata[i]!=255){
        Serial.write(0xB0+CHANNEL);
        Serial.write(i);
        Serial.write(ccdata[i]);
        ccdata_anterior[i]=ccdata[i];
      }        
    }
  }
/**********************************************************************************
** FIN DEL PROGRAMA
**********************************************************************************/
#ifdef DEBUG_TIME
  tiempoAct=micros()-tiempo;
  if(tiempoAct > tiempoMax){
    tiempoMax=tiempoAct;
    Serial.println("--------------");
    Serial.println(tiempoAct);  
  }
#endif 
}

/**********************************************************************************
** FUNCIONES
**********************************************************************************/
void notaCC(byte i){
  switch(i){
    case SESSIONUP:     if(notasCC<127)notasCC++;
    break;
    case SESSIONDOWN:   if(notasCC>0)notasCC--;
    break;
  }
  Serial.write(0xB0);
  Serial.write(SESSIONUP);
  Serial.write(notasCC);
}
/*********************************************************************************/
void cargarEscala(void){
  byte nota=0,octava=0;
  //Apaga todas las notas 
  for(byte i=0;i<128;i++){
    Serial.write(0x80+PIANOCHANNEL);
    Serial.write(i);
    Serial.write(0);
  }
  if(modo==M_PIANO){
    //Comunica con la TFT
    Wire.beginTransmission(MCU_TFT);
    Wire.write(M_PIANO);//Modo
    Wire.write(escalaActual);//Nota on
    Wire.write(octavaActual);
    Wire.write(tonicaActual);//Color
    Wire.endTransmission();
  }
  do{
    for(byte i=0;i<12;i++){//11 notas de la octava
      //Prende las matriz
      if(modo==M_PIANO){
          Wire.beginTransmission(MCU_OUT);
          Wire.write(M_DRUMRACK);//Modo
          Wire.write(0x90+1);//Nota on
          Wire.write(nota);
          if(!i)Wire.write(30);//Color amarillo
          else Wire.write(10);//Color rojo
          Wire.endTransmission();
      }
      //Llena el vector
      if(BitRead(11-i,escala_intervalo[escalaActual])){//Si pertenece a la escala
        MXY[M_PIANO][nota]=i+octava+((octavaActual+2)*12)+tonicaActual;//La guarda en un botón
        nota++;//Siguiente botón
      }
      if(nota==16)i=13;//Rompe el bucle si llega los 16 botones      
    }
    octava+=12;
  }while(nota<16);//Hasta llenar los 16 botones
}
/*********************************************************************************/
void notaOnOff(byte i,byte onoff){
  byte boton;
  //Botones matriz
  switch(i){
    case 0:  boton=MXY[modo][0];
    break;
    case 1:  boton=MXY[modo][1];
    break;
    case 2:  boton=MXY[modo][2];
    break;
    case 3:  boton=MXY[modo][3];
    break;
    case 4:  boton=MXY[modo][4];
    break;
    case 5:  boton=MXY[modo][5];
    break; 
    case 6:  boton=MXY[modo][6];
    break;
    case 7:  boton=MXY[modo][7];
    break;
    case 8:  boton=MXY[modo][8];
    break;
    case 9:  boton=MXY[modo][9];
    break;
    case 10: boton=MXY[modo][10];
    break;
    case 11: boton=MXY[modo][11];
    break;
    case 12: boton=MXY[modo][12];
    break;
    case 13: boton=MXY[modo][13];
    break;
    case 14: boton=MXY[modo][14];
    break;
    case 15: boton=MXY[modo][15];
    break;
  //Botones extra
    case 16: boton=255;//Shift
    break;
    case SCENE_0:       boton=SCENE_0;
    break;
    case SCENE_1:       boton=SCENE_1;
    break;
    case SCENE_2:       boton=SCENE_2;
    break;
    case SCENE_3:       boton=SCENE_3;
    break;
    case TRACKSTOP_0:   boton=TRACKSTOP_0;
    break; 
    case TRACKSTOP_1:   boton=TRACKSTOP_1;
    break;
    case TRACKSTOP_2:   boton=TRACKSTOP_2;
    break;
    case TRACKSTOP_3:   boton=TRACKSTOP_3;
    break;
    case SESSIONUP:     boton=SESSIONUP;
    break;
    case SESSIONDOWN:   boton=SESSIONDOWN;
    break;
    case SESSIONLEFT:   boton=SESSIONLEFT;
    break;
    case SESSIONRIGHT:  boton=SESSIONRIGHT;
    break;
    case REC:           boton=REC;
    break;
    case PLAY:          boton=PLAY;
    break;
    case STOP:          boton=STOP;
    break;
  }
  if(boton!=255){
#ifdef DEBUG 
    if(onoff==1)Serial.print("Nota ON/OFF: ");
    if(onoff==0)Serial.print("Nota OFF: ");
    Serial.print(boton);
    Serial.println();
#else
    //MIDI OUT
    if(i<16){
      if(modo==M_SESSION){
        if(onoff==1)Serial.write(0x90+CHANNEL);
        if(onoff==0)Serial.write(0x80+CHANNEL);
      }
      else if(modo==M_PIANO){
        if(onoff==1)Serial.write(0x90+PIANOCHANNEL);
        if(onoff==0)Serial.write(0x80+PIANOCHANNEL);
      }
      else if(modo==M_DRUMRACK){
        if(onoff==1)Serial.write(0x90+PADCHANNEL);
        if(onoff==0)Serial.write(0x80+PADCHANNEL);
      }
    }
    else{
      if(onoff==1)Serial.write(0x90+CHANNEL);
      if(onoff==0)Serial.write(0x80+CHANNEL);
    }
    Serial.write(boton);
    Serial.write(127);
    
    //TFT OUT
    if(modo==M_SESSION){
      Wire.beginTransmission(MCU_TFT);
      Wire.write(modo);
      if(onoff==1)Wire.write(0x90+CHANNEL);
      if(onoff==0)Wire.write(0x80+CHANNEL);
      Wire.write(boton);
      Wire.write(127);
      Wire.endTransmission();
    }
#endif
  }
}
/*********************************************************************************/
byte BitRead(byte pos,unsigned int param){//Devuelve un bit de un byte
  return((param>>pos) & 1); //0x8000 = 0b1000 0000 0000 0000, corre 15 para devolver el bit 0  
}
/*********************************************************************************/
unsigned int BitWrite(byte pos, byte estado,unsigned int param){ //Escribe un bit en una posicion                           
  if(estado==1)param = param | (1<<pos); //0000 0000 -> 0000 0010: 0000 0000 |  1<<2                          //SET
  if(estado==0)param = param & ~(1<<pos);//1111 1111 -> 1111 1101: 1111 1111 & ~1<<2 -> 1111 1111 & 1111 1101 //CLEAR
  return param;
}
/*********************************************************************************/
void mixer(void){
  modo = M_MIXER;
  shift_btn=1;//Desactiva el shift
  
  //Apaga el led del boton de shift
  /*Wire.beginTransmission(MCU_OUT);     
  Wire.write(M_EXTRA);
  Wire.write(LEDONOFF);//Nota on
  Wire.write(OUTLED1);//Nota
  Wire.write(0);//Velocity
  Wire.endTransmission();*/
  
  //Apagar matriz
  for(byte i=0;i<16;i++){
    Wire.beginTransmission(MCU_OUT);     
    Wire.write(3);//Identificador modo + cambio de param
    Wire.write(0x80+1);//Nota on/off
    Wire.write(i);//Nota
    Wire.write(0);//Velocity
    Wire.endTransmission();
  }
  
  Wire.beginTransmission(MCU_OUT);
  Wire.write(modo);//Identificador
  Wire.endTransmission();

  Wire.beginTransmission(MCU_TFT);
  Wire.write(BitWrite(7,1,modo));//Identificador
  Wire.endTransmission();   
}
/*********************************************************************************/
void drumrack(void){
  modo = M_DRUMRACK;
  shift_btn=1;//Desactiva el shift
  
  //Apaga el led del boton de shift
  /*Wire.beginTransmission(MCU_OUT);     
  Wire.write(M_EXTRA);
  Wire.write(LEDONOFF);//Nota on
  Wire.write(OUTLED1);//Nota
  Wire.write(0);//Velocity
  Wire.endTransmission();*/
  
  //Apagar matriz
  for(byte i=0;i<16;i++){
    Wire.beginTransmission(MCU_OUT);     
    Wire.write(3);//Identificador modo + cambio de param
    Wire.write(0x80+1);//Nota on/off
    Wire.write(i);//Nota
    Wire.write(0);//Velocity
    Wire.endTransmission();
  }
  
  Wire.beginTransmission(MCU_OUT);
  Wire.write(modo);//Identificador
  Wire.endTransmission();

  Wire.beginTransmission(MCU_TFT);
  Wire.write(BitWrite(7,1,modo));//Identificador
  Wire.endTransmission();
}
/*********************************************************************************/
void piano(void){
  modo = M_PIANO;
  shift_btn=1;//Desactiva el shift
  
  //Apaga el led del boton de shift
  /*Wire.beginTransmission(MCU_OUT);     
  Wire.write(M_EXTRA);
  Wire.write(LEDONOFF);//Nota on
  Wire.write(OUTLED1);//Nota
  Wire.write(0);//Velocity
  Wire.endTransmission();*/
  
  cargarEscala();
}
/*********************************************************************************/
void editor(void){
  modo = M_EDITOR;
  shift_btn=1;//Desactiva el shift

  //Apaga el led del boton de shift
  /*Wire.beginTransmission(MCU_OUT);     
  Wire.write(M_EXTRA);
  Wire.write(LEDONOFF);//Nota on
  Wire.write(OUTLED1);//Nota
  Wire.write(0);//Velocity
  Wire.endTransmission();*/
  
  //Apagar matriz
  for(byte i=0;i<16;i++){
    Wire.beginTransmission(MCU_OUT);     
    Wire.write(3);//Identificador modo + cambio de param
    Wire.write(0x80+1);//Nota on/off
    Wire.write(i);//Nota
    Wire.write(0);//Velocity
    Wire.endTransmission();
  }
  
  Wire.beginTransmission(MCU_OUT);
  Wire.write(modo);//Identificador
  Wire.endTransmission();

  Wire.beginTransmission(MCU_TFT);
  Wire.write(BitWrite(7,1,modo));//Identificador
  Wire.endTransmission();
}
/*********************************************************************************/
void matrixInit(void){ //Muestra el logo y limpia la matriz
  unsigned int logo=0b0100111101100100;

  for(byte i=0;i<128;i++){
    if(i== 1 || i==22  || i==41 || i==64 ||
       i==81 || i==106 || i==127){
      for(byte x=0;x<16;x++){
        Wire.beginTransmission(MCU_OUT);
        Wire.write(3);
        if(BitRead(x,logo))Wire.write(0x90+1);//Nota on
        else Wire.write(0x80+1);//Nota off
        Wire.write(x);//nota
        Wire.write(i);//Color
        Wire.endTransmission();
      }
      delay(100);
    }
  }
  for(byte i=0;i<16;i++){
    Wire.beginTransmission(MCU_OUT);     
    Wire.write(3);//Identificador modo + cambio de param
    Wire.write(0x80+1);//Nota on/off
    Wire.write(i);//Nota
    Wire.write(0);//Velocity
    Wire.endTransmission();
  }
}
/*********************************************************************************/
void processIncomingByte (byte inByte){//Procesa los bytes recibidos
  static byte recibido[3]={0,0,0};
  static byte recibido_pos=0;
  byte error;
  if(recibido_pos < 3){
      recibido[recibido_pos] = inByte;
      recibido_pos++;
  }
  if(recibido_pos==3){
    
    recibido_pos=0;
    Wire.beginTransmission(MCU_OUT);
    Wire.write(modo);//Identificador modo + cambio de param
    Wire.write(recibido[0]);//Nota on/off
    Wire.write(recibido[1]);//Nota
    Wire.write(recibido[2]);//Velocity
    Wire.endTransmission();
  }
}

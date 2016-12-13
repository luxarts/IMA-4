/*  Funciones TFT:
 *  drawPixel(x, y, color)
 *  drawLine(x0, y0, x1, y1, color)
 *  drawRect(x, y, w, h, color)
 *  fillRect(x, y, w, h, color)
 *  fillScreen(uint16_t color)
 *  drawChar(x, y, c, color, bg, size)
 *  drawChar(c, x, y, size)
 *  drawCircle(x0, y0, r, color)
 *  fillCircle(x0, y0, r, color)
 *  drawTriangle(x0, y0, x1, y1, x2, y2, color)
 *  fillTriangle(x0, y0, x1, y1, x2, y2, color)
 *  drawRoundRect(x0, y0, w, h, radius, color)
 *  fillRoundRect(x0, y0, w, h, radius, color)
 *  setTextColor(c)
 *  setTextColor(c, bg)
 *  setTextSize(s)
 *  setTextWrap(w)
 *  setRotation(r)
 *  drawNumber(long_num,poX, poY, size)
 *  drawString(*string, poX, poY, size)
 *  drawCentreString(*string, dX, poY, size)
 *  drawRightString(*string, dX, poY, size)
 *  drawFloat(floatNumber, decimal, poX, poY, size)
 *  drawRAW(name,x,y,w,h);
 *  printVar(var,x,y)
 */
#include <Wire.h>
#include <Adafruit_GFX_AS.h>     // Core graphics library
#include <Adafruit_ILI9341_AS.h> // Hardware-specific library
#include <SdFat.h>               // More compact and faster than SD
#include <SPI.h> // SPI libray obviously!

//#define DEBUG

//MCU en el bus I2C
#define MCU_IN1  0x10
#define MCU_IN2  0x11
#define MCU_CC1  0x12
#define MCU_CC2  0x13
#define MCU_OUT  0x14
#define MCU_GRAL 0x15
#define MCU_TFT  0x16

//Pinout TFT y SD
#define _sclk  13
#define _miso  12 
#define _mosi  11 
#define _cs    10
#define _dc    9
#define _sdcs  8
#define _rst   7

//Modos
#define M_SESSION   0
#define M_MIXER     1
#define M_PIANO     2
#define M_DRUMRACK  3
#define M_EDITOR    4
#define CHANNEL     0
#define PADCHANNEL  1

//Botones
#define MUTE_0        100
#define MUTE_1        101
#define MUTE_2        102
#define MUTE_3        103
#define SOLO_0        104
#define SOLO_1        105
#define SOLO_2        106
#define SOLO_3        107
#define REC_0         108
#define REC_1         109
#define REC_2         110
#define REC_3         111
#define SESIONUP      112
#define SESSIONDOWN   113
#define SESSIONLEFT   114
#define SESSIONRIGHT  115
#define ZOOMUP        116
#define ZOOMDOWN      117
#define ZOOMLEFT      118
#define ZOOMRIGHT     119
#define SCENE_0       120
#define SCENE_1       121
#define SCENE_2       122
#define SCENE_3       123
#define STOP_0        124
#define STOP_1        125
#define STOP_2        126
#define STOP_3        127

//CC1
#define PARAMCONTROL_0  0//Encoder
#define PARAMCONTROL_1  1//Encoder
#define PARAMCONTROL_2  2//Encoder
#define PARAMCONTROL_3  3//Encoder
#define PARAMCONTROL_4  4//Encoder
#define PARAMCONTROL_5  5//Encoder
#define PARAMCONTROL_6  6//Encoder
#define TRACKVOL_0      7//Pot
#define TRACKVOL_1      8//Pot
#define TRACKVOL_2      9//Pot
#define TRACKVOL_3      10//Pot
//CC2
#define PARAMCONTROL_7  11//Encoder
#define PARAMTRACK_0    12//Encoder
#define PARAMTRACK_1    13//Encoder
#define PARAMTRACK_2    14//Encoder
#define PARAMTRACK_3    15//Encoder
//Encoder extra 16
//Encoder extra 17
#define MASTERVOLUME    18//Pot
#define MASTERBALANCE   19//Pot
//Pot extra 20
//Pot extra 21


Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(_cs, _dc, _rst); // Invoke custom library
SdFat SD;                        // For SD compatibility

//Variables globales
byte modoActual=M_SESSION;
byte modo;
byte recibido[6]={0,0,0,0,0,0};
byte newData=0;

byte trackOffset=0;
byte sceneOffset=0;
byte Mute=0b00000000;
byte Solo=0b00000000;//Solo
byte Rec =0b00000000;//Rec 8-1
byte track[8]={0,0,0,0,0,0,0,0}; //Pistas 1-8
byte macro[8]={0,0,0,0,0,0,0,0}; //Macros 1-8

byte tonicaActual=0;
byte escalaActual=0;
char octavaActual=0;

void setup(){  
  Wire.begin(MCU_TFT);
  Wire.setClock(400000L);
  digitalWrite(SDA,0);
  digitalWrite(SCL,0);
  Wire.onReceive(receiveEvent);
  delay(10);
  SD.begin(_sdcs, SPI_HALF_SPEED);
  tft.init();
  tft.setRotation(1);
  drawRAW("LogoOn.raw",0,0,320,240);
  delay(500); 
  sesion();
}

void loop(){
  if(newData){//Si hay un dato nuevo
    newData=0;//Limpia bandera
    recibido[0]=recibido[3];//Copia el buffer
    recibido[1]=recibido[4];
    recibido[2]=recibido[5];
    /****************
    if(newData)break;//Rompe el programa si se recibió algo en el medio
    /****************/
    //Comprueba si solo es un cambio de modo
    if(BitRead(7,modo)){//Es solo un cambio de modos
      modo=BitWrite(7,0,modo);//Escribe un 0 en el bit 7 de modo
      
      if(modo==M_SESSION){
        if(modoActual!=modo)sesion();
        trackOffset=recibido[0];
        sceneOffset=recibido[1];
      }
      if(modo==M_MIXER){
        //if(modoActual!=modo)sesion();
      }
      if(modo==M_DRUMRACK){
        //if(modoActual!=modo)sesion();
      }
      if(modo==M_EDITOR){
        //if(modoActual!=modo)sesion();
      }
    }
    //Es un cambio de modo + parametro
    else{
      if(modo==M_SESSION){ //Máscara en modo
        if(modoActual!=modo)sesion();
        
        if(recibido[0]==0x80+CHANNEL || recibido[0]==0x90+CHANNEL){
          switch(recibido[1]){
          //Botones de solo
            case SOLO_0: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printSolo(0,1);//Si es un nota on y velocity es mayor a 0
                         else printSolo(0,0);
            break;
            case SOLO_1: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printSolo(1,1);
                         else printSolo(1,0);
            break;
            case SOLO_2: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printSolo(2,1);
                         else printSolo(2,0);
            break;
            case SOLO_3: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printSolo(3,1);
                         else printSolo(3,0);
            break;
          //Botones de Mute
            case MUTE_0: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printMute(0,1);//Si es un nota on y velocity es mayor a 0
                         else printMute(0,0);
            break;
            case MUTE_1: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printMute(1,1);
                         else printMute(1,0);
            break;
            case MUTE_2: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printMute(2,1);
                         else printMute(2,0);
            break;
            case MUTE_3: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printMute(3,1);
                         else printMute(3,0);
            break;
          //Botones de Rec
            case REC_0: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printRec(0,1);//Si es un nota on y velocity es mayor a 0
                        else printRec(0,0);
            break;
            case REC_1: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printRec(1,1);
                        else printRec(1,0);
            break;
            case REC_2: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printRec(2,1);
                        else printRec(2,0);
            break;
            case REC_3: if(recibido[0]==0x90+CHANNEL && recibido[2]>0)printRec(3,1);
                        else printRec(3,0);
            break;
            case SESSIONLEFT: if(recibido[0]==0x90+CHANNEL && recibido[2]>0){
                                if(trackOffset>0){
                                  trackOffset--;
                                  sesionRefresh();
                                }      
                              }
            break;
            case SESSIONRIGHT: if(recibido[0]==0x90+CHANNEL && recibido[2]>0){
                                 if(trackOffset<4){
                                   trackOffset++;
                                   sesionRefresh();
                                 }
                               }
            break;
            case ZOOMLEFT:  if(recibido[0]==0x90+CHANNEL && recibido[2]>0){
                              trackOffset=0;
                              sesionRefresh();
                            }
            break;
            case ZOOMRIGHT: if(recibido[0]==0x90+CHANNEL && recibido[2]>0){
                              trackOffset=4;
                              sesionRefresh();
                            }
            break;
          }
        }
        if(recibido[0]==0xB0+CHANNEL){
          switch(recibido[1]){
            case TRACKVOL_0: barraPista(0,recibido[2]);
            break;
            case TRACKVOL_1: barraPista(1,recibido[2]);
            break;
            case TRACKVOL_2: barraPista(2,recibido[2]);
            break;
            case TRACKVOL_3: barraPista(3,recibido[2]);
            break;
            case PARAMCONTROL_0: barraRack(0,recibido[2]);
            break;
            case PARAMCONTROL_1: barraRack(1,recibido[2]);
            break;
            case PARAMCONTROL_2: barraRack(2,recibido[2]);
            break;
            case PARAMCONTROL_3: barraRack(3,recibido[2]);
            break;
            case PARAMCONTROL_4: barraRack(4,recibido[2]);
            break;
            case PARAMCONTROL_5: barraRack(5,recibido[2]);
            break;
            case PARAMCONTROL_6: barraRack(6,recibido[2]);
            break;
            case PARAMCONTROL_7: barraRack(7,recibido[2]);
            break;
          }
        }
      }
      if(modo==M_MIXER){ //Máscara en modo
        //if(modoActual!=modo)sesion();
      }
      if(modo==M_PIANO){ //Máscara en modo
        if(modoActual!=modo)piano();
        
        if(recibido[0]!=escalaActual){
          escalaActual=recibido[0];
          printEscala();
        }
        if(recibido[1]!=octavaActual){
          octavaActual=recibido[1];
          printOctava();
        }
        if(recibido[2]!=tonicaActual){
          tonicaActual=recibido[2];
          printTonica();
        }  
      }
      if(modo==M_DRUMRACK){ //Máscara en modo
        //if(modoActual!=modo)sesion();
      }
      if(modo==M_EDITOR){ //Máscara en modo
        //if(modoActual!=modo)sesion();
      }
    }  
  }
}

#ifdef DEBUG
void printVar(byte var,byte x,byte y){
  tft.fillRect(x,y,40,20,ILI9341_BLUE);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_GREEN);
  tft.drawNumber(var,x+2,y+1,2);
}
#endif

void sesion(void){
  modoActual=M_SESSION;
  drawRAW("Sesion.raw",0,0,320,240);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  //Inicializa los números de rack
  for(byte i=0;i<4;i++)tft.drawNumber(i+5,13+160+40*i,207,2);//Rack inf
  for(byte i=0;i<4;i++)tft.drawNumber(i+1,13+160+40*i,109,2);//Rack sup
  //Inicializa los números de las pistas  
  printPistas();
  for(byte i=0;i<8;i++)barraRack(i,macro[i]);
  //Inicializa todas las pistas
  for(byte pista=0;pista<4;pista++)barraPista(pista,track[pista+trackOffset]);
  //Inicializa todos los solo
  for(byte pista=0;pista<4;pista++)printSolo(pista,BitRead(pista+trackOffset,Solo));
  //Inicializa todos los mute
  for(byte pista=0;pista<4;pista++)printMute(pista,BitRead(pista+trackOffset,Mute));
  //Inicializa todos los rec
  for(byte pista=0;pista<4;pista++)printRec(pista,BitRead(pista+trackOffset,Rec));
}
void piano(void){
  modoActual=M_PIANO;
  drawRAW("Piano.raw",0,0,320,240);
  printEscala();
  printOctava();
  printTonica();
}
//Funciones de Piano
void printTonica(void){
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_BLACK);
  tft.fillRect(240,176,60,52,ILI9341_WHITE);
  switch(tonicaActual){
    case 0: tft.drawString("C",250,177,2);
    break;
    case 1: tft.drawString("C#",250,177,2);
    break;
    case 2: tft.drawString("D",250,177,2);
    break;
    case 3: tft.drawString("D#",250,177,2);
    break;
    case 4: tft.drawString("E",250,177,2);
    break;
    case 5: tft.drawString("F",250,177,2);
    break;
    case 6: tft.drawString("F#",250,177,2);
    break;
    case 7: tft.drawString("G",250,177,2);
    break;
    case 8: tft.drawString("G#",250,177,2);
    break;
    case 9: tft.drawString("A",250,177,2);
    break;
    case 10:  tft.drawString("A#",250,177,2);
    break;
    case 11:  tft.drawString("B",250,177,2);
    break;
  }
}
void printOctava(void){
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_BLACK);
  tft.fillRect(240,77,60,52,ILI9341_WHITE);
  tft.drawNumber(octavaActual,250,79,2);
}
void printEscala(void){
  tft.setTextSize(2);
  tft.fillRect(20,76+33*0,200,26,ILI9341_WHITE);
  tft.fillRect(20,76+33*1,200,26,ILI9341_WHITE);
  tft.fillRect(20,76+33*2,200,26,ILI9341_RED);
  tft.fillRect(20,76+33*3,200,26,ILI9341_WHITE);
  tft.fillRect(20,76+33*4,200,26,ILI9341_WHITE);

  switch(escalaActual){
    case 0: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Arabe",22,70+66+33*0,2);//Actual
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Armonico menor",22,70+66+33*1,2);//+1
            tft.drawString("Aumentada",22,70+66+33*2,2);//+2
    break;
    case 1: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Armonico menor",22,70+33+33*1,2);//Actual
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Arabe",22,70+33+33*0,2);//-1
            tft.drawString("Aumentada",22,70+33+33*2,2);//+1
            tft.drawString("Bebop dom.",22,70+33+33*3,2);//+2
    break;
    case 2: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Aumentada",22,70+33*2,2);//Actual
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Arabe",22,70+33*0,2);//-2
            tft.drawString("Armonico menor",22,70+33*1,2);//-1
            tft.drawString("Bebop dom.",22,70+33*3,2);//+1
            tft.drawString("Bebop mayor",22,70+33*4,2);//+2
    break;
    case 3: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Bebop dom.",22,70+33*2,2);//Actual
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Armonico menor",22,70+33*0,2);//-2
            tft.drawString("Aumentada",22,70+33*1,2);//-1
            tft.drawString("Bebop mayor",22,70+33*3,2);//+1
            tft.drawString("Blues",22,70+33*4,2);//+2
    break;
    case 4: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Bebop mayor",22,70+33*2,2);//Actual
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Aumentada",22,70+33*0,2);//-2
            tft.drawString("Bebop dom.",22,70+33*1,2);//-1
            tft.drawString("Blues",22,70+33*3,2);//+1
            tft.drawString("Cromatica",22,70+33*4,2);//+2
    break;
    case 5: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Blues",22,70+33*2,2);//Actual
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Bebop dom.",22,70+33*0,2);//-2
            tft.drawString("Bebop mayor",22,70+33*1,2);//-1
            tft.drawString("Cromatica",22,70+33*3,2);//+1
            tft.drawString("Disminuida",22,70+33*4,2);//+2
    break;
    case 6: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Cromatica",22,70+33*2,2);//Actual
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Bebop mayor",22,70+33*0,2);//-2
            tft.drawString("Blues",22,70+33*1,2);//-1
            tft.drawString("Disminuida",22,70+33*3,2);//+1
            tft.drawString("Dorico",22,70+33*4,2);//+2
    break;
    case 7: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Disminuida",22,70+33*2,2);
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Blues",22,70+33*0,2);
            tft.drawString("Cromatica",22,70+33*1,2);
            tft.drawString("Dorico",22,70+33*3,2);
            tft.drawString("Enigmatica",22,70+33*4,2);
    break;
    case 8: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Dorico",22,70+33*2,2);
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Cromatica",22,70+33*0,2);
            tft.drawString("Disminuida",22,70+33*1,2);
            tft.drawString("Enigmatica",22,70+33*3,2);
            tft.drawString("Mayor",22,70+33*4,2);
    break;
    case 9: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Enigmatica",22,70+33*2,2);
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Disminuida",22,70+33*0,2);
            tft.drawString("Dorico",22,70+33*1,2);
            tft.drawString("Mayor",22,70+33*3,2);
            tft.drawString("Menor",22,70+33*4,2);
    break;
    case 10: tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Mayor",22,70+33*2,2);
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Dorico",22,70+33*0,2);
            tft.drawString("Enigmatica",22,70+33*1,2);
            tft.drawString("Menor",22,70+33*3,2);
            tft.drawString("Frigio",22,70+33*4,2);
    break;
    case 11:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Menor",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Enigmatica",22,70+33*0,2);
              tft.drawString("Mayor",22,70+33*1,2);
              tft.drawString("Frigio",22,70+33*3,2);
              tft.drawString("Japonesa",22,70+33*4,2);
    break;
    case 12:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Frigio",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Mayor",22,70+33*0,2);
              tft.drawString("Menor",22,70+33*1,2);
              tft.drawString("Japonesa",22,70+33*3,2);
              tft.drawString("Lidio",22,70+33*4,2);
    break;
    case 13:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Japonesa",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Menor",22,70+33*0,2);
              tft.drawString("Frigio",22,70+33*1,2);
              tft.drawString("Lidio",22,70+33*3,2);
              tft.drawString("Locrio",22,70+33*4,2);
    break;
    case 14:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Lidio",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Frigio",22,70+33*0,2);
              tft.drawString("Japonesa",22,70+33*1,2);
              tft.drawString("Locrio",22,70+33*3,2);
              tft.drawString("Magiar",22,70+33*4,2);
    break;
    case 15:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Locrio",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Japonesa",22,70+33*0,2);
              tft.drawString("Lidio",22,70+33*1,2);
              tft.drawString("Magiar",22,70+33*3,2);
              tft.drawString("Melodica menor",22,70+33*4,2);
    break;
    case 16:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Magiar",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Lidio",22,70+33*0,2);
              tft.drawString("Locrio",22,70+33*1,2);
              tft.drawString("Melodica menor",22,70+33*3,2);
              tft.drawString("Mixolidio",22,70+33*4,2);
    break;
    case 17:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Melodica menor",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Locrio",22,70+33*0,2);
              tft.drawString("Magiar",22,70+33*1,2);
              tft.drawString("Mixolidio",22,70+33*3,2);
              tft.drawString("Napolitana",22,70+33*4,2);
    break;
    case 18:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Mixolidio",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Magiar",22,70+33*0,2);
              tft.drawString("Melodica menor",22,70+33*1,2);
              tft.drawString("Napolitana",22,70+33*3,2);
              tft.drawString("Napo. menor",22,70+33*4,2);
    break;
    case 19:  tft.setTextColor(ILI9341_WHITE);
            tft.drawString("Napolitana",22,70+33*2,2);
            tft.setTextColor(ILI9341_BLACK);
            tft.drawString("Melodica menor",22,70+33*0,2);
            tft.drawString("Mixolidio",22,70+33*1,2);
            tft.drawString("Napo. menor",22,70+33*3,2);
            tft.drawString("Pentafonica",22,70+33*4,2);
    break;
    case 20:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Napo. menor",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Mixolidio",22,70+33*0,2);
              tft.drawString("Napolitana",22,70+33*1,2);
              tft.drawString("Pentafonica",22,70+33*3,2);
              tft.drawString("Penta. mayor",22,70+33*4,2);
    break;
    case 21:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Pentafonica",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Napolitana",22,70+33*0,2);
              tft.drawString("Napo. menor",22,70+33*1,2);
              tft.drawString("Penta. mayor",22,70+33*3,2);
              tft.drawString("Penta. menor",22,70+33*4,2);
    break;
    case 22:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Penta. mayor",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Napo. menor",22,70+33*0,2);
              tft.drawString("Pentafonica",22,70+33*1,2);
              tft.drawString("Penta. menor",22,70+33*3,2);
              tft.drawString("Whole",22,70+33*4,2);
    break;
    case 23:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Penta. menor",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Pentafonica",22,70+33*0,2);
              tft.drawString("Penta. mayor",22,70+33*1,2);
              tft.drawString("Whole",22,70+33*3,2);

    break;
    case 24:  tft.setTextColor(ILI9341_WHITE);
              tft.drawString("Whole",22,70+33*2,2);
              tft.setTextColor(ILI9341_BLACK);
              tft.drawString("Penta. mayor",22,70+33*0,2);
              tft.drawString("Penta. menor",22,70+33*1,2);
    break;
  }
}

//Funciones de Sesion
void sesionRefresh(void){
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  //Inicializa los números de las pistas  
  printPistas();
  //Inicializa todas las pistas
  for(byte pista=0;pista<4;pista++)barraPista(pista,track[pista+trackOffset]);
  //Inicializa todos los solo
  for(byte pista=0;pista<4;pista++)printSolo(pista,BitRead(pista+trackOffset,Solo));
  //Inicializa todos los mute
  for(byte pista=0;pista<4;pista++)printMute(pista,BitRead(pista+trackOffset,Mute));
  //Inicializa todos los rec
  for(byte pista=0;pista<4;pista++)printRec(pista,BitRead(pista+trackOffset,Rec));
}
void barraPista(byte columna,byte valor){
  byte pista=columna;
  track[columna]=valor;
  if(pista>=4)pista-=trackOffset;
  valor=128-valor;
  tft.fillRect(4+40*pista,94,10,valor/2,0x4208);//Fondo gris
  if(valor<22 && valor>0)   tft.fillRect(4+40*pista,94+valor/2,10,64-valor/2,ILI9341_RED);//Fuerte
  else if(valor>44)         tft.fillRect(4+40*pista,94+valor/2,10,64-valor/2,ILI9341_GREEN);//Debil
  if(valor>=22 && valor<=44)tft.fillRect(4+40*pista,94+valor/2,10,64-valor/2,ILI9341_YELLOW);//Medio
  
  tft.fillRect(15+40*pista,94+52,24,12,ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  valor=(128-valor)*100/127;//Mapeo
  tft.drawNumber(valor,15+40*pista,94+51,2);
}
void barraRack(byte rack,byte valor){
  if(rack<4){ //Las 4 filas superiores
    valor=128-valor;
    tft.fillRect(4+160+40*rack,144-99,10,valor/2,0x4208);//Fondo gris
    if(valor<22 && valor>0)tft.fillRect(4+160+40*rack,144-99+valor/2,10,64-valor/2,ILI9341_RED);//Fuerte
    else if(valor>44)tft.fillRect(4+160+40*rack,144-99+valor/2,10,64-valor/2,ILI9341_GREEN);//Debil
    if(valor>=22 && valor<=44)tft.fillRect(4+160+40*rack,144-99+valor/2,10,64-valor/2,ILI9341_YELLOW);//Medio
    
    tft.fillRect(15+160+40*rack,144-99+52,24,12,ILI9341_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_WHITE);
    valor=(128-valor)*100/127;//Mapeo
    tft.drawNumber(valor,15+160+40*rack,144-99+51,2);
  }
  if(rack>=4 && rack<8){  //Las 4 filas inferiores
    rack-=4;
    valor=128-valor;
    tft.fillRect(4+160+40*rack,144,10,valor/2,0x4208);//Fondo gris
    if(valor<22 && valor>0)tft.fillRect(4+160+40*rack,144+valor/2,10,64-valor/2,ILI9341_RED);//Fuerte
    else if(valor>44)tft.fillRect(4+160+40*rack,144+valor/2,10,64-valor/2,ILI9341_GREEN);//Debil
    if(valor>=22 && valor<=44)tft.fillRect(4+160+40*rack,144+valor/2,10,64-valor/2,ILI9341_YELLOW);//Medio
    
    tft.fillRect(15+160+40*rack,144+52,24,12,ILI9341_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_WHITE);
    valor=(128-valor)*100/127;//Mapeo
    tft.drawNumber(valor,15+160+40*rack,144+51,2);
  } 
}
void printSolo(byte pista,byte estado){
  if(pista<4){
    if(estado){//Activado
      tft.fillRect(4+40*pista,206,32,15,ILI9341_BLUE);
      Solo=BitWrite(pista+trackOffset,1,Solo);
    }
    else{
      tft.fillRect(4+40*pista,206,32,15,0x4208);//Gris
      Solo=BitWrite(pista+trackOffset,0,Solo);
    }
  }
}
void printMute(byte pista,byte estado){
  if(pista<4){
    if(!estado){//Activado
      tft.fillRect(4+40*pista,190,32,15,0xEBA0);//Naranja
      Mute=BitWrite(pista+trackOffset,1,Mute);
    }
    else{
      tft.fillRect(4+40*pista,190,32,15,0x4208);//Gris
      Mute=BitWrite(pista+trackOffset,0,Mute);
    }
  }
}
void printRec(byte pista,byte estado){
  if(pista<4){
    if(estado){//Activado
      tft.fillRect(4+40*pista,222,32,15,ILI9341_RED);
      Rec=BitWrite(pista+trackOffset,1,Rec);
    }
    else{
      tft.fillRect(4+40*pista,222,32,15,0x4208);//Gris
      Rec=BitWrite(pista+trackOffset,0,Rec);
    }
  }
}
void printPistas(void){
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  for(byte i=0;i<4;i++)tft.fillRect(4+40*i,160,30,25,ILI9341_WHITE);
  //Inicializa los números de track
  for(byte i=0;i<4;i++)tft.drawNumber(i+1+trackOffset,13+40*i,157,2);//Pistas
}

//Funciones generales
byte BitRead(byte pos,byte param){//Devuelve un bit de un byte
  return((param>>pos) & 1); //0x8000 = 0b1000 0000 0000 0000, corre 15 para devolver el bit 0  
}
byte BitWrite(byte pos, byte estado,byte param){ //Escribe un bit en una posicion                           
  if(estado==1)param = param | (1<<pos); //0000 0000 -> 0000 0010: 0000 0000 |  1<<2                          //SET
  if(estado==0)param = param & ~(1<<pos);//1111 1111 -> 1111 1101: 1111 1111 & ~1<<2 -> 1111 1111 & 1111 1101 //CLEAR
  return param;
}
/***************************************************************************************
** Function name:           drawRAW
** Descriptions:            draw a 565 format 16 bit raw image file
***************************************************************************************/
#define BUFF_SIZE 40
void drawRAW(char *filename, int16_t x, int16_t y, int16_t rawWidth, int16_t rawHeight) {
  File     rawFile;
  uint8_t  sdbuffer[2 * BUFF_SIZE];   // SD read pixel buffer (16 bits per pixel)

  // Check file exists and open it
  if ((rawFile = SD.open(filename)) == NULL) {
    Serial.println(F("File not found"));
    return;
  }

  tft.setAddrWindow(x, y, x + rawWidth - 1, y + rawHeight - 1);

  // Work out how many whole buffers to send
  uint16_t nr = ((long)rawHeight * rawWidth)/BUFF_SIZE;
  while(nr--) {
    rawFile.read(sdbuffer, sizeof(sdbuffer));
    tft.pushColors(sdbuffer, BUFF_SIZE);
  }
  
  // Send any partial buffer
  nr = ((long)rawHeight * rawWidth)%BUFF_SIZE;
  if (nr) {
    rawFile.read(sdbuffer, nr<<1); // We load  2 x BUFF_SIZE bytes
    tft.pushColors(sdbuffer, nr);      // We send BUF_SIZE pixels
  }
  rawFile.close();
}

//Evento I2C
void receiveEvent (int how_many){
  modo=Wire.read();
  recibido[3]=Wire.read();//Nota on/off
  recibido[4]=Wire.read();//Nota
  recibido[5]=Wire.read();//Velocity  
  newData=1;//Se recibió una nueva trama
}


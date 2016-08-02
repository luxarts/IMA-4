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

#define m_session 0
#define m_mixer 1
#define m_piano 2
#define m_drumrack 3
#define m_editor 4
#define CHANNEL_CLIP 0
#define CHANNEL_DRUM 1

#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#ifdef DEBUG_TIME
long tiempo,tiempoMax,tiempoAct;
#endif

byte BitRead(byte,unsigned int);
void notaOn(byte);
void notaOff(byte);
void mixer(void);
void drumrack(void);
void piano(void);
void editor(void);

byte MXY[5][16]= {                    
                  {0x00,0x01,0x02,0x03,//session
                   0x10,0x11,0x12,0x13,
                   0x20,0x21,0x22,0x23,
                   0x30,0x31,0x32,0x33},
                   
                  {1,1,1,1,//mixer
                   1,1,1,1,
                   1,1,1,1,
                   1,1,1,1},
                   
                  {8,10,255,13,//piano
                   7,9,11,12,
                   1,3,255,6,
                   0,2,4,5},
                   
                  {1,1,1,1,//drumrack
                   1,1,1,1,
                   1,1,1,1,
                   1,1,1,1},

                  {1,1,1,1,//editor
                   1,1,1,1,
                   1,1,1,1,
                   1,1,1,1}
                };

unsigned int e_nuevo[2]={0xFFFF,0xFFFF};
unsigned int e_anterior[2];
byte shift_btn=1;
byte modo;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
  lcd.begin(16, 2);
  lcd.setCursor(16/2-3,0);
  lcd.print("|IMA4|");
  lcd.setCursor(16/2-3,1);
  lcd.print("+----+");
}

void loop() {
#ifdef DEBUG_TIME
  tiempo=micros();
#endif

  byte actualMin=0;
  byte actualMax=0;
    
  e_anterior[0]=e_nuevo[0];
  e_anterior[1]=e_nuevo[1];
      
  Wire.requestFrom(MCU_IN2, 2);    // pide dos bytes 

  delayMicroseconds(1); //Estabiliza
  
  while (Wire.available()) { // mientras el buffer este lleno
    actualMin = Wire.read();
    actualMax = Wire.read();
    e_nuevo[1] = ((actualMax<<8) + actualMin); //Reconstruye el dato
  }
           
  if(e_nuevo[1]!=e_anterior[1]){ //compara bytes anterior y nuevo
    for(byte i=0;i<13;i++){ //compara bit a bit anterior y nuevo      
      if(BitRead(i,e_nuevo[1]) != BitRead(i,e_anterior[1])){
        if(BitRead(i,e_nuevo[1]) == 0){  //Si se presionó
          if(i==0){ //Si es el botón de shift
            shift_btn=!shift_btn;
            lcd.clear();
            Serial.print("Shift: ");
            Serial.print(shift_btn);
            lcd.print("Shift: ");
            lcd.print(shift_btn);
            modo = m_session;//retorna modo session
            Serial.print(" Session");
            Serial.println();
          }
          if((shift_btn!=0 || i>4 ) && i!=0){
            notaOn(i+16);
          }
          else{
            switch(i){
              case 1: mixer();
              break;
              case 2: drumrack();
              break;
              case 3: piano();
              break;
              case 4: editor();
              break;
            }
          }
        }
        if(BitRead(i,e_nuevo[1])==1){
          if((shift_btn!=0 || i>4 ) && i!=0){
            notaOff(i+16);
          } 
        }
      }
    }  
  } 
  Wire.requestFrom(MCU_IN1, 2);    
  delayMicroseconds(1); 
  while (Wire.available()) { 
    actualMin = Wire.read(); 
    actualMax = Wire.read();
    e_nuevo[0] = ((actualMax<<8) + actualMin);
  } 
  if(e_nuevo[0]!=e_anterior[0]){
    for(int i=0;i<16;i++){
      if(BitRead(i,e_nuevo[0])!=BitRead(i,e_anterior[0])){
        if(BitRead(i,e_nuevo[0])==0)notaOn(i);
        if(BitRead(i,e_nuevo[0])==1)notaOff(i);    
      }
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
void notaOn (byte i){
  byte boton;
  
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

    case 16:  boton=100;
    break;
    case 17:  boton=101;
    break;
    case 18:  boton=102;
    break;
    case 19:  boton=103;
    break;
    case 20:  boton=104;
    break;
    case 21:  boton=105;
    break; 
    case 22:  boton=106;
    break;
    case 23:  boton=107;
    break;
    case 24:  boton=108;
    break;
    case 25:  boton=109;
    break;
    case 26: boton=110;
    break;
    case 27: boton=111;
    break;
    case 28: boton=112;
    break;
    case 29: boton=113;
    break;
    case 30: boton=114;
    break;
    case 31: boton=115;
    break;
  }
  //Wire.beginTransmission(MCU_GRAL); //Inicia la transmisión al esclavo MCU_GRAL
  //Wire.write(0x90); //Envía el nombre en el I2C       
  //Wire.write(boton); //Boton presionado
  //Wire.write(127); //Boton presionado
  //Wire.endTransmission(); //Termina la transmisión

#ifdef DEBUG 
  Serial.print("Nota ON: ");
  Serial.print(boton);
  Serial.println();
#else
  Serial.write(0x90+CHANNEL_DRUM);
  Serial.write(boton);
  Serial.write(127);
#endif
}

void notaOff (byte i){
  byte boton;
  
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

    
    case 16:  boton=100;
    break;
    case 17:  boton=101;
    break;
    case 18:  boton=102;
    break;
    case 19:  boton=103;
    break;
    case 20:  boton=104;
    break;
    case 21:  boton=105;
    break; 
    case 22:  boton=106;
    break;
    case 23:  boton=107;
    break;
    case 24:  boton=108;
    break;
    case 25:  boton=109;
    break;
    case 26:  boton=110;
    break;
    case 27:  boton=111;
    break;
    case 28:  boton=112;
    break;
    case 29:  boton=113;
    break;
    case 30:  boton=114;
    break;
    case 31:  boton=115;
    break;
  }
  //Wire.beginTransmission(MCU_GRAL); //Inicia la transmisión al esclavo MCU_GRAL
  //Wire.write(0x80); //Envía el nombre en el I2C       
  //Wire.write(boton); //Boton presionado
  //Wire.write(127); //Boton presionado
  //Wire.endTransmission(); //Termina la transmisión

#ifdef DEBUG   
  Serial.print("Nota OFF: ");
  Serial.print(boton);
  Serial.println();
#else
  Serial.write(0x80+CHANNEL_DRUM);
  Serial.write(boton);
  Serial.write(0);
#endif
}

byte BitRead(byte pos,unsigned int n){//Devuelve un bit de un int
    n=n<<pos;
  return((n&0x8000)>>15); //0x8000 = 0b1000 0000 0000 0000, corre 15 para devolver el bit 0  
}

void mixer(void){
  Serial.print(" Mixer");
  Serial.println();
  lcd.clear();
  lcd.print("Mixer");
  modo = m_mixer;
}

void drumrack(void){
  Serial.print(" Drum Rack");
  Serial.println();
  lcd.clear();
  lcd.print("Drum Rack");
  modo = m_drumrack;
}

void piano(void){
  Serial.print("Piano");
  Serial.println();
  lcd.clear();
  lcd.print("Piano");
  modo = m_piano;
}

void editor(void){
  Serial.print(" Editor");
  Serial.println();
  lcd.clear();
  lcd.print("Editor");
  modo = m_editor;
}   

/*
 *      Interfaz Midi Argentina 4
 * www.github.com/luxarts/IMA-4
 * Versión: 1.0
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


// Valores de medida
byte anterior[7] = {1,1,1,1,1,1,1};

int pote[4];

byte ccAnt[4]={128,128,128,128};
byte cc_data[11]={0,0,0,0,0,0,0,128,128,128,128};//Primeros 7 Encoders(Contadores), otros 4 potes(valor mapeado)
byte paso[7]={0,0,0,0,0,0,0};

#ifdef DEBUG_TIME
long tiempo,tiempoMax,tiempoAct;
#endif

void setup(){
  Wire.begin(MCU_CC1);
  Wire.setClock(400000L);
  digitalWrite(SDA,0);
  digitalWrite(SCL,0);
  //Wire.onRequest(requestEvent); //Interrupción al recibir un dato
  Serial.begin(115200);
  for(byte i=0;i<14;i++){
    pinMode(i, INPUT); //Entradas del encoder N° par = A, N° impar = B
    digitalWrite(i,HIGH); //Pull Up
  }
  //for(byte i=0;i<4;i++)pote_anterior[i]=analogRead(i);
}
 
void loop()
{
  // Lectura de A, si es 0, volvemos a medir para asegurar el valor
  byte actual;
  byte x=0;

#ifdef DEBUG_TIME
  tiempo=micros();
#endif

  for(byte i=0;i<14;i+=2){    
    actual = digitalRead(i);
    
    if(actual == 0){ //Anti rebote
      actual = digitalRead(i);
    } 
    // Actuamos únicament en el flanco descendente
    if(anterior[x] == 1 && actual == 0){//Si recibe un pulso
      
      if(digitalRead(i+1) == 1){ //Sentido de giro
        if(!paso[x])cc_data[x]++;//Si B=1, aumentamos
        else cc_data[x]+=5;
        if(cc_data[x]>127)cc_data[x]=127;
      }
      else{
        if(!paso[x])cc_data[x]--;// Si B = 0, reducimos
        else cc_data[x]-=5;
        if(cc_data[x]>245)cc_data[x]=0;//Overflow
      }
#ifdef DEBUG       
      // Escribimos el valor en pantalla
      /*
      Serial.print("Contador ");
      Serial.print(x);
      Serial.print(": ");
      Serial.print(cc_data[x]);
      Serial.println();       
      */
#endif
    }
    anterior[x] = actual;
    x++;
  }
//FIN ENCODERS

  for(byte i=0;i<4;i++){
    pote[i]=analogRead(i);//1er Lectura del puerto analogico
    for(byte x=0;x<100;x++){
      pote[i]+=analogRead(i);// 10 Lecturas + del puerto analogico
      pote[i]>>=1;
    }
    cc_data[i+7]=map(pote[i],0,1023,0,127);//Mapeo entre 0 y 1023 a 0 y 127
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

void requestEvent(){
  
  for(byte i=0;i<4;i++){
    if(cc_data[i+7]!=ccAnt[i]){
      ccAnt[i]=cc_data[i+7];
    }
    else{
      cc_data[i+7]=255;
    }
  }
  Wire.write(cc_data,11);
}


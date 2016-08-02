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

// Valores de medida
byte anterior[9] = {1,1,1,1,1,1,1,1,1};
byte contador[9] = {0,0,0,0,0,0,0,0,0};
byte x=0;

#ifdef DEBUG_TIME
long tiempo,tiempoMax,tiempoAct;
#endif

void setup(){
  // Inicializamos la lectura del encoder
  for(byte i=2;i<20;i++){
    pinMode(i, INPUT);
    digitalWrite(i,HIGH);
  }
  Serial.begin(115200);
}
 
void loop()
{
  // Lectura de A, si es 0, volvemos a medir para asegurar el valor
  byte actual;
  x =0;

#ifdef DEBUG_TIME
  tiempo=micros();
#endif

  for(byte i=2;i<19;i+=2){    
    actual = digitalRead(i);
    
    if(actual == 0){ //Anti rebote
      actual = digitalRead(i);
    } 
    // Actuamos únicament en el flanco descendente
    if(anterior[x] == 1 && actual == 0){ //Si recibe un pulso
      
      if(digitalRead(i+1) == 1){ //Sentido de giro
        if(contador[x]<127)contador[x]++;  // Si B = 1, aumentamos el contador
      }
      else{
        if(contador[x]>0)contador[x]--;  // Si B = 0, reducimos el contador
      }
      
      // Escribimos el valor en pantalla
      Serial.print("Contador ");
      Serial.print(x+1);
      Serial.print(": ");
      Serial.print(contador[x]);
      Serial.println();
      
    }
    anterior[x] = actual;
    x++;
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

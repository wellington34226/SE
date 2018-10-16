#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

//LCD I2C for display delay LED and state
LiquidCrystal_I2C i2cLcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);
//Servo:
Servo servoMotor;
//interval | oldInfrared | atualizarLcd | oldMotor
uint8_t states = B11111000;
int32_t vibrationValue = 0;
uint64_t timeAction;//para manter um intervalo entre cada operação verificada

void displayPrint(void){
  PORTB ^= (1 << PB5);//faz LED_BUILTIN piscar
  String msg = "VB:";
  if(vibrationValue < 100000){
    msg += "0";
  }
  if(vibrationValue < 10000){
    msg += "0";
  }
  if(vibrationValue < 1000){
    msg += "0";
  }
  if(vibrationValue < 100){
    msg += "0";
  }
  if(vibrationValue < 10){
    msg += "0";
  }
  msg += vibrationValue;
  msg += "Hz|";
  if(states & B00000100){
    msg += " IR:SIM ";
  }else{
    msg += " IR:NAO ";
  }
  i2cLcd.setCursor(0,3);
  i2cLcd.print(msg);
  Serial.print(msg);
  Serial.println();
  PORTB ^= (1 << PB5);//faz LED_BUILTIN piscar
}

void setup() {
  DDRD  &= ~(1 << PD2);PORTD &= ~(1 << PD2);//pinMode(2,INPUT);//infrared sensor
  DDRD  &= ~(1 << PD3);PORTD &= ~(1 << PD3);//pinMode(3,INPUT);//vibration sensor
  DDRB  |=  (1 << PB5);PORTB &= ~(1 << PB5);//pinMode(13,OUTPUT);digitalWrite(13,LOW);//LED_BUILTIN
  ACSR   = 0;//ativar comparador analógico
  servoMotor.attach(9);//define servo motor PWM no pino 9
  i2cLcd.begin(20,4);
  i2cLcd.clear();
  Serial.begin(115200);
  i2cLcd.setCursor(0,0);
  i2cLcd.print(" Wellington Rodrigo ");
  Serial.print(" Wellington Rodrigo ");
  Serial.println();
  i2cLcd.setCursor(0,1);
  i2cLcd.print(" Software Embarcado ");
  Serial.print(" Software Embarcado ");
  Serial.println();
  i2cLcd.setCursor(0,2);
  i2cLcd.print("     TAREFA  03     ");
  Serial.print("     TAREFA  03     ");
  Serial.println();
  displayPrint();
  servoMotor.write(0);
  timeAction = millis();
}

void loop() {

  if(millis() >= timeAction+((states & B11111100) << 1)){
    timeAction = millis();
    uint8_t curStates = 0;
    //Detectando obstáculo com sensor infravermelho
    curStates = (~PIND & B00000100);//máscara para pegar o pino PD2 Infrared
    if(curStates ^ (states & (1 << 2))){
      states ^= (1 << 2);//atualiza somente se o valor mudou
      states |= (1 << 1);//habilita atualização do display
    }
    //Detectando vibrações
    int32_t vibration = vibrationValue;
    vibrationValue = pulseIn(3,HIGH);
  	if(vibrationValue != vibration){
      states |= (1 << 1);//habilita atualização do display
  	}
    if(vibrationValue >=10000){
      curStates |= (1 << 1);//se maior que 10000 ativa bit indicando sua ativação
    }
    //verifica comparador analógico.
    if(~ACSR & (1 << ACO)){
      curStates |= (1 << 0);//ativa bit indicando sua ativação
    }
    //verifica se precisa atualizar o display
    if(states & (1 << 1)){
      displayPrint();
      states &= ~(1 << 1);
    }
    //verifica se o motor deve ser acionado
    if(curStates > 0){
      states ^= (1 << 0);
      if(states & (1 << 0)){
       servoMotor.write(180);
      }else{
       servoMotor.write(0);
      }
    }
  }
}

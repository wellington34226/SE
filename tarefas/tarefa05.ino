/* Cáculo para estouro do timer
 * Estouro = Timer2_cont x prescaler x ciclo de máquina
 * Ciclo de máquina = 1/Fosc = 1/16E6 = 62,5ns = 62,5E-9s
 * Estouro = 156 x 1024 x 62,5E-9 = 9,984000
*/
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//LCD I2C for display delay LED and states
LiquidCrystal_I2C i2cLcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);

uint64_t timeClock;
uint64_t timeButtonHora;
uint64_t timeButtonMinuto;
uint64_t holdButtons;
//interval | SET CLOCK | oldStateMinuto | oldStateHora
uint8_t states   = B11111000;
uint8_t hora     = 0;
uint8_t minuto   = 0;
uint8_t segundo  = 0;
uint8_t msegundo = 0;//max 100*10

void displayPrint(void){
  PORTB ^= (1 << PB5);//faz LED_BUILTIN piscar
  String msg=" ";
  if(hora < 10){
    msg += "0";
  }
  msg += hora;
  msg += ":";
  if(minuto < 10){
    msg += "0";
  }
  msg += minuto;
  msg += ":";
  if(segundo < 10){
    msg += "0";
  }
  msg += segundo;
  if(states & (1 << 2)){
    msg += "  SETTING  ";
  }else{
    msg += ":";
    if(msegundo < 100){
      msg += "0";
    }
    if(msegundo < 10){
      msg += "0";
    }
    msg += msegundo;
    msg += "0";
  }
  i2cLcd.setCursor(0,3);
  i2cLcd.print(msg);
  Serial.print(msg);
  Serial.println();
  PORTB ^= (1 << PB5);//faz LED_BUILTIN piscar
}

void setup() {
  DDRD  &= ~(1 << PD2);PORTD &= ~(1 << PD2);//pinMode(2,INPUT);//botao da hora
  DDRD  &= ~(1 << PD3);PORTD &= ~(1 << PD3);//pinMode(4,INPUT);//botao do minuto
  DDRB  |=  (1 << PB5);PORTB &= ~(1 << PB5);//pinMode(13,OUTPUT);digitalWrite(13,LOW);//LED_BUILTIN
  //Timer operando em modo CTC e Set OC1A COM1A:11 -
  //Compare Output Mode COM 11=3
  TCCR1A |=  (1 << COM1A0);
  TCCR1A |=  (1 << COM1A1);
  //Waveform Generation Mode WGM 010=2
  TCCR1A &= ~(1 << WGM10);
  TCCR1A |=  (1 << WGM11);
  TCCR1B &= ~(1 << WGM12);

  //definindo prescaler para 1024 CS:101
  TCCR1B |=  (1 << CS10);
  TCCR1B &= ~(1 << CS11);
  TCCR1B |=  (1 << CS12);
  //Definindo valor limite para comparação
  OCR1A = 156; //contagem -->> 156*1024*62,5/1000000000 = 0,009984s = 9,984ms

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
  i2cLcd.print("     TAREFA  05     ");
  Serial.print("     TAREFA  05     ");
  Serial.println();
  displayPrint();
  //Limpa a flag Output Compare A Match Flag
  TIFR1  |=  (1 << OCF1A);
  timeClock = millis();
}

void loop() {

  //buttons control
	uint8_t curButtons = (PIND & B00001100) >> 2;//máscara para pegar os pinos PD2 e PD3
	switch(states & B00000011){
    case 0://HORA OFF | MINUTO OFF
			switch(curButtons){
				case 3:
					if(millis() >= timeButtonHora+((states & B11111000) >> 1) && millis() >= timeButtonHora+((states & B11111000) >> 1)){
						holdButtons = millis();
						states ^= (1 << 0);
						states ^= (1 << 1);
					}
					break;
				case 2:
					if(millis() >= timeButtonHora+((states & B11111000) >> 1)){
						timeButtonMinuto = millis();
            if(states & (1 << 2)){
              minuto++;
              if(minuto > 59){
                minuto = 0;
              }
						  displayPrint();
            }
            states ^= (1 << 1);
					}
					break;
				case 1:
					if(millis() >= timeButtonHora+((states & B11111000) >> 1)){
						timeButtonHora = millis();
            if(states & (1 << 2)){
              hora++;
              if(hora > 23){
                hora = 0;
              }
              displayPrint();
            }
						states ^= (1 << 0);
					}
					break;
			};
      break;
    case 1://HORA ON  | MINUTO OFF
			switch(curButtons){
				case 3:
					if(millis() >= timeButtonHora+((states & B11111000) >> 1) && millis() >= timeButtonHora+((states & B11111000) >> 1)){
						holdButtons = millis();
						states ^= (1 << 1);
					}
					break;
				case 2:
					if(millis() >= timeButtonHora+((states & B11111000) >> 1)){
						timeButtonMinuto = millis();
            if(states & (1 << 2)){
              minuto++;
              if(minuto > 59){
                minuto = 0;
              }
  						displayPrint();
            }
	          states ^= (1 << 0);
						states ^= (1 << 1);
					}
					break;
				case 1:
					if(millis() >= timeButtonHora+(states & B11111000)){
            timeButtonHora = millis();
            if(states & (1 << 2)){
              hora++;
              if(hora > 23){
                hora = 0;
              }
  						displayPrint();
            }
          }
					break;
				case 0:
					states ^= (1 << 0);
					break;
			};
      break;
    case 2://HORA OFF | MINUTO ON
			switch(curButtons){
				case 3:
					if(millis() >= timeButtonHora+((states & B11111000) >> 1) && millis() >= timeButtonHora+((states & B11111000) >> 1)){
						holdButtons = millis();
						states ^= (1 << 0);
					}
					break;
				case 2:
          if(millis() >= timeButtonMinuto+(states & B11111000)){
            timeButtonMinuto = millis();
            if(states & (1 << 2)){
              minuto++;
              if(minuto > 59){
                minuto = 0;
              }
  						displayPrint();
            }
          }
					break;
				case 1:
					if(millis() >= timeButtonMinuto+((states & B11111000) >> 1)){
						timeButtonHora = millis();
            if(states & (1 << 2)){
              hora++;
              if(hora > 23){
                hora = 0;
              }
  						displayPrint();
            }
	          states ^= (1 << 0);
						states ^= (1 << 1);
					}
					break;
				case 0:
					states ^= (1 << 1);
					break;
			};
      break;
    case 3://HORA ON  | MINUTO ON
			switch(curButtons){
				case 0:
					if(millis() >= holdButtons+ ((states & B11111000) << 1)){
	          states ^= (1 << 2);
						displayPrint();
	        }
					states ^= (1 << 0);
					states ^= (1 << 1);
          timeClock        = millis();
          timeButtonHora   = millis();
          timeButtonMinuto = millis();
					break;
				case 1:
					if(millis() >= holdButtons+ ((states & B11111000) << 1)){
	          states ^= (1 << 2);
						displayPrint();
            timeClock = millis();
	        }
					states ^= (1 << 0);
          timeClock        = millis();
          timeButtonHora   = millis();
          timeButtonMinuto = millis();
					break;
				case 2:
					if(millis() >= holdButtons+ ((states & B11111000) << 1)){
	          states ^= (1 << 2);
						displayPrint();
            timeClock = millis();
	        }
					states ^= (1 << 1);
          timeClock        = millis();
          timeButtonHora   = millis();
          timeButtonMinuto = millis();
					break;
			};
			break;
  };
  if(TIFR1 & (1 << OCF1A)){
    //Limpa a flag Output Compare A Match Flag
    TIFR1  |=  (1 << OCF1A);
    msegundo++;
    if(msegundo > 99){
      msegundo = 0;
      segundo++;
    }
    if(segundo > 59){
      segundo = 0;
      minuto++;
    }
		if(minuto > 59){
			minuto = 0;
			hora++;
		}
		if(hora > 23){
			hora = 0;
		}
    displayPrint();
  }
  // if(~states & (1 << 2) && millis() >= timeClock+1000){
  //   timeClock = millis();
  //   segundo++;
  //   if(segundo >59){
  //     segundo = 0;
  //     minuto++;
  //   }
	// 	if(minuto > 59){
	// 		minuto = 0;
	// 		hora++;
	// 	}
	// 	if(hora > 23){
	// 		hora = 0;
	// 	}
	// 	displayPrint();
  // }
}

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//LCD I2C for display delay LED and state
LiquidCrystal_I2C i2cLcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);
//debaunce | LOCK LED | oldStateUp | oldStateDown
uint8_t states = B11111000;
uint64_t timeLed;
uint16_t delayLed = 1000;//milliseconds
uint64_t timeButtonDown;
uint64_t timeButtonUp;
uint64_t holdButtons;

void displayPrint(void){
	String 	msg = "";
  if(states & (1 << 2)){
    msg += "    LOCK LED: ";
    if(PINB & (1 << PB0)){
      msg += "ON    ";
    }else{
      msg += "OFF   ";
    }
  }else{
    msg += "DELAY LED: ";
  	if(delayLed < 10000){
  		msg += "0";
  	}
  	if(delayLed < 10000){
  		msg += "0";
  	}
  	if(delayLed < 1000){
  		msg += "0";
  	}
  	if(delayLed < 100){
  		msg += "0";
  	}
  	if(delayLed < 10){
  		msg += "0";
  	}
  	msg += delayLed;
  	msg += "ms ";
  }
  i2cLcd.setCursor(0,3);
  i2cLcd.print(msg);
  Serial.print(msg);
  Serial.println();
}

void setup() {
  DDRD  &= ~(1 << PD2);PORTD &= ~(1 << PD2);//pinMode(2,INPUT);//button DOWN delay LED
  DDRD  &= ~(1 << PD4);PORTD &= ~(1 << PD4);//pinMode(4,INPUT);//button UP delay LED
	DDRB  &= ~(1 << PB0);PORTB &= ~(1 << PB0);//pinMode(8,OUTPUT);digitalWrite(8,LOW);//set LED to LOW
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
  i2cLcd.print("     TAREFA  01     ");
  Serial.print("     TAREFA  01     ");
  Serial.println();
  displayPrint();
	timeLed = millis();
}

void loop() {
  while(states & (1 << 2));//caso seja setado para true trava a execução
  if(millis() >= timeLed+delayLed){
    timeLed = millis();
		//faz o LED piscar
		PORTB ^= (1 << PB0);//ledState = ~ledState;digitalWrite(8,ledState);
  }
  //buttons control
	uint8_t curButtons = (PIND & B00001100) >> 2;//máscara para pegar os pinos PD2 e PD3
	switch(states & B00000011){
    case 0://DOWN OFF | UP OFF
			switch(curButtons){
				case 3:
					if(millis() >= timeButtonDown+((states & B11111000) >> 2) && millis() >= timeButtonDown+((states & B11111000) >> 2)){
						holdButtons = millis();
						states ^= (1 << 0);
						states ^= (1 << 1);
					}
					break;
				case 2:
					if(millis() >= timeButtonDown+((states & B11111000) >> 2)){
						timeButtonUp = millis();
						if(delayLed < 10000){
							delayLed += 100;
						}
						displayPrint();
						states ^= (1 << 1);
					}
					break;
				case 1:
					if(millis() >= timeButtonDown+((states & B11111000) >> 2)){
						timeButtonDown = millis();
						if(delayLed > 100){
							delayLed -= 100;
						}
						displayPrint();
						states ^= (1 << 0);
					}
					break;
			};
      break;
    case 1://DOWN ON  | UP OFF
			switch(curButtons){
				case 3:
					if(millis() >= timeButtonDown+((states & B11111000) >> 2) && millis() >= timeButtonDown+((states & B11111000) >> 2)){
						holdButtons = millis();
						states ^= (1 << 1);
					}
					break;
				case 2:
					if(millis() >= timeButtonDown+((states & B11111000) >> 2)){
						timeButtonUp = millis();
						if(delayLed < 10000){
							delayLed += 100;
						}
						displayPrint();
	          states ^= (1 << 0);
						states ^= (1 << 1);
					}
					break;
				case 1:
					if(millis() >= timeButtonDown+(states & B11111000)){
            timeButtonDown = millis();
	          if(delayLed > 100){
							delayLed -= 100;
						}
						displayPrint();
          }
					break;
				case 0:
					states ^= (1 << 0);
					break;
			};
      break;
    case 2://DOWN OFF | UP ON
			switch(curButtons){
				case 3:
					if(millis() >= timeButtonDown+((states & B11111000) >> 2) && millis() >= timeButtonDown+((states & B11111000) >> 2)){
						holdButtons = millis();
						states ^= (1 << 0);
					}
					break;
				case 2:
          if(millis() >= timeButtonUp+(states & B11111000)){
            timeButtonUp = millis();
	          if(delayLed < 10000){
							delayLed += 100;
						}
						displayPrint();
          }
					break;
				case 1:
					if(millis() >= timeButtonUp+((states & B11111000) >> 2)){
						timeButtonDown = millis();
						if(delayLed < 10000){
							delayLed -= 100;
						}
						displayPrint();
	          states ^= (1 << 0);
						states ^= (1 << 1);
					}
					break;
				case 0:
					states ^= (1 << 1);
					break;
			};
      break;
    case 3://DOWN ON  | UP ON
			switch(curButtons){
				case 0:
					if(millis()>=holdButtons+500){
	          states |= (1 << 2);
						displayPrint();
	        }
					states ^= (1 << 0);
					states ^= (1 << 1);
					break;
				case 1:
					if(millis()>=holdButtons+500){
	          states |= (1 << 2);
						displayPrint();
	        }
					states ^= (1 << 0);
					break;
				case 2:
					if(millis()>=holdButtons+500){
	          states |= (1 << 2);
						displayPrint();
	        }
					states ^= (1 << 1);
					break;
			};
			break;
  };
}

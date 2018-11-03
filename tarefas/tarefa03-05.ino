/* Cáculo para estouro do timer
 * Estouro = Timer1_cont x prescaler x ciclo de máquina
 * Ciclo de máquina = 1/Fosc = 1/16E6 = 62,5ns = 62,5E-9s
 * Estouro = 15625 x 1024 x 62,5E-9 = 1s
*/
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#define SETTING "SETTING"
#define NORMAL  "       "
#define TELA 5
//LCD I2C for display delay LED and states
LiquidCrystal_I2C i2cLcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);
//Servo:
Servo servoMotor;//usa o timer1 necessário modificar para usar outro timer
uint64_t timeButtonHora;
uint64_t timeButtonMinuto;
uint64_t holdButtons;
uint64_t timeMotor;
//intervalo | SET CLOCK | oldStateMinuto | oldStateHora | oldMotor
uint8_t states  = 0x00;
//Cada digito do Relógio
uint8_t seg     = 0;
uint8_t segundo = 0;
uint8_t telaOn  = TELA;
                //  UNIM|DEZM|UNIH|DEZH
uint8_t clock[4] = {0x00,0x00,0x00,0x00};

void displayPrint(){
  String msg = "";
  msg += clock[3];
  msg += clock[2];
  msg += ":";
  msg += clock[1];
  msg += clock[0];
  i2cLcd.setBacklight(telaOn);
  i2cLcd.setCursor(2,3);
  i2cLcd.print(msg);
  Serial.print("  ");
  Serial.print(msg);
  msg = "";
  if(states & (1 << 3)){
    msg += SETTING;
  }else{
    msg += NORMAL;
  }
  i2cLcd.setCursor(10,3);
  i2cLcd.print(msg);
  Serial.print("    ");
  Serial.print(msg);
  Serial.println();
  PORTB &= ~(1 << PB5);//apaga LED_BUILTIN
}

void setup() {
  DDRD  &= ~(1 << PD2);PORTD &= ~(1 << PD2);//pinMode(2,INPUT);//botao da hora
  DDRD  &= ~(1 << PD3);PORTD &= ~(1 << PD3);//pinMode(3,INPUT);//botao do minuto
  DDRD  &= ~(1 << PD4);PORTD &= ~(1 << PD4);//pinMode(4,INPUT);//infrared sensor
  DDRD  &= ~(1 << PD5);PORTD &= ~(1 << PD5);//pinMode(5,INPUT);//vibration sensor
  DDRB  |=  (1 << PB5);PORTB |=  (1 << PB5);//pinMode(13,OUTPUT);digitalWrite(13,HIGH);//LED_BUILTIN
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
  i2cLcd.print("  TAREFA  02/03/05  ");
  Serial.print("  TAREFA  02/03/05  ");
  Serial.println();
  displayPrint();
  servoMotor.write(0);
  timeMotor = millis();
  //Timer operando em modo CTC e Com saídas OC1A e OC1B desconectadas
  //Compare Output Mode COM1A = 00 e WGM = 12 = 0xC
  //TIMER COUNTER CONTROL REGISTER A >>>  COM2A|COM2A|COM2B|COM2B|     |     |WGM21|WGM20
  //TIMER COUNTER CONTROL REGISTER B >>>  FOC2A|FOC2B|     |     |WGM22| CS22| CS21| CS20
  //TIMER INTERRUPTION MASK          >>>       |     |     |     |     |OCIEB|OCIEA| TOIE

  TCCR2A  = 0x02;//Modo CTC          >>>    0  |  0  |  0  |  0  |  0  |  0  |  1  |  0
  TCCR2B  = 0x07;// Prescaler 1024   >>>    0  |  0  |  0  |  0  |  0  |  1  |  1  |  1
  //Definindo valor limite para comparação
  OCR2A  = 0x7D;//contagem -->> 125*(125*1024*62,5/1000000000) = 1s
  TCNT2  = 0x00;//iniciando contador
  TIMSK2 = 0x02;//Habilita interrupção do Comparador da Saída A
}

void loop() {
  if(PINB & (1 << PB5)){
    displayPrint();
  }else{
    //buttons control
  	uint8_t curButtons = (PIND & 0x0C) >> 2;//máscara para pegar os pinos PD2 e PD3
  	switch((states >> 1) & 0x03){
      case 0://HORA OFF | MINUTO OFF
  			switch(curButtons){
  				case 3://HORA ON | MINUTO ON
  					if(millis() >= timeButtonHora+0xFA && millis() >= timeButtonMinuto+0xFA){
  						holdButtons = millis();
  						states ^= (1 << 1);
  						states ^= (1 << 2);
              if(telaOn == 0){
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
  					}
  					break;
  				case 2://HORA OFF | MINUTO ON
  					if(millis() >= timeButtonMinuto+0xFA){
  						timeButtonMinuto = millis();
              if(states & (1 << 3)){
                clock[0]++;
                if(clock[0] > 9){
                  clock[0] = 0;
                  clock[1]++;
                  if(clock[1] > 5){
                    clock[1] = 0;
                  }
                  if(clock[1] > 5){
                    clock[1] = 0;
                  }
                }
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
              if(telaOn == 0){
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
              states ^= (1 << 2);
  					}
  					break;
  				case 1://HORA ON | MINUTO OFF
  					if(millis() >= timeButtonHora+0xFA){
  						timeButtonHora = millis();
              if(states & (1 << 3)){
                clock[2]++;
                if(clock[3] > 1 && clock[2] > 3){
                  clock[2] = 0;
                  clock[3] = 0;
                }else{
                  if(clock[2] > 9){
                    clock[2] = 0;
                    clock[3]++;
                  }
                }
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
              if(telaOn == 0){
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
  						states ^= (1 << 1);
  					}
  					break;
  			};
        break;
      case 1://HORA ON  | MINUTO OFF
  			switch(curButtons){
  				case 3://HORA ON | MINUTO ON
  					if(millis() >= timeButtonHora+0xFA && millis() >= timeButtonMinuto+0xFA){
  						holdButtons = millis();
              if(telaOn == 0){
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
  						states ^= (1 << 2);
  					}
  					break;
  				case 2://HORA OFF | MINUTO ON
  					if(millis() >= timeButtonMinuto+0xFA){
  						timeButtonMinuto = millis();
              if(states & (1 << 3)){
                clock[2]++;
                if(clock[2] > 9){
                  clock[2] = 0;
                  clock[3]++;
                }
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
              if(telaOn == 0){
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
  	          states ^= (1 << 1);
  						states ^= (1 << 2);
  					}
            telaOn = 1;
  					break;
  				case 1://HORA ON | MINUTO OFF
  					if(millis() >= timeButtonHora+(0xFA << 1)){
              timeButtonHora = millis();
              if(states & (1 << 3)){
                clock[2]++;
                if(clock[3] > 1 && clock[2] > 3){
                  clock[2] = 0;
                  clock[3] = 0;
                }else{
                  if(clock[2] > 9){
                    clock[2] = 0;
                    clock[3]++;
                  }
                }
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
              if(telaOn == 0){
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
            }
  					break;
  				case 0:
  					states ^= (1 << 1);
  					break;
  			};
        break;
      case 2://HORA OFF | MINUTO ON
  			switch(curButtons){
  				case 3:
  					if(millis() >= timeButtonHora+0xFA && millis() >= timeButtonMinuto+0xFA){
  						holdButtons = millis();
              if(telaOn == 0){
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
  						states ^= (1 << 1);
  					}
  					break;
  				case 2:
            if(millis() >= timeButtonMinuto+(0xFA << 1)){
              timeButtonMinuto = millis();
              if(states & (1 << 3)){
                clock[0]++;
                if(clock[0] > 9){
                  clock[0] = 0;
                  clock[1]++;
                  if(clock[1] > 5){
                    clock[1] = 0;
                  }
                }
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
              if(telaOn == 0){
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
            }
  					break;
  				case 1:
  					if(millis() >= timeButtonHora+0xFA){
  						timeButtonHora = millis();
              if(states & (1 << 3)){
                clock[2]++;
                if(clock[3] > 1 && clock[2] > 3){
                  clock[2] = 0;
                  clock[3] = 0;
                }else{
                  if(clock[2] > 9){
                    clock[2] = 0;
                    clock[3]++;
                  }
                }
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
              if(telaOn == 0){
                telaOn = TELA;
                PORTB |=  (1 << PB5);//acende LED_BUILTIN
              }
  	          states ^= (1 << 1);
  						states ^= (1 << 2);
  					}
  					break;
  				case 0:
  					states ^= (1 << 2);
  					break;
  			};
        break;
      case 3://HORA ON  | MINUTO ON
  			switch(curButtons){
  				case 0:
  					if(millis() >= holdButtons+ (0xFA << 2)){
  	          states ^= (1 << 3);
              if(!(states & (1 << 3))){
                segundo = 0;
              }
              PORTB |=  (1 << PB5);//acende LED_BUILTIN
  	        }
  					states ^= (1 << 1);
  					states ^= (1 << 2);
            timeButtonHora   = millis();
            timeButtonMinuto = millis();
  					break;
  				case 1:
  					if(millis() >= holdButtons+ (0xFA << 2)){
  	          states ^= (1 << 3);
              if(!(states & (1 << 3))){
                segundo = 0;
              }
              PORTB |=  (1 << PB5);//acende LED_BUILTIN
  	        }
            if(telaOn == 0){
              telaOn = TELA;
              PORTB |=  (1 << PB5);//acende LED_BUILTIN
            }
  					states ^= (1 << 1);
            timeButtonHora   = millis();
            timeButtonMinuto = millis();
  					break;
  				case 2:
  					if(millis() >= holdButtons+ (0xFA << 2)){
  	          states ^= (1 << 3);
              if(!(states & (1 << 3))){
                segundo = 0;
              }
              PORTB |=  (1 << PB5);//acende LED_BUILTIN
  	        }
            if(telaOn == 0){
              telaOn = TELA;
              PORTB |=  (1 << PB5);//acende LED_BUILTIN
            }
  					states ^= (1 << 2);
            timeButtonHora   = millis();
            timeButtonMinuto = millis();
  					break;
  			};
  			break;
    };
    if(~PIND & (1 << PD4)){
      states |= (1 << 0);//ativa bit para ativação do motor
    }
    //Detectando vibrações
    int32_t vibration = pulseIn(5,HIGH);
    if(vibration >= 10000){
      states |= (1 << 0);//ativa bit para ativação do motor
    }
    //verifica comparador analógico.
    if(~ACSR & (1 << ACO)){
      states |= (1 << 0);//ativa bit para ativação do motor
    }
    //Ação do motor a cada 500ms
    if(states & (1 << 0) && millis() >= timeMotor+(0xFA << 1)){
      timeMotor = millis();
      if(servoMotor.read()){
        servoMotor.write(0);
      }else{
        servoMotor.write(180);
      }
      if(telaOn == 0){
        telaOn = TELA;
        PORTB |=  (1 << PB5);//acende LED_BUILTIN
      }
      states &= ~(1 << 0);
    }
  }
}
//Rotina de interrupção para contagem dos segundos e controle do relógio
ISR(TIMER2_COMPA_vect){
  TCNT2 = 0x00;// Reinicializa o registrador do Timer2
  seg++;
  if(seg > 124){
    seg = 0;
    segundo++;
    if(segundo > 59){
      segundo = 0;
      if(telaOn){
        telaOn--;
        if(!telaOn){
          PORTB |=  (1 << PB5);//acende LED_BUILTIN}
        }
      }
      if(!(states & (1 << 3))){
        switch (clock[0]){
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
          //unidades dos minutos
          clock[0]++;
          break;
        default:
          switch (clock[1]){
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
              //dezenas dos minutos
              clock[0]=0;
              clock[1]++;
              break;
            default:
              clock[0] = 0;//zera minutos
              clock[1] = 0;
              switch (clock[2]){
                case 0:
                case 1:
                case 2:
                case 3:
                  if(clock[3] > 1){
                    clock[2] = 0;
                    clock[3] = 0;
                    break;
                  }//verifica se maior que 23 horas
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                  //unidades das horas
                  clock[2]++;
                  break;
                default:
                  switch (clock[3]){
                    clock[2] = 0;
                    clock[3]++;//dezenas das horas
                    break;
                  };
                  break;
              };
              break;
          };
          break;
        };
        PORTB |=  (1 << PB5);//acende LED_BUILTIN}
      }
    }
  }
}

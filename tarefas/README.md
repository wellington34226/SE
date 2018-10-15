# Tarefa 01: LED e 2 Botões
* Pisca LED a cada intervalo de tempo (inicialmente a cada um segundo).
* Dois botões incrementam/decrementam em 100ms o intervalo entre cada piscada.
  * A ação dos botões é executada somente ao pressionar e não ao soltar.
Um debaunce de 62ms foi adicionado para evitar flutuações e deixar a execução mais estável. Caso o botão seja mantido pressionado a ação correspondente é repetida a cada 248ms.
* Caso os dois botões sejam mantidos pressionandos por mais de 500ms a execução é parada, mantendo o último estado do LED.
Cada mudança no intervalo entre cada piscada é enviado via serial e apresentado o novo intervalo em uma tela LCD.

# Tarefa 02: 2 Sensores e 2 Atuadores com acesso direto as portas GPIO
* Sensor Infravermelho para detecção de obstáculos, no caso é usado para detectar se o usuário passou a mão por cima do sensor.
* Sensor de vibração, permitindo verificar se o dispositivo foi movimentado.
* Tela LCD para manter o usuário informado do estado dos sensores.
* Servo motor de 180° controlado por PWM.
 * Quando um o sensor infravermelho detecta um obstáculo ou o sensor de vibração verifica uma oscilação maior que 10000 o motor é acionado.

# Tarefa 03: Conversor ou Comparador Analógico
* Foi acrescentado a Tarefa 02 a funcionalidade de utilizar o comparador analógico utilizando um sensor de força resistivo.
 * Diminui sua resistência conforme a força aplicada sobre ele aumenta.
* Utilizando o comparador análogico:
 * A fonte de referência do comparador analógico (pino 6) foi ligada a saída 3,3vcc e o comparador (pino 7) foi ligado a
# Tarefa 05: Timers

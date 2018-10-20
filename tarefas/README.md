### Tarefa 01: LED e 2 Botões
* Pisca LED a cada intervalo de tempo (inicialmente a cada um segundo).
* Dois botões incrementam/decrementam em 100ms o intervalo entre cada piscada.
  * A ação dos botões é executada somente ao pressionar e não ao soltar.
Um debaunce de 62ms foi adicionado para evitar flutuações e deixar a execução mais estável. Caso o botão seja mantido pressionado a ação correspondente é repetida a cada 248ms.
* Caso os dois botões sejam mantidos pressionandos por mais de 500ms a execução é parada, mantendo o último estado do LED.
Cada mudança no intervalo entre cada piscada é enviado via serial e apresentado o novo intervalo em uma tela LCD.

### Tarefa 02: 2 Sensores e 2 Atuadores com acesso direto as portas GPIO
* Sensor Infravermelho para detecção de obstáculos, usado para detectar se o usuário passou a mão por cima do sensor.
* Sensor de vibração, permite verificar se o equipamento sofreu alguma alteção fisica.
* Tela LCD para manter o usuário informado do estado dos sensores.
* Servo motor de 180° controlado por PWM.
 * Quando o sensor infravermelho detecta um obstáculo ou o sensor de vibração verifica uma oscilação maior que 10000 o motor é acionado.

### Tarefa 03: Conversor ou Comparador Analógico
* Uso do comparador analógico acrescentado a Tarefa 02 (conexões anteriores mantidas e código atualizado)
* Sensor de força utilizado com o comparador analógico
 * Quando pressionado também aciona o motor

### Tarefa 05: Timers
* Relógio digital usando Timer no modo CTC
* Dois botões permitem ajustar a hora e o minuto
 * Segurando os dois botôes ao mesmo tempo por mais de 1s o modo de operação do relógio é alternado entre modo normal (relógio controlado pelo timer) e modo de configuração (relógio controlado pelos botões).
* A tela fica acessa por 5 minutos, após esse tempo ela se apaga para economizar energia. A tela volta a acender caso um dos botões seja pressionado.

### Tarefa 02/03/05: Junção das tarefas 02, 03 e 05
* Sensores de Infravermelho e Vibração.
* Sensor de força resistivo usado com o comparador analógico.
* Relógio digital com uso de timers.
* Dois botões permitem ajustar a hora e o minuto do reloógio.
* A tela LCD fica ligada por um tempo, quando esse tempo termina ela apaga.
 * Qualquer ação (sensores ou botões) fazem a tela ser ligada novamente.

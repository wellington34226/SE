# Tarefa 01: LED e 2 Botões
* Pisca LED a cada intervalo de tempo (inicialmente a cada um segundo).
* Dois botões incrementam/decrementam em 100ms o intervalo entre cada piscada.
  * A ação dos botões é executada somente ao pressionar e não ao soltar.
Um debaunce de 62ms foi adicionado para evitar flutuações e deixar a execução mais estável. Caso o botão seja mantido pressionado a ação correspondente é repetida a cada 248ms.
* Caso os dois botões sejam mantidos pressionandos por mais de 500ms a execução é parada, mantendo o último estado do LED.
Cada mudança no intervalo entre cada piscada é enviado via serial e apresentado o novo intervalo em uma tela LCD.

# Tarefa 02: 2 Sensores e 2 Atuadores com acesso direto as portas GPIO

# Tarefa 03: Conversor ou Comparador Analógico

# Tarefa 05: Timers

# ProjetoFinal-TI.1
Projeto Final da matéria Ti-1 da puc minas

# Explicação do codigo 

Inicialmente é definido o número/nome que cada componente do projeto irá utilizar da Arduino.

A segunda sessão define as configurações iniciais do jogo.
Como o tamanho do filtro do potenciômetro, qual a distância a mão precisa estar para ser lida, além das configurações de tempo, listagem das ações do jogo e estados iniciais do jogo.

A terceira sessão é destinada para a filtragem e tratamento do sensor de movimento e do potenciômetro.

A quarta sessão é para funções que darão retorno do jogo ao usuário.
Então aqui estão as funções que fazem sons e acendem os leds em caso de erro ou acerto no jogo, além de algumas das mensagens exibidas no lcd e também monitor serial. 

Aqui também contém a função para esperar qualquer botão para iniciar/recomeçar o jogo e o led verde fica piscando a cada meio segundo para indicar também que o jogo está pausado e o circuito não está travado. 

Basicamente os botões, por padrão, quando não pressionados ficam no estado HIGH. Quando qualquer botão é pressionado, o loop while é interrompido, e o jogo começa/recomeça. 

A quinta sessão
Aqui é onde realmente o fluxo funciona na prática, onde estão as funções setup e loop.

Na função setup temos as configurações iniciais dos pinos conforme as definições do começo do código, a inicialização do LCD, o início da leitura do potenciômetro e a mensagem inicial no LCD.

Agora na função loop temos realmente o fluxo do circuito.
O loop começa sorteando uma das ações e exibindo a junto com a pontuação do jogador.
 
Após isso temos alguns loops que resetam o potenciômetro e botões, afim de evitar possíveis acionamentos fantasmas e game over indesejados.

Após isso o código entra em um loop while, onde enquanto o tempo de execução estiver dentro do tempo atual da rodada o código:

Vai contando o tempo que o usuário tem para tentar acertar e verifica qual ação foi sorteada e faz as devidas verificações se o usuário fez a ação correta ou a ação errada e a verificação ocorrendo e o valor booleano das variáveis mudar conforme o resultado o código sai do while.

A partir daí é verificado o estado da rodada. 

Se o jogador tiver acertado, sua pontuação aumenta, o som de acerto é emitido, o tempo do jogo é diminuído se ainda não estiver no limite, e então o loop é reiniciado, agora mais rápido par a próxima rodada.

Se o jogador errar, o som de erro é emitido, é exibido seu total de pontos e então é esperado o jogador apertar qualquer botão para o jogo recomeçar e reiniciar o looping.


# Link do Video 
*[Visitar o video do Pitch do grupo 9 de TI-1](https://youtu.be/ilQOjXOv7vg)
*[Visitar o video  feito em sala do grupo 9 do Projeto Final de TI-1](https://youtu.be/6xG0Qpqdp6Q)



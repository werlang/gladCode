# gladCode
Batalha de agentes virtuais inteligentes usando temática fantasia medieval

Regras
Arena 25x25 p(passos)  
Intervalo de tempo 0.1s  
Cada mestre(jogador) faz upload do código de três gladiadores. A cada rodada, um gladiador é escolhido por cada mestre, e eles combatem até que reste somente um sobrevivente. A cada nova rodada, o mestre deve escolher um novo gladiador, levando em consideração que os que já morreram nas rodadas anteriores não podem mais ser escolhidos. Um gladiador sobrevivente de outra rodada quando escolhido para uma nova rodada tem metade de seu hp recuperado. Quando houver somente um gladiador vivo, o torneio termina, e o mestre dono deste gladiador é o vencedor. As próximas colocações são definidas a partir da ordem das mortes dos últimos gladidores de cada mestre.
Cada vez que algum gladiador morrer, todos que desferiram algum dano sobem de nível e ganham um ponto extra em um atributo a sua escolha, definido pela função upgradeXXX(). O limite continua sendo 10 pontos em cada atributo. Caso a função upgrade não tenha sido chamada nenhuma vez, ou o atributo já esteja no máximo, o ponto é perdido.

Características fixas  
Vision radius: 60g - arco de visão do gladiador  
Vision distance: 9p - distancia máxima que o gladiador enxerga  
AP recovery: 3% maxAP/s

Atributos   
Cada gladiador possui atributos que definem no que eles são melhores. São STR (força), AGI (agilidade) e INT (intelecto).  
O gladiador possui 15 pontos pra distribuir, máximo 10 em cada.  

Cada ponto em cada atributo modifica certas características, descritas a seguir:

STR
Damage: 5-15 hp (1 str = 1 damage)
Hit Points: 100-300 hp (1 str = 20 hp)

AGI
Attack speed: 0.5-1.5 att/s (1 agi = 0.1 as)
Movement speed: 1-2 p/s (1 agi = 0.1 speed)

INT
Ability Points: 100-300 ap (1 int = 20 ap)
Casting speed: 0.5-1.5 cast/s (1 int = 0.1 cs)

Habilidade | Custo(ap) | Descrição
-|-|-
Fireball | 20 | Causa 0-5 (INT) de dano num ponto central de impacto e 0-15 (INT) decrescendo com raio 2p ao longo de 3s
Teleport | 30 | Teleporta para ponto. Distancia máxima  5-15 p (INT)
Charge | 10 | Move em direção ao alvo com 4x speed. Ao acertar o alvo, realiza um ataque corpo-a-corpo e causa slow 0-50% (STR) por 5s
Block | 25 | Reduz dano levado em 20-80% (STR) por 5s caso atacante esteja no raio de visão, caso contrario efeito pela metade.
Assassinate | 15 | Faz um ataque contra o alvo. Causa dano adicional de AGI caso o alvo não esteja lhe enxergando e outro adicional de AGI caso o alvo esteja atordoado. 
Ambush |35 | Torna-se invisível por 2-6s (AGI), ou até realizar um ataque ou lançar uma habilidade. Ataques realizados enquanto invisivel atordoam o alvo por 1s.

Métodos

Movimento
- turn(g) - se vira g graus. positivo para direita
- turnTo(x,y) - vira na direção do ponto
- turnToHeading(g) - vira na direção do angulo. 0 = cima
- moveTo(x,y) - vira e move em direção ao ponto
- moveForward() - Se move para frente
- moveBackwards() - Se move para trás
- moveLeft() - Se move para esquerda sem se virar
- moveRight() - Se move para direita sem se virar

Ataque
- attackMelee(x,y) - vira para o ponto e ataca para frente num arco de 180g e distancia 1p, dano com 10% de bonus.
- attackRanged(x,y) - vira para o ponto e ataca com projétil para frente com distancia 15p e tempo de percurso 0.5s. Ao acertar causa dano.

Sentidos
- bool doYouSeeMe(x,y) - returna true caso voce esteja no raio de visão do alvo no ponto
- int getMyX() - retorna sua coordenada x
- int getMyY() - retorna sua coordenada y
- getMyCoords(&x,&y) - atribui suas coordenadas
- int howManyEnemies() - retorna quantos gladiadores existem no campo de visão
- bool getLowHp(&x,&y) - atribui as coordenadas do alvo de menor hp no seu campo de visão e retorna true, ou false caso não haja nenhum alvo no ponto
- bool getHighHp(&x,&y) - atribui as coordenadas do alvo de maior hp no seu campo de visão e retorna true, ou false caso não haja nenhum alvo no ponto
- bool getCloseEnemy(&x,&y) - atribui as coordenadas do alvo mais próximo no seu campo de visão e retorna true, ou false caso não haja nenhum alvo no ponto
- bool getFarEnemy(&x,&y) - atribui as coordenadas do alvo mais longe no seu campo de visão e retorna true, ou false caso não haja nenhum alvo no ponto
- float getEnemyHealth(x,y) - retorna percentagem de vida do alvo no ponto, ou 0 caso não haja alvo no ponto
- int getMyHp() - retorna o hp
- int getMyAp() - retorna o ap
- int getMySTR() - retorna STR
- int getMyAGI() - retorna AGI
- int getMyINT() - retorna INT
- float getMySpeed() - retorna speed
- float getMyHeading() - retorna angulo da direcao 0 = cima
- bool areYouAWarrior(x,y) - retorna true caso o alvo no ponto tenha STR como seu maior atributo
- bool areYouARogue(x,y) - retorna true caso o alvo no ponto tenha AGI como seu maior atributo
- bool areYouAMage(x,y) - retorna true caso o alvo no ponto tenha INT como seu maior atributo
- float whereThatCameFrom() - retorna o angulo que originou o último ataque recebido
- bool lockOnTarget(x,y) - fixa a atenção no alvo no ponto. false se nao tiver alvo
- bool isLockedTargetVisible() retorna true caso alvo fixado esteja no campo de visão
- int getLockedTargetCoords(&x,&y) - atribui as coordenadas do alvo fixado, retorna 0 caso o alvo nao esteja mais visivel
- float getLockedTargetSpeed() - retorna a velocidade do alvo fixado
- float getLockedTargetHeading() - retorna a direção do alvo fixado
- float getLockedTargetRelativeHeading() - retorna a direção do alvo fixado relativa ao gladiador

Habilidades
- fireball(x,y) - lança fireball no ponto
- teleport(x,y) - teleporta para o ponto
- charge() - usa charge no alvo fixado
- block() - usa block
- assassinate() - usa assassinate no alvo fixado
- ambush() - usa ambush
- float getBlockTimeLeft() - retorna o tempo restante da habilidade block
- float getAmbushTimeLeft() - retorna o tempo restante da habilidade ambush

Matemática
- float getDist(x,y) - retorna a distância até o ponto
- float getAngle(x,y) - retorna o angulo até o ponto

Melhorias
- upgradeSTR() - melhora atributo STR na próxima vez que passar de nível
- upgradeAGI() - melhora atributo AGI na próxima vez que passar de nível
- upgradeINT() - melhora atributo INT na próxima vez que passar de nível

Métodos do setup
- registerGlad() - Registra o gladiador no torneio (inicializa variáveis)
- setName(string) - Altera o nome do gladiador
- setSTR(n) - altera a quantidade de pontos de STR que o gladiador possui. Não deve ultrapassar 10, a soma de STR, INT e AGI não deve ultrapassar 15
- setAGI(n) - altera a quantidade de pontos de AGI que o gladiador possui
- setINT(n) - altera a quantidade de pontos de INT que o gladiador possui
- upgradeSTR(), upgradeAGI(), upgradeINT() - pode ser chamado aqui também para definir prioridade inicial

Código

Deve possui duas funções: setup e loop.
setup é executado uma única vez, no início do torneio.
loop é executado a cada intervalo de tempo da rodada (0.1s)

Exemplo na linguagem C:

```C
#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("B.War");
    setSTR(9);
    setAGI(4);
    setINT(2);

}

int hpant = 1000;
loop(){
    float x, y;
    if ( !isLockedTargetVisible() ){
        if ( howManyEnemies() > 0 ){
            getLowHp(&x, &y);
            lockOnTarget(x,y);
            if (doYouSeeMe(x,y) && getBlockTimeLeft() <= 0 && getMyAp() >= 25){
                block();
            }
            if (getMyAp() >= 15 && getDist(x,y) > 2){
                charge();
            }
            else
                moveTo(x,y);
        }
        else{
            if (getMyHp() < hpant){
                hpant = getMyHp();
                turnToHeading(whereThatCameFrom());
            }
            else{
                moveTo(screenW/2, screenH/2);
            }
        }
    }
    else{
        getLockedTargetCoords(&x,&y);
        if (doYouSeeMe(x,y) && getBlockTimeLeft() <= 0 && getMyAp() >= 25){
            block();
        }
        else if (getDist(x,y) <= 1)
            attackMelee(x,y);
        else if (getMyAp() >= 15 && getDist(x,y) > 2){
            charge();
        }
        else
            moveTo(x,y);
    }
}
```

Pastas:

sample - Arquivos fonte de exemplo para cada gladiador. Possuem as funções setup, loop e um include para gladCodeAPI.c
balance - Arquivos fonte feitos para balancear valores de habilidades de cada tipo de gladiador. Contém estratégias básicas.

Arquivos:

gladCodeMain.c - Arquivo principal. Recebe como argumento o número de gladiadores, e o nome do arquivo fonte de cada um. Este programa chama uma thread para cada gladiador.  
gladCodeCore.c - Código que será executado por cada thread. Possui o main que chama setup e loop, bem como a maioria das funções que fazem parte da simulação.
gladCodeAPI.c - Cada função disponível para o programador dos gladiadores está contida aqui.  
gladCodeSMem.c - Possuem as funções responsáveis por manipular a memória compartilhada e o mutex.  
gladCodeGlobals.c - Possui a definição da struct, variáveis globais, constantes e o protótipo de algumas funções.  
output.txt - Arquivo com exexmplo de resultado da simulação.
gladCodeRuntimeRender.c - Responsável por renderizar a batalha enquanto está sendo processada. Utiliza a biblioteca Allegro para isso. É um arquivo provisório, só para dar ideia do que está acontecendo enquanto o programa está correndo. A renderização fo projeto será feita baseada num arquivo texto de saída, futuramente.  

Formato do arquivo de saída da simulação:

Ex:

0.1|3|Magus|1|2|4|9|1.40|0.90|1.40|6.6|4.4|0.0|0.00|140.00|280.00|[0.00,0.0],[0.00,0.0],[0.00,0.0],[0.00,0.0],[0.00,0.0]|8

A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R

A - Tempo da simulação.

B - Código do gladiador, começando em 0.

C - Nome (string).

D - Nível.

E - Atributo STR (força).

F - Atributo AGI (agilidade).

G - Atributo INT (inteligência).

H - Velocidade de movimento (passos/segundo).

I - Velocidade de ataque (ataques/segundo).

J - Velicidade de habilidade (habilidades/segundo).

K - Posição horizontal X (passos), crescente da esquerda para direita.

L - Posição vertical Y (passos), crescente de cima para baixo.

M - Direção que o gladiador está virado (graus), crescente no sentido do relógio, variando de 0 até 360.

N - Tempo que o gladiador está esperando até poder desempenhar uma nova tarefa. Ocorre após um ataque ou lançamento de uma habilidade.

O - Vida. A quantidade de dano que o gladiador suporta até morrer.

P - Pontos de habilidade - Recurso necessário para usar habilidades.

Q - String contendo array de modificadores, que indicam algum status especial no gladiador. Cada tipo de modificador possui uma dupla [V,T] de informações, descritas a seguir:

    V - Valor do modificador presente no gladiador. Dependendo do tipo de modificador, este valor representa uma coisa diferente.
    
    T - Tempo restante até o modificador acabar.
    
    Existem os 5 seguintes modificadores:
    - Queimando - O gladiador irá levar dano no valor V distribuído ao longo do tempo T.
    - Movimento - O gladiador possui sua velocidade de movimento alterada pelo multiplicador V.
    - Resistência - Todo dano que o gladiador levar será reduzido pelo percentual V, ou metade dele caso a fonte do dano não esteja visível pelo gladiador.
    - Invisível - O gladiador até acabar a duração T não pode ser visto por nenhum outro gladiador.
    - Atordoado - O gladiador está atordoado pelo tempo T, e não poderá executar nenhuma ação durante este tempo.
	
    A string do array de modificadores é apresentada da seguinte maneira:
    [V0,T0],[V1,t1],[V2,T2],[V3,T3],[V4,T4] onde:
    V0 e T0 é o valor e tempo do modificador Queimando;
    V1 e T1 é o valor e tempo do modificador Movimento;
    V2 e T2 é o valor e tempo do modificador Resistência;
    V3 e T3 é o valor e tempo do modificador Invisível;
    V4 e T4 é o valor e tempo do modificador Atordoado.

R - Código da ação desempenhada pelo gradiador no corrente instante de tempo da simulação. O significado dos códigos é descrito a seguir:

    0 - Usou a habilidade Fireball
    1 - Usou a habilidade Teleport
    2 - Usou a habilidade Charge
    3 - Usou a habilidade Block
    4 - Usou a habilidade Assassinate
    5 - Usou a habilidade Ambush
    6 - Realizou um ataque corpo-a-corpo
    7 - Realizou um ataque a distãncia
    8 - Realizou uma ação de movimento
    9 - Está esperando o tempo necessário para poder desempenhar a próxima ação
    10 - O gladiador não realizou nenhuma ação

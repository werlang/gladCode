# gladCode
Batalha de agentes virtuais inteligentes usando temática fantasia medieval

Regras
Arena 25x25 p(passos)
Intervalo mínimo de tempo 0.1s
Cada mestre(jogador) faz upload do código de três gladiadores. A cada rodada, um gladiador é escolhido por cada mestre, e eles combatem até que reste somente um sobrevivente. A cada nova rodada, o mestre deve escolher um novo gladiador, levando em consideração que os que já morreram nas rodadas anteriores não podem mais ser escolhidos. Um gladiador sobrevivente de outra rodada quando escolhido para uma nova rodada tem metade de seu hp recuperado. Quando houver somente um gladiador vivo, o torneio termina, e o mestre dono deste gladiador é o vencedor. As próximas colocações são definidas a partir da ordem das mortes dos últimos gladidores de cada mestre.
Cada vez que algum gladiador morrer, todos que desferiram algum dano sobem de nível e ganham um ponto extra em um atributo a sua escolha, definido pela função upgradeXXX(). O limite continua sendo 10 pontos em cada atributo. Caso a função upgrade não tenha sido chamada nenhuma vez, ou o atributo já esteja no máximo, o ponto é perdido.

Características fixas
Vision radius: 60g - arco de visão do gladiador
Vision distance: 9p - distancia máxima que o gladiador enxerga

Atributos
Cada gladiador possui atributos que definem no que eles são melhores. São STR (força), AGI (agilidade) e INT (intelecto).
O gladiador possui 15 pontos pra distribuir, máximo 10 em cada.

Cada ponto em cada atributo modifica certas características, descritas a seguir:

STR
Damage: 5-15 hp (1 str = 1 damage)
Hit Points: 50-150 hp (1 str = 10 hp)

AGI
Attack speed: 0.5-1.5 att/s (1 agi = 0.1 as)
Movement speed: 1-2 p/s (1 agi = 0.1 speed)

INT
Ability Points: 50-150 ap (1 int = 10 ap)
Casting speed: 0.5-1.5 cast/s (1 int = 0.1 cs)

Habilidade	Custo(ap)	Descrição
Fireball	20		Causa dano INT num ponto central de impacto e INT decrescendo com raio 2p ao longo de 2s
Teleport	30		Teleporta para ponto. Distancia máxima  5-15 p (INT)
Charge		10		Move em direção ao alvo com 3x speed. Ao acertar o alvo, causa STR dano e slow 0-50% (STR) por 5s
Block		20		Reduz dano levado no raio de visão em 25-50% (STR) por 5s
Assassinate	10		Causa dano do ataque em um alvo. Caso o alvo não esteja lhe enxergando causa um adicional de AGI
Camouflage	25		Torna-se invisível por 1-2s (AGI)

Métodos

Movimento
-turn(g) - se vira g graus. positivo para direita
-turnTo(x,y) - vira na direção do ponto
-moveTo(x,y) - vira e move em direção ao ponto
-moveForward() - Se move para frente
-moveBackwards() - Se move para trás
-moveLeft() - Se move para esquerda sem se virar
-moveRight() - Se move para direita sem se virar

Ataque
-attackMelee(x,y) - vira para o ponto e ataca para frente num arco de 180g e distancia 1p
-attackRanged(x,y) - vira para o ponto e ataca com projétil para frente com distancia 15p e tempo de percurso 0.5s

Sentidos
-bool doYouSeeMe(x,y) - returna true caso voce esteja no raio de visão do alvo no ponto
-int getMyX() - retorna sua coordenada x
-int getMyY() - retorna sua coordenada y
-getMyCoords(&x,&y) - atribui suas coordenadas
-int howManyEnemies() - retorna quantos gladiadores existem no campo de visão
-bool getLowHp(&x,&y) - atribui as coordenadas do alvo de menor hp no seu campo de visão e retorna true, ou false caso não haja nenhum alvo no ponto
-bool getHighHp(&x,&y) - atribui as coordenadas do alvo de maior hp no seu campo de visão e retorna true, ou false caso não haja nenhum alvo no ponto
-int getEnemyHealth(x,y) - retorna o estado de saúde do alvo no ponto (valores de 1 até 4), ou 0 caso não haja alvo no ponto
-int getMyHp() - retorna o hp
-int getMyAp() - retorna o ap
-int getMySTR() - retorna STR
-int getMyAGI() - retorna AGI
-int getMyINT() - retorna INT
-float getMySpeed() - retorna speed
-bool areYouAWarrior(x,y) - retorna true caso o alvo no ponto tenha STR como seu maior atributo
-bool areYouARogue(x,y) - retorna true caso o alvo no ponto tenha AGI como seu maior atributo
-bool areYouAMage(x,y) - retorna true caso o alvo no ponto tenha INT como seu maior atributo
-float whereThatCameFrom() - retorna o angulo que originou o último ataque recebido
-bool lockOnTarget(x,y) - fixa a atenção no alvo no ponto. false se nao tiver alvo
-bool isLockedTargetVisible() retorna true caso alvo fixado esteja no campo de visão
-getLockedTargetCoords(&x,&y) - atribui as coordenadas do alvo fixado

Habilidades
fireball(x,y) - lança fireball no ponto
teleport(x,y) - teleporta para o ponto
charge(x,y) - usa charge no alvo que está no ponto
block() - usa block
assassinate(x,y) - usa assassinate no alvo que está no ponto
camouflage() - usa camouflage

Matemática
-float getDist(x,y) - retorna a distância até o ponto
-float getAngle(x,y) - retorna o angulo até o ponto
-float getLockedTargetSpeed() - retorna a velocidade do alvo fixado

Melhorias
-upgradeSTR() - melhora atributo STR na próxima vez que passar de nível
-upgradeAGI() - melhora atributo AGI na próxima vez que passar de nível
-upgradeINT() - melhora atributo INT na próxima vez que passar de nível

Métodos do setup
-registerGlad() - Registra o gladiador no torneio (inicializa variáveis)
-setName(string) - Altera o nome do gladiador
-setSTR(n) - altera a quantidade de pontos de STR que o gladiador possui. Não deve ultrapassar 10, a soma de STR, INT e AGI não deve ultrapassar 15
-setAGI(n) - altera a quantidade de pontos de AGI que o gladiador possui
-setINT(n) - altera a quantidade de pontos de INT que o gladiador possui
upgradeSTR(), upgradeAGI(), upgradeINT() - pode ser chamado aqui também para definir prioridade inicial

Código

Deve possui duas funções: setup e loop.
setup é executado uma única vez, no início do torneio.
loop é executado a cada intervalo de tempo da rodada (0.1s)

Exemplo na linguagem C:

------ meu_glad.c -------------
```
setup(){
	registerGlad();
	setName("Tutorius");
	setSTR(7);
	setAGI(5);
	setINT(3);
	upgradeSTR();
}

loop(){
	int x_i, y_i;
	if (getLowHp(&x_i,&y_i)){
		if (calc_dist(x_i,y_i) > 1){
			if (getAp() >= 10)
				charge(x_i,y_i);
			else
				moveTo(x_i,y_i);
		}
		else{
			if (getAp() >= 70 || (getHp() <= 40 && getAp() >= 20))
				block();
			attackMelee();
		}
	}
	else
		turn(60);
	
}
```
-------------------------------


Arquivos:

gladCodeMain.c - Arquivo principal. Recebe como argumento o número de gladiadores, e o nome do arquivo fonte de cada um. Este programa chama uma thread para cada gladiador.
c1.c, c2.c, c3.c - Arquivos fontes de exemplo para cada gladiador. Possuem as funções setup, loop e um include para gladCodeIO.c
gladCodeIO.c - Programa que gerencia o arquivo que contém as informações gladiadores, a tranca para evitar condições de disputa. Possui também o main que chama setup e loop.
gladCodeAPI.c - Cada função disponível para o programador dos gladiadores está contida aqui, bem como outras auxiliares.
gladCodeRuntimeRender.c - Responsável por renderizar a batalha enquanto está sendo processada. Utiliza a biblioteca Allegro para isso. É um arquivo provisório, só para dar ideia do que está acontecendo enquanto o programa está correndo. A renderização fo projeto será feita baseada num arquivo texto de saída, futuramente.

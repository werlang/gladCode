#define PROJECTILE_MAX_DISTANCE 15
#define AP_RECOVERY 0.03
#define GLAD_HITBOX 0.5

#define PROJECTILE_TYPE_ATTACK 0
#define PROJECTILE_TYPE_FIREBALL 1
#define PROJECTILE_TYPE_STUN 2

#define N_BUFFS 5
#define BUFF_BURN 0
#define BUFF_MOVEMENT 1
#define BUFF_RESIST 2
#define BUFF_INVISIBLE 3
#define BUFF_STUN 4

#define ABILITY_FIREBALL 0
#define ABILITY_TELEPORT 1
#define ABILITY_CHARGE 2
#define ABILITY_BLOCK 3
#define ABILITY_ASSASSINATE 4
#define ABILITY_AMBUSH 5

#define ACTION_MELEE_ATTACK 6
#define ACTION_RANGED_ATTACK 7
#define ACTION_MOVEMENT 8
#define ACTION_WAITING 9
#define ACTION_NONE 10

//custos de cada habilidade. os índices batem com as constantes dos buffs
int abilitycost[6] = {20,30,10,25,15,35};

struct buff {
    float timeleft;
    float value;
};

//lista encadeada dos projeteis
struct projectile {
    int type; //tipo de projetil
    float x; //coordenadas
    float y;
    float spdx; //velocidade em nos componentes x e y
    float spdy;
    float dmg; //dano que causa se atingir
    float dist; //distancia ja percorrida desde seu lancamento
    struct projectile *next;
};

//struct dos gladiadores
struct gladiador{
    char name[50];
    float x; //pos horizontal
    float y; //pos vertical
    float head; //angulo da cabeca 0..360, cresce no sentido horário,

    int STR; //forca
    float hp; //vida
    float maxhp; //hp total
    float dmg; //dano

    int AGI; //agilidade
    float as; //velocidade de ataque, em att/s
    float spd; //velocidade de movimento, em p/s

    int INT; //intelecto
    float ap; //pontos de habilidade
    float maxap; //quantidade maxima de ap
    float cs; //velocidade de lancamento de habilidades

    int lvl; //nivel
    float vrad; //raio de visao, em g
    float vis; //alcance de visao, em p
    float lockedfor; //tempo que esta trancado em uma acao
    float lasthit; //angulo que recebeu o último ataque
    int targetlocked; //indice do gladiador que está fixado
    int damagedme; //quem deu dano no gladiador, serve para saber quem sobe de nivel quando este morrer.
    int up; //qual atributo vai ganhar upgrade quando subir de nivel
    struct buff buffs[N_BUFFS]; //vetor de buffs
};

struct gladiador *g;
struct projectile *p;
int nglad = 0; //numero de gladiadores
int endsim = 0; //flag que indica o fim da simulacao
char *outString; //string dinamica que recebe o relatorio de todas as acoes tomadas pelo gladiador
int gladid=0;
//largura e altrua da arena, em passos
float screenW = 25;
float screenH = 25;
float timeInterval = 0.1; //intervalo de tempo da simulacao
float timeLimit = 200; //tempo limite da para o evento de fim da simulação
int actioncode= ACTION_WAITING; //codigo da acao desemprenhada no instante
int rounds, estado;
int winners[3];


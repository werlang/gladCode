/*
buff codes:
0 - burning
1 - haste/slow
2 - resistant
3 - invisible
4 - stun

*/

#define PROJECTILE_TYPE_ATTACK 0
#define PROJECTILE_TYPE_FIREBALL 1
#define PROJECTILE_TYPE_ASSASSINATE 2
#define PROJECTILE_MAX_DISTANCE 15
#define AP_RECOVERY 0.05
#define N_BUFFS 5
#define BUFF_BURN 0
#define BUFF_MOVEMENT 1
#define BUFF_RESIST 2
#define BUFF_INSIBIBLE 3
#define BUFF_STUN 4
#define ABILITY_FIREBALL 0
#define ABILITY_TELEPORT 1
#define ABILITY_CHARGE 2
#define ABILITY_BLOCK 3
#define ABILITY_ASSASSINATE 4
#define ABILITY_CAMOUFLAGE 5
#define ACTION_MELEE_ATTACK 6
#define ACTION_RANGED_ATTACK 7
#define ACTION_MOVEMENT 8
#define ACTION_WAITING 9

int abilitycost[6] = {20,30,10,20,10,30};

struct buff {
    float timeleft;
    float value;
};

struct projectile {
    int type;
    float x;
    float y;
    float spdx;
    float spdy;
    float dmg;
    float dist;
    struct projectile *next;
};

struct gladiador{
    char name[50];
    float x; //pos horizontal
    float y; //pos vertical
    float head; //angulo da cabeca

    int STR; //forca
    float hp; //vida
    float maxhp; //hp total
    float dmg; //dano

    int AGI; //agilidade
    float as; //velocidade de ataque, em att/s
    float spd; //velocidade de movimento, em p/s

    int INT; //intelecto
    float ap; //pontos de habilidade
    float maxap;
    float cs; //velocidade de lancamento de habilidades

    int lvl; //nivel
    float vrad; //raio de visao, em g
    float vis; //alcance de visao, em p
    float lockedfor; //tempo que esta trancado em uma acao
    float lasthit;
    int targetlocked;
    int damagedme;
    int up;
    struct buff buffs[N_BUFFS];
};

struct gladiador *g;
struct projectile *p;
int nglad = 0;
int endsim = 0;
char *outString;
int gladid=0;
//largura e altrua da arena, em passos
float screenW = 25;
float screenH = 25;
float timeInterval = 0.1;
int actioncode= ACTION_WAITING;

float getDist(float x, float y);
void moveTo(float x, float y);
int getLockedTargetCoords(float *x, float *y);
void recordSteps();

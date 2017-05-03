//struct
struct gladiador{
    char name[50];
    float x; //pos horizontal
    float y; //pos vertical
    float head; //angulo da cabeca

    int STR; //forca
    int hp; //vida
    int maxhp; //hp total
    float dmg; //dano

    int AGI; //agilidade
    float as; //velocidade de ataque, em att/s
    float spd; //velocidade de movimento, em p/s

    int INT; //intelecto
    int ap; //pontos de habilidade
    float cs; //velocidade de lancamento de habilidades

    int lvl; //nivel
    float vrad; //raio de visao, em g
    float vis; //alcance de visao, em p
    float lockedfor; //tempo que esta trancado em uma acao
    float lasthit;
    int targetlocked;
    int damagedme;
    int up;
};
int nglad = 0;
struct gladiador *g=NULL;

void updateTime();
void recordSteps();
void updateProjectiles();

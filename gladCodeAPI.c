#define PROJECTILE_TYPE_ATTACK 0
#define PROJECTILE_TYPE_FIREBALL 1

//qual gladiador esta agindo no momento
int gladid=0;
//largura e altrua da arena, em passos
float screenW = 25;
float screenH = 25;
float timeInterval = 0.1;
int actioncode;
//inicializa o gladiador
void registerGlad(){
    //printf("[%i]",gladid);
    (g+gladid)->lvl = 1;
    (g+gladid)->vrad = 60;
    (g+gladid)->vis = 9;
    (g+gladid)->lockedfor = 0;
    //inicia em uma posicao aleatoria do mapa
    (g+gladid)->x = screenW/nglad * (gladid + 0.5);
    (g+gladid)->y = gladid%2 * screenH + 2 - (gladid%2 * 4) ;
    //um angulo de visao aleatorio
    (g+gladid)->head = rand()%360;
    (g+gladid)->targetlocked = -1;
    (g+gladid)->up = 0;
}

void setSTR(int n){
    (g+gladid)->STR = n;
    (g+gladid)->hp = 50 + n*10;
    (g+gladid)->maxhp = (g+gladid)->hp;
    (g+gladid)->dmg = n+5;
}

void setAGI(int n){
    (g+gladid)->AGI = n;
    (g+gladid)->spd = 1 + n*0.1;
    (g+gladid)->as = 0.5 + n*0.1;
}

void setINT(int n){
    (g+gladid)->INT = n;
    (g+gladid)->ap = 50 + n*10;
    (g+gladid)->cs = 0.5 + n*0.1;
}

void setName(char *str){
    strcpy((g+gladid)->name, str);
}

void upgradeSTR(){
    (g+gladid)->up = 1;
}

void upgradeAGI(){
    (g+gladid)->up = 2;
}

void upgradeINT(){
    (g+gladid)->up = 3;
}

//verifica se somatorio da distribuicao nao é 15 pontos, ou se cada atributo nao respeita o intervalo 0-10
int checkSetup(){
    int sum = (g+gladid)->STR + (g+gladid)->AGI + (g+gladid)->INT;
    if (sum != 15)
        return 0;
    if ((g+gladid)->STR < 0 || (g+gladid)->STR > 10)
        return 0;
    if ((g+gladid)->AGI < 0 || (g+gladid)->AGI > 10)
        return 0;
    if ((g+gladid)->INT < 0 || (g+gladid)->INT > 10)
        return 0;

    return 1;
}

//converte de graus para radianos
float gToRad(float g){
    return g*M_PI/180;
}

//converte de rad para graus
float radToG(float r){
    return 180*r/M_PI;
}

//recebe um ponto, retorna a distancia entre estes pontos
float getDist(float x, float y){
    float dx = (g+gladid)->x-x;
    float dy = (g+gladid)->y-y;
    return sqrt( pow(dx,2) + pow(dy,2) );
}

float getAngleFromAB(float xa, float ya, float xb, float yb){
    float dx = xb - xa;
    float dy = ya - yb;
    if (dy > 0)
        return radToG(atan(dx/dy));
    else if (dy < 0)
        return radToG(atan(dx/dy))+180;
    else
        return 90 * (dx/abs(dx));
}

//recebe um ponto, retorna o angulo o ponto do glad e o ponto recebido
float getAngle(float x, float y){
    return getAngleFromAB((g+gladid)->x, (g+gladid)->y, x, y);
}

//recebe hipotenusa e angulo, calcula calcula e atribui por referencia os catetos
void calcSidesFromAngleDist(float *dx, float *dy, float d, float a){
    *dx = sin(gToRad(a))*d;
    *dy = cos(gToRad(a))*d;
}

//impede que o glad saia da tela
void preventLeaving(){
    if ( (g+gladid)->y > screenH)
        (g+gladid)->y = screenH;
    if ( (g+gladid)->y < 0)
        (g+gladid)->y = 0;
    if ( (g+gladid)->x > screenW)
        (g+gladid)->x = screenW;
    if ( (g+gladid)->x < 0)
        (g+gladid)->x = 0;
}

void waitUntilReady(){
    while ( (g+gladid)->lockedfor > 0 ){
        updateProjectiles();
        updateTime();
        recordSteps();
    }
}

//se move para a frente
void moveForward(){
    waitUntilReady();
    waitForMutex();
    loadStructFromMemory();
    float hip = (g+gladid)->spd*timeInterval;
    float ang = (g+gladid)->head;
    float dx, dy;
    calcSidesFromAngleDist(&dx, &dy, hip, ang);

    (g+gladid)->x += dx;
    (g+gladid)->y -= dy;

    preventLeaving();

    (g+gladid)->lockedfor = timeInterval;
    actioncode = 1;
    saveStructToMemory();
    releaseMutex();
}

//se move para tras
void moveBackwards(){
    waitUntilReady();
    waitForMutex();
    loadStructFromMemory();
    float hip = -(g+gladid)->spd*timeInterval;
    float ang = (g+gladid)->head;
    float dx, dy;
    calcSidesFromAngleDist(&dx, &dy, hip, ang);

    (g+gladid)->x += dx;
    (g+gladid)->y -= dy;

    preventLeaving();

    (g+gladid)->lockedfor = timeInterval;
    actioncode = 1;
    saveStructToMemory();
    releaseMutex();
}

//se move para a esquerda
void moveLeft(){
    waitUntilReady();
    waitForMutex();
    loadStructFromMemory();
    float hip = (g+gladid)->spd*timeInterval;
    float ang = (g+gladid)->head-90;
    float dx, dy;
    calcSidesFromAngleDist(&dx, &dy, hip, ang);

    (g+gladid)->x += dx;
    (g+gladid)->y -= dy;

    preventLeaving();

    (g+gladid)->lockedfor = timeInterval;
    actioncode = 1;
    saveStructToMemory();
    releaseMutex();
}

//se move para a direita
void moveRight(){
    waitUntilReady();
    waitForMutex();
    loadStructFromMemory();
    float hip = (g+gladid)->spd*timeInterval;
    float ang = (g+gladid)->head+90;
    float dx, dy;
    calcSidesFromAngleDist(&dx, &dy, hip, ang);

    (g+gladid)->x += dx;
    (g+gladid)->y -= dy;

    preventLeaving();

    (g+gladid)->lockedfor = timeInterval;
    actioncode = 1;
    saveStructToMemory();
    releaseMutex();
}

//ajusta angulos para estarem entro 0-360, onde 0==360==note
float getNormalAngle(float ang){
    if (ang >= 360)
        ang -= 360;
    if (ang < 0)
        ang += 360;
    return ang;
}

//vira a visao tantos graus
void turn(float ang){
    waitUntilReady();
    waitForMutex();
    loadStructFromMemory();
    (g+gladid)->head += ang;
    (g+gladid)->head = getNormalAngle((g+gladid)->head);
    (g+gladid)->lockedfor = timeInterval;
    actioncode = 1;
    saveStructToMemory();
    releaseMutex();
}

//se vira na direcao de um ponto
void turnTo(float x, float y){
    waitUntilReady();
    waitForMutex();
    loadStructFromMemory();
    (g+gladid)->head = getAngle(x,y);
    (g+gladid)->head = getNormalAngle((g+gladid)->head);
    (g+gladid)->lockedfor = timeInterval;
    actioncode = 1;
    saveStructToMemory();
    releaseMutex();
}

//vira para o ponto, e anda em direcao a ele
void moveTo(float x, float y){
    waitUntilReady();
    waitForMutex();
    loadStructFromMemory();
    (g+gladid)->head = getAngle(x,y);
    (g+gladid)->head = getNormalAngle((g+gladid)->head);
    if (getDist(x,y) < (g+gladid)->spd*timeInterval){
        (g+gladid)->x = x;
        (g+gladid)->y = y;
    }
    else{
        float hip = (g+gladid)->spd*timeInterval;
        float ang = (g+gladid)->head;
        float dx, dy;
        calcSidesFromAngleDist(&dx, &dy, hip, ang);

        (g+gladid)->x += dx;
        (g+gladid)->y -= dy;
    }
    preventLeaving();
    (g+gladid)->lockedfor = timeInterval;
    actioncode = 1;
    saveStructToMemory();
    releaseMutex();
}

//retorna X do glad
float getMyX(){
    waitUntilReady();
    return (g+gladid)->x;
}

//retorna Y do glad
float getMyY(){
    waitUntilReady();
    return (g+gladid)->y;
}

//atribui em x e y as coordenadas do glad
void getMyCoords(float *x, float *y){
    waitUntilReady();
    *x = (g+gladid)->x;
    *y = (g+gladid)->y;
}

//retorna hp
int getMyHp(){
    waitUntilReady();
    return (g+gladid)->hp;
}

//retorna ap
int getMyAp(){
    waitUntilReady();
    return (g+gladid)->ap;
}

//retorna STR
int getMySTR(){
    waitUntilReady();
    return (g+gladid)->STR;
}

//retorna AGI
int getMyAGI(){
    waitUntilReady();
    return (g+gladid)->AGI;
}

//retorna INT
int getMyINT(){
    waitUntilReady();
    return (g+gladid)->INT;
}

//retorna velocidade
float getMySpeed(){
    waitUntilReady();
    return (g+gladid)->spd;
}

void setDamagedMe(int me, int en){
    //pego o bit EN, deslocando pra direita, e pegando o ultimo com resto.
    int current = (g+me)->damagedme;
    int cbit = (current >> en)%2;
    if (!cbit)
        (g+me)->damagedme += 1<<en;
}

int haveYouDamagedMe(int en){
    int current = (g+gladid)->damagedme;
    int cbit = (current >> en)%2;
    return cbit;
}

//ataca inimigo em frente num raio de 180g
void attackMelee(float x, float y){
    waitUntilReady();
    waitForMutex();
    loadStructFromMemory();
    (g+gladid)->head = getAngle(x,y);
    (g+gladid)->head = getNormalAngle((g+gladid)->head);
    int i;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0){
            float dist = getDist((g+i)->x, (g+i)->y);
            float ang = getAngle((g+i)->x, (g+i)->y) - (g+gladid)->head;
            ang = getNormalAngle(ang);
            if ( dist <= 1 && (ang <= 90 || ang >= 270) ){
                (g+i)->hp -= (g+gladid)->dmg;
                (g+i)->lasthit = getNormalAngle(getAngleFromAB((g+i)->x, (g+i)->y, (g+gladid)->x, (g+gladid)->y));
                setDamagedMe(i, gladid);
            }
        }
    }
    (g+gladid)->lockedfor = 1/(g+gladid)->as;
    actioncode = 2;
    saveStructToMemory();
    releaseMutex();
}

//retorna quantos inimigos o glad enxerga no campo de visao
int howManyEnemies(){
    waitUntilReady();
    int i, cont=0;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0){
            float dist = getDist((g+i)->x, (g+i)->y);
            float ang = getAngle((g+i)->x, (g+i)->y) - (g+gladid)->head;
            ang = getNormalAngle(ang);
            if ( dist <= (g+gladid)->vis && (ang <= (g+gladid)->vrad/2 || ang >= 360-(g+gladid)->vrad/2) ){
                cont++;;
            }
        }
    }
    return cont;
}

int getCloseEnemy(float *x, float *y){
    waitUntilReady();
    int i, closeri=-1, lowerdist;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0){
            float dist = getDist((g+i)->x, (g+i)->y);
            float ang = getAngle((g+i)->x, (g+i)->y) - (g+gladid)->head;
            ang = getNormalAngle(ang);
            if ( dist <= (g+gladid)->vis && (ang <= (g+gladid)->vrad/2 || ang >= 360-(g+gladid)->vrad/2) ){
                if (closeri == -1 || dist < lowerdist){
                    lowerdist = dist;
                    closeri = i;
                }
            }
        }
    }
    if(closeri == -1)
        return 0;
    else{
        *x = (g+closeri)->x;
        *y = (g+closeri)->y;
        return 1;
    }
}

int getFarEnemy(float *x, float *y){
    waitUntilReady();
    int i, fari=-1, higherdist;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0){
            float dist = getDist((g+i)->x, (g+i)->y);
            float ang = getAngle((g+i)->x, (g+i)->y) - (g+gladid)->head;
            ang = getNormalAngle(ang);
            if ( dist <= (g+gladid)->vis && (ang <= (g+gladid)->vrad/2 || ang >= 360-(g+gladid)->vrad/2) ){
                if (fari == -1 || dist > higherdist){
                    higherdist = dist;
                    fari = i;
                }
            }
        }
    }
    if(fari == -1)
        return 0;
    else{
        *x = (g+fari)->x;
        *y = (g+fari)->y;
        return 1;
    }
}

//atribui em x,y as coordenadas do inimigo no campo de visao de menor hp
int getLowHp(float *x, float *y){
    waitUntilReady();
    int i, loweri=-1;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0){
            float dist = getDist((g+i)->x, (g+i)->y);
            float ang = getAngle((g+i)->x, (g+i)->y) - (g+gladid)->head;
            ang = getNormalAngle(ang);
            if ( dist <= (g+gladid)->vis && (ang <= (g+gladid)->vrad/2 || ang >= 360-(g+gladid)->vrad/2) ){
                if (loweri == -1 || (g+i)->hp < (g+loweri)->hp)
                    loweri = i;
            }
        }
    }
    if(loweri == -1)
        return 0;
    else{
        *x = (g+loweri)->x;
        *y = (g+loweri)->y;
        return 1;
    }
}

//atribui em x,y as coordenadas do inimigo no campo de visao de maior hp
int getHighHp(float *x, float *y){
    waitUntilReady();
    int i, higheri=-1;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0){
            float dist = getDist((g+i)->x, (g+i)->y);
            float ang = getAngle((g+i)->x, (g+i)->y) - (g+gladid)->head;
            ang = getNormalAngle(ang);
            if ( dist <= (g+gladid)->vis && (ang <= (g+gladid)->vrad/2 || ang >= 360-(g+gladid)->vrad/2) ){
                if (higheri == -1 || (g+i)->hp > (g+higheri)->hp)
                    higheri = i;
            }
        }
    }
    if(higheri == -1)
        return 0;
    else{
        *x = (g+higheri)->x;
        *y = (g+higheri)->y;
        return 1;
    }
}

int getEnemyHealth(float x, float y){
    waitUntilReady();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y )
            break;
    }
    if (i == nglad)
        return 0;
    else{
        float porc = (float)((g+i)->hp) / (g+i)->maxhp;
        if (porc > 0.75)
            return 4;
        else if (porc > 0.5)
            return 3;
        else if (porc > 0.25)
            return 2;
        else
            return 1;
    }
}

int areYouAWarrior(float x, float y){
    waitUntilReady();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y )
            break;
    }
    if (i == nglad)
        return 0;
    else{
        if ( (g+i)->STR > (g+i)->AGI && (g+i)->STR > (g+i)->INT )
            return 1;
        else
            return 0;
    }
}

int areYouARogue(float x, float y){
    waitUntilReady();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y )
            break;
    }
    if (i == nglad)
        return 0;
    else{
        if ( (g+i)->AGI > (g+i)->STR && (g+i)->AGI > (g+i)->INT )
            return 1;
        else
            return 0;
    }
}

int areYouAMage(float x, float y){
    waitUntilReady();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y )
            break;
    }
    if (i == nglad)
        return 0;
    else{
        if ( (g+i)->INT > (g+i)->STR && (g+i)->INT > (g+i)->AGI )
            return 1;
        else
            return 0;
    }
}

int doYouSeeMe(float x, float y){
    waitUntilReady();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y )
            break;
    }
    if (i == nglad)
        return 0;
    else{
        float dist = getDist((g+i)->x, (g+i)->y);
        float ang = getAngle((g+i)->x, (g+i)->y) - (g+i)->head + 180;
        ang = getNormalAngle(ang);
        if ( dist <= (g+i)->vis && (ang <= (g+i)->vrad/2 || ang >= 360-(g+i)->vrad/2) )
            return 1;
        else
            return 0;
    }
}

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
struct projectile *p = NULL;

void launchProjectile(float x, float y, float dmg, float spdx, float spdy, int type){
    struct projectile *t = (struct projectile*)malloc(sizeof(struct projectile));

    t->type = type;
    t->x = x;
    t->y = y;
    t->spdx = spdx;
    t->spdy = spdy;
    t->dmg = dmg;
    t->dist = 0;
    t->next = NULL;

    if (p == NULL)
        p = t;
    else{
        struct projectile *a = p;
        while (a->next != NULL)
            a = a->next;
        a->next = t;
    }
}

void removeProjectile(struct projectile *a){
    struct projectile *t = p;
    if (p == a){
        p = p->next;
        free(t);
    }
    else{
        while(t->next != a)
            t = t->next;
        t->next = t->next->next;
        free(a);
    }
}

void updateProjectiles(){
    int j, k;
    float hitbox = 1;
    float travelunit = 3; //em quantas etapas quebra 1 passo
    float speed = 30; //quantos timeInterval ele anda
    struct projectile *a = p;
    int i=0;
    while (a != NULL){
        i++;
        int hitglad = 0;
        waitForMutex();
        loadStructFromMemory();
        for (k=0 ; k < speed * timeInterval * travelunit ; k++){
            a->x += a->spdx / travelunit;
            a->y += a->spdy / travelunit;
            a->dist += 1 / travelunit;
            float xl = a->x - hitbox/2;
            float xr = a->x + hitbox/2;
            float yl = a->y - hitbox/2;
            float yr = a->y + hitbox/2;
            for (j=0 ; j<nglad ; j++){
                if ( (g+j)->hp > 0 && j != gladid){
                    float xg = (g+j)->x;
                    float yg = (g+j)->y;
                    //printf("%i %i %i %.1f %.1f %.1f %.1f %.1f %.1f\n",gladid,i,k,xl,xg,xr,yl,yg,yr);
                    if ( xg >= xl && xg <= xr && yg >= yl && yg <= yr ){
                        (g+j)->hp -= a->dmg;
                        //printf("acertou! I:%i HP:%i\n",j,(g+j)->hp);
                        (g+j)->lasthit = getNormalAngle(getAngleFromAB((g+j)->x, (g+j)->y, a->x - a->spdx / travelunit, a->y - a->spdy / travelunit));
                        setDamagedMe(j, gladid);
                        hitglad = 1;
                        break;
                    }
                }
            }
            if (hitglad)
                break;
        }
        saveStructToMemory();
        releaseMutex();
        if (a->dist >= 15 || hitglad){
            struct projectile *t = a->next;
            removeProjectile(a);
            a = t;
        }
        else
            a = a->next;
        //printf("P%i X:%.1f Y:%.1f SX:%.1f SY:%.1f\n",i,a->x,a->y,a->spdx,a->spdy);
    }
}

int attackRanged(float x, float y){
    waitUntilReady();
    waitForMutex();
    loadStructFromMemory();
    (g+gladid)->head = getAngle(x,y);
    (g+gladid)->head = getNormalAngle((g+gladid)->head);
    float spdx, spdy;
    calcSidesFromAngleDist(&spdx, &spdy, 1, (g+gladid)->head);
    launchProjectile((g+gladid)->x, (g+gladid)->y, (g+gladid)->dmg, spdx, -spdy, PROJECTILE_TYPE_ATTACK);
    (g+gladid)->lockedfor = 1/(g+gladid)->as;
    actioncode = 3;
    saveStructToMemory();
    releaseMutex();
}

float whereThatCameFrom(){
    waitUntilReady();
    return (g+gladid)->lasthit;
}

int lockOnTarget(float x, float y){
    waitUntilReady();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y ){
            (g+gladid)->targetlocked = i;
            return 1;
        }
    }
    return 0;
}

int isLockedTargetVisible(){
    waitUntilReady();
    if ( (g+gladid)->targetlocked == -1 )
        return 0;
    int target = (g+gladid)->targetlocked;
    float dist = getDist( (g+target)->x, (g+target)->y );
    float ang = getAngle((g+target)->x, (g+target)->y) - (g+gladid)->head;
    ang = getNormalAngle(ang);

    if ( dist > (g+gladid)->vis ){
        (g+gladid)->targetlocked = -1;
        return 0;
    }
    if ( ang > (g+gladid)->vrad/2 && ang < 360-(g+gladid)->vrad/2 ){
        (g+gladid)->targetlocked = -1;
        return 0;
    }
    return 1;
}

void getLockedTargetCoords(float *x, float *y){
    waitUntilReady();
    if ( isLockedTargetVisible() ){
        int target = (g+gladid)->targetlocked;
        *x = (g+target)->x;
        *y = (g+target)->y;
    }
}

float getLockedTargetSpeed(){
    waitUntilReady();
    if (!isLockedTargetVisible())
        return 0;
    int target = (g+gladid)->targetlocked;
    return (g+target)->spd;
}

float getLockedTargetHeading(){
    waitUntilReady();
    if (!isLockedTargetVisible())
        return 0;
    int target = (g+gladid)->targetlocked;
    return (g+target)->head;
}


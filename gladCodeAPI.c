#include "gladCodeCore.c"

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
    (g+gladid)->damagedme = 0;
    int i;
    for (i=0 ; i<N_BUFFS ; i++){
        (g+gladid)->buffs[i].timeleft = 0;
        (g+gladid)->buffs[i].value = 0;
    }
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
    (g+gladid)->maxap = (g+gladid)->ap;
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

//se move para a frente
void moveForward(){
    waitLoad();

    moveForwardUnsafe();
    actioncode = ACTION_MOVEMENT;
    (g+gladid)->lockedfor = timeInterval;

    saveRelease();
}

//se move para tras
void moveBackwards(){
    waitLoad();

    float hip = -(g+gladid)->spd*timeInterval;
    float ang = (g+gladid)->head;
    float dx, dy;
    calcSidesFromAngleDist(&dx, &dy, hip, ang);
    (g+gladid)->x += dx;
    (g+gladid)->y -= dy;
    preventCollision(dx, -dy);
    preventLeaving();
    actioncode = ACTION_MOVEMENT;
    (g+gladid)->lockedfor = timeInterval;

    saveRelease();
}

//se move para a esquerda
void moveLeft(){
    waitLoad();

    float hip = (g+gladid)->spd*timeInterval;
    float ang = (g+gladid)->head-90;
    float dx, dy;
    calcSidesFromAngleDist(&dx, &dy, hip, ang);
    (g+gladid)->x += dx;
    (g+gladid)->y -= dy;
    preventCollision(dx, -dy);
    preventLeaving();
    actioncode = ACTION_MOVEMENT;
    (g+gladid)->lockedfor = timeInterval;

    saveRelease();
}

//se move para a direita
void moveRight(){
    waitLoad();

    float hip = (g+gladid)->spd*timeInterval;
    float ang = (g+gladid)->head+90;
    float dx, dy;
    calcSidesFromAngleDist(&dx, &dy, hip, ang);
    (g+gladid)->x += dx;
    (g+gladid)->y -= dy;
    preventCollision(dx, -dy);
    preventLeaving();
    actioncode = ACTION_MOVEMENT;
    (g+gladid)->lockedfor = timeInterval;

    saveRelease();
}

//vira a visao tantos graus
void turn(float ang){
    waitLoad();

    (g+gladid)->head += ang;
    (g+gladid)->head = getNormalAngle((g+gladid)->head);
    actioncode = ACTION_MOVEMENT;
    (g+gladid)->lockedfor = timeInterval;

    saveRelease();
}

//se vira na direcao de um ponto
void turnTo(float x, float y){
    waitLoad();

    turnToUnsafe(x,y);
    actioncode = ACTION_MOVEMENT;
    (g+gladid)->lockedfor = timeInterval;

    saveRelease();
}

//vira para o ponto, e anda em direcao a ele
void moveTo(float x, float y){
    waitLoad();

    moveToUnsafe(x,y);

    actioncode = ACTION_MOVEMENT;
    (g+gladid)->lockedfor = timeInterval;

    saveRelease();
}

//retorna X do glad
float getMyX(){
    loadStructFromMemory();
    return (g+gladid)->x;
}

//retorna Y do glad
float getMyY(){
    loadStructFromMemory();
    return (g+gladid)->y;
}

//atribui em x e y as coordenadas do glad
void getMyCoords(float *x, float *y){
    loadStructFromMemory();
    *x = (g+gladid)->x;
    *y = (g+gladid)->y;
}

//retorna hp
float getMyHp(){
    loadStructFromMemory();
    return (g+gladid)->hp;
}

//retorna ap
float getMyAp(){
    loadStructFromMemory();
    return (g+gladid)->ap;
}

//retorna STR
int getMySTR(){
    loadStructFromMemory();
    return (g+gladid)->STR;
}

//retorna AGI
int getMyAGI(){
    loadStructFromMemory();
    return (g+gladid)->AGI;
}

//retorna INT
int getMyINT(){
    loadStructFromMemory();
    return (g+gladid)->INT;
}

//recebe um ponto, retorna a distancia entre estes pontos
float getDist(float x, float y){
    loadStructFromMemory();
    return getDistUnsafe(x,y);
}

//retorna velocidade
float getMySpeed(){
    loadStructFromMemory();
    return (g+gladid)->spd;
}

//ataca inimigo em frente num raio de 180g
void attackMelee(float x, float y){
    waitLoad();

    attackMeleeUnsafe(x,y);

    actioncode = ACTION_MELEE_ATTACK;
    (g+gladid)->lockedfor = 1/(g+gladid)->as;

    saveRelease();
}

//retorna quantos inimigos o glad enxerga no campo de visao
int howManyEnemies(){
    loadStructFromMemory();
    int i, cont=0;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0){
            float dist = getDistUnsafe((g+i)->x, (g+i)->y);
            float ang = getAngle((g+i)->x, (g+i)->y) - (g+gladid)->head;
            ang = getNormalAngle(ang);
            if ( dist <= (g+gladid)->vis && (ang <= (g+gladid)->vrad/2 || ang >= 360-(g+gladid)->vrad/2) ){
                cont++;
            }
        }
        //printf("varreu %i\n",i);
    }
    //printf("varreu\n");
    return cont;
}

int getCloseEnemy(float *x, float *y){
    loadStructFromMemory();
    int i, closeri=-1, lowerdist;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0){
            float dist = getDistUnsafe((g+i)->x, (g+i)->y);
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
    loadStructFromMemory();
    int i, fari=-1, higherdist;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0){
            float dist = getDistUnsafe((g+i)->x, (g+i)->y);
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
    loadStructFromMemory();
    int i, loweri=-1;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0){
            float dist = getDistUnsafe((g+i)->x, (g+i)->y);
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
    loadStructFromMemory();
    int i, higheri=-1;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0){
            float dist = getDistUnsafe((g+i)->x, (g+i)->y);
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
    loadStructFromMemory();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0)
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
    loadStructFromMemory();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0)
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
    loadStructFromMemory();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0)
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
    loadStructFromMemory();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0)
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
    loadStructFromMemory();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0)
            break;
    }
    if (i == nglad)
        return 0;
    else{
        float dist = getDistUnsafe((g+i)->x, (g+i)->y);
        float ang = getAngle((g+i)->x, (g+i)->y) - (g+i)->head + 180;
        ang = getNormalAngle(ang);
        if ( dist <= (g+i)->vis && (ang <= (g+i)->vrad/2 || ang >= 360-(g+i)->vrad/2) )
            return 1;
        else
            return 0;
    }
}

int attackRanged(float x, float y){
    waitLoad();

    turnToUnsafe(x,y);
    float spdx, spdy;
    calcSidesFromAngleDist(&spdx, &spdy, 1, (g+gladid)->head);
    launchProjectile((g+gladid)->x, (g+gladid)->y, (g+gladid)->dmg, spdx, -spdy, PROJECTILE_TYPE_ATTACK);
    actioncode = ACTION_RANGED_ATTACK;
    (g+gladid)->lockedfor = 1/(g+gladid)->as;

    saveRelease();
}

float whereThatCameFrom(){
    loadStructFromMemory();
    return (g+gladid)->lasthit;
}

int lockOnTarget(float x, float y){
    waitLoad();

    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INSIBIBLE].timeleft <= 0){
            (g+gladid)->targetlocked = i;
            saveRelease();
            return 1;
        }
    }
    saveRelease();
    return 0;
}

int isLockedTargetVisible(){
    loadStructFromMemory();
    if ( (g+gladid)->targetlocked == -1 )
        return 0;

    int fail = 0;

    int target = (g+gladid)->targetlocked;
    if ((g+target)->hp <= 0)
        fail = 1;

    if ((g+target)->buffs[BUFF_INSIBIBLE].timeleft > 0)
        fail = 1;

    float dist = getDistUnsafe( (g+target)->x, (g+target)->y );
    float ang = getAngle((g+target)->x, (g+target)->y) - (g+gladid)->head;
    ang = getNormalAngle(ang);

    if ( dist > (g+gladid)->vis )
        fail = 1;
    if ( ang > (g+gladid)->vrad/2 && ang < 360-(g+gladid)->vrad/2 )
        fail = 1;

    if (fail){
        (g+gladid)->targetlocked = -1;
        return 0;
    }
    return 1;
}

int getLockedTargetCoords(float *x, float *y){
    loadStructFromMemory();
    if ( isLockedTargetVisible() ){
        int target = (g+gladid)->targetlocked;
        *x = (g+target)->x;
        *y = (g+target)->y;
        return 1;
    }
    return 0;
}

float getLockedTargetSpeed(){
    loadStructFromMemory();
    if (!isLockedTargetVisible())
        return 0;
    int target = (g+gladid)->targetlocked;
    return (g+target)->spd;
}

float getLockedTargetHeading(){
    loadStructFromMemory();
    if (!isLockedTargetVisible())
        return 0;
    int target = (g+gladid)->targetlocked;
    return (g+target)->head;
}

float getBlockTimeLeft(){
    loadStructFromMemory();
    return (g+gladid)->buffs[BUFF_RESIST].timeleft;
}

float getCamouflageTimeLeft(){
    loadStructFromMemory();
    return (g+gladid)->buffs[BUFF_INSIBIBLE].timeleft;
}

void fireball(float x, float y){
    waitLoad();

    if ((g+gladid)->ap >= abilitycost[ABILITY_FIREBALL]){

        (g+gladid)->ap -= abilitycost[ABILITY_FIREBALL];
        (g+gladid)->head = getNormalAngle(getAngle(x,y));
        float spdx, spdy;
        calcSidesFromAngleDist(&spdx, &spdy, 1, (g+gladid)->head);
        launchProjectile((g+gladid)->x, (g+gladid)->y, (g+gladid)->INT, spdx, -spdy, PROJECTILE_TYPE_FIREBALL);
        actioncode = ABILITY_FIREBALL;
        (g+gladid)->lockedfor = 1/(g+gladid)->cs;
    }

    saveRelease();
}

void teleport(float x, float y){
    waitLoad();

    if ((g+gladid)->ap >= abilitycost[ABILITY_TELEPORT]){

        turnToUnsafe(x,y);
        if (getDistUnsafe(x,y) <= (g+gladid)->INT + 5){
            (g+gladid)->x = x;
            (g+gladid)->y = y;
            preventCollision(x,y);
        }
        else{
            float rx, ry;
            calcSidesFromMaxDist(x, y, (g+gladid)->INT + 5, &rx, &ry);
            (g+gladid)->x += rx;
            (g+gladid)->y += ry;
            preventCollision(rx, ry);
            //printf("%.1f %.1f\n",(g+gladid)->x, (g+gladid)->y);
        }
        preventLeaving();
        (g+gladid)->ap -= abilitycost[ABILITY_TELEPORT];
        actioncode = ABILITY_TELEPORT;
        (g+gladid)->lockedfor = 1/(g+gladid)->cs;
    }

    saveRelease();
}

void charge(){
    waitLoad();

    if ((g+gladid)->ap >= abilitycost[ABILITY_CHARGE] && isLockedTargetVisible() ){
        int target = (g+gladid)->targetlocked;

        (g+gladid)->ap -= abilitycost[ABILITY_CHARGE];
        addBuff(gladid, BUFF_MOVEMENT, 1/(g+gladid)->cs, 3);

        saveRelease();

        while(getDistUnsafe((g+target)->x, (g+target)->y) > 1 ){
            waitLoad();

            if ((g+gladid)->buffs[BUFF_MOVEMENT].timeleft <= timeInterval){
                addBuff(gladid, BUFF_MOVEMENT, timeInterval*2, 3);
            }
            moveToUnsafe((g+target)->x, (g+target)->y);
            actioncode = ABILITY_CHARGE;
            (g+gladid)->lockedfor = timeInterval;

            saveRelease();
        }

        waitLoad();

        attackMeleeUnsafe((g+target)->x, (g+target)->y);
        actioncode = ABILITY_CHARGE;
        addBuff(target , BUFF_MOVEMENT, 5, 1 - (float)(g+gladid)->STR/20);

        if ( (g+gladid)->buffs[BUFF_MOVEMENT].timeleft > 0){
            (g+gladid)->lockedfor = (g+gladid)->buffs[BUFF_MOVEMENT].timeleft;
            (g+gladid)->buffs[BUFF_MOVEMENT].timeleft = timeInterval;
        }
        else
            (g+gladid)->lockedfor = 0;
    }

    saveRelease();
}

void block(){
    waitLoad();

    if ((g+gladid)->ap >= abilitycost[ABILITY_BLOCK]){
        addBuff(gladid, BUFF_RESIST, 5, 0.25 + (float)(g+gladid)->STR * 0.025);

        (g+gladid)->ap -= abilitycost[ABILITY_BLOCK];
        actioncode = ABILITY_BLOCK;
        (g+gladid)->lockedfor = 1/(g+gladid)->cs;
    }

    saveRelease();
}

void assassinate(float x, float y){
    waitLoad();

    if ((g+gladid)->ap >= abilitycost[ABILITY_ASSASSINATE]){
        int seen = doYouSeeMe(x,y);
        (g+gladid)->ap -= abilitycost[ABILITY_ASSASSINATE];
        turnToUnsafe(x,y);
        float spdx, spdy;
        calcSidesFromAngleDist(&spdx, &spdy, 1, (g+gladid)->head);

        if (!seen)
            launchProjectile((g+gladid)->x, (g+gladid)->y, (g+gladid)->dmg + (g+gladid)->AGI, spdx, -spdy, PROJECTILE_TYPE_ASSASSINATE);
        else
            launchProjectile((g+gladid)->x, (g+gladid)->y, (g+gladid)->dmg, spdx, -spdy, PROJECTILE_TYPE_ASSASSINATE);

        actioncode = ABILITY_ASSASSINATE;
        (g+gladid)->lockedfor = 1/(g+gladid)->cs;
    }

    saveRelease();
}

void camouflage(){
    waitLoad();

    if ((g+gladid)->ap >= abilitycost[ABILITY_CAMOUFLAGE]){
        addBuff(gladid, BUFF_INSIBIBLE, 1 + (float)(g+gladid)->AGI/10*3, 0);

        (g+gladid)->ap -= abilitycost[ABILITY_CAMOUFLAGE];
        actioncode = ABILITY_CAMOUFLAGE;
        (g+gladid)->lockedfor = 1/(g+gladid)->cs;
    }

    saveRelease();
}

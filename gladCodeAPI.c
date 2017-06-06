#include "gladCodeCore.c"

//inicializa o gladiador. cada processo chama esta função no setup
void registerGlad(){
    (g+gladid)->lvl = 1; //lvl inicial
    (g+gladid)->vrad = 60; //raio de visao 60 graus
    (g+gladid)->vis = 9; //alcance da visao 9 passos
    (g+gladid)->lockedfor = 0;
    setStartingPos(); //inicializa a posicao inicial de cada gladiador formando um circulo
    (g+gladid)->targetlocked = -1; //nenhum alvo fixado
    (g+gladid)->up = 0; //qual atributo vai ser melhorado quando subir de nivel
    (g+gladid)->damagedme = 0; //inteiro que convertido em binario contem bits que dizem quem deu dano no gladiador
    (g+gladid)->lasthit = 0; //angulo de onde veio o ultimo ataque
    int i;
    for (i=0 ; i<N_BUFFS ; i++){ //zera todos buffs
        (g+gladid)->buffs[i].timeleft = 0;
        (g+gladid)->buffs[i].value = 0;
    }
}

void setSTR(int n){
    (g+gladid)->STR = n;
    (g+gladid)->hp = 100 + n*20;
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
    (g+gladid)->ap = 100 + n*20;
    (g+gladid)->maxap = (g+gladid)->ap;
    (g+gladid)->cs = 0.5 + n*0.1;
}

void setName(char *str){
    strcpy((g+gladid)->name, str);
}

//indica o atributo que sera feito upgrade quando o gladiador subir de nivel
void upgradeSTR(){
    (g+gladid)->up = 1;
}

void upgradeAGI(){
    (g+gladid)->up = 2;
}

void upgradeINT(){
    (g+gladid)->up = 3;
}

void moveForward(){
    waitLoad();

    moveForwardUnsafe();
    actioncode = ACTION_MOVEMENT;
    (g+gladid)->lockedfor = timeInterval;

    saveRelease();
}

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

//se vira para uma direcao escolhida
void turnToHeading(float ang){
    waitLoad();

    (g+gladid)->head = getNormalAngle(ang);
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

float getMyX(){
    loadStructFromMemory();
    return (g+gladid)->x;
}

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

float getMyHp(){
    loadStructFromMemory();
    return (g+gladid)->hp;
}

float getMyAp(){
    loadStructFromMemory();
    return (g+gladid)->ap;
}

int getMySTR(){
    loadStructFromMemory();
    return (g+gladid)->STR;
}

int getMyAGI(){
    loadStructFromMemory();
    return (g+gladid)->AGI;
}

int getMyINT(){
    loadStructFromMemory();
    return (g+gladid)->INT;
}

float getMyHeading(){
    loadStructFromMemory();
    return (g+gladid)->head;
}

//recebe um ponto, retorna a distancia entre estes pontos e o gladiador
float getDist(float x, float y){
    loadStructFromMemory();
    return getDistUnsafe(x,y);
}

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
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0){
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

//atribui as coordenadas do inimigo mais próximo dentro o campo de visao
int getCloseEnemy(float *x, float *y){
    loadStructFromMemory();
    int i, closeri=-1, lowerdist;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0){
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

//atribui as coordenadas do inimigo mais longe  dentro o campo de visao
int getFarEnemy(float *x, float *y){
    loadStructFromMemory();
    int i, fari=-1, higherdist;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0){
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
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0){
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
        if (i != gladid && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0){
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

//se encontrar inimigo no ponto, retorna valores entre 0 e 1, indicando a percentagem de vida do alvo no ponto
float getEnemyHealth(float x, float y){
    loadStructFromMemory();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0)
            break;
    }
    if (i == nglad)
        return 0;
    else{
        return (float)((g+i)->hp) / (g+i)->maxhp;
    }
}

//verifica qual a especialidade o alvo no ponto, STR, AGI e INT
int areYouAWarrior(float x, float y){
    loadStructFromMemory();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0)
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
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0)
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
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0)
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

//se o gladiador está no campo de visao do inimigo no ponto, retorna 1
int doYouSeeMe(float x, float y){
    loadStructFromMemory();
    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0)
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

//lana um projetil em direcao ao ponto
int attackRanged(float x, float y){
    waitLoad();

    turnToUnsafe(x,y);
    float spdx, spdy;
    calcSidesFromAngleDist(&spdx, &spdy, 1, (g+gladid)->head);

    int projectiletype = PROJECTILE_TYPE_ATTACK;
    if ((g+gladid)->buffs[BUFF_INVISIBLE].timeleft > 0)
        projectiletype = PROJECTILE_TYPE_STUN;

    launchProjectile((g+gladid)->x, (g+gladid)->y, (g+gladid)->dmg, spdx, -spdy, projectiletype);

    actioncode = ACTION_RANGED_ATTACK;
    (g+gladid)->lockedfor = 1/(g+gladid)->as;

    saveRelease();
}

//angulo de onde originou o ultimo ataque recebido
float whereThatCameFrom(){
    loadStructFromMemory();
    return (g+gladid)->lasthit;
}

//fixa em um gladiador no ponto. util para manter o foco em um inimigo
int lockOnTarget(float x, float y){
    waitLoad();

    int i;
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->x == x && (g+i)->y == y && (g+i)->hp > 0 && (g+i)->buffs[BUFF_INVISIBLE].timeleft <= 0){
            (g+gladid)->targetlocked = i;
            saveRelease();
            return 1;
        }
    }
    saveRelease();
    return 0;
}

//verifica se o alvo fixado esta no campo de visao
int isLockedTargetVisible(){
    loadStructFromMemory();
    if ( (g+gladid)->targetlocked == -1 )
        return 0;

    int fail = 0;

    int target = (g+gladid)->targetlocked;
    if ((g+target)->hp <= 0)
        fail = 1;

    if ((g+target)->buffs[BUFF_INVISIBLE].timeleft > 0)
        fail = 1;

    float dist = getDistUnsafe( (g+target)->x, (g+target)->y );
    float ang = getAngle((g+target)->x, (g+target)->y) - (g+gladid)->head;
    ang = getNormalAngle(ang);

    if ( dist > (g+gladid)->vis )
        fail = 1;
    if ( ang > (g+gladid)->vrad/2 && ang < 360-(g+gladid)->vrad/2 )
        fail = 1;

    if (fail){
        waitLoad();
        (g+gladid)->targetlocked = -1;
        saveRelease();
        return 0;
    }
    return 1;
}

//recebe as coordenadas do alvo fixado
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

//direcao do alvo fixado
float getLockedTargetHeading(){
    loadStructFromMemory();
    if (!isLockedTargetVisible())
        return 0;
    int target = (g+gladid)->targetlocked;
    return (g+target)->head;
}

//direcao relativa do alvo fixado
float getLockedTargetRelativeHeading(){
    loadStructFromMemory();
    if (!isLockedTargetVisible())
        return 0;
    int target = (g+gladid)->targetlocked;
    float dif = getNormalAngle((g+target)->head - (g+gladid)->head);
    return dif;
}

//quanto tempo para acabar o buff de protecao
float getBlockTimeLeft(){
    loadStructFromMemory();
    return (g+gladid)->buffs[BUFF_RESIST].timeleft;
}

//quanto tempo para acabar o buff de invisibilidade
float getAmbushTimeLeft(){
    loadStructFromMemory();
    return (g+gladid)->buffs[BUFF_INVISIBLE].timeleft;
}

//lanca um projetil do tipo fireball
void fireball(float x, float y){
    waitLoad();

    if ((g+gladid)->ap >= abilitycost[ABILITY_FIREBALL]){

        (g+gladid)->ap -= abilitycost[ABILITY_FIREBALL];
        (g+gladid)->head = getNormalAngle(getAngle(x,y));
        float spdx, spdy;
        calcSidesFromAngleDist(&spdx, &spdy, 1, (g+gladid)->head);
        launchProjectile((g+gladid)->x, (g+gladid)->y, (g+gladid)->INT * 0.5, spdx, -spdy, PROJECTILE_TYPE_FIREBALL);
        actioncode = ABILITY_FIREBALL;
        (g+gladid)->lockedfor = 1/(g+gladid)->cs;
    }

    saveRelease();
}

//vai para o ponto, ou o mais proximo que conseguir dele
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

//se move com velocidade aumentada para o alvo fixado, e entao ataca.
void charge(){
    if (isLockedTargetVisible()){
        waitLoad();
        if ((g+gladid)->ap >= abilitycost[ABILITY_CHARGE] ){
            int target = (g+gladid)->targetlocked;

            (g+gladid)->ap -= abilitycost[ABILITY_CHARGE];
            addBuff(gladid, BUFF_MOVEMENT, 1/(g+gladid)->cs, 4);

            saveRelease();

            //se move em direcao ao alvo
            int stopcharge = 0;
            while(getDistUnsafe((g+target)->x, (g+target)->y) > 1 && !stopcharge ){
                waitLoad();

                if ((g+gladid)->buffs[BUFF_MOVEMENT].timeleft <= timeInterval){
                    addBuff(gladid, BUFF_MOVEMENT, timeInterval*2, 4);
                }
                moveToUnsafe((g+target)->x, (g+target)->y);
                actioncode = ABILITY_CHARGE;
                (g+gladid)->lockedfor = timeInterval;

                //para a charge se o alvo esta invisivel ou saiu do alcance (teleport)
                if ( (g+target)->buffs[BUFF_INVISIBLE].timeleft > 0 || getDistUnsafe((g+target)->x, (g+target)->y) > (g+gladid)->vis )
                    stopcharge = 1;

                saveRelease();
            }

            waitLoad();

            if (!stopcharge){
                attackMeleeUnsafe((g+target)->x, (g+target)->y);
                actioncode = ABILITY_CHARGE;
                addBuff(target , BUFF_MOVEMENT, 5, 1 - (float)(g+gladid)->STR/20);
            }

            if ( (g+gladid)->buffs[BUFF_MOVEMENT].timeleft > 0){
                (g+gladid)->lockedfor = (g+gladid)->buffs[BUFF_MOVEMENT].timeleft;
                (g+gladid)->buffs[BUFF_MOVEMENT].timeleft = timeInterval;
            }
            else
                (g+gladid)->lockedfor = 0;
        }

        saveRelease();
    }
}

//ganha buff de protecao para ataques pela frente
void block(){
    waitLoad();

    if ((g+gladid)->ap >= abilitycost[ABILITY_BLOCK]){
        addBuff(gladid, BUFF_RESIST, 5, 0.2 + (float)(g+gladid)->STR * 0.06);

        (g+gladid)->ap -= abilitycost[ABILITY_BLOCK];
        actioncode = ABILITY_BLOCK;
        (g+gladid)->lockedfor = 1/(g+gladid)->cs;
    }

    saveRelease();
}

//ataca alvo no ponto, se estiver perto atordoa, se o alvo nao enxergar o gladiador e/ou estiver com stun, causa dano extra
void assassinate(){
    if (isLockedTargetVisible()){
        waitLoad();

        if ((g+gladid)->ap >= abilitycost[ABILITY_ASSASSINATE]){
            float x,y;
            getLockedTargetCoords(&x, &y);
            int seen = doYouSeeMe(x,y);
            (g+gladid)->ap -= abilitycost[ABILITY_ASSASSINATE];
            turnToUnsafe(x,y);
            float spdx, spdy;
            calcSidesFromAngleDist(&spdx, &spdy, 1, (g+gladid)->head);

            float damage = (g+gladid)->dmg;
            if (!seen || (g+gladid)->buffs[BUFF_INVISIBLE].timeleft > 0)
                damage += (g+gladid)->AGI;

            int target = (g+gladid)->targetlocked;
            if ((g+target)->buffs[BUFF_STUN].timeleft > 0)
                damage += (g+gladid)->AGI;

            int projectiletype = PROJECTILE_TYPE_ATTACK;
            if ((g+gladid)->buffs[BUFF_INVISIBLE].timeleft > 0)
                projectiletype = PROJECTILE_TYPE_STUN;

            launchProjectile((g+gladid)->x, (g+gladid)->y, damage, spdx, -spdy, projectiletype);

            actioncode = ABILITY_ASSASSINATE;
            (g+gladid)->lockedfor = 1/(g+gladid)->cs;
        }

        saveRelease();
    }
}

//ganha buff de invisibilidade, impossiblitando que inimigos enxergue o gladiador e faz que proximo ataque cause stun
void ambush(){
    waitLoad();

    if ((g+gladid)->ap >= abilitycost[ABILITY_AMBUSH]){
        addBuff(gladid, BUFF_INVISIBLE, 2 + (g+gladid)->AGI * 0.4, 0);

        (g+gladid)->ap -= abilitycost[ABILITY_AMBUSH];
        actioncode = ABILITY_AMBUSH;
        (g+gladid)->lockedfor = 1/(g+gladid)->cs;
    }

    saveRelease();
}

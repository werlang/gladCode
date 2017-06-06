#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<Windows.h>

#include"GladCodeGlobals.c"
#include "GladCodeSMem.c"

void startStructFromMemory(){
    g = (struct gladiador*)malloc(sizeof(struct gladiador) * (nglad));
    int i;
    //todos começam com lvl 0, para saber quem já foi carregado da memoria
    for (i=0 ; i<nglad ; i++)
        (g+i)->lvl = 0;

    //o primeiro executa o setup
    if (gladid == 0){
        setup();
        if (!checkSetup()){
            printf("Error setup %i\n",gladid);
            return;
        }
        waitForMutex();
        saveStructToMemory();
        releaseMutex();
        printf("%s (thread %i) setup complete.\n",(g+gladid)->name, gladid);
    }
    else{
        //todos os outros esperam pelo gladiador anterior
        while ((g+gladid-1)->lvl != 1){
            waitForMutex();
            loadStructFromMemory();
            releaseMutex();
        }
        setup();
        if (!checkSetup()){
            printf("Error setup %i\n",gladid);
            return;
        }
        waitForMutex();
        saveStructToMemory();
        releaseMutex();
        printf("%s (thread %i) setup complete.\n",(g+gladid)->name, gladid);
    }
    //espera o ultimo estar pronto
    while ((g+nglad-1)->lvl != 1){
        waitForMutex();
        loadStructFromMemory();
        releaseMutex();
    }
    printf("%s (thread %i) entered the arena.\n", (g+gladid)->name, gladid);

}

//converte de graus para radianos
float gToRad(float g){
    return g*M_PI/180;
}

//converte de rad para graus
float radToG(float r){
    return 180*r/M_PI;
}

float getAngleFromAB(float xa, float ya, float xb, float yb){
    float dx = xb - xa;
    float dy = ya - yb;
    float ang;
    if (dy == 0 && dx == 0)
        return 0;
    else if (dy > 0)
        ang = radToG(atan(dx/dy));
    else if (dy < 0)
        ang = radToG(atan(dx/dy))+180;
    else
        ang = 90 * (dx/abs(dx));
    return ang;
}

//recebe hipotenusa e angulo, calcula calcula e atribui por referencia os catetos
void calcSidesFromAngleDist(float *dx, float *dy, float d, float a){
    *dx = sin(gToRad(a))*d;
    *dy = cos(gToRad(a))*d;
}

//ajusta angulos para estarem entro 0-360, onde 0==360==note
float getNormalAngle(float ang){
    if (ang >= 360)
        ang -= 360;
    if (ang < 0)
        ang += 360;
    return ang;
}

//recebe um ponto, retorna o angulo o ponto do glad e o ponto recebido
float getAngle(float x, float y){
    return getAngleFromAB((g+gladid)->x, (g+gladid)->y, x, y);
}

float getDistUnsafe(float x, float y){
    float dx = (g+gladid)->x - x;
    float dy = (g+gladid)->y - y;
    return sqrt( pow(dx,2) + pow(dy,2) );
}

//calcula por semelhança de triangulos os catetos maxx, e maxy, a partir do triangulo de catetos destx, desty e hipotenusa maxdist
void calcSidesFromMaxDist(float destx, float desty,  float maxdist, float *maxx, float *maxy){
    float dx1, dy1, dh1;
    dh1 = getDistUnsafe(destx,desty);
    dx1 = destx - (g+gladid)->x;
    dy1 = desty - (g+gladid)->y;
    *maxx = maxdist/dh1 * dx1;
    *maxy = maxdist/dh1 * dy1;
}

void setDamagedMe(int me, int en){
    //1 caso o gladiador já tenha me acertado, do contrario 0.
    //o damagedme é um inteiro representado pela conversão binária dos 0s e 1s da convenção acima.
    //o gladiador 0 é o bit menos significativo.
    int current = (g+me)->damagedme;
    int cbit = (current >> en)%2;
    if (!cbit)
        (g+me)->damagedme += 1<<en;
}

int haveYouDamagedMe(int en){
    //cada
    int current = (g+gladid)->damagedme;
    int cbit = (current >> en)%2;
    return cbit;
}

void addBuff(int id, int code, float timeleft, float value){
    if (code == BUFF_BURN){
        //caso ja tenha um burn, recumeça o tempo, e adiciona o dano que faltava
        if ((g+id)->buffs[code].timeleft > 0){
            float remaining = (g+id)->buffs[code].value * (g+id)->buffs[code].timeleft/2;
            (g+id)->buffs[code].value = value + remaining;
        }
        else
            (g+id)->buffs[code].value = value;
    }
    else if (code == BUFF_MOVEMENT){
        //o valor do buff novo substitui o do anterior
        if ((g+id)->buffs[code].timeleft > 0)
            (g+id)->spd /= (g+id)->buffs[code].value;
        (g+id)->buffs[code].value = value;
        (g+id)->spd *= value;
    }
    else if (code == BUFF_RESIST){
        (g+id)->buffs[code].value = value;
    }
    (g+id)->buffs[code].timeleft = timeleft;
}

void dealDamage(int id, float value){
    //se estiver invisivel, causa stun
    if ((g+gladid)->buffs[BUFF_INVISIBLE].timeleft > 0){
        addBuff(id, BUFF_STUN, 1 , 0);
    }
    //caso a vitima tenha o buff de resistencia e esteja virado para o atacante, leva menos dano
    if ((g+id)->buffs[BUFF_RESIST].timeleft > 0){
        if ((g+id)->head >= (g+id)->lasthit - (g+id)->vrad/2 && (g+id)->head <= (g+id)->lasthit + (g+id)->vrad/2){
            value *= (1 - (g+id)->buffs[BUFF_RESIST].value);
        }
        else{
            value *= (1 - (g+id)->buffs[BUFF_RESIST].value / 2);
        }
    }
    (g+id)->hp -= value;
}

void updateBuffs(){
    int i,j;
    for (i=0 ; i<N_BUFFS ; i++){
        if ((g+gladid)->buffs[i].timeleft > 0){
            (g+gladid)->buffs[i].timeleft -= timeInterval;

            if (i == BUFF_BURN){
                (g+gladid)->hp -= (g+gladid)->buffs[i].value / 2 * (1 - (g+gladid)->buffs[BUFF_RESIST].value/2) * timeInterval;
            }
            //caso o buff acabe, reduz de volta a velocidade
            else if (i == BUFF_MOVEMENT){
                if ((g+gladid)->buffs[i].timeleft <= 0){
                    (g+gladid)->spd /= (g+gladid)->buffs[i].value;
                }
            }
            //se atacar perde o buff
            else if (i == BUFF_INVISIBLE){
                if (actioncode != ACTION_WAITING && actioncode != ACTION_MOVEMENT && actioncode != ABILITY_AMBUSH && actioncode != ACTION_NONE)
                    (g+gladid)->buffs[i].timeleft = 0;
            }
        }
        else {
            (g+gladid)->buffs[i].value = 0;
            (g+gladid)->buffs[i].timeleft = 0;
        }
    }
}

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
    //varre a lista de projeteis
    while (a != NULL){
        i++;
        int hitglad = 0;
        waitForMutex();
        loadStructFromMemory();
        //numero de intervalos que um projetil anda num mesmo intervalo de tempo do gladiador
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
                    //acertou
                    if ( xg >= xl && xg <= xr && yg >= yl && yg <= yr ){
                        (g+j)->lasthit = getNormalAngle(getAngleFromAB((g+j)->x, (g+j)->y, a->x - a->spdx / travelunit, a->y - a->spdy / travelunit));
                        setDamagedMe(j, gladid);
                        dealDamage(j, a->dmg);
                        hitglad = 1;
                        break;
                    }
                }
            }
            if (hitglad)
                break;
        }
        if (a->dist >= PROJECTILE_MAX_DISTANCE || hitglad){
            //causa burn da fireball
            if (a->type == PROJECTILE_TYPE_FIREBALL){
                int m;
                for (m=0 ; m<nglad ; m++){
                    float dx = (g+m)->x - a->x;
                    float dy = (g+m)->y - a->y;
                    float dist = sqrt( pow(dx,2) + pow(dy,2) );
                    if (dist <= 2){
                        float dmg = (1-(dist/2)) * a->dmg * 3;
                        addBuff(m, BUFF_BURN, 2, dmg);
                        (g+m)->lasthit = getNormalAngle(getAngleFromAB((g+m)->x, (g+m)->y, a->x - a->spdx / travelunit, a->y - a->spdy / travelunit));
                        setDamagedMe(m, gladid);
                    }
                }
            }
            else if (hitglad && a->type == PROJECTILE_TYPE_STUN){
                addBuff(j, BUFF_STUN, 1 , 0);
            }
            struct projectile *t = a->next;
            removeProjectile(a);
            a = t;
        }
        else
            a = a->next;
        //printf("P%i X:%.1f Y:%.1f SX:%.1f SY:%.1f\n",i,a->x,a->y,a->spdx,a->spdy);
        saveStructToMemory();
        releaseMutex();
    }
}

void printOutput(){
    FILE *arq = fopen("output.txt","a");
    if (arq == NULL){
        arq = fopen("output.txt","w");
        fclose(arq);
        arq = fopen("output.txt","a");
    }
    fputs(outString, arq);
    fclose(arq);
}

//atualiza tudo que depende do tempo no gladiador
void updateTime(){
    int i;
    waitForMutex();
    loadStructFromMemory();
    //lvl up dos gladiadores que bateram no que morreu
    if ((g+gladid)->hp <= 0 && !endsim){
        for (i=0 ; i<nglad ; i++){
            if (haveYouDamagedMe(i) && i != gladid && (g+i)->hp > 0){
                (g+i)->lvl++;
                if ( (g+i)->STR < 10 && (g+i)->up == 1 )
                    (g+i)->STR++;
                if ( (g+i)->AGI < 10 && (g+i)->up == 2 )
                    (g+i)->AGI++;
                if ( (g+i)->INT < 10 && (g+i)->up == 3 )
                    (g+i)->INT++;

                //printf("%s %i %i %i\n",(g+i)->name, (g+i)->STR, (g+i)->AGI, (g+i)->INT);
            }
        }
        endsim = 1;
    }
    saveStructToMemory();
    releaseMutex();

    //atualiza o tempo na memoria, e espera pela acão dos outros, para evitar que um ande na frente na simulação
    updateSimCounter(gladid, timeInterval);
    double cont[nglad], lower;
    int loweri, timeout = 10000;
    do{
        getAllSimCounters(cont);
        int alive = 0;
        lower = -1;
        for (i=0 ; i<nglad ; i++){
            if ( (g+i)->hp > 0 && i != gladid ){
                alive++;
                if ( lower == -1 || cont[i] < lower ){
                    lower = cont[i];
                    loweri = i;
                }
            }
        }
        timeout--;
        //printf("%i %.1lf %.1lf lower %i alive\n",gladid,cont[gladid], lower, alive);
        if (!alive)
            endsim = 1;
    }while (cont[gladid] > lower && !endsim && timeout > 0);
    if (timeout == 0){
        printf("%s (thread %i) timed out!\n",(g+loweri)->name,loweri);
        updateSimCounter(loweri, timeInterval);
    }

    if ((g+gladid)->hp > 0 && !endsim){
        waitForMutex();
        loadStructFromMemory();

        //recupera ap
        (g+gladid)->ap += (g+gladid)->maxap * AP_RECOVERY * timeInterval;
        if ((g+gladid)->ap > (g+gladid)->maxap)
            (g+gladid)->ap = (g+gladid)->maxap;

        if (cont[gladid] >= timeLimit)
            (g+gladid)->hp -= timeInterval;

        updateBuffs();

        //passa fica mais proximo de poder agir de novo
        if ((g+gladid)->lockedfor > 0)
            (g+gladid)->lockedfor -= timeInterval;

        saveStructToMemory();
        releaseMutex();
    }
}

//grava na outString o que o gladiador fez no presente momento
void recordSteps(){
    double simtime = getSimCounter(gladid);
    char buffer[500], buffstr[100]="";
    int i;

    for (i=0 ; i<N_BUFFS ; i++){
        if (i == 0)
            sprintf(buffstr, "[%.2f,%.1f]",(g+gladid)->buffs[i].value,(g+gladid)->buffs[i].timeleft);
        else{
            sprintf(buffer, ",[%.2f,%.1f]",(g+gladid)->buffs[i].value,(g+gladid)->buffs[i].timeleft);
            strcat(buffstr, buffer);
        }
    }
    sprintf(buffer, "%.1lf|%i|%s|%i|%i|%i|%i|%.2f|%.2f|%.2f|%.1f|%.1f|%.1f|%.2f|%.2f|%.2f|%s|%i\n",
            simtime, //time elapsed
            gladid, //thread num
            (g+gladid)->name, //glad name
            (g+gladid)->lvl, //lvl
            (g+gladid)->STR, //STR
            (g+gladid)->AGI, //AGI
            (g+gladid)->INT, //INT
            (g+gladid)->spd, //movement speed
            (g+gladid)->as, //attack speed
            (g+gladid)->cs, //cast speed
            (g+gladid)->x, //X
            (g+gladid)->y, //Y
            (g+gladid)->head, //heading (0-359.9)
            (g+gladid)->lockedfor, //time until can act again
            (g+gladid)->hp, //life
            (g+gladid)->ap, //ability points
            buffstr, //buffs array
            actioncode); //code for action done
    //printf(buffer);
    if (outString == NULL){
        outString = (char*)malloc(sizeof(char) * (strlen(buffer)+1) );
        strcpy(outString, buffer);
    }
    else{
        int size = sizeof(char) * (strlen(outString));
        outString = (char*)realloc(outString, size + sizeof(char) * (strlen(buffer)+1) );
        strcat(outString, buffer);
    }

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

//impede que um gladiador invada o hitbox de outro
void preventCollision(float lastdx, float lastdy){
    int i;
    float h;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0){
            h = getDistUnsafe( (g+i)->x, (g+i)->y );
            if (h < GLAD_HITBOX){
                if (h == 0){
                    (g+gladid)->x -= lastdx;
                    (g+gladid)->y -= lastdy;
                    h = getDistUnsafe( (g+i)->x, (g+i)->y );
                }
                float dx, dy, dfx, dfy;
                dx = (g+i)->x - (g+gladid)->x;
                dy = (g+i)->y - (g+gladid)->y;
                dfx = GLAD_HITBOX * dx / h;
                dfy = GLAD_HITBOX * dy / h;

                (g+gladid)->x = (g+i)->x - dfx;
                (g+gladid)->y = (g+i)->y - dfy;
            }
        }
    }
}

//as funcoes unsafe cumprem o papel das mesmas funções da API, porém sem mexer na memória, mutex, alterar o actioncode ou trancar o gladiador.
//servem para ser usadas dentro de outras funções.
void turnToUnsafe(float x, float y){
    (g+gladid)->head = getNormalAngle(getAngle(x,y));
}

void moveForwardUnsafe(){
    float hip = (g+gladid)->spd * timeInterval;
    float ang = (g+gladid)->head;
    float dx, dy;
    calcSidesFromAngleDist(&dx, &dy, hip, ang);

    (g+gladid)->x += dx;
    (g+gladid)->y -= dy;

    preventLeaving();
    preventCollision(dx,-dy);
}

void moveToUnsafe(float x, float y){
    turnToUnsafe(x,y);
    if (getDistUnsafe(x,y) < (g+gladid)->spd*timeInterval){
        (g+gladid)->x = x;
        (g+gladid)->y = y;
        preventCollision(x,y);
        preventLeaving();
    }
    else{
        moveForwardUnsafe();
    }
}

void attackMeleeUnsafe(float x, float y){
    turnToUnsafe(x,y);
    int i;
    for (i=0 ; i<nglad ; i++){
        if (i != gladid && (g+i)->hp > 0){
            float dist = getDistUnsafe((g+i)->x, (g+i)->y);
            float ang = getAngle((g+i)->x, (g+i)->y) - (g+gladid)->head;
            ang = getNormalAngle(ang);
            if ( dist <= 1 && (ang <= 90 || ang >= 270) ){ //180g de raio de ataque
                (g+i)->lasthit = getNormalAngle(getAngleFromAB((g+i)->x, (g+i)->y, (g+gladid)->x, (g+gladid)->y));
                setDamagedMe(i, gladid);
                dealDamage(i, (g+gladid)->dmg * 1.2);
            }
        }
    }
}

//define a posicao e direcao inicial do gladiador
void setStartingPos(){
    float centerX = screenW/2;
    float centerY = screenH/2;
    float radius = screenH/2 * 0.8;
    float dang = 360/nglad;
    float x,y;
    calcSidesFromAngleDist(&x, &y, radius, dang * gladid);
    (g+gladid)->x = centerX + x;
    (g+gladid)->y = centerY + y;
    turnToUnsafe(centerX, centerY);
}

//aguarda o tempo ate o gladiador estar pronto para agir novamente
void waitForLockedStatus(){
    //updates nos projeteis, tempo e grava na arquivo, mesmo quando o gladiador nao pode agir
    while ( ((g+gladid)->lockedfor > 0 || (g+gladid)->buffs[BUFF_STUN].timeleft > 0) && !endsim){
        updateProjectiles();
        updateTime();
        recordSteps();
        actioncode = ACTION_WAITING;
    }
}

//espera a mutex estar liberada e carrega o conteudo da memoria na struct
void waitLoad(){
    waitForMutex();
    loadStructFromMemory();
}

//guarda struct na memoria, libera a mutex e espera o gladiador estar pronto para agir de novo
void saveRelease(){
    saveStructToMemory();
    releaseMutex();
    waitForLockedStatus();
}

int main(int argc, char *argv[]){
    srand(time(NULL));
    g = NULL; //struct dos gladiadores
    p = NULL; //struct dos projeteis
    outString = NULL; //string que vai ser escrita no arquivo de saida

    gladid = atoi(argv[1]);
    nglad = atoi(argv[2]);

    startMutex();

    startStructFromMemory();

    //loop() é o código do usuário
    while(!kbhit() && !endsim){
        actioncode = ACTION_NONE;
        loop();
        if (actioncode == ACTION_NONE){
            updateProjectiles();
            updateTime();
            recordSteps();
        }
    }
    if (endsim){
        waitForMutex();
        if ((g+gladid)->hp <= 0)
            printf("%s (thread %i) died\n",(g+gladid)->name, gladid);
        else
            printf("Winner: %s (thread %i)\n", (g+gladid)->name, gladid);
        printOutput();
        releaseMutex();
    }
}

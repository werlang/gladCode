#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<Windows.h>
#include"GladCodeStruct.c"
#include "GladCodeSMem.c"
#include"GladCodeAPI.c"

int endsim = 0;
char *outString = NULL;

void startStructFromMemory(){
    int size = sizeof(struct gladiador) * nglad;
    g = (struct gladiador*)malloc(size);
    int i;
    for (i=0 ; i<nglad ; i++)
        (g+i)->lvl = -1;

    g = (struct gladiador*)readFromSharedMemory(size);

    setup();
    if (!checkSetup()){
        printf("Error setup %i\n",gladid);
        return;
    }

    writeOnSharedMemory(g, size);

    printf("%s (thread %i) setup complete.\n",(g+gladid)->name, gladid);
}

void waitForSetup(){
    int size = sizeof(struct gladiador) * nglad;
    int i, ready = 0, message = 0;
    while (!ready){
        if (g != NULL)
            free(g);
        g = (struct gladiador*)readFromSharedMemory(size);
        for (i=0 ; i<nglad ; i++){
            if ( (g+i)->lvl != 1 ){
                if (!message){
                    printf("%s (thread %i) is ready to fight...\n", (g+gladid)->name, gladid);
                    message = 1;
                }
                break;
            }
        }
        if (i==nglad)
            ready = 1;
    }
    printf("%s (thread %i) entered the arena.\n", (g+gladid)->name, gladid);
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
    HANDLE hThread;

void updateTime(){
    int i;
    if ((g+gladid)->hp <= 0){
        for (i=0 ; i<nglad ; i++){
            if (haveYouDamagedMe(i)){
                (g+i)->lvl++;
                if ( (g+i)->STR < 10 && (g+i)->up == 1 )
                    (g+i)->STR++;
                if ( (g+i)->AGI < 10 && (g+i)->up == 2 )
                    (g+i)->AGI++;
                if ( (g+i)->INT < 10 && (g+i)->up == 3 )
                    (g+i)->INT++;

                //printf("%i %i %i",(g+i)->STR, (g+i)->AGI, (g+i)->INT);
            }
        }
        endsim = 1;
    }
    updateSimCounter(gladid, timeInterval);
    double cont[nglad], lower;
    do{
        getAllSimCounters(cont);
        int alive = 0;
        lower = -1;
        for (i=0 ; i<nglad ; i++){
            if ( (g+i)->hp > 0 && i != gladid ){
                alive++;
                if ( lower == -1 || cont[i] < lower )
                    lower = cont[i];
            }
        }
        if (!alive)
            endsim = 1;
    }while (cont[gladid] > lower && !endsim);

    waitForMutex();
    loadStructFromMemory();
    if ((g+gladid)->lockedfor > 0)
        (g+gladid)->lockedfor -= timeInterval;
    saveStructToMemory();
    releaseMutex();
}

void recordSteps(){
    double simtime = getSimCounter(gladid);
    char buffer[500];
    //time|thread num|nome|x|y|heading|lockedfor|hp|ap|action code
    sprintf(buffer, "%.1lf|%i|%s|%.1f|%.1f|%.1f|%.1f|%i|%i|%i\n", simtime, gladid, (g+gladid)->name, (g+gladid)->x, (g+gladid)->y, (g+gladid)->head, (g+gladid)->lockedfor, (g+gladid)->hp, (g+gladid)->ap, actioncode);
    printf(buffer);
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

int main(int argc, char *argv[]){
    srand(time(NULL));

    gladid = atoi(argv[1]);
    nglad = atoi(argv[2]);

    startMutex();

    waitForMutex();
    startStructFromMemory();
    releaseMutex();

    waitForSetup();

    while(!kbhit() && !endsim){
        actioncode = 0;
        loop();
        updateProjectiles();
        updateTime();
        recordSteps();
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





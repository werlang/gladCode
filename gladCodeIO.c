#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<Windows.h>
#include"GladCodeAPI.c"

void startStructFromFile(){
    g = (struct gladiador*)malloc(sizeof(struct gladiador)*(nglad));
    int i;
    for (i=0 ; i<nglad ; i++)
        (g+i)->lvl = -1;

    FILE *arq;
    arq = fopen("struct","rb");
    if (arq != NULL)
        fread(g, sizeof(struct gladiador), nglad, arq);
    fclose(arq);

    setup();
    if (!checkSetup()){
        printf("Error setup %i\n",gladid);
        return;
    }

    arq = fopen("struct","wb");
    fwrite(g, sizeof(struct gladiador), nglad, arq);
    fclose(arq);

    printf("%s (thread %i) setup complete.\n",(g+gladid)->name, gladid);
}

int setupReady(){
    FILE *arq;
    int ready = 1, i;
    arq = fopen("struct","rb");
    fread(g, sizeof(struct gladiador), nglad, arq);
    fclose(arq);
    for (i=0 ; i<nglad ; i++){
        if ( (g+i)->lvl != 1 )
            return 0;
    }
    return 1;
}

void loadStructFromFile(){
    FILE *arq = fopen("struct","rb");
    fread(g, sizeof(struct gladiador), nglad, arq);
    fclose(arq);
}

void saveStructToFile(){
    FILE *arq = fopen("struct","wb");
    fwrite(g, sizeof(struct gladiador), nglad, arq);
    fclose(arq);
}

void printStruct(){
    FILE *arq = fopen("output.txt","a");
    char str[200];
    sprintf(str, "%i %s X:%.1f Y:%.1f H:%.1f HP:%i AP:%i\n", gladid, (g+gladid)->name, (g+gladid)->x, (g+gladid)->y, (g+gladid)->head, (g+gladid)->hp, (g+gladid)->ap);
    fputs(str, arq);
    //printf(str);
    fclose(arq);
}

void lockFile(){
    FILE *arq = fopen("lock","wb");
    int value = 1;
    fwrite(&value, sizeof(int), 1, arq);
    fclose(arq);
}

void unlockFile(){
    FILE *arq = fopen("lock","wb");
    int value = 0;
    fwrite(&value, sizeof(int), 1, arq);
    fclose(arq);
}

int isFileLocked(){
    FILE *arq = fopen("lock","rb");
    if (arq == NULL)
        return 0;
    int value;
    fread(&value, sizeof(int), 1, arq);
    fclose(arq);
    return value;
}

void updateTime(){
    if ((g+gladid)->lockedfor > 0)
        (g+gladid)->lockedfor -= timeInterval;

    if ((g+gladid)->hp <= 0){
        int i;
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
    }
}

int main(int argc, char *argv[]){
    srand(time(NULL));

    gladid = atoi(argv[1]);
    nglad = atoi(argv[2]);
    startStructFromFile();
    while(!setupReady());

    while(!kbhit() && (g+gladid)->hp > 0){
        while(isFileLocked());
        lockFile();
        loadStructFromFile();
        loop();
        updateProjectiles();
        updateTime();
        saveStructToFile();
        unlockFile();
        printStruct();
        //printf("%i: %i %.1f| ",gladid, (g+gladid)->hp, (g+gladid)->lockedfor);
    }
    if ((g+gladid)->hp <= 0)
        printf("%s (thread %i) died\n",(g+gladid)->name, gladid);
}

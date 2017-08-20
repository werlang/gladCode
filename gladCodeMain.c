#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "GladCodeGlobals.c"
#include "GladCodeSMem.c"
//#include "gladCodeRuntimeRender.c"
#include "gladCodeUI.c"

struct params {
    char code[50];
    char name[50];
    int id;
    int n;
    float life;
    int lvl, STR, AGI, INT;
};

char *getTime(){
    time_t now;
    if ( time(&now) != (time_t)(-1) ){
        struct tm *mytime = localtime(&now);
        if ( mytime ){
            char timestamp[20];
            if ( strftime(timestamp, sizeof timestamp, "%y%m%d%H%M%S", mytime) ){
                return timestamp;
            }
        }
    }
    return NULL;
}

//thread que compila e roda cada gladiador
DWORD WINAPI thread(LPVOID p){
    struct params *input = p;

    printf("thread started received %s %i %i %s %.1f %i %i %i %i\n",input->code,input->id, input->n, input->name, input->life, input->lvl, input->STR, input->AGI, input->INT);

    char call[100];
    sprintf(call, "copy /Y usr\\%s tmp\\%s", input->code, input->code);
    system(call);
    sprintf(call, "tcc\\tcc -o tmp\\%s.exe tmp\\%s", input->code, input->code);
    printf("Compiling %s...\n",input->code);
    system(call);
    printf("%s compilled.\n",input->code);

    if (input->life < 100 || input->lvl > 1)
        sprintf(call, "tmp\\%s.exe %i %i %s %.1f %i %i %i %i",input->code, input->id, input->n, input->name, input->life, input->lvl, input->STR, input->AGI, input->INT);
    else
        sprintf(call, "tmp\\%s.exe %i %i %s",input->code, input->id, input->n, input->name);
    printf("Running %s.exe...\n",input->code);
    system(call);
    printf("%s.exe stopped.\n",input->code);
    return 0;
}

void getAtributos(char atributos[25][100], char original[]){
    char *b, *e;
    char buffer[strlen(original)];
    strcpy(buffer, original);
    int ca;
    b = buffer;
    e = strstr(buffer, "|");
    *e = '\0';
    strcpy(atributos[0], b);
    b = e+1;
    ca = 1;
    while(e != NULL){
        e = strstr(b, "|");
        if (e == NULL){
            strcpy(atributos[ca], b);
            ca++;
        }
        else{
            *e = '\0';
            strcpy(atributos[ca], b);
            ca++;
            b = e+1;
        }
        //printf("%s\n",atributos[ca-1]);
    }

}

//o output contém contém em sequencia as ações de cada gladiador, este função ordena as linhas por tempo.
void mergeOutput(int c[]){
    FILE *arq = fopen("tmp/output.txt","w");
    FILE *temparq[nglad];
    char filename[50];
    int i, j;
    for (i=0 ; i<nglad ; i++){
        sprintf(filename, "tmp\\output%i.txt",i);
        temparq[i] = fopen(filename,"r");
        if (temparq == NULL)
            return;
    }

    char buffer[500];
    char atributos[25][100];


    fprintf(arq, "#SETUP\n");
    fprintf(arq, "%i\n",nglad);

    for (i=0 ; i<nglad ; i++){
        if (fgets(buffer, 499, temparq[i])){
            getAtributos(atributos, buffer);
            fprintf(arq, "%s|%.1f|%.1f|%s\n", (equipe+c[i])->gladiador[(equipe+c[i])->selected - 1].nome, atof(atributos[9]), atof(atributos[10]), (equipe+c[i])->gladiador[(equipe+c[i])->selected - 1].folha);
            rewind(temparq[i]);
        }
    }
    fprintf(arq, "#BEGIN SIMULATION\n");

    int contend = 0;
    i=0;
    float timenow, timeant;
    float remlife[nglad];
    int remlvl[nglad][4];
    do{
        if (fgets(buffer, 499, temparq[i])){
            getAtributos(atributos, buffer);
            timeant = timenow;
            timenow = atof(atributos[0]);

            contend = 0;
            if (timenow >= timeant)
                fputs(buffer, arq);
            //else
            //    printf("%s\n",buffer);


            for (j=0 ; j<4 ; j++){
                remlvl[i][j] = atoi(atributos[j+2]); //lvl,str,agi,int
            }
            remlife[i] = max(atof(atributos[13]), 0); //hp
            remlife[i] = remlife[i]/atof(atributos[14])*100; //maxhp

            if (remlife[i] <= 0){
                if (c[i] != winners[0]){
                    if (c[i] != winners[1])
                        winners[2] = winners[1];
                    winners[1] = winners[0];
                    winners[0] = c[i];
                }
            }
        }
        else
            contend++;
        i = (i+1)%nglad;
    } while (contend < nglad);
    fclose(arq);

    char ts[20],logname[50];
    strcpy(ts,getTime());
    sprintf(logname, "log/%s.txt",ts);
    FILE *log = fopen(logname,"w");
    arq = fopen("tmp/output.txt","r");
    while(!feof(arq)){
        if (fgets(buffer, 500, arq)){
            fputs(buffer, log);
        }
    }
    fclose(log);
    fclose(arq);

    for (i=0 ; i<nglad ; i++){
        fclose(temparq[i]);
        (equipe+c[i])->gladiador[(equipe+c[i])->selected-1].lvl = remlvl[i][0];
        (equipe+c[i])->gladiador[(equipe+c[i])->selected-1].STR = remlvl[i][1];
        (equipe+c[i])->gladiador[(equipe+c[i])->selected-1].AGI = remlvl[i][2];
        (equipe+c[i])->gladiador[(equipe+c[i])->selected-1].INT = remlvl[i][3];
        (equipe+c[i])->gladiador[(equipe+c[i])->selected-1].life = remlife[i];
    }
    for (i=0 ; i<nglad ; i++){
        float r = (equipe+c[i])->gladiador[(equipe+c[i])->selected-1].life;
        //printf("%i %i %.1f\n",i, contAlive(), remlife[i]);
        if (contAlive() == 1 && r > 0){
            if (c[i] != winners[0]){
                if (c[i] != winners[1])
                    winners[2] = winners[1];
                winners[1] = winners[0];
                winners[0] = c[i];
            }
        }
    }

}

int main(){
    rounds = 1;
    estado = 0;
    for (i=0 ; i<3 ; i++)
        winners[i] = -1;

    do{

        printf("Starting UI... \n");
        if (!loadUI())
            break;
        printf("Simulation setup.\n");

        if (nglad > 0 && contAlive() > 1 && estado == 0){
            DWORD dwThreadId;

            int corresp[nglad];

            struct params p[nglad];
            HANDLE hThread[nglad];

            //inicia memoria que sera compartilhada para struct e para o contador de tempo de cada processo
            startStructSharedMemory();
            startCounterSharedMemory();

            //inicializações do allegro para o render runtime
            /*
            printf("Starting render... ");
            if (!renderInit()){
                return -1;
            }
            printf("Done.\n");
            */

            int i=0,j=0;
            //chama uma thread para cada gladiador
            while (j<nglad){
                if ((equipe+i)->selected != 0){
                    strcpy(p[j].code, (equipe+i)->gladiador[(equipe+i)->selected - 1].codigo);
                    p[j].id = j;
                    p[j].n = nglad;
                    strcpy(p[j].name, (equipe+i)->gladiador[(equipe+i)->selected - 1].nome);
                    p[j].life = (equipe+i)->gladiador[(equipe+i)->selected - 1].life;
                    p[j].lvl = (equipe+i)->gladiador[(equipe+i)->selected - 1].lvl;
                    p[j].STR = (equipe+i)->gladiador[(equipe+i)->selected - 1].STR;
                    p[j].AGI = (equipe+i)->gladiador[(equipe+i)->selected - 1].AGI;
                    p[j].INT = (equipe+i)->gladiador[(equipe+i)->selected - 1].INT;
                    corresp[j] = i;

                    hThread[j] = CreateThread(
                        NULL, // default security attributes
                        0, // use default stack size
                        thread, // thread function
                        &p[j], // argument to thread function
                        0, // use default creation flags
                        &dwThreadId); // returns the thread identifier

                    // Check the return value for success. If something wrong...
                    if (hThread[j] == NULL)
                        printf("CreateThread() failed, error: %i.\n", (int)GetLastError());
                    j++;
                }
                i++;
            }

            //render em tempo de execução
            //renderLoop();

            //espera todas threads terminarem
            WaitForMultipleObjects(
                nglad,
                hThread,
                1,
                INFINITE);

            //ordena as linhas do arquivo de saída de acordo com o tempo da simulação
            printf("All threads closed.\n");

            printf("Writing output file...  ");
            mergeOutput(corresp);
            printf("Done.\n");

            printf("Rendering simulation.\n");
            system("gladCodeRender.jar");

            restartSharedMemory();
        }
        else{
            printf("Source codes not loaded.\n");
            break;
        }

        if (contAlive() == 1){
            estado++;
            system("del /Q /A-R tmp");
        }

        rounds++;
    }while(estado < 2);

    printf("Program terminate.\n");

    destroyAllegro();


    return 0;
}

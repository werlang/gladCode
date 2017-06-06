#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "GladCodeGlobals.c"
#include "GladCodeSMem.c"
#include "gladCodeRuntimeRender.c"

struct params {
    char code[50];
    int id;
    int n;
};

//thread que compila e roda cada gladiador
DWORD WINAPI thread(LPVOID p){
    struct params *input = p;

    printf("thread started received %s %i %i\n",input->code,input->id, input->n);

    char call[100];
    sprintf(call, "gcc -o %s.exe %s", input->code, input->code);
    printf("Compiling %s...\n",input->code);
    system(call);
    printf("%s compilled.\n",input->code);
    sprintf(call, "%s.exe %i %i",input->code, input->id, input->n);
    printf("Running %s.exe...\n",input->code);
    system(call);
    printf("%s.exe stopped.\n",input->code);
    return 0;
}

//o output contém contém em sequencia as ações de cada gladiador, este função ordena as linhas por tempo.
void sortOutput(){
    FILE *arq = fopen("output.txt","r");
    if (arq != NULL){
        char **outmat = NULL;
        char buffer[500];
        int lines = 0;

        //guarda em outmat todo conteudo de output
        while (!feof(arq)){
            fgets(buffer, 499, arq);
            if (!feof(arq)){
                if (outmat == NULL)
                    outmat = (char**)malloc(sizeof(char*));
                else
                    outmat = (char**)realloc(outmat, sizeof(char*) * (lines+1) );
                outmat[lines] = (char*)malloc(sizeof(char) * 500 );
                strcpy(outmat[lines], buffer);
                lines++;
            }
        }
        fclose(arq);
        int troca, i;
        char strval[500], *pos;
        float val1, val2;
        char temp[500];
        //ordena de acordo com o tempo no inicio de cada linha
        do{
            troca = 0;
            for (i=0 ; i<lines-1 ; i++){
                pos = strstr(outmat[i], "|");
                strncpy(strval, outmat[i], sizeof(char) * (pos - outmat[i]) );
                strval[pos - outmat[i]] = '\0';
                val1 = atof(strval);

                pos = strstr(outmat[i+1], "|");
                strncpy(strval, outmat[i+1], sizeof(char) * (pos - outmat[i+1]) );
                strval[pos - outmat[i+1]] = '\0';
                val2 = atof(strval);

                if (val1 > val2){
                    strcpy(temp, outmat[i]);
                    strcpy(outmat[i], outmat[i+1]);
                    strcpy(outmat[i+1], temp);
                    troca=1;
                }
            }
        } while(troca != 0);

        arq = fopen("output.txt","w");
        for (i=0 ; i<lines ; i++){
            fputs(outmat[i], arq);
        }
        fclose(arq);
    }
}

int main(){
    DWORD dwThreadId;

    //apaga aquivo de output caso ele já exista
    FILE *arq = fopen("output.txt","r");
    if (arq != NULL){
        fclose(arq);
        system("del output.txt");
    }
    scanf("%i",&nglad);
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

    //chama uma thread para cada gladiador
    int i;
    for (i=0 ; i<nglad ; i++){
        char input[100];
        scanf("%s",p[i].code);
        p[i].id = i;
        p[i].n = nglad;

        hThread[i] = CreateThread(
            NULL, // default security attributes
            0, // use default stack size
            thread, // thread function
            &p[i], // argument to thread function
            0, // use default creation flags
            &dwThreadId); // returns the thread identifier

        // Check the return value for success. If something wrong...
        if (hThread[i] == NULL)
            printf("CreateThread() failed, error: %i.\n", (int)GetLastError());
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
    printf("Sorting output file...  ");
    sortOutput();
    printf("Done.\nProgram terminate.\n");

    return 0;
}

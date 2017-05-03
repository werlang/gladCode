#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "GladCodeStruct.c"
#include "GladCodeSMem.c"
#include "gladCodeRuntimeRender.c"

struct params {
    char code[50];
    int id;
    int n;
};

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

void sortOutput(){
    FILE *arq = fopen("output.txt","r");
    if (arq != NULL){
        char **outmat = NULL;
        char buffer[500];
        int lines = 0;

        while (!feof(arq)){
            fgets(buffer, 499, arq);
            if (!feof(arq)){
                if (outmat == NULL)
                    outmat = (char**)malloc(sizeof(char*));
                else
                    outmat = (char**)realloc(outmat, sizeof(char*) * (lines+1) );
                outmat[lines] = (char*)malloc(sizeof(char) * (strlen(buffer)+1) );
                strcpy(outmat[lines], buffer);
                lines++;
            }
        }
        fclose(arq);

        int troca, i;
        char strval[7], *pos;
        float val1, val2;
        char temp[500];
        do{
            troca = 0;
            for (i=0 ; i<lines-1 ; i++){
                pos = strstr(outmat[i], "|");
                strncpy(strval, outmat[i], sizeof(char) * (pos - outmat[i]) );
                val1 = atof(strval);
                pos = strstr(outmat[i], "|");
                strncpy(strval, outmat[i+1], sizeof(char) * (pos - outmat[i]) );
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

    FILE *arq = fopen("output.txt","r");
    if (arq != NULL){
        fclose(arq);
        system("del output.txt");
    }
    scanf("%i",&nglad);
    struct params p[nglad];
    HANDLE hThread[nglad];

    startStructSharedMemory(); //reserva memoria que sera compartilhada
    startCounterSharedMemory();

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
            printf("CreateThread() failed, error: %d.\n", GetLastError());
    }


    printf("Starting render...\n");

    if (!renderInit()){
        return -1;
    }
    printf("Render started.\n");
    renderLoop();


    WaitForMultipleObjects(
        nglad,
        hThread,
        1,
        INFINITE);

    sortOutput();
    printf("All threads stopped. Program terminate.\n");

    return 0;
}
